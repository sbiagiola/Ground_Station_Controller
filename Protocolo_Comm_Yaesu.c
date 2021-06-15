#include "UART.h"
#include "RingBuffer.h"
#include "Protocolo_Comm_Yaesu.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/*
 * (CR)0xD -> Retorno de carro
 * (LF)0xA -> Avance de l�nea 
  
 * Se debe devolver un 'CR' desde el micro a la pc despu�s de cada comando y tambi�n un 
  0Ah se envian datos desde el micro

 * Comando no valido causa el retorno de "?>" y se debe borrar el comando recibido.
 
 * �ngulos en grados
 
 * Acimut: Incrementos positivos en sentido antihorario
 
 * Elevaci�n: Incrementos positivos respecto al plano horizontal
 
 * Para finalizar los comandos debemos enviar, despu�s del mismo,  'CR'
 * 
 * Si se envia una mayuscula o minuscula el comando debe ser ejecutado, si es valido.

 * Los �ngulos enviados a la interfaz de control deben tener 3 d�gitos (relleno a la izquierda con ceros), 
   y los �ngulos devueltos, en algunos casos, tener 4 d�gitos con un "+0" al principio.
 */

/*======================= [Lista de comandos validos] =======================*/
/*
  Trama (PC -> Interface):  Envio: XXX'CR'             (XXX comando valido)
  Trama (intercafe -> PC):  Respuesta: 'CR'            (Sin pedidos de datos)
                                       'CR''LF'XXX     (XXX datos)
                                       ?>              (Comando no valido)
 * 
 Nota: Los comandos basicos del Yaesu solo soportan 3 digitos por �ngulo, en el caso de los
 comando extendidos se enviaran/recibiran m�s caracteres.
 */
                    /*============ Acimut ============*/
/*
R           // Clockwise Rotation
L           // Counter Clockwise Rotation
A           // CW/CCW Rotation Stop
C           // Retornar el valor de actual del �ngulo de de acimut 
Maaa        // Girar "aaa" grados el �ngulo de acimut
*/
                    /*============ Elevaci�n ============*/
/*
U           // UP Direction Rotation
D           // DOWN Direction Rotation
E           // UP/DOWN Direction Rotation Stop
B           // Retornar el valor de actual del �ngulo de elevaci�n
*/
                /*============ Elevaci�n - Acimut ============*/
/*
C2          // Retornar el valor de actual del �ngulo de acimut y de elevaci�n "+0aaa+0eee"
Waaa eee    // Girar "aaa" grados en acimut y "eee" grados en elevaci�n.
S           // Parar todo moviento asociado a cualquier �ngulo
 */
                   /*============ Extensiones ============*/
/*
Z1          // Velocidad m�s lenta de giro de elevaci�n
Z2          // Velocidad media 1 de giro de elevaci�n
Z3          // Velocidad media 2 de giro de elevaci�n
Z4          // Velocidad m�s r�pida de giro de elevaci�n
PA          // Formato de mayor precisi�n para acimut
PE          // Formato de mayor precisi�n para elevaci�n
PC          // Formato de mayor precisi�n para combinaci�n
*/
/*===========================================================================*/

typedef enum {
    Estableciendo_Conexion = 0, 
    Esperando_Datos,
    Recopilando_Datos,
    Validando_Comando,
    Error_Recibiendo_Datos,
}Estado_Comm;

typedef struct{
    uint8_t Comando_Actual;
    uint8_t Proximo_Comando;
    uint8_t Comando_Acimut;
    uint8_t Comando_Elevacion;
    uint8_t Velocidad_E;
}Comandos_Procesados;

typedef struct{
    char Ultimo_Comando_Almacenado[MAX_SIZE_COMMAND_AVALIBLE];
    char Ang_Acimut[MAX_LONG_DATA_ANGLE];     //123.4\0
    char Ang_Elevacion[MAX_LONG_DATA_ANGLE];  //160.8\0
}Data_Control;

/*===================== [Variables Internas (Globales)] =====================*/
uint8_t Mensaje_Env[MAX_SIZE_DATA_SEND];
uint8_t Caracter_Rec;
uint8_t Mensaje_Error[] = "?>";

uint32_t FlagRec;
uint32_t Indice_Rec = 0;

Data_Control Control;
Comandos_Procesados Situacion_Actual;

char Comando_Recibido[MAX_SIZE_COMMAND_AVALIBLE];
volatile int Habilitar_Comunicacion;       // Inicializaci�n luego de la configuraci�n del micro en main.c
extern volatile int Error_UART_U2;
/*===========================================================================*/

