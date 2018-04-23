/*
	Rutina que setea los canales elegidos en el ADC para que solo barra esos.
*/
void setADCChan(void);

/*
	Rutina que lee el ADC en secuencia
	Cada vez que ejecuta lee todos los canales seteados
*/
void adcSrv(void);

/*
	Setea el tiempo minimo de muestreo aceptado de acuerdo a la comunicación utilizada
*/
void setTiempoMuestreoMin(void);

/*
	Activa los timers necesarios para la transmisión de acuerdo a la variable "salida"

		unsigned long tiempo:     Tiempo de transmisión
		SalidaAdquisicion salida: Salida de los datos leidos por el ADC [RS232, EEPROM, WIFI]
*/
void transmisionTemporizada(unsigned long tiempo, SalidaAdquisicion salida);

/*
	Transmite por RS232 los valores de todos canales analógicos leídos. 
*/
void transmTemp(void);

/*
	Registra en la memoria EEPROM-I2C (24AA1025) los canales analógicos leídos. 
*/
void regTemp(void);

/*
	Transmite por WIFI los valores de todos canales analógicos leídos. 
*/
void transmTempWifi(void);

/*
	Rutina que graba los datos de inicialización de registro en la EEPROM
*/
void grabaIniE2(void);