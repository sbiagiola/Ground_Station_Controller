#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <p33FJ128GP804.h>

#include "UART.h"
#include "RingBuffer.h"
#include "Protocolo_Comm_Yaesu.h"
#include "IO_Basic.h"
#include "IO_Accionamiento.h"
/*
 * (CR)0xD -> Retorno de carro
 * (LF)0xA -> Avance de línea 
  
 * Se debe devolver un 'CR' desde el micro a la pc después de cada comando y también un 
  0Ah se envian datos desde el micro

 * Comando no valido causa el retorno de "?>" y se debe borrar el comando recibido.
 
 * Ángulos en grados
 
 * Acimut: Incrementos positivos en sentido antihorario
 
 * Elevación: Incrementos positivos respecto al plano horizontal
 
 * Para finalizar los comandos debemos enviar, después del mismo,  'CR'
 * 
 * Si se envia una mayuscula o minuscula el comando debe ser ejecutado, si es valido.

 * Los ángulos enviados a la interfaz de control deben tener 3 dígitos (relleno a la izquierda con ceros), 
   y los ángulos devueltos, en algunos casos, tener 4 dígitos con un "+0" al principio.
 */

/*======================= [Lista de comandos validos] =======================*/
/*
  Trama (PC -> Interface):  Envio: XXX'CR'             (XXX comando valido)
  Trama (intercafe -> PC):  Respuesta: 'CR'            (Sin pedidos de datos)
                                       'CR''LF'XXX     (XXX datos)
                                       ?>              (Comando no valido)
 * 
 Nota: Los comandos basicos del Yaesu solo soportan 3 digitos por ángulo, en el caso de los
 comando extendidos se enviaran/recibiran más caracteres.
 */

/* =======================   Comandos manuales   =======================
 * Acimut:
 *  R               // Clockwise Rotation
 *  L               // Counter Clockwise Rotation
 *  A               // CW/CCW Rotation Stop
 * Elevacion:
 *  U               // UP Direction Rotation
 *  D               // DOWN Direction Rotation
 *  E               // UP/DOWN Direction Rotation Stop
 * 
 * =======================   Comandos lectura   =========================
 *  C               // Retornar el valor actual del ángulo de acimut
 *  B               // Retornar el valor de actual del ángulo de elevación
 * 
 * =======================   Comandos tracking   ========================
 *  S               // Parar todo moviento asociado a cualquier ángulo
 *  H               // Volver a posicion de HOME
 *  Paaa.a eee.e    // Establecer objetivo de tracking
 
   ====================================================================== */

/*===================== [Variables Internas (Globales)] =====================*/
uint8_t Mensaje_Env[MAX_SIZE_DATA_SEND];
uint8_t Mensaje_Error[] = "?>";
uint8_t Mensaje_Recibido_Correcto[] = "\r";

uint8_t Caracter_Rec;
uint32_t FlagRec;
uint32_t Indice_Rec = 0;
char Buffer_Recepcion[MAX_SIZE_COMMAND_AVALIBLE];

Info_Comandos_Procesados Comando_Procesado;    
Comando_Almacenado Char_Comando;
uint8_t comandoLectura = 0;
uint8_t nuevoComando = 0;
/*===========================================================================*/

/*===================== [Variables Externas (Globales)] =====================*/
extern volatile int Error_UART_U2;
//const extern uint8_t tracking_flag;
const extern ID_Comandos estado_Accionamiento;
/*===========================================================================*/

