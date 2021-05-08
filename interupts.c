/*
Inicializacion de interrupciones  
 * fecha: 24/04/2021
 * proyecto ground stations controlers
 
 */

#include <p33FJ128GP804.h>
#include "interrupts.h"

void enableInterrupts(void)
{
/* Set CPU IPL to 0, enable level 1-7 interrupts */
/* No restoring of previous CPU IPL state performed here */
    SRbits.IPL = 0;
    
    
    
    return;
}

void disableInterrupts(void)
{
/* Se deshabilitan todas las interrupciones */
/* Set CPU IPL to 7, disable level 1-7 interrupts */
/* No saving of current CPU IPL setting performed here */
    SRbits.IPL = 7;
    return;
}

void initInterrupts(void)
{
    /* Enable Timer1 interrupt */
    IEC0bits.T1IE = 1;
    
    /* Enable ADC1*/
    IEC0bits.AD1IE = 1;
    
    /* Enable UART1 TX*/
    IEC0bits.U1TXIE= 1;
    
    /* Enable UART1 TX*/
    IEC0bits.U1RXIE= 1;
    
    /* Enable UART2 TX*/
    IEC1bits.U2TXIE = 1;
    
    /* Enable UART2 RX*/
    IEC1bits.U2RXIE = 1;
    
    /* Seteo de prioridades */
    
    /* Set Timer1 interrupt priority to 2, a modificar */
    IPC0bits.T1IP = 2;
    
    /* Set UART1 RX interrupt priority to 3*/
    IPC2bits.U1RXIP = 3;
    
    /* Set UART1 TX interupt priority to 3*/
    IPC3bits.U1TXIP = 3;
    
    /* Set UART2 RX interrupt priority to 3*/
    IPC7bits.U2RXIP = 3;
    
    /* Set UART2 TX interupt priority to 3*/
    IPC7bits.U2TXIP = 3;
    
    /* Set priority ADC1 interrupt priority to 1 */
    IPC3bits.AD1IP = 1;
    
    return;
}

void disableUART1(void)
{
    /* De ser necesario ante el sobrepaso de la fifo*/
    /* Enable UART1 TX*/
    IEC0bits.U1TXIE= 0;
    
    /* Enable UART1 TX*/
    IEC0bits.U1RXIE= 0;
    return;
}

void disableUART2(void)
{
    /* De ser necesario ante el sobrepaso de la fifo*/
    /* Enable UART1 TX*/
    IEC1bits.U2TXIE= 0;
    
    /* Enable UART1 TX*/
    IEC1bits.U2RXIE= 0;
    return;
}

void disableADC1(void)
{
    
    /* Disable ADC1*/
    IEC0bits.AD1IE = 0;
    return;
}

void disableTIMER1(void)
{
    
   /* Disable Timer1 interrupt */
    IEC0bits.T1IE = 0;
    return;
}

/********************************************************************************/

/* Esta permite modifirar el nombre pero es necesario especificarle el número de la 
   tabla de interrupciones,también pueden agregarse  más opciones, para que pueda vincular 
   la funcion "MyIRQ" con la tabla de int
 
 void __attribute__((interrupt(auto_psv,irq(52)))) MyIRQ(void){}
 
*/
/* Con esta de aca abajo se define de una manera más sencilla y pero se debe usar 
e  el nombre que figura en la tabla de vector de interrupciones

    void _ISR _T1Interrupt(void){}

*/

/* Hay una instruccion en ASSEMBLER que desactiva todas las interrupciones *** Útil para secciones criticas

An alternative approach is to write directly to the DISICNT register to enable interrupts. The DISICNT register may be modified only after a 
DISI instruction has been issued and if the contents of the DISICNT register are not zero.
 
__builtin_disi(0x3FFF); disable interrupts   <---- Esta función no parece estar implementada en nuestro compilador 
// ... protected C code ... 
DISICNT = 0x0000;  enable interrupts          <----   Este registro existe.
 #include <p33FJ128GP804.h>                   <----   aca
 */


