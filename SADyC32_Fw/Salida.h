

//Permite setear un PWM en el pin seleccionado ((pinPwm) de 10 a 13) con el Duty-Cicle (dtyCicle)
void setPwm(byte pin, int dtyCicle) {
  if(pin > 9 && pin < 14) {               //No se usa el pin13 (led) ni los pines 0 y 1 (Rx0 y Tx0).
    analogWrite(pin, dtyCicle);
  }
}

//Setea el DAC seleccionado (pinDac) con un valor específico (Valor)
void setDac(byte pin, int valor) {
  if(pin > 65 && pin < 68){
    analogWrite(pin, valor);
  }
}

//Permite setear un pin de salida digital ((pinDig) de 2 a 9) con el ((estado) 0 o 1)
void setSalDig(byte pinDig, bool estado){
  if (pinDig > 1 && pinDig < 10) {               //No se usa el pin13 (led) ni los pines 0 y 1 (Rx0 y Tx0).
    digitalWrite(pinDig, estado);
  }
}

//Rutina que lee los canales digitales de entrada y los unifica en un solo byte "digi_in"
byte leerCanalesDigitales(){
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

//Transmite por RS232 los valores de todos los canales digitales y los canales
//analógicos leídos. Primero copia el vector de canales analógicos
//a modo de congelamiento temporal de datos
void transmTemp() {
  byte i, aux;
  int vectAux1[8];
  int vectAux2[4];

  //modo = s.getModo();

  //Copia el vector de datos de los canales analógicos en un vector auxiliar para su manipulación
  memcpy(vectAux1, vectCanales1, sizeof vectCanales1);
  memcpy(vectAux2, vectCanales2, sizeof vectCanales2);

  Serial.write(leerCanalesDigitales());

  if ((modo == 1) || (modo == 2)) {
    for(i = 0; i < cantCanAnalog1; i++) {
      aux = byte(vectAux1[i] >> 8);    
      Serial.write(aux);    
      Serial.write(vectAux1[i]);
    }
  }
  if ((modo == 3) || (modo == 4)) {
    for(i = 0;i < cantCanAnalog2;i++) {
      aux = byte(vectAux2[i] >> 8);    
      Serial.write(aux);    
      Serial.write(vectAux2[i]);
    }
  }
}
