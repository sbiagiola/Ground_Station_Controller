/* 
 * File:   Estructuras_Definiciones.h
 * Author: Jere
 *
 * Created on 24 de junio de 2021, 10:46
 */
#include <xc.h>
#include "Entradas.h"
#include "stdint.h"

#ifndef ESTRUCTURAS_DEFINICIONES_H
#define	ESTRUCTURAS_DEFINICIONES_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct{
    long Encoder_1_Pulsos;
    long Encoder_1_Vueltas;  
    long Encoder_2_Pulsos;
    long Encoder_2_Vueltas;
    int Anemometr0;
}Contador;

typedef struct{
    uint16_t    Encoder_1_A;
    uint16_t    Encoder_1_B;
    uint16_t    Encoder_1_Z;
    uint16_t    Encoder_2_A;
    uint16_t    Encoder_2_B;
    uint16_t    Encoder_2_Z;
    uint16_t    Anemometr0;
    uint16_t    End_Stop_1;
    uint16_t    End_Stop_2;
    uint16_t    Parad_Emerg;
}Last_Value;



#ifdef	__cplusplus
}
#endif

#endif	/* ESTRUCTURAS_DEFINICIONES_H */

