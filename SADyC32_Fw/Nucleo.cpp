#include <SD.h>               //Biblioteca para manejo de la memoria SD/MMC
#include <SerialFlash.h>      //Biblioteca para manejo de la memoria Flash
#include <DueTimer.h>              //Biblioteca para setear tareas controladas por timer 
#include "Arduino.h"
#include "Globales.h"
#include "Comunicacion.h"
#include "Reloj.h"
#include "Wifi.h"

//Impresión de los datos básicos del equipo
void datosEquipo(void) {
  serialOutput(F(proyecto), true);
  serialOutput(F(equipo), true);
  serialOutput(F(vers), true);
  serialOutput(F(fecha), true);
}

void setModo(byte mode) {
  if (mode < 1 || mode > 5) { mode = 1; }
  modo = mode;
  rtcWrite(RTC_M_MODO, modo);
}

byte getModo(void) {
  modo = rtcRead(RTC_M_MODO);
  if (modo > 5) { modo = 1; }
  return modo;
}

// Setea la cantidad de canales analógicos y los guarda en la RAM del RTC
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
// Setea la cantidad de canales analógicos de tipo inAmp y los guarda en la RAM del RTC
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

// Setea el puntero de memoria de la EEPROM (I2C)
void setPumem(unsigned long pmem) {
  pumem = pmem;
  rtcWrite(RTC_M_ST + 1, byte(pmem>>0));
  rtcWrite(RTC_M_ST + 2, byte(pmem>>8));
  rtcWrite(RTC_M_ST + 3, byte(pmem>>16));
  rtcWrite(RTC_M_ST + 4, byte(pmem>>24));
}

// Lee el puntero de memoria de la EEPROM (I2C) que se encuentra grabado en la RAM del RTC
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

//Permite setear un PWM en el pin seleccionado ((pinPwm) de 10 a 13) con el Duty-Cicle (dtyCicle)
void setPwm(byte pin, int dtyCicle) {
  if(pin > 9 && pin < 14) {               //No se usa el pin13 (led) ni los pines 0 y 1 (Rx0 y Tx0).
    analogWrite(pin, dtyCicle);
  }
}

//Setea el DAC seleccionado ((pinDac) de 00 a 01) con un valor específico (Valor)
void setDac(byte pin, int valor) {
  if(pin > 65 && pin < 68){
    analogWrite(pin, valor);
  }
}

//Permite setear un pin de salida digital ((pinDig) de 02 a 09) con el ((estado) 0 o 1)
void setSalDig(byte pinDig, bool estado) {
  if (pinDig > 1 && pinDig < 10) {               //No se usa el pin13 (led) ni los pines 0 y 1 (Rx0 y Tx0).
    digitalWrite(pinDig, estado);
  }
}

//Permite setear la amplificación de los inAmps. Entrada analógica ((in_inAmp) de 09 a 12)
//con la amplificación ((amplif) 0 a 3)
void setInAmp(byte in_inAmp, byte amplif) {
  if((in_inAmp > 8 && in_inAmp < 13) && amplif < 4) {
    in_inAmp = in_inAmp + 27;               
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
    digitalWrite(in_inAmp,LOW);
    delayMicroseconds(1);    
    digitalWrite(in_inAmp,HIGH);
  }
}

//Rutina que lee los canales digitales de entrada y los unifica en un solo byte "digi_in"
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

//Para todos los modos de trabajo, timers y desacopla interrupciones
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

//Inicialización de todas las variables para una correcta ejecución del programa
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


// Función para vaciar el auxbuffer
void vaciarAuxBuffer() {
  for (int i = 1; i < (int)sizeof(auxbuffer); i++) {
    auxbuffer[i] = 0;
  }
}