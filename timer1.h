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
void init_timer1(void);
void delayPIC_ms(uint64_t);
//void SetTimer(uint8_t, int);
//int GetTimer(uint8_t);
uint64_t millis(void);
/*===========================================================================*/

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER1_H */

