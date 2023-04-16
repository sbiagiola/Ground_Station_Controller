#include <p33FJ128GP804.h>
#include "UART.h"
#include "Clock.h"
#include "libpic30.h"
#include "RingBuffer.h"

/*==================== [Macros y definiciones] ===========================*/
#define BAUDRATE 9600
#define BRGVAL ((FCY/BAUDRATE)/16) - 1

#define RING_BUFFER_SIZE MAX_SIZE_DATA_SEND
/*========================================================================*/


// NO BORRAR LAS \ QUE ESTAN ACA O SE ROMPE TODA LA INICIALIZACION
#define Create_RingBuffer_TX_U1(pRingBuffer)                \
    static uint8_t pBuf1[MAX_SIZE_COMMAND_AVALIBLE];        \
    ringBufferData_struct pRingBuffer = {              \
        .count = 0,                                         \
        .indexRead =0,                                      \
        .indexWrite= 0,                                     \
        .pBuf = pBuf1,                                      \
        .size = MAX_SIZE_COMMAND_AVALIBLE                   \
    }

#define Create_RingBuffer_RX_U1(pRingBuffer)                \
    static uint8_t pBuf2[MAX_SIZE_COMMAND_AVALIBLE];        \
    ringBufferData_struct pRingBuffer = {              \
        .count = 0,                                         \
        .indexRead =0,                                      \
        .indexWrite= 0,                                     \
        .pBuf = pBuf2,                                      \
        .size = MAX_SIZE_COMMAND_AVALIBLE                   \
    }

#define Create_RingBuffer_TX_U2(pRingBuffer)                \
    static uint8_t pBuf3[MAX_SIZE_COMMAND_AVALIBLE];        \
    ringBufferData_struct pRingBuffer = {              \
        .count = 0,                                         \
        .indexRead =0,                                      \
        .indexWrite= 0,                                     \
        .pBuf = pBuf3,                                      \
        .size = MAX_SIZE_COMMAND_AVALIBLE                   \
    }

#define Create_RingBuffer_RX_U2(pRingBuffer)                \
    static uint8_t pBuf4[MAX_SIZE_COMMAND_AVALIBLE];        \
    ringBufferData_struct pRingBuffer = {              \
        .count = 0,                                         \
        .indexRead =0,                                      \
        .indexWrite= 0,                                     \
        .pBuf = pBuf4,                                      \
        .size = MAX_SIZE_COMMAND_AVALIBLE                   \
    }

Create_RingBuffer_TX_U1(pRingBufferTx_U1);
Create_RingBuffer_RX_U1(pRingBufferRx_U1);
Create_RingBuffer_TX_U2(pRingBufferTx_U2);
Create_RingBuffer_RX_U2(pRingBufferRx_U2);

uint8_t Respuesta;
int Error_UART_U1;
volatile int Error_UART_U2 = 0;

/* ======================================================================================= */

