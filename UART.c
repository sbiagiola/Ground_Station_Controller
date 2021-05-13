#include <p33FJ128GP804.h>
#include "UART.h"
#include "Clock.h"
#include "libpic30.h"

void Config_UART(void){

    /*******************        Configuración UART 1        *******************/
    
    //Register U1MODE
    U1MODEbits.UEN = 0b00;          // Solamente utilizamos los pines U1TX U1RX  
    U1MODEbits.BRGH = 0b0;          // Baud Rate Generator genera 16 clock por bit
    U1MODEbits.STSEL = 0b0;         // 1-stop bit
    U1MODEbits.PDSEL = 0b0;         // No Parity, 8-data bits
    
    //Register U1STA
    U1STAbits.UTXISEL0 = 0b1;       // Interrupcion de TX cuando se encuentren vacío ambos registros
    U1STAbits.UTXISEL1 = 0b0;       
    U1STAbits.URXISEL = 0b00;       // Interrupciones cuando hay 1 (o +) datos en el buffer de recepción
      
    // Con este valor de BRG obtengo un Baud Rate de 9615 y un error del 0.125% respecto al buscado de 9600 */
    U1BRG = 259;                    //Valor del Baud Rate Generator Register de la UART1

    /*******************        Configuración UART 2        *******************/
      
    //Register U2MODE
    U2MODEbits.UEN = 0b00;          // Solamente utilizamos los pines U1TX U1RX
    U2MODEbits.BRGH = 0b0;          // Baud Rate Generator genera 16 clock por bit
    U2MODEbits.STSEL = 0b0;         // 1-stop bit
    U2MODEbits.PDSEL = 0b0;         // No Parity, 8-data bits
    
    //Register U2STA
    U2STAbits.UTXISEL0 = 0b1;       // Interrupcion de TX cuando se encuentren vacío ambos registros
    U2STAbits.UTXISEL1 = 0b0;       
    U2STAbits.URXISEL = 0b00;       // Interrupciones cuando hay 1 (o +) datos en el buffer de recepción
    
    U2BRG = U1BRG;                  // Valor del Baud Rate Generator de la UART2
}

/*******************        UART1        *******************/

void Enable_UART1(void){
    U1MODEbits.UARTEN = 0b1;        // Habilito la UART1
    __delay_us(60);
    U1STAbits.UTXEN = 0b1;          // Habilito el transmisor de la UART1
    __delay_us(150);                // Se recomienda este delay luego de habilitar el transmisor UART
}

void Disable_UART1(void){
    // Antes de deshabilitar la UART debemos retirar todos los datos de la FIFO RX y TX
    // sino se pierden todos los caracteres almacenados en ella
    U1STAbits.UTXEN = 0b0;          // Deshabilito el transmisor de la UART1
    __delay_us(200);                // Delay por seguridad
    U1MODEbits.UARTEN = 0b0;        // Deshabilito la UART1
}

unsigned int Tx_Reg_U1_State(void){
    if(U1STAbits.UTXBF == 1){
        // Buffer de transmisión lleno
        return(1);
    }
    else    
        // Al menos una palabra puede escribirse en la pila
    return(0);
}

unsigned int Tx_Shift_Reg_U1_State(void){
    if(U1STAbits.TRMT == 0){
        // Transmitiendo datos y/o también existe datos en cola.
        return(1);
    }
    else    
        // Registro vacio y no hay más datos a enviar
    return(0);
}

unsigned int Rx_Reg_U1_State(void){
    if(U1STAbits.URXDA == 1){
        // Al menos un caracter disponible en el buffer de recepción.
        return(1);
    }
    else
        return(0);
}

unsigned int Rx_Shift_Reg_U1_State(void){
    if(U1STAbits.RIDLE == 1){
        // Registro de recepción vacío (Esperando recepción)
        return(1);
    }
    else
        // Recibiendo datos.
        return(0);
}

void Get_Char_Rx_Reg_U1(char *data){
     *data = U1RXREG;
}

void Send_Char_Tx_Reg_U1(char *data){
    U1TXREG = *data;
}

/*******************        UART2        *******************/

void Enable_UART2(void){
    U2MODEbits.UARTEN = 0b1;        // Habilito la UART2
    __delay_us(60);
    U2STAbits.UTXEN = 0b1;          // Habilito el transmisor de la UART2
    __delay_us(150);                // Se recomienda este delay luego de habilitar el transmisor UART
}

void Disable_UART2(void){
    // Antes de deshabilitar la UART se debe retirar todos los datos de las FIFO's RX y TX
    // sino se pierden todos los caracteres almacenados en ella
    U2STAbits.UTXEN = 0b0;          // Deshabilito el transmisor de la UART1
    __delay_us(200);                // Delay por seguridad
    U2MODEbits.UARTEN = 0b0;        // Deshabilito la UART1
};

