/*
 * File:   main.c
 * Author: Jere
 *
 * Created on 18 de marzo de 2021, 14:50
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "DAC.h"
#include "Clock.h"
#include "libpic30.h"   
#include "IO_Config.h"
#include "UART.h"
#include "ADC.h"
#include "RingBuffer.h"
#include "Protocolo_Comm_Yaesu.h"
#include "interrupts.h"

/*==================== [macros and definitions] ==========================*/
#define CHAR_CR  13     // Retorno del carro
#define CHAR_LF  10     // Fin de linea
#define BUFFER_SIZE 8   // Tamaño del buffer de caracteres a utilizar
/*========================================================================*/

int main(){
    
    Create_RingBuffer();    // Ponerlo antes de habilitar el uso de UART
    
    /*============ Configuración interna del microcontrolador ============*/
    Config_Clock();
    Config_IO();
    Config_UART();
    Config_ADC();
    initInterrupts();
    /*====================================================================*/
    extern volatile uint8_t Micro_Ready; 
    Micro_Ready = 1;
    
    while(1) {

    }
    return (EXIT_SUCCESS);
}