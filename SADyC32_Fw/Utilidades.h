byte bin2bcd(byte bin) {
  return (bin / 10 * 16) + (bin % 10);
}

byte bcd2bin(byte bcd) {
  return (bcd / 16 * 10) + (bcd % 16);
}

// Función para vaciar el auxbuffer
void vaciarAuxBuffer() {
  for (int i = 1; i < (int)sizeof(auxbuffer); i++) {
    auxbuffer[i] = 0;
  }
}

// Función que sirve para parsear los carácteres que entran por el puerto serial
// inicio: Posición donde se comienza a leer el auxbuffer
// cantidad: Cantidad de carácteres a leer en auxbuffer
int parseValor(int inicio, int cantidad) {
  int val = 0;
  int pot = cantidad - 1;
  int hasta = inicio + cantidad;

  for (int i = inicio; i < hasta + 1; i++) { 
    val += auxbuffer[i] * pow(10, pot--);
  }
  return val;
}

void http(String output) {
  Serial3.print("AT+CIPSEND=0,");               // AT+CIPSEND=0, num
  Serial3.println(output.length());
  if (Serial3.find(">")){                        // Si recibimos la peticion del mensaje
    Serial3.println(output);                    //Aqui va el string hacia el WIFI 
    delay(10);
    while (Serial3.available() > 0 ){ 
      if(Serial3.find("SEND OK")) break;        // Busca el OK en la respuesta del wifi
    }
  }
}

void serialOutput(String data, bool newLine) {
  switch (serialActivo) {
    case SERIAL_0:
      Serial.print(data);
      if (newLine == true) { Serial.println(""); }
    break;
    case SERIAL_2:
    Serial.println("Transmision RS485");
      digitalWrite(RS485_DIR, RS485_Tx);
      Serial2.print(data);
      if (newLine == true) { Serial2.println(""); }
      digitalWrite(RS485_DIR, RS485_Rx);
    break;
    case SERIAL_3:
      wifiSendTimeout = 1000;
      http(data);
      if (newLine == true) { http("\r\n"); }
      delay(10);
    break;
  }
}