/*
    Esta función segmenta un string de acuerdo a los delimitadores definidos y 
separa la cadena ingresada en segmentos que se encuentren entre ellos. Se debe
corroborar el formato de la misma antes de el llamado de esta función, dado que 
esta solo divide la cadema ingresada.

Para el uso se debe pasar la dirección de memoria del string a partir de la cual se 
comenzara a filtra los datos, una vez detectado un espacio o un 'CR' se copiara dicha cadena
y se devolveran los datos a traves de Out_Data_Ac y Out_Data_El, si no se detecto el final del string.
En caso de querer un único segmento de salida, repetir el dato de salida en los dos campos anteriores.
  
    char *Dato_No_Filtrada   (IN)        ->  Puntero al string a filtrar
    char* Out_Data_Ac        (IN/OUT)    ->  Primer dato de salida
    char* Out_Data_El        (IN/OUT)    ->  Segundo dato de salida
*/
void Segmentar_Datos(char *Raw_Data, char* Out_Data_Ac, char* Out_Data_El){
    
    while(!isdigit(*Raw_Data)){
        Raw_Data++;     // Sacamos los datos que no son digitos
    }
    
    char * token;
    char *Delimitador = " \r";  //Caracteres de a delimitar el string: ' ' (space) y 'CHAR_CR' o '\r' (carriage return))
    token = strtok(Raw_Data, Delimitador);
    strcpy(Out_Data_Ac,token);
    token = strtok(NULL, Delimitador);
    if( token != NULL){
        strcpy(Out_Data_El,token);
    }
}


/* =========================================================================================================== */
/* 
    Esta función análiza el formato del dato enviado por la PC a la interface. Es la encargada de 
determinar si un dato esta correcto de acuerdo a la forma de definición del comando según lo comentado
en la parte superior del archivo.
Retorna un 1 si el comando esta correcto. En caso contrario se devolvera un 0
 
    char* Segmento      (IN)    ->  Puntero a cadena a analizar.
 */
int Analizando_Datos(char* Segmento){
    
    int j = 1, Angulo_Num = 0, Cant_Dig_Antes = 0;
    
    while(Segmento[j - 1] != '\r')
    {
        Angulo_Num++;
        if(Angulo_Num > 2) {
//            putrsUART2("[Analizando_Datos] Error: Mas de un angulo detectado\n");
            return(0);
        }
        
        if(Segmento[j] == '-') {
//            putrsUART2("[Analizando_Datos] Angulo negativo detectado\n");
            j++;
        } //else putrsUART2("[Analizando_Datos] Angulo positivo detectado\n");
        
        for( ; Segmento[j] != '.'; j++) {
            if(!isdigit(Segmento[j])){
                //Error detectando digitos
//                putrsUART2("[Analizando_Datos] Error: Digito no valido (1)\n");
                return 0;
            }
            Cant_Dig_Antes++;
            if(Cant_Dig_Antes > 3) {
                // + de 3 digitos en el ángulo antes del '.' o ' '
//                putrsUART2("[Analizando_Datos] Error: + de 3 digitos en el ángulo antes del '.'\n");
                return 0;
            }
        }
        
        j++;
        
        for( ; !isspace(Segmento[j]); j++){
            if(!isdigit(Segmento[j])){
                //Error detectando digitos
//                putrsUART2("[Analizando_Datos] Error: Digito no valido (2)\n");
                return 0;
            }
        }
        
        j++;
        Cant_Dig_Antes = 0;
    }
    
    if(Angulo_Num < 2) {
        // Menos de dos angulos detectados
//        putrsUART2("[Analizando_Datos] Error: Menos de un angulo detectado\n");
        return(0);
    }
    
    // Dato valido
    return 1;
}


/* =========================================================================================================== */

