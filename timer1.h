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
void Temporizar_X_ms(void);
int Set_Temporizador(int Contador_Num,uint16_t Cant_ms);
int Get_Estado_Temporizadores(void);
void Function_Events_ms(void);
/*===========================================================================*/

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER1_H */