void Config_UART(void){

    // ========= Configuración UART 1
    
    U1MODEbits.UEN = 0b00;          // Solamente utilizamos los pines U1TX U1RX  
    U1MODEbits.BRGH = 0b0;          // Baud Rate Generator genera 16 clock por bit
    U1MODEbits.STSEL = 0b0;         // 1-stop bit
    U1MODEbits.PDSEL = 0b0;         // No Parity, 8-data bits
    
    IFS0bits.U1RXIF = 0;            // Clear RX Interrupt flag
    IFS0bits.U1TXIF = 0;            // Clear TX Interrupt flag
    
    U1STAbits.UTXISEL0 = 0b0;       // Interrupción cuando se transmita hay lugar en TXREG o TSR esta vacio
    U1STAbits.UTXISEL1 = 0b0;      
    U1STAbits.URXISEL = 0b00;       // Interrupciones cuando hay 1 caracter en RXREG
      
    // Con este valor de BRG obtengo un Baud Rate de 9615 y un error del 0.125% respecto al buscado de 9600 */
    U1BRG = BRGVAL;                    //Valor del Baud Rate Generator Register de la UART1
    
    U1MODEbits.UARTEN = 0b1;        // Habilito la UART1
    __delay_us(60);
    U1STAbits.UTXEN = 0b1;          // Habilito el transmisor de la UART1
    __delay_us(150);                // Se recomienda este delay luego de habilitar el transmisor UART
    
    // ========= Configuración UART 2
      
    U2MODEbits.UEN = 0b00;          // Solamente utilizamos los pines U1TX U1RX
    U2MODEbits.BRGH = 0b0;          // Baud Rate Generator genera 16 clock por bit
    U2MODEbits.STSEL = 0b0;         // 1-stop bit
    U2MODEbits.PDSEL = 0b0;         // No Parity, 8-data bits
    
    IFS1bits.U2RXIF = 0;            // Clear RX Interrupt flag
    IFS1bits.U2TXIF = 0;            // Clear TX Interrupt flag
    
    U2STAbits.UTXISEL0 = 0b0;       // Interrupción cuando se transmita hay lugar en TXREG o TSR esta vacio
    U2STAbits.UTXISEL1 = 0b0;       
    U2STAbits.URXISEL = 0b00;       // Interrupciones cuando hay 1 caracter en RXREG
    
    // Con este valor de BRG obtengo un Baud Rate de 9615 y un error del 0.125% respecto al buscado de 9600 */
    U2BRG = BRGVAL;                 // Valor del Baud Rate Generator de la UART2
    
    U2MODEbits.UARTEN = 0b1;        // Habilito la UART2
    __delay_us(60);
    U2STAbits.UTXEN = 0b1;          // Habilito el transmisor de la UART2
    __delay_us(150);                // Se recomienda este delay luego de habilitar el transmisor UART
}

/* ======================================================================================= */
/* =================================     UART 1     ====================================== */

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
    else{
        // FIFO vacia
        return(0);
    }
}

unsigned int Rx_Shift_Reg_U1_State(void){
    if(U1STAbits.RIDLE == 1){
        // Registro de recepción vacío (Esperando recepción)
        return(0);
    }
    else{
        // Recibiendo datos.
        return(1);
    }
}

void Get_Char_Rx_Reg_U1(uint8_t *data){
     *data = U1RXREG;
}

void Send_Char_Tx_Reg_U1(uint8_t *data){
    U1TXREG = *data;
}

/* ======================================================================================= */
/* =================================     UART 2     ====================================== */

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
    else{    
        //Al menos un caracter puede escribirse en la pila
        return(0);
    }
}

unsigned int Tx_Shift_Reg_U2_State(void){
    if(U2STAbits.TRMT == 0){
        // Transmitiendo datos y/o también existe datos en cola.
        return(1);
    }
    else{   
        // Registro vacio y no hay más datos a enviar
    return(0);
    }
}

unsigned int Rx_Reg_U2_State(void){
    if(U2STAbits.URXDA == 1){
        //Al menos un caracter disponible en el buffer de recepción.
        return(1);
    }
    else{
        // FIFO vacia
        return(0);
    }
}

unsigned int Rx_Shift_Reg_U2_State(void){
    if(U2STAbits.RIDLE == 1){
        // Registro de recepción vacío (Esperando recepción)
        return(0);
    }
    else{
        // Recibiendo datos.
        return(1);
    }
}

void Get_Char_Rx_Reg_U2(uint8_t* data){
     *data = U2RXREG;  
}

void Send_Char_Tx_Reg_U2(uint8_t* data){
    U2TXREG = *data;
}

/* ======================================================================================= */
/* ==============================     LOOPBACK MODE     ================================== */
// Posible testeo de las UARTS conectando transmisor y receptor.

