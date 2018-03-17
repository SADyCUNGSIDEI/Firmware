#define MAIN
#include <SimpleTimer.h>      //Biblioteca para setear tareas controladas por timer
#include <Wire.h>             //Biblioteca para manejo del bus I2C

#include "Globales.h"
#include "Nucleo.h"
#include "Comunicacion.h"
#include "Interrupciones.h"

void setup() {
  Serial.begin(115200);                 //Para comunicación serial (TX0, RX0) vía conector USB

  Wire.begin();  

  pinMode(TST, OUTPUT);

  digitalWrite(TST, LOW);

  estadoEspera = false;
  k_g = 0;
  inString = "";
  wifiBuffer = "";
  wifiSendTimeout = 0;
}

void loop() {
  timer.run();

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