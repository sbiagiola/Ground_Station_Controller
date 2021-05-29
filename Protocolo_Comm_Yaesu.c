#include "stdint.h"
#include "UART.h"
#include "RingBuffer.h"
#include "Protocolo_Comm_Yaesu.h"

#define MAX_SIZE_COMMAND_AVALIBLE 16    //  Max dado por PC344.01 -133.01'CR' (Sin contar el 'CR')
#define MAX_SIZE_DATA_SEND  18          //  'CR''LF'+0344.01+0-133.01

/*
 * (CR)0xD -> Retorno de carro
 * (LF)0xA -> Avance de línea 
  
 * Se debe devolver un 'CR' desde el micro a la pc después de cada comando y también un 
  0Ah se envian datos desde el micro

 * Comando no valido causa el retorno de "?>" y se debe borrar el comando recibido.
 
 * Ángulos en grados
 
 * Acimut: Incrementos positivos en sentido antihorario
 
 * Elevación: Incrementos positivos respecto al plano horizontal
 
 * Para finalizar los comandos debemos enviar, después del mismo,  'CR'
 * 
 * Si se envia una mayuscula o minuscula el comando debe ser ejecutado, si es valido.

 * Los ángulos enviados a la interfaz de control deben tener 3 dígitos (relleno a la izquierda con ceros), 
   y los ángulos devueltos, en algunos casos, tener 4 dígitos con un "+0" al principio.
 */

/*======================= [Lista de comandos validos] =======================*/
/*
  Trama (PC -> Interface):  Envio: XXX'CR'             (XXX comando valido)
  Trama (intercafe -> PC):  Respuesta: 'CR'            (Sin pedidos de datos)
                                       'CR''LF'XXX     (XXX datos)
                                       'CR''LF'?>      (Comando no valido)
 * 
 Nota: Los comandos basicos del Yaesu solo soportan 3 digitos por ángulo, en el caso de los
 comando extendidos se enviaran/recibiran más caracteres.
 */
                    /*============ Acimut ============*/
/*
R           // Clockwise Rotation
L           // Counter Clockwise Rotation
A           // CW/CCW Rotation Stop
C           // Retornar el valor de actual del ángulo de de acimut 
Maaa        // Girar "aaa" grados el ángulo de acimut
*/
                    /*============ Elevación ============*/
/*
U           // UP Direction Rotation
D           // DOWN Direction Rotation
E           // UP/DOWN Direction Rotation Stop
B           // Retornar el valor de actual del ángulo de elevación
*/
                /*============ Elevación - Acimut ============*/
/*
C2          // Retornar el valor de actual del ángulo de acimut y de elevación "+0aaa+0eee"
Waaa eee    // Girar "aaa" grados en acimut y "eee" grados en elevación.
S           // Parar todo moviento asociado a cualquier ángulo
 */
                        /*============ Extensiones ============*/
/*
Z1          // Velocidad más lenta de giro de elevación
Z2          // Velocidad media 1 de giro de elevación
Z3          // Velocidad media 2 de giro de elevación
Z4          // Velocidad más rápida de giro de elevación
PA          // Formato de mayor precisión para acimut
PE          // Formato de mayor precisión para elevación
PC          // Formato de mayor precisión para combinación
*/
/*===========================================================================*/

typedef enum {
    Estableciendo_Conexion = 0, 
    Esperando_Datos,
    Recopilando_Datos,
    Error_Analizando_Datos,
}Estado_Comm;

typedef enum{ 
                         /*============ Acimut ============*/
Giro_Horario = 1,               // Clockwise Rotation
Giro_Antihorario,               // Counter Clockwise Rotation
Stop_Acimut,                    // CW/CCW Rotation Stop
Devolver_Valor_Acimut,          // Retornar el valor de actual del ángulo de de acimut 
Hacia_aaa_grados,               // Girar "aaa" grados el ángulo de acimut

                        /*============ Elevación ============*/
Arriba,                         // UP Direction Rotation
Abajo,                          // DOWN Direction Rotation
Stop_Elevacion,                 // UP/DOWN Direction Rotation Stop
Devolver_Valor_Elevacion,       // Retornar el valor de actual del ángulo de elevación
        
                    /*============ Elevación - Acimut ============*/
Devolver_Valor_A_E,             // Retornar el valor de actual del ángulo de acimut y de elevación "+0aaa+0eee"
Hacia_aaa_eee_grados,           // Girar "aaa" grados en acimut y "eee" grados en elevación.
Parar_Todo,                     // Stop Global
        
                        /*============ Extensiones ============*/
Velocidad_1_Elevacion,          // Velocidad más lenta de giro de elevación
Velocidad_2_Elevacion,          // Velocidad media 1 de giro de elevación
Velocidad_3_Elevacion,          // Velocidad media 2 de giro de elevación
Velocidad_4_Elevacion,          // Velocidad más rápida de giro de elevación
Mayor_Presicion_a_grados,       // Formato de mayor precisión para acimut
Mayor_Presicione_e_grados,      // Formato de mayor precisión para elevación
Mayor_Presicion_a_e_grados,     // Formato de mayor precisión para combinación
}Comandos_Habilitados;

typedef struct{
   uint8_t Comando_Actual;
   uint8_t Proximo_Comando;
   uint8_t Comando_Acimut;
   uint8_t Comando_Elevacion;
   uint8_t Velocidad_E;
}Comandos_Procesados;

/*===================== [Variables Internas (Globales)] =====================*/
uint8_t Mensaje_Env[MAX_SIZE_DATA_SEND];
uint8_t Caracter_Rec;
uint8_t Commando_Recibido[MAX_SIZE_COMMAND_AVALIBLE];
uint32_t FlagRec;
uint32_t Indice_Rec = 0;
Comandos_Procesados Situacion_Actual;
volatile uint8_t Micro_Ready;       // Config. del microcontrolador terminada
extern volatile int Error_UART_U2;
/*===========================================================================*/


