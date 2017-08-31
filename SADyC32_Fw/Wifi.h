bool espSend(String comando, int tiempo) {
  bool respuesta;
  Serial3.println(comando);
  delay(tiempo);
  respuesta = Serial3.find("OK");
  return respuesta;
}

bool espCheck() {
  return espSend("AT", 500);
}

bool espConnect(String red, String clave) {
  return espSend("AT+CWJAP_CUR=\""+red+"\",\""+clave+"\"", 7500);
}

bool espServer() {
  if (espSend("AT+CIPMUX=1", 500))
    return espSend("AT+CIPSERVER=1,80", 500);
  else
    return false;
}

bool ini_Wifi(void) {
  if (espCheck()) {
    if (espConnect(WIFI_AP, WIFI_CLAVE)) {
      if (espServer()) {        
        return true;
      }
      else {
        Serial.println("No se pudo levantar el server");
        return false;
      }
    }
    else {
      Serial.println("Error en la conexion a la red");
      return false;
    }
  }
  else {
    Serial.println("No se encontro el módulo ESP8266");
    return false;
  }
}

void transmTemp2() {
  byte i, aux;
  int vectAux1[8];
  int vectAux2[4];
  //byte buff[8];
  String buff = "";
  String aux2 = "";

  //Copia el vector de datos de los canales analógicos en un vector auxiliar para su manipulación
  memcpy(vectAux1, vectCanales1, sizeof vectCanales1);
  memcpy(vectAux2, vectCanales2, sizeof vectCanales2);

  Serial3.println("AT+CIPSEND=0,4");
  delay(5);
  Serial3.write(leerCanalesDigitales());
  if ((modo == 1) || (modo == 2)) {
    for (i = 0; i < cantCanAnalog1; i++) {
      aux = byte(vectAux1[i] >> 8);
      Serial3.write(aux);
      Serial3.write(vectAux1[i]);
    }
  }
  if ((modo == 3) || (modo == 4)) {
    for(i = 0; i < cantCanAnalog1; i++){
      Serial.write(byte(vectAux1[i] >> 8));
      Serial.write(vectAux1[i]);
    }
  }
}
