#include "UART.h"
#include "RingBuffer.h"
#include "Protocolo_Comm_Yaesu.h"
#include "Entradas.h"
#include "Salidas_Motores.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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
                    /*============ Acimut ============*/
/*
R           // Clockwise Rotation
L           // Counter Clockwise Rotation
A           // CW/CCW Rotation Stop
C           // Retornar el valor de actual del ángulo de de acimut 
Maaa        // Girar "aaa" grados el ángulo de acimut
*/
                    /*============ Elevación ============*/
/*
U           // UP Direction Rotation
D           // DOWN Direction Rotation
E           // UP/DOWN Direction Rotation Stop
B           // Retornar el valor de actual del ángulo de elevación
*/
                /*============ Elevación - Acimut ============*/
/*
C2          // Retornar el valor de actual del ángulo de acimut y de elevación "+0aaa+0eee"
Waaa eee    // Girar "aaa" grados en acimut y "eee" grados en elevación.
S           // Parar todo moviento asociado a cualquier ángulo
 */
                   /*============ Extensiones ============*/
/*
Z1          // Velocidad más lenta de giro de elevación
Z2          // Velocidad media 1 de giro de elevación
Z3          // Velocidad media 2 de giro de elevación
Z4          // Velocidad más rápida de giro de elevación
PA          // Formato de mayor precisión para acimut
PE          // Formato de mayor precisión para elevación
PC          // Formato de mayor precisión para combinación
*/
/*===========================================================================*/

/*===================== [Variables Internas (Globales)] =====================*/
uint8_t Mensaje_Env[MAX_SIZE_DATA_SEND];
uint8_t Caracter_Rec;
uint8_t Mensaje_Error[] = "?>";
uint8_t Mensaje_Recibido_Correcto[] = "\r";
uint32_t FlagRec;
uint32_t Indice_Rec = 0;

Comando_Almacenado Comando;

char Comando_Recibido[MAX_SIZE_COMMAND_AVALIBLE];
volatile int Habilitar_Comunicacion;       // Inicialización luego de la configuración del micro en main.c

/*===================== [Variables Externas (Globales)] =====================*/
extern volatile int Error_UART_U2;
extern Data_Control Control;
extern Info_Comandos_Procesados Comando_Procesado;
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
  
    char *Dato_No_Filtrada  (IN)        ->  Puntero al string a filtrar
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

/* 
    Esta función análiza el formato del dato enviado por la PC a la interface. Es la encargada de 
determinar si un dato esta correcto de acuerdo a la forma de definición del comando según lo comentado
en la parte superior del archivo.
Retorna un 1 si el comando esta correcto. En caso contrario se devolvera un 0
 
    char* Segmento      (IN)    ->  Puntero a cadena a analizar.
 */
