#include "Arduino.h"
#include "SADyC32.h"

class Salida {
  private:
    static int vect_canales1[8];
    byte _leerCanalesDigitales(void);

  public:
    Salida();
    void setPwm(byte pin, int dtyCicle);
    void setDac(byte pin, int steps);
    void setSalDig(byte pin, bool estado);
    void transTemp(SADyC32 &s);
    void adcSrv(SADyC32 &s);
};
