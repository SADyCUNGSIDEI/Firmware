#include "Salida.h"

//Salida::Salida(int pin) {
//  _pin = pin ;
//}

//Permite setear un PWM en el pin seleccionado ((pinPwm) de 10 a 13) con el Duty-Cicle (dtyCicle)
void Salida::SetPwm(byte pin, int dtyCicle) {  //solo admitre los parámetros INPUT e INPUT_PULLUP
  if(pin > 9 && pin < 14){               //No se usa el pin13 (led) ni los pines 0 y 1 (Rx0 y Tx0).
    analogWrite(pin, dtyCicle);
  }
}

//Setea el DAC seleccionado (pinDac) con el un valor específico (Valor)
void Salida::SetDac(byte pin, int steps) {
  if(pin > 65 && pin < 68){
    analogWrite(pin, steps);
  }
}

//Permite setear un pin de salida digital ((pinDig) de 2 a 9) con el ((estado) 0 o 1)
void Salida::SetSalDig(byte pinDig, bool estado){
  if (pinDig > 1 && pinDig < 10) {               //No se usa el pin13 (led) ni los pines 0 y 1 (Rx0 y Tx0).
    digitalWrite(pinDig, estado);
  }
}
