#define MAIN
#include <SimpleTimer.h>      //Biblioteca para setear tareas controladas por timer
#include <Wire.h>             //Biblioteca para manejo del bus I2C

#include "Globales.h"
#include "Nucleo.h"
#include "Comunicacion.h"
#include "Interrupciones.h"
#include "Adquisicion.h"

void setup() {
  Serial.begin(delta_B_S0);                 //Para comunicación serial (TX0, RX0) vía conector USB
  Serial3.begin(delta_B_WiFi);                //Para comunicación por WiFi (TX3, RX3) (ESP8266)

  Wire.begin();  

  pinMode(TST, OUTPUT);

  digitalWrite(TST, LOW);

  pinMode(CS1, OUTPUT);
  digitalWrite(CS1, HIGH);
  pinMode(HOLD, OUTPUT);
  digitalWrite(HOLD, HIGH);
  pinMode(WP, OUTPUT);
  digitalWrite(WP, HIGH);

  pinMode(CS2, OUTPUT);
  digitalWrite(CS2, HIGH);
  pinMode(WP_SD, INPUT);
  pinMode(CD, INPUT);

  estadoEspera = false;
  k_g = 0;
  inString = "";
  wifiBuffer = "";
  wifiSendTimeout = 0;

  analogReference(AR_DEFAULT);                            //VER COMO SE SETEA LA REFERENCIA EXTERNA !!!!!!!
  REG_ADC_MR = (REG_ADC_MR & 0xFFF0FFFF) | 0x00020000;    //Para convertir en 6uS aprox. en vez de 40uS.
  REG_ADC_MR |= 0xC0;                                     //Modo: free running (página 1333 del manual del Sam3X) 
  mask_Can1 = 0x00FF;                                     //Default
  mask_Can2 = 0x3C00;                                     //Default
  analogReadResolution(MAX_RESOL);
  analogWriteResolution(MAX_RESOL);

  iniVar();
  setADCChan();                                         //Seteo en el ADC de los canales elegidos. Solo barre esos.
  REG_ADC_CR=0x02;                                        //Comienzo de la conversión 
}

void loop() {
  if (comm0_out != 0) {
    digitalWrite(TST, HIGH);
    comm0_out--;
  }
  else {
    digitalWrite(TST, LOW);
    estadoEspera = false;
    vaciarAuxBuffer();
  }
  
  checkMedicion();
}

void serialEvent() {
  serialActivo = SERIAL_0;
  serialInput(Serial);
}

// RS232
void serialEvent1() {
  serialActivo = SERIAL_1;
  serialInput(Serial1);
}
// RS485
void serialEvent2() {
  serialActivo = SERIAL_2;
  serialInput(Serial2);
}

//--------------------------------------------------------------------------------------------------
//Evento serial por el que se reciben datos por WiFi (Placa ESP8266)
void serialEvent3() {
  serialActivo = SERIAL_3;
  serialInput(Serial3);
}