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

typedef enum{
    Temporizador_1=1,
    Temporizador_2,
    Temporizador_3,
}Temporizadores;

/*============================ [Funciones] ==================================*/   
void init_timer1(void);
uint16_t temporizador(void);
int Temporizar_X_ms(void);
void Set_Temporizador(int Contador_Num,uint16_t Cant_ms);
void Function_Events_1ms(void);
/*===========================================================================*/

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER1_H */

