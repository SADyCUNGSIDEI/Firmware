/*
	Función que sirve para parsear los valores numéricos que entran por el puerto serial

		int inicio: 	Posición donde se comienza a leer el auxbuffer
		int cantidad: Cantidad de carácteres a leer en auxbuffer

		return int: devuelve el número
*/
int parseValor(int inicio, int cantidad);

/*
	Envía la respuesta a serialActivo, variable en dónde es seteada la salida de acuerdo 
	a donde fué enviada la petición
		Serial  --> RS232 (Terminal Arduino)
		Serial1 --> RS232 (Propio de la placa)
		Serial2 --> RS485
		Serial3 --> WiFi (ESP8266)

		String data		String de texto a enviar
		bool newLine	Booleano que define si envía una nueva línea (TRUE: Si, FALSE: No)
*/
void serialOutput(String data, bool newLine);

/*
	Función que va definiendo los estados a medida que recibe los comandos envíados.
	Recibe cada dato en secuencia hsta que encuentra el fin de comando

									Formato: ESC Comando CR
	
	Si el comando no se completa o no existe se va por tiempo
	Si el comando existe ejecuta en consecuencia llamando a la función "ejecucion"

		char inChar:	Char recibido
*/
void InComando(char inChar);

/*
	Función que determina de dónde proviene el comando leído y setea el serialActivo,
	para luego ser usado por el serialOutput

		HardwareSerial &port: Puerto de dónde se recibe el comando
*/
void serialInput(HardwareSerial &port);
