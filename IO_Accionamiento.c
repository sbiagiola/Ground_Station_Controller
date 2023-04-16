
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
uint8_t flag_hs_e = 0;
uint8_t flag_hs_a = 0;


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

//ID_Comandos estado_Accionamiento = GoToHome_Acimut;
ID_Comandos estado_Accionamiento_anterior = Sleep;
ID_Comandos estado_Accionamiento = Sleep;
uint16_t ciclos_sin_comandos;
uint8_t stateEncoder_AZ = 0;
uint8_t stateEncoder_EL = 0;

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
    
    contador.encoderAz_Pulsos = 92571;
    contador.encoderElev_Pulsos = 90;
}

uint32_t millis_ELEV_A = 0;
uint32_t millis_ELEV_B = 0;

void __attribute__((interrupt,no_auto_psv)) _CNInterrupt(void){
    
    /* ---------------------------   ENCODER ELEVACION   --------------------------- */
    
    /* ----------------------------------------------------------------------------- */
    
    /* -----------------------------   ENCODER ACIMUT   ---------------------------- */
    // Contador de pulsos
    
    
    if(ENCODER_AZ_A != valor_anterior.encoderAz_A)
    {
        if(ENCODER_AZ_A == HIGH)
        {
            if(stateEncoder_AZ == 0)
            {
                stateEncoder_AZ = 1;
            }
            else 
            {
                stateEncoder_AZ = 2;
            }
        }
        else
        {
            if(stateEncoder_AZ == 2)
            {
                stateEncoder_AZ = 3;
            }
            else
            {
                if(stateEncoder_AZ == 3)
                {
                    // pulso
                    contador.encoderAz_Pulsos--;
                }
                else
                {
                    // RUIDO. Se descarta el pulso
                }
                stateEncoder_AZ = 0;
            }
        }
        valor_anterior.encoderAz_A = ENCODER_AZ_A;
    }
    
    if(ENCODER_AZ_B != valor_anterior.encoderAz_B)
    {
        if(ENCODER_AZ_B == HIGH)
        {
            if(stateEncoder_AZ == 1)
            {
                stateEncoder_AZ = 2;
            }
            else 
            {
                if(stateEncoder_AZ == 0)
                {
                    stateEncoder_AZ = 1;
                }
                else
                {
                    stateEncoder_AZ = 2;
                }
            }
        }
        else
        {
            if(stateEncoder_AZ == 3)
            {
                contador.encoderAz_Pulsos++;
                stateEncoder_AZ = 0;
            }
            else
            {
                if(stateEncoder_AZ == 2)
                {
                    stateEncoder_AZ = 3;
                }
                else
                {
                    // RUIDO. Se descarta el pulso
                    stateEncoder_AZ = 0;
                }
            }
        }
        valor_anterior.encoderAz_B = ENCODER_AZ_B;
    }

    /* ----------------------------------------------------------------------------- */
    
    /* --------------------------   HOME STOP ELEVACION   -------------------------- */
    if(HOME_STOP_ELEV != valor_anterior.home_stop_Elev){
        if(HOME_STOP_ELEV == HIGH) {
            flag_hs_e = 1;
        }
        valor_anterior.home_stop_Elev = HOME_STOP_ELEV;
    }
    /* ----------------------------------------------------------------------------- */
    
    /* ----------------------------   HOME STOP ACIMUT   --------------------------- */
//    if(HOME_STOP_AZ == HIGH) {putrsUART2("1");}
//    else if(HOME_STOP_AZ == LOW) {putrsUART2("0");}
//    
//    if(valor_anterior.home_stop_Az == HIGH) {putrsUART2("a1\n\r");}
//    else if(valor_anterior.home_stop_Az == LOW) {putrsUART2("a0\n\r");}
    
    if(HOME_STOP_AZ != valor_anterior.home_stop_Az){
        valor_anterior.home_stop_Az = HOME_STOP_AZ;
        
        if(HOME_STOP_AZ == HIGH && READ_RELE_2 == ON){
            flag_hs_a = 1;
        }
    }
    /* ----------------------------------------------------------------------------- */
    
    /* --------------------------   PARADA DE EMERGENCIA   ------------------------- */
    if(PARADA_EMERGENCIA != valor_anterior.parada_emergencia && PARADA_EMERGENCIA == LOW){
//        putrsUART2("====================================\n\r");
//        putrsUART2("=====  ¡PARADA DE EMERGENCIA!  =====\n\r");
//        putrsUART2("====================================\n\r");
//        Stop(ALL);
        estado_Accionamiento = Sleep;
       
        valor_anterior.parada_emergencia = PARADA_EMERGENCIA;
    }
    
    /* ----------------------------------------------------------------------------- */
    
    IFS1bits.CNIF = 0; // Clear CN interrupt
}

double get_Acimut(void){
    double ang = (contador.encoderAz_Pulsos * GRADOS_POR_VUELTA) / RESOLUCION_ENCODER;
    ang = ang*REDUCCION_ENCODER_ANTENA_ACIMUT;
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
    if(Data_Control.Target_Acimut < 160){
        Data_Control.Target_Acimut = Data_Control.Target_Acimut + 360;
    }
    Data_Control.Target_Elevacion = atof(Char_Comando.Char_Elevacion);
}