/*
    Esta funci�n segmenta un string de acuerdo a los delimitadores definidos y 
separa la cadena ingresada en segmentos que se encuentren entre ellos. Se debe
corroborar el formato de la misma antes de el llamado de esta funci�n, dado que 
esta solo divide la cadema ingresada.

Para el uso se debe pasar la direcci�n de memoria del string a partir de la cual se 
comenzara a filtra los datos, una vez detectado un espacio o un 'CR' se copiara dicha cadena
y se devolveran los datos a traves de Out_Data_1 y Out_Data_2, si no se detecto el final del string.
En caso de querer un �nico segmento de salida, repetir el dato de salida en los dos campos anteriores.
  
    char *Dato_No_Filtrada  (IN)        ->  Puntero al string a filtrar
    char* Out_Data_1        (IN/OUT)    ->  Primer dato de salida
    char* Out_Data_2        (IN/OUT)    ->  Segundo dato de salida
*/
void Segmentar_Datos(char *Dato_No_Filtrada, char* Out_Data_1, char* Out_Data_2){
    char * token;
    char *Delimitador = " \r";  //Caracteres de a delimitar el string: ' ' (space) y 'CHAR_CR' o '\r' (carriage return))
    token = strtok(Dato_No_Filtrada, Delimitador);
    strcpy(Out_Data_1,token);
    token = strtok(NULL, " ");
    if( token != NULL){
        strcpy(Out_Data_2,token);
    }
}

/* 
    Esta funci�n an�liza el formato del dato enviado por la PC a la interface. Es la encargada de 
determinar si un dato esta correcto de acuerdo a la forma de definici�n del comando seg�n lo comentado
en la parte superior del archivo.
Retorna un 1 si el comando esta correcto. En caso contrario se devolvera un 0
 
    char* Segmento      (IN)    ->  Puntero a cadena a analizar.
 */
int Analizando_Datos(char* Segmento){
    int j,Angulo_Num=1;
    int Cant_Dig_Antes=0;
    
    if(Segmento[1] == 'C'){     //PC123.0 150.9\r
        j=2;
        while(Segmento[j] != '\0' && Angulo_Num <= 2){
            for( ; Segmento[j] != '.' && !isspace(Segmento[j]); j++){
                if(!isdigit(Segmento[j])){
                    //Error detectando digitos
                    return 0;
                }
                Cant_Dig_Antes++;
                if(Cant_Dig_Antes > 3){
                    // + de 3 digitos en el �ngulo antes del '.' o ' '
                    return 0;
                }
            }
            if(Segmento[j] == '.' || isspace(Segmento[j])){
                j++;
            }
            else{
                // Punto o espacio no detectado
                return 0;
            }
            if(Segmento[j-1] == '.'){
                for( ; !isspace(Segmento[j]); j++){
                    if(!isdigit(Segmento[j])){
                        //Error detectando digitos
                        return 0;
                    }
                }                
            }
            Angulo_Num++;
            j++;
            Cant_Dig_Antes = 0;
        }
        if(Angulo_Num > 3){
            // M�s de dos angulos se detectaron.
            return 0;
        }
    // Dato valido
    return 1; 
    }
    
    if(Segmento[1]=='E' || Segmento[1]=='A'){
        j=2;
        for(  ; Segmento[j] != '.' && !isspace(Segmento[j]); j++){
            if(!isdigit(Segmento[j])){
                // Error detectando digitos
                return 0;
            }
            Cant_Dig_Antes++;
            if(Cant_Dig_Antes > 3){
                // + de 3 digitos en el �ngulo antes del '.' o ' '
                return 0;
            }
        }
        if(Segmento[j] == '.' || isspace(Segmento[j])){
            j++;
        }
        else{
            printf("Detecte algo raro\n");
            return 0;
        }
        if(Segmento[j-1] == '.'){
            for(  ; !isspace(Segmento[j]); j++){
                if(!isdigit(Segmento[j])){
                    // Error detectando digitos
                    return 0;
                }
            }                
        }
        j++;
        Cant_Dig_Antes = 0;
    //Dato valido
    return 1;
    }
    
    if(Segmento[0] == 'W'){
        j=1;
        while(Segmento[j] != '\0' && Angulo_Num <=2){
            for( ; !isspace(Segmento[j]); j++){
                if(!isdigit(Segmento[j])){
                    // Error detectando digitos
                    return 0;
                }
                Cant_Dig_Antes++;
            }
            if(Cant_Dig_Antes != 3){
                //Datos invalidos alg�n �ngulo
                return 0;
            }
            if(isspace(Segmento[j]) && Cant_Dig_Antes <= 3){
                j++;
            }
            else{
                //No se detecto el espacio o se detecto + de 3 digitos en alg�n �ngulo
                return 0;
            }
            Angulo_Num++;
            Cant_Dig_Antes = 0;
        }
    //Dato valido
    return 1; 
    }
    
    if(Segmento[0] == 'M'){
            for( j=1; !isspace(Segmento[j]); j++){
                if(!isdigit(Segmento[j])){
                    // Error detectando digitos
                    return 0;
                }
                Cant_Dig_Antes++;
            }
            if(Cant_Dig_Antes != 3){
                //Se detecto + de 3 digitos en el �ngulo
                return 0;
            }
    //"Dato valido
    return 1; 
    }
    
return 0;
}

