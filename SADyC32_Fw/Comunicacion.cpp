#include "Arduino.h"
#include "Globales.h"
#include "Nucleo.h"
#include "Reloj.h"
#include "Memorias.h"
#include "Comunicacion.h"
#include "Interrupciones.h"
#include "Adquisicion.h"
#include "Wifi.h"

// Función que sirve para parsear los carácteres que entran por el puerto serial
// inicio: Posición donde se comienza a leer el auxbuffer
// cantidad: Cantidad de carácteres a leer en auxbuffer
int parseValor(int inicio, int cantidad) {
  int val = 0;
  int pot = cantidad - 1;
  int hasta = inicio + cantidad;

  for (int i = inicio; i < hasta + 1; i++) { 
    val += auxbuffer[i] * pow(10, pot--);
  }
  return val;
}

void serialOutput(String data, bool newLine) {
  switch (serialActivo) {
    case SERIAL_0:
      Serial.print(data);
      if (newLine == true) { Serial.println(""); }
    break;
    case SERIAL_1:

    break;
    case SERIAL_2:
    // Serial.println("Transmision RS485");
    //   digitalWrite(RS485_DIR, RS485_Tx);
    //   Serial2.print(data);
    //   if (newLine == true) { Serial2.println(""); }
    //   digitalWrite(RS485_DIR, RS485_Rx);
    break;
    case SERIAL_3:
      // wifiSendTimeout = 1000;
      // http(data);
      // if (newLine == true) { http("\r\n"); }
      // delay(10);
    break;
    default:
    break;
  }
}

