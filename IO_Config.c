/* 
 * File:   IO_Config.c
 * Author: Jere
 *
 * Created on 22 de marzo de 2021, 18:06
 */
#include <xc.h>
#include <stdio.h>

#include "IO_Config.h"
#include "UART.h"

int Config_IO(void){
    //Podemos poner una verificación con los retornos de cada función
    Set_Pin_As_A_or_D();
    Define_IO_Pins();
    Remappeable_Pins();
return(11);
}

int Change_Config_UART1(void){
    
    Disable_UART1();
    
    //Remapeo de pines de UART1 hacia el MAX232
    RPINR18bits.U1RXR = 0b00100;   /***** UART1 Receive connected to RP4  *****/
    RPOR1bits.RP3R =  0b00011;      /***** UART1 Transmit connected to RP3 *****/
    
    Enable_UART1();
return(12);
}

int Remappeable_Pins(void){ 
    
    /********* Input *********/
//   Vienen tiradas a VSS por defecto luego del reset. La configuración debajo la dejo por que ya habia escrito todo esto.
   
//   RPINR0bits.INT1R  = 0b11111;                                    /***** External interrupt 1 *****/
//   RPINR1bits.INT2R  = 0b11111;                                    /***** External interrupt 2 *****/
//   RPINR3bits.T2CKR  = 0b11111;    RPINR3bits.T3CKR = 0b11111;     /***** External clock Timer 2 y 3 *****/
//   RPINR4bits.T4CKR  = 0b11111;    RPINR4bits.T5CKR = 0b11111;     /***** External clock Timer 4 y 5 *****/
//   RPINR7bits.IC1R   = 0b11111;    RPINR7bits.IC2R  = 0b11111;     /***** Input capture 1 y 2 *****/
//   RPINR10bits.IC7R  = 0b11111;    RPINR10bits.IC8R = 0b11111;     /***** Input capture 7 y 8 *****/
//   RPINR11bits.OCFAR = 0b11111;                                    /***** Output Compare A *****/
//   RPINR18bits.U1CTSR = 0b11111;   RPINR18bits.U1RXR = 0b11111 ;   /***** UART1 Clear to Send and Receive  *****/
//   RPINR19bits.U2CTSR = 0b11111;   RPINR19bits.U2RXR = 0b00010;    /***** UART2 Clear to Send and Receive  *****/    
//   RPINR20bits.SCK1R = 0b11111;    RPINR20bits.SDI1R = 0b11111;    /***** SPI1 Clock and Data Input *****/
//   RPINR21bits.SS1R  = 0b11111;                                    /***** SPI1 Slave Select Input *****/
//   RPINR22bits.SCK2R = 0b11111;    RPINR22bits.SDI2R = 0b11111;    /***** SPI2 Clock and Data Input *****/
//   RPINR23bits.SS2R  = 0b11111;                                    /***** SPI2 Slave Select Input *****/                                
//   RPINR24bits.CSCKR = 0b11111;    RPINR24bits.CSDIR = 0b11111;    /***** DCI Serial Clock and Data Input *****/ 
//   RPINR25bits.COFSR = 0b11111;                                    /***** DCI Frame Sync Input *****/ 
//   RPINR26bits.C1RXR = 0b11111;                                    /***** ECAN1 Receive *****/
    
    RPINR18bits.U1RXR = 0b00000;    /***** UART1 Receive connected to RP0  *****/    
    RPINR19bits.U2RXR = 0b00010;    /***** UART2 Receive connected to RP2  *****/ 
    
    /********* Outputs *********/
    
    RPOR0bits.RP1R =  0b00011;      /***** UART1 Transmit connected to RP1 *****/
    RPOR8bits.RP17R = 0b00101;      /***** UART2 Transmit connected to RP17 *****/
return(13);
}

