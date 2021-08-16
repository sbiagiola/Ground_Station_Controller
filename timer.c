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
static uint16_t count_seg = 0;
static uint16_t count_timer = 0;

static uint8_t Contador_Tiempo_ms_1 = 0;
static uint8_t Contador_Tiempo_ms_2 = 0;
static uint8_t Contador_Tiempo_ms_3 = 0;

static uint8_t Temporizador_1_Habilitado = 0;
static uint8_t Temporizador_2_Habilitado = 0;
static uint8_t Temporizador_3_Habilitado = 0;

static uint8_t Temporizador_1_Estado_Conteo = 0;
static uint8_t Temporizador_2_Estado_Conteo = 0;
static uint8_t Temporizador_3_Estado_Conteo = 0;
/*===========================================================================*/

/*===================== [Variables Externas (Globales)] =====================*/

/*===========================================================================*/
void init_timer1(){
    T1CONbits.TON = 0;// Disable Timer
    T1CONbits.TCS = 0;// Select internal instruction cycle clock 
    T1CONbits.TGATE = 0;// Disable Gated Timer mode
    T1CONbits.TCKPS = 0b10;// Select 1:1 Prescaler. puede ser modificado en 8-64-128
    TMR1 = 0x00; // Clear timer register
    PR1 = 4; // Load the period value. para que quede mas simple
    
    //IFS0bits.T1IF = 0;// Clear Timer1 Interrupt Flag
    T1CONbits.TON = 1;// Start Timer
    
}

/*
 * accedo al valor de contador que se incrementa cada 1ms
 */
//uint16_t temporizador(void)
//{
//    return count_seg;           
//}

/*No extender demasiado esta función ya que se ejecuta en la rutina de interrupciones*/
//void Function_Events_ms(){ 
//    Temporizar_X_ms();
//}
/*Set_Temporizador es una función que permite el seteo de hasta 3 temporizadores independientes
entre sí. Una vez asignado un valor al temporizador a utilizar se bloquea y no se permiten actualizaciones
hasta terminar el conteo anterior.
Retorna un valor no nulo si pudo setear el temporizador.
  
 Parametros:
    int Contador_Num   ->   Número de timer a utilizar
    uint16_t Cant_ms   ->   Cantidad de mili-segundos a contar en el mismo
*/
int Set_Temporizador(int Contador_Num,uint16_t Cant_ms){
    
    if(Contador_Num == Temporizador_1 && !Temporizador_1_Habilitado && Temporizador_1_Estado_Conteo == 0){
        Temporizador_1_Habilitado = 1;
        Contador_Tiempo_ms_1 = Cant_ms;
        return 1;
    }
    
    if(Contador_Num == Temporizador_2 && !Temporizador_2_Habilitado && Temporizador_2_Estado_Conteo == 0){
        Temporizador_2_Habilitado = 1;
        Contador_Tiempo_ms_2 = Cant_ms;
        return 1;
    }
    
    if(Contador_Num == Temporizador_3 && !Temporizador_3_Habilitado && Temporizador_3_Estado_Conteo == 0){
        Temporizador_3_Habilitado = 1;
        Contador_Tiempo_ms_3 = Cant_ms;
        return 1;
    }
return 0;
}

/*Temporizador que lleva al cabo el conteo de tiempo. Se ejecutara dentro función Function_Events_ms() 
en la rutina de interrupciones del timer1  cada 1 mSeg, según sea la configuración de este último.
 */
void Temporizar_X_ms(void){
    
    if(Temporizador_1_Habilitado)
    {
        if(Contador_Tiempo_ms_1 == 0)
        {
            Temporizador_1_Habilitado = 0;
            Temporizador_1_Estado_Conteo = Temporizador_1;
        } else {
            Contador_Tiempo_ms_1--;
        }      
    }
    
    if(Temporizador_2_Habilitado)
    {
        if(Contador_Tiempo_ms_2 == 0)
        {
            Temporizador_2_Habilitado = 0;
            Temporizador_2_Estado_Conteo = Temporizador_2;
        } else {
            Contador_Tiempo_ms_2--;
        }      
    }
    
    if(Temporizador_3_Habilitado)
    {
        if(Contador_Tiempo_ms_3 == 0)
        {
            Temporizador_3_Habilitado = 0;
            Temporizador_3_Estado_Conteo = Temporizador_3;
        } else {
            Contador_Tiempo_ms_3--;
        }      
    }
}
/*Retorna un valor correspondiente al temporizador que a alcanzado la condición de 
cuenta o cero si ninguno aún a alcanzado dicha condición.
 */
int Get_Estado_Temporizadores(void){
    
    if(Temporizador_1_Estado_Conteo == Temporizador_1){
        Temporizador_1_Estado_Conteo = 0;
        return Temporizador_1;
    }
    if(Temporizador_2_Estado_Conteo == Temporizador_2){
        Temporizador_2_Estado_Conteo = 0;
        return Temporizador_2;
    }
     if(Temporizador_3_Estado_Conteo == Temporizador_3){
        Temporizador_3_Estado_Conteo = 0;
        return Temporizador_3;
    }
return 0;
}


/*        TEST       */

int tiempoCuenta = 0;
int timerFlag = 0;

void SetTimer(int cont)
{
    tiempoCuenta = cont;
}

int GetTimer()
{
    if (timerFlag == 0)
    {
        return 0;
    } else {
        timerFlag = 0;
        return 1;
    }
}

/* Timer1 ISR 
    Interrup salta con una frecuencia de 5Mhz 
*/
void __attribute__((interrupt,no_auto_psv)) _T1Interrupt(void)
{
    if(count_timer == 125){ // cumple 1ms y aumenta count_seg
        
        count_timer = 0;
//        count_seg++;
        
//        if(count_seg == 1000)
//        {
//            LATAbits.LATA4 = !PORTAbits.RA4;
//            count_seg=0;
//        }
//        Function_Events_ms();
        if(tiempoCuenta > 0) {
            tiempoCuenta--;
        } else {
            timerFlag = 1;
        }        
    }
    count_timer++;
    IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag 
}

