#include <xc.h>
#include "Clock.h"
#include "libpic30.h" 

_FOSCSEL(FNOSC_FRC & IESO_OFF);                     // Internal FRC start-up without PLL, no Two Speed Start-up
_FOSC(FCKSM_CSECMD & OSCIOFNC_ON & POSCMD_XT)       // Clock switch enabled, OSC2 pin has clock out function , Primarly Oscillator XT 
_FPOR(FPWRT_PWR64)                                  // TPWRT de 64ms      
//_FWDT(FWDTEN_OFF);                                // Watchdog Timer disabled
    
void Config_Clock(void){
    // Inicializa con el clock FRC interno divided by n (1 por defecto) hasta 
    // que el cristal y PLL se estabilizen, luego modificamos y permitimos el switcheo de clock.
    
    // Fosc = (FIN / 2) * M /(N1*N2)
    // Verificar siempre que estemos entre los rangos de frecuencias especificados por el fabricante
    
    // Configuración de PLL
    PLLFBDbits.PLLDIV = 0b000011110;            // M = 32 
    CLKDIVbits.PLLPRE = 0b00000;                // N1 = 2   
    CLKDIVbits.PLLPOST = 0b00;                  // N2 = 2  
    
    int16_t Sel_Tipo_Clock = 0b011;             // Selección de clock. XT with PLL
    
    __builtin_write_OSCCONH(Sel_Tipo_Clock);    
    __builtin_write_OSCCONL(OSCCON | 0b1);      // Habilitamos el switcheo
    
    while(OSCCONbits.OSWEN){
        //Esperamos que se complete el cambio de clock
    }
   
    while(OSCCONbits.COSC != 0b011){
        //Esperamos que se cambie la configuración del clock
    }
    
}