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

/*===================== [Variables Internas (Globales)] =====================*/
Struct_Data_Control Data_Control;
Info_Comandos_Procesados Comando_Procesado;

char Datos_A_Enviados[MAX_SIZE_DATA_SEND];
uint32_t Cant_Carac_A_Enviar;
/*===========================================================================*/

/*===================== [Variables Externas (Globales)] =====================*/
extern _Contador Contador;
extern uint8_t Flag_Parada_Emergencia; 
extern Comando_Almacenado Char_Comando;
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

void MEF_Accionamiento(void){
           
    switch(Comando_Procesado.Actual){
        case Parar_Todo:
            Bajar_Salidas();
        break;

        case Giro_Horario:
            // Hay problema cuando estas en este estado y te mandan otro comando. Ahi quedan las salidas arriba. ver esto
            if(Get_Estado_Temporizadores() != Temporizador_1){
                if(Set_Temporizador(Temporizador_1,5)){
                    Girar_Horario();
                }
            }else{  //Get_Estado_Temporizadores() == Temporizador_2
                Comando_Procesado.Actual = Stop_Acimut;
            }
        break;

        case Giro_Antihorario:
            // Disparo un temporizador que en X mSeg cambie el estado a parar acimut.
            if(Get_Estado_Temporizadores() != Temporizador_1){
                if(Set_Temporizador(Temporizador_1,5)){
                    Girar_Antihorario();
                }
            }else{  //Get_Estado_Temporizadores() == Temporizador_2
                Comando_Procesado.Actual = Stop_Acimut;
            }
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
            if(Get_Estado_Temporizadores() != Temporizador_2){
                if(Set_Temporizador(Temporizador_2,5)){
                    Mov_Arriba();
                }
            }else{  //Get_Estado_Temporizadores() == Temporizador_1
                Comando_Procesado.Actual = Stop_Elevacion;
            }
        break;

        case Abajo:
            // Disparo un temporizador que en X mSeg cambie el estado a parar elevacion.
            if(Get_Estado_Temporizadores() != Temporizador_2){
                if(Set_Temporizador(Temporizador_2,5)){
                    Mov_Abajo();
                }
            }else{  //Get_Estado_Temporizadores() == Temporizador_1
                Comando_Procesado.Actual = Stop_Elevacion;
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

void Actualizar_Objetivos(uint8_t ID_Comando){
    
    if(ID_Comando == Hacia_aaa_grados || ID_Comando == Mayor_Presicion_a_grados){
        Data_Control.Target_Acimut = atof(Char_Comando.Char_Acimut);
    }
    
    if(ID_Comando == Mayor_Presicion_e_grados){
        Data_Control.Target_Elevacion = atof(Char_Comando.Char_Elevacion);
    }
    
    if(ID_Comando == Mayor_Presicion_a_e_grados || ID_Comando == Hacia_aaa_eee_grados){
        Data_Control.Target_Acimut = atof(Char_Comando.Char_Acimut);
        Data_Control.Target_Elevacion = atof(Char_Comando.Char_Elevacion);
    }

}

Estado_MEF_Principal Identificar_Tipo_Comando(uint8_t ID_Comando){
    
    if(ID_Comando == Giro_Horario || ID_Comando == Giro_Antihorario || ID_Comando == Stop_Acimut || 
       ID_Comando == Arriba || ID_Comando == Abajo || ID_Comando == Stop_Elevacion){
        return Movimiento_Manual;
    }
    
    if(ID_Comando == Hacia_aaa_grados || ID_Comando == Hacia_aaa_eee_grados || ID_Comando == Mayor_Presicion_a_e_grados ||
       ID_Comando == Mayor_Presicion_a_grados || ID_Comando == Mayor_Presicion_e_grados){
        return Tracking;
    }
    
    if(ID_Comando == Devolver_Valor_A_E || ID_Comando == Devolver_Valor_Acimut || ID_Comando == Devolver_Valor_Elevacion){
        return Enviar_Posicion;
    }
    if(ID_Comando == Velocidad_1_Elevacion || ID_Comando == Velocidad_2_Elevacion || 
       ID_Comando == Velocidad_3_Elevacion || ID_Comando == Velocidad_4_Elevacion){
        return Cambio_Veloc_Elevacion;
    }

return Stop;  
}

void MEF_Principal(void){
    static Estado_MEF_Principal Estado_Actual_MEF_Principal = Stop;
    static Estado_MEF_Principal Estado_Anterior_MEF_Principal = Stop;
    
    Estado_Anterior_MEF_Principal = Estado_Actual_MEF_Principal;
    Estado_Actual_MEF_Principal = Identificar_Tipo_Comando(Comando_Procesado.Actual);
    
    switch(Estado_Actual_MEF_Principal){
        
        case Stop:
            Bajar_Salidas();
        break;
        
        case Tracking:
            
            if(Estado_Anterior_MEF_Principal == Movimiento_Manual){
                Bajar_Salidas();
            }
            
            if(Flag_Parada_Emergencia){
                Estado_Actual_MEF_Principal = Parar_Todo;
                break;
            }
            
            Actualizar_Objetivos(Comando_Procesado.Actual);
            Calcular_Posicion_Actual(&Contador);
            Control_Posicion_Acimut();
            Control_Posicion_Elevacion();
            
        break;
        
        case Movimiento_Manual:
            
            if(Estado_Anterior_MEF_Principal == Tracking){
                Bajar_Salidas();
            }
            
            if(Flag_Parada_Emergencia){
                Estado_Actual_MEF_Principal = Parar_Todo;
                break;
            }
            
            MEF_Movimiento_Manual();
            
        break;
        
        case Enviar_Posicion:
            Generar_Formato_Mensaje(Datos_A_Enviados,Comando_Procesado.Actual);
            uart_ringBuffer_envDatos_U2((uint8_t*)Datos_A_Enviados,Cant_Carac_A_Enviar);
            Estado_Actual_MEF_Principal = Estado_Anterior_MEF_Principal;
        break;
        
        case Cambio_Veloc_Elevacion:
            if(Comando_Procesado.Actual == Velocidad_1_Elevacion){
                Velocidad_1_El();
            }
            if(Comando_Procesado.Actual == Velocidad_2_Elevacion){
                Velocidad_2_El();
            }
            if(Comando_Procesado.Actual == Velocidad_3_Elevacion){
                Velocidad_3_El();
            }
            if(Comando_Procesado.Actual == Velocidad_4_Elevacion){
                Velocidad_4_El();
            }
            Estado_Actual_MEF_Principal = Estado_Anterior_MEF_Principal;
        break;
        
        default: Estado_Actual_MEF_Principal = Stop;

    }
}

void MEF_Movimiento_Manual(){
    static ID_Comandos Estado_Actual_MEF_Manual ;
    static ID_Comandos Estado_Anterior_MEF_Manual;
    
    Estado_Anterior_MEF_Manual = Estado_Actual_MEF_Manual;
    Estado_Actual_MEF_Manual = Comando_Procesado.Actual;
    
    switch(Estado_Actual_MEF_Manual){
        
        case Giro_Horario:
            // Hay problema cuando estas en este estado y te mandan otro comando. Ahi quedan las salidas arriba. ver esto
            if(Get_Estado_Temporizadores() != Temporizador_1){
                if(Set_Temporizador(Temporizador_1,5)){
                    Girar_Horario();
                }
            }else{  //Get_Estado_Temporizadores() == Temporizador_2
                Estado_Actual_MEF_Manual = Stop_Acimut;
            }
        break;

        case Giro_Antihorario:
            // Disparo un temporizador que en X mSeg cambie el estado a parar acimut.
            if(Get_Estado_Temporizadores() != Temporizador_1){
                if(Set_Temporizador(Temporizador_1,5)){
                    Girar_Antihorario();
                }
            }else{  //Get_Estado_Temporizadores() == Temporizador_2
                Estado_Actual_MEF_Manual = Stop_Acimut;
            }
        break;

        case Stop_Acimut:
            Parar_Acimut();
        break;
        
        case Arriba:
            if(Get_Estado_Temporizadores() != Temporizador_2){
                if(Set_Temporizador(Temporizador_2,5)){
                    Mov_Arriba();
                }
            }else{  //Get_Estado_Temporizadores() == Temporizador_1
                Estado_Actual_MEF_Manual = Stop_Elevacion;
            }
        break;

        case Abajo:
            // Disparo un temporizador que en X mSeg cambie el estado a parar elevacion.
            if(Get_Estado_Temporizadores() != Temporizador_2){
                if(Set_Temporizador(Temporizador_2,5)){
                    Mov_Abajo();
                }
            }else{  //Get_Estado_Temporizadores() == Temporizador_1
                Estado_Actual_MEF_Manual = Stop_Elevacion;
            }
        break;

        case Stop_Elevacion:
            Parar_Elevacion();
        break;
        
        default: Bajar_Salidas();
        
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
