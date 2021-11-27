/*
 * Author: seba
 / file: Control_automatico.c/
 * Create:18/07/2021
 */
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Control_automatico.h"
#include "DAC.h"
#include "Clock.h"
#include "libpic30.h"   
#include "IO_Config.h"
#include "UART.h"
#include "ADC.h"
#include "RingBuffer.h"
#include "Protocolo_Comm_Yaesu.h"
#include "interrupts.h"
//#include "Entradas.h"
//#include "Salidas_Motores.h"
#include "IO_Accionamiento.h"


/*
 * Variables 
 */

uint32_t  posicion_final_acimut ;
uint32_t  posicion_final_elevacion ;
uint32_t  posicion_inicial_acimut ;
uint32_t  posicion_inicial_elevacion;
bool posicion_acimut_alcanzada=false;
bool posicion_elevacion_alcanzada=false;
uint8_t State= RESET;




/*
 * Fin de variables
 */

/*
 * debo llamar a esta funcion para inciar movimiento
 */
void inicia_control( uint32_t *Pos_final_acimut,uint32_t *Pos_final_elevacion){
    posicion_final_acimut=*Pos_final_acimut;
//    posicion_inicial_acimut= posicion_actual_acimut();
    posicion_final_elevacion=*Pos_final_elevacion;
//    posicion_inicial_elevacion= posicion_actual_elevacion();
    State=CARGANDO_VAR;
}

/*
 * se debe correr cada cierto tiempo,
 */
//void Control_automatico(void){
//    /*
//     * Debo consultar si cargaron variables
//     */
//    if (State==CARGANDO_VAR){
//    
//        //acimut
//         if (posicion_final_acimut > posicion_actual_acimut()){
//
//            Girar_Horario();
//         }
//
//         if (posicion_final_acimut < posicion_actual_acimut()){
//             Girar_Antihorario();
//         }
//
//         if((abs(posicion_final_acimut)-abs(posicion_actual_acimut()<0.1))){
//            posicion_acimut_alcanzada=true;
//         }
//
//
//
//         //elevacion
//
//         if (posicion_final_elevacion >= posicion_actual_elevacion()){
//
//             if((posicion_final_elevacion-posicion_actual_elevacion())>=3){
//                 //velocidad rapida elevacion
//             }
//
//             if((posicion_final_elevacion-posicion_actual_elevacion())<3){
//                 //velocidad lenta elevacion
//             }
//
//         }
//
//
//         if (posicion_final_elevacion >= posicion_actual_elevacion()){
//
//             if((posicion_final_elevacion-posicion_actual_elevacion())>=3){
//                 //velocidad rapida elevacion
//             }
//
//             if((posicion_final_elevacion-posicion_actual_elevacion())<3){
//                 //velocidad lenta elevacion
//             }
//
//         }
//         if (posicion_final_elevacion > posicion_actual_elevacion()){
//
//             if((posicion_final_elevacion-posicion_actual_elevacion())>=3){
//                 //velocidad rapida elevacion negativa
//             }
//
//             if((posicion_final_elevacion-posicion_actual_elevacion())<3){
//                 //velocidad lenta elevacion negativa
//             }
//
//         }
//         if((abs(posicion_final_elevacion)-abs(posicion_actual_elevacion()<0.1))){
//             posicion_elevacion_alcanzada=true;
//         }
//         
//         if(( posicion_elevacion_alcanzada=true)&&( posicion_acimut_alcanzada=true)){
//             State=POS_ALCANZADA;
//         }
//         
//    }
//    else {
//        State=POS_FAILED;
//    }
//     
//}

uint8_t Estado_control_automatico(){
    return State;
}