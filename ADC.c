#include <xc.h>
//#include <p33FJ64GP804.h>
#include "ADC.h"
#include "libpic30.h"

// 2.44 mV el ADC cuenta + (o -) 1 con una resolución de 10 bits 
// Resolución: (2.5 V- 0V) / 2^10 Bit (Ver página 76 dela nota de ADC)

void Config_ADC(void){
              
    AD1CON2bits.VCFG = 0b001;       // Voltajes utilizados: VREF+ y AVSS(DGND)
    
    AD1CON3bits.ADCS = 0b00100000;  // Clock del ADC Tad. Tad = ADCS<7:0> * Tcy
    AD1CON3bits.ADRC = 0b0;         // Selección de clock de entrada (System Clock)   
    
    AD1CON2bits.CHPS = 0b00;        // Definición de canales a utiliza. Canal 0 --> Una única entrada.
    AD1CHS0bits.CH0SA = 0b00110;    // Positive input to channel 0 - AN6
    AD1CHS0bits.CH0NA = 0b0;        // Negative inputto channel 0 - VREL (AVSS)
    
    AD1CON1bits.SSRC = 0b111;       // Sample Clock Source --> Contador interno
    AD1CON3bits.SAMC = 0b00001;     // Tiempo de Auto-Sampleo (Tsampleo = SAMC<4:0>*Tad)
    
    //Tiempo de conversión de ADC = 12*Tad (Valor por defecto en 10 bits) + T_Sampleo = 15 Tad.
    //Tconv(total) = 10.4 µSeg
    
    AD1CON1bits.AD12B = 0b0;        // Data result in 10 bits
    AD1CON1bits.FORM = 0b00;        // Formato del dato de salida (Unsigned Int).

    AD1CON1bits.ASAM = 0b1;         // Sample Auto-Start bit (Luego de convertir la muestra anterior)

    AD1CON1bits.ADON = 0b1;         // Modulo de ADC habilitado
}

uint16_t Get_Data(void){
    while(!AD1CON1bits.DONE){
        //No hacemos nada. Tiempo muerto relacionado con Tad (Cuidado)
    }
    return(ADC1BUF0);   //Retiramos el último valor de conversión.
}
