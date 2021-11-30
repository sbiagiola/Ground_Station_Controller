
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

typedef enum{
    Resta = 0,
    Suma,
}Operacion_Pulsos;  //No es triunfo pero paso cerca...

/*===================== [Variables Internas (Globales)] =====================*/
Last_Value valor_anterior;
_Contador contador;


static Operacion_Pulsos estado_Operacion_Encoder_1;
static Operacion_Pulsos estado_Operacion_Encoder_2;
uint8_t Bandera_Home_Stop_1 = 0;
uint8_t Bandera_Home_Stop_2 = 0;
uint8_t Flag_Parada_Emergencia = 0;
static uint8_t Bandera_Parad_Emerg = 0;
/*===========================================================================*/

/*===================== [Variables Internas (Globales)] =====================*/
Struct_Data_Control Data_Control;


char Datos_A_Enviados[MAX_SIZE_DATA_SEND];
uint32_t Cant_Carac_A_Enviar;
/*===========================================================================*/

extern uint8_t elevation_inHome;
extern Info_Comandos_Procesados Comando_Procesado;
extern Comando_Almacenado Char_Comando;
extern Info_Comandos_Procesados Comando_Procesado;
extern uint8_t nuevoComando;
uint8_t tracking_flag = 0;

void initCN()
{
    valor_anterior.encoder_1_A = ENCODER_1_A;
    valor_anterior.encoder_1_B = ENCODER_1_B;
    valor_anterior.encoder_1_Z = ENCODER_1_Z;
    
    valor_anterior.encoder_2_A = ENCODER_2_A;
    valor_anterior.encoder_2_B = ENCODER_2_B;
    valor_anterior.encoder_2_Z = ENCODER_2_Z;
    
    valor_anterior.anemometro = ANEMOMETRO;
    valor_anterior.parada_emergencia = PARADA_EMERGENCIA;
    
    valor_anterior.home_stop_1 = HOME_STOP_1;
    valor_anterior.home_stop_2 = HOME_STOP_2;
    
    contador.encoder_2_Pulsos = 8;
}

