/*
 * File:   DAC.c
 * Author: biagi
 *
 * Created on 24 de mayo de 2021, 13:20
 */

#include <xc.h>
#include "stdint.h"
#include "DAC.h"
#include "libpic30.h"

void init_DAC(void){
    DAC1CONbits.DACEN = 1;              /* Enables module*/
    DAC1CONbits.DACSIDL = 0;            /*Continue module operation in Idle mode*/
    DAC1CONbits.AMPON = 1;              /*Analog Output Amplifier is enabled during Sleep Mode/Stop-in Idle mode*/
    DAC1CONbits.FORM = 0;               /* Data Format is Unsigned */
    DAC1CONbits.DACFDIV = 0000101;      /* Divide input clock by 6 (default)*/
    
    DAC1STATbits.LOEN = 0;              /* DAC Left Channel Disable */
    DAC1STATbits.ROEN = 1;              /* DAC Right Channel Enable */
    DAC1STATbits.RMVOEN = 1;            /*Midpoint DAC output is enabled*/
    
    DAC1STATbits.LITYPE = 0;            /*Left Channel Type of Interrupt bit*/ 
    DAC1STATbits.RITYPE = 1;            /*Right Channel Type of Interrupt bit*/
    DAC1STATbits.RFULL  = 1;            /*Status, Right Channel Data Input FIFO is Full bit*/
    
    //IFS4bits.DAC1RIF = 0			/* Right Channel DAC Interrupt Flag Clear */
            
            
    /* Turns off the DAC */ 
            
            
}
void Send_DAC(uint16_t *data){
   DAC1RDAT = *data;
}
