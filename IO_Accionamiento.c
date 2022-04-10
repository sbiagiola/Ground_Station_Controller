
#include <xc.h>
#include "IO_Accionamiento.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "RingBuffer.h"
#include "Protocolo_Comm_Yaesu.h"
#include "UART.h"
#include "timer1.h"

/*===================== [Variables Internas (Globales)] =====================*/
Last_Value valor_anterior;
_Contador contador;


//static Operacion_Pulsos estado_Encoder_Elev;
//static Operacion_Pulsos estado_Encoder_Az;
uint8_t flag_HomeStop_Elev = 0;
uint8_t flag_HomeStop_Az = 0;
uint8_t HomeStop_Elev_init = 0;
uint8_t HomeStop_Az_init = 0;
uint8_t goingToHome_Az = 0;
uint8_t goingToHome_Elev = 0;

uint8_t acimutInTarget = 0;
uint8_t elevacionInTarget = 0;

uint8_t Flag_Parada_Emergencia = 0;
/*===========================================================================*/

/*===================== [Variables Internas (Globales)] =====================*/
Struct_Data_Control Data_Control;


char Datos_A_Enviados[MAX_SIZE_DATA_SEND];
uint32_t Cant_Carac_A_Enviar;
/*===========================================================================*/

ID_Comandos estado_Accionamiento = GoToHome_Acimut;
ID_Comandos estado_Accionamiento_anterior = Sleep;
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

void initCN()
{
    valor_anterior.encoderElev_A = ENCODER_ELEV_A;
    valor_anterior.encoderElev_B = ENCODER_ELEV_B;
    valor_anterior.encoderElev_Z = ENCODER_ELEV_Z;
    
    valor_anterior.encoderAz_A = ENCODER_AZ_A;
    valor_anterior.encoderAz_B = ENCODER_AZ_B;
    valor_anterior.encoderAz_Z = ENCODER_AZ_Z;
    
    valor_anterior.anemometro = ANEMOMETRO;
    valor_anterior.parada_emergencia = PARADA_EMERGENCIA;
    
    valor_anterior.home_stop_Elev = HOME_STOP_ELEV;
    valor_anterior.home_stop_Az = HOME_STOP_AZ;
    
    contador.encoderAz_Pulsos = 8;
    contador.encoderElev_Pulsos = 8;
}

void __attribute__((interrupt,no_auto_psv)) _CNInterrupt(void){
    
    /* ---------------------------   ENCODER ELEVACION   --------------------------- */
    // Contador de pulsos
    if((ENCODER_ELEV_A != valor_anterior.encoderElev_A)) {
        if(ENCODER_ELEV_A == HIGH) {
            if(ENCODER_ELEV_B == HIGH) {
//                putrsUART2("[CNInterrupt]: Encoder ELEVACION (B)\n\r");
                contador.encoderElev_Pulsos++;
            } else {
//                putrsUART2("[CNInterrupt]: Encoder ELEVACION (A)\n\r");
                contador.encoderElev_Pulsos--;
            }
        }
        valor_anterior.encoderElev_A = ENCODER_ELEV_A;
    }
    /* ----------------------------------------------------------------------------- */
    
    /* -----------------------------   ENCODER ACIMUT   ---------------------------- */
    // Contador de pulsos
    if((ENCODER_AZ_A != valor_anterior.encoderAz_A)) {
        if(ENCODER_AZ_A == HIGH) {
            if(ENCODER_AZ_B == HIGH) {
//                putrsUART2("[CNInterrupt]: Encoder ACIMUT (B)\n\r");                
                contador.encoderAz_Pulsos++;
            } else {
//                putrsUART2("[CNInterrupt]: Encoder ACIMUT (A)\n\r");
                contador.encoderAz_Pulsos--;
            }
        }
        valor_anterior.encoderAz_A = ENCODER_AZ_A;
    }
    /* ----------------------------------------------------------------------------- */
    
    /* --------------------------   HOME STOP ELEVACION   -------------------------- */
    if(HOME_STOP_ELEV != valor_anterior.home_stop_Elev){
        if(HOME_STOP_ELEV == HIGH) {
//            putrsUART2(" === ELEVACION: HOME STOP DETECTADO! ===\n\r");
            Stop(ELEVACION);
//            delayPIC_ms(DELAY_CAMBIO_SENTIDO);
            contador.encoderElev_Pulsos = 0;
//            if(HomeStop_Elev_init == 1)
//                elevacionInTarget = 1;
                //estado_Accionamiento = Sleep;
            flag_HomeStop_Elev = 1;
        }
        valor_anterior.home_stop_Elev = HOME_STOP_ELEV;
    }
    /* ----------------------------------------------------------------------------- */
    
    /* ----------------------------   HOME STOP ACIMUT   --------------------------- */
    if(HOME_STOP_AZ != valor_anterior.home_stop_Az){
        if(HOME_STOP_AZ == HIGH) {
//            putrsUART2(" === ACIMUT: HOME STOP DETECTADO! ===\n\r");
            Stop(ACIMUT);
//            delayPIC_ms(DELAY_CAMBIO_SENTIDO);
            contador.encoderAz_Pulsos = RESOLUCION_ENCODER;
//            if(HomeStop_Az_init == 1)
//                acimutInTarget = 1;
                //estado_Accionamiento = Sleep;
            flag_HomeStop_Az = 1;
        }
        valor_anterior.home_stop_Az = HOME_STOP_AZ;
    }
    /* ----------------------------------------------------------------------------- */
    
    /* --------------------------   PARADA DE EMERGENCIA   ------------------------- */
    if(PARADA_EMERGENCIA != valor_anterior.parada_emergencia && PARADA_EMERGENCIA == LOW){
//        putrsUART2("====================================\n\r");
//        putrsUART2("=====  ¡PARADA DE EMERGENCIA!  =====\n\r");
//        putrsUART2("====================================\n\r");
        Stop(ALL);
        estado_Accionamiento = Sleep;
       
        valor_anterior.parada_emergencia = PARADA_EMERGENCIA;
    }
    
    /* ----------------------------------------------------------------------------- */
    
    IFS1bits.CNIF = 0; // Clear CN interrupt
}

