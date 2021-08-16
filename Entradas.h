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
#define Enconder_1_Fase_A   PORTBbits.RB6
#define Enconder_1_Fase_B   PORTBbits.RB7
#define Enconder_1_Fase_Z   PORTBbits.RB8
    
#define Enconder_2_Fase_A   PORTCbits.RC3
#define Enconder_2_Fase_B   PORTCbits.RC4
#define Enconder_2_Fase_Z   PORTCbits.RC5
    
#define Parada_Emergencia   PORTCbits.RC2
#define Anemometro          PORTBbits.RB9
    
#define Home_Stop_2         PORTBbits.RB5
#define Home_Stop_1         PORTAbits.RA4
    
typedef struct{
    long Encoder_1_Pulsos;
    long Encoder_1_Vueltas;  
    long Encoder_2_Pulsos;
    long Encoder_2_Vueltas;
    int Anemometr0;
}_Contador;

typedef struct{
    uint16_t    Encoder_1_A;
    uint16_t    Encoder_1_B;
    uint16_t    Encoder_1_Z;
    uint16_t    Encoder_2_A;
    uint16_t    Encoder_2_B;
    uint16_t    Encoder_2_Z;
    uint16_t    Anemometr0;
    uint16_t    Home_St0p_1;
    uint16_t    Home_St0p_2;
    uint16_t    Parad_Emerg;
}Last_Value;

#define HIGH 1
#define LOW 0
/*========================================================================*/
    
/*===========================  Funciones   ===============================*/
void initCN(void);
void Config_CN_Pins(void);
long posicion_actual_acimut(void);
long posicion_actual_elevacion(void);
/*========================================================================*/
#ifdef	__cplusplus
}
#endif

#endif	/* DATOS_MOTOR_H */

