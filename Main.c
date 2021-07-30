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
#include "Entradas.h"
#include "Salidas_Motores.h"

/*===================== [Variables Internas (Estáticas)] =====================*/
static uint8_t Bandera_Home_Stop_1 = 1;
/*===========================================================================*/

/*===================== [Variables Externas (Globales)] =====================*/
extern Last_Value Valor_Anterior;
/*===========================================================================*/

void Chequear_Home_Stop_1(void){
    if(Home_Stop_1 != Valor_Anterior.Home_St0p_1){
        if(Home_Stop_1 == HIGH && Bandera_Home_Stop_1 == 1){
            //Seteo de posicion de reposo de alguna manera
            Bandera_Home_Stop_1 = 0;
        }
        if(Home_Stop_1 == HIGH && Bandera_Home_Stop_1 == 0){
            //Definir acciones
        }
        Home_Stop_1 = Valor_Anterior.Home_St0p_1;
    }
}

int main(){
    
    LATAbits.LATA4 = !PORTAbits.RA4;
    __delay_ms(100);
        
    Create_RingBuffer();    // Ponerlo antes de habilitar el uso de UART
    
    /*============ Configuración interna del microcontrolador ============*/
    Config_Clock();
    Config_IO();
    Config_UART();
    Config_ADC();

    initInterrupts();
    /*====================================================================*/

    // Change_Config_UART1();       // Recordar de remapear los pines de la UART 1
    extern ringBufferData_struct_TEST pRingBufferTx_U1_TEST;
    extern ringBufferData_struct_TEST pRingBufferRx_U1_TEST;
    extern ringBufferData_struct_TEST pRingBufferTx_U2_TEST;
    extern ringBufferData_struct_TEST pRingBufferRx_U2_TEST;
    pRingBufferTx_U1_TEST.pBuf[0]='a';
    pRingBufferRx_U1_TEST.pBuf[0]='b';
    pRingBufferRx_U2_TEST.pBuf[0]='c';
    pRingBufferTx_U2_TEST.pBuf[0]='d';
    while(1) {
        //Comm_PC_Interface();
        //MEF_Accionamiento();
        //MEF_Principal();
        //Chequear_Home_Stop_1();
    }
    return (EXIT_SUCCESS);
}