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
 * - Rutina para volver a home despues de PARADA DE EMERGENCIA
 *      - Evaluar si es necesario volver home en algun otro caso
 *          - Luego de un timeout sin comandos
 * - Enviar angulo actual a PC
 * 
 * Logica general:
 * - Que pasa cuando alguien sube un txt que tenga angulos Elev negativos?
 *      - Se toma como archivo invalido? En este caso no seria posible
 *        por ejemplo seguir al sol por mas de un dia.
 *      - Se toma como archivo valido pero no se envian los angulos
 *        negativos? En ese caso que se hace con la posicion de la antena?
 *        Ponele si yo quiero que la antena siga al sol durante 3 dias,
 *        durante la noche se va a tener que quedar quieta en un lugar,
 *        donde?
 * - Agregar comando de fin de tracking? Cuando llegaria este comando se iria
 *   a home
 * - Que hacemos cuando durante el traking pega el home stop?
 *      - Se detiene y se queda ahi? Vuelve a home?
 * ======================================================================== */

//static uint8_t Bandera_Home_Stop_1 = 1;

extern Last_Value Valor_Anterior;
const extern ID_Comandos estado_Accionamiento;
uint8_t elevation_inHome = 0;
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
    initCN();
    Config_UART();
    Config_ADC();
    
    init_timer1();
    initInterrupts();
    /* ======================================================================== */
    // Change_Config_UART1();       // Recordar de remapear los pines de la UART 1
    
    double angulo = 0;
    char char_Angulo[10] = {};
    char char_Millis[15] = {};

    
    
    
    unsigned long millis_ANGULO;
  
    while(1) {
        
        MasterLEDS();
        
        if (millis() - millis_ANGULO > 1000)
        {
            angulo = (get_Acimut()*360.0)/100.0;
            sprintf(char_Millis, "%llu", millis());
            sprintf(char_Angulo, "%.2f", angulo);


            putrsUART2(char_Millis);
            putrsUART2(" - Angulo leido = ");
            putrsUART2(char_Angulo);
            putrsUART2("\n");
            millis_ANGULO = millis();
        }

//        if(elevation_inHome)
            Comm_PC_Interface();
        
        MEF_Accionamiento();
        ClrWdt();
        
    }
    return (EXIT_SUCCESS);
}