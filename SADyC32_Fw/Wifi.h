/*
	Rutina que envía el comando AT especificado para el control del ESP8266

		String comando: 	Comando a enviar
		int tiempo:				Tiempo de espera para la respuesta del ESP8266

		return bool: True si la respuesta fue satisfactoria. False caso contrario
*/
bool espSend(String comando, int tiempo);

/*
	Chequea que el ESP8266 se encuentre disponible

		return bool: True si la respuesta fue satisfactoria. False caso contrario
*/
bool espCheck(void);

/*
	Deshabilita el eco en el ESP8266

		return bool: True si la respuesta fue satisfactoria. False caso contrario
*/
bool disableEcho(void);

/*
	Se desconecta del WiFi

		return bool: True si la respuesta fue satisfactoria. False caso contrario
*/
bool disconnectWifi(void);

/*
	Conecta el ESP8266 a la red WiFi indicada

		String red:		Nombre del access point
		String clave:	Clave del access point
	
		return bool: True si la respuesta fue satisfactoria. False caso contrario
*/
bool espConnect(String red, String clave);

/*
	Configura el ESP8266 en modo server

		return bool: True si la respuesta fue satisfactoria. False caso contrario
*/
bool espServer(void);

/*
	Inicializa el ESP8266 conectandose a la red y poniendose en modo servidor
	quedando a la escucha en el puerto 80

		return bool: True si la respuesta fue satisfactoria. False caso contrario
*/
bool iniWifi(void);

/*
	Chequeo para la recepción de comandos por WiFi ***** No está funcionando REVISAR
*/
void checkEnvioWiFi(void);