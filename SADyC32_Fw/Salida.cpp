#include "Salida.h"

Salida::Salida() {

}

//Permite setear un PWM en el pin seleccionado ((pinPwm) de 10 a 13) con el Duty-Cicle (dtyCicle)
void Salida::setPwm(byte pin, int dtyCicle) {
  if(pin > 9 && pin < 14) {               //No se usa el pin13 (led) ni los pines 0 y 1 (Rx0 y Tx0).
    analogWrite(pin, dtyCicle);
  }
}

//Setea el DAC seleccionado (pinDac) con el un valor específico (Valor)
void Salida::setDac(byte pin, int steps) {
  if(pin > 65 && pin < 68){
    analogWrite(pin, steps);
  }
}

//Permite setear un pin de salida digital ((pinDig) de 2 a 9) con el ((estado) 0 o 1)
void Salida::setSalDig(byte pinDig, bool estado){
  if (pinDig > 1 && pinDig < 10) {               //No se usa el pin13 (led) ni los pines 0 y 1 (Rx0 y Tx0).
    digitalWrite(pinDig, estado);
  }
}

//Transmite por RS232 los valores de todos los canales digitales y los canales
//analógicos leídos. Primero copia el vector de canales analógicos
//a modo de congelamiento temporal de datos
void Salida::transTemp(SADyC32 &s) {
  byte i;
  byte modo;
  int _vect_aux1[8];
  int _vect_aux2[4];

  modo = s.getModo();

  //Copia el vector de datos de los canales analógicos en un vector auxiliar para su manipulación
  s.getVectCanales1(_vect_aux1);
  s.getVectCanales2(_vect_aux2);

  Serial.write(this->_leerCanalesDigitales());

  if ((modo == 1) || (modo == 2)) {
    for(i = 0; i< s.getCanAnalog1(); i++){
      Serial.write(byte(_vect_aux1[i] >> 8));
      Serial.write(_vect_aux1[i]);
    }
  }
  if ((modo == 3) || (modo == 4)) {
    for(i = 0;i < s.getCanAnalog2();i++){
      Serial.write(byte(_vect_aux1[i] >> 8));
      Serial.write(_vect_aux1[i]);
    }
  }
}

//Rutina que lee los canales digitales de entrada y los unifica en un solo byte "digi_in"
byte Salida::_leerCanalesDigitales(void){
  byte digi_in;
  
  digi_in = 0x00 | byte(digitalRead(DIN0))
                 | (byte(digitalRead(DIN1)) << 1)
                 | (byte(digitalRead(DIN2)) << 2)
                 | (byte(digitalRead(DIN3)) << 3)
                 | (byte(digitalRead(DIN4)) << 4)
                 | (byte(digitalRead(DIN5)) << 5)
                 | (byte(digitalRead(DIN6)) << 6)
                 | (byte(digitalRead(DIN7)) << 7);
  return digi_in;
}

void Salida::adcSrv(SADyC32 &s) {
  byte i, modo;
  modo = s.getModo();
  
  if ((modo == 1) || (modo == 2)) {
    for(i = 0; i < s.getCanAnalog1(); i++) {
      s.setVectCanales1(analogRead(54 + i), i);
    }
  }
  if ((modo == 3) || (modo == 4)) {
    for(i = 0; i < s.getCanAnalog2(); i++) {
      s.setVectCanales2(analogRead(62 + i), i);
    }
  }
}