double get_Acimut(void){
    double ang = (contador.encoderAz_Pulsos * GRADOS_POR_VUELTA) / RESOLUCION_ENCODER;
    return ang;
}

double get_Elevacion(void){
    double ang = (contador.encoderElev_Pulsos * GRADOS_POR_VUELTA) / RESOLUCION_ENCODER;
    return  ang;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Actualizar_Objetivos(){
    Data_Control.Target_Acimut = atof(Char_Comando.Char_Acimut);
    Data_Control.Target_Elevacion = atof(Char_Comando.Char_Elevacion);
}

void Move(OUT out) {
    switch(out) {
        case ACIMUT_RIGHT:
            if(READ_RELE_2 == ON) {
                OUT_RELE_2 = OFF;
                delayPIC_ms(DELAY_CAMBIO_SENTIDO);
            }
            OUT_RELE_1 = ON;
            break;
            
        case ACIMUT_LEFT:
            if(READ_RELE_1 == ON) {
                OUT_RELE_1 = OFF;
                delayPIC_ms(DELAY_CAMBIO_SENTIDO);
            }
            OUT_RELE_2 = ON;
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
    switch(out)
    {
        case ALL:
            OUT_RELE_1  = OFF;
            OUT_RELE_2  = OFF;
            OUT_VAR_1   = ON;
            OUT_VAR_2   = ON;
            break;
        
        case ACIMUT:
            OUT_RELE_1 = OFF;
            OUT_RELE_2 = OFF;
            break;
            
        case ELEVACION:
            OUT_VAR_1 = ON;
            OUT_VAR_2 = ON;
            break;
                    
        default:
            break;
    }
}

uint8_t Tracking(double acimutTarget, double elevacionTarget) {
    
    Data_Control.Valor_Actual_Acimut = get_Acimut();
    Data_Control.Valor_Actual_Elevacion = get_Elevacion();
            
    if(!acimutInTarget) {
        if(Data_Control.Valor_Actual_Acimut < (acimutTarget - 1))
            Move(ACIMUT_RIGHT); 
        else if (Data_Control.Valor_Actual_Acimut > (acimutTarget + 1))
            Move(ACIMUT_LEFT);
        else {
            Stop(ACIMUT);
            acimutInTarget = 1;
        }
    }

    if(!elevacionInTarget) {
        if(Data_Control.Valor_Actual_Elevacion < (elevacionTarget - 1))
            Move(ELEVACION_UP);
        else if (Data_Control.Valor_Actual_Elevacion > (elevacionTarget + 1))
            Move(ELEVACION_DOWN);
        else {
            Stop(ELEVACION);
            elevacionInTarget = 1;
        }
    }
    
    if(acimutInTarget && elevacionInTarget) return 1;
    else return 0;
}

void MEF_Accionamiento(){
    
    if(nuevoComando > 0)
    {
        estado_Accionamiento_anterior = estado_Accionamiento;
        estado_Accionamiento = Comando_Procesado.Actual;
        nuevoComando = 0;
        millis_COMANDO = millis();
    } else {
        if(millis() - millis_COMANDO > TIMEOUT_COMANDO) { // Dos horas sin comandos
//            putrsUART2("TIMEOUT COMANDOS... Volviendo a HOME\n\r");
            millis_COMANDO = millis();
            estado_Accionamiento = Return_ToHome;
        }
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
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
            Stop(ALL);
            estado_Accionamiento = Sleep;
            break;
            
        /* =========  Movimiento tracking  ========== */
            
        case GoToHome_Acimut:
            if(estado_Accionamiento != estado_Accionamiento_anterior) {
                estado_Accionamiento_anterior = estado_Accionamiento;
                Move(ACIMUT_RIGHT);
            }
            
            // Timeout de 10 min
            if(millis() - millis_INIT > TIMEOUT_INIT) {
                Stop(ALL);
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
                if(abs(Data_Control.Valor_Actual_Acimut - HOME_ACIMUT) <= 1)
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
                Move(ELEVACION_DOWN);
            }
            
            // Timeout de 10 min
            if(millis() - millis_INIT > TIMEOUT_INIT) {
                Stop(ALL);
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
//                    putrsUART2("Estacion en HOME... Inicializacion finalizada!\n\r");
                    estado_Accionamiento = Sleep;
                }
            }
                
            break;
            
        case Return_ToHome:
            if(estado_Accionamiento != estado_Accionamiento_anterior) {
                estado_Accionamiento_anterior = estado_Accionamiento;
                acimutInTarget = 0;
                elevacionInTarget = 0;
            }
            
            // Timeout de 5 min
            if(millis() - millis_TRACKING > TIMEOUT_TRACKING) {
                Stop(ALL);
                estado_Accionamiento = Sleep;
            }
            
//            Data_Control.Valor_Actual_Acimut = get_Acimut();
//            Data_Control.Valor_Actual_Elevacion = get_Elevacion();
            
//            if(goingToHome_Az) {
//                if(Data_Control.Valor_Actual_Acimut < (HOME_ACIMUT - 1))
//                    Move(ACIMUT_RIGHT); 
//                else if (Data_Control.Valor_Actual_Acimut > (HOME_ACIMUT + 1))
//                    Move(ACIMUT_LEFT);
//                else {
//                    Stop(ACIMUT);
//                    goingToHome_Az = 0;
//                }
//            }
//            
//            if(goingToHome_Elev) {
//                if(Data_Control.Valor_Actual_Elevacion < (HOME_ELEVACION - 1))
//                    Move(ELEVACION_UP);
//                else if (Data_Control.Valor_Actual_Elevacion > (HOME_ELEVACION + 1))
//                    Move(ELEVACION_DOWN);
//                else {
//                    Stop(ELEVACION);
//                    goingToHome_Elev = 0;
//                }
//            }
            
            if(Tracking(HOME_ACIMUT, HOME_ELEVACION)) {
//                putrsUART2("====================================\n\r");
//                putrsUART2("==== ¡LA ESTACION LLEGO A HOME! ====\n\r");
//                putrsUART2("====================================\n\r");
                estado_Accionamiento = Sleep;
            }
            
            break;
            
        case Objetivo_Tracking:
            if(estado_Accionamiento != estado_Accionamiento_anterior) {
                estado_Accionamiento_anterior = estado_Accionamiento;
                Stop(ALL);
                delayPIC_ms(DELAY_CAMBIO_SENTIDO);
                acimutInTarget = 0;
                elevacionInTarget = 0;
//                putrsUART2("Iniciando tracking...\n\r");
                millis_TRACKING = millis();
            }
            
//            Data_Control.Valor_Actual_Acimut = get_Acimut();
//            Data_Control.Valor_Actual_Elevacion = get_Elevacion();

//            if(!acimutInTarget) {
//                if(Data_Control.Valor_Actual_Acimut < (Data_Control.Target_Acimut - 1))
//                    Move(ACIMUT_RIGHT); 
//                else if (Data_Control.Valor_Actual_Acimut > (Data_Control.Target_Acimut + 1))
//                    Move(ACIMUT_LEFT);
//                else {
//                    Stop(ACIMUT);
//                    acimutInTarget = 1;
//                }
//            }
//            
//            if(!elevacionInTarget) {
//                if(Data_Control.Valor_Actual_Elevacion < (Data_Control.Target_Elevacion - 1))
//                    Move(ELEVACION_UP);
//                else if (Data_Control.Valor_Actual_Elevacion > (Data_Control.Target_Elevacion + 1))
//                    Move(ELEVACION_DOWN);
//                else {
//                    Stop(ELEVACION);
//                    elevacionInTarget = 1;
//                }
//            }
            
            // Timeout de 5 min
            if(millis() - millis_TRACKING > TIMEOUT_TRACKING) {
                Stop(ALL);
                estado_Accionamiento = Sleep;
            }
            
            if(Tracking(Data_Control.Target_Acimut, Data_Control.Target_Elevacion)) {
//                putrsUART2("===================================\n\r");
//                putrsUART2("== ¡LA ESTACION LLEGO A DESTINO! ==\n\r");
//                putrsUART2("===================================\n\r");
                estado_Accionamiento = Sleep;
            }
            
            break;
            
        /* ================  Sleep  ================= */
            
        case Sleep:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
            {
                estado_Accionamiento_anterior = estado_Accionamiento;
//                putrsUART2("estado_Accionamiento = SLEEP\n\r");
            }
//            delayPIC_ms(100);
            break;
            
        default:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
            estado_Accionamiento = Sleep;
    }
}