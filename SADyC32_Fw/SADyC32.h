//Impresión de los datos básicos del equipo
void datosEquipo(void){
  serialOutput(F(proyecto), true);
  serialOutput(F(equipo), true);
  serialOutput(F(vers), true);
  serialOutput(F(fecha), true);
}

// Setea el puntero de memoria de la EEPROM (I2C)
void setPumem(unsigned long pmem) {
  pumem = pmem;
  rtcWrite(RTC_M_ST + 1, byte(pmem>>0));
  rtcWrite(RTC_M_ST + 2, byte(pmem>>8));
  rtcWrite(RTC_M_ST + 3, byte(pmem>>16));
  rtcWrite(RTC_M_ST + 4, byte(pmem>>24));
}

// Lee el puntero de memoria de la EEPROM (I2C) que se encuentra grabado en la RAM del RTC
unsigned long getPumem() {
  pumem = pumem + (unsigned long)rtcRead(RTC_M_ST + 1);
  pumem = pumem + ((unsigned long)rtcRead(RTC_M_ST + 2) << 8);  
  pumem = pumem + ((unsigned long)rtcRead(RTC_M_ST + 3) << 16);  
  pumem = pumem + ((unsigned long)rtcRead(RTC_M_ST + 4) << 24);
  return pumem;
}

// Setea el modo de trabajo del equipo y lo guarda en la RAM del RTC
void setModo(byte mode) {
  if (mode < 1 || mode > 5) { mode = 1; }
  modo = mode;
  rtcWrite(RTC_M_MODO, modo);
}

byte getModo() {
  modo = rtcRead(RTC_M_MODO);
  if (modo > 5) { modo = 1; }
  return modo;
}

// Setea la cantidad de canales analógicos y los guarda en la RAM del RTC
void setCanAnalog1(byte cant) {
  if (cant < 1 || cant > 8) { cant = 1; }
  cantCanAnalog1 = cant;
  rtcWrite(RTC_M_CCA1, cant);
}

byte getCanAnalog1() {
  cantCanAnalog1 = rtcRead(RTC_M_CCA1);
  if (cantCanAnalog1 > 8) { cantCanAnalog1 = 1; }
  return cantCanAnalog1;
}

// Setea la cantidad de canales analógicos de tipo inAmp y los guarda en la RAM del RTC
void setCanAnalog2(byte cant) {
  if (cant < 1 || cant > 4) { cant = 1; }
  cantCanAnalog2 = cant;
  rtcWrite(RTC_M_CCA2, cant);
}

byte getCanAnalog2() {
  cantCanAnalog2 = rtcRead(RTC_M_CCA2);
  if (cantCanAnalog2 > 4) { cantCanAnalog2 = 1; }
  return cantCanAnalog2;
}

void setCodigoInicio(byte codigo) {
  rtcWrite(RTC_M_C_INI, codigo);
}

byte getCodigoInicio() {
  return rtcRead(RTC_M_C_INI);
}

void setRegFlag(byte regFlag) {
  rtcWrite(RTC_M_RF, regFlag);
}

byte getRegFlag() {
  return rtcRead(RTC_M_RF);
}

void getWifiParams() {
  for (int i=0; i<=15; i++) {
    nombreAP[i] = rtcRead(RTC_M_AP + i);
    claveAP[i] = rtcRead(RTC_M_PW + i);
  }
}

//Setea en la RAM del RTC el tiempo de registro
void setTiempoReg(unsigned int tiempoReg) {
  rtcWrite(RTC_M_ST + 9, byte(tiempoReg >> 0));
  rtcWrite(RTC_M_ST + 10, byte(tiempoReg >> 8));
}

unsigned int getTiempoReg() {
  unsigned int tiempoReg = 0;
  tiempoReg = tiempoReg + (unsigned int)rtcRead(RTC_M_ST + 9);
  tiempoReg = tiempoReg + ((unsigned int)rtcRead(RTC_M_ST + 10) << 8);
  return tiempoReg;
}

