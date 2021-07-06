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

typedef enum{
    Resta = 0,
    Suma,
}Operacion_Pulsos;  //No es triunfo pero paso cerca...

/*===================== [Variables Internas (Globales)] =====================*/ 
Last_Value Valor_Anterior;
_Contador Contador;

static Operacion_Pulsos Estado_Operacion;
static uint8_t Bandera_Encoder_1_A = 1;
static uint8_t Bandera_Encoder_1_B = 1;
static uint8_t Bandera_Encoder_2_A = 1;
static uint8_t Bandera_Encoder_2_B = 1;
static uint8_t Bandera_Home_Stop_2 = 1;
static uint8_t Bandera_Parad_Emerg = 1;
/*===========================================================================*/

/*===================== [Variables Externas (Globales)] =====================*/
extern Info_Comandos_Procesados Comando_Procesado;
extern uint8_t Flag_Bloqueo_Actualizacion;
/*===========================================================================*/

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
            Bandera_Encoder_1_A = 0;
            Bandera_Encoder_1_B = 1;
        }
        
        if(Enconder_1_Fase_A == HIGH && Enconder_1_Fase_B == LOW && Enconder_1_Fase_Z == LOW && Bandera_Encoder_1_A == 1){
            Bandera_Encoder_1_A = 1;
            Bandera_Encoder_1_B = 0;
        }
        
        if(Enconder_1_Fase_A == HIGH && Enconder_1_Fase_B == LOW && Enconder_1_Fase_Z == LOW && Bandera_Encoder_1_A == 1){
            Contador.Encoder_1_Pulsos++;
            Estado_Operacion = Suma;
        }

        if(Enconder_1_Fase_A == LOW && Enconder_1_Fase_B == HIGH && Enconder_1_Fase_Z == LOW && Bandera_Encoder_1_B == 1){
            Contador.Encoder_1_Pulsos--;
            Estado_Operacion = Resta;
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
            if(Estado_Operacion == Suma){
                Contador.Encoder_1_Vueltas++;
            }
            if(Estado_Operacion == Resta){
                Contador.Encoder_1_Vueltas--;
            }
            Contador.Encoder_1_Pulsos = 0;
        }
        Enconder_1_Fase_Z = Valor_Anterior.Encoder_1_Z;
    }

    if((Enconder_2_Fase_A != Valor_Anterior.Encoder_2_A) || (Enconder_2_Fase_B != Valor_Anterior.Encoder_2_B)){
        
        if(Enconder_2_Fase_A == LOW && Enconder_2_Fase_B == HIGH && Enconder_2_Fase_Z == LOW && Bandera_Encoder_2_B == 1){
            Bandera_Encoder_2_A = 0;
            Bandera_Encoder_2_B = 1;
        }
        
        if(Enconder_2_Fase_A == HIGH && Enconder_2_Fase_B == LOW && Enconder_2_Fase_Z == LOW && Bandera_Encoder_2_A == 1){
            Bandera_Encoder_2_A = 1;
            Bandera_Encoder_2_B = 0;
        }
        
        if(Enconder_2_Fase_A == HIGH && Enconder_2_Fase_B == LOW && Enconder_2_Fase_Z == LOW && Bandera_Encoder_2_A == 1){
            Contador.Encoder_2_Pulsos++;
            Estado_Operacion = Suma;
        }

        if(Enconder_2_Fase_A == LOW && Enconder_2_Fase_B == HIGH && Enconder_2_Fase_Z == LOW && Bandera_Encoder_2_B == 1){
            Contador.Encoder_2_Pulsos--;
            Estado_Operacion = Resta;
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
            
            if(Estado_Operacion == Suma){
                Contador.Encoder_2_Vueltas++;
            }
            if(Estado_Operacion == Resta){
                Contador.Encoder_2_Vueltas--;
            }
            Contador.Encoder_2_Pulsos = 0;
        }
        Enconder_2_Fase_Z = Valor_Anterior.Encoder_2_Z;
    }
    
    if(Anemometro != Valor_Anterior.Anemometr0){
        if(Anemometro == HIGH){
            Contador.Anemometr0++;
            //Falta tener el anemometro y definir un número máximo
        }
        Anemometro = Valor_Anterior.Anemometr0;
    }
    /*  Esta parte nunca va a darse dado que el puerto A no tiene la capacidad de CN.
    
    if(Home_Stop_1 != Valor_Anterior.Home_St0p_1){
        if(Home_Stop_1 == HIGH && Bandera_Home_Stop_1 == 1){
            //Seteo de posicion de reposo de alguna manera
            Bandera_Home_Stop_1 = 0;
        }
        if(Home_Stop_1 == HIGH && Bandera_Home_Stop_1 == 0){
            //Definir acciones
        }
        Home_Stop_1 = Valor_Anterior.Home_St0p_1;
    }
    */
    if(Home_Stop_2 != Valor_Anterior.Home_St0p_2){
        if(Home_Stop_2 == HIGH && Bandera_Home_Stop_2 == 1){
            //Seteo de posicion de reposo de alguna manera
            Bandera_Home_Stop_2 = 0;
        }
        if(Home_Stop_2 == HIGH && Bandera_Home_Stop_2 == 0){
            //Definir acciones
        }
        Home_Stop_2 = Valor_Anterior.Home_St0p_2;
    }
    
    if(Parada_Emergencia != Valor_Anterior.Parad_Emerg){
        if(Parada_Emergencia == HIGH && Bandera_Parad_Emerg == 1){
            Bandera_Parad_Emerg = 0;
            Flag_Bloqueo_Actualizacion = Bandera_Parad_Emerg;
            Comando_Procesado.Proximo = Comando_Procesado.Actual;
            Comando_Procesado.Actual = Parar_Todo;
        }
        if(Parada_Emergencia == HIGH && Bandera_Parad_Emerg == 0){
            Bandera_Parad_Emerg = 1;
            Flag_Bloqueo_Actualizacion = Bandera_Parad_Emerg;
        }
        Parada_Emergencia = Valor_Anterior.Parad_Emerg;
    }
    
IFS1bits.CNIF = 0; // Clear CN interrupt
}