uint8_t Verificando_Comando(){
    if(Comando_Recibido[0] == 'R' || Comando_Recibido[0] == 'r'){return Giro_Horario;}
    if(Comando_Recibido[0] == 'L' || Comando_Recibido[0] == 'l'){return Giro_Antihorario;}
    if(Comando_Recibido[0] == 'A' || Comando_Recibido[0] == 'a'){return Stop_Acimut;}
    
    if(Comando_Recibido[0] == 'M' || Comando_Recibido[0] == 'm'){ // M123'CR'   M123'\r'
        if(Analizando_Datos(&Comando_Recibido[0])){
            Segmentar_Datos(&Comando_Recibido[1],Control.Ang_Acimut,Control.Ang_Acimut); 
        }
        else{
            return Comando_No_Valido;
        }
    return Hacia_aaa_grados;
    }
    
    if(Comando_Recibido[0] == 'U' || Comando_Recibido[0] == 'u'){return Arriba;}
    if(Comando_Recibido[0] == 'D' || Comando_Recibido[0] == 'd'){return Abajo;}
    if(Comando_Recibido[0] == 'E' || Comando_Recibido[0] == 'e'){return Stop_Elevacion;}
    if(Comando_Recibido[0] == 'B' || Comando_Recibido[0] == 'b'){return Devolver_Valor_Elevacion;}
    
    if(Comando_Recibido[0] == 'C' || Comando_Recibido[0] == 'c'){
        if(Comando_Recibido[1] == '2'){
            return Devolver_Valor_A_E;
        }
        else return Devolver_Valor_Acimut;
    }
                                                                    // 0  3 5 7      // 0  3 5 7
    if(Comando_Recibido[0] == 'W' || Comando_Recibido[0] == 'w'){   // W123 356'CR'     W123 356'\r'
        if(Analizando_Datos(&Comando_Recibido[0])){
            Segmentar_Datos(&Comando_Recibido[1],Control.Ang_Acimut,Control.Ang_Elevacion); 
        }
        else{
            return Comando_No_Valido;
        }
    return Hacia_aaa_eee_grados;
    }
    
    if(Comando_Recibido[0] == 'S' || Comando_Recibido[0] == 's'){return Parar_Todo;}
    
    if(Comando_Recibido[0] == 'Z' || Comando_Recibido[0] == 'z'){
        if(Comando_Recibido[1] == '1'){return Velocidad_1_Elevacion;}
        if(Comando_Recibido[1] == '2'){return Velocidad_2_Elevacion;}
        if(Comando_Recibido[1] == '3'){return Velocidad_3_Elevacion;}
        if(Comando_Recibido[1] == '4'){return Velocidad_4_Elevacion;}
    }
                                                                  
    if(Comando_Recibido[0] == 'P' || Comando_Recibido[0] == 'p'){
                                                                        //   2   6 
        if(Comando_Recibido[1] == 'A' || Comando_Recibido[1] == 'a'){   // PA344.1'CR'
            if(Analizando_Datos(&Comando_Recibido[0])){
                Segmentar_Datos(&Comando_Recibido[2],Control.Ang_Acimut,Control.Ang_Acimut);
            }
            else{
                return Comando_No_Valido;
            }
        return Mayor_Presicion_a_grados; 
        }
                                                                        //   2   6
        if(Comando_Recibido[1] == 'E' || Comando_Recibido[1] == 'e'){   // PE344.1'CR'    
            if(Analizando_Datos(&Comando_Recibido[0])){
                Segmentar_Datos( &Comando_Recibido[2],Control.Ang_Elevacion,Control.Ang_Elevacion);  
            }
            else{
                return Comando_No_Valido;
            }   
        return Mayor_Presicione_e_grados;
        }  
                                                                      //   2     8
        if(Comando_Recibido[1] == 'C' || Comando_Recibido[1] == 'c'){ // PC344.1 133.1'CR'             
            if(Analizando_Datos(&Comando_Recibido[0])){
                Segmentar_Datos( &Comando_Recibido[2],Control.Ang_Acimut,Control.Ang_Elevacion );   
            }
            else{
                return Comando_No_Valido;
            }           
        return Mayor_Presicion_a_e_grados;
        }      
    }
return Comando_No_Valido;
}

