/* 
 * File:   Protocol_Comm_Yaesu.h
 * Author: Jere
 *
 * Created on 24 de mayo de 2021, 10:35
 */

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
/*========================================================================*/

/*========================== [Funciones] =================================*/
void Comm_PC_Interface(void);
uint8_t Verificando_Comando(void);
/*========================================================================*/
 
#ifdef	__cplusplus
}
#endif

#endif	/* PROTOCOL_COMM_YAESU_H */