void __attribute__((interrupt,no_auto_psv)) _CNInterrupt(void){
    
    /* ---------------------------   ENCODER 1   --------------------------- */
    // Contador de pulsos
    if((ENCODER_1_A != valor_anterior.encoder_1_A)) {
        if(ENCODER_1_A == HIGH) {
            if(ENCODER_1_B == HIGH) {
                putrsUART2("[CNInterrupt]: Encoder 1 (fase B)\n\r");
                contador.encoder_1_Pulsos++;
                estado_Operacion_Encoder_1 = Suma;
            } else {
                putrsUART2("[CNInterrupt]: Encoder 1 (fase A)\n\r");
                contador.encoder_1_Pulsos--;
                estado_Operacion_Encoder_1 = Resta;
            }
        }
        valor_anterior.encoder_1_A = ENCODER_1_A;
    }
    
    // Contador de vueltas
    // [TO DO] Esto no sirve para nada en nuestro proyecto, se deja o se saca?
    if(ENCODER_1_Z != valor_anterior.encoder_1_Z){
        if(ENCODER_1_Z == HIGH){
            if(estado_Operacion_Encoder_1 == Suma){
                contador.encoder_1_Vueltas++;
            } else {
                contador.encoder_1_Vueltas--;
            }
        }
        valor_anterior.encoder_1_Z = ENCODER_1_Z;
    }
    /* --------------------------------------------------------------------- */
    
    /* ---------------------------   ENCODER 2   --------------------------- */
    // Contador de pulsos
    if((ENCODER_2_A != valor_anterior.encoder_2_A)) {
        if(ENCODER_2_A == HIGH) {
            if(ENCODER_2_B == HIGH) {
                putrsUART2("[CNInterrupt]: Encoder 2 (fase B)\n\r");                
                contador.encoder_2_Pulsos++;
                estado_Operacion_Encoder_2 = Suma;
            } else {
                putrsUART2("[CNInterrupt]: Encoder 2 (fase A)\n\r");
                contador.encoder_2_Pulsos--;
                estado_Operacion_Encoder_2 = Resta;
            }
        }
        valor_anterior.encoder_2_A = ENCODER_2_A;
    }
    
    // Contador de vueltas
    // [TO DO] Esto no sirve para nada en nuestro proyecto, se deja o se saca?
    if(ENCODER_2_Z != valor_anterior.encoder_2_Z){
        if(ENCODER_2_Z == HIGH){
            if(estado_Operacion_Encoder_2 == Suma){
                contador.encoder_2_Vueltas++;
            } else {
                contador.encoder_2_Vueltas--;
            }
        }
        valor_anterior.encoder_2_Z = ENCODER_2_Z;
    }
    /* --------------------------------------------------------------------- */
    
    /* --------------------------   HOME STOP 1   -------------------------- */
    if(HOME_STOP_1 != valor_anterior.home_stop_1){
        putrsUART2("[_CNInterrupt] Home_Stop_1 interrupt detected!");
        
        if(HOME_STOP_1 == HIGH) {
            Stop(ACIMUT);
            contador.encoder_2_Pulsos = 0;
            Bandera_Home_Stop_1 = 1;
        }
        
        valor_anterior.home_stop_1 = HOME_STOP_1;
    }
    /* --------------------------------------------------------------------- */
    
    /* --------------------------   HOME STOP 2   -------------------------- */
    if(HOME_STOP_2 != valor_anterior.home_stop_2){
        putrsUART2("[_CNInterrupt] Home_Stop_2 interrupt detected!");
        
        if(HOME_STOP_2 == HIGH && Bandera_Home_Stop_2 == 0)
            Stop(ELEVACION);
            Bandera_Home_Stop_2 = 1;

        valor_anterior.home_stop_2 = HOME_STOP_2;
    }
    /* --------------------------------------------------------------------- */
    
    /* ----------------------   PARADA DE EMERGENCIA   --------------------- */
    // [TO DO] Cambiar comando por stop directo ??
    if(PARADA_EMERGENCIA != valor_anterior.parada_emergencia){
        putrsUART2("[_CNInterrupt] Parada_Emergencia interrupt detected!");
        if(PARADA_EMERGENCIA == LOW && Bandera_Parad_Emerg == 0){
            Bandera_Parad_Emerg = 1;
            Flag_Parada_Emergencia = Bandera_Parad_Emerg;
            Comando_Procesado.Proximo = Comando_Procesado.Actual;
            Comando_Procesado.Actual = Stop_Global;
            putrsUART2("PE encendida!\n\r");
        }
        else if(PARADA_EMERGENCIA == LOW && Bandera_Parad_Emerg == 1){
            Bandera_Parad_Emerg = 0;
            Flag_Parada_Emergencia = Bandera_Parad_Emerg;
            putrsUART2("PE apagada!\n\r");
        }
        valor_anterior.parada_emergencia = PARADA_EMERGENCIA;
    }
    /* --------------------------------------------------------------------- */
    
    IFS1bits.CNIF = 0; // Clear CN interrupt
}

long get_Acimut(void){
    return contador.encoder_1_Pulsos;
}

