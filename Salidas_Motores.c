/* 
 * File:   Datos_Motor
 * Author: Jere
 *
 * Created on 24 de junio de 2021, 10:30
 */

#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


#include "Salidas_Motores.h"

#include "Protocolo_Comm_Yaesu.h"
#include "Entradas.h"
#include "RingBuffer.h"
#include "timer1.h"
#include "UART.h"

/*===================== [Variables Internas (Globales)] =====================*/
Struct_Data_Control Data_Control;


char Datos_A_Enviados[MAX_SIZE_DATA_SEND];
uint32_t Cant_Carac_A_Enviar;
/*===========================================================================*/

/*===================== [Variables Externas (Globales)] =====================*/
extern _Contador contador;

extern uint8_t Flag_Parada_Emergencia; 

extern Comando_Almacenado Char_Comando;
extern Info_Comandos_Procesados Comando_Procesado;
extern uint8_t nuevoComando;

uint8_t tracking_flag = 0;
/*===========================================================================*/

//void Generar_Formato_Mensaje(char* Data_A_Enviar,uint8_t Id_Comando){
//
//    Calcular_Posicion_Actual(&contador);
//    
//    switch(Id_Comando){
//        
//        case Leer_Acimut:
//            Cant_Carac_A_Enviar = sprintf(Data_A_Enviar,"\n+0%.1f\r",Data_Control.Valor_Actual_Acimut);
//        break;
//        
//        case Leer_Elevacion:
//            Cant_Carac_A_Enviar = sprintf(Data_A_Enviar,"\n+0%.1f\r",Data_Control.Valor_Actual_Elevacion);
//        break;
//        
//        default: ;
//    }
//}

/* Esta funcion se encarga de actualizar los valores actuales de los ángulos de acimut y
   elevación en cada llamado de la misma en función de la cantidad de pulsos enviados por cada
   encoder.
 * 
    const _Contador Data (IN) (Solo lectura) -> Cantidad de pulsos y vueltas de cada encoder
 */
//void Calcular_Posicion_Actual(const _Contador* Data){
//    Data_Control.Valor_Actual_Acimut = (360*Data->Encoder_1_Vueltas+Data->Encoder_1_Pulsos)*RESOLUCION_POR_PULSO_ACIMUT;
//    Data_Control.Valor_Actual_Elevacion = (360*Data->Encoder_2_Vueltas+Data->Encoder_2_Pulsos)*RESOLUCION_POR_PULSO_ELEVACION;
//}

void Bajar_Salidas(void){
    LI1_Variador = LOW;     
    LI2_Variador = LOW;
    LI3_Variador = LOW;     
    LI4_Variador = LOW;

    OUT_RELE_1 = LOW;       
    OUT_RELE_2 = LOW;
    OUT_RELE_3 = LOW;       
    OUT_RELE_4 = LOW;
}

void Parar_Elevacion(void){
    LI1_Variador = LOW;     
    LI2_Variador = LOW;
    LI3_Variador = LOW;     
    LI4_Variador = LOW;
}

void Parar_Acimut(void){
    OUT_RELE_1 = LOW;       
    OUT_RELE_2 = LOW;
    OUT_RELE_3 = LOW;       
    OUT_RELE_4 = LOW;
}

void Velocidad_1_El(void){
    /*Analizar cual de toda*/

    /*Sentido uno, arriba o abajo  */
    LI1_Variador = HIGH;     
    LI2_Variador = LOW;

    /*Sentido dos, arriba o abajo */ 
    LI1_Variador = LOW;     
    LI2_Variador = HIGH;
}

void Velocidad_2_El(void){
    /*Analizar cual de toda*/

    /*Sentido uno, arriba o abajo  */
    LI1_Variador = HIGH;     
    LI2_Variador = LOW;

    /*Sentido dos, arriba o abajo */ 
    LI1_Variador = LOW;     
    LI2_Variador = HIGH;
    
    LI4_Variador = LOW;
    LI3_Variador = HIGH;
}

