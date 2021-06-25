/* 
 * File:   Salidas_Motores.h
 * Author: Jere
 *
 * Created on 24 de junio de 2021, 10:31
 */

#ifndef SALIDAS_MOTORES_H
#define	SALIDAS_MOTORES_H

#ifdef	__cplusplus
extern "C" {
#endif

/*==================== [Macros y Definiciones] ========================*/    
#define LI1_Variador        PORTBbits.RB15
#define LI2_Variador        PORTBbits.RB14
#define LI3_Variador        PORTAbits.RA7
#define LI4_Variador        PORTAbits.RA10
    
#define OUT_RELE_1          PORTCbits.RC9
#define OUT_RELE_2          PORTCbits.RC8
#define OUT_RELE_3          PORTCbits.RC7
#define OUT_RELE_4          PORTCbits.RC6
    
typedef struct{
    float Cero_Acimut;
    float Target_Acimut;
    float Ultimo_Ang_Acimut;
    float Cero_Elevacion;
    float Target_Elevacion;
    float Ultimo_Ang_Elevacion;
}Data_Control;

typedef struct{
    uint8_t Comando_Actual;
    uint8_t Proximo_Comando;
}Info_Comandos_Procesados;

#define OFFSET_ANGULAR 0.1
#define REDUCCION_ACIMUT   7/90000
#define PULSOS_POR_VUELTA_ENCODER  360
#define RELACION_POR_VUELTA_ACIMUT  1/REDUCCION_ACIMUT
#define PULSOS_POR_VUELTA_ACIMUT RELACION_POR_VUELTA_ACIMUT*PULSOS_POR_VUELTA_ENCODER
/*========================================================================*/
    
/*===========================  Funciones   ===============================*/
void Generar_Formato_Mensaje(uint8_t* data,uint8_t Id_Comando);
void Calculando_Posicion(void);
/*========================================================================*/
#ifdef	__cplusplus
}
#endif

#endif	/* SALIDAS_MOTORES_H */