int Define_IO_Pins(void){
    
    //Port A
    TRISAbits.TRISA4 = 0;  // Salida al led. Pin34
    TRISAbits.TRISA7 = 0;  // Salida a motor. Pin13
    TRISAbits.TRISA9 = 1;  // Entrada de encoder. Pin 35.
    TRISAbits.TRISA10 = 0; // Salida a motor. Pin12
    
    //Port B
    TRISBbits.TRISB9 = 1;  // Entrada de encoder. Pin 1.
    TRISBbits.TRISB5 = 1;  // Entrada de encoder. Pin 41.
    TRISBbits.TRISB6 = 1;  // Entrada de encoder. Pin 42.
    TRISBbits.TRISB7 = 1;  // Entrada de encoder. Pin 43.
    TRISBbits.TRISB8 = 1;  // Entrada de encoder. Pin 44.
    TRISBbits.TRISB14 = 0; // Salida a motor. Pin14
    TRISBbits.TRISB15 = 0; // Salida a motor. Pin15
    
    //Port C
    
    TRISCbits.TRISC2 = 1;  // Entrada de parada de emergencia. Pin25
    TRISCbits.TRISC3 = 1;  // Entrada de encoder. Pin 36.
    TRISCbits.TRISC4 = 1;  // Entrada de encoder. Pin 37.
    TRISCbits.TRISC5 = 1;  // Entrada de encoder. Pin 38.
    TRISCbits.TRISC6 = 0;  // Salida a motor. Pin2
    TRISCbits.TRISC7 = 0;  // Salida a motor. Pin3
    TRISCbits.TRISC8 = 0;  // Salida a motor. Pin4
    TRISCbits.TRISC9 = 0;  // Salida a motor. Pin5
    
    //Pines tolerantes de 5V
    //Por seguridad pusimos todos en 5V y no quemar un pin por descuido.
    ODCAbits.ODCA9 = 0b1;   //Entrada de Encoder
    
    ODCBbits.ODCB0 = 0b1;   //Receptor Rx del GPS
    ODCBbits.ODCB2 = 0b1;   //Receptor Rx del USB 
    ODCBbits.ODCB4 = 0b1;   //Receptor Rx del MAX232
    ODCBbits.ODCB5 = 0b1;   //Entrada de Encoder
    ODCBbits.ODCB6 = 0b1;   //Entrada de Encoder
    ODCBbits.ODCB7 = 0b1;   //Entrada de Encoder
    ODCBbits.ODCB8 = 0b1;   //Entrada de Encoder
    ODCBbits.ODCB9 = 0b1;   //Entrada de Encoder
    
    ODCCbits.ODCC0 = 0b1;   //Entrada de analógica AN6
    ODCCbits.ODCC3 = 0b1;   //Entrada de Encoder
    ODCCbits.ODCC4 = 0b1;   //Entrada de Encoder
    ODCCbits.ODCC5 = 0b1;   //Entrada de Encoder
    
return(14);
}

int Set_Pin_As_A_or_D(void){
    AD1PCFGL = 0xFFFF;          // Seteo todas como Digital
    
    AD1PCFGLbits.PCFG0 = 0b0;   // Analog Mode
    AD1PCFGLbits.PCFG1 = 0b1;   // Digital Mode
    AD1PCFGLbits.PCFG2 = 0b1;   // Digital Mode
    AD1PCFGLbits.PCFG3 = 0b1;   // Digital Mode   
    AD1PCFGLbits.PCFG4 = 0b1;   // Digital Mode   
    AD1PCFGLbits.PCFG5 = 0b1;   // Digital Mode
    AD1PCFGLbits.PCFG6 = 0b0;   // Analog Mode
    AD1PCFGLbits.PCFG7 = 0b1;   // Digital Mode
    AD1PCFGLbits.PCFG8 = 0b1;   // Digital Mode
    AD1PCFGLbits.PCFG9 = 0b1;   // Digital Mode
    AD1PCFGLbits.PCFG10 = 0b1;  // Digital Mode
//    AD1PCFGLbits.PCFG11 = ;   //Preguntar sobre configuración de PIN
//    AD1PCFGLbits.PCFG12 = ;   //Preguntar sobre configuración de PIN
            
return(15);
}
