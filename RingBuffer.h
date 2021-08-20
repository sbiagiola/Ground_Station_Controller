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

typedef struct{
    int32_t indexRead;      // Ubicación/posición del indice de lectura
    int32_t indexWrite;     // Ubicación/posición del indice de escritura
    int32_t count;          // Cantidad de elementos en el mismo
    int32_t size;           // Tamaño del RingBuffer
    uint8_t *pBuf;       // Dato a almacenar en la posición determinada por indexRead o indexWrite del RingBuffer
}ringBufferData_struct;

/** \brief inicializa buffer circular de bytes
 **
 ** \param[in] size tamaño del buffer
 **
 ** \return puntero a buffer circular, NULL si no se pudo crear
 **/
//void *ringBuffer_init(int32_t size);

unsigned int ringBuffer_isFull(ringBufferData_struct *);
//unsigned int ringBuffer_isFull(void *pRingBuffer);

unsigned int ringBuffer_isEmpty(ringBufferData_struct *);
//unsigned int ringBuffer_isEmpty(void *pRingBuffer);

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
unsigned int ringBuffer_putData(ringBufferData_struct *, uint8_t );
//unsigned int ringBuffer_putData(void *pRingBuffer, uint8_t data);

/** \brief devuelve dato más antiguo del buffer
 **
 ** \param[inout] pRingBuffer puntero al buffer circular
 ** \param[in] data puntero a donde guardar el dato
 **
 ** \return true si se extrajo el dato del buffer
 **         false si no había datos en el buffer
 **/
unsigned int ringBuffer_getData(ringBufferData_struct *, uint8_t *);
//unsigned int ringBuffer_getData(void *pRingBuffer, uint8_t *data);

//void Clean_RingBufferRx_U1(void);
//void Clean_RingBufferRx_U2(void);

void Clean_RingBufferRx_U2(void);
void Clean_RingBufferRx_U1(void);

#ifdef	__cplusplus
}
#endif

#endif	/* RINGBUFFER_H */

