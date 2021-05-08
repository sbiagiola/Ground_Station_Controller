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

int main(void) {
    
// CONFIGURACION DEL PLL
PLLFBDbits.PLLDIV = 0b10;       // M = 8  
CLKDIVbits.PLLPRE = 0b0000;     // N1 = 2
CLKDIVbits.PLLPOST = 0b00;      // N2 = 2

     AD1PCFGL = 0xFFFF; // Todos los puertos como digitales
    
    // Registros de pines I/O:
    // - TRISx --> CONFIG pin
    // - PORTx --> READ pin
    // - LATx --> WRITE pin
    
    TRISB = 0x0000;
    while(1) {
        LATB = 0x00FF;
        __delay_ms(500);
        LATB = 0x0000;
        __delay_ms(500);

    }
    return 1;
}