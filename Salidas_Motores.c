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
#include "timer1.h"

/*===================== [Variables Internas (Globales)] =====================*/
Struct_Data_Control Data_Control;
Info_Comandos_Procesados Comando_Procesado;

char Datos_A_Enviados[MAX_SIZE_DATA_SEND];
uint32_t Cant_Carac_A_Enviar;
/*===========================================================================*/

/*===================== [Variables Externas (Globales)] =====================*/
extern _Contador Contador;
/*===========================================================================*/

void Generar_Formato_Mensaje(char* Data_A_Enviar,uint8_t Id_Comando){

    Calcular_Posicion_Actual(&Contador);
    
    switch(Id_Comando){
        
        case Devolver_Valor_Acimut:
            Cant_Carac_A_Enviar = sprintf(Data_A_Enviar,"\n+0%.1f\r",Data_Control.Valor_Actual_Acimut);
        break;
        
        case Devolver_Valor_Elevacion:
            Cant_Carac_A_Enviar = sprintf(Data_A_Enviar,"\n+0%.1f\r",Data_Control.Valor_Actual_Elevacion);
        break;
        
        case Devolver_Valor_A_E:
           Cant_Carac_A_Enviar = sprintf(Data_A_Enviar,"\n+0%.1f+0%.1f\r",Data_Control.Valor_Actual_Acimut,Data_Control.Valor_Actual_Elevacion);
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
void Calcular_Posicion_Actual(const _Contador* Data){
    Data_Control.Valor_Actual_Acimut = (360*Data->Encoder_1_Vueltas+Data->Encoder_1_Pulsos)*RESOLUCION_POR_PULSO_ACIMUT;
    Data_Control.Valor_Actual_Elevacion = (360*Data->Encoder_2_Vueltas+Data->Encoder_2_Pulsos)*RESOLUCION_POR_PULSO_ELEVACION;
}

void Parar(void){
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

void MEF_Accionamiento(void){
           
    switch(Comando_Procesado.Actual){
        case Parar_Todo:
            Parar();
        break;

        case Giro_Horario:
            // Disparo un temporizador que en X mSeg cambie el estado a parar acimut.
            
            // Hay que ver cual salida a relé genera el giro horario y bajar los que no 
            //OUT_RELE_1 = HIGH;       
            //OUT_RELE_2 = HIGH;
            //OUT_RELE_3 = HIGH;       
            //OUT_RELE_4 = HIGH;
        break;

        case Giro_Antihorario:
            // Disparo un temporizador que en X mSeg cambie el estado a parar acimut.
            
            // Hay que ver cual salida a relé genera el giro antihorario y bajar los que no 
            //OUT_RELE_1 = HIGH;       
            //OUT_RELE_2 = HIGH;
            //OUT_RELE_3 = HIGH;       
            //OUT_RELE_4 = HIGH;
        break;

        case Stop_Acimut:
            Parar_Acimut();
        break;

        case Devolver_Valor_Acimut:
            Generar_Formato_Mensaje(Datos_A_Enviados,Devolver_Valor_Acimut);
            uart_ringBuffer_envDatos_U2((uint8_t*)Datos_A_Enviados,Cant_Carac_A_Enviar);
            Comando_Procesado.Actual = Comando_Procesado.Ultimo;
        break;

        case Hacia_aaa_grados:
             Calcular_Posicion_Actual(&Contador);
             Control_Posicion_Acimut();
        break;

        case Arriba:
            // Disparo un temporizador que en X mSeg cambie el estado a parar elevacion.
            Set_Temporizador(Temporizador_1,5);
            
        break;

        case Abajo:
            // Disparo un temporizador que en X mSeg cambie el estado a parar elevacion.
            Set_Temporizador(Temporizador_1,5);
            Mov_Abajo();
            if(Temporizar_X_ms() == Temporizador_1){
                Parar_Elevacion();
            }
            
        break;

        case Stop_Elevacion:
            Parar_Elevacion();
        break;

        case Devolver_Valor_Elevacion:
            Generar_Formato_Mensaje(Datos_A_Enviados,Devolver_Valor_Elevacion);
            uart_ringBuffer_envDatos_U2((uint8_t*)Datos_A_Enviados,Cant_Carac_A_Enviar);
            Comando_Procesado.Actual = Comando_Procesado.Ultimo;
        break;

        case Velocidad_1_Elevacion:
            Velocidad_1_El();
            Comando_Procesado.Actual = Comando_Procesado.Ultimo;
        break;

        case Velocidad_2_Elevacion:
            Velocidad_2_El();
            Comando_Procesado.Actual = Comando_Procesado.Ultimo;
        break;

        case Velocidad_3_Elevacion:   
            Velocidad_3_El();
            Comando_Procesado.Actual = Comando_Procesado.Ultimo;
        break;

        case Velocidad_4_Elevacion:    
            Velocidad_4_El();
            Comando_Procesado.Actual = Comando_Procesado.Ultimo;
        break;

        case Hacia_aaa_eee_grados:
            Calcular_Posicion_Actual(&Contador);
            Control_Posicion_Acimut();
            Control_Posicion_Elevacion();
        break;

        case Devolver_Valor_A_E:
            Generar_Formato_Mensaje(Datos_A_Enviados,Devolver_Valor_A_E);
            uart_ringBuffer_envDatos_U2((uint8_t*)Datos_A_Enviados,Cant_Carac_A_Enviar);
            Comando_Procesado.Actual = Comando_Procesado.Ultimo;
        break;

        case Mayor_Presicion_a_grados:
            Calcular_Posicion_Actual(&Contador);
            Control_Posicion_Acimut();
        break;

        case Mayor_Presicion_e_grados:
            Calcular_Posicion_Actual(&Contador);
            Control_Posicion_Elevacion();
        break;

        case Mayor_Presicion_a_e_grados:
            Calcular_Posicion_Actual(&Contador);
            Control_Posicion_Acimut();
            Control_Posicion_Elevacion();
        break;
        
        default: Comando_Procesado.Actual = Parar_Todo;
        }
 }

void Ejecutar_Movimiento_Manual(uint8_t ID_Comando){
    switch(ID_Comando){
        case Giro_Horario:

        break;

        case Giro_Antihorario:
             
        break;
        
        case Arriba: 

        break;

        case Abajo:

        break;
    }
}

void Seteo_Velocidad_Elev(uint8_t Set){
    switch(Set){
        case Velocidad_1_Elevacion:
            Velocidad_1_El();
        break;

        case Velocidad_2_Elevacion:
            Velocidad_2_El();
        break;

        case Velocidad_3_Elevacion:
            Velocidad_3_El();
        break;

        case Velocidad_4_Elevacion:
            Velocidad_4_El();
        break;   
    }
}

void Accionamiento(uint8_t ID_Comando, uint8_t Tipo_Comando){
    
    if(Tipo_Comando == Stop_A){
        Parar_Acimut();
    }
    
    if(Tipo_Comando == Stop_E){
        Parar_Elevacion();
    }
    
    if(Tipo_Comando == Stop){
        Parar();
    }
    
    if(Tipo_Comando == Movimiento_Manual){
        Ejecutar_Movimiento_Manual(ID_Comando);
    }
    
    if(Tipo_Comando == Envio_Datos){
        Generar_Formato_Mensaje(Datos_A_Enviados,ID_Comando);
        uart_ringBuffer_envDatos_U2((uint8_t*)Datos_A_Enviados,Cant_Carac_A_Enviar);
    }
    
    if(Tipo_Comando == Seguimiento){
        // Queda por definir
    }
    
    if(Tipo_Comando == Cambio_Veloc_Elev){
        Seteo_Velocidad_Elev(ID_Comando);
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
