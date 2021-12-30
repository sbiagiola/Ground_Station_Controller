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
 * >> LIMPIAR EL BUFFER CUANDO TERMINA LA INICIALIZACION
 * - Enviar angulo actual a PC
 *
 * - Cambiar timeout de comando a 2 horas
 * 
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
 * >> AGREGAR COMANDO HOME QUE SERA ENVIADO CUANDO EL PYTHON DETECTE ALGUNOS
 *    ANGULOS NEGATIVOS.
 * 
 * - Agregar comando de fin de tracking? Cuando llegaria este comando se iria
 *   a home
 * >> USAMOS EL COMANDO HOME
 * 
 * - Que hacemos cuando durante el traking pega el home stop?
 *      - Se detiene y se queda ahi? Vuelve a home?
 * >> SE TARA EL ANGULO CORRESPONDIENTE Y SIGUE YENDO AL TARGET
 * 
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
        
        if (millis() - millis_ANGULO > 1000)
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

//        if(elevation_inHome)
            Comm_PC_Interface();
        
        MEF_Accionamiento();
        ClrWdt();
        
    }
    return (EXIT_SUCCESS);
}