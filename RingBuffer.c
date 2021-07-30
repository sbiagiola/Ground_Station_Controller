/*
 * File:   main.c
 * Author: Jere
 *
 * Created on 13 de mayo de 2021, 14:50
 */

#include <xc.h>

#include "RingBuffer.h"
#include "stdlib.h"
#include "stdint.h"

#define CIRC_BBUF_DEF(x,y)                \
    uint8_t x##_data_space[y];            \
    ringBufferData_struct_TEST x = {                     \
        .pBuf = x##_data_space,         \
        .indexRead = 0,                        \
        .indexWrite = 0,                        \
        .size = y                       \
    }

typedef struct{
    int32_t indexRead;      // Ubicación/posición del indice de lectura
    int32_t indexWrite;     // Ubicación/posición del indice de escritura
    int32_t count;          // Cantidad de elementos en el mismo
    int32_t size;           // Tamaño del RingBuffer
    uint8_t *pBuf;          // Dato a almacenar en la posición determinada por indexRead o indexWrite del RingBuffer
}ringBufferData_struct;

void* ringBuffer_init(int32_t size){
    ringBufferData_struct *pRingBuffer;
    pRingBuffer = malloc(sizeof(ringBufferData_struct));
    
    pRingBuffer->pBuf = malloc(size);
    
    pRingBuffer->indexRead = 0;
    pRingBuffer->indexWrite = 0;
    pRingBuffer->count = 0;
    pRingBuffer->size = size;

return pRingBuffer;
}

unsigned int ringBuffer_putData(void *pRingBuffer, uint8_t data){
    ringBufferData_struct *rb = pRingBuffer;    
    unsigned int ret = 1;
    
    rb->pBuf[rb->indexWrite] = data;        // Se el dato a grabar en la posición indexWrite 

    rb->indexWrite++;                       // Se actualiza el indice de escritura
    if (rb->indexWrite >= rb->size)
        rb->indexWrite = 0;

    if (rb->count < rb->size)
    {
        rb->count++;
    }
    else
    {
        /* si el buffer está lleno incrementa en uno indexRead
         * haciendo que se pierda el dato más viejo y devuelve
         * true para indicar que se estan perdiendo datos */
        rb->indexRead++;
        if (rb->indexRead >= rb->size)
            rb->indexRead = 0;
        ret = 0;
    }

    return ret;
}

unsigned int ringBuffer_getData(void *pRingBuffer, uint8_t *data){
    ringBufferData_struct *rb = pRingBuffer;
    unsigned int ret = 1; 

    if (rb->count)      // Al menos hay 1 dato que leer, sino pasa de largo
    {
        *data = rb->pBuf[rb->indexRead];

        rb->indexRead++;
        if (rb->indexRead >= rb->size)
            rb->indexRead = 0;
        rb->count--;
    }
    else
    {
        ret = 0;        // No hay datos almacenados en el RingBuffer
    }

    return ret;
}

unsigned int ringBuffer_isFull(void *pRingBuffer){
    ringBufferData_struct *rb = pRingBuffer;

    return rb->count == rb->size;       // Devuelve un "true" si el RingBuffer esta lleno
}

unsigned int ringBuffer_isEmpty(void *pRingBuffer){
    ringBufferData_struct *rb = pRingBuffer;

    return rb->count == 0;              // Devuelve un "true" si el RingBuffer esta vacío
}

unsigned int ringBuffer_putData_TEST(ringBufferData_struct_TEST *pRingBuffer, uint8_t data){
    unsigned int ret = 1;
    
    pRingBuffer->pBuf[pRingBuffer->indexWrite] = data;        // Se el dato a grabar en la posición indexWrite 

    pRingBuffer->indexWrite++;                       // Se actualiza el indice de escritura
    if (pRingBuffer->indexWrite >= pRingBuffer->size)
        pRingBuffer->indexWrite = 0;

    if (pRingBuffer->count < pRingBuffer->size)
    {
        pRingBuffer->count++;
    }
    else
    {
        /* si el buffer está lleno incrementa en uno indexRead
         * haciendo que se pierda el dato más viejo y devuelve
         * true para indicar que se estan perdiendo datos */
        pRingBuffer->indexRead++;
        if (pRingBuffer->indexRead >= pRingBuffer->size)
            pRingBuffer->indexRead = 0;
        ret = 0;
    }

    return ret;
}

unsigned int ringBuffer_getData_TEST(ringBufferData_struct_TEST *pRingBuffer, uint8_t *data){
    unsigned int ret = 1; 

    if (pRingBuffer->count)      // Al menos hay 1 dato que leer, sino pasa de largo
    {
        *data = pRingBuffer->pBuf[pRingBuffer->indexRead];

        pRingBuffer->indexRead++;
        if (pRingBuffer->indexRead >= pRingBuffer->size)
            pRingBuffer->indexRead = 0;
        pRingBuffer->count--;
    }
    else
    {
        ret = 0;        // No hay datos almacenados en el RingBuffer
    }

    return ret;
}

unsigned int ringBuffer_isFull_TEST(ringBufferData_struct_TEST *pRingBuffer){
    return pRingBuffer->count == pRingBuffer->size;       // Devuelve un "true" si el RingBuffer esta lleno
}

unsigned int ringBuffer_isEmpty_TEST(ringBufferData_struct_TEST *pRingBuffer){
    return pRingBuffer->count == 0;              // Devuelve un "true" si el RingBuffer esta vacío
}