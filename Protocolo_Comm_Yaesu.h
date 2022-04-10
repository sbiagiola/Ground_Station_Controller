/* 
 * File:   Protocol_Comm_Yaesu.h
 * Author: Jere
 *
 * Created on 24 de mayo de 2021, 10:35
 */
#include "stdint.h"
#include "UART.h"

#ifndef PROTOCOL_COMM_YAESU_H
#define	PROTOCOL_COMM_YAESU_H

#ifdef	__cplusplus
extern "C" {
#endif
    
/*==================== [Macros de Comunicaciones] ========================*/
#define CHAR_LF  10             // Fin de linea  
#define CHAR_CR  13             // Retorno del carro

#define MAX_LONG_DATA_ANGLE 7           // Máxima longitud de datos asociada al ángulo
/*========================================================================*/
    

    
/*===================== [Macros y Definiciones] ==========================*/
typedef struct{
    char Comando_Recibido[MAX_SIZE_COMMAND_AVALIBLE];
    char Char_Acimut[MAX_LONG_DATA_ANGLE];     //123.4\0
    char Char_Elevacion[MAX_LONG_DATA_ANGLE];  //160.8\0
} Comando_Almacenado;

typedef enum {
    Esperando_Datos = 0,
    Recopilando_Datos,
    Validando_Comando,
    Limpiando_Buffer,
    Comando_No_Reconocido
}Estado_Comunicacion;

typedef enum{
    Comando_No_Valido = 0,
        
    // Comandos manuales
    //Acimut
    Giro_Horario,               // Clockwise Rotation
    Giro_Antihorario,           // Counter Clockwise Rotation
    Stop_Acimut,                // CW/CCW Rotation Stop
    // Elevacion
    Giro_Arriba,                // UP Direction Rotation
    Giro_Abajo,                 // DOWN Direction Rotation
    Stop_Elevacion,             // UP/DOWN Direction Rotation Stop

    // Comandos de lectura
    Leer_Posicion,              // Leer posicion actual

    // Comandos de tracking
    Stop_Global,                // Stop Global
    GoToHome_Elevacion,
    GoToHome_Acimut,
    Return_ToHome,
    Objetivo_Tracking,          // Formato de mayor precisión para combinación
            
    Sleep,
}ID_Comandos;


/*===========================================================================*/

/*============================ [Funciones] ==================================*/
void Comm_PC_Interface(void);
uint8_t Verificando_Comando(void);
/*===========================================================================*/
 
#ifdef	__cplusplus
}
#endif

#endif	/* PROTOCOL_COMM_YAESU_H */