void Velocidad_3_El(void){
    /*Analizar cual de toda*/

    /*Sentido uno, arriba o abajo  */
    LI1_Variador = HIGH;     
    LI2_Variador = LOW;

    /*Sentido dos, arriba o abajo */ 
    LI1_Variador = LOW;     
    LI2_Variador = HIGH;
    
    LI3_Variador = LOW;
    LI4_Variador = HIGH;
}

void Velocidad_4_El(void){
    /*Analizar cual de toda*/

    /*Sentido uno, arriba o abajo  */
    LI1_Variador = HIGH;     
    LI2_Variador = LOW;

    /*Sentido dos, arriba o abajo */ 
    LI1_Variador = LOW;     
    LI2_Variador = HIGH;
    
    LI3_Variador = HIGH;
    LI4_Variador = HIGH;
}

void Mov_Arriba(void){
    //Analizar cual de toda

    //Sentido uno, arriba o abajo  
    LI1_Variador = HIGH;     
    LI2_Variador = LOW;

    //Sentido dos, arriba o abajo 
    LI1_Variador = LOW;     
    LI2_Variador = HIGH;
}

void Mov_Abajo(void){
    //Analizar cual de toda

    //Sentido uno, arriba o abajo  
    LI1_Variador = LOW;     
    LI2_Variador = HIGH;

    //Sentido dos, arriba o abajo  
    LI1_Variador = HIGH;     
    LI2_Variador = LOW;
}

void Girar_Horario(void){
    // Hay que ver cual salida a relé genera el giro horario y bajar los que no 
    //OUT_RELE_1 = HIGH;       
    //OUT_RELE_2 = HIGH;
    //OUT_RELE_3 = HIGH;       
    //OUT_RELE_4 = HIGH;    
}

void Girar_Antihorario(void){
    // Hay que ver cual salida a relé genera el giro horario y bajar los que no 
    //OUT_RELE_1 = HIGH;       
    //OUT_RELE_2 = HIGH;
    //OUT_RELE_3 = HIGH;       
    //OUT_RELE_4 = HIGH;
}

//void MEF_Accionamiento(void){
//           
//    switch(Comando_Procesado.Actual){
//        case Parar_Todo:
//            Bajar_Salidas();
//        break;
//
//        case Giro_Horario:
//            // Hay problema cuando estas en este estado y te mandan otro comando. Ahi quedan las salidas arriba. ver esto
//            if(Get_Estado_Temporizadores() != Temporizador_1){
//                if(Set_Temporizador(Temporizador_1,5)){
//                    Girar_Horario();
//                }
//            }else{  //Get_Estado_Temporizadores() == Temporizador_2
//                Comando_Procesado.Actual = Stop_Acimut;
//            }
//        break;
//
//        case Giro_Antihorario:
//            // Disparo un temporizador que en X mSeg cambie el estado a parar acimut.
//            if(Get_Estado_Temporizadores() != Temporizador_1){
//                if(Set_Temporizador(Temporizador_1,5)){
//                    Girar_Antihorario();
//                }
//            }else{  //Get_Estado_Temporizadores() == Temporizador_2
//                Comando_Procesado.Actual = Stop_Acimut;
//            }
//        break;
//
//        case Stop_Acimut:
//            Parar_Acimut();
//        break;
//
//        case Devolver_Valor_Acimut:
//            Generar_Formato_Mensaje(Datos_A_Enviados,Devolver_Valor_Acimut);
//            uart_ringBuffer_envDatos_U2((uint8_t*)Datos_A_Enviados,Cant_Carac_A_Enviar);
//            Comando_Procesado.Actual = Comando_Procesado.Ultimo;
//        break;
//
//        case Arriba:
//            // Disparo un temporizador que en X mSeg cambie el estado a parar elevacion.
//            if(Get_Estado_Temporizadores() != Temporizador_2){
//                if(Set_Temporizador(Temporizador_2,5)){
//                    Mov_Arriba();
//                }
//            }else{  //Get_Estado_Temporizadores() == Temporizador_1
//                Comando_Procesado.Actual = Stop_Elevacion;
//            }
//        break;
//
//        case Abajo:
//            // Disparo un temporizador que en X mSeg cambie el estado a parar elevacion.
//            if(Get_Estado_Temporizadores() != Temporizador_2){
//                if(Set_Temporizador(Temporizador_2,5)){
//                    Mov_Abajo();
//                }
//            }else{  //Get_Estado_Temporizadores() == Temporizador_1
//                Comando_Procesado.Actual = Stop_Elevacion;
//            }
//        break;
//
//        case Stop_Elevacion:
//            Parar_Elevacion();
//        break;
//
//        case Devolver_Valor_Elevacion:
//            Generar_Formato_Mensaje(Datos_A_Enviados,Devolver_Valor_Elevacion);
//            uart_ringBuffer_envDatos_U2((uint8_t*)Datos_A_Enviados,Cant_Carac_A_Enviar);
//            Comando_Procesado.Actual = Comando_Procesado.Ultimo;
//        break;
//
//        case Mayor_Presicion_a_e_grados:
//            Calcular_Posicion_Actual(&Contador);
//            Control_Posicion_Acimut();
//            Control_Posicion_Elevacion();
//        break;
//        
//        default: Comando_Procesado.Actual = Parar_Todo;
//        }
// }