uint8_t Verificando_Comando(){
    
    if(Commando_Recibido[0] == 'R' || Commando_Recibido[0] == 'r'){return Giro_Horario;}
    if(Commando_Recibido[0] == 'L' || Commando_Recibido[0] == 'l'){return Giro_Antihorario;}
    if(Commando_Recibido[0] == 'A' || Commando_Recibido[0] == 'a'){return Stop_Acimut;}
    if(Commando_Recibido[0] == 'M' || Commando_Recibido[0] == 'm'){return Hacia_aaa_grados;}
    
    if(Commando_Recibido[0] == 'U' || Commando_Recibido[0] == 'u'){return Arriba;}
    if(Commando_Recibido[0] == 'D' || Commando_Recibido[0] == 'd'){return Abajo;}
    if(Commando_Recibido[0] == 'E' || Commando_Recibido[0] == 'e'){return Stop_Elevacion;}
    if(Commando_Recibido[0] == 'B' || Commando_Recibido[0] == 'b'){return Devolver_Valor_Elevacion;}
    
    if(Commando_Recibido[0] == 'C' || Commando_Recibido[0] == 'c'){
        if(Commando_Recibido[1] == '2'){
            return Devolver_Valor_A_E;
        }
        else return Devolver_Valor_Acimut;
    }
    
    if(Commando_Recibido[0] == 'W' || Commando_Recibido[0] == 'w'){return Hacia_aaa_eee_grados;}
    if(Commando_Recibido[0] == 'S' || Commando_Recibido[0] == 's'){return Parar_Todo;}
    
    if(Commando_Recibido[0] == 'Z' || Commando_Recibido[0] == 'z'){
        if(Commando_Recibido[1] == '1'){return Velocidad_1_Elevacion;}
        if(Commando_Recibido[1] == '2'){return Velocidad_2_Elevacion;}
        if(Commando_Recibido[1] == '3'){return Velocidad_3_Elevacion;}
        if(Commando_Recibido[1] == '4'){return Velocidad_4_Elevacion;}
    }
    
    if(Commando_Recibido[0] == 'P' || Commando_Recibido[0] == 'p'){
        if(Commando_Recibido[1] == 'A' || Commando_Recibido[1] == 'a'){return Mayor_Presicion_a_grados;}
        if(Commando_Recibido[1] == 'E' || Commando_Recibido[1] == 'e'){return Mayor_Presicione_e_grados;}
        if(Commando_Recibido[1] == 'C' || Commando_Recibido[1] == 'c'){return Mayor_Presicion_a_e_grados;}
    }

return 0;   // Corresponde a un error, deberiamos retornar "?>"      
}
void Comm_PC_Interface(){
   
    /*====================== Inicialización por segutidad ======================*/
                Situacion_Actual.Comando_Actual = Parar_Todo;
                Situacion_Actual.Comando_Acimut = Stop_Acimut;
                Situacion_Actual.Comando_Elevacion = Stop_Elevacion;
                Situacion_Actual.Velocidad_E = Velocidad_1_Elevacion;
   /*===========================================================================*/
                
    static Estado_Comm Estado_Actual = Estableciendo_Conexion;      
    FlagRec = uart_ringBuffer_recDatos_U2(&Caracter_Rec, sizeof(Caracter_Rec));
    
        switch (Estado_Actual) {
            
            case Estableciendo_Conexion:
                
                if( Micro_Ready ){
                    Mensaje_Env[0] = BELL;
                    Send_Char_Tx_Reg_U2(&Mensaje_Env[0]);  // Notificación de que estamos queriendo establecer comunicación
                }
                
                if ( FlagRec != 0 && Caracter_Rec == ACKNOWLEDGE){   // Esperas un ACK de la respuesta de la PC
                    Estado_Actual = Esperando_Datos;
                }
                /* Se puede poner un timer en la PC para que al cabo de tantos seg tire un error en al comunicacion*/
            break;
                
            case Esperando_Datos:
                
                if( FlagRec != 0 ){
                    Commando_Recibido[Indice_Rec] = Caracter_Rec;
                    Indice_Rec++; 
                }      
                
            break;
            
            case Recopilando_Datos:
                
                if(Error_UART_U2 == 1){ // Dado que solo nos comunicamos con la PC por la UART2
                    Indice_Rec = 0;
                    Clean_RingBufferRx_U2();
                    Estado_Actual = Esperando_Datos;
                    break;
                }
                
                if( (FlagRec != 0) && (Caracter_Rec != CHAR_CR) ){
                    if(Indice_Rec <= MAX_SIZE_COMMAND_AVALIBLE){
                        Commando_Recibido[Indice_Rec] = Caracter_Rec;
                        Indice_Rec++;
                    }
                    else{ 
                        Estado_Actual = Error_Analizando_Datos;
                        break;
                    }
                }
            
                if( (FlagRec != 0) && (Caracter_Rec == CHAR_CR) ){
                    Commando_Recibido[Indice_Rec] = Caracter_Rec;

                    Situacion_Actual.Proximo_Comando = Verificando_Comando();
                    
                    /* Se podría poner que cada cierto tiempo los comandos "manuales se borren, como para que no queden
                       girando, o moviendose indefinidamente según ese comando recibido. Creo que un polling de 10 ms
                       nos ayudaria aca. No debe afectar a los comandos de posicionamiento. */
                    
                    switch(Situacion_Actual.Proximo_Comando){
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
                        
                        case 0:
                            /* Comando no valido o erroneo. */
                        break;
                    }
                }
            break;
            
            default: Estado_Actual = Esperando_Datos;
        }
}

