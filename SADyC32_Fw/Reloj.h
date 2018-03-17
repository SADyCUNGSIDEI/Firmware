byte bin2bcd(byte bin);
byte bcd2bin(byte bcd);
boolean setFechaHora(byte horas, byte minutos, byte segundos, byte dia, byte mes, byte anio);
String getFechaHora(void);
boolean getFechaHoraReg(void);
void rtcWrite(byte addr, byte data);
byte rtcRead(byte addr);