void Actualizar_Objetivos(){
    Data_Control.Target_Acimut = atof(Char_Comando.Char_Acimut);
    Data_Control.Target_Elevacion = atof(Char_Comando.Char_Elevacion);
}

//Estado_MEF_Principal Identificar_Tipo_Comando(uint8_t ID_Comando){
//    
//    if(ID_Comando == Giro_Horario || ID_Comando == Giro_Antihorario || ID_Comando == Stop_Acimut || 
//       ID_Comando == Arriba || ID_Comando == Abajo || ID_Comando == Stop_Elevacion){
//        return Movimiento_Manual;
//    }
//    
//    if(ID_Comando == Hacia_aaa_grados || ID_Comando == Hacia_aaa_eee_grados || ID_Comando == Mayor_Presicion_a_e_grados ||
//       ID_Comando == Mayor_Presicion_a_grados || ID_Comando == Mayor_Presicion_e_grados){
//        return Tracking;
//    }
//    
//    if(ID_Comando == Devolver_Valor_A_E || ID_Comando == Devolver_Valor_Acimut || ID_Comando == Devolver_Valor_Elevacion){
//        return Enviar_Posicion;
//    }
//    if(ID_Comando == Velocidad_1_Elevacion || ID_Comando == Velocidad_2_Elevacion || 
//       ID_Comando == Velocidad_3_Elevacion || ID_Comando == Velocidad_4_Elevacion){
//        return Cambio_Veloc_Elevacion;
//    }
//
//return Stop;  
//}

