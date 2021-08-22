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
//static uint16_t count_seg = 0;
//static uint16_t count_timer = 0;
//
//static uint8_t Contador_Tiempo_ms_1 = 0;
//static uint8_t Contador_Tiempo_ms_2 = 0;
//static uint8_t Contador_Tiempo_ms_3 = 0;
//
//static uint8_t Temporizador_1_Habilitado = 0;
//static uint8_t Temporizador_2_Habilitado = 0;
//static uint8_t Temporizador_3_Habilitado = 0;
//
//static uint8_t Temporizador_1_Estado_Conteo = 0;
//static uint8_t Temporizador_2_Estado_Conteo = 0;
//static uint8_t Temporizador_3_Estado_Conteo = 0;
/*===========================================================================*/
struct timer timer1[CANT_TIMER];
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
    uint8_t i;
    for(i = 0; i < CANT_TIMER; i++) {
        timer1[i].enable = 0;
        timer1[i].tiempoCuenta = 0;
        timer1[i].timerFlag = 0;
    }
}


/* ======================================================================== */

void SetTimer(uint8_t index, int cont)
{
    timer1[index].enable = 1;
    timer1[index].timerFlag = 0;
    timer1[index].tiempoCuenta = cont * 125; // Cada 125 veces que salta _T1Interrupt se cumple un 1 ms 
}


int GetTimer(uint8_t index)
{
    if (timer1[index].timerFlag == 0)
    {
        return 0;
    } else {
        timer1[index].timerFlag = 0;
        timer1[index].enable = 0;
        return 1;
    }
    return 0;
}

/* Timer1 ISR 
    Interrup salta con una frecuencia de 5Mhz 
*/

// [TO DO]  fijarse si esta bien calibrado o si pierde mucho tiempo en los if (probar con 5 timers)
//          en ese caso se puede sacar el timerflag y comprobar si es igual a 0 el tiempoCuenta

void __attribute__((interrupt,no_auto_psv)) _T1Interrupt(void)
{
    uint8_t i;
    for(i = 0; i < CANT_TIMER; i++) {
        if(timer1[i].enable == 1)
        {
            if(timer1[i].tiempoCuenta > 0) {
                timer1[i].tiempoCuenta--;
            } else {
                timer1[i].timerFlag = 1;
            }
        }
    }
    
    IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag
//    
//    
//    if(count_timer == 125){ // cumple 1ms y aumenta count_seg
//        
//        count_timer = 0;
//        count_seg++;
        
//        if(count_seg == 1000)
//        {
//            LATAbits.LATA4 = !PORTAbits.RA4;
//            count_seg=0;
//        }
//        Function_Events_ms();
//        if(tiempoCuenta > 0) {
//            tiempoCuenta--;
//        } else {
//            timerFlag = 1;
//        }        
//    }
//    count_timer++;
     
}

