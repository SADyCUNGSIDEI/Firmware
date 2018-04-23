#include <SD.h>               //Biblioteca para manejo de la memoria SD/MMC
#include <SerialFlash.h>      //Biblioteca para manejo de la memoria Flash
#include <DueTimer.h>              //Biblioteca para setear tareas controladas por timer 
#include "Arduino.h"
#include "Globales.h"
#include "Comunicacion.h"
#include "Reloj.h"
#include "Wifi.h"
#include "Memorias.h"
#include "Adquisicion.h"
#include "Interrupciones.h"

void datosEquipo(void) {
  serialOutput(F(proyecto), true);
  serialOutput(F(equipo), true);
  serialOutput(F(vers), true);
  serialOutput(F(fecha), true);
}

void setModo(byte mode) {
  if (mode < 1 || mode > 4) { mode = 1; }
  modo = mode;
  rtcWrite(RTC_M_MODO, modo);
}

byte getModo(void) {
  modo = rtcRead(RTC_M_MODO);
  if (modo > 4) { modo = 1; }
  return modo;
}

void setCanAnalog1(byte cant) {
  if (cant < 1 || cant > 8) { cant = 1; }
  cantCanAnalog1 = cant;
  rtcWrite(RTC_M_CCA1, cant);
}

byte getCanAnalog1(void) {
  cantCanAnalog1 = rtcRead(RTC_M_CCA1);
  if (cantCanAnalog1 > 8) { cantCanAnalog1 = 1; }
  return cantCanAnalog1;
}

void setCanAnalog2(byte cant) {
  if (cant < 1 || cant > 4) { cant = 1; }
  cantCanAnalog2 = cant;
  rtcWrite(RTC_M_CCA2, cant);
}

byte getCanAnalog2(void) {
  cantCanAnalog2 = rtcRead(RTC_M_CCA2);
  if (cantCanAnalog2 > 4) { cantCanAnalog2 = 1; }
  return cantCanAnalog2;
}

void setCodigoInicio(byte codigo) {
  rtcWrite(RTC_M_C_INI, codigo);
}

byte getCodigoInicio(void) {
  return rtcRead(RTC_M_C_INI);
}

void setRegFlag(byte regFlag) {
  rtcWrite(RTC_M_RF, regFlag);
}

byte getRegFlag(void) {
  return rtcRead(RTC_M_RF);
}

void setWiFiParams(void) {
  int i = 0, j = 0;
  
  while (inString[i] != 0xff) {
    rtcWrite(RTC_M_AP+i,inString[i]);
    i++;
  }
  rtcWrite(RTC_M_AP+i,NULL);
  
  i++;
  while (inString[i] != 0xff) {
    rtcWrite(RTC_M_PW+j,inString[i]);
    i++;
    j++;
  }
  rtcWrite(RTC_M_PW+j,NULL);
}

void getWifiParams(void) {
  for (int i=0; i<=15; i++) {
    nombreAP[i] = rtcRead(RTC_M_AP + i);
    claveAP[i] = rtcRead(RTC_M_PW + i);
  }
}

//Setea en la RAM del RTC el tiempo de registro
void setTiempoReg(unsigned int tiempoReg) {
  rtcWrite(RTC_M_ST + 9, byte(tiempoReg >> 0));
  rtcWrite(RTC_M_ST + 10, byte(tiempoReg >> 8));
}

//Lee el tiempo de registro y lo devuelve
unsigned int getTiempoReg(void) {
  unsigned int tiempoReg = 0;
  tiempoReg = tiempoReg + (unsigned int)rtcRead(RTC_M_ST + 9);
  tiempoReg = tiempoReg + ((unsigned int)rtcRead(RTC_M_ST + 10) << 8);
  return tiempoReg;
}

void setPumem(unsigned long pmem) {
  pumem = pmem;
  rtcWrite(RTC_M_ST + 1, byte(pmem>>0));
  rtcWrite(RTC_M_ST + 2, byte(pmem>>8));
  rtcWrite(RTC_M_ST + 3, byte(pmem>>16));
  rtcWrite(RTC_M_ST + 4, byte(pmem>>24));
}

unsigned long getPumem(void) {
  pumem = pumem + (unsigned long)rtcRead(RTC_M_ST + 1);
  pumem = pumem + ((unsigned long)rtcRead(RTC_M_ST + 2) << 8);  
  pumem = pumem + ((unsigned long)rtcRead(RTC_M_ST + 3) << 16);  
  pumem = pumem + ((unsigned long)rtcRead(RTC_M_ST + 4) << 24);
  return pumem;
}

void saludo(void) {
  for (int i=1;i<=4;i++){
    digitalWrite(TST,HIGH);
    delay(250);
    digitalWrite(TST,LOW);
    delay(250);
  }
  datosEquipo();
 serialOutput("Modo: ", false);
 serialOutput(String(modo), true);
 serialOutput("Cantidad canales analogicos: ", false);
 serialOutput(String(cantCanAnalog1), true);
 serialOutput("Cantidad canales analogicos inAmp: ", false);
 serialOutput(String(cantCanAnalog2), true);
 serialOutput("Pumem: ", false);
 serialOutput(String(pumem), true);
 serialOutput("Código de inicio: ", false);
 serialOutput(String(getCodigoInicio()), true);
 serialOutput("RegFlag: ", false);
 serialOutput(String(getRegFlag()), true);
}

