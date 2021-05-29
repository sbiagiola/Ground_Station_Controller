#include <p33FJ128GP804.h>
#include "UART.h"
#include "Clock.h"
#include "libpic30.h"
#include "RingBuffer.h"
#include "stdint.h"
#include "Protocolo_Comm_Yaesu.h"

/*==================== [Macros y definiciones] ===========================*/
#define RING_BUFFER_SIZE 16   // Tamaño del Ring Buffer
/*========================================================================*/

/*======================= [Variables Internas] ===========================*/
void* pRingBufferTx_U1;     
void* pRingBufferRx_U1;  
void* pRingBufferTx_U2;  
void* pRingBufferRx_U2;
uint8_t Respuesta;
int Error_UART_U1;
volatile int Error_UART_U2;
/*========================================================================*/

void Config_UART(void){

    /*******************        Configuración UART 1        *******************/
    
    //Register U1MODE
    U1MODEbits.UEN = 0b00;          // Solamente utilizamos los pines U1TX U1RX  
    U1MODEbits.BRGH = 0b0;          // Baud Rate Generator genera 16 clock por bit
    U1MODEbits.STSEL = 0b0;         // 1-stop bit
    U1MODEbits.PDSEL = 0b0;         // No Parity, 8-data bits
    
    //Register U1STA
    U1STAbits.UTXISEL0 = 0b0;       // Interrupción cuando se transmita al registro TSR un caracter y el buffer de transmisión este vacío
    U1STAbits.UTXISEL1 = 0b1;      
    U1STAbits.URXISEL = 0b00;       // Interrupciones cuando un caracter es transferido de RSR al RXREG
      
    // Con este valor de BRG obtengo un Baud Rate de 9615 y un error del 0.125% respecto al buscado de 9600 */
    U1BRG = 259;                    //Valor del Baud Rate Generator Register de la UART1

    /*******************        Configuración UART 2        *******************/
      
    //Register U2MODE
    U2MODEbits.UEN = 0b00;          // Solamente utilizamos los pines U1TX U1RX
    U2MODEbits.BRGH = 0b0;          // Baud Rate Generator genera 16 clock por bit
    U2MODEbits.STSEL = 0b0;         // 1-stop bit
    U2MODEbits.PDSEL = 0b0;         // No Parity, 8-data bits
    
    //Register U2STA
    U2STAbits.UTXISEL0 = 0b0;       // Interrupción cuando se transmita al registro TSR un caracter y el buffer de transmisión
    U2STAbits.UTXISEL1 = 0b1;       
    U2STAbits.URXISEL = 0b00;       // Interrupciones cuando un caracter es transferido de RSR al RXREG
    
    U2BRG = U1BRG;                  // Valor del Baud Rate Generator de la UART2
}

