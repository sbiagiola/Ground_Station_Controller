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
    
    // Change_Config_UART1();
    extern volatile int Habilitar_Comunicacion; 
    Habilitar_Comunicacion = 1;

    while(1) {
        
    }
    return (EXIT_SUCCESS);
}