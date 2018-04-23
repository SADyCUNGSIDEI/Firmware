#include "Arduino.h"
#include "Globales.h"
#include "Nucleo.h"
#include "Reloj.h"
#include "Comunicacion.h"
#include "Wifi.h"
#include "Memorias.h"

int parseValor(int inicio, int cantidad) {
  int val = 0;
  int pot = cantidad - 1;
  int hasta = inicio + cantidad;

  for (int i = inicio; i < hasta + 1; i++) { 
    val += auxbuffer[i] * pow(10, pot--);
  }
  return val;
}

void serialOutput(String data, bool newLine) {
  switch (serialActivo) {
    case SERIAL_0:
      Serial.print(data);
      if (newLine == true) { Serial.println(""); }
    break;
    case SERIAL_1:

    break;
    case SERIAL_2:
    Serial.println("Transmision RS485");
      // digitalWrite(RS485_DIR, RS485_Tx);
      // Serial2.print(data);
      // if (newLine == true) { Serial2.println(""); }
      // digitalWrite(RS485_DIR, RS485_Rx);
    break;
    case SERIAL_3:
      // wifiSendTimeout = 1000;
      // http(data);
      // if (newLine == true) { http("\r\n"); }
      // delay(10);
    break;
    default:
    break;
  }
}

void InComando(char inChar) {
  // Si estadoEspera = true evalúa los caracteres recibidos
  if (estadoEspera) {
    comm0_out = TIME_OUT;
    // Si es el primer carácter, es el comando a ejecutar
    if (k_g == 0) {
      comando = inChar;
    }
    else {
      inString += inChar;
      if (isDigit(inChar)) {
        if (k_g < VECT_END) {
          inChar = inChar - 48;
          auxbuffer[k_g] = inChar;
        }
      }
    }
    k_g++;
  }

  // Si se recibe el carácter de ESCAPE estadoEspera = true
  if (inChar == ESCAPE) {
    comm0_out = TIME_OUT;
    estadoEspera = true;
  }

  // Si es IN_END y el time out no es 0 llama a parsing para ejecutar el comando
  if ((inChar == IN_END) && (comm0_out != 0)) {
    ejecucion(comando);
    inString = "";
    comm0_out = 0;
    k_g = 0;
  }
}

void serialInput(HardwareSerial &port) {
  if (port.available()) {
    if (serialActivo == SERIAL_0) {
      InComando(port.read());  
    }
    if (serialActivo == SERIAL_1) {
      //Serial.print(port.read());
    }
    if (serialActivo == SERIAL_2) {
      InComando(port.read());
    }
    if (serialActivo == SERIAL_3) {
      // Si se detecta el carácter "+" es una petición http
      if (port.read() == '+') {
          if (port.find("?com=")) {
            char comando;
            delay(30);
            InComando(ESCAPE);
            while (comando != ' ') {
              comando = port.read();
              InComando(comando);
            }
            InComando(IN_END);
          }
          else {
            // envioPagina("SADyC32", "Cuerpo", 5);
          }
      }
    }
  }
}
