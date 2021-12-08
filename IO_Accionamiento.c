
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

//typedef enum{
//    Resta = 0,
//    Suma,
//}Operacion_Pulsos;  //No es triunfo pero paso cerca...

/*===================== [Variables Internas (Globales)] =====================*/
Last_Value valor_anterior;
_Contador contador;


//static Operacion_Pulsos estado_Encoder_Elev;
//static Operacion_Pulsos estado_Encoder_Az;
uint8_t flag_HomeStop_Elev = 0;
uint8_t flag_HomeStop_Az = 0;
uint8_t HomeStop_Az_init = 0;
uint8_t Flag_Parada_Emergencia = 0;
static uint8_t Bandera_Parad_Emerg = 0;
/*===========================================================================*/

/*===================== [Variables Internas (Globales)] =====================*/
Struct_Data_Control Data_Control;


char Datos_A_Enviados[MAX_SIZE_DATA_SEND];
uint32_t Cant_Carac_A_Enviar;
/*===========================================================================*/

ID_Comandos estado_Accionamiento = GoToHome_Acimut;
ID_Comandos estado_Accionamiento_anterior = Sleep;
uint16_t ciclos_sin_comandos;
uint8_t tracking_home;

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
}

void __attribute__((interrupt,no_auto_psv)) _CNInterrupt(void){
    
    /* ---------------------------   ENCODER ELEVACION   --------------------------- */
    // Contador de pulsos
    if((ENCODER_ELEV_A != valor_anterior.encoderElev_A)) {
        if(ENCODER_ELEV_A == HIGH) {
            if(ENCODER_ELEV_B == HIGH) {
                putrsUART2("[CNInterrupt]: Encoder 1 (fase B)\n\r");
                contador.encoderElev_Pulsos++;
//                estado_Encoder_Elev = Suma;
            } else {
                putrsUART2("[CNInterrupt]: Encoder 1 (fase A)\n\r");
                contador.encoderElev_Pulsos--;
//                estado_Encoder_Elev = Resta;
            }
        }
        valor_anterior.encoderElev_A = ENCODER_ELEV_A;
    }
    
    // Contador de vueltas
    // [TO DO] Esto no sirve para nada en nuestro proyecto, se deja o se saca?
//    if(ENCODER_ELEV_Z != valor_anterior.encoderElev_Z){
//        if(ENCODER_ELEV_Z == HIGH){
//            if(estado_Encoder_Elev == Suma){
//                contador.encoderElev_Vueltas++;
//            } else {
//                contador.encoderElev_Vueltas--;
//            }
//        }
//        valor_anterior.encoderElev_Z = ENCODER_ELEV_Z;
//    }
    /* ----------------------------------------------------------------------------- */
    
    /* -----------------------------   ENCODER ACIMUT   ---------------------------- */
    // Contador de pulsos
    if((ENCODER_AZ_A != valor_anterior.encoderAz_A)) {
        if(ENCODER_AZ_A == HIGH) {
            if(ENCODER_AZ_B == HIGH) {
                putrsUART2("[CNInterrupt]: Encoder 2 (fase B)\n\r");                
                contador.encoderAz_Pulsos++;
            } else {
                putrsUART2("[CNInterrupt]: Encoder 2 (fase A)\n\r");
                contador.encoderAz_Pulsos--;
            }
        }
        valor_anterior.encoderAz_A = ENCODER_AZ_A;
    }
    
    // Contador de vueltas
    // [TO DO] Esto no sirve para nada en nuestro proyecto, se deja o se saca?
//    if(ENCODER_AZ_Z != valor_anterior.encoderAz_Z){
//        if(ENCODER_AZ_Z == HIGH){
//            if(estado_Encoder_Az == Suma){
//                contador.encoderAz_Vueltas++;
//            } else {
//                contador.encoderAz_Vueltas--;
//            }
//        }
//        valor_anterior.encoderAz_Z = ENCODER_AZ_Z;
//    }
    /* ----------------------------------------------------------------------------- */
    
    /* --------------------------   HOME STOP ELEVACION   -------------------------- */
    if(HOME_STOP_ELEV != valor_anterior.home_stop_Elev){
        putrsUART2("[_CNInterrupt] Home_Stop_1 interrupt detected!\n\r");
        
        if(HOME_STOP_ELEV == HIGH) {
            Stop(ELEVACION);
            
            flag_HomeStop_Elev = 1;
        }
        
        valor_anterior.home_stop_Elev = HOME_STOP_ELEV;
    }
    /* ----------------------------------------------------------------------------- */
    
    /* ----------------------------   HOME STOP ACIMUT   --------------------------- */
    if(HOME_STOP_AZ != valor_anterior.home_stop_Az){
        putrsUART2("[_CNInterrupt] Home_Stop_2 interrupt detected!\n\r");
        
        if(HOME_STOP_AZ == HIGH)
        {
            Stop(ACIMUT);
            contador.encoderAz_Pulsos = 0;
            if(HomeStop_Az_init == 1)
                estado_Accionamiento = Sleep;
            flag_HomeStop_Az = 1;
        }

        valor_anterior.home_stop_Az = HOME_STOP_AZ;
    }
    /* ----------------------------------------------------------------------------- */
    
    /* --------------------------   PARADA DE EMERGENCIA   ------------------------- */
    // [TO DO] Cambiar comando por stop directo ??
    if(PARADA_EMERGENCIA != valor_anterior.parada_emergencia && PARADA_EMERGENCIA == LOW){
        putrsUART2("[_CNInterrupt] Parada_Emergencia interrupt detected!\n\r");
        Stop(ALL);
        estado_Accionamiento = Sleep;
       
//        if(PARADA_EMERGENCIA == LOW && Bandera_Parad_Emerg == 0){
//            Bandera_Parad_Emerg = 1;
//            Flag_Parada_Emergencia = Bandera_Parad_Emerg;
//            Comando_Procesado.Proximo = Comando_Procesado.Actual;
//            Comando_Procesado.Actual = Stop_Global;
//            putrsUART2("PE encendida!\n\r");
//        }
//        else if(PARADA_EMERGENCIA == LOW && Bandera_Parad_Emerg == 1){
//            Bandera_Parad_Emerg = 0;
//            Flag_Parada_Emergencia = Bandera_Parad_Emerg;
//            putrsUART2("PE apagada!\n\r");
//        }
        
    }
    valor_anterior.parada_emergencia = PARADA_EMERGENCIA;
    /* ----------------------------------------------------------------------------- */
    
    IFS1bits.CNIF = 0; // Clear CN interrupt
}

