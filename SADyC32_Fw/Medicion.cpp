#include "Medicion.h"

Medicion::Medicion() {
  _startFlag = false;
  _stopFlag = false;
}

void Medicion::_trigSamePin() {
  if (Medicion::instances[0] != NULL)
    Medicion::instances[0]->_intSamePin();
}

void Medicion::_trigPin1() {
  if (Medicion::instances[1] != NULL)
    Medicion::instances[1]->_intPin1();
}

void Medicion::_trigPin2() {
  if (Medicion::instances[2] != NULL)
    Medicion::instances[2]->_intPin2();
}

void Medicion::setMedicion(byte pin1, byte pin2) {
  if((pin1 > 48) && (pin1 < 54) && (pin2 > 48) && (pin2 < 54)) {
    if (pin1 == pin2) {
      attachInterrupt (pin1, this->_trigSamePin, RISING);
      instances[0] = this;
    }
    else {
      attachInterrupt (pin1, this->_trigPin1, CHANGE);
      attachInterrupt (pin2, this->_trigPin2, CHANGE);
      instances [1] = this;
      instances [2] = this;
    }
  }
}

void Medicion::checkMedicion() {
  if((this->_startFlag == true) && (this->_stopFlag == true)) {
    this->_toma2 = this->_toma2 - this->_toma1;
    Serial.print(F("Tiempo entre eventos: "));
    Serial.print(_toma2);
    Serial.println(F(" microsegundos"));
    this->_startFlag = false;
    this->_stopFlag = false;
  }
}
  
void Medicion::_intSamePin() {
  if ((this->_startFlag == true) && (this->_stopFlag == false)) {
    this->_toma2 = micros();
    this->_stopFlag = true;
  }
  if ((this->_startFlag == false) && (this->_stopFlag == false)) {
    Serial.println("Inicio medicion");
    this->_toma1 = micros();
    this->_startFlag = true;
  }
}

void Medicion::_intPin1() {
  if((this->_startFlag == false) && (this->_stopFlag == false)) {
    this->_toma1 = micros();
    this->_startFlag = true;
  }
}

void Medicion::_intPin2() {
  if((this->_startFlag == true) && (this->_stopFlag == false)) {
    this->_toma2 = micros();
    this->_stopFlag = true;
  }
}

