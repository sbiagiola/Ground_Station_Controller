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
#include "IO_Accionamiento.h"
#include "timer1.h"

/* ========================================================================
 * ============================   TO DO LIST   ============================
 * ========================================================================
 * Firmware:
 * - Error UART OVERFLOW cuando envio por primera vez comando P
 * - Enviar angulo actual a PC
 * - Probar armar una funcion "tracking" que se pueda usar en ReturnToHome y
 *   Objetivo_Tracking a la que haya que pasarle solo los targets
 *      > Evaluar si hace falta las banderas (depende del rebote de la antena)
 *      > Esto solo esta bueno si no hace falta meterle las banderas
 * ======================================================================== */

const extern ID_Comandos estado_Accionamiento;
unsigned long millis_LED;

void MasterLEDS() {
    int frecLED;
    if(estado_Accionamiento == Objetivo_Tracking) frecLED = 100;
    else frecLED = 500;
    
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
    Stop(ALL);
    initCN();
    Config_UART();
    Config_ADC();
    
    init_timer1();
    initInterrupts();
    /* ======================================================================== */
    // Change_Config_UART1();       // Recordar de remapear los pines de la UART 1
    
    double angulo_Az = 0;
    double angulo_Elev = 0;
    char char_Angulo_Az[10] = {};
    char char_Angulo_Elev[10] = {};
    char char_Millis[15] = {};

    unsigned long millis_ANGULO;
  
    while(1) {
        
        MasterLEDS();
        
        if (millis() - millis_ANGULO > 2000)
        {
            angulo_Az = get_Acimut();
            sprintf(char_Angulo_Az, "%.2f", angulo_Az);
            
            angulo_Elev = get_Elevacion();
            sprintf(char_Angulo_Elev, "%.2f", angulo_Elev);
            
            sprintf(char_Millis, "%llu", millis());
            
            putrsUART2(">>  az = ");
            putrsUART2(char_Angulo_Az);
            putrsUART2("°  |  elev = ");
            putrsUART2(char_Angulo_Elev);
            putrsUART2("°  << (");
            putrsUART2(char_Millis);
            putrsUART2(")\n\r");
            millis_ANGULO = millis();
        }

        Comm_PC_Interface();        
        MEF_Accionamiento();
        ClrWdt();
        
    }
    return (EXIT_SUCCESS);
}