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
typedef struct{
    long Encoder_1_Pulsos;
    long Encoder_1_Vueltas;
    
    long Encoder_2_Pulsos;
    long Encoder_2_Vueltas;
    
    int Anemometr0;
}Contador;

typedef struct{
    uint16_t    Encoder_1_A;
    uint16_t    Encoder_1_B;
    uint16_t    Encoder_1_Z;
    
    uint16_t    Encoder_2_A;
    uint16_t    Encoder_2_B;
    uint16_t    Encoder_2_Z;
    
    uint16_t    Anemometr0;
}Last_Value;
/*==================== [Macros y Definiciones] ========================*/  
#define OUTPUT 0
#define INPUT 1
#define ANALOGIC 0
#define DIGITAL 1
#define HIGH 1
#define LOW 0

Last_Value Valor_Anterior;
Contador  Conteo;
/*========================================================================*/

void Config_IO(void){
    //Podemos poner una verificación con los retornos de cada función
    Set_Pin_As_A_or_D();
    Define_IO_Pins();
    Config_CN_Pins();
    Remappeable_Pins();
}

void Change_Config_UART1(void){
    
    Disable_UART1();
    
    //Remapeo de pines de UART1 hacia el MAX232
    RPINR18bits.U1RXR = 0b00100;   /***** UART1 Receive connected to RP4  *****/
    RPOR1bits.RP3R =  0b00011;      /***** UART1 Transmit connected to RP3 *****/
    
    Enable_UART1();
}

void Remappeable_Pins(void){ 
    
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
}

void Define_IO_Pins(void){
    
    //Port A
    TRISAbits.TRISA4 = OUTPUT;  // Salida al led.           Pin 34.
    TRISAbits.TRISA7 = OUTPUT;  // Salida a motor.          Pin 13.
    TRISAbits.TRISA8 = OUTPUT;  // Salida HMI_S/L           Pin 32.
    TRISAbits.TRISA9 = INPUT;   // Entrada de END/STOP 1.   Pin 35.
    TRISAbits.TRISA10 = OUTPUT; // Salida a motor.          Pin 12.
    
    //Port B
    
    TRISBbits.TRISB5 = INPUT;   // Entrada de END/STOP 2.       Pin 41.
    TRISBbits.TRISB6 = INPUT;   // Entrada de encoder1 FASE A.  Pin 42.
    TRISBbits.TRISB7 = INPUT;   // Entrada de encoder1 FASE B.  Pin 43.
    TRISBbits.TRISB8 = INPUT;   // Entrada de encoder1 FASE Z.  Pin 44.
    TRISBbits.TRISB9 = INPUT;   // Entrada de Anemometro.       Pin 1.
    TRISBbits.TRISB12 = OUTPUT; // Salida DACRP.                Pin 10.
    TRISBbits.TRISB13 = OUTPUT; // Salida DACRN.                Pin 11.
    TRISBbits.TRISB14 = OUTPUT; // Salida a motor.              Pin 14.
    TRISBbits.TRISB15 = OUTPUT; // Salida a motor.              Pin 15.
    
    //Port C
    
    TRISCbits.TRISC2 = INPUT;   // Entrada de parada de emergencia. Pin 27.
    TRISCbits.TRISC3 = INPUT;   // Entrada de encoder2 FASE A.      Pin 36.
    TRISCbits.TRISC4 = INPUT;   // Entrada de encoder2 FASE B.      Pin 37.
    TRISCbits.TRISC5 = INPUT;   // Entrada de encoder2 FASE Z.      Pin 38.
    TRISCbits.TRISC6 = OUTPUT;  // Salida a motor.                  Pin 2.
    TRISCbits.TRISC7 = OUTPUT;  // Salida a motor.                  Pin 3.
    TRISCbits.TRISC8 = OUTPUT;  // Salida a motor.                  Pin 4.
    TRISCbits.TRISC9 = OUTPUT;  // Salida a motor.                  Pin 5.
    
    //Pines tolerantes de 5V
    //Por seguridad pusimos todos en 5V y no quemar un pin por descuido.
    ODCAbits.ODCA9 = 0b1;   //Entrada de Encoder
    
    ODCBbits.ODCB0 = 0b1;   //Receptor Rx del GPS
    ODCBbits.ODCB2 = 0b1;   //Receptor Rx del USB 
    ODCBbits.ODCB4 = 0b1;   //Receptor Rx del MAX232
    ODCBbits.ODCB5 = 0b1;   //Entrada de Encoder    (END/STOP 2)
    ODCBbits.ODCB6 = 0b1;   //Entrada de Encoder    (FASE Z ENCODER 1)
    ODCBbits.ODCB7 = 0b1;   //Entrada de Encoder    (FASE B ENCODER 1)
    ODCBbits.ODCB8 = 0b1;   //Entrada de Encoder    (FASE A ENCODER 1)
    ODCBbits.ODCB9 = 0b1;   //Entrada de Anemometro   
    
  //ODCCbits.ODCC0 = 0b1;   //Entrada de analógica  AN6 (ODC solo nos sirve para I/O digital)
    ODCCbits.ODCC3 = 0b1;   //Entrada de Encoder    (FASE A ENCODER 2)
    ODCCbits.ODCC4 = 0b1;   //Entrada de Encoder    (FASE B ENCODER 2)
    ODCCbits.ODCC5 = 0b1;   //Entrada de Encoder    (FASE Z ENCODER 2)
}

