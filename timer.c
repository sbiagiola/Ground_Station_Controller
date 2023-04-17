/*
 * File:   timer.c
 * Author: biagiola
 *
 * Created on 26 de mayo de 2021, 20:07
 */

#include "xc.h"
#include "stdint.h"
#include "timer1.h"
#include "libpic30.h"
#include "UART.h"
#include "IO_Basic.h"

/*===================== [Variables Internas (Globales)] =====================*/ 
static uint8_t cont_timer = 0;
static uint64_t _millis;
static uint64_t delayTimer1 = 0;
/*===========================================================================*/

/*===================== [Variables Externas (Globales)] =====================*/

void init_timer1(){
    
    // Configuracion de timers
    T1CONbits.TON = 0;// Disable Timer
    T1CONbits.TCS = 0;// Select internal instruction cycle clock 
    T1CONbits.TGATE = 0;// Disable Gated Timer mode
    T1CONbits.TCKPS = 0b10;// Select 1:1 Prescaler. puede ser modificado en 8-64-128
    TMR1 = 0x00; // Clear timer register
    PR1 = 4; // Load the period value. para que quede mas simple
    
    //IFS0bits.T1IF = 0;// Clear Timer1 Interrupt Flag
    T1CONbits.TON = 1;// Start Timer
}


/* ======================================================================== */

uint64_t millis() {
    return _millis;
}

void delayPIC_ms(uint64_t _delay) {
    delayTimer1 = millis();
    while(millis() - delayTimer1 < _delay) {
        read_EncoderElev();
        ClrWdt();
    }
}

/* Timer1 ISR 
    Interrup salta con una frecuencia de 5Mhz 
*/
void __attribute__((interrupt,no_auto_psv)) _T1Interrupt(void)
{
    if(cont_timer == 125) {
        cont_timer = 0;
        _millis++;
    }
    cont_timer++;
    
    IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag     
}

