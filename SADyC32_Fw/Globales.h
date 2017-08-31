SimpleTimer timer;            //Asignación del timer (timer) utilizado para las transmiciones sincrónicas
SimpleTimer timer1;           //Asignación del timer (timer1) utilizado para las transmiciones sincrónicas


#define WP1 30                //Salida que maneja el WRITE PROTECT protect de la memoria EEprom-I2C: 24AA1025
#define MEM_FIN 524200        //Fin de la EEProm (se reservan los 88 bytes del final)

#define WP 32                 //Salida que maneja el WRITE PROTECT de la memoria EEprom-SPI: S25FL127S
#define HOLD 33               //Salida que maneja el HOLD de la memoria EEprom-SPI: S25FL127S 
#define CS1 31                //Salida que maneja el CHIP SELECT de la memoria EEprom-SPI: S25FL127S

#define CS2 47                //Salida que maneja el CHIP SELECT de la memoria SD (placa nueva --> 34 ; Mikroe --> 47)
#define WP_SD 35              //Entrada para detectar el Write Protect de la memoria SD
#define CD 44                 //Entrada para detectar presencia del CHIP SD

#define RS485_DIR 43          //Salida que maneja la direccón de la comunicación RS232
#define RS485_Rx LOW          //Valor para que el RS485-tranceiver este en recepción
#define RS485_Tx HIGH         //Valor para que el RS485-tranceiver este en transmición


#define ESCAPE 0x1b           //Escape --> 0x1B - Definido "99" solo para pruebas en consola que no admiten "0x1B"
#define IN_END 0x0d           //Fin de mensajes
#define TIME_OUT 25000       //Time-out de recepción
#define VECT_END 30           //Tamaño vector para valores de los canales 
#define LARGO_ARCH 15         //Máximo largo de archivo

#define WIFI_AP "NOMBRE_RED"
#define WIFI_CLAVE "CLAVE_RED"

//**********************************************************

#define proyecto "PROYECTO:   SADyC32"
#define equipo   "EQUIPO:     ACQ-II"
#define vers     "VERSION:    1.7.7"
#define fecha    "DE FECHA:   30-08-2017"

#define RTC_ADDR 0x68          //Dirección del RTC-I2C:  DS1307
#define RTC_M_ST 0x08          //Dirección de comienzo ram del RTC

#define TST 13                //TST para indicar encendido de la placa SADyC32 (Pin 45)- En "13" solo para pruebas de placa Arduino sola

#define DIN0 29                //Pin correspondiente a la entrada digital DIN0
#define DIN1 27                //Pin correspondiente a la entrada digital DIN1
#define DIN2 25                //Pin correspondiente a la entrada digital DIN2
#define DIN3 23                //Pin correspondiente a la entrada digital DIN3
#define DIN4 22                //Pin correspondiente a la entrada digital DIN4
#define DIN5 24                //Pin correspondiente a la entrada digital DIN5
#define DIN6 26                //Pin correspondiente a la entrada digital DIN6
#define DIN7 28                //Pin correspondiente a la entrada digital DIN7

#define DIG36 36              //Pin para validar ganancia inAmp (Canal 09)
#define DIG37 37              //Pin para validar ganancia inAmp (Canal 10)
#define DIG38 38              //Pin para validar ganancia inAmp (Canal 11)
#define DIG39 39              //Pin para validar ganancia inAmp (Canal 12)
#define DIG40 40              //Pin para setear ganancia inAmp (Bit bajo)
#define DIG41 41              //Pin para setear ganancia inAmp (Bit alto)

#define MAX_RESOL 12           //Máxima resolución del conversor

#define CANT_ANALOG 8
#define CANT_ANALOG_INAMP 4

//--------------------------------------------------------------------------------------------------
//Seteo del bus SPI para manejo de la memoria SD/MMC
SPISettings Spi_mem_param(50000000, MSBFIRST, SPI_MODE0); 

//--------------------------------------------------------------------------------------------------
const byte mem_ad = 0xa0;       //Dirección de base de la memoria EEProm-I2C: 24AA1025     

//volatile => estan en RAM y no en registros. Muy importante en interrupciones.

boolean estadoEspera = false;

bool mem_full;                  //Indicador de memoria EEProm llena
volatile bool Wifi_ini;         //Indicador que la placa WiFi eta inicializada (True --> inicializada)
//------
volatile byte modo;
volatile byte reg_flag;
byte auxbuffer[VECT_END];
byte k_g = 0;
volatile byte pin1, pin2;
//------
int comando;
int i;
int id_Transm_Temp;
int id_Registro_Temp;
int vectCanales1[8];
int vectCanales2[4];
volatile byte cantCanAnalog1;
volatile byte cantCanAnalog2;
volatile unsigned int tiempo_reg;
//------
unsigned long comm0_out;
volatile unsigned long pumem;
volatile unsigned long Toma1, Toma2;
//------
String inString = "";    
//------
String archivo;
//--------------------------------------------------------------------------------------------------      