uint8_t Verificando_Comando(){
    
//    double angulo = 0;
//    char char_Angulo[MAX_LONG_DATA_ANGLE];
    
    /* --------------------   Comandos manuales   -------------------- */
    
    // Acimut:
    
    {
        if(Buffer_Recepcion[0] == 'R' || Buffer_Recepcion[0] == 'r') {
//            putrsUART2("[Verificando_Comando] Comando GIRO_HORARIO detectado\n");
            if(estado_Accionamiento != Objetivo_Tracking) return Giro_Horario;
        }

        if(Buffer_Recepcion[0] == 'L' || Buffer_Recepcion[0] == 'l') {
//            putrsUART2("[Verificando_Comando] Comando GIRO_ANTIHORARIO detectado\n");
            if(estado_Accionamiento != Objetivo_Tracking) return Giro_Antihorario;
        }

        if(Buffer_Recepcion[0] == 'A' || Buffer_Recepcion[0] == 'a') {
//            putrsUART2("[Verificando_Comando] Comando STOP_ACIMUT detectado\n");
            if(estado_Accionamiento != Objetivo_Tracking) return Stop_Acimut;
        }

        // Elevacion:

        if(Buffer_Recepcion[0] == 'U' || Buffer_Recepcion[0] == 'u') {
//            putrsUART2("[Verificando_Comando] Comando GIRO_ARRIBA detectado\n");
            if(estado_Accionamiento != Objetivo_Tracking) return Giro_Arriba;
        }

        if(Buffer_Recepcion[0] == 'D' || Buffer_Recepcion[0] == 'd') {
//            putrsUART2("[Verificando_Comando] Comando GIRO_ABAJO detectado\n");
            if(estado_Accionamiento != Objetivo_Tracking) return Giro_Abajo;
        }

        if(Buffer_Recepcion[0] == 'E' || Buffer_Recepcion[0] == 'e') {
//            putrsUART2("[Verificando_Comando] Comando STOP_ELEVACION detectado\n");
            if(estado_Accionamiento != Objetivo_Tracking) return Stop_Elevacion;
        }
    }
    
    /* --------------------   Comandos tracking   -------------------- */
    
    // Stop global
    if(Buffer_Recepcion[0] == 'S' || Buffer_Recepcion[0] == 's'){
//        putrsUART2("[Verificando_Comando] Comando STOP_GLOBAL detectado\n");
        return Stop_Global;
    }
    
    // Return to home
    if(Buffer_Recepcion[0] == 'H' || Buffer_Recepcion[0] == 'h'){
//        putrsUART2("[Verificando_Comando] Comando RETURN_TOHOME detectado\n");
        return GoToHome_Acimut;
    }
    
    // Comando objetivo tracking
    if(Buffer_Recepcion[0] == 'P' || Buffer_Recepcion[0] == 'p'){

        if(Analizando_Datos(Buffer_Recepcion)){
            Segmentar_Datos(Buffer_Recepcion,Char_Comando.Char_Acimut,Char_Comando.Char_Elevacion);
            
            Actualizar_Objetivos();
            return Objetivo_Tracking;
        }
        else{
            return Comando_No_Valido;
        }
    }
       
    /* --------------------   Comandos lectura   -------------------- */

    if(Buffer_Recepcion[0] == 'B' || Buffer_Recepcion[0] == 'b'){
        char dataToSend[MAX_SIZE_DATA_SEND] = "B,A,";
//        putrsUART2("[Verificando_Comando] Comando LEER_ELEVACION detectado\n\r");
        
        double angulo_Az = 0;
        double angulo_Elev = 0;
        char char_Angulo_Az[10] = {};
        char char_Angulo_Elev[10] = {};
        
        angulo_Az = get_Acimut();
        if(angulo_Az>360){
            angulo_Az = angulo_Az - 360;
        }
        sprintf(char_Angulo_Az, "%.1f", angulo_Az);

        angulo_Elev = get_Elevacion();
        sprintf(char_Angulo_Elev, "%.1f", angulo_Elev);
        
        strcat(dataToSend,char_Angulo_Az);
        strcat(dataToSend,",E,");
        strcat(dataToSend,char_Angulo_Elev);
        strcat(dataToSend,"\r\n");
        putrsUART2(dataToSend);
        
        comandoLectura = 1;
        return Leer_Posicion;
    }

//    putrsUART2("[Verificando_Comando] Comando no valido\n\r");
    return Comando_No_Valido;
}


/* =========================================================================================================== */

