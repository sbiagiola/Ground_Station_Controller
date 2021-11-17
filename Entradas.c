/* 
 * File:   Datos_Motor
 * Author: Jere
 *
 * Created on 24 de junio de 2021, 10:20
 */
#include <xc.h>
#include "Entradas.h"
#include "stdint.h"
#include "Salidas_Motores.h"
#include "Protocolo_Comm_Yaesu.h"
#include "UART.h"

typedef enum{
    Resta = 0,
    Suma,
}Operacion_Pulsos;  //No es triunfo pero paso cerca...

/*===================== [Variables Internas (Globales)] =====================*/
Last_Value valor_anterior;
_Contador contador;


static Operacion_Pulsos estado_Operacion_Encoder_1;
static Operacion_Pulsos estado_Operacion_Encoder_2;
static uint8_t Bandera_Home_Stop_1 = 1;
static uint8_t Bandera_Home_Stop_2 = 1;
static uint8_t Bandera_Parad_Emerg = 0;
/*===========================================================================*/

/*===================== [Variables Externas (Globales)] =====================*/
extern Info_Comandos_Procesados Comando_Procesado;
// [TO DO] Para que usa esta flag ???
extern uint8_t Flag_Parada_Emergencia;
/*===========================================================================*/

void initCN()
{
    valor_anterior.encoder_1_A = ENCODER_1_A;
    valor_anterior.encoder_1_B = ENCODER_1_B;
    valor_anterior.encoder_1_Z = ENCODER_1_Z;
    
    valor_anterior.encoder_2_A = ENCODER_2_A;
    valor_anterior.encoder_2_B = ENCODER_2_B;
    valor_anterior.encoder_2_Z = ENCODER_2_Z;
    
    valor_anterior.anemometro = ANEMOMETRO;
    valor_anterior.parada_emergencia = PARADA_EMERGENCIA;
    
    valor_anterior.home_stop_1 = HOME_STOP_1;
    valor_anterior.home_stop_2 = HOME_STOP_2;
    
    contador.encoder_2_Pulsos = 50;
}

void Config_CN_Pins(){
    CNEN1bits.CN10IE = 1;   // Enable CN10 pin for interrupt detection  RC2 (PARADA EMERG)
    CNEN2bits.CN21IE = 1;   // Enable CN22 pin for interrupt detection  RB9 (ANEMOMETRO)

    CNEN2bits.CN22IE = 1;   // Enable CN22 pin for interrupt detection  RB8 (FASE A ENCODER 1)
    CNEN2bits.CN23IE = 1;   // Enable CN23 pin for interrupt detection  RB7 (FASE B ENCODER 1)
    CNEN2bits.CN24IE = 1;   // Enable CN24 pin for interrupt detection  RB6 (FASE Z ENCODER 1)

    CNEN2bits.CN27IE = 1;   // Enable CN27 pin for interrupt detection  RB5 (END/STOP 2)

    CNEN2bits.CN25IE = 1;   // Enable CN25 pin for interrupt detection  RC4 (FASE B ENCODER 2)
    CNEN2bits.CN26IE = 1;   // Enable CN26 pin for interrupt detection  RC5 (FASE Z ENCODER 2)
    CNEN2bits.CN28IE = 1;   // Enable CN28 pin for interrupt detection  RC3 (FASE A ENCODER 2)

    IFS1bits.CNIF = 0;      // Reset CN interrupt   (Recomendaban esto)|
}

