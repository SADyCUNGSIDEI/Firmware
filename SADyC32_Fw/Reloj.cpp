#include <Wire.h>             //Biblioteca para manejo del bus I2C

#include "Arduino.h"
#include "Globales.h"
#include "Reloj.h"

byte bin2bcd(byte bin) {
  return (bin / 10 * 16) + (bin % 10);
}

byte bcd2bin(byte bcd) {
  return (bcd / 16 * 10) + (bcd % 16);
}

boolean setFechaHora(byte horas, byte minutos, byte segundos, byte dia, byte mes, byte anio) {
  Wire.beginTransmission(RTC_ADDR);

  Wire.write(0x00);

  Wire.write(bin2bcd(segundos & 0x7F)); // <--- Esto hace que el reloj comience a trabajar
  Wire.write(bin2bcd(minutos));
  Wire.write(bin2bcd(horas));
  Wire.write(bin2bcd(0));
  Wire.write(bin2bcd(dia));
  Wire.write(bin2bcd(mes));
  Wire.write(bin2bcd(anio));
 
  if (Wire.endTransmission() != 0)
    return false;
 
  // Retornar verdadero si se escribio con éxito
  return true;
}

String getFechaHora(void) {
  byte segundo, minuto, hora, wday, dia, mes, anio;

  // Iniciar el intercambio de información con el DS1307
  Wire.beginTransmission(RTC_ADDR);

  Wire.write(0x00);

  if (Wire.endTransmission() != 0)
    return "";

  // Si el DS1307 esta presente, comenzar la lectura de 8 bytes
  Wire.requestFrom(RTC_ADDR, 8);

  segundo = bcd2bin(Wire.read());
  minuto = bcd2bin(Wire.read());
  hora = bcd2bin(Wire.read());
  wday = bcd2bin(Wire.read());
  dia = bcd2bin(Wire.read());
  mes = bcd2bin(Wire.read());
  anio = bcd2bin(Wire.read());

  return String(hora) + ":" + String(minuto) + ":" +  String(segundo) + " " +
          String(dia) + "/" + String(mes) + "/20" + String(anio);
}

boolean getFechaHoraReg(void) {
  byte segundo, minuto, hora, wday, dia, mes, anio;

  // Iniciar el intercambio de información con el DS1307
  Wire.beginTransmission(RTC_ADDR);

  Wire.write(0x00);

  if (Wire.endTransmission() != 0)
    return true;


  // Si el DS1307 esta presente, comenzar la lectura de 8 bytes
  Wire.requestFrom(RTC_ADDR, 8);

  segundo = bcd2bin(Wire.read());
  minuto = bcd2bin(Wire.read());
  hora = bcd2bin(Wire.read());
  wday = bcd2bin(Wire.read());
  dia = bcd2bin(Wire.read());
  mes = bcd2bin(Wire.read());
  anio = bcd2bin(Wire.read());

  auxbuffer[1] = hora / 10;
  auxbuffer[2] = hora - int(hora/10)*10;
  auxbuffer[3] = minuto / 10;
  auxbuffer[4] = minuto - int(minuto/10)*10;
  auxbuffer[5] = segundo / 10;
  auxbuffer[6] = segundo - int(segundo/10)*10;
  auxbuffer[7] = dia / 10;
  auxbuffer[8] = dia - int(dia/10)*10;
  auxbuffer[9] = mes / 10;
  auxbuffer[10] = mes - int(mes/10)*10;
  auxbuffer[11] = 2;
  auxbuffer[12] = 0;
  auxbuffer[13] = anio / 10;
  auxbuffer[14] = anio - int(anio/10)*10;
}

void rtcWrite(byte addr, byte data) {
  Wire.beginTransmission(RTC_ADDR);
  Wire.write(addr);                
  Wire.write(data);                
  Wire.endTransmission(true);    
}  

byte rtcRead(byte addr) {
  byte c; 

  Wire.beginTransmission(RTC_ADDR);
  Wire.write(addr);
  Wire.endTransmission(false);
  Wire.requestFrom((int)RTC_ADDR, 1, true);
  c = Wire.read();
  return c;
}