long get_Acimut(void){
    return contador.encoderAz_Pulsos;
}

long get_Elevacion(void){
    return  contador.encoderElev_Pulsos;
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
                delayPIC_ms(2000);
            }
            OUT_RELE_1 = ON;
            break;
            
        case ACIMUT_LEFT:
            if(READ_RELE_1 == ON) {
                OUT_RELE_1 = OFF;
                delayPIC_ms(2000);
            }
            OUT_RELE_2 = ON;
            break;
            
        case ELEVACION_UP:
            if(READ_RELE_4 == ON) {
                OUT_RELE_4 = OFF;
                delayPIC_ms(2000);
            }
            OUT_RELE_3 = ON;
            break;
            
        case ELEVACION_DOWN:
            if(READ_RELE_3 == ON) {
                OUT_RELE_3 = OFF;
                delayPIC_ms(2000);
            }
            OUT_RELE_4 = ON;
            break;
            
        default:
            break;
    }
}

void Stop(OUT out) {
    switch(out)
    {
        case ALL:
            OUT_RELE_1 = OFF;
            OUT_RELE_2 = OFF;
            OUT_RELE_3 = OFF;
            OUT_RELE_4 = OFF;
            break;
        
        case ACIMUT:
            OUT_RELE_1 = OFF;
            OUT_RELE_2 = OFF;
            break;
            
        case ELEVACION:
            OUT_RELE_3 = OFF;
            OUT_RELE_4 = OFF;
            break;
                    
        default:
            break;
    }
}

