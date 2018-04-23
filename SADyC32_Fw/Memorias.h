/*
	Rutina que escribe un dato en la EEPROM

		unsigned long e2Addr:	Dirección completa de puntero a banco de memoria en 4 bytes
		byte data:						Datos a guardar
*/
void eeWrite(unsigned long e2Addr, byte data);

/*
	Rutina que lee un dato de la EEPROM
		unsigned long e2addr --> dirección completa de puntero a banco de memoria en 4 bytes

		return byte: Devuelve byte leído
*/
byte eeRead(unsigned long eeAddr);

/*
	Borrado rápido de la memoria EEPROM
*/
void eeErase(void);

/*
	Testea cuantos chips de memoria EEPROM hay instalados, escribiendo y lellendo en cada 
	uno el dato ingresado por el usuario

		byte data:	Dato a escribir en la memoria
*/
void eeTest(byte data);

/*
	Rutina que graba un datos en la EEPROM y guarda el nuevo pumem en la ram del reloj

		byte data:	Dato a grabar - Se asume que "pumem" apunta al primer valor libre
*/
void grabaDatoE2(byte data);

/*
	Devuelve el contenido de la memoria EEPROM
*/
void descargaE2(void);

/*
	Rutina que borra la EEPROM e inicializa el puntero de registro "pumem" a cero
*/
void borrarE2(void);

/*
	Rutina que agrega un dato en un archivo en la memoria SD

		String archivo:	Nombre del archivo en donde se desea guardar el dato.
		byte data:		 	Dato a guardar

		return bool: True si se grabó el dato. False si ocurrió algún error
*/
bool grabarDatoSD(String archivo, byte data);

/*
	Rutina que lee un archivo de la memoria SD y lo manda por RS232

		String archivo:	Nombre del archivo en donde se desea guardar el dato.

		return bool: True si se pudo abrir el archivo crrectamente. False si ocurrió algún error
*/
bool leerArchSD(String archivo);

/*
	Rutina que agrega un dato en un archivo en la memoria Flash
		char archivo:	Nombre del archivo en donde se desea guardar el dato. Si no existe lo crea.
		byte data:		Dato a guardar

		return bool: True si grabó el dato correctamente. False si ocurrió algún error
*/
bool grabarDatoFlash(char *archivo, byte data);

/*
	Rutina que lee un archivo de la memoria Flash y lo manda por RS232
		char archivo:		Nombre del archivo en donde se desea guardar el dato.
		
		return bool: True si se pudo abrir el archivo crrectamente. False si ocurrió algún error
*/
bool leerArchFlash(char *archivo);

/*
	Rutina que borra toda la Flash
*/
void borrarFlash(void);