void Comm_PC_Interface(){
    static Estado_Comunicacion Estado_Comm = Esperando_Datos;      
    FlagRec = uart_ringBuffer_recDatos_U2(&Caracter_Rec, sizeof(Caracter_Rec));

    int i = 0;
    
        switch (Estado_Comm){
            
            case Esperando_Datos:
                
                if( FlagRec != 0 ){
//                    putrsUART2("[Comm_PC_Interface] Comando recibido\n\n");
                    Buffer_Recepcion[Indice_Rec] = Caracter_Rec;
                    Indice_Rec++;
//                    putrsUART2(Buffer_Recepcion);
//                    putrsUART2("\n\r");
                    Estado_Comm = Recopilando_Datos;
                }      
                
                break;
            
            case Recopilando_Datos:
                
                if(Error_UART_U2 == 1) { 
                    Indice_Rec = 0;
                    Clean_RingBufferRx_U2();
                    Mensaje_Env[0] = ACKNOWLEDGE;
//                    putrsUART2("[Comm_PC_Interface] ERROR en la UART2 \n");
                    //uart_ringBuffer_envDatos_U2(Mensaje_Env,sizeof(char));
                    Estado_Comm = Esperando_Datos;
                    Error_UART_U2 = 0;
                    break;
                }
                
                if((FlagRec != 0) && (Caracter_Rec != CHAR_CR)) {
                    if(Indice_Rec <= MAX_SIZE_COMMAND_AVALIBLE){
                        Buffer_Recepcion[Indice_Rec] = Caracter_Rec;
                        Indice_Rec++;
                    }
                    else{
                        Indice_Rec = 0;
                        Clean_RingBufferRx_U2();                        
                        Estado_Comm = Comando_No_Reconocido;
                        break;
                    }
                }
            
                if( (FlagRec != 0) && (Caracter_Rec == CHAR_CR) ){
                    Buffer_Recepcion[Indice_Rec] = Caracter_Rec;
                    Estado_Comm = Validando_Comando;
                    break;
                }
            break;
            
            case Validando_Comando:
                
//                putrsUART2("[Comm_PC_Interface] Comando recibido: ");
//                putrsUART2(Buffer_Recepcion);
//                putrsUART2("[Comm_PC_Interface] Validando comando...\n");
                
                    
            /* Se podría poner que cada cierto tiempo los comandos "manuales se borren, como para que no queden
                girando, o moviendose indefinidamente según ese comando recibido. Creo que un polling de 10 ms
                nos ayudaria aca. No debe afectar a los comandos de posicionamiento. 
            */
                Comando_Procesado.Proximo = Verificando_Comando();

                if(Comando_Procesado.Proximo == Comando_No_Valido){
                    Estado_Comm = Comando_No_Reconocido;
                    putrsUART2("?>\r\n");//envio comando de confirmacion 
                    break;
                }
                
//                uart_ringBuffer_envDatos_U2(Mensaje_Recibido_Correcto,sizeof(Mensaje_Recibido_Correcto));
//                strcpy(Char_Comando.Comando_Recibido,Buffer_Recepcion);
                    
//                putrsUART2("[Comm_PC_Interface] Comando valido!\n");
//                Comando_Procesado.Ultimo = Comando_Procesado.Actual;
                if (!comandoLectura) {
                    Comando_Procesado.Actual = Comando_Procesado.Proximo;
                
                    nuevoComando = 1;
                    
                } else comandoLectura = 0;
                putrsUART2("\r\n");//envio comando de confirmacion 
                Estado_Comm = Limpiando_Buffer;
                
                break;
                
            
            case Limpiando_Buffer:
                
//                putrsUART2("[Comm_PC_Interface] Limpiando buffer...\n\n");
                
                while(i < MAX_SIZE_COMMAND_AVALIBLE) {
                    Buffer_Recepcion[i] = '\0';
                    i++;
                }
                Indice_Rec = 0;
                Estado_Comm = Esperando_Datos;
                
                break;
                
                
            case Comando_No_Reconocido:
//                uart_ringBuffer_envDatos_U2(Mensaje_Error,sizeof(Mensaje_Error));
//                putrsUART2("[Comm_PC_Interface] Comando NO RECONOCIDO\n\r");
                
                Estado_Comm = Limpiando_Buffer;
                break;
            
            default: Estado_Comm = Limpiando_Buffer;
        }
}