void Loopback_Mode(void){
    
    // ========= Configuración UART 1
    
    U1MODEbits.UEN = 0b00;          // Solamente utilizamos el pin U1TX (U1RX esta sin utilización).
    U1MODEbits.UARTEN = 0b1;        // Habilito la UART1     
    U1MODEbits.BRGH = 0b0;          // Baud Rate Generator genera 16 clock por bit
    U1MODEbits.STSEL = 0b0;         // 1-stop bit
    U1MODEbits.PDSEL = 0b0;         // No Parity, 8-data bits
        
    // Con este valor de BRG obtengo un Baud Rate de 9615 y un error del 0.125% respecto al buscado de 9600 */
    U1BRG = 259;    //Valor del Baud Rate Generator Register de la UART1
    
    //Era recomendado ponerlo luego de haber configurado todo
    U1MODEbits.LPBACK = 0b01;       //Loopback Mode habilitado U1TX conectado internamente a U1RX
    
    // ========= Configuración UART 2
      
    U2MODEbits.UEN = 0b00;          // Solamente utilizamos el pin U1TX (U1RX esta sin utilización).
    U2MODEbits.UARTEN = 0b1;        // Habilito la UART2
    U2MODEbits.BRGH = 0b0;          // Baud Rate Generator genera 16 clock por bit
    U2MODEbits.STSEL = 0b0;         // 1-stop bit
    U2MODEbits.PDSEL = 0b0;         // No Parity, 8-data bits
    
    /* Con este valor de BRG obtengo un Baud Rate de 9615 y un error del 0.125% respecto al buscado de 9600 */
    U2BRG = U1BRG;    //Valor del Baud Rate Generator de la UART2. Ponemos el mismo en las dos UART.
    
    //Era recomendado ponerlo luego de haber configurado todo
    U2MODEbits.LPBACK = 0b01;       //Loopback Mode habilitado U2TX conectado internamente a U2RX
}

/* ======================================================================================= */
/* =============================     SERIAL PRINT     ==================================== */

void WriteUART2(unsigned int data) {
    while(U2STAbits.UTXBF);
    if(U2MODEbits.PDSEL == 2)
        U2TXREG = data;
    else
        U2TXREG = data & 0x00FF;
}

void putrsUART2(const char *buffer) {
    while(*buffer) {
        WriteUART2(*buffer);
        buffer++;
    }
}

/* ======================================================================================= */
/* ===============================     INTERRUPTS     ==================================== */

void __attribute__((interrupt,no_auto_psv)) _U1TXInterrupt(void){
    uint8_t data;           //Variable temporal - Almacena 1 dato del RB
    
    if(!ringBuffer_isEmpty(&pRingBufferTx_U1)){
        U1STAbits.UTXISEL0 = 0b0;    // Interrupción cuando transmito de FIFO a TSR y generas que se vacie la FIFO
        U1STAbits.UTXISEL1 = 0b1;
    }else{
        IEC0bits.U1TXIE = 0b0;          // Habilito interrupciones de TX, no hay datos para mandar.
    }
    
    if(!Tx_Reg_U1_State() && !ringBuffer_isEmpty(&pRingBufferTx_U1)){      //Hay al menos un lugar en el buffer de transmisión y tenemos datos en el RB
        while( !Tx_Reg_U1_State() && ringBuffer_getData(&pRingBufferTx_U1, &data) ){     //Hasta que no se llene el registro de transmisión
            Send_Char_Tx_Reg_U1(&data);
        }
    }

        // No hay datos en RB, se limpia solo la bandera seteada por que se vacío el TXREG 
    IFS0bits.U1TXIF = 0;    // Clear TX Interrupt flag
}

void __attribute__((interrupt,no_auto_psv)) _U1RXInterrupt(void){
    uint8_t data;           //Variable temporal - Almacena 1 dato del RB
   
        while( Rx_Reg_U1_State() ){    // Existe al menos un dato para leer en la FIFO de recepción    
            Get_Char_Rx_Reg_U1(&data);      
            ringBuffer_putData(&pRingBufferRx_U1,data);
        }
    
    IFS0bits.U1RXIF = 0;    // Clear RX Interrupt flag
}

