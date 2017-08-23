#ifndef RELOJ_h
#define RELOJ_h

#include "Arduino.h"
#include "SADyC32.h"

#include <Wire.h>             //Biblioteca para manejo del bus I2C

class Reloj {
  private:
    byte _bin2bcd(byte bin);
    byte _bcd2bin(byte bin);
    void _memWrite(byte addr, byte data);
    byte _memRead(byte addr);

  public:
    Reloj();
    boolean setFechaHora(byte horas, byte minutos, byte segundos, byte dia, byte mes, byte anio);
    String getFechaHora();
    byte getMemModo();
    void setMemModo(byte modo);
    byte getMemCanAnalog1();
    void setMemCanAnalog1(byte cant);
    byte getMemCanAnalog2();
    void setMemCanAnalog2(byte cant);
};

#endif
