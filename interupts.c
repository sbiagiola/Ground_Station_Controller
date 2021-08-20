/*
Inicializacion de interrupciones  
 * fecha: 24/04/2021
 * proyecto ground stations controlers
 
 */

#include <p33FJ128GP804.h>
#include "interrupts.h"

void enableInterrupts(void){
/* Set CPU IPL to 0, enable level 1-7 interrupts */
/* No restoring of previous CPU IPL state performed here */
    SRbits.IPL = 0;
    return;
}

void disableInterrupts(void){
/* Se deshabilitan todas las interrupciones */
/* Set CPU IPL to 7, disable level 1-7 interrupts */
/* No saving of current CPU IPL setting performed here */
    SRbits.IPL = 7;
}

void initInterrupts(void){
    
    // Luego del reset las interrupciones son por añidamiento.
    
    // Timer 1:
    IEC0bits.T1IE   = 0b1;      // Enable Timer1 interrupt
    IPC0bits.T1IP = 1;          // Set Timer1 interrupt priority to 1

    // UART 1:
//    IEC0bits.U1TXIE = 0b1;      // Enable UART1 TX interrupt
//    IEC0bits.U1RXIE = 0b1;      // Enable UART1 RX interrupt
//    IEC4bits.U1EIE  = 0b1;      // Enable UART1 Error interrupt
//    IPC16bits.U1EIP = 5;        // Set UART1 Error interupt priority to 5
//    IPC2bits.U1RXIP = 4;        // Set UART1 RX interrupt priority to 4
//    IPC3bits.U1TXIP = 3;        // Set UART1 TX interupt priority to 3
    
    // UART 2:
    IEC1bits.U2TXIE = 0b1;      // Enable UART2 TX interrupt
    IEC1bits.U2RXIE = 0b1;      // Enable UART2 RX interrupt
    IEC4bits.U2EIE  = 0b1;      // Enable UART2 Error interrupt
    IPC16bits.U2EIP = 5;        // Set UART2 Error interupt priority to 5
    IPC7bits.U2RXIP = 4;        // Set UART2 RX interrupt priority to 4
    IPC7bits.U2TXIP = 3;        // Set UART2 TX interupt priority to 3
    
    // Change notification pins:
    IEC1bits.CNIE = 1;          // Enable CN interrupts
    IPC4bits.CNIP = 2;          // Set Change Notification pins interupt priority to 2
    
    // Rigth Channel DAC:
//    IEC4bits.DAC1RIE = 1;     // Right Channel DAC Interrupt Enable
//    IPC19bits.DAC1RIP = 1;	// Right Channel DAC Interrupt Priority Set
    
    // ADC1:
//    IEC0bits.AD1IE  = 0b1;    // Enable ADC1 interrupt
//    IPC3bits.AD1IP = 1;       // Set priority ADC1 interrupt priority to 1
}

void disableUART1(void){
    /* De ser necesario ante el sobrepaso de la fifo*/
    IEC0bits.U1TXIE = 0b0;          /* Disable UART1 TX*/
    IEC4bits.U1EIE  = 0b0;          /* Disable UART1 Error interrupt*/
    IEC0bits.U1RXIE = 0b0;          /* Disable UART1 RX*/
}

void disableUART2(void){
    /* De ser necesario ante el sobrepaso de la fifo*/
    IEC1bits.U2TXIE = 0b0;          /* Disable UART2 TX*/
    IEC4bits.U2EIE  = 0b0;          /* Disable UART2 Error interrupt*/
    IEC1bits.U2RXIE = 0b0;          /* Disable UART2 RX*/
}

void disableADC1(void){
    IEC0bits.AD1IE = 0b0;           /* Disable ADC1*/
}

void disableTIMER1(void){
    IEC0bits.T1IE = 0b0;            /* Disable Timer1 interrupt */
}

void disableCN(void){
    IEC1bits.CNIE = 0b0;          /* Disable CN interrupts*/
}
/********************************************************************************/

/* Esta permite modifirar el nombre pero es necesario especificarle el número de la 
   tabla de interrupciones,también pueden agregarse  más opciones, para que pueda vincular 
   la funcion "MyIRQ" con la tabla de interrupciones. En esta declaración se pueden guardar
   valor de variables propias de la ISR
 * 
 
 void __attribute__((interrupt,no_auto_psv,irq(n))) MyIRQ(void){}
 
 Con esta de aca abajo se define de una manera más sencilla y pero se debe usar 
   el nombre que figura en la tabla de vector de interrupciones

    void _ISR _T1Interrupt(void){}

*/

/* Hay una instruccion en ASSEMBLER que desactiva todas las interrupciones *** Útil para secciones criticas

An alternative approach is to write directly to the DISICNT register to enable interrupts. The DISICNT register may be modified only after a 
DISI instruction has been issued and if the contents of the DISICNT register are not zero.
 
__builtin_disi(0x3FFF); disable interrupts   <---- Esta función no parece estar implementada en nuestro compilador 
// ... protected C code ... 
DISICNT = 0x0000;  enable interrupts          <----   Este registro existe.
 #include <p33FJ128GP804.h>                   <----   aca
 
 _T1Interrupt
        _U1RXInterrupt      (Done)
        _U1TXInterrupt      (Done)
 _ADC1Interrupt             Puede no utilizarse la del ADC
        _U2RXInterrupt      (Done)
        _U2TXInterrupt      (Done)
        _U1ErrInterrupt     (Done)
        _U2ErrInterrupt     (Done)
*/


