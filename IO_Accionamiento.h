
// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef IO_ACCIONAMIENTO_H
#define	IO_ACCIONAMIENTO_H

#include <xc.h> // include processor files - each processor file is guarded.  

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
    
#define OUT_RELE_1          LATCbits.LATC9
#define READ_RELE_1         PORTCbits.RC9
#define OUT_RELE_2          LATCbits.LATC8
#define READ_RELE_2         PORTCbits.RC8
#define OUT_RELE_3          LATCbits.LATC7
#define READ_RELE_3         PORTCbits.RC7
#define OUT_RELE_4          LATCbits.LATC6
#define READ_RELE_4         PORTCbits.RC6
// ----------------------------------------

#define HOME_ACIMUT             180
#define HOME_ELEVACION          90

#define DELAY_CAMBIO_SENTIDO    2*1000    // 2 segundos
#define TIMEOUT_COMANDO         (unsigned long)7200*1000 // 2 horas
#define TIMEOUT_TRACKING        (unsigned long)300*1000  // 5 minutos
#define TIMEOUT_MANUAL          30*1000   // 30 segundos

#define GRADOS_POR_VUELTA       (double)359.9
#define RESOLUCION_ENCODER      400

/* ========================================================================= */
//#define GRADOS_POR_VUELTA                       360
//
//#define RESOLUCION_ENCODER_ACIMUT               360
//#define RELACION_CAJA_1                         (double)25/1      // No modificar el (double) sino se pierde el valor pequeño de la relación
//#define RELACION_CAJA_2                         (double)60/1      // No modificar el (double) sino se pierde el valor pequeño de la relación
//#define RELACION_CAJA_3                         (double)60/7      // No modificar el (double) sino se pierde el valor pequeño de la relación
//#define REDUCCION_ACIMUT_COMPLETA               (1/(RELACION_CAJA_1*RELACION_CAJA_2*RELACION_CAJA_3))
//#define REDUCCION_ENCODER_ANTENA_ACIMUT         (1/(RELACION_CAJA_2*RELACION_CAJA_3))
//
//#define OFFSET_ANGULAR_ENCODER_ACIMUT           (REDUCCION_ENCODER_ANTENA_ACIMUT*(GRADOS_POR_VUELTA/RESOLUCION_ENCODER_ACIMUT))
//#define CANT_PULSOS_VUELTA_ENCODER              ((1/OFFSET_ANGULAR_ENCODER_ACIMUT))
//#define RESOLUCION_POR_PULSO_ACIMUT             (int)(1/OFFSET_ANGULAR_ENCODER_ACIMUT)
//
///* Hay que definir el valor de reducción de elevación para determinar el mínimo ángulo de giro*/
//#define RESOLUCION_ENCODER_ELEVACION            360
////#define REDUCCION_CAJA_4                      (double)            // Determinar por ensayos
//#define REDUCCION_CAJA_5                        (double)7/60        // No modificar el (double) sino se pierde el valor pequeño de la relación
//
//#define OFFSET_ANGULAR_ELEVACION                1                   //Nos queda así por la ubicación del encoder en el eje de la antena.
//#define RESOLUCION_POR_PULSO_ELEVACION          1
/* ========================================================================= */

/////////////////
#define HIGH    1
#define LOW     0
#define ON      1
#define OFF     0
/////////////////

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
    ACIMUT_RIGHT,
    ACIMUT_LEFT,
    ELEVACION,
    ELEVACION_UP,
    ELEVACION_DOWN,
}OUT;



/*===========================  Funciones Entradas   =========================*/
void initCN(void);

long get_Acimut(void);
long get_Elevacion(void);
/*========================================================================*/

/*===========================  Funciones Salidas   ==========================*/
void Stop(OUT);
void Move(OUT);

void Generar_Formato_Mensaje(char* Data_A_Enviar,uint8_t Id_Comando);
void MEF_Accionamiento(void);
void Control_Posicion_Acimut(void);
void Control_Posicion_Elevacion(void);
void MEF_Movimiento_Manual(void);
void Actualizar_Objetivos(void);

void Girar_Horario(void);
void Girar_Antihorario(void);
void Mov_Abajo(void);
void Mov_Arriba(void);
void Bajar_Salidas(void);
/*========================================================================*/



#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

