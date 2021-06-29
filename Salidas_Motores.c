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

#include "Salidas_Motores.h"
#include "Protocolo_Comm_Yaesu.h"
#include "Entradas.h"
#include "RingBuffer.h"

/*===================== [Variables Internas (Globales)] =====================*/
Data_Control Control;
Info_Comandos_Procesados Comando_Procesado;
char Datos_A_Enviados[MAX_SIZE_DATA_SEND];
/*===========================================================================*/

/*===================== [Variables Externas (Globales)] =====================*/
extern _Contador Contador;
/*===========================================================================*/

void Generar_Formato_Mensaje(char* Data_A_Enviar,uint8_t Id_Comando){

    Calculando_Posicion(Contador);
    
    // Esta parte de abajo genera un 5% de código de programa y 1% de data, capaz
    // hay otra forma más liviana de hacer esto. Un solo sprintf genera 2.000 lineals de código assembler
    switch(Id_Comando){
        
        case Devolver_Valor_Acimut:
            sprintf(Data_A_Enviar,"\n+0%.1f\r",Control.Valor_Actual_Acimut);
        break;
        
        case Devolver_Valor_Elevacion:
            sprintf(Data_A_Enviar,"\n+0%.1f\r",Control.Valor_Actual_Elevacion);
        break;
        
        case Devolver_Valor_A_E:
            sprintf(Data_A_Enviar,"\n+0%.1f+0%.1f\r",Control.Valor_Actual_Acimut,Control.Valor_Actual_Elevacion);
        break;
        
        default: ;
    }
}

/* Esta funcion se encarga de actualizar los valores actuales de los ángulos de acimut y
   elevación en cada llamado de la misma en función de la cantidad de pulsos enviados por cada
   encoder.
 * 
    const _Contador Data (IN) (Solo lectura) -> Cantidad de pulsos y vueltas de cada encoder
 */
void Calculando_Posicion(const _Contador Data){

/*Con la cantidad de pulsos y eso convertimso a un valor angular y actualizar 
  Control.Valor_Actual_Acimut o Control.Valor_Actual_Elevacion*/
    Control.Valor_Actual_Acimut = (360*Data.Encoder_1_Vueltas+Data.Encoder_1_Pulsos)*RESOLUCION_POR_PULSO_ACIMUT;
    Control.Valor_Actual_Elevacion = (360*Data.Encoder_2_Vueltas+Data.Encoder_2_Pulsos)*RESOLUCION_POR_PULSO_ELEVACION;
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
            Generar_Formato_Mensaje(Datos_A_Enviados,Devolver_Valor_Acimut);
            uart_ringBuffer_envDatos_U1((uint8_t*)Datos_A_Enviados,sizeof(Datos_A_Enviados));
        break;

        case Hacia_aaa_grados:
            Calculando_Posicion(Contador);
            //Comparar de alguna manera no con target
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
            LI1_Variador = LOW;     
            LI2_Variador = HIGH;
            
            /*Sentido dos, arriba o abajo */ 
            LI1_Variador = HIGH;     
            LI2_Variador = LOW;
        break;

        case Stop_Elevacion:
            LI1_Variador = LOW;     
            LI2_Variador = LOW;
            LI3_Variador = LOW;     
            LI4_Variador = LOW;
        break;

        case Devolver_Valor_Elevacion:
            Generar_Formato_Mensaje(Datos_A_Enviados,Devolver_Valor_Elevacion);
            uart_ringBuffer_envDatos_U1((uint8_t*)Datos_A_Enviados,sizeof(Datos_A_Enviados));
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
            Calculando_Posicion(Contador);
            //Comparar de alguna manera no con target
        break;

        case Devolver_Valor_A_E:
            Generar_Formato_Mensaje(Datos_A_Enviados,Devolver_Valor_A_E);
            uart_ringBuffer_envDatos_U1((uint8_t*)Datos_A_Enviados,sizeof(Datos_A_Enviados));
        break;

        case Mayor_Presicion_a_grados:
            Calculando_Posicion(Contador);
            //Comparar de alguna manera no con target
        break;

        case Mayor_Presicione_e_grados:
            Calculando_Posicion(Contador);
            //Comparar de alguna manera no con target
        break;

        case Mayor_Presicion_a_e_grados:
            Calculando_Posicion(Contador);
            //Comparar de alguna manera no con target
        break;
        
        default: Comando_Procesado.Comando_Actual = Parar_Todo;
        }
 }
 