//Permite setear la amplificación de los inAmps. Entrada analógica ((in_inAmp) de 09 a 12)
//con la amplificación ((amplif) 0 a 3)
void setInAmp(byte in_inAmp, byte amplif){
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

//--------------------------------------------------------------------------------------------------
//Rutina que lee el ADC en secuencia
//Cada vez que ejecuta lee el siguiente canal hasta el último seteado
void adcSrv() {
  byte i;
  
  if ((modo == 1) || (modo == 2)) {
    for(i = 0; i < cantCanAnalog1; i++) {
      vectCanales1[i] = analogRead(54 + i);
    }
  }
  if ((modo == 3) || (modo == 4)) {
    for(i = 0; i < cantCanAnalog2; i++) {
      vectCanales2[i] = analogRead(62 + i);
    }
  }
  yield();
}

//Para todos los modos de trabajo, timers y desacopla interrupciones
void parada(void) {
  timer.disable(idTransmTemp);
  timer.deleteTimer(idTransmTemp);
  timer.disable(idRegistroTemp);
  timer.deleteTimer(idRegistroTemp);
  timer.disable(idTransmTempWifi);
  timer.deleteTimer(idTransmTempWifi);
  detachInterrupt(digitalPinToInterrupt(pin1)); 
  detachInterrupt(digitalPinToInterrupt(pin2));
  setRegFlag(0);
}

//--------------------------------------------------------------------------------------------------
//Rutina que borra la EEProm e inicializa el puntero de registro "pumem" a cero
void borrarE2(void) {
  parada();
  eeErase();
  setPumem(0);
}

//--------------------------------------------------------------------------------------------------
//Rutina que graba un datos en la EEProm y guarda el nuevo pumem en la ram del reloj
//Data --> dato a grabar - Se asume que "pumem" apunta al primer valor libre
void grabaDatoE2(byte data) {
  if (mem_full == 0) {
      eeWrite(pumem, data);
      pumem++;
      if (pumem >= MEM_FIN) {
        mem_full = 1;
        parada();
      }
      setPumem(pumem);
  }
}

//--------------------------------------------------------------------------------------------------
//Rutina que graba los datos de inicialización de registro en la EEProm
void grabaIniE2(void) {
  int i;
  char buf[6];

  sprintf(buf,"%04d",getTiempoReg());
  getFechaHoraReg();
  grabaDatoE2(0xff);
  grabaDatoE2(0xff);
  for (i=1;i<=14;i++) {
    grabaDatoE2(char(auxbuffer[i]));
  }
  for (i=0;i<=3;i++) {
    grabaDatoE2(buf[i]);
  }
  grabaDatoE2(char(modo));
  grabaDatoE2(cantCanAnalog1);
  grabaDatoE2(char(cantCanAnalog2));
  grabaDatoE2(0x38);
  grabaDatoE2(getCodigoInicio());
  grabaDatoE2(0xff);
  grabaDatoE2(0xff);
}

//--------------------------------------------------------------------------------------------------
//Rutina de registro temporizado en EEProm (I2C) de los canales y el modo seteados
//Pumem apunta a la primera posición libre de la memoria
void RegistroTempE2(void) {
  int i;
  int vectAux1[8];
  int vectAux2[4];

  Serial.println(F("Rutina de registro"));

  //Copia el vector de datos de los canales analógicos en un vector auxiliar para su manipulación
  memcpy(vectAux1, vectCanales1, sizeof vectCanales1);
  memcpy(vectAux2, vectCanales2, sizeof vectCanales2);

  grabaDatoE2(leerCanalesDigitales());                       //Garaba estado de los 8 canales digitales

  if (modo == 2) {
    for (i = 0; i < cantCanAnalog1; i++) {
      grabaDatoE2(byte(vectAux1[i]));       //Graba valores de los canales analógicos seteados
      grabaDatoE2(byte(vectAux1[i] >> 8));
    }
  }
  if (modo == 4) {
    for (i = 0; i < cantCanAnalog2; i++){
      grabaDatoE2(byte(vectAux2[i]));       //Graba valores de los canales analógicos inAmp seteados
      grabaDatoE2(byte(vectAux2[i] >> 8));
    }
  }
}

//--------------------------------------------------------------------------------------------------
//Rutina que descarga los datos grabados en la EEProm y los trnsmite por RS232
void descargaE2(void) {
  Serial.write(byte(pumem>>24));
  Serial.write(byte(pumem>>16));
  Serial.write(byte(pumem>>8));
  Serial.write(byte(pumem>>0));

  for (unsigned long i = 0; i < pumem; i++){
    Serial.write(eeRead(i));
  }
}

void transmisionTemporizada(int tiempo, boolean wifi) {
  if (tiempo <= 10000 && modo != 0) {
    if (tiempo == 0) {
      if (wifi) {
        transmTempWifi();
      }
      else {
        transmTemp();
      }
    }
    else {
      if (modo == 1 || modo == 3) {
        if (wifi) {
          idTransmTempWifi = timer.setInterval(tiempo, transmTempWifi);
        }
        else {
          idTransmTemp = timer.setInterval(tiempo, transmTemp);  
        }
      }
    }
  }
}

void registroTemporizado(int tiempoReg) {
  if(modo == 2 || modo == 4) {
    if (tiempoReg >= 1 && tiempoReg <= 3600) {
      parada();
      setCodigoInicio(0x01);                         //Guarda en reloj código de inicio
      setTiempoReg(tiempoReg);                       //Guarda en reloj tiempo de registro
      setRegFlag(1);                                 //Guarda en reloj flag de registro
      grabaIniE2();                                  //Almacena en EEProm encabezado de inicio de registro
      idRegistroTemp = timer.setInterval(tiempoReg * 1000, RegistroTempE2);
    }
  }
}

//--------------------------------------------------------------------------------------------------
//Graba el nombre de Red y su Password en la ram del reloj
void configWiFiParams(void) {
  int i=0,j=0;
  
  while (inString[i] != 0xff) {
    rtcWrite(RTC_M_AP+i,inString[i]);
    i++;
  }
  rtcWrite(RTC_M_AP+i,NULL);
  
  i++;
  while (inString[i] != 0xff) {
    rtcWrite(RTC_M_PW+j,inString[i]);
    i++;
    j++;
  }
  rtcWrite(RTC_M_PW+j,NULL);
}

void saludo() {
  for (int i=1;i<=4;i++){
    digitalWrite(TST,HIGH);
    delay(250);
    digitalWrite(TST,LOW);
    delay(250);
  }
  datosEquipo();
  serialOutput("Modo: ", false);
  serialOutput(String(modo), true);
  serialOutput("Cantidad canales analogicos: ", false);
  serialOutput(String(cantCanAnalog1), true);
  serialOutput("Cantidad canales analogicos inAmp: ", false);
  serialOutput(String(cantCanAnalog2), true);
  serialOutput("Pumem: ", false);
  serialOutput(String(pumem), true);
  serialOutput("Código de inicio: ", false);
  serialOutput(String(getCodigoInicio()), true);
  serialOutput("RegFlag: ", false);
  serialOutput(String(getRegFlag()), true);
}

//Maquina de estados que interpreta la secuencia de un comando por RS232
//Recibe cada dato en secuencia hsta que encuentra el fin de comando
//O sea, el formato es: -->   ESC  Comando  CR
//Si el comando no se completa o no existe se va por tiempo
//Si el comando existe ejecuta en consecuencia
void parsing(void) {
  int tiempo;
  unsigned long aux;

  switch(comando){
    case '1':                               //Test de los chips de EEProm instalados -( ESC 1 Valor CR ) - Valor => 0 a 255
      byte data;

      data = parseValor(1,3);

      eeTest(data);
    break;
    case '2':                               //Test de la comunicación RS232 - ( ESC 2 Char CR ) - Char => cialquier caracter ASCII
      Serial.println(inString);      
    break;
    case 'A':                               //Recibe cantidad de canales analógicos - ( ESC A Cantidad CR) - Cantidad => 1 a 8
      aux = inString.toInt();
      setCanAnalog1(aux);
    break;
    case 'B':                               //Recibe cantidad de canales analógicos tipo inAmp - ( ESC B Cantidad CR) - Cantidad => 1 a 4
      aux = inString.toInt();
      setCanAnalog2(aux);
    break;
    case 'C':                               //Seteo de la amplificación de los inAmp - (ESC C Entrada Amplificación CR) - Entrada => 09 a 12 - Amplificación => 0 a 3
      byte in_inAmp, amplif;

      in_inAmp = parseValor(1,2);
      amplif = parseValor(3,1);

      setInAmp(in_inAmp, amplif);
    break;
    case 'D':                               //Medición de tiempo entre eventos - ( ESC D Pin1 Pin2 CR ) - Pin1 y Pin2 => patas válidas, puede ser la misma
        pin1 = parseValor(1, 2);            //Patas válidas:  46, 47, 49, 50, 51, 52, 53
        pin2 = parseValor(3, 2);

        setMedicion(pin1, pin2);
    break;
    case 'E':                               //Recibe el modo de trabajo del equipo - ( ESC E Modo CR ) - Modo => 1 a 5
        aux = inString.toInt();
        setModo(aux);
    break;
    case 'F':                               //Borrado EEProm - ( ESC F CR )
        if(modo == 1) {
          borrarE2();
        }
    break;
    case 'H':                               //Seteo del reloj de tiempo real por I2c - ( ESC H hhmmssDDMMAAAA CR )
      byte hora;
      byte minuto;
      byte segundo;
      byte dia;
      byte mes;
      int anio;

      hora = parseValor(1,2);
      minuto = parseValor(3,2);
      segundo = parseValor(5,2);
      dia = parseValor(7,2);
      mes = parseValor(9,2);
      anio = parseValor(13,2);
    
      setFechaHora(hora, minuto, segundo, dia, mes, anio);
    break;
    case 'M':                               //Generación de PWM - ( ESC M Pin Duty CR) - Pin => 10 al 13 - Duty => 0000 a 4095
      int pinPwm;
      int dtyCicle;

      pinPwm = parseValor(1, 2);
      dtyCicle = parseValor(3, 4);

      setPwm(pinPwm, dtyCicle);
    break;
    case 'N':                               //Generación de señales analógicas en DACs - ( ESC N Pin Steps CR ) - Pin => 00 al 01 - Steps => 0000 a 4095
      int pinDac;
      int valor;

      pinDac = parseValor(1, 2);
      valor = parseValor(3, 4);

      setDac(pinDac, valor);
    break;
    case 'O':                               //Transmite datos de un archivo en memoria SD - ( ESC O Archivo CR ) - Archivo en formato --> 15.3 máximo
      archivo = inString;
      leerArchSD(archivo);
    break;
    case 'P':                               //Parada de los modos del equipo - ( ESC P CR )
      parada();
    break;
    case 'Q':                               //Graba un dato fijo en un archivo en memoria SD - ( ESC Q Archivo CR ) - Archivo en formato --> 15.3 máximo
      archivo = inString;
      grabarDatoSD(archivo, 'a');
    break;
    case 'R':                               //Registro temporizado de los canales digitales y analógicos seteados - ( ESC R Tiempo CR ) - Tiempo => 1 a 3600 segundos
      tiempo = inString.toInt();
      registroTemporizado(tiempo);
    break;
    case 'S':                               //Seteo de la salida digital - (ESC S Salida Estado CR) - Salida => 02 a 09 - Estado 0 o 1
      byte pinDig;
      bool estado;

      pinDig = parseValor(1,2);
      estado = parseValor(3,2);

      setSalDig(pinDig, estado);
    break;
    case 'T':                               //Transmisión temporizada de los canales digitales y analógicos seteados - ( ESC T Tiempo CR ) - Tiempo => 1 a 10000 milisegundos || Tiempo=0 -> una transmisión
      tiempo = inString.toInt();            //(En los datos analógicos se transmite primero el byte alto y luego el bajo)
      transmisionTemporizada(tiempo, false);
    break;
    case 'W':                               //Seteo de Red y Password para WiFi - (ESC W Red 0xFF Pass 0xFF CR) - Red y Pass máximo 15 caracteres ASCII
      configWiFiParams();
    case 'd':                               //Descarga de datos desde la EEProm - ( ESC d CR )
      parada();
      descargaE2();
      setCodigoInicio(0x02);                //Guarda en reloj código de descarga
      //ini_var();
    break;
    case 'e':                               //Transmite datos del equipo - ( ESC e CR ) - Responde con los datos básicos del proyecto
      datosEquipo();
      espGetIP();
    break;
    case 'h':                               //Transmite día y hora del RTC - ( ESC h CR ) - Responde con día y hora del equipo
      serialOutput(getFechaHora(), true);
    break;
    case 'g':
      saludo();
    break;
    case 'k':
      tiempo = inString.toInt();
      transmisionTemporizada(tiempo, true);
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
      parsing();
      inString = "";
      comm0_out = 0;
      k_g = 0;
    }
}

//--------------------------------------------------------------------------------------------------
//Inicialización de todas las variables para una correcta ejecución del programa
void iniVar(void) {
  setCodigoInicio(0x00);                              //Guarda en reloj código de inicio
  pumem = getPumem();
  modo = getModo();
  cantCanAnalog1 = getCanAnalog1();
  cantCanAnalog2 = getCanAnalog2();
  getWifiParams();

  SerialActivo serialActivo = NO_SERIAL;

  inString = "";

  archivo.reserve(LARGO_ARCH);

  if (getRegFlag() == 1) {
    grabaIniE2();                                             //Almacena en EEProm encabezado por vuelta de alimentación
    idRegistroTemp = timer.setInterval(getTiempoReg() * 1000, RegistroTempE2);
  }
  // Inicialización de la tarjeta SD
  if (!SD.begin(CS2)) {
    Serial.println(F("Tarjeta SD no presente, sin formato  o fallada"));
//    return;
  } else {
    Serial.println(F("card initialized."));
  }

  // Inicialización de la memoria flash (SPI)
  if (!SerialFlash.begin(CS1)) { 
    Serial.println(F("No se puede acceder a la memoria Flash por SPI")); 
  } else {
    Serial.println(F("Flash encontrada e inicializada."));
  }

  // Inicialización del Wi-Fi
  if (iniWifi()) {
    Serial.println("WiFi ESP8266 inicializada");
  }else{
    Serial.println("Wifi ESP8266 no encontrada");
  }
}



//--------------------------------------------------------------------------------------------------
//Formateo de una página en html para enviarla por la placa ESP8266 a un browser remoto
void envioPagina(String titulo, String cuerpo, int tiempo) {
  http("<!DOCTYPE HTML>");
  http("<html>");
  http("<head><title>");
  http(titulo);
  http("</title>");
  http("<style>");
  http("iframe {");
  http("width:100%;");
  http("height:500px;");
  http("}");
  http("fieldset {");
  http("margin:0;");
  http("padding:0;");
  http("width:100%;");
  http("height:100%;");
  http("}");
  http("</style>");
  http("<script>");
  http("function reloadIFrame(com) {");
  http("document.getElementById('contenido').src = \"http://192.168.0.104/?com=\" + com");
  http("}");
  http("</script>");
  http("<body>");
  http("<table width=\"100%\">");
  http("<tr>");
  http("<td width=\"10%\">Comando:</td>");
  http("<td width=\"70%\"><input type=\"text\" style=\"width:98%\" id=\"comando\"></td>");
  http("<td width=\"20%\"><button onclick=\"reloadIFrame(document.getElementById('comando').value)\">Enviar</button></td>");
  http("</tr>");
  http("<tr>");
  http("<td width=\"100%\" colspan=\"3\">");
  http("<fieldset>");
  http("<iframe src=\"www.google.com\" id=\"contenido\" frameborder=\"0\"></iframe>");
  http("</fieldset>");
  http("</td>");
  http("</tr>");
  http("</table>");
  http("</body></html>");
  delay(1);
  Serial3.println("AT+CIPCLOSE=0");
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
            envioPagina("SADyC32", "Cuerpo", 5);
          }
      }
    }
  }
}
