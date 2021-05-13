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

int main(){
    Config_Clock();
    Config_IO();
    Config_UART();
    Config_ADC();
    
    __delay_ms(100);        //Definir el retraso introducido para adquisición de datos del GPS.
    Change_Config_UART1();  //Remapeo de pines de la UART1
    
    // Quedan por ver la configuración del DAC y los pines asociados al mismo.
    // Falta la incorporación del timer 1 
    
    while(1) {
        __delay_ms(100);
        //__delay32(FCY);
    }
    return (EXIT_SUCCESS);
}