void setPwm(byte pin, int dtyCicle) {
  if(pin > 9 && pin < 14) {               //No se usa el pin13 (led) ni los pines 0 y 1 (Rx0 y Tx0).
    analogWrite(pin, dtyCicle);
  }
}

void setDac(byte pin, int valor) {
  if(pin > 65 && pin < 68){
    analogWrite(pin, valor);
  }
}

void setSalDig(byte pinDig, bool estado) {
  if (pinDig > 1 && pinDig < 10) {               //No se usa el pin13 (led) ni los pines 0 y 1 (Rx0 y Tx0).
    digitalWrite(pinDig, estado);
  }
}

void setInAmp(byte inInAmp, byte amplif) {
  if((inInAmp > 8 && inInAmp < 13) && amplif < 4) {
    inInAmp = inInAmp + 27;               
    switch(amplif){
      case 0:
        digitalWrite(40,LOW);
        digitalWrite(41,LOW);
      break;
      case 1:
        digitalWrite(40,HIGH);
        digitalWrite(41,LOW);
      break;
      case 2:
        digitalWrite(40,LOW);
        digitalWrite(41,HIGH);
      break;
      case 3:
        digitalWrite(40,HIGH);
        digitalWrite(41,HIGH);
      break;
    }
    digitalWrite(inInAmp,LOW);
    delayMicroseconds(1);    
    digitalWrite(inInAmp,HIGH);
  }
}

byte leerCanalesDigitales(void) {
  byte digi_in;
  
  digi_in = 0x00 | byte(digitalRead(DIN0))
                 | (byte(digitalRead(DIN1)) << 1)
                 | (byte(digitalRead(DIN2)) << 2)
                 | (byte(digitalRead(DIN3)) << 3)
                 | (byte(digitalRead(DIN4)) << 4)
                 | (byte(digitalRead(DIN5)) << 5)
                 | (byte(digitalRead(DIN6)) << 6)
                 | (byte(digitalRead(DIN7)) << 7);
  return digi_in;
}

void parada(void) {
  Timer3.stop();
  Timer3.detachInterrupt();
  Timer4.stop();
  Timer4.detachInterrupt();
  Timer5.stop();
  Timer5.detachInterrupt();
  detachInterrupt(digitalPinToInterrupt(pin1)); 
  detachInterrupt(digitalPinToInterrupt(pin2));
  setRegFlag(0);
}

void iniVar(void) {
  setCodigoInicio(0x00);                              //Guarda en reloj código de inicio
  pumem = getPumem();
  modo = getModo();
  cantCanAnalog1 = getCanAnalog1();
  cantCanAnalog2 = getCanAnalog2();
  getWifiParams();

  SerialActivo serialActivo = NO_SERIAL;

  inString = "";

  archivo.reserve(LARGO_ARCH);

  /*if (getRegFlag() == 1) {
    grabaIniE2();                                             //Almacena en EEProm encabezado por vuelta de alimentación
    idRegistroTemp = timer.setInterval(getTiempoReg() * 1000, RegistroTempE2);
  }*/
  // Inicialización de la tarjeta SD
  if (!SD.begin(CS2)) {
    Serial.println(F("Tarjeta SD no presente, sin formato  o fallada"));
//    return;
  } else {
    Serial.println(F("card initialized."));
  }

  // Inicialización de la memoria flash (SPI)
  if (!SerialFlash.begin(CS1)) { 
    Serial.println(F("No se puede acceder a la memoria Flash por SPI")); 
  } else {
    Serial.println(F("Flash encontrada e inicializada."));
  }

  // Inicialización del Wi-Fi
  if (iniWifi()) {
    Serial.println("WiFi ESP8266 inicializada");
  } else {
    Serial.println("Wifi ESP8266 no encontrada");
  }
  saludo();
}

void vaciarAuxBuffer() {
  for (int i = 1; i < (int)sizeof(auxbuffer); i++) {
    auxbuffer[i] = 0;
  }
}

void ejecucion(int comando) {
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
      transmisionTemporizada(tiempo, EEPROM);
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
      // transmisionTemporizada(tiempo, false);
      transmisionTemporizada(tiempo, RS232);
    break;
    case 'W':                               //Seteo de Red y Password para WiFi - (ESC W Red 0xFF Pass 0xFF CR) - Red y Pass máximo 15 caracteres ASCII
      Serial.println(inString);
      setWiFiParams();
    break;
    case 'd':                               //Descarga de datos desde la EEProm - ( ESC d CR )
      parada();
      descargaE2();
      setCodigoInicio(0x02);                //Guarda en reloj código de descarga
    break;
    case 'e':                               //Transmite datos del equipo - ( ESC e CR ) - Responde con los datos básicos del proyecto
      datosEquipo();
    break;
    case 'h':                               //Transmite día y hora del RTC - ( ESC h CR ) - Responde con día y hora del equipo
      serialOutput(getFechaHora(), true);
    break;
    case 'g':
      saludo();
    break;
    case 'k':
      tiempo = inString.toInt();
      transmisionTemporizada(tiempo, WIFI);
    break;
  }
}