void __attribute__((interrupt,no_auto_psv)) _U1ErrInterrupt(void){
    uint8_t data;
    Error_UART_U1 = 1;     // Seteamos una flag para vaciar el comando recibido en el RB
    Respuesta = NEGATIVE_ACKNOWLEDGE;
    uart_ringBuffer_envDatos_U1(&Respuesta,sizeof(char));
    
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
    
    if(!ringBuffer_isEmpty(&pRingBufferTx_U2)){
        // Registro TSR vacio y hay datos en RB
        U2STAbits.UTXISEL0 = 0b0;    // Interrupción cuando transmito de FIFO a TSR y generas que se vacie la FIFO
        U2STAbits.UTXISEL1 = 0b1;
    }else{
        IEC1bits.U2TXIE = 0b0;  // Deshabilito interrupciones de TX, no hay datos para mandar.
    }
    
    if(!Tx_Reg_U2_State() && !ringBuffer_isEmpty(&pRingBufferTx_U2)){
        //Hay al menos un lugar en el buffer de transmisión y tenemos datos en el RB
        while( !Tx_Reg_U2_State() && ringBuffer_getData(&pRingBufferTx_U2, &data) ){
            Send_Char_Tx_Reg_U2(&data);
        }
    }
    
    IFS1bits.U2TXIF = 0;    // Clear TX Interrupt flag
}

void __attribute__((interrupt,no_auto_psv)) _U2RXInterrupt(void){
    uint8_t data;           //Variable temporal - Almacena 1 dato del RB
  
    while( Rx_Reg_U2_State() ){        // Existe al menos un dato para leer en la FIFO de recepción
        Get_Char_Rx_Reg_U2(&data);      // Saco un dato x iteración, lo guardo dentro de data
//        ringBuffer_putData(pRingBufferRx_U2, data);     // Envio el dato recuperado al RB
        
        // Testear esto
        ringBuffer_putData(&pRingBufferRx_U2,data);
    }
    
    IFS1bits.U2RXIF = 0;    // Clear RX Interrupt flag 
}

void __attribute__((interrupt,no_auto_psv)) _U2ErrInterrupt(void){
    uint8_t data;
    int i;
    
    
//    Error_UART_U2 = 1;     // Seteamos una flag para vaciar el comando recibido en el RB
//    putrsUART2("[UART] ERROR_UART_U2\n");
    Respuesta = NEGATIVE_ACKNOWLEDGE;
    uart_ringBuffer_envDatos_U2(&Respuesta,sizeof(char));

    if(U2STAbits.OERR && !U2STAbits.FERR){     
        U2STAbits.OERR = 0b0;       // Clear del overrun para permitir recepción de más datos. Vaciamos la FIFO
//        putrsUART2("[UART Error] ERROR por overflow \n");
    }
    else if(U2STAbits.PERR && !U2STAbits.FERR){
//        putrsUART2("[UART Error] ERROR de pariedad  \n");
        for (i=1;i<=5;i++){
            if(!U2STAbits.PERR){ 
                Get_Char_Rx_Reg_U2(&data);  // Saco un dato x iteración, solo vacio la FIFO
            }
            if(U2STAbits.PERR){             // El char en la FIFO contiene falla en el bit de STOP
//                putrsUART2("[UART Error] Error en el caracter\n");
                Get_Char_Rx_Reg_U2(&data);  
                break;
            }
        }
    }
    else if(U2STAbits.FERR && !U2STAbits.OERR){
//        putrsUART2("[UART Error] ERROR en el bit STOP \n");
        for (i=1;i<=5;i++){
            if(!U2STAbits.FERR){ 
                Get_Char_Rx_Reg_U2(&data);  // Saco un dato x iteración, solo vacio la FIFO
            }
            if(U2STAbits.FERR){             // El char en la FIFO contiene falla en el bit de STOP
//                putrsUART2("[UART Error] Error en el caracter \n");
                Get_Char_Rx_Reg_U2(&data);  
                break;
            }
        }
        Error_UART_U2 = 0;
    }
    else Error_UART_U2 = 0; // Error no reconocible
    
    IFS4bits.U2EIF = 0;     // Clear Error Interrupt flag 
}

/* ======================================================================================= */
/* ===============================     RINGBUFFER     ==================================== */

