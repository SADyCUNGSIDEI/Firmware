/*
	Impresión de los datos básicos del equipo
*/
void datosEquipo(void);

/*
	Setea el modo de trabajo del equipo (1 a 4) y lo graba en la RAM del RTC

		byte mode: Modo de trabajo
*/
void setModo(byte mode);

/*
	Lee el modo de trabajo del equipo seteado guardados en la RAM del RTC

		return byte: Modo de trabajo del equipo
*/
byte getModo(void);

/*
	Setea la cantidad de canales analógicos y los guarda en la RAM del RTC

		byte cant: Cantidad de canales analógicos
*/
void setCanAnalog1(byte cant);

/* 
	Lee la cantidad de canales analógicos de la RAM del RTC

		return byte: Cantidad de canales analógicos
*/
byte getCanAnalog1(void);

/*
	Setea la cantidad de canales analógicos de tipo inAmp y los guarda en la RAM del RTC

		byte cant: Cantidad de canales analógicos inAmp
*/
void setCanAnalog2(byte cant);


/* 
	Lee la cantidad de canales analógicos de tipo inAmp de la RAM del RTC

		return byte: Cantidad de canales analógicos inAmp
*/
byte getCanAnalog2(void);

/*
	Setea el código de inicio y lo guarda en la RAM del RTC
*/
void setCodigoInicio(byte codigo);

/*
	Lee el código de inicio de la RAM del RTC

		return byte: Código de inicio
*/
byte getCodigoInicio(void);

/*
	Setea el flag de registro en la RAM del RTC
*/
void setRegFlag(byte regFlag);

/*
	Lee el flag de registro de la RAM del RTC

		return byte: Flag de registro
*/
byte getRegFlag(void);

/*
	Setea el nombre de red y su password en la RAM del RTC
*/
void setWifiParams(void);

/*
	Lee los parametros de conexión del WiFi (usuario y clave) de la RAM del RTC
*/
void getWifiParams(void);

/*
	Setea el tiempo de registro en la RAM del RTC

		unsigned int tiempoReg: Tiempo de registro
*/
void setTiempoReg(unsigned int tiempoReg);

/*
	Lee el tiempo de registro de la RAM del RTC

		return unsigned int: Tiempo de registro
*/
unsigned int getTiempoReg(void);

/*
	Muestra datos de configuración del equipo y emite un parpadeo en el TST
*/
void saludo(void);

/*
	Setea el puntero de memoria de la EEPROM (I2C)

		unsigned long pmem: Dirección del puntero de memoria
*/
void setPumem(unsigned long pmem);

/*
	Lee el puntero de memoria de la EEPROM (I2C) que se encuentra grabado en la RAM del RTC

		return unsigned long: Puntero de memoria
*/
unsigned long getPumem(void);

/*
	Setea una salida PWM en el pin indicado, con el duty cicle indicado

		byte pin: 		Pin de salida de PWM (del 10 al 13)
		int dtyCicle: Duty cicle
*/
void setPwm(byte pin, int dtyCicle);

/*
	Setea el DAC seleccionado con un valor específico

		byte pin: 	Pin seleccionado
		int valor: 	Valor
*/
void setDac(byte pin, int valor);

/*
	Permite setear un pin de salida digital con el estado indicado

		byte pinDig: 	Pin del 2 a 9
		bool estado:	Estado (0 o 1)
*/
void setSalDig(byte pinDig, bool estado);

/*
	Permite setear la amplificación de los inAmps. Entrada analógica con la 
	amplificación seleccionada

		byte inInAmp: Pin de 9 a 12
		byte amplif:	Amplificación (0 a 3)
*/
void setInAmp(byte inInAmp, byte amplif);

/*
	Rutina que lee los canales digitales de entrada y los unifica en un solo byte

		return byte: Lectura de los canales digitales
*/
byte leerCanalesDigitales(void);

/*
	Para todos los modos de trabajo, timers y desacopla interrupciones
*/
void parada(void);

/*
	Inicialización de todas las variables para una correcta ejecución del programa
*/
void iniVar(void);

/*
	Rutina para vaciar el auxbuffer
*/
void vaciarAuxBuffer(void);

/*
	Recibe el código del comando a ejecutar y llama a la función especifica para
	ejecutar dicho comando

		int comando: Comando a ejecutar
*/
void ejecucion(int comando);