void Comm_PC_Interface(){
    /*====================== Inicializaci�n por seguridad ======================*/
                Situacion_Actual.Comando_Actual = Parar_Todo;
                Situacion_Actual.Comando_Acimut = Stop_Acimut;
                Situacion_Actual.Comando_Elevacion = Stop_Elevacion;
                Situacion_Actual.Velocidad_E = Velocidad_1_Elevacion;
   /*===========================================================================*/
                
    static Estado_Comm Estado_Actual = Estableciendo_Conexion;      
    FlagRec = uart_ringBuffer_recDatos_U2(&Caracter_Rec, sizeof(Caracter_Rec));
    
        switch (Estado_Actual) {
            
            case Estableciendo_Conexion:
                if( Habilitar_Comunicacion ){
                    Mensaje_Env[0] = ACKNOWLEDGE;
                    uart_ringBuffer_envDatos_U2(&Mensaje_Env[0],sizeof(char));  // Notificaci�n de que estamos queriendo establecer comunicaci�n
                }
                
                if ( FlagRec != 0 && Caracter_Rec == ACKNOWLEDGE){   // Esperas un ACK de la respuesta de la PC
                    Estado_Actual = Esperando_Datos;
                }
                /* Se puede poner un timer en la PC para que al cabo de tantos seg tire un error en al comunicacion*/
            break;
                
            case Esperando_Datos:
                
                if( FlagRec != 0 ){
                    Comando_Recibido[Indice_Rec] = Caracter_Rec;
                    Indice_Rec++; 
                }      
                
            break;
            
            case Recopilando_Datos:
                
                if(Error_UART_U2 == 1){ 
                    Indice_Rec = 0;
                    Clean_RingBufferRx_U2();
                    Mensaje_Env[0] = ACKNOWLEDGE;
                    uart_ringBuffer_envDatos_U2(&Mensaje_Env[0],sizeof(char));
                    Estado_Actual = Esperando_Datos;
                    break;
                }
                
                if( (FlagRec != 0) && (Caracter_Rec != CHAR_CR) ){
                    if(Indice_Rec <= MAX_SIZE_COMMAND_AVALIBLE){
                        Comando_Recibido[Indice_Rec] = Caracter_Rec;
                        Indice_Rec++;
                    }
                    else{
                        Indice_Rec = 0;
                        Clean_RingBufferRx_U2();
                        Estado_Actual = Error_Recibiendo_Datos;
                        break;
                    }
                }
            
                if( (FlagRec != 0) && (Caracter_Rec == CHAR_CR) ){
                    Comando_Recibido[Indice_Rec] = Caracter_Rec; 
                    Estado_Actual = Validando_Comando;
                }
            break;
            
            case Validando_Comando:
                    
            /* Se podr�a poner que cada cierto tiempo los comandos "manuales se borren, como para que no queden
                girando, o moviendose indefinidamente seg�n ese comando recibido. Creo que un polling de 10 ms
                nos ayudaria aca. No debe afectar a los comandos de posicionamiento. 
            */
                Situacion_Actual.Proximo_Comando = Verificando_Comando();
                    
                if(Situacion_Actual.Proximo_Comando != Comando_No_Valido){
                        strcpy(Control.Ultimo_Comando_Almacenado,Comando_Recibido);
                }
                    
                    switch(Situacion_Actual.Proximo_Comando){
                        case Parar_Todo:
                            
                        break;

                        case Giro_Horario:
                            
                        break;

                        case Giro_Antihorario:

                        break;

                        case Stop_Acimut:

                        break;

                        case Devolver_Valor_Acimut:

                        break;

                        case Hacia_aaa_grados:
                            
                        break;

                        case Arriba:
                            
                        break;

                        case Abajo:
                            
                        break;

                        case Stop_Elevacion:

                        break;

                        case Devolver_Valor_Elevacion:

                        break;

                        case Velocidad_1_Elevacion:

                        break;

                        case Velocidad_2_Elevacion:

                        break;

                        case Velocidad_3_Elevacion:

                        break;

                        case Velocidad_4_Elevacion:

                        break;

                        case Hacia_aaa_eee_grados:

                        break;

                        case Devolver_Valor_A_E:

                        break;

                        case Mayor_Presicion_a_grados:

                        break;

                        case Mayor_Presicione_e_grados:

                        break;

                        case Mayor_Presicion_a_e_grados:

                        break;

                        case Comando_No_Valido:
                            uart_ringBuffer_envDatos_U2(Mensaje_Error,sizeof(Mensaje_Error));
                            Estado_Actual = Esperando_Datos;
                        break;
                        }
            break;
                
            case Error_Recibiendo_Datos:
                uart_ringBuffer_envDatos_U2(Mensaje_Error,sizeof(Mensaje_Error));
                Estado_Actual = Esperando_Datos;
            break;
            
            default: Estado_Actual = Esperando_Datos;
        }
}