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
#include "RingBuffer.h"

/*===================== [Variables Internas (Globales)] =====================*/
Data_Control Control;
Info_Comandos_Procesados Comando_Procesado;
uint8_t Datos_Enviados[MAX_SIZE_DATA_SEND];
/*===========================================================================*/

/*===================== [Variables Externas (Globales)] =====================*/

/*===========================================================================*/

void Generar_Formato_Mensaje(uint8_t* data,uint8_t Id_Comando){
    /*fprint del código pero necesitamos primero el valor actual en función
     de los pulsos detectados por el momento*/
    /* Generar un codigo para cada caso de comando*/
}
void Calculando_Posicion(void){
    extern _Contador Contador;
/*Con la cantidad de pulsos y eso convertimso a un valor angular*/
    Obtener_Pulsos(Contador);
}

void MEF_Accionamiento(void){
           
    switch(Comando_Procesado.Comando_Actual){
        case Parar_Todo:
            LI1_Variador = LOW;     
            LI2_Variador = LOW;
            LI3_Variador = LOW;     
            LI4_Variador = LOW;

            OUT_RELE_1 = LOW;       
            OUT_RELE_2 = LOW;
            OUT_RELE_3 = LOW;       
            OUT_RELE_4 = LOW;
        break;

        case Giro_Horario:
            /* Hay que ver cual salida a relé genera el giro horario y bajar los que no */
            /*
            OUT_RELE_1 = HIGH;       
            OUT_RELE_2 = HIGH;
            OUT_RELE_3 = HIGH;       
            OUT_RELE_4 = HIGH;
             */
        break;

        case Giro_Antihorario:
            /* Hay que ver cual salida a relé genera el giro antihorario y bajar los que no */
            /*
            OUT_RELE_1 = HIGH;       
            OUT_RELE_2 = HIGH;
            OUT_RELE_3 = HIGH;       
            OUT_RELE_4 = HIGH;
             */
        break;

        case Stop_Acimut:
            OUT_RELE_1 = LOW;       
            OUT_RELE_2 = LOW;
            OUT_RELE_3 = LOW;       
            OUT_RELE_4 = LOW;
        break;

        case Devolver_Valor_Acimut:
            Generar_Formato_Mensaje(Datos_Enviados,Devolver_Valor_Acimut);
            uart_ringBuffer_envDatos_U1(Datos_Enviados,sizeof(Datos_Enviados));
        break;

        case Hacia_aaa_grados:
            
        break;

        case Arriba: 
            /*Analizar cual de toda*/
            
            /*Sentido uno, arriba o abajo  */
            LI1_Variador = HIGH;     
            LI2_Variador = LOW;

            /*Sentido dos, arriba o abajo */ 
            LI1_Variador = LOW;     
            LI2_Variador = HIGH;
        break;

        case Abajo:
            /*Analizar cual de toda*/
            
            /*Sentido uno, arriba o abajo  */
            LI1_Variador = HIGH;     
            LI2_Variador = LOW;
            
            /*Sentido dos, arriba o abajo */ 
            LI1_Variador = LOW;     
            LI2_Variador = HIGH;
        break;

        case Stop_Elevacion:
            /*Bajando solo la terminal de sentido se corta el movimiento pero bajamos todo 
             para evitar problemas en el próximo accionamiento*/
            LI1_Variador = LOW;     
            LI2_Variador = LOW;
            LI3_Variador = LOW;     
            LI4_Variador = LOW;
        break;

        case Devolver_Valor_Elevacion:
            Generar_Formato_Mensaje(Datos_Enviados,Devolver_Valor_Elevacion);
            uart_ringBuffer_envDatos_U1(Datos_Enviados,sizeof(Datos_Enviados));
        break;

        case Velocidad_1_Elevacion:
            /*Analizar cual de toda*/
            
            /*Sentido uno, arriba o abajo  */
            LI1_Variador = HIGH;     
            LI2_Variador = LOW;
            
            /*Sentido dos, arriba o abajo */ 
            LI1_Variador = LOW;     
            LI2_Variador = HIGH;
        break;

        case Velocidad_2_Elevacion:
            LI4_Variador = LOW;
            LI3_Variador = HIGH;
        break;

        case Velocidad_3_Elevacion:   
            LI3_Variador = LOW;
            LI4_Variador = HIGH;
        break;

        case Velocidad_4_Elevacion:    
            LI3_Variador = HIGH;
        break;

        case Hacia_aaa_eee_grados:
            
        break;

        case Devolver_Valor_A_E:
            Generar_Formato_Mensaje(Datos_Enviados,Devolver_Valor_A_E);
            uart_ringBuffer_envDatos_U1(Datos_Enviados,sizeof(Datos_Enviados));
        break;

        case Mayor_Presicion_a_grados:
            
        break;

        case Mayor_Presicione_e_grados:
            
        break;

        case Mayor_Presicion_a_e_grados:

        break;
        
        default: Comando_Procesado.Comando_Actual = Parar_Todo;
        }
 }
 