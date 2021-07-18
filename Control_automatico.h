/* 
 * File:   Control_automatico.h
 * Author: biagi
 *
 * Created on 18 de julio de 2021, 13:12
 */

#ifndef CONTROL_AUTOMATICO_H
#define	CONTROL_AUTOMATICO_H

/*
 * definiciones
*/
#define RESET            00
#define CARGANDO_VAR     01
#define ACTUANDO         02
#define POS_ALCANZADA    03
#define POS_FAILED       04   


/*
 * Funciones
 */
void inicia_control( uint32_t *Pos_final_acimut,uint32_t *Pos_final_elevacion);
void Control_automatico(void);
uint8_t Estado_control_automatico();


#ifdef	__cplusplus
extern "C" {
#endif



#ifdef	__cplusplus
}
#endif

#endif	/* CONTROL_AUTOMATICO_H */

