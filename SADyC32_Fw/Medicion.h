#ifndef MEDICION_h
#define MEDICION_h

#include "Arduino.h"

class Medicion {
  private:
    static Medicion * instances[3];
    volatile bool _startFlag;
    volatile bool _stopFlag;
    unsigned long _toma1;
    unsigned long _toma2;

    static void _trigSamePin(void);
    static void _trigPin1(void);
    static void _trigPin2(void);
    void _intSamePin(void);
    void _intPin1(void);
    void _intPin2(void);

  public:
    Medicion();
    void setMedicion(byte pin1, byte pin2);
    void checkMedicion(void);
};

#endif