void MEF_Accionamiento(){
    
    if(nuevoComando > 0)
    {
        putrsUART2("NUEVO COMANDO\n");
        estado_Accionamiento_anterior = estado_Accionamiento;
        estado_Accionamiento = Comando_Procesado.Actual;
        nuevoComando = 0;
        ciclos_sin_comandos = 0;
    } else {
        ciclos_sin_comandos++;
        if(ciclos_sin_comandos == 100000) {
            // [TO DO] Rutina para volver a posicion de home antes de Sleep ???
            estado_Accionamiento = Sleep;
            ciclos_sin_comandos = 0;
        }
    }
    
    // Parada de emergencia
//    if(Flag_Parada_Emergencia == 1) {
//        Stop(ALL);
//        estado_Accionamiento = Sleep;
//    }
    
    switch(estado_Accionamiento){
        
        /* =========  Movimiento manual  ========== */
        
        // ------- Acimut:
        
        case Giro_Horario:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
//            if(OUT_RELE_2) {
//                OUT_RELE_2 = OFF;
//                delayPIC_ms(2000); // [TO DO] Evaluar el tiempo de delay
//            }
//            OUT_RELE_1 = ON;
            Move(ACIMUT_RIGHT);
            estado_Accionamiento = Sleep;
            break;
            
        case Giro_Antihorario:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
//            if(OUT_RELE_1) {
//                OUT_RELE_1 = OFF;
//                delayPIC_ms(2000);
//            }
//            OUT_RELE_2 = ON;
            Move(ACIMUT_LEFT);
            estado_Accionamiento = Sleep;
            break;
            
        case Stop_Acimut:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
            Stop(ACIMUT);
            estado_Accionamiento = Sleep;
            break;
            
        // ------- Elevacion:
                    
        case Giro_Arriba:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
//            if(OUT_RELE_4) {
//                OUT_RELE_4 = OFF;
//                delayPIC_ms(2000);
//            }
//            OUT_RELE_3 = ON;
            Move(ELEVACION_UP);
            estado_Accionamiento = Sleep;
            break;
            
        case Giro_Abajo:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
//            if(OUT_RELE_3) {
//                OUT_RELE_3 = OFF;
//                delayPIC_ms(2000);
//            }
//            OUT_RELE_4 = ON;
            Move(ELEVACION_DOWN);
            estado_Accionamiento = Sleep;
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
            
            if(flag_HomeStop_Az) {
                delayPIC_ms(2000);
                Move(ACIMUT_LEFT);
                flag_HomeStop_Az = 0;
                tracking_home = 1;
            }
    
            if(tracking_home) {
                Data_Control.Valor_Actual_Acimut = (get_Acimut()*360.0)/100.0;
                if(Data_Control.Valor_Actual_Acimut > (180 - 1) && Data_Control.Valor_Actual_Acimut < (180 + 1))
                {
                    Stop(ACIMUT);
                    tracking_home = 0;
                    //elevation_inHome = 1;
                    HomeStop_Az_init = 1;
                    estado_Accionamiento = Sleep;
                }
            }
                
            break;
            
            
        case Objetivo_Tracking:
            if(estado_Accionamiento != estado_Accionamiento_anterior) {
                estado_Accionamiento_anterior = estado_Accionamiento;
                Stop(ALL);
                delayPIC_ms(2000);
//                tracking_flag = 1;
            }
            
            Data_Control.Valor_Actual_Acimut = (get_Acimut()*360.0)/100.0;
            
//            Calcular_Posicion_Actual(&contador);

            if(Data_Control.Valor_Actual_Acimut < (Data_Control.Target_Acimut - 1))
            {
                Move(ACIMUT_RIGHT);
            } 
            else if (Data_Control.Valor_Actual_Acimut > (Data_Control.Target_Acimut + 1))
            {
                Move(ACIMUT_LEFT);
            } else
            {
                Stop(ACIMUT);
//                tracking_flag = 0;
                estado_Accionamiento = Sleep;
            }      
            
            break;
            
        /* ================  Sleep  ================= */
            
        case Sleep:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
            {
                estado_Accionamiento_anterior = estado_Accionamiento;
                putrsUART2("estado_Accionamiento = SLEEP\n\r");
            }
            delayPIC_ms(100);
            break;
            
        default:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
            estado_Accionamiento = Sleep;
    }
}