unsigned int Tx_Reg_U2_State(void){
    if(U2STAbits.UTXBF == 1){
        //Buffer de transmisión lleno
        return(1);
    }
    else    
    //Al menos un caracter puede escribirse en la pila
    return(0);
}

unsigned int Tx_Shift_Reg_U2_State(void){
    if(U2STAbits.TRMT == 0){
        // Transmitiendo datos y/o también existe datos en cola.
        return(1);
    }
    else    
        // Registro vacio y no hay más datos a enviar
    return(0);
}

unsigned int Rx_Reg_U2_State(void){
    if(U2STAbits.URXDA == 1){
        //Al menos un caracter disponible en el buffer de recepción.
        return(1);
    }
    else
        //Buffer de recepción de la UART vacío
        return(0);
}

unsigned int Rx_Shift_Reg_U2_State(void){
    if(U2STAbits.RIDLE == 1){
        // Registro de recepción vacío (Esperando recepción)
        return(1);
    }
    else
        // Recibiendo datos.
        return(0);
}

void Get_Char_Rx_Reg_U2(char *data){
     *data = U2RXREG;
}

void Send_Char_Tx_Reg_U2(char *data){
    U2TXREG = *data;
}

// Posible testeo de las UARTS conectando transmisor y receptor.
void Loopback_Mode(void){
    
    /*******************        Configuración UART 1        *******************/
    
    //Register U1MODE
    U1MODEbits.UEN = 0b00;          // Solamente utilizamos el pin U1TX (U1RX esta sin utilización).
    U1MODEbits.UARTEN = 0b1;        // Habilito la UART1     
    U1MODEbits.BRGH = 0b0;          // Baud Rate Generator genera 16 clock por bit
    U1MODEbits.STSEL = 0b0;         // 1-stop bit
    U1MODEbits.PDSEL = 0b0;         // No Parity, 8-data bits
    
    //Register U1STA
    //U1STAbits.URXISEL = 0b00;     // Configuración de interrupción del transmisor
    //U1STAbits.URXISEL = 0b00;     // Modo de interrupción por Buffer de recepción
        
    // Con este valor de BRG obtengo un Baud Rate de 9615 y un error del 0.125% respecto al buscado de 9600 */
    U1BRG = 259;    //Valor del Baud Rate Generator Register de la UART1
    
    //Era recomendado ponerlo luego de haber configurado todo
    U1MODEbits.LPBACK = 0b01;       //Loopback Mode habilitado U1TX conectado internamente a U1RX
    
    /*******************        Configuración UART 2        *******************/
      
    //Register U2MODE
    U2MODEbits.UEN = 0b00;          // Solamente utilizamos el pin U1TX (U1RX esta sin utilización).
    U2MODEbits.UARTEN = 0b1;        // Habilito la UART2
    U2MODEbits.BRGH = 0b0;          // Baud Rate Generator genera 16 clock por bit
    U2MODEbits.STSEL = 0b0;         // 1-stop bit
    U2MODEbits.PDSEL = 0b0;         // No Parity, 8-data bits
    
    //Register U2STA
    //U2STAbits.URXISEL = 0b00;     // Configuración de interrupción del transmisor
    //U2STAbits.URXISEL = 0b00;     // Modo de interrupción por Buffer de recepción
    
    U2BRG = U1BRG;    //Valor del Baud Rate Generator de la UART2. Ponemos el mismo en las dos UART.
    
    //Era recomendado ponerlo luego de haber configurado todo
    U2MODEbits.LPBACK = 0b01;       //Loopback Mode habilitado U2TX conectado internamente a U2RX
}

/* Dado que no utilizamos el registro PSV ni tenemos mapeados en dicha memoria ninguna variable
 que nos sea de interes guardar y no perder, no tenemos que utilizar ciclos de clocks de seguridad 
 en escrituras/lecturas asociadas a dicho registro.
 */

void __attribute__((interrupt,no_auto_psv)) _U1TXInterrupt(void){
    IFS0bits.U1TXIF = 0; // Clear TX Interrupt flag
    //Código de interrupción
}

void __attribute__((interrupt,no_auto_psv)) _U1RXInterrupt(void){
    IFS0bits.U1RXIF = 0;    // Clear RX Interrupt flag
    //Código de interrupción
}

void __attribute__((interrupt,no_auto_psv)) _U1ErrInterrupt(void){
    IFS4bits.U1EIF = 0;     // Clear Error Interrupt flag 
    //Código de interrupción
}

void __attribute__((interrupt,no_auto_psv)) _U2TXInterrupt(void){
    IFS1bits.U2TXIF = 0;    // Clear TX Interrupt flag
    //Código de interrupción
}

void __attribute__((interrupt,no_auto_psv)) _U2RXInterrupt(void){
    IFS1bits.U2RXIF = 0;    // Clear RX Interrupt flag
    //Código de interrupción
}

void __attribute__((interrupt,no_auto_psv)) _U2ErrInterrupt(void){
    IFS4bits.U2EIF = 0;     // Clear Error Interrupt flag 
    //Código de interrupción
}

