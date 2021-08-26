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
/*===================== [Variables Internas (Globales)] =====================*/ 
static uint8_t cont_timer = 0;
static uint64_t _millis;
/*===========================================================================*/
//struct timer timer1[CANT_TIMER];
/*===================== [Variables Externas (Globales)] =====================*/
/*===========================================================================*/
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
    
    // Inicializacion de las struct timer
//    uint8_t i;
//    for(i = 0; i < CANT_TIMER; i++) {
//        timer1[i].enable = 0;
//        timer1[i].tiempoCuenta = 0;
//        timer1[i].timerFlag = 0;
//    }
}


/* ======================================================================== */

//void SetTimer(uint8_t index, int cont)
//{
//    timer1[index].enable = 1;
//    timer1[index].timerFlag = 0;
//    timer1[index].tiempoCuenta = cont * 125; // Cada 125 veces que salta _T1Interrupt se cumple un 1 ms 
//}
//
//
//int GetTimer(uint8_t index)
//{
//    if (timer1[index].timerFlag == 0)
//    {
//        return 0;
//    } else {
//        timer1[index].timerFlag = 0;
//        timer1[index].enable = 0;
//        return 1;
//    }
//    return 0;
//}

uint64_t millis() {
    return _millis;
}

/* Timer1 ISR 
    Interrup salta con una frecuencia de 5Mhz 
*/


void __attribute__((interrupt,no_auto_psv)) _T1Interrupt(void)
{
//    uint8_t i;
//    for(i = 0; i < CANT_TIMER; i++) {
//        if(timer1[i].enable == 1)
//        {
//            if(timer1[i].tiempoCuenta > 0) {
//                timer1[i].tiempoCuenta--;
//            } else {
//                timer1[i].timerFlag = 1;
//            }
//        }
//    }
    if(cont_timer == 125) {
        cont_timer = 0;
        _millis++;
    }
    cont_timer++;
    
    IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag     
}

