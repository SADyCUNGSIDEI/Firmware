//Seteo del reloj
//Si no se puede retorna "false" sino "true"
//Formato de entrada -->  hhmmssDDMMAAAA
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
 
  // Retornar verdadero si se escribio con exito
  return true;
}

//Transmite por RS232 dìa y hora actual
String getFechaHora() {
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

//Transmite por RS232 dìa y hora actual
boolean getFechaHoraReg() {
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
//Escribe un dato en el RTC 
//rtc_addr --> dirección interna de los registros y memoria del RTC
//data --> dato a guardar
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
