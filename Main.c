/*
 * File:   main.c
 * Author: Jere
 *
 * Created on 18 de marzo de 2021, 14:50
 */
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
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

int main(){
        
//    Create_RingBuffer();    // Ponerlo antes de habilitar el uso de UART
    
    /* ============   Configuración interna del microcontrolador   ============ */
    Config_Clock();
    Config_IO();
    Config_UART();
    Config_ADC();
    
    init_timer1();

    // [TO DO] Verificar por que los reles se apagan solos despues de un tiempo
    initInterrupts();
    /* ======================================================================== */
    double angulo = 0;
    char *char_Angulo;
    //char char_Pulsos[MAX_LONG_DATA_ANGLE];
    // Change_Config_UART1();       // Recordar de remapear los pines de la UART 1
    
    unsigned long millis_LED;
    unsigned long millis_ANGULO;
  
    while(1) {
        
        if(millis() - millis_LED > 500)
        {
            LATAbits.LATA4 = !PORTAbits.RA4;
            millis_LED = millis();
        }
        
        if (millis() - millis_ANGULO > 1000)
        {
            angulo = (get_Elevacion()*360.0)/100.0;
            //sprintf(char_Pulsos, "%d", get_Elevacion());
            sprintf(char_Angulo, "%.2f", angulo);
            
            WriteUART2(get_Elevacion());
            //putrsUART2(char_Pulsos);
            putrsUART2(" pulsos - Angulo leido = ");
            putrsUART2(char_Angulo);
            putrsUART2("\n");
            millis_ANGULO = millis();
        }
        
        Comm_PC_Interface();
        MEF_Accionamiento();
        
    }
    return (EXIT_SUCCESS);
}