int Analizando_Datos(char* Segmento){
    int j,Angulo_Num=1;
    int Cant_Dig_Antes=0;
    
    if(Segmento[1] == 'C'){     //PC123.0 150.9\r
        j=2;
        while(Segmento[j] != '\r' && Angulo_Num <= 2){
            for( ; Segmento[j] != '.' && !isspace(Segmento[j]); j++){
                if(!isdigit(Segmento[j])){
                    //Error detectando digitos
                    return 0;
                }
                Cant_Dig_Antes++;
                if(Cant_Dig_Antes > 3){
                    // + de 3 digitos en el ángulo antes del '.' o ' '
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
        if(Angulo_Num > 2){
            // Más de dos angulos se detectaron.
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
                // + de 3 digitos en el ángulo antes del '.' o ' '
                return 0;
            }
        }
        if(Segmento[j] == '.' || isspace(Segmento[j])){
            j++;
        }
        else{
            //Detectamos algo raro
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
        Cant_Dig_Antes = 0;
    //Dato valido
    return 1;
    }
    
    if(Segmento[0] == 'W'){
        j=1;
        while(Segmento[j] != '\r' && Angulo_Num <=2){
            for( ; !isspace(Segmento[j]); j++){
                if(!isdigit(Segmento[j])){
                    // Error detectando digitos
                    return 0;
                }
                Cant_Dig_Antes++;
            }
            if(Cant_Dig_Antes != 3){
                //Datos invalido. Algún ángulo tiene menos de 3 digitos
                return 0;
            }
            if(isspace(Segmento[j]) && Cant_Dig_Antes == 3){
                j++;
            }
            else{
                //No se detecto el espacio o se detecto + de 3 digitos en algún ángulo
                return 0;
            }
            Angulo_Num++;
            Cant_Dig_Antes = 0;
        }
        if(Angulo_Num > 2){
            // Más de dos angulos se detectaron.
            return 0;
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
                //Se detecto + de 3 digitos en el ángulo
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
        if(Analizando_Datos(Comando_Recibido)){
            Segmentar_Datos(Comando_Recibido,Comando.Char_Acimut,Comando.Char_Acimut);
            Control.Ultimo_Ang_Acimut = Control.Target_Acimut;
            Control.Target_Acimut = atof(Comando.Char_Acimut);
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

    if(Comando_Recibido[0] == 'W' || Comando_Recibido[0] == 'w'){
        if(Analizando_Datos(Comando_Recibido)){
            Segmentar_Datos(Comando_Recibido,Comando.Char_Acimut,Comando.Char_Elevacion);
            Control.Ultimo_Ang_Acimut = Control.Target_Acimut;
            Control.Target_Acimut = atof(Comando.Char_Acimut);
            Control.Ultimo_Ang_Elevacion = Control.Target_Elevacion;
            Control.Target_Elevacion = atof(Comando.Char_Elevacion);
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
        if(Comando_Recibido[1] == 'A' || Comando_Recibido[1] == 'a'){   
            if(Analizando_Datos(Comando_Recibido)){
                Segmentar_Datos(Comando_Recibido,Comando.Char_Acimut,Comando.Char_Acimut);
                Control.Ultimo_Ang_Acimut = Control.Target_Acimut;
                Control.Target_Acimut = atof(Comando.Char_Acimut);
            }
            else{
                return Comando_No_Valido;
            }
        return Mayor_Presicion_a_grados; 
        }
        
        if(Comando_Recibido[1] == 'E' || Comando_Recibido[1] == 'e'){       
            if(Analizando_Datos(Comando_Recibido)){
                Segmentar_Datos(Comando_Recibido,Comando.Char_Elevacion,Comando.Char_Elevacion);
                Control.Ultimo_Ang_Elevacion = Control.Target_Elevacion;
                Control.Target_Elevacion = atof(Comando.Char_Elevacion);
            }
            else{
                return Comando_No_Valido;
            }   
        return Mayor_Presicione_e_grados;
        }  

        if(Comando_Recibido[1] == 'C' || Comando_Recibido[1] == 'c'){              
            if(Analizando_Datos(Comando_Recibido)){
                Segmentar_Datos(Comando_Recibido,Comando.Char_Acimut,Comando.Char_Elevacion);
                Control.Ultimo_Ang_Acimut = Control.Target_Acimut;
                Control.Target_Acimut = atof(Comando.Char_Acimut);
                Control.Ultimo_Ang_Elevacion = Control.Target_Elevacion;
                Control.Target_Elevacion = atof(Comando.Char_Elevacion);
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
    static Estado_Comunicacion Estado_Comm = Estableciendo_Conexion;      
    FlagRec = uart_ringBuffer_recDatos_U2(&Caracter_Rec, sizeof(Caracter_Rec));
    
        switch (Estado_Comm) {
            
            case Estableciendo_Conexion:
                if( Habilitar_Comunicacion ){
                    Mensaje_Env[0] = ACKNOWLEDGE;
                    uart_ringBuffer_envDatos_U2(Mensaje_Env,sizeof(char));  // Notificación de que estamos queriendo establecer comunicación
                }
                
                if ( FlagRec != 0 && Caracter_Rec == ACKNOWLEDGE){   // Esperas un ACK de la respuesta de la PC
                    Estado_Comm = Esperando_Datos;
                    break;
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
                    uart_ringBuffer_envDatos_U2(Mensaje_Env,sizeof(char));
                    Estado_Comm = Esperando_Datos;
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
                        Estado_Comm = Comando_No_Reconocido;
                        break;
                    }
                }
            
                if( (FlagRec != 0) && (Caracter_Rec == CHAR_CR) ){
                    Comando_Recibido[Indice_Rec] = Caracter_Rec; 
                    Estado_Comm = Validando_Comando;
                    break;
                }
            break;
            
            case Validando_Comando:
                    
            /* Se podría poner que cada cierto tiempo los comandos "manuales se borren, como para que no queden
                girando, o moviendose indefinidamente según ese comando recibido. Creo que un polling de 10 ms
                nos ayudaria aca. No debe afectar a los comandos de posicionamiento. 
            */
                Comando_Procesado.Proximo_Comando = Verificando_Comando();

                if(Comando_Procesado.Proximo_Comando != Comando_No_Valido){
                    uart_ringBuffer_envDatos_U2(Mensaje_Recibido_Correcto,sizeof(Mensaje_Recibido_Correcto));
                    strcpy(Comando.Ultimo_Comando_Almacenado,Comando_Recibido);
                    Comando_Procesado.Comando_Actual = Comando_Procesado.Proximo_Comando;
                }else{
                    Estado_Comm = Comando_No_Reconocido;
                    break;
                }
                
                Estado_Comm = Esperando_Datos;
            break;
                
            case Comando_No_Reconocido:
                uart_ringBuffer_envDatos_U2(Mensaje_Error,sizeof(Mensaje_Error));
                Estado_Comm = Esperando_Datos;
            break;
            
            default: Estado_Comm = Esperando_Datos;
        }
}
