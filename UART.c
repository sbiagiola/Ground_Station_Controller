#include <p33FJ128GP804.h>
#include "UART.h"
#include "Clock.h"
#include "libpic30.h"

// VER EL CHEQUEO DE INTERRUPCIONES Y ESAS COSAS. PREGUNTAR A SEBA QUE VIO DE INTERRUPCIONES.

void Config_UART(void){

    /*******************        Configuración UART 1        *******************/
    
    //Register U1MODE
    U1MODEbits.UEN = 0b00;          // Solamente utilizamos los pines U1TX U1RX  
    U1MODEbits.BRGH = 0b0;          // Baud Rate Generator genera 16 clock por bit
    U1MODEbits.STSEL = 0b0;         // 1-stop bit
    U1MODEbits.PDSEL = 0b0;         // No Parity, 8-data bits
    
    //Register U1STA
    //U1STAbits.URXISEL = 0b00;     // Configuración de interrupción del transmisor
    //U1STAbits.URXISEL = 0b00;     // Modo de interrupción por Buffer de recepción
        
    // Con este valor de BRG obtengo un Baud Rate de 9615 y un error del 0.125% respecto al buscado de 9600 */
    U1BRG = 259;    //Valor del Baud Rate Generator Register de la UART1

    /*******************        Configuración UART 2        *******************/
      
    //Register U2MODE
    U2MODEbits.UEN = 0b00;          // Solamente utilizamos los pines U1TX U1RX
    U2MODEbits.BRGH = 0b0;          // Baud Rate Generator genera 16 clock por bit
    U2MODEbits.STSEL = 0b0;         // 1-stop bit
    U2MODEbits.PDSEL = 0b0;         // No Parity, 8-data bits
    
    //Register U2STA
    //U2STAbits.URXISEL = 0b00;     // Configuración de interrupción del transmisor
    //U2STAbits.URXISEL = 0b00;     // Modo de interrupción por Buffer de recepción
    
    U2BRG = U1BRG;    //Valor del Baud Rate Generator de la UART2. Ponemos el mismo en las dos UART.
}

/*******************        UART1        *******************/

void Enable_UART1(void){
    U1MODEbits.UARTEN = 0b1;        // Habilito la UART1
    __delay_us(30);
    U1STAbits.UTXEN = 0b1;          //Habilito el transmisor de la UART1
    __delay_us(150);                //Se recomienda este delay luego de habilitar el transmisor UART
}

unsigned int State_Transmission_Reg_U1(void){
    if(U1STAbits.UTXBF == 1){
        //Buffer lleno
        return(1);
    }
    else    
    //Al menos una palabra puede escribirse en la pila
    return(0);
}

unsigned int State_Shift_Register_U1(void){
    if(U1STAbits.TRMT == 0){
        //Transmitiendo datos.
        return(1);
    }
    else    
    //Registro vacio
    return(0);
}

unsigned int Char_Avalible_U1(void){
    if(U1STAbits.URXDA == 1)        
    {
        //Al menos un caracter disponible en el registro de datos de recepción.
        return(1);
    }
    else
        return(0);
}

void Get_Char_U1(char *data){
     *data = U1RXREG;
}

void Send_Char_U1(char *data){
    U1RXREG = *data;
}

void Disable_UART1(void){
    U1STAbits.UTXEN = 0b0;          // Deshabilito el transmisor de la UART1
    __delay_us(60);                 // Delay por seguridad
    U1MODEbits.UARTEN = 0b0;        // Deshabilito la UART1
}
/*******************        UART2        *******************/

void Enable_UART2(void){
    U2MODEbits.UARTEN = 0b1;        // Habilito la UART2
    __delay_us(30);
    U2STAbits.UTXEN = 0b1;          //Habilito el transmisor de la UART2
    __delay_us(150);                //Se recomienda este delay luego de habilitar el transmisor UART
}

unsigned int State_Transmission_Reg_U2(void){
    if(U2STAbits.UTXBF == 1){
        //Buffer lleno
        return(1);
    }
    else    
    //Al menos una palabra puede escribirse en la pila
    return(0);
}

unsigned int State_Shift_Register_U2(void){
    if(U2STAbits.TRMT == 0){
        //Transmitiendo datos.
        return(1);
    }
    else    
    //Registro vacio
    return(0);
}

unsigned int Char_Avalible_U2(void){
    if(U2STAbits.URXDA == 1)        //Al menos un caracter en el buffer de datos esta disponible
    {
        //Al menos un dato disponible en el registro de datos de recepción.
        return(1);
    }
    else
        return(0);
}

void Get_Data_U2(char *data){
     *data = U2RXREG;
}

void Send_Char_U2(char *data){
    U2TXREG = *data;
}

void Disable_UART2(void){
    U2STAbits.UTXEN = 0b0;          // Deshabilito el transmisor de la UART1
    __delay_us(60);                 // Delay por seguridad
    U2MODEbits.UARTEN = 0b0;        // Deshabilito la UART1
};

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
