
// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef IO_ACCIONAMIENTO_H
#define	IO_ACCIONAMIENTO_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define HOME_ACIMUT             340
#define HOME_ELEVACION          90

#define DELAY_CAMBIO_SENTIDO    5*1000    // 2 segundos
#define TIMEOUT_COMANDO         (unsigned long)7200*1000 // 2 horas
#define TIMEOUT_TRACKING        (unsigned long)600*1000  // 5 minutos
#define TIMEOUT_MANUAL          (unsigned long)60*1000   // 60 segundos
#define TIMEOUT_INIT_AZ         (unsigned long)900*1000 // 15 minutos
#define TIMEOUT_INIT_ELEV       (unsigned long)300*1000 // 15 minutos

#define POSICION_HS_AZ          515 // grados
#define POSICION_HS_ELEV        40  // grados

#define OFFSET_ANGULAR_ELEVACION                0.5                   //Nos queda así por la ubicación del encoder en el eje de la antena.
#define OFFSET_ANGULAR_ACIMUT                   0.2                   //Nos queda así por la ubicación del encoder en el eje de la antena.

/* ========================================================================= */

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

/*========================================================================*/

/*===========================  Funciones Salidas   ==========================*/
void Stop(OUT);
void Move(OUT);

uint8_t Tracking(double, double);

void MEF_Accionamiento(void);
void Actualizar_Objetivos(void);

void verificar_HS(void);
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

