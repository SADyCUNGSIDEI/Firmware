#include "Globales.h"
#include "Wifi.h"

bool espSend(String comando, int tiempo) {
  bool respuesta;
  Serial3.println(comando);
  delayMicroseconds(tiempo * 1000);
  respuesta = Serial3.find("OK");
  return respuesta;
}

bool espCheck(void) {
  return espSend("AT", 500);
}

bool disableEcho(void) {
  return espSend("ATE0", 500);
}

bool disconnectWifi(void) {
  return espSend("AT+CWQAP", 500);
}

bool espConnect(String red, String clave) {
  return espSend("AT+CWJAP_CUR=\""+red+"\",\""+clave+"\"", 7500);
}

bool espGetIP(void) {
/*  Serial3.println("AT+CIPSTA?");
  while (Serial3.available()) {
    Serial.print(Serial3.read());
  }*/
  return true;
}

bool espServer(void) {
  if (espSend("AT+CIPMUX=1", 500))
    return espSend("AT+CIPSERVER=1,80", 500);
  else
    return false;
}

bool iniWifi(void) {
  disableEcho();
  disconnectWifi();
  if (espCheck()) {
    if (espConnect(nombreAP, claveAP)) {
      if (espServer()) {
        espGetIP();
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
    Serial.println("No se encontro el modulo ESP8266");
    return false;
  }
}

void checkEnvioWiFi(void) {
  if (wifiSendTimeout > 0) {
    if (wifiSendTimeout == 1) { 
      espSend("AT+CIPCLOSE=0", 500);
      wifiBuffer = "";
    }
    wifiSendTimeout--;
  }
}