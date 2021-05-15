/*
 * File:   main.c
 * Author: Jere
 *
 * Created on 18 de marzo de 2021, 14:50
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

#include "Clock.h"
#include "libpic30.h"   
#include "IO_Config.h"
#include "UART.h"
#include "ADC.h"
#include "RingBuffer.h"

/*==================== [macros and definitions] ==========================*/
#define CHAR_CR  13     // Retorno del carro
#define CHAR_LF  10     // Fin de linea
#define BUFFER_SIZE 8   // Tamaño del buffer de caracteres a utilizar
/*========================================================================*/

/*======================= [Variables Internas] ===========================*/
unsigned int FlagRec;
uint32_t ByteRec;
static uint8_t IndexRec;
static uint8_t bufferRec[BUFFER_SIZE]; 
/*========================================================================*/

/****** Para volver a mirar o definir: 
            * FALTA DEFINIR EL TAMAÑO DEL RING BUFFER
            * FALTA DEFINIR EL TAMAÑO del mensaje relacionado con azimut o elevación 
 */

int main(){
  
    /* Configuración interna del microcontrolador*/
    Config_Clock();
    Config_IO();
    Config_UART();
    Config_ADC();
    Create_RingBuffer();
    
    /*
    __delay_ms(100);        // Definir el retraso introducido para adquisición de datos del GPS.
    Change_Config_UART1();  // Remapeo de pines de la UART1
    */
    
    while(1) {
        __delay_ms(100);
        //__delay32(FCY);
    }
    return (EXIT_SUCCESS);
}