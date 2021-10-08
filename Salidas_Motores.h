/* 
 * File:   Salidas_Motores.h
 * Author: Jere
 *
 * Created on 24 de junio de 2021, 10:31
 */
#include "Entradas.h"
#include "Protocolo_Comm_Yaesu.h"   // Necesita saber el formato de _Contador

#ifndef SALIDAS_MOTORES_H
#define	SALIDAS_MOTORES_H

#ifdef	__cplusplus
extern "C" {
#endif
    


/*==================== [Macros y Definiciones] ========================*/    
#define LI1_Variador        LATBbits.LATB15
#define LI2_Variador        LATBbits.LATB14
#define LI3_Variador        LATAbits.LATA7
#define LI4_Variador        LATAbits.LATA10
    
#define OUT_RELE_1          LATCbits.LATC9
#define OUT_RELE_2          LATCbits.LATC8
#define OUT_RELE_3          LATCbits.LATC7
#define OUT_RELE_4          LATCbits.LATC6
    
#define ON 1
#define OFF 0

typedef struct{
    double Cero_Acimut;
    double Valor_Actual_Acimut;
    double Target_Acimut;
    double Ultimo_Ang_Acimut;
    double Cero_Elevacion;
    double Valor_Actual_Elevacion;
    double Target_Elevacion;
    double Ultimo_Ang_Elevacion;
}Struct_Data_Control;

typedef struct{
    uint8_t Actual;
    uint8_t Proximo;
    uint8_t Ultimo;
}Info_Comandos_Procesados;

typedef enum {
    ALL = 1,
    ACIMUT,
    ACIMUT_RIGTH,
    ACIMUT_LEFT,
    ELEVACION,
    ELEVACION_UP,
    ELEVACION_DOWN,
}OUT;

//typedef enum{
//    Stop = 1,
//    Tracking,
//    Movimiento_Manual,
//    Enviar_Posicion,
//    Cambio_Veloc_Elevacion,
//}Estado_MEF_Principal;

#define GRADOS_POR_VUELTA                       360

#define RESOLUCION_ENCODER_ACIMUT               360
#define RELACION_CAJA_1                         (double)25/1      // No modificar el (double) sino se pierde el valor pequeño de la relación
#define RELACION_CAJA_2                         (double)60/1      // No modificar el (double) sino se pierde el valor pequeño de la relación
#define RELACION_CAJA_3                         (double)60/7      // No modificar el (double) sino se pierde el valor pequeño de la relación
#define REDUCCION_ACIMUT_COMPLETA               (1/(RELACION_CAJA_1*RELACION_CAJA_2*RELACION_CAJA_3))
#define REDUCCION_ENCODER_ANTENA_ACIMUT         (1/(RELACION_CAJA_2*RELACION_CAJA_3))

#define OFFSET_ANGULAR_ENCODER_ACIMUT           (REDUCCION_ENCODER_ANTENA_ACIMUT*(GRADOS_POR_VUELTA/RESOLUCION_ENCODER_ACIMUT))
#define CANT_PULSOS_VUELTA_ENCODER              ((1/OFFSET_ANGULAR_ENCODER_ACIMUT))
#define RESOLUCION_POR_PULSO_ACIMUT             (int)(1/OFFSET_ANGULAR_ENCODER_ACIMUT)

/* Hay que definir el valor de reducción de elevación para determinar el mínimo ángulo de giro*/
#define RESOLUCION_ENCODER_ELEVACION            360
//#define REDUCCION_CAJA_4                      (double)            // Determinar por ensayos
#define REDUCCION_CAJA_5                        (double)7/60        // No modificar el (double) sino se pierde el valor pequeño de la relación

#define OFFSET_ANGULAR_ELEVACION                1                   //Nos queda así por la ubicación del encoder en el eje de la antena.
#define RESOLUCION_POR_PULSO_ELEVACION          1
/*========================================================================*/
    
/*===========================  Funciones   ===============================*/
void Stop(OUT);

void Generar_Formato_Mensaje(char* Data_A_Enviar,uint8_t Id_Comando);
void Calcular_Posicion_Actual(const _Contador* Data);
void MEF_Accionamiento(void);
void Control_Posicion_Acimut(void);
void Control_Posicion_Elevacion(void);
void MEF_Movimiento_Manual(void);
void Actualizar_Objetivos(uint8_t ID_Comando);

void Girar_Horario(void);
void Girar_Antihorario(void);
void Mov_Abajo(void);
void Mov_Arriba(void);
void Bajar_Salidas(void);
/*========================================================================*/
#ifdef	__cplusplus
}
#endif

#endif	/* SALIDAS_MOTORES_H */

