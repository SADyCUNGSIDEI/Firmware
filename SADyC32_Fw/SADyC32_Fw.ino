//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//IDEI - UNGS
//Proyecto SADyC32
//Se utiliza la placa microcontroladora Arduino Due (uC: ATSam3X8E)
//--------------------------------------------------------------------------------------------------
/*Historia de cambios
 *
 * 04/05/2017 - Ver: 0.5
 * Se agregan las rutinas básicas de manejo de la SD
 * Se agrega una segunda rutina de recepción por RS232
 * para recibir el nombre del archivo para la SD
 * 
 *05/05/2017 - Ver: 0.6
 *Se emprolija secciones de rutinas
 *
 *11/05/2017 - Ver: 0.7
 *Se emprolijaron nombres de variables para que empiecen con minúscula
 *
 *26/05/2017 - Ver: 0.8 
 *Se agregan rutinas de la memoria EEprom-SPI: S25FL127S (NO ESTAN PROBADAS!!!!!)
 *Se agrega timer (Timer1) para registro y comando "R" para iniciar el registro de datos
 *
 *29/05/2017 - Ver: 0.9
 *Se agrega la configuración para la comunicación RS485 sobre puerto serial 2 (TX2, RX2)
 *Se agrega un evento serial-2 para la recepción por RS485 
 *
 *01/06/2017 - Ver: 1.0
 *Se agrega manejo de salidas digitales con el comando "S"
 *
 *06/06/2017 - Ver: 1.1
 *Se agrega seteo de ganancia de los inAmps
 *Se agrega el manejo de "modo" y se dividen los canales analógicos
 *en 8 y 4. Tanto el ADC como las trnsmisiones se adecuaron a esto.
 *
 *08/06/2017 - Ver: 1.2
 *Se emprolijan comentarios de comandos
 *
 *09/06/2017 - Ver: 1.3
 *Se modifica la rutina Transm_Temp para que si el tiempo=0 transmita los estados de los canales
 *una sola vez
 *
 *11/06/2017 - Ver: 1.4
 *Se agregan las rutinas de registro en EEProm, de acuerdo a los canales y modo seteados.
 *Se resguardan las variables importantes en la ram del reloj
 *
 *12/06/2017 - Ver: 1.5
 *Se agrega rutina de descarga de datos de la EEProm por RS232
 *
*13/06/2017 - Ver: 1.6
 *Se cambia la rutina del ADC para barrido completo de canales por cada ejecución de la misma
 *
*14/06/2017 - Ver: 1.7
 *Se eliminan variables no usadas 
 *
*15/06/2017 - Ver: 1.7.1
 *y se elimina doble definicion de MEM_FIN
 *
*16/06/2017 - Ver: 1.7.2
 *Se agrega seteo de modo de pines de entradas digitales
 *Se corrige la rutina "Adc_Srv" para que tome bien los canales superiores (del 8 al 11)(inAmp)
 *
*20/06/2017 - Ver: 1.7.3
 *Se agregan comentarios y se pasan mensajes a flash memory
 *
*25/06/2017 - Ver: 1.7.4
 *Se agrega rutina de inicialización y comunicación serial para Wifi ESP8266 
 *
*04/07/2017 - Ver: 1.7.5
 *Se emprolija inicialización de Wifi con vector de comandos
 *
*23/08/2017 - Ver: 1.7.6
 *Se emprolija Comentarios de comando "T"
 *
*30/08/2017 - Ver: 1.7.7
 *Se empezó la modularización del código en archivos. Se mejoró la máquina de estados para prescindir 
 *de algunas funciones, Se mejoró la rutina de conexión a WiFi. Se implementó una transmisión
 *temporizada de los canales analógicos por WiFi (En prueba)
 *
*
*/
//--------------------------------------------------------------------------------------------------
#include <SimpleTimer.h>      //Biblioteca para setear tareas controladas por timer
#include <Scheduler.h>        //Biblioteca para schedulear tareas independiente del lazo principal
#include <Wire.h>             //Biblioteca para manejo del bus I2C
#include <SPI.h>              //Biblioteca para manejo del bus SPI
#include <SD.h>               //Biblioteca para manejo de la memoria SD/MMC
#include <SerialFlash.h>      //Biblioteca para manejo de la memoria Flash 

#include "Globales.h"
#include "Utilidades.h"
#include "Memorias.h"
#include "Salida.h"
#include "Reloj.h"
#include "Interrupciones.h"
#include "Wifi.h"
#include "SADyC32.h"

//--------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);                 //Para comunicación serial (TX0, RX0) vía conector USB

  Serial1.begin(115200);                 //Para comunicación serial (TX1, RX1) vía conector RS232
    
  pinMode(RS485_DIR, OUTPUT);     
  digitalWrite(RS485_DIR, RS485_Rx);    //Prepara para recepción por RS485
  Serial2.begin(4800);                  //Para comunicación por RS485 (TX2, RX2)
  
  Serial3.begin(115200);                //Para comunicación por WiFi (TX3, RX3) (ESP8266)

  Wire.begin();
  
  pinMode(TST, OUTPUT);
  digitalWrite(TST, LOW);

  pinMode(DIG36, OUTPUT);                 
  pinMode(DIG37, OUTPUT);
  pinMode(DIG38, OUTPUT);
  pinMode(DIG39, OUTPUT);
  pinMode(DIG40, OUTPUT);
  pinMode(DIG41, OUTPUT);

  pinMode(DIN0, INPUT);
  pinMode(DIN1, INPUT);
  pinMode(DIN2, INPUT);
  pinMode(DIN3, INPUT);
  pinMode(DIN4, INPUT);
  pinMode(DIN5, INPUT);
  pinMode(DIN6, INPUT);
  pinMode(DIN7, INPUT);

  pinMode(A8, INPUT_PULLUP);                 

  pinMode(WP1, OUTPUT);                 
  digitalWrite(WP1, HIGH);
  
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
 
  if (!SD.begin(CS2)) {
    Serial.println(F("Tarjeta SD no presente, sin formato  o fallada"));
//    return;
  } else {
    Serial.println(F("card initialized."));
  }

  if (!SerialFlash.begin(CS1)) { 
    Serial.println(F("No se puede acceder a la memoria Flash por SPI")); 
  } else {
    Serial.println(F("Flash encontrada e inicializada."));
  }
  
  analogReference(AR_DEFAULT);                            //VER COMO SE SETEA LA REFERENCIA EXTERNA !!!!!!!
  REG_ADC_MR = (REG_ADC_MR & 0xFFF0FFFF) | 0x00020000;    //Para convertir en 3,97uS aprox. en vez de 40uS.
  analogReadResolution(MAX_RESOL);
  analogWriteResolution(MAX_RESOL);

  Scheduler.startLoop(Adc_Srv);

  iniVar();
  saludo();
}
//--------------------------------------------------------------------------------------------------
/*void callFunction(int index, uint32_t c, uint8_t wait) {
  (*functionPtrs[index])(c, wait); //calls the function at the index of `index` in the array
}*/
//--------------------------------------------------------------------------------------------------
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

  yield();
}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//---------------------------     RUTINAS     ------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//Máquina de estados que recibe datos por RS232 (Terminal arduino)
//Valida la recepción de un comando

void serialEvent() {
  serialManager(Serial);
}

//--------------------------------------------------------------------------------------------------
//Rutina que lee el ADC en secuencia
//Cada vez que ejecuta lee el siguiente canal hasta el último seteado
void Adc_Srv() {
  adcSrv();
  yield();
}
