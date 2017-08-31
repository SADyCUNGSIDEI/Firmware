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

  //rtc_write(rtc_m_st + 7, 0x00);                              //Guarda en reloj código de inicio
  ini_var();
  Scheduler.startLoop(Adc_Srv);
  
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
//Inicialización de todas las variables para una correcta ejecución del programa

void ini_var(void){
  modo = 1;
  cantCanAnalog1 = 1;
  /*pumem = 0;
  pumem = pumem + (unsigned long)rtc_read(rtc_m_st + 1);
  pumem = pumem + ((unsigned long)rtc_read(rtc_m_st + 2) << 8);  
  pumem = pumem + ((unsigned long)rtc_read(rtc_m_st + 3) << 16);  
  pumem = pumem + ((unsigned long)rtc_read(rtc_m_st + 4) << 24);  */
  
  //reg_flag = rtc_read(rtc_m_st + 8);
  tiempo_reg = 0;
/*  tiempo_reg = tiempo_reg + (unsigned int)rtc_read(rtc_m_st + 9);
  tiempo_reg = tiempo_reg + ((unsigned int)rtc_read(rtc_m_st + 10) << 8);  */


//  contCan_analog1 = 0;
//  contCan_analog1 = 0;
  inString = "";
  Wifi_ini = false;

  archivo.reserve(LARGO_ARCH);

  ini_var_def();                        //COMENTAR PARA VERSION DEFINITIVA !!!!!!!

  if(reg_flag == 1) {
    graba_ini_E2();                                             //Almacena en EEProm encabezado por vuelta de alimentación
    id_Registro_Temp = timer.setInterval(tiempo_reg * 1000, Registro_Temp_E2);
  }
  if(ini_Wifi()){
    Serial.println("WiFi ESP8266 inicializada");
    Wifi_ini = true;
  }else{
    Serial.println("Wifi ESP8266 no encontrada");
  }
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Inicialización de todas las variables para una correcta ejecución del programa

void ini_var_def(void){
  //cantCan_analog1 = 8;
  //cantCan_analog2 = 4;
  //modo = 1;
  reg_flag = 0;
}
//--------------------------------------------------------------------------------------------------
//Rutina que lee el ADC en secuencia
//Cada vez que ejecuta lee el siguiente canal hasta el último seteado
void Adc_Srv(){
  adcSrv();
  yield();
}

//--------------------------------------------------------------------------------------------------
//Testea cuantos chips de memoria EEProm hay instalados, escribiendo y lledendo en cada uno el dato ingresado por el usuario

void test_eeprom(byte data){
  unsigned long aux;
  byte i;

    aux = 32000;          
    for(i=1;i<=2;i++) {
      e2prom_write(aux, data);
      aux = aux + 65536;
    }
    delay(10);
    aux = 32000;          
    for(i=1;i<=2;i++){
      data = e2prom_read(aux);
      Serial.print(F("Block "));
      Serial.print(i);
      Serial.print(F(" - Memoria "));
      if(aux<100000){Serial.print(" ");}
      Serial.print(aux);
      Serial.print(F(" --> "));
      if(data<100){Serial.print(" ");}
      if(data<10){Serial.print(" ");}
      Serial.println(data);
      aux = aux + 65536;
    }  
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que graba un datos en la EEProm y guarda el nuevo pumem en la ram del reloj
//Data --> dato a grabar - Se asume que "pumem" apunta al primer valor libre
void graba_dato_E2(byte data) {

  if(mem_full == 0){
      e2prom_write(pumem, data);
      pumem++;
      if(pumem >= MEM_FIN) {
        mem_full = 1;
        parada();
      }
      //rtc_write(rtc_m_st + 1, byte(pumem>>0));
      //rtc_write(rtc_m_st + 2, byte(pumem>>8));
      //rtc_write(rtc_m_st + 3, byte(pumem>>16));
      //rtc_write(rtc_m_st + 4, byte(pumem>>24));
  }
}

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que borra la EEProm e inicializa el puntero de registro "pumem" a cero
void borrado_E2(void) {
    parada();
    e2prom_erase();
    pumem = 0;
    //rtc_write(rtc_m_st + 1, byte(pumem>>0));
    //rtc_write(rtc_m_st + 2, byte(pumem>>8));
    //rtc_write(rtc_m_st + 3, byte(pumem>>16));
    //rtc_write(rtc_m_st + 4, byte(pumem>>24));
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que graba los datos de inicialización de registro en la EEProm
void graba_ini_E2(void) {
  int i;
  char buf[6];

  sprintf(buf,"%04d",tiempo_reg);
//  get_time();
  graba_dato_E2(0xff);
  graba_dato_E2(0xff);
  for(i=1;i<=10;i++){
    graba_dato_E2(auxbuffer[i]);
  }
  graba_dato_E2(0x32);
  graba_dato_E2(0x30);
  graba_dato_E2(auxbuffer[11]);
  graba_dato_E2(auxbuffer[12]);
  for(i=0;i<=3;i++){
    graba_dato_E2(buf[i]);
  }
  graba_dato_E2(modo);
  //graba_dato_E2(cantCan_analog1);
  //graba_dato_E2(cantCan_analog2);
  graba_dato_E2(0x08);
  //graba_dato_E2(rtc_read(rtc_m_st + 7));
  graba_dato_E2(0xff);
  graba_dato_E2(0xff);
}

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina de registro temporizado en EEProm (I2C) de los canales y el modo seteados
//Pumem apunta a la primera posición libre de la memoria
void Registro_Temp_E2(void) {
 // int i;

  Serial.println(F("Rutina de registro"));

  //analog_foto();

  graba_dato_E2(leerCanalesDigitales());                       //Garaba estado de los 8 canales digitales
/*
  if(modo == 2) {
    for(i=0;i<cantCan_analog1;i++){
      graba_dato_E2(byte(vect_aux1[i]));       //Graba valores de los canales analógicos seteados
      graba_dato_E2(byte(vect_aux1[i] >> 8));
    }
  }
  if(modo == 4) {
    for(i=0;i<cantCan_analog2;i++){
      graba_dato_E2(byte(vect_aux2[i]));       //Graba valores de los canales analógicos inAmp seteados
      graba_dato_E2(byte(vect_aux2[i] >> 8));
    }
  }*/
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------

void registro_temporizado(int tiempo_reg) {
    if(modo == 2 || modo == 4) {
    if(tiempo_reg>=1 && tiempo_reg<=3600) {
      parada();
      //rtc_write(rtc_m_st + 7, 0x01);                              //Guarda en reloj código de inicio
      //rtc_write(rtc_m_st + 9, byte(tiempo_reg>>0));               //Guarda en reloj tiempo de registro
      //rtc_write(rtc_m_st + 10, byte(tiempo_reg>>8));              //Guarda en reloj tiempo de registro
      reg_flag = 1;
      //rtc_write((rtc_m_st + 8), reg_flag);                        //Guarda en reloj flag de registro
      graba_ini_E2();                                             //Almacena en EEProm encabezado de inicio de registro
      id_Registro_Temp = timer.setInterval(tiempo_reg * 1000, Registro_Temp_E2);
    }          
  }
}
//--------------------------------------------------------------------------------------------------
//Rutina que descarga los datos grabados en la EEProm y los trnsmite por RS232
void descarga_E2(void){
  unsigned long aux_p;

  Serial.write(byte(pumem>>24));
  Serial.write(byte(pumem>>16));
  Serial.write(byte(pumem>>8));
  Serial.write(byte(pumem>>0));

  for(aux_p=0;aux_p<pumem;aux_p++){
    Serial.write(e2prom_read(aux_p));
  }
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//                         Rutinas para memoria SD
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que agrega un dato en un archivo en la memoria SD
//archivo --> nombre del archivo en donde se desea guardar el dato.
//data --> dato a guardar

bool grabo_dato_sd(String archivo, byte data){
  String dataString = "";

  dataString += String(data);
  File dataFile = SD.open(archivo, FILE_WRITE);

  if (dataFile) {
    dataFile.print(dataString);
    dataFile.close();
    Serial.println(dataString);
    return true;
  }
  else {
    Serial.print("Error al abrir ");
    Serial.println(archivo);
    return false;
  }
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que lee un archivo de la memoria SD y lo manda por RS232
//archivo --> nombre del archivo en donde se desea guardar el dato.

bool leo_arch_sd(String archivo){
  
  File dataFile = SD.open(archivo, FILE_READ);

  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
    return true;

  } else {
    Serial.print(F("Error al abrir "));
    Serial.println(archivo);
    return false;
  }
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//                    Rutinas para memoria FLASH EEPROM (SPI) - S25FL127S
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que agrega un dato en un archivo en la memoria Flash
//archivo --> nombre del archivo en donde se desea guardar el dato. Si no existe lo crea.
//data --> dato a guardar

bool grabo_dato_flash(char *archivo, byte data){
  char buff[256];

  buff[0] = data;
  if (SerialFlash.exists(archivo) == false) {
        SerialFlash.create(archivo, 256);
  }

  SerialFlashFile S_File = SerialFlash.open(archivo);
  if (S_File) {
    S_File.write(buff, 1);
    S_File.close();
    Serial.println(buff);
    return true;
  } else {
    Serial.print(F("Error al abrir "));
    Serial.println(archivo);
    return false;
  }
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que lee un archivo de la memoria Flash y lo manda por RS232
//archivo --> nombre del archivo en donde se desea guardar el dato.
//cant_datos --> cantidad de datos a leer (0 --> todos)

bool leo_arch_flash(char *archivo){

  if (SerialFlash.exists(archivo)) {
    SerialFlashFile S_File = SerialFlash.open(archivo);
  
    if (S_File) {
      Serial.println(archivo);
      char buffer[256];
      S_File.read(buffer, 256);
      Serial.print(buffer);
      S_File.close();
      return true;
    } else {
      Serial.print(F("Error al abrir "));
      Serial.println(archivo);
      return false;
    }
  } else {
    Serial.print(archivo);
    Serial.println(F(" no existe"));
    return false;
  }    
}
 
//--------------------------------------------------------------------------------------------------
//Rutina que borra toda la Flash

void borro_flash(void) {

  SerialFlash.eraseAll();
  
  while (SerialFlash.ready() == false) {
     // Loop de espera hasta que se borre. Puede tardar de 30 seg. a 2 min.
  }
}
 
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//                    Rutinas para memoria EEPROM (I2C) - 24AA1025
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que escribe un dato en la E2
//e2addr --> dirección completa de puntero a banco de memoria en 4 bytes
//data --> datos a guardar

void e2prom_write(unsigned long e2_addr, byte data){
  byte memaddl, memaddh,i; 
  
  memaddl = (byte) (e2_addr & 0xff);
  memaddh = (byte) ((e2_addr >> 8) & 0xff);
  i = (byte) ((e2_addr >> 13) & 0x08);
  i = i | mem_ad;
  i = i >> 1;
  digitalWrite(WP1, LOW);
  Wire.beginTransmission(i);
  Wire.write(memaddh);                
  Wire.write(memaddl);                
  Wire.write(data);                
  Wire.endTransmission(true);
  delay(10);    
  digitalWrite(WP1, HIGH);
}  
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que lee un dato de la E2
//e2addr --> dirección completa de puntero a banco de memoria en 4 bytes
//Devuelve byte leído

byte e2prom_read(unsigned long e2_addr){
  byte memaddl, memaddh, i, c; 
  
  memaddl = (byte) (e2_addr & 0xff);
  memaddh = (byte) ((e2_addr >> 8) & 0xff);
  i = (byte) ((e2_addr >> 13) & 0x08);
  i = i | mem_ad;
  i = i >> 1;
  Wire.beginTransmission(i);
  Wire.write(memaddh);                
  Wire.write(memaddl);                
  Wire.endTransmission(false);    
  Wire.requestFrom((int)i, 1, true); 
  c = Wire.read();
  return c;    
}  
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Borrado rápido de la memoria E2

void e2prom_erase(void){
  byte memaddl, memaddh, i, data, k; 
  unsigned long e2_addr;
  
  data = 0xff;
  e2_addr = 0;
  digitalWrite(WP, LOW);
  do{
    memaddl = (byte) (e2_addr & 0xff);
    memaddh = (byte) ((e2_addr >> 8) & 0xff);
    i = (byte) ((e2_addr >> 13) & 0x08);
    i = i | mem_ad;
    i = i >> 1;
    Wire.beginTransmission(i);
    Wire.write(memaddh);                
    Wire.write(memaddl);
    for(k=1;k<=64;k++){                
       Wire.write(data);
       e2_addr++;                
    }
    Wire.endTransmission(true);    
    delay(10);
  }while(e2_addr < MEM_FIN);
  digitalWrite(WP, HIGH);
}
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//                             FIN DEL PROGRAMA
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------


