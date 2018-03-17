#include <SimpleTimer.h>      //Biblioteca para setear tareas controladas por timer

//globals.h
#ifndef _GLOBALS_H
#define _GLOBALS_H

#if defined MAIN
#define EXTERN
#else
#define EXTERN extern
#endif

#define proyecto "PROYECTO:   SADyC32"
#define equipo   "EQUIPO:     ACQ-II"
#define vers     "VERSION:    1.9.0"
#define fecha    "DE FECHA:   07-10-2017"

#define RTC_ADDR      0x68          //Dirección del RTC-I2C:  DS1307
#define RTC_M_ST      0x08          //Dirección de comienzo ram del RTC
#define RTC_M_CCA1    0x0D          //Dirección: Cantidad de canales analógicos
#define RTC_M_CCA2    0x0E          //Dirección: Cantidad de canales analógicos de tipo inAmp
#define RTC_M_C_INI   0x0F          //Dirección: Código de inicio
#define RTC_M_MODO    0x13          //Dirección: Modo de trabajo del equipo
#define RTC_M_RF      0x10          //Dirección: Flag de registro
#define RTC_M_AP      0x18          //Dirección: Inicio del nombre del access point (WiFi) - 16 bytes
#define RTC_M_PW      0x28          //Dirección: Inicio del password del acces point (WiFi) - 16 bytes

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

#define TIME_OUT 250000       //Time-out de recepción
#define VECT_END 20           //Tamaño vector de buffer de recepcion 
#define IN_END 0x0d           //Fin de mensajes
#define ESCAPE 0x1b           //Escape --> 0x1B - Definido "99" solo para pruebas en consola que no admiten "0x1B"

#define EE_ADDR 0x51          //Dirección de base de la memoria EEProm-I2C: 24AA1025     
#define WP1 30                //Salida que maneja el WRITE PROTECT protect de la memoria EEprom-I2C: 24AA1025
#define MEM_FIN 30000         //Fin de la EEProm (se reservan los 88 bytes del final)

#define WP 32                 //Salida que maneja el WRITE PROTECT de la memoria EEprom-SPI: S25FL127S
#define HOLD 33               //Salida que maneja el HOLD de la memoria EEprom-SPI: S25FL127S 
#define CS1 31                //Salida que maneja el CHIP SELECT de la memoria EEprom-SPI: S25FL127S

enum SerialActivo {
    NO_SERIAL,
    SERIAL_0,
    SERIAL_1,  
    SERIAL_2,
    SERIAL_3
};

EXTERN SimpleTimer timer;            //Asignación del timer (timer) utilizado para las transmiciones sincrónicas

EXTERN SerialActivo serialActivo;
EXTERN unsigned long comm0_out;           // Contador descendente para controlar el fin de recepción de comandos.
EXTERN boolean estadoEspera;              // Determina si la máquina de estados está en modo espera o en modo de recepción de comandos
EXTERN byte k_g;
EXTERN int comando;
EXTERN byte auxbuffer[VECT_END];
EXTERN String inString;

EXTERN volatile byte modo;
EXTERN volatile byte cantCanAnalog1;
EXTERN volatile byte cantCanAnalog2;

EXTERN char nombreAP[16];
EXTERN char claveAP[16];

EXTERN volatile byte pin1, pin2;

EXTERN int vectCanales1[8];
EXTERN int vectCanales2[4];

EXTERN int idTransmTemp;
EXTERN int idRegistroTemp;
EXTERN int idTransmTempWifi;

EXTERN volatile unsigned long pumem;
EXTERN bool mem_full;                  //Indicador de memoria EEProm llena

EXTERN String wifiBuffer;
EXTERN int wifiSendTimeout;

#endif // _GLOBALS_H

