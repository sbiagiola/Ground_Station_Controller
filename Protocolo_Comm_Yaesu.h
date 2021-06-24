/* 
 * File:   Protocol_Comm_Yaesu.h
 * Author: Jere
 *
 * Created on 24 de mayo de 2021, 10:35
 */
#include "stdint.h"

#ifndef PROTOCOL_COMM_YAESU_H
#define	PROTOCOL_COMM_YAESU_H

#ifdef	__cplusplus
extern "C" {
#endif
    
/*==================== [Macros de Comunicaciones] ========================*/
#define START_OF_HEADING 1      // Comienzo de encabezado
#define START_OF_TEXT 2         // Comienzo de transmisión de un texto
#define END_OF_TEXT 3           // Fin de transmisión de un texto
#define END_OF_TRANSMISION 4    // Fin de la transmisión
#define ACKNOWLEDGE  6          // Dato recibido correctamente
#define BELL 7                  // Campana de llamado de atención 
#define CHAR_LF  10             // Fin de linea  
#define CHAR_CR  13             // Retorno del carro
#define NEGATIVE_ACKNOWLEDGE 21 // Dato recibido incorrectamente

#define MAX_SIZE_COMMAND_AVALIBLE 13    // Máximo dado por PC344.1 133.1'CR'
#define MAX_SIZE_DATA_SEND  16          // 'LF'+0344.1+0133.1'CR'
#define MAX_LONG_DATA_ANGLE 6           // Máxima longitud de datos asociada al ángulo
/*========================================================================*/
    
/*===================== [Macros y Definiciones] ==========================*/
typedef struct{
    char Ultimo_Comando_Almacenado[MAX_SIZE_COMMAND_AVALIBLE];
    char Char_Acimut[MAX_LONG_DATA_ANGLE];     //123.4\0
    char Char_Elevacion[MAX_LONG_DATA_ANGLE];  //160.8\0
}Comando_Almacenado;

typedef enum {
    Estableciendo_Conexion = 0, 
    Esperando_Datos,
    Recopilando_Datos,
    Validando_Comando,
    Comando_No_Reconocido,
}Estado_Comunicacion;

typedef enum{
Comando_No_Valido = 0,
                         /*============ Acimut ============*/
Giro_Horario,               // Clockwise Rotation
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
/*===========================================================================*/

/*============================ [Funciones] ==================================*/
void Comm_PC_Interface(void);
uint8_t Verificando_Comando(void);
/*===========================================================================*/
 
#ifdef	__cplusplus
}
#endif

#endif	/* PROTOCOL_COMM_YAESU_H */

