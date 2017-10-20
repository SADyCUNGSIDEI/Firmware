//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//                    Rutinas para memoria EEPROM (I2C) - 24AA1025
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que escribe un dato en la E2
//e2addr --> dirección completa de puntero a banco de memoria en 4 bytes
//data --> datos a guardar
void eeWrite(unsigned long e2_addr, byte data) {
  byte memaddl, memaddh,i; 
  
  memaddl = (byte) (e2_addr & 0xff);
  memaddh = (byte) ((e2_addr >> 8) & 0xff);
//  i = (byte) ((e2_addr >> 13) & 0x08);
//  i = i | mem_ad;
//  i = i >> 1;
  digitalWrite(WP1, LOW);
  Wire.beginTransmission(EE_ADDR);
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
byte eeRead(unsigned long eeAddr) {
  byte memAddL, memAddH, i, c; 
  
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
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Borrado rápido de la memoria E2
void eeErase(void) {
  byte memAddL, memAddH, i, data, k; 
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
    delay(10);
  } while(eeAddr < MEM_FIN);
  digitalWrite(WP, HIGH);
}

//--------------------------------------------------------------------------------------------------
//Testea cuantos chips de memoria EEProm hay instalados, escribiendo y lledendo en cada uno el dato ingresado por el usuario
void eeTest(byte data) {
  unsigned long aux;
  byte i;

    aux = 32000;          
    for (i=1;i<=2;i++) {
      eeWrite(aux, data);
      aux = aux + 65536;
    }
    delay(10);
    aux = 32000;          
    for(i=1;i<=2;i++) {
      data = eeRead(aux);
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
//Rutina que agrega un dato en un archivo en la memoria SD
//archivo --> nombre del archivo en donde se desea guardar el dato.
//data --> dato a guardar
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
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que lee un archivo de la memoria SD y lo manda por RS232
//archivo --> nombre del archivo en donde se desea guardar el dato.
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

//Rutina que agrega un dato en un archivo en la memoria Flash
//archivo --> nombre del archivo en donde se desea guardar el dato. Si no existe lo crea.
//data --> dato a guardar
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
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que lee un archivo de la memoria Flash y lo manda por RS232
//archivo --> nombre del archivo en donde se desea guardar el dato.
//cant_datos --> cantidad de datos a leer (0 --> todos)
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
 
//--------------------------------------------------------------------------------------------------
//Rutina que borra toda la Flash

void borrarFlash(void) {

  SerialFlash.eraseAll();
  
  while (SerialFlash.ready() == false) {
     // Loop de espera hasta que se borre. Puede tardar de 30 seg. a 2 min.
  }
}