void Create_RingBuffer(void){
    /* Inicialización de buffer de recepción y transmisión*/
    pRingBufferTx_U1 = ringBuffer_init(RING_BUFFER_SIZE);
    pRingBufferRx_U1 = ringBuffer_init(RING_BUFFER_SIZE);     
    pRingBufferTx_U2 = ringBuffer_init(RING_BUFFER_SIZE);
    pRingBufferRx_U2 = ringBuffer_init(RING_BUFFER_SIZE);
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

void Get_Char_Rx_Reg_U1(uint8_t *data){
     *data = U1RXREG;
}

void Send_Char_Tx_Reg_U1(uint8_t *data){
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

void Get_Char_Rx_Reg_U2(uint8_t *data){
     *data = U2RXREG;
}

void Send_Char_Tx_Reg_U2(uint8_t *data){
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

int32_t uart_ringBuffer_recDatos_U1(uint8_t *pBuf, int32_t size){
    int32_t ret = 0;
    
    /* Es necesario deshabilitar las demás interrupciónes para garantizar que no perdamos algún dato
    
     Para desactivar las interrupciones de prioridad entre 1 y 6 es necesario setear el bit
     INTCON2bits.DISI en 1 para que se ejecute la instrucción DISI. Luego es necesario setear el número de
     ciclos de clock que vamos tener deshabilitadas estas en el registro DISICNT. Si este registro llega
     a 0 se vuelven a habilitar las interrupciones. 
    
    */
    
    INTCON2bits.DISI = 0b1;     // Deshabilito todas las interrupciones 
    DISICNT = 16384;            // Máximo valor posible de ciclos de deshabilitación de interrupciones 
       
    /*================== Sección critica de código ==================*/
    
    while (!ringBuffer_isEmpty(pRingBufferRx_U1) && ret < size)
    {
    	uint8_t dato;

        ringBuffer_getData(pRingBufferRx_U1, &dato);
        pBuf[ret] = dato;
        ret++;
        DISICNT = 16384;        // Recarga del contador
    }
    DISICNT = 0;                // Habilitamos nuevamente las interrupciones
    
    /*================== Fin de sección critica de código ==================*/
return ret;
}

int32_t uart_ringBuffer_envDatos_U1(uint8_t *pBuf, int32_t size){
    int32_t ret = 0;
    
    /* Es necesario deshabilitar las demás interrupciónes para garantizar que no perdamos algún dato
    
     Para desactivar las interrupciones de prioridad entre 1 y 6 es necesario setear el bit
     INTCON2bits.DISI en 1 para que se ejecute la instrucción DISI. Luego es necesario setear el número de
     ciclos de clock que vamos tener deshabilitadas estas en el registro DISICNT. Si este registro llega
     a 0 se vuelven a habilitar las interrupciones. 
    
    */ 
    
    INTCON2bits.DISI = 0b1;     // Deshabilito todas las interrupciones 
    DISICNT = 16384;            // Máximo valor posible de ciclos de deshabilitación de interrupciones 
       
    /*================== Sección critica de código ==================*/
    
    /* si el buffer estaba vacío hay que habilitar la int TX */
    if (ringBuffer_isEmpty(pRingBufferTx_U1))

    while (!ringBuffer_isFull(pRingBufferTx_U1) && ret < size)
    {
        ringBuffer_putData(pRingBufferTx_U1, pBuf[ret]);
        ret++;
        DISICNT = 16384;        // Recarga del contador
    }
    DISICNT = 0;                // Habilitamos nuevamente las interrupciones
    
    /*============== Fin de sección critica de código ===============*/
return ret;
}

int32_t uart_ringBuffer_recDatos_U2(uint8_t *pBuf, int32_t size){
    int32_t ret = 0;
    
    /* Es necesario deshabilitar las demás interrupciónes para garantizar que no perdamos algún dato
    
     Para desactivar las interrupciones de prioridad entre 1 y 6 es necesario setear el bit
     INTCON2bits.DISI en 1 para que se ejecute la instrucción DISI. Luego es necesario setear el número de
     ciclos de clock que vamos tener deshabilitadas estas en el registro DISICNT. Si este registro llega
     a 0 se vuelven a habilitar las interrupciones. 
    
     */
    
    INTCON2bits.DISI = 0b1;     // Deshabilito todas las interrupciones 
    DISICNT = 16384;            // Máximo valor posible de ciclos de deshabilitación de interrupciones 
       
    /*================== Sección critica de código ==================*/
    
    while (!ringBuffer_isEmpty(pRingBufferRx_U2) && ret < size)
    {
    	uint8_t dato;

        ringBuffer_getData(pRingBufferRx_U2, &dato);
        pBuf[ret] = dato;
        ret++;
        DISICNT = 16384;        // Recarga del contador
    }
    DISICNT = 0;                // Habilitamos nuevamente las interrupciones
    
    /*============== Fin de sección critica de código ===============*/
return ret;
}

int32_t uart_ringBuffer_envDatos_U2(uint8_t *pBuf, int32_t size){
    int32_t ret = 0;
    
    /* Es necesario deshabilitar las demás interrupciónes para garantizar que no perdamos algún dato
    
     Para desactivar las interrupciones de prioridad entre 1 y 6 es necesario setear el bit
     INTCON2bits.DISI en 1 para que se ejecute la instrucción DISI. Luego es necesario setear el número de
     ciclos de clock que vamos tener deshabilitadas estas en el registro DISICNT. Si este registro llega
     a 0 se vuelven a habilitar las interrupciones. 
    
     */
    
    INTCON2bits.DISI = 0b1;     // Deshabilito todas las interrupciones 
    DISICNT = 16384;            // Máximo valor posible de ciclos de deshabilitación de interrupciones 
    
    /*================== Sección critica de código ==================*/
    
    /* si el buffer estaba vacío hay que habilitar la int TX */
    if (ringBuffer_isEmpty(pRingBufferTx_U2))

    while (!ringBuffer_isFull(pRingBufferTx_U2) && ret < size)
    {
        ringBuffer_putData(pRingBufferTx_U2, pBuf[ret]);
        ret++;
        DISICNT = 16384;        // Recarga del contador
    }
    
    DISICNT = 0;                // Habilitamos nuevamente las interrupciones
    
    /*============== Fin de sección critica de código ===============*/
return ret;
}

void __attribute__((interrupt,no_auto_psv)) _U1TXInterrupt(void){
    uint8_t data;           //Variable temporal - Almacena 1 dato del RB

    if(!Tx_Reg_U1_State() && ringBuffer_getData(pRingBufferTx_U1, &data)){      //Hay al menos un lugar en el buffer de transmisión y tenemos datos en el RB
        while( !Tx_Reg_U1_State() && ringBuffer_getData(pRingBufferTx_U1, &data) ){     //Hasta que no se llene el registro de transmisión
            Send_Char_Tx_Reg_U1(&data);
        }
    }
    else
        // No hay datos en RB, se limpia solo la bandera seteada por que se vacío el TXREG 
    IFS0bits.U1TXIF = 0;    // Clear TX Interrupt flag
}

void __attribute__((interrupt,no_auto_psv)) _U1RXInterrupt(void){
    uint8_t data;           //Variable temporal - Almacena 1 dato del RB
   
        while( !Rx_Reg_U1_State() ){        // Hay al menos 1 dato en la FIFO de RXREG, vaciamos el registro
            Get_Char_Rx_Reg_U1(&data);      // Saco un dato x iteración
            ringBuffer_putData(pRingBufferRx_U2, data);
        }
    
    IFS0bits.U1RXIF = 0;    // Clear RX Interrupt flag
}

void __attribute__((interrupt,no_auto_psv)) _U1ErrInterrupt(void){
    uint8_t data;
    Error_UART_U1 = 1;     // Seteamos una flag para vaciar el comando recibido en el RB
    Respuesta = NEGATIVE_ACKNOWLEDGE;
    Send_Char_Tx_Reg_U1(&Respuesta);

    if(U1STAbits.OERR && !U1STAbits.FERR){ // Overflow 
        U1STAbits.OERR = 0b0; // Clear del overrun para permitir recepción de más datos
    }
    
    if(U1STAbits.FERR && !U1STAbits.OERR){
        while(1){
            if(!U1STAbits.FERR){ 
                Get_Char_Rx_Reg_U1(&data);  // Saco un dato x iteración, solo vacio la FIFO
            }
            if(U1STAbits.FERR){             // el char en la FIFO contiene falla en el bit de STOP
                Get_Char_Rx_Reg_U1(&data);  // Saco un dato x iteración, solo vacio la FIFO
                break;
            }
        }
    }
        IFS4bits.U1EIF = 0;     // Clear Error Interrupt flag
}  
    
void __attribute__((interrupt,no_auto_psv)) _U2TXInterrupt(void){
    uint8_t data;           //Variable temporal - Almacena 1 dato del RB
    
    if(!Tx_Reg_U1_State() && ringBuffer_getData(pRingBufferTx_U1, &data)){
        
        //Hay al menos un lugar en el buffer de transmisión y tenemos datos en el RB
        
        while( !Tx_Reg_U2_State() && ringBuffer_getData(pRingBufferTx_U2, &data) ){
            Send_Char_Tx_Reg_U2(&data);
        }
        
    }
    else
        // No hay datos en RB, se limpia solo la bandera seteada.
        
    IFS1bits.U2TXIF = 0;    // Clear TX Interrupt flag
}

void __attribute__((interrupt,no_auto_psv)) _U2RXInterrupt(void){
    uint8_t data;           //Variable temporal - Almacena 1 dato del RB
  
    while( !Rx_Reg_U2_State() ){
        Get_Char_Rx_Reg_U2(&data);      // Saco un dato x iteración, lo guardo dentro de data
        ringBuffer_putData(pRingBufferRx_U2, data);     // Envio el dato recuperado al RB
    }
    
    IFS1bits.U2RXIF = 0;    // Clear RX Interrupt flag 
}

void __attribute__((interrupt,no_auto_psv)) _U2ErrInterrupt(void){
    uint8_t data;
    
    Error_UART_U2 = 1;     // Seteamos una flag para vaciar el comando recibido en el RB
    Respuesta = NEGATIVE_ACKNOWLEDGE;
    Send_Char_Tx_Reg_U2(&Respuesta);

    
    if(U2STAbits.OERR && !U2STAbits.FERR){     
        U2STAbits.OERR = 0b0;       // Clear del overrun para permitir recepción de más datos. Vaciamos la FIFO
    }
    
    if(U2STAbits.FERR && !U2STAbits.OERR){
        while(1){
            if(!U2STAbits.FERR){ 
                Get_Char_Rx_Reg_U2(&data);  // Saco un dato x iteración, solo vacio la FIFO
            }
            if(U2STAbits.FERR){             // el char en la FIFO contiene falla en el bit de STOP
                Get_Char_Rx_Reg_U2(&data);  // Saco un dato x iteración, solo vacio la FIFO
                break;
            }
        }
    }
    IFS4bits.U2EIF = 0;     // Clear Error Interrupt flag 
}

void Clean_RingBufferRx_U2(void){
    uint8_t data;
    while(!ringBuffer_isEmpty(pRingBufferRx_U2)){
        ringBuffer_getData(pRingBufferRx_U2, &data); 
    }
}

void Clean_RingBufferRx_U1(void){
    uint8_t data;
    while(!ringBuffer_isEmpty(pRingBufferRx_U1)){
        ringBuffer_getData(pRingBufferRx_U1, &data); 
    }
}