//static Estado_MEF_Principal Estado_Actual_MEF_Principal = Sleep;
//
//void MEF_Principal(void){
//    
//    if(nuevoComando > 0)
//    {
//        Estado_Actual_MEF_Principal = Comando_Procesado.Actual;
//        nuevoComando = 0;
//    } else {
//        Estado_Actual_MEF_Principal = Sleep;
//    }
//    
////    static Estado_MEF_Principal Estado_Actual_MEF_Principal = Sleep;
////    static Estado_MEF_Principal Estado_Anterior_MEF_Principal = Sleep;
//    
////    Estado_Anterior_MEF_Principal = Estado_Actual_MEF_Principal;
////    Estado_Actual_MEF_Principal = Identificar_Tipo_Comando(Comando_Procesado.Actual);
//    
//    switch(Estado_Actual_MEF_Principal){
//        
//        case Giro_Horario:
//            OUT_RELE_1 = ON;
//            Estado_Actual_MEF_Principal = Sleep;
//            break;
//            
//        case Giro_Antihorario:
//            OUT_RELE_2 = ON;
//            Estado_Actual_MEF_Principal = Sleep;
//            break;
//            
//        case Stop_Acimut:
//            OUT_RELE_1 = OFF;
//            OUT_RELE_2 = OFF;
//            Estado_Actual_MEF_Principal = Sleep;
//            break;
//                    
//        case Arriba:
//            OUT_RELE_3 = ON;
//            Estado_Actual_MEF_Principal = Sleep;
//            break;
//            
//        case Abajo:
//            OUT_RELE_4 = ON;
//            Estado_Actual_MEF_Principal = Sleep;
//            break;
//            
//        case Stop_Elevacion:
//            OUT_RELE_3 = OFF;
//            OUT_RELE_4 = OFF;
//            Estado_Actual_MEF_Principal = Sleep;
//            break;
//        
//        case Stop:
//            Bajar_Salidas();
//        break;
//        
//        case Tracking:
//            
//            if(Estado_Anterior_MEF_Principal == Movimiento_Manual){
//                Bajar_Salidas();
//            }
//            
//            if(Flag_Parada_Emergencia){
//                Estado_Actual_MEF_Principal = Parar_Todo;
//                break;
//            }
//            
//            Actualizar_Objetivos(Comando_Procesado.Actual);
//            Calcular_Posicion_Actual(&Contador);
//            Control_Posicion_Acimut();
//            Control_Posicion_Elevacion();
//            
//        break;
//        
//        case Movimiento_Manual:
//            
//            if(Estado_Anterior_MEF_Principal == Tracking){
//                Bajar_Salidas();
//            }
//            
//            if(Flag_Parada_Emergencia){
//                Estado_Actual_MEF_Principal = Parar_Todo;
//                break;
//            }
//            
//            MEF_Movimiento_Manual();
//            
//        break;
//        
//        case Enviar_Posicion:
//            Generar_Formato_Mensaje(Datos_A_Enviados,Comando_Procesado.Actual);
//            uart_ringBuffer_envDatos_U2((uint8_t*)Datos_A_Enviados,Cant_Carac_A_Enviar);
//            Estado_Actual_MEF_Principal = Estado_Anterior_MEF_Principal;
//        break;
//        
//        case Cambio_Veloc_Elevacion:
//            if(Comando_Procesado.Actual == Velocidad_1_Elevacion){
//                Velocidad_1_El();
//            }
//            if(Comando_Procesado.Actual == Velocidad_2_Elevacion){
//                Velocidad_2_El();
//            }
//            if(Comando_Procesado.Actual == Velocidad_3_Elevacion){
//                Velocidad_3_El();
//            }
//            if(Comando_Procesado.Actual == Velocidad_4_Elevacion){
//                Velocidad_4_El();
//            }
//            Estado_Actual_MEF_Principal = Estado_Anterior_MEF_Principal;
//        break;
//        
//        case Sleep:
//            break;
//        
//        default: Estado_Actual_MEF_Principal = Stop;
//
//    }
//}

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

ID_Comandos estado_Accionamiento = Sleep;
ID_Comandos estado_Accionamiento_anterior = Sleep;
uint16_t ciclos_sin_comandos;

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






void Control_Posicion_Acimut(void){
    //if(){   //Mientras no sale el HOME_STOP_X podemos mover
        if(Data_Control.Target_Acimut > (Data_Control.Valor_Actual_Acimut + OFFSET_ANGULAR_ENCODER_ACIMUT)){
            // Lógica de control
            //Girar_Horario();
        }else{
            if(Data_Control.Target_Acimut < (Data_Control.Valor_Actual_Acimut + OFFSET_ANGULAR_ENCODER_ACIMUT)){
                // Lógica de control
                //Girar_Antihorario();
            }
            else{   //Estamos dentro de la zona muerta que no podemos detectar
                Parar_Acimut();
            }
        }
    //}
}
void Control_Posicion_Elevacion(void){
    //if(){   //Mientras no sale el HOME_STOP_X podemos mover
        if(Data_Control.Target_Elevacion > (Data_Control.Valor_Actual_Elevacion + OFFSET_ANGULAR_ELEVACION)){
            // Logica de control
            // Mov_Arriba();
        }else{
            if(Data_Control.Target_Elevacion < (Data_Control.Valor_Actual_Elevacion + OFFSET_ANGULAR_ELEVACION)){
            // Logica de control
            // Mov_Abajo();
            }
            else{   //Estamos dentro de la zona muerta que no podemos detectar
                Parar_Elevacion();
            }
        }
    // }
}
