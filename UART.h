/* 
 * File:   UART.h
 * Author: Jere
 *
 * Created on 15 de abril de 2021, 09:52
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif
/***************  Definiciones   **************/   

/***********************************************/   
    
/****************  Funciones   *****************/  
void Config_UART(void);
void Create_RingBuffer(void);
void Loopback_Mode(void);

/**********     UART1     **********/
unsigned int Tx_Reg_U1_State(void);
unsigned int Tx_Shift_Reg_U1_State(void);
unsigned int Rx_Reg_U1_State(void);
unsigned int Rx_Shift_Reg_U1_State(void);
void Get_Char_Rx_Reg_U1(uint8_t *data);
void Send_Char_Tx_Reg_U1(uint8_t *data);
void Enable_UART1(void);
void Disable_UART1(void);

/**********     UART2     **********/
unsigned int Tx_Reg_U2_State(void);
unsigned int Tx_Shift_Reg_U2_State(void);
unsigned int Rx_Reg_U2_State(void);
unsigned int Rx_Shift_Reg_U2_State(void);
void Get_Char_U2(uint8_t *data);
void Send_Char_Tx_Reg_U2(uint8_t *data);
void Enable_UART2(void);
void Disable_UART2(void);
/***********************************************/

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

