
#include <xc.h>
#include "IO_Accionamiento.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "RingBuffer.h"
#include "Protocolo_Comm_Yaesu.h"
#include "UART.h"
#include "IO_Basic.h"
#include "timer1.h"

/*===================== [Variables Internas (Globales)] =====================*/
uint8_t flag_hs_e = 0;
uint8_t flag_hs_a = 0;

uint8_t flag_HomeStop_Elev = 0;
uint8_t flag_HomeStop_Az = 0;
uint8_t HomeStop_Elev_init = 0;
uint8_t HomeStop_Az_init = 0;
uint8_t goingToHome_Az = 0;
uint8_t goingToHome_Elev = 0;

uint8_t acimutInTarget = 0;
uint8_t elevacionInTarget = 0;

uint8_t flag_Emergencia = 0;

uint8_t calibracion = 0;
/*===========================================================================*/

/*===================== [Variables Internas (Globales)] =====================*/
Struct_Data_Control Data_Control;

char Datos_A_Enviados[MAX_SIZE_DATA_SEND];
uint32_t Cant_Carac_A_Enviar;
/*===========================================================================*/
ID_Comandos estado_Accionamiento_anterior = Sleep;
ID_Comandos estado_Accionamiento = Sleep;
uint16_t ciclos_sin_comandos;


unsigned long millis_COMANDO;
unsigned long millis_TRACKING;
unsigned long millis_MANUAL;
unsigned long millis_INIT;

extern uint8_t elevation_inHome;
extern Info_Comandos_Procesados Comando_Procesado;
extern Comando_Almacenado Char_Comando;
extern Info_Comandos_Procesados Comando_Procesado;
extern uint8_t nuevoComando;
//uint8_t tracking_flag = 0;