uint8_t getStatusEL()
{
    return stateEncoder_EL;
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

void MEF_Accionamiento(){
    
    if(flag_hs_e==1){
        delayPIC_ms(200);
        if(HOME_STOP_ELEV == HIGH){
            putrsUART2("HE\n\r");
            Stop(ELEVACION);
//            delayPIC_ms(DELAY_CAMBIO_SENTIDO);
            contador.encoderElev_Pulsos = 45;
//            if(HomeStop_Elev_init == 1)
//                elevacionInTarget = 1;
                //estado_Accionamiento = Sleep;
            flag_HomeStop_Elev = 1;
        }
        flag_hs_e=0;
    }

    if(flag_hs_a==1){
        delayPIC_ms(200);
        if(HOME_STOP_AZ == HIGH){
            putrsUART2("HA\n\r");
            Stop(ACIMUT);
//            delayPIC_ms(DELAY_CAMBIO_SENTIDO);
            double ang = 510;
            contador.encoderAz_Pulsos = ((ang / REDUCCION_ENCODER_ANTENA_ACIMUT)* RESOLUCION_ENCODER)/GRADOS_POR_VUELTA;
//            (contador.encoderAz_Pulsos * GRADOS_POR_VUELTA) / RESOLUCION_ENCODER;
//    ang = ang*REDUCCION_ENCODER_ANTENA_ACIMUT;
    
//            contador.encoderAz_Pulsos = 185142;
//            if(HomeStop_Az_init == 1)
//                acimutInTarget = 1;
                //estado_Accionamiento = Sleep;
            flag_HomeStop_Az = 1;
        }
        flag_hs_a = 0;
    }
    
    if(ENCODER_ELEV_A != valor_anterior.encoderElev_A)
    {
        valor_anterior.encoderElev_A = ENCODER_ELEV_A;
        if(ENCODER_ELEV_A == HIGH)
        {
            switch(stateEncoder_EL)
            {
                case 0:
                    stateEncoder_EL = 1;
//                        putrsUART2("1A\n\r");
                    break;

                case 1:
                    stateEncoder_EL = 2;
//                        putrsUART2("2A\n\r");
                    break;

                case 2:
                case 3:
                    stateEncoder_EL = 0;
//                        putrsUART2("r0A\n\r");
                    break;
            }
        }
        else if(ENCODER_ELEV_A == LOW)
        {
            switch(stateEncoder_EL)
            {
                case 0:
                case 1:
                    stateEncoder_EL = 0;
//                        putrsUART2("r0A\n\r");
                    break;

                case 2:
                    stateEncoder_EL = 3;
//                        putrsUART2("3A\n\r");
                    break;

                case 3:
                    // pulso
                    contador.encoderElev_Pulsos++;
                    stateEncoder_EL = 0;
//                        putrsUART2("p0A\n\r");
                    break;
            }
        }
    }

    if(ENCODER_ELEV_B != valor_anterior.encoderElev_B)
    {
        valor_anterior.encoderElev_B = ENCODER_ELEV_B;
        if(ENCODER_ELEV_B == HIGH)
        {
            switch(stateEncoder_EL)
            {
                case 0:
                    stateEncoder_EL = 1;
//                        putrsUART2("1B\n\r");
                    break;

                case 1:
                    stateEncoder_EL = 2;
//                        putrsUART2("2B\n\r");
                    break;

                case 2:
                case 3:
                    stateEncoder_EL = 0; // 2
//                        putrsUART2("r0B\n\r");
                    break;
            }
        }
        else if(ENCODER_ELEV_B == LOW)
        {
            switch(stateEncoder_EL)
            {
                case 0:
                case 1:
                    stateEncoder_EL = 0;
//                        putrsUART2("0B\n\r");
                    break;

                case 2:
                    stateEncoder_EL = 3;
//                        putrsUART2("3B\n\r");
                    break;

                case 3:
                    // pulso
                    contador.encoderElev_Pulsos--;
                    stateEncoder_EL = 0;
//                        putrsUART2("p0B\n\r");
                    break;
            }
        }
    }
    
    if(nuevoComando > 0 && (estado_Accionamiento != GoToHome_Acimut && estado_Accionamiento != GoToHome_Elevacion))
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
                Move(ACIMUT_RIGHT); // me muevo antihorario buscando el 0
            }
            
            // Timeout de 10 min
            if(millis() - millis_INIT > TIMEOUT_INIT) {
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
                millis_INIT = millis();
                Move(ELEVACION_DOWN);
            }
            
            // Timeout de 10 min
            if(millis() - millis_INIT > TIMEOUT_INIT) {
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
//                    putrsUART2("Estacion en HOME... Inicializacion finalizada!\n\r");
                    estado_Accionamiento = Sleep;
                }
            }
                
            break;
            
        case Return_ToHome:
//            if(estado_Accionamiento != estado_Accionamiento_anterior) {
//                estado_Accionamiento_anterior = estado_Accionamiento;
//                acimutInTarget = 0;
//                elevacionInTarget = 0;
//            }
//            
//            // Timeout de 5 min
//            if(millis() - millis_TRACKING > TIMEOUT_TRACKING) {
//                Stop(ALL);
//                estado_Accionamiento = Sleep;
//            }
//            
//            if(Tracking(HOME_ACIMUT, HOME_ELEVACION)) {
////                putrsUART2("====================================\n\r");
////                putrsUART2("==== ¡LA ESTACION LLEGO A HOME! ====\n\r");
////                putrsUART2("====================================\n\r");
//                estado_Accionamiento = Sleep;
//            }
            
            estado_Accionamiento = GoToHome_Acimut;
            
            break;
            
        case Objetivo_Tracking:
            if(estado_Accionamiento != estado_Accionamiento_anterior) {
                estado_Accionamiento_anterior = estado_Accionamiento;
//                Stop(ALL);
//                delayPIC_ms(DELAY_CAMBIO_SENTIDO);
                acimutInTarget = 0;
                elevacionInTarget = 0;
 //               putrsUART2("Iniciando tracking...\n\r");
                millis_TRACKING = millis();
            }
            
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
//                putrsUART2("== ¡LA ESTACION LLEGO A DESTINO! ==\n\r");
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