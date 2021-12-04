
#ifndef CLOCK_H
#define	CLOCK_H

#ifdef	__cplusplus
    extern "C" {
#endif

/***************  Definiciones   **************/   
#define FCY 40000000      //Frecuencia de trabajo de 40 MHz. Necesaria para utilizar _delay
/**********************************************/
        
/****************  Funciones   ****************/         
void Config_Clock(void);
/**********************************************/

/*  
 FOSCSEL: Oscillator Source Selection Register
    The FOSCSEL register selects the initial oscillator source and start-up option. FOSCSEL
    contains the following Configuration bit:
 
    The Initial Oscillator Source Selection Configuration bits (FNOSC<2:0>) in the Oscillator
    Source Selection register (FOSCSEL<2:0>) determine the clock source that is used at a
    Power-on Reset (POR). Thereafter, the clock source can be changed between
    permissible clock sources with clock switching.
    The Internal FRC Oscillator with postscaler (FRCDIVN) is the default (unprogrammed)
    selection.
 */

/*
** _FOSCSEL( OPT1_ON & OPT2_OFF & OPT3_PLL )
**
**   Oscillator Mode:
**     FNOSC_FRC            Internal Fast RC (FRC)
**     FNOSC_FRCPLL         Internal Fast RC (FRC) w/ PLL
**     FNOSC_PRI            Primary Oscillator (XT, HS, EC)
**     FNOSC_PRIPLL         Primary Oscillator (XT, HS, EC) w/ PLL
**     FNOSC_SOSC           Secondary Oscillator (SOSC)
**     FNOSC_LPRC           Low Power RC Oscillator (LPRC)
**     FNOSC_FRCDIV16       Internal Fast RC (FRC) divide by 16

**   Internal External Switch Over Mode:
**     IESO_OFF             Start-up device with user-selected oscillator source
**     IESO_ON              Start-up device with FRC, then automatically switch to user-selected oscillator source when ready
 
*/     
        
/*
FOSC: Oscillator Configuration Register
    The FOSC register configures the Primary Oscillator mode, OSCO pin function, peripheral
    pin select, and the fail-safe and clock switching modes. FOSC contains the following
    Configuration bits:
 
    - The Primary Oscillator Mode Selection Configuration bits (POSCMD<1:0> in the Oscillator
    Configuration Register (FOSC<1:0>)select the operation mode of the POSC.
    - The OSC2 Pin Function (OSCIOFNC) configuration bit in the Oscillator Configuration
    Register (FOSC<2>) selects the OSC2 pin function, except in HS or Medium-Speed
    Oscillator (XT) mode.
 
    When OSCIOFNC is unprogrammed (?1?), the FCY clock is output on the OSC2 pin.
    When OSCIOFNC is programmed (?0?), the OSC2 pin becomes a general purpose I/O pin
*/

 /*
**Only one invocation of FOSC should appear in a project,
** at the top of a C source file (outside of any function).
**
** The following constants can be used to set FOSC.
** Multiple options may be combined, as shown:
**
** _FOSC( OPT1_ON & OPT2_OFF & OPT3_PLL )
**
**   Primary Oscillator Source:
**     POSCMD_EC            EC Oscillator Mode
**     POSCMD_XT            XT Oscillator Mode
**     POSCMD_HS            HS Oscillator Mode
**     POSCMD_NONE          Primary Oscillator Disabled
**
**   OSC2 Pin Function:
**     OSCIOFNC_ON          OSC2 pin has digital I/O function
**     OSCIOFNC_OFF         OSC2 pin has clock out function
**
**   Peripheral Pin Select Configuration:
**     IOL1WAY_OFF          Allow Multiple Re-configurations
**     IOL1WAY_ON           Allow Only One Re-configuration
**
**   Clock Switching and Monitor:
**     FCKSM_CSECME         Both Clock Switching and Fail-Safe Clock Monitor are enabled
**     FCKSM_CSECMD         Clock switching is enabled, Fail-Safe Clock Monitor is disabled
**     FCKSM_CSDCMD         Both Clock Switching and Fail-Safe Clock Monitor are disabled

*/
                  
/*
** Only one invocation of FPOR should appear in a project,
** at the top of a C source file (outside of any function).
**
** The following constants can be used to set FPOR.
** Multiple options may be combined, as shown:
**
** _FPOR( OPT1_ON & OPT2_OFF & OPT3_PLL )
**
**   POR Timer Value:
**     FPWRT_PWR1           Disabled
**     FPWRT_PWR2           2ms
**     FPWRT_PWR4           4ms
**     FPWRT_PWR8           8ms
**     FPWRT_PWR16          16ms
**     FPWRT_PWR32          32ms
**     FPWRT_PWR64          64ms
**     FPWRT_PWR128         128ms
**
**   Alternate I2C  pins:
**     ALTI2C_ON            I2C mapped to ASDA1/ASCL1 pins
**     ALTI2C_OFF           I2C mapped to SDA1/SCL1 pins
**
*/
     
//TPWRT es un timer programable que se utiliza luego del Brown-out Reset (BOR) de manera agregar
//un delay en SYSRST para que las fuentes de alimentaciones (interna o externas) hayan alcanzado 
//la estabilidad en su valor nominal.

#ifdef	__cplusplus
    }
#endif
#endif	/* CLOCK_H */