void __attribute__((interrupt,no_auto_psv)) _CNInterrupt(void){
    
    /* -----------------------------   ENCODER ACIMUT   ---------------------------- */
    read_EncoderAz();
    /* ----------------------------------------------------------------------------- */
    
    
    /* --------------------------   HOME STOP ELEVACION   -------------------------- */
    flag_hs_e = read_HS_Elev();
//    if(HOME_STOP_ELEV != valor_anterior.home_stop_Elev){
//        if(HOME_STOP_ELEV == HIGH) {
//            flag_hs_e = 1;
//        }
//        valor_anterior.home_stop_Elev = HOME_STOP_ELEV;
//    }
    /* ----------------------------------------------------------------------------- */
    
    /* ----------------------------   HOME STOP ACIMUT   --------------------------- */
    flag_hs_a = read_HS_Az();
//    if(HOME_STOP_AZ != valor_anterior.home_stop_Az){
//        valor_anterior.home_stop_Az = HOME_STOP_AZ;
//        
//        if(HOME_STOP_AZ == HIGH && READ_RELE_2 == ON){
//            flag_hs_a = 1;
//        }
//    }
    /* ----------------------------------------------------------------------------- */
    
    if(read_Emergencia())
    {
//        Stop(ALL);
//        putrsUART2("BOTON");
        estado_Accionamiento = Sleep;
        flag_Emergencia = 1;
    }
    
    IFS1bits.CNIF = 0; // Clear CN interrupt
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Actualizar_Objetivos(){
    Data_Control.Target_Acimut = atof(Char_Comando.Char_Acimut);
    if(Data_Control.Target_Acimut < 160){
        Data_Control.Target_Acimut = Data_Control.Target_Acimut + 360;
    }
    
    double nuevo_elev = atof(Char_Comando.Char_Elevacion);
    if(nuevo_elev>POSICION_HS_ELEV)
    {
        Data_Control.Target_Elevacion = nuevo_elev;
    }
}

void Move(OUT out) {
    switch(out) {
        case ACIMUT_RIGHT:
            OUT_RELE_1 = ON;
            delayPIC_ms(500);
            if(READ_RELE_3 == ON) {
                OUT_RELE_3 = OFF;
                delayPIC_ms(DELAY_CAMBIO_SENTIDO);
            }
            OUT_RELE_2 = ON;
            break;
            
        case ACIMUT_LEFT:
            OUT_RELE_1 = ON;
            delayPIC_ms(500);
            if(READ_RELE_2 == ON) {
                OUT_RELE_2 = OFF;
                delayPIC_ms(DELAY_CAMBIO_SENTIDO);
            }
            OUT_RELE_3 = ON;
            break;
            
        case ELEVACION_UP:
            if(OUT_VAR_2 == OFF) {
                OUT_VAR_2 = ON;
                delayPIC_ms(DELAY_CAMBIO_SENTIDO);
            }
            OUT_VAR_1 = OFF;            
            break;
            
        case ELEVACION_DOWN:
            if(OUT_VAR_1 == OFF) {
                OUT_VAR_1 = ON;
                delayPIC_ms(DELAY_CAMBIO_SENTIDO);
            }
            OUT_VAR_2 = OFF;           
            break;
            
        default:
            break;
    }
}

void Stop(OUT out) {
    ClrWdt();
    switch(out)
    {
        case ALL:
            OUT_RELE_1 = OFF;
            delayPIC_ms(500);
            if(READ_RELE_2) OUT_RELE_2 = OFF;
            if(READ_RELE_3) OUT_RELE_3 = OFF;

            OUT_VAR_1   = ON;
            OUT_VAR_2   = ON;
            break;
        
        case ACIMUT:
            
            OUT_RELE_1 = OFF;
            delayPIC_ms(500);
            if(READ_RELE_2) OUT_RELE_2 = OFF;
            if(READ_RELE_3) OUT_RELE_3 = OFF;
            break;
            
        case ELEVACION:
            OUT_VAR_1 = ON;
            OUT_VAR_2 = ON;
            break;
                    
        default:
            break;
    }
    ClrWdt();
}

uint8_t Tracking(double acimutTarget, double elevacionTarget) {
    
    Data_Control.Valor_Actual_Acimut = get_Acimut();
    Data_Control.Valor_Actual_Elevacion = get_Elevacion();
            
    if(!acimutInTarget) {
        if(Data_Control.Valor_Actual_Acimut < (acimutTarget - OFFSET_ANGULAR_ACIMUT))
            Move(ACIMUT_RIGHT); 
        else if (Data_Control.Valor_Actual_Acimut > (acimutTarget + OFFSET_ANGULAR_ACIMUT))
            Move(ACIMUT_LEFT);
        else {
            Stop(ACIMUT);
            acimutInTarget = 1;
        }
    }

    if(!elevacionInTarget) {
        if(Data_Control.Valor_Actual_Elevacion < (elevacionTarget - OFFSET_ANGULAR_ELEVACION)) 
            Move(ELEVACION_UP);
        else if (Data_Control.Valor_Actual_Elevacion > (elevacionTarget + OFFSET_ANGULAR_ELEVACION))
            Move(ELEVACION_DOWN);
        else {
            Stop(ELEVACION);
            elevacionInTarget = 1;
        }
    }
    
    if(acimutInTarget && elevacionInTarget) return 1;
    else return 0;
}

void verificar_HS(void)
{
    if(flag_hs_e==1){
        delayPIC_ms(200);
        if(HOME_STOP_ELEV == HIGH){
            putrsUART2("HE\n\r");
            Stop(ELEVACION);
            set_Contador(POSICION_HS_ELEV,ELEVACION);
            
            flag_HomeStop_Elev = 1;
        }
        flag_hs_e=0;
    }

    if(flag_hs_a==1){
        delayPIC_ms(200);
        if(HOME_STOP_AZ == HIGH){
            putrsUART2("HA\n\r");
            Stop(ACIMUT);
            long pulsos = POSICION_HS_AZ / REDUCCION_ENCODER_ANTENA_ACIMUT;
            pulsos = (pulsos * RESOLUCION_ENCODER)/GRADOS_POR_VUELTA;
            set_Contador(pulsos,ACIMUT);

            flag_HomeStop_Az = 1;
        }
        flag_hs_a = 0;
    }
}

/* ================   MEF de Accionamiento  =================== */

void MEF_Accionamiento(){
    
    verificar_HS();
    read_EncoderElev();
    
    // Verifico si llego un nuevo comando
    // Si el nuevo comando llega durante la calibracion no se tiene en cuenta
    if(nuevoComando > 0 && !flag_Emergencia && (estado_Accionamiento != GoToHome_Acimut && estado_Accionamiento != GoToHome_Elevacion))
    {
        estado_Accionamiento_anterior = estado_Accionamiento;
        estado_Accionamiento = Comando_Procesado.Actual;
        nuevoComando = 0;
        millis_COMANDO = millis();  
    } else {
        if(millis() - millis_COMANDO > TIMEOUT_COMANDO) { // Dos horas sin comandos
            millis_COMANDO = millis();
            estado_Accionamiento = Return_ToHome;
        }
        nuevoComando = 0;
    }
    
    
    
    switch(estado_Accionamiento){
        
        /* =========  Movimiento manual  ========== */
        
        // ------- Acimut:
        
        case Giro_Horario:
            if(estado_Accionamiento != estado_Accionamiento_anterior) {
                estado_Accionamiento_anterior = estado_Accionamiento;
                Move(ACIMUT_RIGHT);
                millis_MANUAL = millis();
            }
            
            if(millis() - millis_MANUAL > TIMEOUT_MANUAL) {
                Stop(ACIMUT);
                estado_Accionamiento = Sleep;
            }
            
            break;
            
        case Giro_Antihorario:
            if(estado_Accionamiento != estado_Accionamiento_anterior) {
                estado_Accionamiento_anterior = estado_Accionamiento;
                Move(ACIMUT_LEFT);
                millis_MANUAL = millis();
            }
                
            if(millis() - millis_MANUAL > TIMEOUT_MANUAL) {
                Stop(ACIMUT);
                estado_Accionamiento = Sleep;
            }
            
            break;
            
        case Stop_Acimut:
            ClrWdt();
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
            Stop(ACIMUT);
            estado_Accionamiento = Sleep;
            break;
            
        // ------- Elevacion:
                    
        case Giro_Arriba:
            if(estado_Accionamiento != estado_Accionamiento_anterior) {
                estado_Accionamiento_anterior = estado_Accionamiento;
                Move(ELEVACION_UP);
               // putrsUART2("== Giro para arriba ==\n\r");
                millis_MANUAL = millis();
            }
            
            if(millis() - millis_MANUAL > TIMEOUT_MANUAL) {
                Stop(ELEVACION);
                estado_Accionamiento = Sleep;
            }
            
            break;
            
        case Giro_Abajo:
            if(estado_Accionamiento != estado_Accionamiento_anterior) {
                estado_Accionamiento_anterior = estado_Accionamiento;
                Move(ELEVACION_DOWN);
                millis_MANUAL = millis();
            }
            
            if(millis() - millis_MANUAL > TIMEOUT_MANUAL) {
                Stop(ELEVACION);
                estado_Accionamiento = Sleep;
            }
            
            break;
            
        case Stop_Elevacion:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
            Stop(ELEVACION);
            estado_Accionamiento = Sleep;
            break;
            
        // ------- Stop_Global:
            
        case Stop_Global:
            ClrWdt();
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
            Stop(ALL);
            estado_Accionamiento = Sleep;
 //           putrsUART2("== stop global ==\n\r");
            break;
            
        /* =========  Movimiento tracking  ========== */
            
        case GoToHome_Acimut:
            if(estado_Accionamiento != estado_Accionamiento_anterior) {
                estado_Accionamiento_anterior = estado_Accionamiento;
                millis_INIT = millis();
                putrsUART2("C1\r\n"); // Inicio de la calibracion
                calibracion = 1;
                Move(ACIMUT_RIGHT); // me muevo antihorario buscando el 0
            }
            
            // Timeout de 15 min
            if(millis() - millis_INIT > TIMEOUT_INIT_AZ) {
                Stop(ALL);
                putrsUART2("C2\r\n"); // Error en la calibracion
                estado_Accionamiento = Sleep;
            }
            
            if(flag_HomeStop_Az) {
                delayPIC_ms(DELAY_CAMBIO_SENTIDO);
                Move(ACIMUT_LEFT);
                flag_HomeStop_Az = 0;
                goingToHome_Az = 1;
            }
    
            if(goingToHome_Az) {
                Data_Control.Valor_Actual_Acimut = get_Acimut();
                if(abs(Data_Control.Valor_Actual_Acimut - HOME_ACIMUT) <= OFFSET_ANGULAR_ACIMUT)
                {
                    Stop(ACIMUT);
                    goingToHome_Az = 0;
                    HomeStop_Az_init = 1;
                    estado_Accionamiento = GoToHome_Elevacion;
                }
            }
                
            break;
            
        case GoToHome_Elevacion:
            if(estado_Accionamiento != estado_Accionamiento_anterior) {
                estado_Accionamiento_anterior = estado_Accionamiento;
                millis_INIT = millis();
                Move(ELEVACION_DOWN);
            }
            
            // Timeout de 5 min
            if(millis() - millis_INIT > TIMEOUT_INIT_ELEV) {
                Stop(ALL);
                putrsUART2("C2\r\n"); // Error en la calibracion
                estado_Accionamiento = Sleep;
            }
            
            if(flag_HomeStop_Elev) {
                delayPIC_ms(DELAY_CAMBIO_SENTIDO);
                Move(ELEVACION_UP);
                flag_HomeStop_Elev = 0;
                goingToHome_Elev = 1;
                Clean_RingBufferRx_U1();
                Clean_RingBufferRx_U2();
            }
    
            if(goingToHome_Elev) {
                Data_Control.Valor_Actual_Elevacion = get_Elevacion();
                if(abs(Data_Control.Valor_Actual_Elevacion - HOME_ELEVACION) <= 1)
                {
                    Stop(ELEVACION);
                    goingToHome_Elev = 0;
                    HomeStop_Elev_init = 1;
                    putrsUART2("C0\r\n"); // Fin de la calibracion
                    calibracion = 0;
//                    putrsUART2("Estacion en HOME... Inicializacion finalizada!\n\r");
                    estado_Accionamiento = Sleep;
                }
            }
                
            break;
            
        case Return_ToHome:
            if(estado_Accionamiento != estado_Accionamiento_anterior){
                estado_Accionamiento_anterior = estado_Accionamiento;
                acimutInTarget = 0;
                elevacionInTarget = 0;
            }
            
            // Timeout de 5 min
            if(millis() - millis_TRACKING > TIMEOUT_INIT_AZ) {
                Stop(ALL);
                estado_Accionamiento = Sleep;
            }
           
            if(Tracking(HOME_ACIMUT, HOME_ELEVACION)) {
                estado_Accionamiento = Sleep;
            }
            
            break;
            
        case Objetivo_Tracking:
            if(estado_Accionamiento != estado_Accionamiento_anterior) {
                estado_Accionamiento_anterior = estado_Accionamiento;
                acimutInTarget = 0;
                elevacionInTarget = 0;
                millis_TRACKING = millis();
            }
            
            // Timeout de 10 min
            if(millis() - millis_TRACKING > TIMEOUT_TRACKING) {
                Stop(ALL);
                estado_Accionamiento = Sleep;
            }
            
            if(Tracking(Data_Control.Target_Acimut, Data_Control.Target_Elevacion)) {
                estado_Accionamiento = Sleep;
            }
            
            break;
            
        /* ================  Sleep  ================= */
            
        case Sleep:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
            if(flag_Emergencia)
            {
                Stop(ALL);
                putrsUART2("C3\r\n"); // Fin de la calibracion
                delayPIC_ms(10000);
                flag_Emergencia = 0;
            }
//            delayPIC_ms(100);
            break;
            
        default:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
            estado_Accionamiento = Sleep;
    }
}