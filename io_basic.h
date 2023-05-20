/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef IO_BASIC_H
#define	IO_BASIC_H

#include <xc.h> // include processor files - each processor file is guarded.

/////////////////
#define HIGH    1
#define LOW     0
#define ON      1
#define OFF     0
/////////////////

// Entradas -----------------------------
#define ENCODER_ELEV_A      PORTBbits.RB6
#define ENCODER_ELEV_B      PORTBbits.RB7
#define ENCODER_ELEV_Z      PORTBbits.RB8
    
#define ENCODER_AZ_A        PORTCbits.RC5
#define ENCODER_AZ_B        PORTCbits.RC4
#define ENCODER_AZ_Z        PORTCbits.RC3

#define HOME_STOP_ELEV      PORTBbits.RB9
#define HOME_STOP_AZ        PORTBbits.RB5
    
#define PARADA_EMERGENCIA   PORTCbits.RC2
#define ANEMOMETRO          PORTAbits.RA4
// ----------------------------------------

// Salidas --------------------------------
#define OUT_VAR_1           LATAbits.LATA10
#define OUT_VAR_2           LATAbits.LATA7
#define OUT_VAR_3           LATBbits.LATB14
#define OUT_VAR_4           LATBbits.LATB15
    
#define OUT_RELE_4          LATCbits.LATC9
#define READ_RELE_4         PORTCbits.RC9
#define OUT_RELE_2          LATCbits.LATC8
#define READ_RELE_2         PORTCbits.RC8
#define OUT_RELE_3          LATCbits.LATC7
#define READ_RELE_3         PORTCbits.RC7
#define OUT_RELE_1          LATCbits.LATC6
#define READ_RELE_1         PORTCbits.RC6
// ----------------------------------------

#define GRADOS_POR_VUELTA       (double)359.9
#define RESOLUCION_ENCODER      360

#define RELACION_CAJA_1                         (double)25/1      // No modificar el (double) sino se pierde el valor pequeño de la relación
#define RELACION_CAJA_2                         (double)60/1      // No modificar el (double) sino se pierde el valor pequeño de la relación
#define RELACION_CAJA_3                         (double)60/7      // No modificar el (double) sino se pierde el valor pequeño de la relación
#define REDUCCION_ENCODER_ANTENA_ACIMUT         (1/(RELACION_CAJA_2*RELACION_CAJA_3))

typedef enum {
    ALL = 1,
    ACIMUT,
    ACIMUT_RIGHT,
    ACIMUT_LEFT,
    ELEVACION,
    ELEVACION_UP,
    ELEVACION_DOWN,
}OUT;

typedef struct{
    long encoderElev_Pulsos;
//    long encoderElev_Vueltas;
    long encoderAz_Pulsos;
//    long encoderAz_Vueltas;
    long anemometro;
}_Contador;

typedef struct{
    uint16_t encoderElev_A;
    uint16_t encoderElev_B;
    uint16_t encoderElev_Z;
    uint16_t encoderAz_A;
    uint16_t encoderAz_B;
    uint16_t encoderAz_Z;
    uint16_t anemometro;
    uint16_t home_stop_Elev;
    uint16_t home_stop_Az;
    uint16_t parada_emergencia;
}Last_Value;

/*===========================  Funciones Entradas   =========================*/
void initCN(void);

double get_Acimut(void);
double get_Elevacion(void);

void read_EncoderAz(void);
void read_EncoderElev(void);
uint8_t read_HS_Az(void);
uint8_t read_HS_Elev(void);
uint8_t read_Emergencia(void);

uint8_t set_Contador(long value, OUT direccion);

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

