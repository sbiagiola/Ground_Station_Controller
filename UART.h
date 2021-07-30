/* 
 * File:   UART.h
 * Author: Jere
 *
 * Created on 15 de abril de 2021, 09:52
 */
#include "stdint.h"

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
void Get_Char_Rx_Reg_U1(uint8_t* data);
void Send_Char_Tx_Reg_U1(uint8_t* data);
void Enable_UART1(void);
void Disable_UART1(void);

/**********     UART2     **********/
unsigned int Tx_Reg_U2_State(void);
unsigned int Tx_Shift_Reg_U2_State(void);
unsigned int Rx_Reg_U2_State(void);
unsigned int Rx_Shift_Reg_U2_State(void);
void Get_Char_Rx_Reg_U2(uint8_t* data);
void Send_Char_Tx_Reg_U2(uint8_t* data);
void Enable_UART2(void);
void Disable_UART2(void);

/***********************************************/

/** \brief recibe datos por puerto serie accediendo al RB
 **
 ** \param[inout] pBuf buffer a donde guardar los datos
 ** \param[in] size tamaño del buffer
 ** \return cantidad de bytes recibidos
 **/
int32_t uart_ringBuffer_recDatos_U1(uint8_t *pBuf, int32_t size);

/** \brief envía datos por puerto serie accediendo al RB
 **
 ** \param[inout] pBuf buffer a donde estan los datos a enviar
 ** \param[in] size tamaño del buffer
 ** \return cantidad de bytes enviados
 **/
int32_t uart_ringBuffer_envDatos_U1(uint8_t *pBuf, int32_t size);

/** \brief recibe datos por puerto serie accediendo al RB
 **
 ** \param[inout] pBuf buffer a donde guardar los datos
 ** \param[in] size tamaño del buffer
 ** \return cantidad de bytes recibidos
 **/
int32_t uart_ringBuffer_recDatos_U2(uint8_t *pBuf, int32_t size);

/** \brief envía datos por puerto serie accediendo al RB
 **
 ** \param[inout] pBuf buffer a donde estan los datos a enviar
 ** \param[in] size tamaño del buffer
 ** \return cantidad de bytes enviados
 **/
int32_t uart_ringBuffer_envDatos_U2(uint8_t *pBuf, int32_t size);

/*Testeos*/

int32_t uart_ringBuffer_envDatos_U2_TEST(uint8_t *pBuf, int32_t size);
int32_t uart_ringBuffer_recDatos_U2_TEST(uint8_t *pBuf, int32_t size);

void Clean_RingBufferRx_U2_TEST(void);
void Clean_RingBufferRx_U1_TEST(void);
#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

