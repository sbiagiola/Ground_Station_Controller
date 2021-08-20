/* 
 * File:   interrupts.h
 * Author: Usuario
 *
 * Created on 24 de abril de 2021, 18:04
 */

#ifndef INTERRUPTS_H
#define	INTERRUPTS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
void enableInterrupts(void);
void disableInterrupts(void);

void initInterrupts(void);

void disableUART1(void);
void disableUART2(void);
void disableADC1(void);
void disableTIMER1(void);
void disableCN(void);

#ifdef	__cplusplus
}
#endif

#endif	/* INTERRUPTS_H */

