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
#include "Salidas_Motores.h"
#include "Entradas.h"
#include "timer1.h"

//static uint8_t Bandera_Home_Stop_1 = 1;

extern Last_Value Valor_Anterior;
const extern uint8_t tracking_flag;
uint8_t elevation_inHome = 0;


//void Chequear_Home_Stop_1(void){
//    if(Home_Stop_1 != Valor_Anterior.Home_St0p_1){
//        if(Home_Stop_1 == HIGH && Bandera_Home_Stop_1 == 1){
//            //Seteo de posicion de reposo de alguna manera
//            Bandera_Home_Stop_1 = 0;
//        }
//        if(Home_Stop_1 == HIGH && Bandera_Home_Stop_1 == 0){
//            //Definir acciones
//        }
//        Home_Stop_1 = Valor_Anterior.Home_St0p_1;
//    }
//}
int primerinicio = 1;

unsigned long millis_LED;

void MasterLEDS() {
    int frecLED;
    if(tracking_flag) frecLED = 100;
    else frecLED = 500;
    
    if(millis() - millis_LED > frecLED)
    {
        LATAbits.LATA4 = !PORTAbits.RA4;
        millis_LED = millis();
    }
}

int main(){
        
//    Create_RingBuffer();    // Ponerlo antes de habilitar el uso de UART
    
    /* ============   Configuración interna del microcontrolador   ============ */
    Config_Clock();
    Config_IO();
    Config_UART();
    Config_ADC();
    
    init_timer1();

    initInterrupts();
    
    /* ======================================================================== */
    double angulo = 0;
    char char_Angulo[10] = {};
    char char_Millis[15] = {};

    // Change_Config_UART1();       // Recordar de remapear los pines de la UART 1
    
    
    unsigned long millis_ANGULO;
  
    while(1) {
        
        MasterLEDS();
        
        if (millis() - millis_ANGULO > 1000)
        {
            angulo = (get_Elevacion()*360.0)/100.0;
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