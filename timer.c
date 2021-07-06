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
/*===================== [Variables Internas (Globales)] =====================*/ 
static uint16_t count_seg = 0;
static uint16_t count_timer = 0;
/*===========================================================================*/

/*===================== [Variables Externas (Globales)] =====================*/
static uint8_t Event_1ms;
/*===========================================================================*/
void init_timer1(){
    T1CONbits.TON = 0;// Disable Timer
    T1CONbits.TCS = 0;// Select internal instruction cycle clock 
    T1CONbits.TGATE = 0;// Disable Gated Timer mode
    T1CONbits.TCKPS = 0b10;// Select 1:1 Prescaler. puede ser modificado en 8-64-128
    TMR1 = 0x00; // Clear timer register
    PR1 = 8; // Load the period value. para que quede mas simple
    
    //IFS0bits.T1IF = 0;// Clear Timer1 Interrupt Flag
    T1CONbits.TON = 1;// Start Timer
    
}

/*
 * accedo al valor de contador que se incrementa cada 1ms
 */
uint16_t temporizador()
{
    return count_seg;           
}

void Function_Events_1ms(){

}

int Temporizador_X_ms(uint16_t Cant_ms){
    if(Cant_ms == 0){
        return 1;
    }
    if(Event_1ms){
        Cant_ms--;
    }
return 0;
}

/* Timer1 ISR 
    Interrup salta con una frecuencia de 5Mhz 
*/
void __attribute__((interrupt,no_auto_psv)) _T1Interrupt(void){
    
    if(count_timer==625){ // cumple 1ms y aumenta count_seg
    count_timer=0;
    count_seg++;
        
        if(count_seg==65000)
        {
            count_seg=0;
        }
    Event_1ms = 1;
    }
    
Event_1ms = 0;
count_timer++;
   
IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag 
}

