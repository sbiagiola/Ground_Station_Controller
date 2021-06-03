/* 
 * File:   RingBuffer.h
 * Author: Jere
 *
 * Created on 13 de mayo de 2021, 19:37
 */
#include "stdint.h"     

#ifndef RINGBUFFER_H
#define	RINGBUFFER_H

#ifdef	__cplusplus
extern "C" {
#endif

/** \brief inicializa buffer circular de bytes
 **
 ** \param[in] size tamaño del buffer
 **
 ** \return puntero a buffer circular, NULL si no se pudo crear
 **/
void *ringBuffer_init(int32_t size);

unsigned int ringBuffer_isFull(void *pRingBuffer);

unsigned int ringBuffer_isEmpty(void *pRingBuffer);

/** \brief pone nuevo dato en el buffer circular
 ** si el buffer esta lleno se pisa el dato más antiguo
 ** y se avanza el puntero de lectura
 **
 ** \param[inout] pRingBuffer puntero al buffer circular
 ** \param[in] data dato a colocar en el buffer
 **
 ** \return true si se pudo poner el dato correctamente
 **         false si se puso y se piso un dato antiguo
 **/
unsigned int ringBuffer_putData(void *pRingBuffer, uint8_t data);

/** \brief devuelve dato más antiguo del buffer
 **
 ** \param[inout] pRingBuffer puntero al buffer circular
 ** \param[in] data puntero a donde guardar el dato
 **
 ** \return true si se extrajo el dato del buffer
 **         false si no había datos en el buffer
 **/
unsigned int ringBuffer_getData(void *pRingBuffer, uint8_t *data);

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

#ifdef	__cplusplus
}
#endif

#endif	/* RINGBUFFER_H */