void Set_Pin_As_A_or_D(void){
    AD1PCFGL = 0xFFFF;          // Seteo todas como Digital
    
    AD1PCFGLbits.PCFG0 = ANALOGIC;  
    AD1PCFGLbits.PCFG1 = DIGITAL;   
    AD1PCFGLbits.PCFG2 = DIGITAL;   
    AD1PCFGLbits.PCFG3 = DIGITAL;      
    AD1PCFGLbits.PCFG4 = DIGITAL;      
    AD1PCFGLbits.PCFG5 = DIGITAL;   
    AD1PCFGLbits.PCFG6 = ANALOGIC;  
    AD1PCFGLbits.PCFG7 = DIGITAL;   
    AD1PCFGLbits.PCFG8 = DIGITAL;   
    AD1PCFGLbits.PCFG9 = DIGITAL;   
    AD1PCFGLbits.PCFG10 = DIGITAL;  
    AD1PCFGLbits.PCFG11 = ANALOGIC;  
    AD1PCFGLbits.PCFG12 = ANALOGIC;           
}

void Config_CN_Pins(){
CNEN1bits.CN10IE = 1;   // Enable CN10 pin for interrupt detection  RC2 (PARADA EMERG)
CNEN2bits.CN21IE = 1;   // Enable CN22 pin for interrupt detection  RB9 (ANEMOMETRO)

CNEN2bits.CN22IE = 1;   // Enable CN22 pin for interrupt detection  RB8 (FASE A ENCODER 1)
CNEN2bits.CN23IE = 1;   // Enable CN23 pin for interrupt detection  RB7 (FASE B ENCODER 1)
CNEN2bits.CN24IE = 1;   // Enable CN24 pin for interrupt detection  RB6 (FASE Z ENCODER 1)

CNEN2bits.CN27IE = 1;   // Enable CN27 pin for interrupt detection  RB5 (END/STOP 2)

CNEN2bits.CN25IE = 1;   // Enable CN25 pin for interrupt detection  RC4 (FASE B ENCODER 2)
CNEN2bits.CN26IE = 1;   // Enable CN26 pin for interrupt detection  RC5 (FASE Z ENCODER 2)
CNEN2bits.CN28IE = 1;   // Enable CN28 pin for interrupt detection  RC3 (FASE A ENCODER 2)

IFS1bits.CNIF = 0;      // Reset CN interrupt   (Recomendaban esto)|
}

