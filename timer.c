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

void init_timer1()
{
    T1CONbits.TON = 0;// Disable Timer
    T1CONbits.TCS = 0;// Select internal instruction cycle clock 
    T1CONbits.TGATE = 0;// Disable Gated Timer mode
    T1CONbits.TCKPS = 0b00;// Select 1:1 Prescaler. puede ser modificado en 8-64-128
    TMR1 = 0x00; // Clear timer register
    PR1 = 8; // Load the period value. para que quede mas simple
    
    //IFS0bits.T1IF = 0;// Clear Timer1 Interrupt Flag
    T1CONbits.TON = 1;// Start Timer
    
}

/* Timer1 ISR */
/*
 * interrup salta con una frecuencia de 5Mhz 
 */
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void){
  
    if(count_timer==5000000){
      count_timer=0;
      count_seg++;
  } 
  count_timer++;
      
  IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag}  
}/* Interrupt Service Routine code goes here */
