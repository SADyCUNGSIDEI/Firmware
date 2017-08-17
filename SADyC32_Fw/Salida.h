#include "Arduino.h"

class Salida {
  private:
    int _pin;
    boolean _dtyCicle;
    boolean _steps;

  public:
    void SetPwm(byte pin, int dtyCicle);
    void SetDac(byte pin, int steps);
    void SetSalDig(byte pin, bool estado);
};
