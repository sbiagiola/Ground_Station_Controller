/*
 * File:   IO_basic.c
 * Author: Gustavo
 *
 * Created on 17 de abril de 2023, 18:43
 */


#include "xc.h"
#include "IO_Basic.h"
#include "IO_Accionamiento.h"

Last_Value valor_anterior;
_Contador contador;

uint8_t stateEncoder_AZ = 0;
uint8_t stateEncoder_EL = 0;

void initCN()
{
    valor_anterior.encoderElev_A = ENCODER_ELEV_A;
    valor_anterior.encoderElev_B = ENCODER_ELEV_B;
    valor_anterior.encoderElev_Z = ENCODER_ELEV_Z;
    
    valor_anterior.encoderAz_A = ENCODER_AZ_A;
    valor_anterior.encoderAz_B = ENCODER_AZ_B;
    valor_anterior.encoderAz_Z = ENCODER_AZ_Z;
    
    valor_anterior.anemometro = ANEMOMETRO;
    valor_anterior.parada_emergencia = PARADA_EMERGENCIA;
    
    valor_anterior.home_stop_Elev = HOME_STOP_ELEV;
    valor_anterior.home_stop_Az = HOME_STOP_AZ;
    
    contador.encoderAz_Pulsos = 92571;
    contador.encoderElev_Pulsos = 90;
}

double get_Acimut(void){
    double ang = (contador.encoderAz_Pulsos * GRADOS_POR_VUELTA) / RESOLUCION_ENCODER;
    ang = ang*REDUCCION_ENCODER_ANTENA_ACIMUT;
    return ang;
}

double get_Elevacion(void){
    double ang = (contador.encoderElev_Pulsos * GRADOS_POR_VUELTA) / RESOLUCION_ENCODER;
    return  ang;
}

void read_EncoderAz(void)
{
    if(ENCODER_AZ_A != valor_anterior.encoderAz_A)
    {
        if(ENCODER_AZ_A == HIGH)
        {
            if(stateEncoder_AZ == 0)
            {
                stateEncoder_AZ = 1;
            }
            else 
            {
                stateEncoder_AZ = 2;
            }
        }
        else
        {
            if(stateEncoder_AZ == 2)
            {
                stateEncoder_AZ = 3;
            }
            else
            {
                if(stateEncoder_AZ == 3)
                {
                    // pulso
                    contador.encoderAz_Pulsos--;
                }
                else
                {
                    // RUIDO. Se descarta el pulso
                }
                stateEncoder_AZ = 0;
            }
        }
        valor_anterior.encoderAz_A = ENCODER_AZ_A;
    }
    
    if(ENCODER_AZ_B != valor_anterior.encoderAz_B)
    {
        if(ENCODER_AZ_B == HIGH)
        {
            if(stateEncoder_AZ == 1)
            {
                stateEncoder_AZ = 2;
            }
            else 
            {
                if(stateEncoder_AZ == 0)
                {
                    stateEncoder_AZ = 1;
                }
                else
                {
                    stateEncoder_AZ = 2;
                }
            }
        }
        else
        {
            if(stateEncoder_AZ == 3)
            {
                contador.encoderAz_Pulsos++;
                stateEncoder_AZ = 0;
            }
            else
            {
                if(stateEncoder_AZ == 2)
                {
                    stateEncoder_AZ = 3;
                }
                else
                {
                    // RUIDO. Se descarta el pulso
                    stateEncoder_AZ = 0;
                }
            }
        }
        valor_anterior.encoderAz_B = ENCODER_AZ_B;
    }
}

void read_EncoderElev(void)
{
    if(ENCODER_ELEV_A != valor_anterior.encoderElev_A)
    {
        valor_anterior.encoderElev_A = ENCODER_ELEV_A;
        if(ENCODER_ELEV_A == HIGH)
        {
            switch(stateEncoder_EL)
            {
                case 0:
                    stateEncoder_EL = 1;
                    break;

                case 1:
                    stateEncoder_EL = 2;
                    break;

                case 2:
                case 3:
                    stateEncoder_EL = 0;
                    break;
            }
        }
        else if(ENCODER_ELEV_A == LOW)
        {
            switch(stateEncoder_EL)
            {
                case 0:
                case 1:
                    stateEncoder_EL = 0;
                    break;

                case 2:
                    stateEncoder_EL = 3;
                    break;

                case 3:
                    // pulso
                    contador.encoderElev_Pulsos++;
                    stateEncoder_EL = 0;
                    break;
            }
        }
    }

    if(ENCODER_ELEV_B != valor_anterior.encoderElev_B)
    {
        valor_anterior.encoderElev_B = ENCODER_ELEV_B;
        if(ENCODER_ELEV_B == HIGH)
        {
            switch(stateEncoder_EL)
            {
                case 0:
                    stateEncoder_EL = 1;
                    break;

                case 1:
                    stateEncoder_EL = 2;
                    break;

                case 2:
                case 3:
                    stateEncoder_EL = 0;
                    break;
            }
        }
        else if(ENCODER_ELEV_B == LOW)
        {
            switch(stateEncoder_EL)
            {
                case 0:
                case 1:
                    stateEncoder_EL = 0;
                    break;

                case 2:
                    stateEncoder_EL = 3;
                    break;

                case 3:
                    // pulso
                    contador.encoderElev_Pulsos--;
                    stateEncoder_EL = 0;
                    break;
            }
        }
    }
}

uint8_t read_HS_Az()
{
    if(HOME_STOP_AZ != valor_anterior.home_stop_Az){
        valor_anterior.home_stop_Az = HOME_STOP_AZ;
        
        if(HOME_STOP_AZ == HIGH && READ_RELE_2 == ON){
            return 1;
        }
    }
    return 0;
}

uint8_t read_HS_Elev()
{
    if(HOME_STOP_ELEV != valor_anterior.home_stop_Elev){
        if(HOME_STOP_ELEV == HIGH) {
            return 1;
        }
        valor_anterior.home_stop_Elev = HOME_STOP_ELEV;
    }
    return 0;
}

uint8_t read_Emergencia()
{
    if(PARADA_EMERGENCIA != valor_anterior.parada_emergencia){
        valor_anterior.parada_emergencia = PARADA_EMERGENCIA;
        if(PARADA_EMERGENCIA == HIGH)
            return 1;
    }
    return 0;
}

uint8_t set_Contador(long value, OUT direccion)
{
    if(direccion == ACIMUT) contador.encoderAz_Pulsos = value;
    else if(direccion == ELEVACION) contador.encoderElev_Pulsos = value;
    else return 0;
    
    return 1;
}
