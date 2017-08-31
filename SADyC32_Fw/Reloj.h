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
  // Iniciar el intercambio de información con el DS1307 (0xD0)
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

//Escribe un dato en el RTC 
//rtc_addr --> dirección interna de los registros y memoria del RTC
//data --> dato a guardar
void memWrite(byte addr, byte data) {
  Wire.beginTransmission(RTC_ADDR);
  Wire.write(addr);                
  Wire.write(data);                
  Wire.endTransmission(true);    
}  

byte memRead(byte addr) {
  byte c; 

  Wire.beginTransmission(RTC_ADDR);
  Wire.write(addr);
  Wire.endTransmission(false);
  Wire.requestFrom((int)RTC_ADDR, 1, true);
  c = Wire.read();
  return c;
}  

byte getMemModo() {
  byte modo = memRead(RTC_M_ST + 11);
  if (modo < 1 || modo > 5) { modo = 1; }
  return modo;
}

void setMemModo(byte modo) {
  memWrite((RTC_M_ST + 11), modo);
}

byte getMemCanAnalog1() {
  byte can_analog = memRead(RTC_M_ST + 5);
  if (can_analog < 1 || can_analog > 8) { can_analog = 8; }
  return can_analog;
}

void setMemCanAnalog1(byte cant) {
  memWrite((RTC_M_ST + 5), cant);
}

byte getMemCanAnalog2() {
  byte can_analog = memRead(RTC_M_ST + 6);
  if (can_analog < 1 || can_analog > 4) { can_analog = 4; }
  return can_analog;
}

void setMemCanAnalog2(byte cant) {
  memWrite((RTC_M_ST + 6), cant);
}
