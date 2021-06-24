/* 
 * File:   Datos_Motor
 * Author: Jere
 *
 * Created on 24 de junio de 2021, 10:30
 */

#include <xc.h>
#include <string.h>
#include "Salidas_Motores.h"
#include "stdint.h"
#include "Protocolo_Comm_Yaesu.h"
#include "Entradas.h"

#define OFFSET_ANGULAR 0.1
#define REDUCCION_ACIMUT   7/90000
#define PULSOS_POR_VUELTA_ENCODER  360
#define RELACION_POR_VUELTA_ACIMUT  1/REDUCCION_ACIMUT
#define PULSOS_POR_VUELTA_ACIMUT RELACION_POR_VUELTA_ACIMUT*PULSOS_POR_VUELTA_ENCODER

Data_Control Control;
Comandos_Procesados Situacion_Actual;
extern _Contador Contador;
 
void Accionando_Salidas(void){
   
}

void MEF_Accionamiento(void){
           
    switch(Situacion_Actual.Comando_Actual){
        case Parar_Todo:

        break;

        case Giro_Horario:

        break;

        case Giro_Antihorario:

        break;

        case Stop_Acimut:

        break;

        case Devolver_Valor_Acimut:

        break;

        case Hacia_aaa_grados:

        break;

        case Arriba:

        break;

        case Abajo:

        break;

        case Stop_Elevacion:

        break;

        case Devolver_Valor_Elevacion:

        break;

        case Velocidad_1_Elevacion:

        break;

        case Velocidad_2_Elevacion:

        break;

        case Velocidad_3_Elevacion:

        break;

        case Velocidad_4_Elevacion:

        break;

        case Hacia_aaa_eee_grados:

        break;

        case Devolver_Valor_A_E:

        break;

        case Mayor_Presicion_a_grados:

        break;

        case Mayor_Presicione_e_grados:

        break;

        case Mayor_Presicion_a_e_grados:

        break;
        
        default: Situacion_Actual.Comando_Actual = Parar_Todo;
        }
 }
 