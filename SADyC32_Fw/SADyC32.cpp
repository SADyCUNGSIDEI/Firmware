#include "SADyC32.h"

SADyC32::SADyC32() {

}

//Impresión de los datos básicos del equipo
void SADyC32::datosEquipo(void){
  Serial.println(F(proyecto));
  Serial.println(F(equipo));
  Serial.println(F(vers));
  Serial.println(F(fecha));
}

void SADyC32::saludo() {
  for(int i=1;i<=4;i++){
    digitalWrite(TST,HIGH);
    delay(250);
    digitalWrite(TST,LOW);
    delay(250);
  }
  this->datosEquipo();
  Serial.print("Modo: ");
  Serial.println(this->getModo());
  Serial.print("Cantidad canales analogicos: ");
  Serial.println(this->getCanAnalog1());
  Serial.print("Cantidad canales analogicos inAmp: ");
  Serial.println(this->getCanAnalog2());
}

void SADyC32::setModo(byte modo) {
  _modo = modo;
}

byte SADyC32::getModo() {
  return _modo;
}

void SADyC32::setCanAnalog1(byte cant) {
  if (cant < 1 || cant > 8) { cant = 1; }
  _cantCan_analog1 = cant;
}

byte SADyC32::getCanAnalog1() {
  return _cantCan_analog1;
}

void SADyC32::setCanAnalog2(byte cant) {
  if (cant < 1 || cant > 4) { cant = 1; }
  _cantCan_analog2 = cant;
}

byte SADyC32::getCanAnalog2() {
  return _cantCan_analog2;
}

//Permite setear la amplificación de los inAmps. Entrada analógica ((in_inAmp) de 09 a 12)
//con la amplificación ((amplif) 0 a 3)

void SADyC32::setInAmp(byte in_inAmp, byte amplif){
  if((in_inAmp > 8 && in_inAmp < 13) && amplif < 4){
    in_inAmp = in_inAmp + 27;               
    switch(amplif){
      case 0:
        digitalWrite(40,LOW);
        digitalWrite(41,LOW);
      break;
      case 1:
        digitalWrite(40,HIGH);
        digitalWrite(41,LOW);
      break;
      case 2:
        digitalWrite(40,LOW);
        digitalWrite(41,HIGH);
      break;
      case 3:
        digitalWrite(40,HIGH);
        digitalWrite(41,HIGH);
      break;
    }
    digitalWrite(in_inAmp,LOW);
    delayMicroseconds(1);    
    digitalWrite(in_inAmp,HIGH);
  }
}

void SADyC32::setVectCanales1(int val, byte pos) {
  _vect_canales1[pos] = val;
}

void *SADyC32::getVectCanales1(int *vect_canales) {
  memcpy(vect_canales, _vect_canales1, sizeof _vect_canales1);
}

void SADyC32::setVectCanales2(int val, byte pos) {
  _vect_canales1[pos] = val;
}

void *SADyC32::getVectCanales2(int *vect_canales) {
  memcpy(vect_canales, _vect_canales2, sizeof _vect_canales2);
}