//Maquina de estados que interpreta la secuencia de un comando por RS232
//Recibe cada dato en secuencia hsta que encuentra el fin de comando
//O sea, el formato es: -->   ESC  Comando  CR
//Si el comando no se completa o no existe se va por tiempo
//Si el comando existe ejecuta en consecuencia
void parsing(int comando) {
  int tiempo;
  unsigned long aux;

  switch (comando) {
    case '1':                               //Test de los chips de EEProm instalados -( ESC 1 Valor CR ) - Valor => 0 a 255
      byte data;
      
      data = parseValor(1,3);
      eeTest(data);
    break;
    case '2':                               //Test de la comunicación RS232 - ( ESC 2 Char CR ) - Char => cialquier caracter ASCII
      serialOutput(inString, true);
    break;
    case 'A':                               //Recibe cantidad de canales analógicos - ( ESC A Cantidad CR) - Cantidad => 1 a 8
      aux = inString.toInt();
      setCanAnalog1(aux);
    break;
    case 'B':                               //Recibe cantidad de canales analógicos tipo inAmp - ( ESC B Cantidad CR) - Cantidad => 1 a 4
      aux = inString.toInt();
      setCanAnalog2(aux);
    break;
    case 'C':                               //Seteo de la amplificación de los inAmp - (ESC C Entrada Amplificación CR) - Entrada => 09 a 12 - Amplificación => 0 a 3
      byte in_inAmp, amplif;

      in_inAmp = parseValor(1,2);
      amplif = parseValor(3,1);

      setInAmp(in_inAmp, amplif);
    break;
    case 'D':                               //Medición de tiempo entre eventos - ( ESC D Pin1 Pin2 CR ) - Pin1 y Pin2 => patas válidas, puede ser la misma
        pin1 = parseValor(1, 2);            //Patas válidas:  46, 47, 49, 50, 51, 52, 53
        pin2 = parseValor(3, 2);

        setMedicion(pin1, pin2);
    break;
    case 'E':                               //Recibe el modo de trabajo del equipo - ( ESC E Modo CR ) - Modo => 1 a 5
        aux = inString.toInt();
        setModo(aux);
    break;
    case 'F':                               //Borrado EEProm - ( ESC F CR )
        if(modo == 1) {
          borrarE2();
        }
    break;
    case 'H':                               //Seteo del reloj de tiempo real por I2c - ( ESC H hhmmssDDMMAAAA CR )
      byte hora;
      byte minuto;
      byte segundo;
      byte dia;
      byte mes;
      int anio;

      hora = parseValor(1,2);
      minuto = parseValor(3,2);
      segundo = parseValor(5,2);
      dia = parseValor(7,2);
      mes = parseValor(9,2);
      anio = parseValor(13,2);
    
      setFechaHora(hora, minuto, segundo, dia, mes, anio);
    break;
    case 'M':                               //Generación de PWM - ( ESC M Pin Duty CR) - Pin => 10 al 13 - Duty => 0000 a 4095
      int pinPwm;
      int dtyCicle;

      pinPwm = parseValor(1, 2);
      dtyCicle = parseValor(3, 4);

      setPwm(pinPwm, dtyCicle);
    break;
    case 'N':                               //Generación de señales analógicas en DACs - ( ESC N Pin Steps CR ) - Pin => 00 al 01 - Steps => 0000 a 4095
      int pinDac;
      int valor;

      pinDac = parseValor(1, 2);
      valor = parseValor(3, 4);

      setDac(pinDac, valor);
    break;
    case 'O':                               //Transmite datos de un archivo en memoria SD - ( ESC O Archivo CR ) - Archivo en formato --> 15.3 máximo
      leerArchSD(inString);
    break;
    case 'P':                               //Parada de los modos del equipo - ( ESC P CR )
      parada();
    break;
    case 'Q':                               //Graba un dato fijo en un archivo en memoria SD - ( ESC Q Archivo CR ) - Archivo en formato --> 15.3 máximo
      grabarDatoSD(inString, 'a');
    break;
    case 'R':                               //Registro temporizado de los canales digitales y analógicos seteados - ( ESC R Tiempo CR ) - Tiempo => 1 a 3600 segundos
      tiempo = inString.toInt();
      registroTemporizado(tiempo);
    break;
    case 'S':                               //Seteo de la salida digital - (ESC S Salida Estado CR) - Salida => 02 a 09 - Estado 0 o 1
      byte pinDig;
      bool estado;

      pinDig = parseValor(1,2);
      estado = parseValor(3,2);

      setSalDig(pinDig, estado);
    break;
    case 'T':                               //Transmisión temporizada de los canales digitales y analógicos seteados - ( ESC T Tiempo CR ) - Tiempo => 1 a 10000 milisegundos || Tiempo=0 -> una transmisión
      tiempo = inString.toInt();            //(En los datos analógicos se transmite primero el byte alto y luego el bajo)
      transmisionTemporizada(tiempo, false);
    break;
    case 'W':                               //Seteo de Red y Password para WiFi - (ESC W Red 0xFF Pass 0xFF CR) - Red y Pass máximo 15 caracteres ASCII
      Serial.println(inString);
      configWiFiParams();
    break;
    case 'd':                               //Descarga de datos desde la EEProm - ( ESC d CR )
      parada();
      descargaE2();
      setCodigoInicio(0x02);                //Guarda en reloj código de descarga
    break;
    case 'e':                               //Transmite datos del equipo - ( ESC e CR ) - Responde con los datos básicos del proyecto
      datosEquipo();
      espGetIP();
    break;
    case 'h':                               //Transmite día y hora del RTC - ( ESC h CR ) - Responde con día y hora del equipo
      serialOutput(getFechaHora(), true);
    break;
    case 'g':
      saludo();
    break;
    case 'k':
      tiempo = inString.toInt();
      transmisionTemporizada(tiempo, true);
    break;
  }
}

void InComando(char inChar) {
  // Si estadoEspera = true evalúa los caracteres recibidos
  if (estadoEspera) {
    comm0_out = TIME_OUT;
    // Si es el primer carácter, es el comando a ejecutar
    if (k_g == 0) {
      comando = inChar;
    }
    else {
      inString += inChar;
      if (isDigit(inChar)) {
        if (k_g < VECT_END) {
          inChar = inChar - 48;
          auxbuffer[k_g] = inChar;
        }
      }
    }
    k_g++;
  }

  // Si se recibe el carácter de ESCAPE estadoEspera = true
  if (inChar == ESCAPE) {
    comm0_out = TIME_OUT;
    estadoEspera = true;
  }

  // Si es IN_END y el time out no es 0 llama a parsing para ejecutar el comando
  if ((inChar == IN_END) && (comm0_out != 0)) {
    parsing(comando);
    inString = "";
    comm0_out = 0;
    k_g = 0;
  }
}

void serialInput(HardwareSerial &port) {
   if (port.available()) {
     if (serialActivo == SERIAL_0) {
       InComando(port.read());  
     }
   }
}
