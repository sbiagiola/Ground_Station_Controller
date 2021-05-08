/* 
 * File:   ADC.h
 * Author: Jere
 *
 * Created on 26 de abril de 2021, 15:46
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

/***************  Definiciones   **************/   
/**********************************************/
        
/****************  Funciones   ****************/         
void Config_ADC(void);
uint16_t Get_Data(void);
/**********************************************/

#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

