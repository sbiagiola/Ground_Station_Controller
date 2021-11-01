/* 
 * File:   Datos_Motor.h
 * Author: Jere
 *
 * Created on 24 de junio de 2021, 10:24
 */

#ifndef DATOS_MOTOR_H
#define	DATOS_MOTOR_H

#ifdef	__cplusplus
extern "C" {
#endif

/*==================== [Macros y Definiciones] ========================*/

#define HIGH    1
#define LOW     0
    
#define ENCODER_1_A         PORTBbits.RB6
#define ENCODER_1_B         PORTBbits.RB7
#define ENCODER_1_Z         PORTBbits.RB8
    
#define ENCODER_2_A         PORTCbits.RC5
#define ENCODER_2_B         PORTCbits.RC4
#define ENCODER_2_Z         PORTCbits.RC3

#define HOME_STOP_1         PORTBbits.RB9
#define HOME_STOP_2         PORTBbits.RB5
    
#define PARADA_EMERGENCIA   PORTCbits.RC2
#define ANEMOMETRO          PORTAbits.RA4
    
typedef struct{
    long encoder_1_Pulsos;
    long encoder_1_Vueltas;
    int encoder_2_Pulsos;
    long encoder_2_Vueltas;
    int anemometro;
}_Contador;

typedef struct{
    uint16_t encoder_1_A;
    uint16_t encoder_1_B;
    uint16_t encoder_1_Z;
    uint16_t encoder_2_A;
    uint16_t encoder_2_B;
    uint16_t encoder_2_Z;
    uint16_t anemometro;
    uint16_t home_stop_1;
    uint16_t home_stop_2;
    uint16_t parada_emergencia;
}Last_Value;

/*========================================================================*/
    
/*===========================  Funciones   ===============================*/
void initCN(void);
void Config_CN_Pins(void);
long get_Acimut(void);
long get_Elevacion(void);
/*========================================================================*/
#ifdef	__cplusplus
}
#endif

#endif	/* DATOS_MOTOR_H */

