/* 
 * File:   Salidas_Motores.h
 * Author: Jere
 *
 * Created on 24 de junio de 2021, 10:31
 */
#include "Entradas.h"   // Necesita saber el formato de _Contador

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
    double Cero_Acimut;
    double Valor_Actual_Acimut;
    double Target_Acimut;
    double Ultimo_Ang_Acimut;
    double Cero_Elevacion;
    double Valor_Actual_Elevacion;
    double Target_Elevacion;
    double Ultimo_Ang_Elevacion;
}Data_Control;

typedef struct{
    uint8_t Comando_Actual;
    uint8_t Proximo_Comando;
}Info_Comandos_Procesados;

#define OFFSET_ANGULAR_ACIMUT 0.1
#define REDUCCION_ACIMUT_ENCODER   ((double)1/25)   // No modificar el (double) si se pierde el valor pequeño de relacion
#define REDUCCION_ACIMUT_COMPLETA ((double)7/90000) // No modificar el (double) si se pierde el valor pequeño de relacion
#define PULSOS_POR_VUELTA_ENCODER  360
#define RELACION_POR_VUELTA_ENCODER_ACIMUT  1/REDUCCION_ACIMUT_ENCODER
#define PULSOS_POR_VUELTA_ACIMUT RELACION_POR_VUELTA_ENCODER*PULSOS_POR_VUELTA_ENCODER

#define OFFSET_ANGULAR_ELEVACION 1
#define PULSOS_POR_VUELTA_ENCODER  360
#define RELACION_POR_VUELTA_ENCODER_ELEVACION  1
#define PULSOS_POR_VUELTA_ELEVACION RELACION_POR_VUELTA_ENCODER*PULSOS_POR_VUELTA_ENCODER
/*========================================================================*/
    
/*===========================  Funciones   ===============================*/
void Generar_Formato_Mensaje(char* Data_A_Enviar,uint8_t Id_Comando);
void Calculando_Posicion(const _Contador Data);
/*========================================================================*/
#ifdef	__cplusplus
}
#endif

#endif	/* SALIDAS_MOTORES_H */

