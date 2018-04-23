/*
	Rutina para convertir el valor en el seteo de la fecha y hora del equipo

		return byte: Valor
*/
byte bin2bcd(byte bin);


/*
	Rutina para convertir el valor de lectura de la fecha y hora del equipo

		return byte: Valor
*/
byte bcd2bin(byte bcd);

/*
	Setea la fecha y la hora del equipo. Formato de entrada: hhmmssDDMMAAAA

		byte horas:			Hora actual
		byte minutos:		Minuto actual
		byte segundos: 	Segundo actual
		byte dia:				Dia actual
		byte mes:				Mes actual
		byte anio: 			Año actual

		return boolean: True si la fecha fué seteada correctamente. False en caso contrario
*/
boolean setFechaHora(byte horas, byte minutos, byte segundos, byte dia, byte mes, byte anio);

/*
	Devuelve la fecha y la hora del RTC en formato HH:MM:SS DD/MM/YYYY

		return String: Fecha y hora
*/
String getFechaHora(void);

/*

*/
boolean getFechaHoraReg(void);

/*
	Escribe un dato en el RTC 

		byte addr:	Dirección interna de los registros y memoria del RTC
		byte data:	Dato a guardar
*/
void rtcWrite(byte addr, byte data);

/*
	Lee un dato en la memoria RAM del RTC

		byte addr: Dirección a leer

		return byte: Byte leído
*/
byte rtcRead(byte addr);
