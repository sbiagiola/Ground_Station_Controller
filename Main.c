/*
 * File:   main.c
 * Author: Jere
 *
 * Created on 18 de marzo de 2021, 14:50
 */
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "DAC.h"
#include "Clock.h"
#include "libpic30.h"   
#include "IO_Config.h"
#include "UART.h"
#include "ADC.h"
#include "RingBuffer.h"
#include "Protocolo_Comm_Yaesu.h"
#include "interrupts.h"
#include "IO_Basic.h"
#include "IO_Accionamiento.h"
#include "timer1.h"


#define LED_PIN     1
#define LED_ON      0
#define LED_OFF     1
#define LED_DELAY   500


/* ========================================================================
 * ============================   TO DO LIST   ============================
 * ========================================================================
 * Firmware:
 * - Error UART OVERFLOW cuando envio por primera vez comando P
 * ======================================================================== */

const extern ID_Comandos estado_Accionamiento;
unsigned long millis_LED, millisHE;

void MasterLEDS() {
    int frecLED;
    if(estado_Accionamiento == Objetivo_Tracking) frecLED = 100;
    else frecLED = 1000;
    
    if(millis() - millis_LED > frecLED)
    {
        LATAbits.LATA4 = !PORTAbits.RA4;
        millis_LED = millis();
    }
}

int main(){
    
    /* ============   Configuración interna del microcontrolador   ============ */
    Config_Clock();
    Config_IO();
    OUT_VAR_1   = ON;
    OUT_VAR_2   = ON;
    initCN();
    Config_UART();
    Config_ADC();
    
    init_timer1();
    initInterrupts();
    
//    uint16_t reset = RCON;
//    char char_rcon[10] = {};
//    
//    sprintf(char_rcon,"%d\n\r", reset);
//    putrsUART2(char_rcon);
    
    /* ======================================================================== */
  
    while(1) {
        
        MasterLEDS();
        
        Comm_PC_Interface();        
        MEF_Accionamiento();
        
        ClrWdt();
        
    }
    return (EXIT_SUCCESS);
}