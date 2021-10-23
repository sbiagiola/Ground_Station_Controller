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
Last_Value Valor_Anterior;
_Contador Contador;

static Operacion_Pulsos Estado_Operacion_Encoder_1;
static Operacion_Pulsos Estado_Operacion_Encoder_2;
static uint8_t Bandera_Encoder_1_A = 1;
static uint8_t Bandera_Encoder_1_B = 1;
static uint8_t Bandera_Encoder_2_A = 1;
static uint8_t Bandera_Encoder_2_B = 1;
static uint8_t Bandera_Home_Stop_1 = 1;
static uint8_t Bandera_Home_Stop_2 = 1;
static uint8_t Bandera_Parad_Emerg = 0;
/*===========================================================================*/

/*===================== [Variables Externas (Globales)] =====================*/
extern Info_Comandos_Procesados Comando_Procesado;
extern uint8_t Flag_Parada_Emergencia;
/*===========================================================================*/

void initCN()
{
    Valor_Anterior.Encoder_1_A = Enconder_1_Fase_A;
    Valor_Anterior.Encoder_1_B = Enconder_1_Fase_B;
    Valor_Anterior.Encoder_1_Z = Enconder_1_Fase_Z;
    
    Valor_Anterior.Encoder_2_A = Enconder_2_Fase_A;
    Valor_Anterior.Encoder_2_B = Enconder_2_Fase_B;
    Valor_Anterior.Encoder_2_Z = Enconder_2_Fase_Z;
    
    Valor_Anterior.Anemometr0 = Anemometro;
    Valor_Anterior.Parad_Emerg = Parada_Emergencia;
    
    Valor_Anterior.Home_St0p_1 = Home_Stop_1;
    Valor_Anterior.Home_St0p_2 = Home_Stop_2;
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
    
    if( (Enconder_1_Fase_A != Valor_Anterior.Encoder_1_A) || (Enconder_1_Fase_B != Valor_Anterior.Encoder_1_B) ){

        if(Enconder_1_Fase_A == LOW && Enconder_1_Fase_B == HIGH && Enconder_1_Fase_Z == LOW && Bandera_Encoder_1_B == 1){
            //putrsUART2("Entro 1\n\r");
            Bandera_Encoder_1_A = 0;
            Bandera_Encoder_1_B = 1;
        }
        
        if(Enconder_1_Fase_A == HIGH && Enconder_1_Fase_B == LOW && Enconder_1_Fase_Z == LOW && Bandera_Encoder_1_A == 1){
            //putrsUART2("Entro 2\n\r");
            Bandera_Encoder_1_A = 1;
            Bandera_Encoder_1_B = 0;
        }
        
        if(Enconder_1_Fase_A == HIGH && Enconder_1_Fase_B == LOW && Enconder_1_Fase_Z == LOW && Bandera_Encoder_1_A == 1){
            Contador.Encoder_1_Pulsos++;
            Estado_Operacion_Encoder_1 = Suma;
        }

        if(Enconder_1_Fase_A == LOW && Enconder_1_Fase_B == HIGH && Enconder_1_Fase_Z == LOW && Bandera_Encoder_1_B == 1){
            Contador.Encoder_1_Pulsos--;
            Estado_Operacion_Encoder_1 = Resta;
        }
        
        if(Enconder_1_Fase_A != Valor_Anterior.Encoder_1_A){
            Enconder_1_Fase_A = Valor_Anterior.Encoder_1_A;
        }
        
        if(Enconder_1_Fase_B != Valor_Anterior.Encoder_1_B){
            Enconder_1_Fase_B = Valor_Anterior.Encoder_1_B;
        } 
    }
    
    if(Enconder_1_Fase_Z != Valor_Anterior.Encoder_1_Z){
        
        if(Enconder_1_Fase_Z == HIGH){
            if(Estado_Operacion_Encoder_1 == Suma){
                Contador.Encoder_1_Vueltas++;
            }
            if(Estado_Operacion_Encoder_1 == Resta){
                Contador.Encoder_1_Vueltas--;
            }
            Contador.Encoder_1_Pulsos = 0;
        }
        Enconder_1_Fase_Z = Valor_Anterior.Encoder_1_Z;
    }

    if((Enconder_2_Fase_A != Valor_Anterior.Encoder_2_A) || (Enconder_2_Fase_B != Valor_Anterior.Encoder_2_B)){
        
        if(Enconder_2_Fase_A == LOW && Enconder_2_Fase_B == HIGH && Enconder_2_Fase_Z == LOW && Bandera_Encoder_2_B == 1){
            putrsUART2("[Encoder 2] - Entramos a la parte de flag 2, fase B \n\r");
            Bandera_Encoder_2_A =  0;
            Bandera_Encoder_2_B = 1;
        }
        
        if(Enconder_2_Fase_A == HIGH && Enconder_2_Fase_B == LOW && Enconder_2_Fase_Z == LOW && Bandera_Encoder_2_A == 1){
            putrsUART2("[Encoder 2] - Entramos a la parte de flag 2, fase A \n\r");
            Bandera_Encoder_2_A = 1;
            Bandera_Encoder_2_B = 0;
        }
        
        if(Enconder_2_Fase_A == HIGH && Enconder_2_Fase_B == LOW && Enconder_2_Fase_Z == LOW && Bandera_Encoder_2_A == 1){
            putrsUART2("[Encoder 2] - Sumamo' un pulso' \n\r");
            
            Contador.Encoder_2_Pulsos++;
            Estado_Operacion_Encoder_2 = Suma;
        }

        if(Enconder_2_Fase_A == LOW && Enconder_2_Fase_B == HIGH && Enconder_2_Fase_Z == LOW && Bandera_Encoder_2_B == 1){
            putrsUART2("[Encoder 2] - Restamo' un pulso' \n\r");
            Contador.Encoder_2_Pulsos--;
            Estado_Operacion_Encoder_2 = Resta;
        }
        
        if(Enconder_2_Fase_A != Valor_Anterior.Encoder_2_A){
            Enconder_2_Fase_A = Valor_Anterior.Encoder_2_A;
        }
        
        if(Enconder_2_Fase_B != Valor_Anterior.Encoder_2_B){
            Enconder_2_Fase_B = Valor_Anterior.Encoder_2_B;
        } 
    }
    
    if(Enconder_2_Fase_Z != Valor_Anterior.Encoder_2_Z){
        
        if(Enconder_2_Fase_Z == HIGH){
            
            if(Estado_Operacion_Encoder_2 == Suma){
                Contador.Encoder_2_Vueltas++;
            }
            if(Estado_Operacion_Encoder_2 == Resta){
                Contador.Encoder_2_Vueltas--;
            }
            Contador.Encoder_2_Pulsos = 0;
        }
        Enconder_2_Fase_Z = Valor_Anterior.Encoder_2_Z;
    }
    
    // ---------- HOME STOP 1
    if(Home_Stop_1 != Valor_Anterior.Home_St0p_1){
        putrsUART2("[_CNInterrupt] Home_Stop_1 interrupt detected!");
        
        // [TO DO] Para que es este if?
        if(Home_Stop_1 == HIGH && Bandera_Home_Stop_1 == 1){
            //Seteo de posicion de reposo de alguna manera
            Bandera_Home_Stop_1 = 0;
        }
        
        if(Home_Stop_1 == HIGH && Bandera_Home_Stop_1 == 0)
            Stop(ACIMUT);
        
        Home_Stop_1 = Valor_Anterior.Home_St0p_1;
    }
    
    // ---------- HOME STOP 2
    if(Home_Stop_2 != Valor_Anterior.Home_St0p_2){
        putrsUART2("[_CNInterrupt] Home_Stop_2 interrupt detected!");
        
        // [TO DO] Para que es este if?
        if(Home_Stop_2 == HIGH && Bandera_Home_Stop_2 == 1){
            //Seteo de posicion de reposo de alguna manera
            Bandera_Home_Stop_2 = 0;
        }
        
        if(Home_Stop_2 == HIGH && Bandera_Home_Stop_2 == 0)
            Stop(ELEVACION);

        Home_Stop_2 = Valor_Anterior.Home_St0p_2;
    }
    
    // ---------- PARADA DE EMERGENCIA
    // [TO DO] Cambiar comando por stop directo
    if(Parada_Emergencia != Valor_Anterior.Parad_Emerg){
        putrsUART2("[_CNInterrupt] Parada_Emergencia interrupt detected!");
        if(Parada_Emergencia == LOW && Bandera_Parad_Emerg == 0){
            Bandera_Parad_Emerg = 1;
            Flag_Parada_Emergencia = Bandera_Parad_Emerg;
            Comando_Procesado.Proximo = Comando_Procesado.Actual;
            Comando_Procesado.Actual = Stop_Global;
            putrsUART2("PE encendida!\n\r");
        }
        else if(Parada_Emergencia == LOW && Bandera_Parad_Emerg == 1){
            Bandera_Parad_Emerg = 0;
            Flag_Parada_Emergencia = Bandera_Parad_Emerg;
            putrsUART2("PE apagada!\n\r");
        }
        Valor_Anterior.Parad_Emerg = Parada_Emergencia;
    }
    
    IFS1bits.CNIF = 0; // Clear CN interrupt
}

/*
 * Considero enconder 1 acimut
 */
long posicion_actual_acimut(void){
    return Contador.Encoder_1_Pulsos;
}

/*
 * Considero enconder 2 elevacion y la relax}cion lineal
 */
long posicion_actual_elevacion(void){
    return  Contador.Encoder_2_Pulsos;
}