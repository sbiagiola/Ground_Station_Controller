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


/*======================= [Variables Externas] ===========================*/
extern ringBufferData_struct pRingBufferRx_U1;
extern ringBufferData_struct pRingBufferRx_U2;
/*========================================================================*/

unsigned int ringBuffer_putData(ringBufferData_struct *pRingBuffer, uint8_t data){
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

unsigned int ringBuffer_getData(ringBufferData_struct *pRingBuffer, uint8_t *data){
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

unsigned int ringBuffer_isFull(ringBufferData_struct *pRingBuffer){
    return pRingBuffer->count == pRingBuffer->size;       // Devuelve un "true" si el RingBuffer esta lleno
}

unsigned int ringBuffer_isEmpty(ringBufferData_struct *pRingBuffer){
    return pRingBuffer->count == 0;              // Devuelve un "true" si el RingBuffer esta vacío
}

void Clean_RingBufferRx_U2(void){
    uint8_t data;
    while(!ringBuffer_isEmpty(&pRingBufferRx_U2)){
        ringBuffer_getData(&pRingBufferRx_U2, &data); 
    }
}

void Clean_RingBufferRx_U1(void){
    uint8_t data;
    while(!ringBuffer_isEmpty(&pRingBufferRx_U1)){
        ringBuffer_getData(&pRingBufferRx_U1, &data); 
    }
}