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
  String fechaHora;
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

void getFechaHoraReg() {
  byte segundo, minuto, hora, wday, dia, mes, anio;
  String fechaHora;
  // Iniciar el intercambio de información con el DS1307
  Wire.beginTransmission(RTC_ADDR);

  Wire.write(0x00);

  // Si el DS1307 esta presente, comenzar la lectura de 8 bytes
  Wire.requestFrom(RTC_ADDR, 8);

  auxbuffer[1] = bcd2bin(Wire.read()); // Segundos
  auxbuffer[2] = bcd2bin(Wire.read()); // Minutos
  auxbuffer[3] = bcd2bin(Wire.read()); // Horas
  bcd2bin(Wire.read()); // Día de la semana. Se descarta
  auxbuffer[4] = bcd2bin(Wire.read()); // Día
  auxbuffer[5] = bcd2bin(Wire.read()); // Mes
  auxbuffer[6] = bcd2bin(Wire.read()); // Año
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
