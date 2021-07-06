/* 
 * File:   timer1.h
 * Author: biagi
 *
 * Created on 26 de mayo de 2021, 20:10
 */

#ifndef TIMER1_H
#define	TIMER1_H

#ifdef	__cplusplus
extern "C" {
#endif
    
/*============================ [Funciones] ==================================*/   
void init_timer1();
uint16_t temporizador();
int Temporizador_X_ms(uint16_t Cant_ms);
void Function_Events_1ms();
/*===========================================================================*/

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER1_H */

