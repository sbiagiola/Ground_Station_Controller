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
#define DESIRED_BAUD_RATE 9600;  //Baud rate de la comunicación deseado
/***********************************************/   
    
/****************  Funciones   *****************/  
void Config_UART(void);
void Loopback_Mode(void);

/**********     UART1     **********/
unsigned int State_Transmission_Reg_U1(void);
unsigned int State_Shift_Register_U1(void);
unsigned int Char_Avalible_U1(void);
void Get_Char_U1(char *data);
void Send_Char_U1(char *data);
void Enable_UART1(void);
void Disable_UART1(void);

/**********     UART2     **********/
unsigned int State_Transmission_Reg_U2(void);
unsigned int State_Shift_Register_U2(void);
unsigned int Char_Avalible_U2(void);
void Get_Char_U2(char *data);
void Send_Char_U2(char *data);
void Enable_UART2(void);
void Disable_UART2(void);
/***********************************************/

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

