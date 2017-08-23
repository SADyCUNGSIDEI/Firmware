#include "Reloj.h"

Reloj::Reloj() {
}

//Seteo del reloj
//Si no se puede retorna "false" sino "true"
//Formato de entrada -->  hhmmssDDMMAAAA
boolean Reloj::setFechaHora(byte horas, byte minutos, byte segundos, byte dia, byte mes, byte anio) {
  Wire.beginTransmission(RTC_ADDR);

  Wire.write(0x00);

  Wire.write(this->_bin2bcd(segundos & 0x7F)); // <--- Esto hace que el reloj comience a trabajar
  Wire.write(this->_bin2bcd(minutos));
  Wire.write(this->_bin2bcd(horas));
  Wire.write(this->_bin2bcd(0));
  Wire.write(this->_bin2bcd(dia));
  Wire.write(this->_bin2bcd(mes));
  Wire.write(this->_bin2bcd(anio));
 
  if (Wire.endTransmission() != 0)
    return false;
 
  // Retornar verdadero si se escribio con exito
  return true;
}

//Transmite por RS232 dìa y hora actual
String Reloj::getFechaHora() {
  byte segundo, minuto, hora, wday, dia, mes, anio;
  String fechaHora;
  // Iniciar el intercambio de información con el DS1307 (0xD0)
  Wire.beginTransmission(RTC_ADDR);

  Wire.write(0x00);

  if (Wire.endTransmission() != 0)
    return "";

  // Si el DS1307 esta presente, comenzar la lectura de 8 bytes
  Wire.requestFrom(RTC_ADDR, 8);

  segundo = this->_bcd2bin(Wire.read());
  minuto = this->_bcd2bin(Wire.read());
  hora = this->_bcd2bin(Wire.read());
  wday = this->_bcd2bin(Wire.read());
  dia = this->_bcd2bin(Wire.read());
  mes = this->_bcd2bin(Wire.read());
  anio = this->_bcd2bin(Wire.read());

  return String(hora) + ":" + String(minuto) + ":" +  String(segundo) + " " +
          String(dia) + "/" + String(mes) + "/20" + String(anio);
}

byte Reloj::getMemModo() {
  byte modo = this->_memRead(RTC_M_ST + 11);
  if (modo < 1 || modo > 5) { modo = 1; }
  this->setMemModo(modo);
  return modo;
}

void Reloj::setMemModo(byte modo) {
  this->_memWrite((RTC_M_ST + 11), modo);
}

byte Reloj::getMemCanAnalog1() {
  byte can_analog = this->_memRead(RTC_M_ST + 5);
  if (can_analog < 1 || can_analog > 8) { can_analog = 8; }
  this->setMemCanAnalog1(can_analog);
  return can_analog;
}

void Reloj::setMemCanAnalog1(byte cant) {
  this->_memWrite((RTC_M_ST + 5), cant);
}

byte Reloj::getMemCanAnalog2() {
  byte can_analog = this->_memRead(RTC_M_ST + 6);
  if (can_analog < 1 || can_analog > 4) { can_analog = 4; }
  this->setMemCanAnalog2(can_analog);
  return can_analog;
}

void Reloj::setMemCanAnalog2(byte cant) {
  this->_memWrite((RTC_M_ST + 6), cant);
}

//Escribe un dato en el RTC 
//rtc_addr --> dirección interna de los registros y memoria del RTC
//data --> dato a guardar
void Reloj::_memWrite(byte addr, byte data) {
  Wire.beginTransmission(RTC_ADDR);
  Wire.write(addr);                
  Wire.write(data);                
  Wire.endTransmission(true);    
}  

byte Reloj::_memRead(byte addr) {
  byte c; 

  Wire.beginTransmission(RTC_ADDR);
  Wire.write(addr);
  Wire.endTransmission(false);
  Wire.requestFrom((int)RTC_ADDR, 1, true);
  c = Wire.read();
  return c;
}  

byte Reloj::_bin2bcd(byte bin) {
  return (bin / 10 * 16) + (bin % 10);
}

byte Reloj::_bcd2bin(byte bcd) {
  return (bcd / 16 * 10) + (bcd % 16);
}

