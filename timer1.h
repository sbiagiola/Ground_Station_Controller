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

//#define CANT_TIMER  2       // Cantidad maxima de timers que se pueden setear
    
//typedef enum{
//    TEMP_1 = 0,
//    TEMP_2,
//    TEMP_3
//}timer_index;
//
//struct timer {
//    uint8_t enable;      // Index de timer
//    uint64_t tiempoCuenta;       // Tiempo de cuenta de timer
//    uint8_t timerFlag;      // Flag de tiempo cumplido
//};

/*============================ [Funciones] ==================================*/   
void init_timer1(void);
//void SetTimer(uint8_t, int);
//int GetTimer(uint8_t);
uint64_t millis(void);
/*===========================================================================*/

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER1_H */