long get_Elevacion(void){
    return  contador.encoder_2_Pulsos;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Actualizar_Objetivos(){
    Data_Control.Target_Acimut = atof(Char_Comando.Char_Acimut);
    Data_Control.Target_Elevacion = atof(Char_Comando.Char_Elevacion);
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

ID_Comandos estado_Accionamiento = GoToHome_Elevacion;
ID_Comandos estado_Accionamiento_anterior = Sleep;
uint16_t ciclos_sin_comandos;
uint8_t tracking_home;

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
    if(Flag_Parada_Emergencia == 1) {
        Stop(ALL);
        estado_Accionamiento = Sleep;
    }
    
    switch(estado_Accionamiento){
        
        /* =========  Movimiento manual  ========== */
        
        // ------- Acimut:
        
        case Giro_Horario:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
            if(OUT_RELE_2) {
                OUT_RELE_2 = OFF;
                delayPIC_ms(2000); // [TO DO] Evaluar el tiempo de delay
            }
            OUT_RELE_1 = ON;
            estado_Accionamiento = Sleep;
            break;
            
        case Giro_Antihorario:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
            if(OUT_RELE_1) {
                OUT_RELE_1 = OFF;
                delayPIC_ms(2000);
            }
            OUT_RELE_2 = ON;
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
            
            if(OUT_RELE_4) {
                OUT_RELE_4 = OFF;
                delayPIC_ms(2000);
            }
            OUT_RELE_3 = ON;
            estado_Accionamiento = Sleep;
            break;
            
        case Giro_Abajo:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
            if(OUT_RELE_3) {
                OUT_RELE_3 = OFF;
                delayPIC_ms(2000);
            }
            OUT_RELE_4 = ON;
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
            
        case GoToHome_Elevacion:
            if(estado_Accionamiento != estado_Accionamiento_anterior) {
                estado_Accionamiento_anterior = estado_Accionamiento;
                OUT_RELE_1 = 1;
            }
            
            if(Bandera_Home_Stop_1) {
                delayPIC_ms(2000);
                OUT_RELE_2 = 1;
                Bandera_Home_Stop_1 = 0;
                tracking_home = 1;
            }
    
            if(tracking_home) {
                Data_Control.Valor_Actual_Elevacion = (get_Elevacion()*360.0)/100.0;
                if(Data_Control.Valor_Actual_Elevacion > (180 - 1) && Data_Control.Valor_Actual_Elevacion < (180 + 1))
                {
                    OUT_RELE_1 = OFF;
                    OUT_RELE_2 = OFF;
                    tracking_home = 0;
                    elevation_inHome = 1;
                    estado_Accionamiento = Sleep;
                }
            }
                
            break;
            
            
        case Objetivo_Tracking:
            if(estado_Accionamiento != estado_Accionamiento_anterior) {
                estado_Accionamiento_anterior = estado_Accionamiento;
                Stop(ALL);
                delayPIC_ms(2000);
                tracking_flag = 1;
            }
            
//            Actualizar_Objetivos(Comando_Procesado.Actual);
            Data_Control.Valor_Actual_Elevacion = (get_Elevacion()*360.0)/100.0;
            
//            Calcular_Posicion_Actual(&contador);
//            Control_Posicion_Acimut();
//            Control_Posicion_Elevacion();
            if(Data_Control.Valor_Actual_Elevacion < (Data_Control.Target_Elevacion - 1))
            {
                OUT_RELE_2 = OFF;
                OUT_RELE_1 = ON;
            } 
            else if (Data_Control.Valor_Actual_Elevacion > (Data_Control.Target_Elevacion + 1))
            {
                OUT_RELE_1 = OFF;
                OUT_RELE_2= ON;
            } else
            {
                OUT_RELE_1 = OFF;
                OUT_RELE_2 = OFF;
                tracking_flag = 0;
                estado_Accionamiento = Sleep;
            }      
            
            break;
            
        /* ================  Sleep  ================= */
            
        case Sleep:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
            delayPIC_ms(100);
            break;
            
        default:
            if(estado_Accionamiento != estado_Accionamiento_anterior)
                estado_Accionamiento_anterior = estado_Accionamiento;
            
            estado_Accionamiento = Sleep;
    }
}

//void Control_Posicion_Acimut(void){
//    //if(){   //Mientras no sale el HOME_STOP_X podemos mover
//        if(Data_Control.Target_Acimut > (Data_Control.Valor_Actual_Acimut + OFFSET_ANGULAR_ENCODER_ACIMUT)){
//            // Lógica de control
//            //Girar_Horario();
//        }else{
//            if(Data_Control.Target_Acimut < (Data_Control.Valor_Actual_Acimut + OFFSET_ANGULAR_ENCODER_ACIMUT)){
//                // Lógica de control
//                //Girar_Antihorario();
//            }
//            else{   //Estamos dentro de la zona muerta que no podemos detectar
//                Parar_Acimut();
//            }
//        }
//    //}
//}
//
//void Control_Posicion_Elevacion(void){
//    //if(){   //Mientras no sale el HOME_STOP_X podemos mover
//        if(Data_Control.Target_Elevacion > (Data_Control.Valor_Actual_Elevacion + OFFSET_ANGULAR_ELEVACION)){
//            // Logica de control
//            // Mov_Arriba();
//        }else{
//            if(Data_Control.Target_Elevacion < (Data_Control.Valor_Actual_Elevacion + OFFSET_ANGULAR_ELEVACION)){
//            // Logica de control
//            // Mov_Abajo();
//            }
//            else{   //Estamos dentro de la zona muerta que no podemos detectar
//                Parar_Elevacion();
//            }
//        }
//    // }
//}