void __attribute__((interrupt,no_auto_psv)) _CNInterrupt(void){
/*
uint16_t Valor_Act_Enconder_1_A,Valor_Act_Enconder_1_B,Valor_Act_Enconder_1_Z;
uint16_t Valor_Act_Enconder_2_A,Valor_Act_Enconder_2_B,Valor_Act_Enconder_2_Z;
uint16_t Valor_Act_Parada_Emergencia,Valor_Act_Anemometro;
uint16_t Valor_Act_END_STOP_1,Valor_Act_END_STOP_2;

Valor_Act_Enconder_1_A = PORTBbits.RB6;
Valor_Act_Enconder_1_B = PORTBbits.RB7;
Valor_Act_Enconder_1_Z = PORTBbits.RB8;
    
Valor_Act_Enconder_2_A = PORTBbits.RB6;
Valor_Act_Enconder_2_B = PORTBbits.RB7;
Valor_Act_Enconder_2_Z = PORTBbits.RB8;
    
Valor_Act_Parada_Emergencia = PORTCbits.RC2
Valor_Act_Anemometro = PORTBbits.RB9
    
Valor_Act_END_STOP_2 = PORTBbits.RB5;
Valor_Act_END_STOP_1 = PORTAbits.RA4;
*/
    
    // Máxima RPM, según datasheet: 30.000 rpm 
    
    if( (Enconder_1_Fase_A != Valor_Anterior.Encoder_1_A) || (Enconder_1_Fase_B != Valor_Anterior.Encoder_1_B) || Enconder_1_Fase_Z != Valor_Anterior.Encoder_1_Z){
        if(Enconder_1_Fase_A == HIGH && Enconder_1_Fase_B == LOW){
            Valor_Anterior.Encoder_1_A = Enconder_1_Fase_A;
            Conteo.Encoder_1_Pulsos++;
        }
        if(Enconder_1_Fase_B == HIGH && Enconder_1_Fase_A == LOW){
            Valor_Anterior.Encoder_1_B = Enconder_1_Fase_B;
            Conteo.Encoder_1_Pulsos--;
        }
        if(Enconder_1_Fase_Z == HIGH){
            Conteo.Encoder_1_Vueltas++;
            Conteo.Encoder_1_Pulsos = 0;
        }
    } 

    if( (Enconder_2_Fase_A != Valor_Anterior.Encoder_2_A) || (Enconder_2_Fase_B != Valor_Anterior.Encoder_2_B) || Enconder_2_Fase_Z != Valor_Anterior.Encoder_2_Z){
        if(Enconder_2_Fase_A == HIGH && Enconder_2_Fase_B == LOW && Enconder_2_Fase_Z == HIGH){
            Valor_Anterior.Encoder_2_A = Enconder_2_Fase_A;
            Conteo.Encoder_2_Pulsos++;
        }
        if(Enconder_2_Fase_A == LOW){
            Valor_Anterior.Encoder_2_A = Enconder_2_Fase_A;
        }
        if(Enconder_2_Fase_B == HIGH && Enconder_2_Fase_A == LOW && Enconder_2_Fase_Z == HIGH){
            Valor_Anterior.Encoder_2_B = Enconder_2_Fase_B;
            Conteo.Encoder_2_Pulsos--;
        }else{
            Valor_Anterior.Encoder_2_B = Enconder_2_Fase_B;
        }
        if(Enconder_2_Fase_Z == LOW){
            Conteo.Encoder_2_Vueltas++;
            Conteo.Encoder_2_Pulsos = 0;
        }
    }

    if(Anemometro == HIGH){
        Conteo.Anemometr0++;
        //Falta tener el anemometro y definir un número máximo
    }
    
    if(END_STOP_1 == HIGH){
        //Definir acciones
    }
    
    if(END_STOP_2 == HIGH){
        //Definir acciones
    }
    
    if(Parada_Emergencia == HIGH){
        //Definir acciones
    }
    
IFS1bits.CNIF = 0; // Clear CN interrupt
}