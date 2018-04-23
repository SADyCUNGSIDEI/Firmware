#include <Wire.h>             //Biblioteca para manejo del bus I2C
#include <SD.h>               //Biblioteca para manejo de la memoria SD/MMC
#include <SerialFlash.h>      //Biblioteca para manejo de la memoria Flash 

#include "Arduino.h"
#include "Globales.h"
#include "Nucleo.h"
#include "Memorias.h"
#include "Comunicacion.h"

void eeWrite(unsigned long e2Addr, byte data) {
  byte memaddl, memaddh; 
  
  memaddl = (byte) (e2Addr & 0xff);
  memaddh = (byte) ((e2Addr >> 8) & 0xff);
  digitalWrite(WP1, LOW);
  Wire.beginTransmission(EE_ADDR);
  Wire.write(memaddh);
  Wire.write(memaddl);
  Wire.write(data);
  Wire.endTransmission(true);
  delayMicroseconds(10000);
  digitalWrite(WP1, HIGH);
}

byte eeRead(unsigned long eeAddr) {
  byte memAddL, memAddH, c; 
  
  memAddL = (byte) (eeAddr & 0xff);
  memAddH = (byte) ((eeAddr >> 8) & 0xff);
//  i = (byte) ((eeAddr >> 13) & 0x08);
//  i = i | mem_ad;
//  i = i >> 1;
  Wire.beginTransmission(EE_ADDR);
  Wire.write(memAddH);                
  Wire.write(memAddL);                
  Wire.endTransmission(false);    
  Wire.requestFrom(EE_ADDR, 1, true); 
  c = Wire.read();
  return c;    
}

void eeErase(void) {
  byte memAddL, memAddH, data, k; 
  unsigned long eeAddr;

  data = 0xff;
  eeAddr = 0;
  digitalWrite(WP, LOW);
  do {
    memAddL = (byte) (eeAddr & 0xff);
    memAddH = (byte) ((eeAddr >> 8) & 0xff);
//    i = (byte) ((eeAddr >> 13) & 0x08);
//    i = i | mem_ad;
//    i = i >> 1;
    Wire.beginTransmission(EE_ADDR);
    Wire.write(memAddH);                
    Wire.write(memAddL);
    for(k=1;k<=64;k++){                
       Wire.write(data);
       eeAddr++;                
    }
    Wire.endTransmission(true);    
    delayMicroseconds(10000);
  } while(eeAddr < MEM_FIN);
  digitalWrite(WP, HIGH);
}

void eeTest(byte data) {
  unsigned long aux;
  byte i;

    aux = 32000;          
    for (i=1;i<=2;i++) {
      eeWrite(aux, data);
      aux = aux + 65536;
    }
    delayMicroseconds(10000);
    aux = 32000;          
    for(i=1;i<=2;i++) {
      data = eeRead(aux);
      serialOutput("Block", true);
      serialOutput(F("Block "), false);
      serialOutput(String(i), false);
      serialOutput(F(" - Memoria "), false);
      if(aux<100000){serialOutput(" ", false);}
      serialOutput(String(aux), false);
      serialOutput(F(" --> "), false);
      if(data<100){serialOutput(" ", false);}
      if(data<10){serialOutput(" ", false);}
      serialOutput(String(data), true);
      aux = aux + 65536;
    }  
}

void grabaDatoE2(byte data) {
  if (mem_full == 0) {
      eeWrite(pumem, data);
      pumem++;
      if (pumem >= MEM_FIN) {
        mem_full = 1;
        parada();
      }
      setPumem(pumem);
  }
}

void descargaE2(void) {
  Serial.write(byte(pumem>>24));
  Serial.write(byte(pumem>>16));
  Serial.write(byte(pumem>>8));
  Serial.write(byte(pumem>>0));

  for (unsigned long i = 0; i < pumem; i++){
    Serial.write(eeRead(i));
  }
}

void borrarE2(void) {
  parada();
  eeErase();
  setPumem(0);
}

bool grabarDatoSD(String archivo, byte data){
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

bool leerArchSD(String archivo){ 
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

bool grabarDatoFlash(char *archivo, byte data){
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

bool leerArchFlash(char *archivo){

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
 
void borrarFlash(void) {

  SerialFlash.eraseAll();
  
  while (SerialFlash.ready() == false) {
     // Loop de espera hasta que se borre. Puede tardar de 30 seg. a 2 min.
  }
}