int32_t uart_ringBuffer_recDatos_U1(uint8_t *pBuf, int32_t size){
    int32_t ret = 0;
    
    /* Es necesario deshabilitar las demás interrupciónes para garantizar que no perdamos algún dato
    
     Para desactivar las interrupciones de prioridad entre 1 y 6 es necesario setear el bit
     INTCON2bits.DISI en 1 para que se ejecute la instrucción DISI. Luego es necesario setear el número de
     ciclos de clock que vamos tener deshabilitadas estas en el registro DISICNT. Si este registro llega
     a 0 se vuelven a habilitar las interrupciones. 
    
    */
    
    INTCON2bits.DISI = 0b1;     // Deshabilito todas las interrupciones 
    DISICNT = 16380;            // Máximo valor posible de ciclos de deshabilitación de interrupciones 
       
    /*================== Sección critica de código ==================*/
    
    while (!ringBuffer_isEmpty(&pRingBufferRx_U1) && ret < size)
    {
    	uint8_t dato;

        ringBuffer_getData(&pRingBufferRx_U1, &dato);
        pBuf[ret] = dato;
        ret++;
        DISICNT = 16380;        // Recarga del contador
    }
    DISICNT = 0;                // Habilitamos nuevamente las interrupciones
    INTCON2bits.DISI = 0b0;     // Habilito todas las interrupciones 
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
    DISICNT = 16380;            // Máximo valor posible de ciclos de deshabilitación de interrupciones 
       
    /*================== Sección critica de código ==================*/
    
        /* si el buffer estaba vacío hay que habilitar la interrupción TX */
        if (ringBuffer_isEmpty(&pRingBufferTx_U1)){
            U1STAbits.UTXISEL0 = 0b0;       // Interrupciones por TSR vacio o hay lugar en TXREG (Entro a la rutina si o si después del DISI))
            U1STAbits.UTXISEL1 = 0b0;
            IEC0bits.U1TXIE = 0b1;          // Habilito interrupciones de TX
        }

        while (!ringBuffer_isFull(&pRingBufferTx_U1) && ret < size)
        {
            ringBuffer_putData(&pRingBufferTx_U1, pBuf[ret]);
            ret++;
            DISICNT = 16380;        // Recarga del contador
        }
    /*============== Fin de sección critica de código ===============*/
    DISICNT = 0;                // Habilitamos nuevamente las interrupciones
    INTCON2bits.DISI = 0b0;     // Habilito todas las interrupciones 

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
    DISICNT = 16380;            // Máximo valor posible de ciclos de deshabilitación de interrupciones 
       
    /*================== Sección critica de código ==================*/
        while (!ringBuffer_isEmpty(&pRingBufferRx_U2) && ret < size)
        {
            uint8_t dato;

            ringBuffer_getData(&pRingBufferRx_U2, &dato);
            pBuf[ret] = dato;
            ret++;
            DISICNT = 16380;        // Recarga del contador
        }
    /*============== Fin de sección critica de código ===============*/
    
    DISICNT = 0;                // Habilitamos nuevamente las interrupciones
    INTCON2bits.DISI = 0b0;     // Habilito todas las interrupciones 
    
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
    DISICNT = 16380;            // Máximo valor posible de ciclos de deshabilitación de interrupciones 
    
    /*================== Sección critica de código ==================*/
    
    /* si el buffer estaba vacío hay que habilitar la int TX */
    if (ringBuffer_isEmpty(&pRingBufferTx_U2)){
        U2STAbits.UTXISEL0 = 0b0;       // Interrupciones por TSR vacio o hay lugar en TXREG (Entro a la rutina si o si después del DISI))
        U2STAbits.UTXISEL1 = 0b0;
        IEC1bits.U2TXIE = 0b1;          // Habilito interrupciones de TX
    }
    
    while (!ringBuffer_isFull(&pRingBufferTx_U2) && ret < size)
    {
        ringBuffer_putData(&pRingBufferTx_U2, pBuf[ret]);
        ret++;
        DISICNT = 16380;        // Recarga del contador
    }
    /*============== Fin de sección critica de código ===============*/
    DISICNT = 0;                // Habilitamos nuevamente las interrupciones
    INTCON2bits.DISI = 0b0;     // Habilito todas las interrupciones 
    
return ret;
}

