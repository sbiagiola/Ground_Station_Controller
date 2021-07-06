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

static uint8_t Contador_Tiempo_ms_1 = 0;
static uint8_t Contador_Tiempo_ms_2 = 0;
static uint8_t Contador_Tiempo_ms_3 = 0;

uint8_t Temporizador_1_Habilitado = 0;
uint8_t Temporizador_2_Habilitado = 0;
uint8_t Temporizador_3_Habilitado = 0;

/*===========================================================================*/

/*===================== [Variables Externas (Globales)] =====================*/

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
uint16_t temporizador(void)
{
    return count_seg;           
}

/*No extender demasiado esta función ya que se ejecuta en la rutina de interrupciones*/
void Function_Events_1ms(){ 
    Temporizar_X_ms();
}

void Set_Temporizador(int Contador_Num,uint16_t Cant_ms){
    
    if(Contador_Num == Temporizador_1 && !Temporizador_1_Habilitado){
        Temporizador_1_Habilitado = 1;
        Contador_Tiempo_ms_1 = Cant_ms;
    }
    
    if(Contador_Num == Temporizador_2 && !Temporizador_2_Habilitado){
        Temporizador_2_Habilitado = 1;
        Contador_Tiempo_ms_2 = Cant_ms;
    }
    
    if(Contador_Num == Temporizador_3 && !Temporizador_3_Habilitado){
        Temporizador_3_Habilitado = 1;
        Contador_Tiempo_ms_3 = Cant_ms;
    }
}

int Temporizar_X_ms(void){
    
    if(Temporizador_1_Habilitado){
        Contador_Tiempo_ms_1--;
    }
    if(Temporizador_2_Habilitado){
        Contador_Tiempo_ms_2--;
    }
    if(Temporizador_3_Habilitado){
        Contador_Tiempo_ms_3--;
    }
    
    if(Contador_Tiempo_ms_1 == 0){
        Temporizador_1_Habilitado = 0;
        return Temporizador_1;
    }
    if(Contador_Tiempo_ms_2 == 0){
        Temporizador_2_Habilitado = 0;
        return Temporizador_2;
    }
     if(Contador_Tiempo_ms_3 == 0){
        Temporizador_3_Habilitado = 0;
        return Temporizador_3;
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
    Function_Events_1ms();
    }
    
count_timer++;
   
IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag 
}

