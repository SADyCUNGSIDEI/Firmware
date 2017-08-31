volatile byte startFlag = false;
volatile byte stopFlag = false;
volatile unsigned long toma1, toma2;

void trigSamePin() {
  if ((startFlag == true) && (stopFlag == false)) {
    toma2 = micros();
    stopFlag = true;
  }
  if ((startFlag == false) && (stopFlag == false)) {
    Serial.println("Inicio medicion");
    toma1 = micros();
    startFlag = true;
  }
}

void trigPin1() {
  if((startFlag == false) && (stopFlag == false)) {
    toma1 = micros();
    startFlag = true;
  }
}

void trigPin2() {
  if((startFlag == true) && (stopFlag == false)) {
    toma2 = micros();
    stopFlag = true;
  }
}

void setMedicion(byte pin1, byte pin2) {
  if((pin1 > 48) && (pin1 < 54) && (pin2 > 48) && (pin2 < 54)) {
    if (pin1 == pin2) {
      attachInterrupt (pin1, trigSamePin, RISING);
    }
    else {
      attachInterrupt (pin1, trigPin1, CHANGE);
      attachInterrupt (pin2, trigPin2, CHANGE);
    }
  }
}

void checkMedicion() {
  if((startFlag == true) && (stopFlag == true)) {
    toma2 = toma2 - toma1;
    Serial.print(F("Tiempo entre eventos: "));
    Serial.print(toma2);
    Serial.println(F(" microsegundos"));
    startFlag = false;
    stopFlag = false;
  }
}
