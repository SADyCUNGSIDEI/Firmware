#ifndef SADYC32_h
#define SADYC32_h

#include "Arduino.h"

#define proyecto "PROYECTO:   SADyC32"
#define equipo   "EQUIPO:     ACQ-II"
#define vers     "VERSION:    1.7.7"
#define fecha    "DE FECHA:   04-07-2017"

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

class SADyC32 {
  private:
    byte _modo;
    byte _cantCan_analog1;
    byte _cantCan_analog2;
    int _vect_canales1[CANT_ANALOG];
    int _vect_canales2[CANT_ANALOG_INAMP];

  public:
    SADyC32();
    void saludo(void);
    void datosEquipo(void);
    void setModo(byte modo);
    byte getModo(void);
    void setCanAnalog1(byte cant);
    byte getCanAnalog1();
    void setCanAnalog2(byte cant);
    byte getCanAnalog2();
    void setInAmp(byte in_inAmp, byte amplif);
    void setVectCanales1(int val, byte pos);
    void *getVectCanales1(int *vect_canales);
    void setVectCanales2(int val, byte pos);
    void *getVectCanales2(int *vect_canales);
};
#endif