void __attribute__((interrupt,no_auto_psv)) _CNInterrupt(void){
    
    /* ---------------------------   ENCODER 1   --------------------------- */
    // Contador de pulsos
    if((ENCODER_1_A != valor_anterior.encoder_1_A)) {
        if(ENCODER_1_A == HIGH) {
            if(ENCODER_1_B == HIGH) {
                putrsUART2("[CNInterrupt]: Encoder 1 (fase B)\n\r");
                contador.encoder_1_Pulsos++;
                estado_Operacion_Encoder_1 = Suma;
            } else {
                putrsUART2("[CNInterrupt]: Encoder 1 (fase A)\n\r");
                contador.encoder_1_Pulsos--;
                estado_Operacion_Encoder_1 = Resta;
            }
        }
        valor_anterior.encoder_1_A = ENCODER_1_A;
    }
    
    // Contador de vueltas
    // [TO DO] Esto no sirve para nada en nuestro proyecto, se deja o se saca?
    if(ENCODER_1_Z != valor_anterior.encoder_1_Z){
        if(ENCODER_1_Z == HIGH){
            if(estado_Operacion_Encoder_1 == Suma){
                contador.encoder_1_Vueltas++;
            } else {
                contador.encoder_1_Vueltas--;
            }
        }
        valor_anterior.encoder_1_Z = ENCODER_1_Z;
    }
    
    /* ---------------------------   ENCODER 2   --------------------------- */
    // Contador de pulsos
    if((ENCODER_2_A != valor_anterior.encoder_2_A)) {
        if(ENCODER_2_A == HIGH) {
            if(ENCODER_2_B == HIGH) {
                putrsUART2("[CNInterrupt]: Encoder 2 (fase B)\n\r");                
                contador.encoder_2_Pulsos++;
                estado_Operacion_Encoder_2 = Suma;
            } else {
                putrsUART2("[CNInterrupt]: Encoder 2 (fase A)\n\r");
                contador.encoder_2_Pulsos--;
                estado_Operacion_Encoder_2 = Resta;
            }
        }
        valor_anterior.encoder_2_A = ENCODER_2_A;
    }
    
    // Contador de vueltas
    // [TO DO] Esto no sirve para nada en nuestro proyecto, se deja o se saca?
    if(ENCODER_2_Z != valor_anterior.encoder_2_Z){
        if(ENCODER_2_Z == HIGH){
            if(estado_Operacion_Encoder_2 == Suma){
                contador.encoder_2_Vueltas++;
            } else {
                contador.encoder_2_Vueltas--;
            }
        }
        valor_anterior.encoder_2_Z = ENCODER_2_Z;
    }
    
    /* --------------------------   HOME STOP 1   -------------------------- */
    if(HOME_STOP_1 != valor_anterior.home_stop_1){
        putrsUART2("[_CNInterrupt] Home_Stop_1 interrupt detected!");
        
        // [TO DO] Para que es este if?
        if(HOME_STOP_1 == HIGH && Bandera_Home_Stop_1 == 1){
            //Seteo de posicion de reposo de alguna manera
            Bandera_Home_Stop_1 = 0;
        }
        
        if(HOME_STOP_1 == HIGH && Bandera_Home_Stop_1 == 0)
            Stop(ACIMUT);
        
        valor_anterior.home_stop_1 = HOME_STOP_1;
    }
    
    /* --------------------------   HOME STOP 2   -------------------------- */
    if(HOME_STOP_2 != valor_anterior.home_stop_2){
        putrsUART2("[_CNInterrupt] Home_Stop_2 interrupt detected!");
        
        // [TO DO] Para que es este if?
        if(HOME_STOP_2 == HIGH && Bandera_Home_Stop_2 == 1){
            //Seteo de posicion de reposo de alguna manera
            Bandera_Home_Stop_2 = 0;
        }
        
        if(HOME_STOP_2 == HIGH && Bandera_Home_Stop_2 == 0)
            Stop(ELEVACION);

        valor_anterior.home_stop_2 = HOME_STOP_2;
    }
    
    /* ----------------------   PARADA DE EMERGENCIA   --------------------- */
    // [TO DO] Cambiar comando por stop directo ??
    if(PARADA_EMERGENCIA != valor_anterior.parada_emergencia){
        putrsUART2("[_CNInterrupt] Parada_Emergencia interrupt detected!");
        if(PARADA_EMERGENCIA == LOW && Bandera_Parad_Emerg == 0){
            Bandera_Parad_Emerg = 1;
            Flag_Parada_Emergencia = Bandera_Parad_Emerg;
            Comando_Procesado.Proximo = Comando_Procesado.Actual;
            Comando_Procesado.Actual = Stop_Global;
            putrsUART2("PE encendida!\n\r");
        }
        else if(PARADA_EMERGENCIA == LOW && Bandera_Parad_Emerg == 1){
            Bandera_Parad_Emerg = 0;
            Flag_Parada_Emergencia = Bandera_Parad_Emerg;
            putrsUART2("PE apagada!\n\r");
        }
        valor_anterior.parada_emergencia = PARADA_EMERGENCIA;
    }
    
    IFS1bits.CNIF = 0; // Clear CN interrupt
}

long get_Acimut(void){
    return contador.encoder_1_Pulsos;
}

long get_Elevacion(void){
    return  contador.encoder_2_Pulsos;
}