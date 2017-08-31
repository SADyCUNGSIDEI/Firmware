//Impresión de los datos básicos del equipo
void datosEquipo(void){
  Serial.println(F(proyecto));
  Serial.println(F(equipo));
  Serial.println(F(vers));
  Serial.println(F(fecha));
}

void saludo() {
  for (int i=1;i<=4;i++){
    digitalWrite(TST,HIGH);
    delay(250);
    digitalWrite(TST,LOW);
    delay(250);
  }
  datosEquipo();
  Serial.print("Modo: ");
  Serial.println(modo);
  Serial.print("Cantidad canales analogicos: ");
  Serial.println(cantCanAnalog1);
  Serial.print("Cantidad canales analogicos inAmp: ");
  Serial.println(cantCanAnalog2);
}

void setModo(byte modo) {
  modo = modo;
}

void setCanAnalog1(byte cant) {
  if (cant < 1 || cant > 8) { cant = 1; }
  cantCanAnalog1 = cant;
}

void setCanAnalog2(byte cant) {
  if (cant < 1 || cant > 4) { cant = 1; }
  cantCanAnalog2 = cant;
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
}

//Para todos los modos de trabajo, timers y desacopla interrupciones
void parada(void){
  timer.disable(id_Transm_Temp);
  timer.deleteTimer(id_Transm_Temp);
  timer.disable(id_Registro_Temp);
  timer.deleteTimer(id_Registro_Temp);
  detachInterrupt(digitalPinToInterrupt(pin1)); 
  detachInterrupt(digitalPinToInterrupt(pin2));
  reg_flag = 0;
  //rtc_write((rtc_m_st + 8), reg_flag);
}

//Maquina de estados que interpreta la secuencia de un comando por RS232
//Recibe cada dato en secuencia hsta que encuentra el fin de comando
//O sea, el formato es: -->   ESC  Comando  CR
//Si el comando no se completa o no existe se va por tiempo
//Si el comando existe ejecuta en consecuencia
void parsing(void){
  int tiempo;
  unsigned long aux;

  switch(comando){
    case '1':                               //Test de los chips de EEProm instalados -( ESC 1 Valor CR ) - Valor => 0 a 255
      byte data;

      data = parseValor(1,3);

//      test_eeprom(data);
    break;
    case '2':                               //Test de la comunicación RS232 - ( ESC 2 Char CR ) - Char => cialquier caracter ASCII
//      Serial.print(inChar);      
    break;
    case 'A':                               //Recibe cantidad de canales analógicos - ( ESC A Cantidad CR) - Cantidad => 1 a 8
      aux = inString.toInt();
      setCanAnalog1(aux);
      setMemCanAnalog1(aux);
    break;
    case 'B':                               //Recibe cantidad de canales analógicos tipo inAmp - ( ESC B Cantidad CR) - Cantidad => 1 a 4
      aux = inString.toInt();
      setCanAnalog1(aux);
      setMemCanAnalog2(aux);
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
        setMemModo(aux);
    break;
    case 'F':                               //Borrado EEProm - ( ESC F CR )
     // if(modo == 0) {
//        borrado_E2();
     // }
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
    case 'M':                               //Generación de PWM - ( ESC M Pin Duty CR) - Pin => 02 al 13 - Duty => 0000 a 4095
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
//        leo_arch_sd(archivo);
    break;
    case 'P':                               //Parada de los modos del equipo - ( ESC P CR )
      parada();
    break;
    case 'Q':                               //Graba un dato fijo en un archivo en memoria SD - ( ESC Q Archivo CR ) - Archivo en formato --> 15.3 máximo
      archivo = inString;
//      grabo_dato_sd(archivo, 'a');
    break;
    case 'R':                               //Registro temporizado de los canales digitales y analógicos seteados - ( ESC R Tiempo CR ) - Tiempo => 1 a 3600 segundos
      tiempo = inString.toInt();
//      registro_temporizado(tiempo);
    break;
    case 'S':                               //Seteo de la salida digital - (ESC S Salida Estado CR) - Salida => 2 a 9 - Estado 0 o 1
      byte pinDig;
      bool estado;

      pinDig = parseValor(1,1);
      estado = parseValor(2,1);

      setSalDig(pinDig, estado);
    break;
    case 'T':                               //Transmisión temporizada de los canales digitales y analógicos seteados - ( ESC T Tiempo CR ) - Tiempo => 1 a 10000 milisegundos || Tiempo=0 -> una transmisión
      tiempo = inString.toInt();            //(En los datos analógicos se transmite primero el byte alto y luego el bajo)
      if (tiempo <= 10000 && modo != 0) {
        if (tiempo == 0){
          transmTemp();
        }
        else {
          if(modo == 1 || modo == 3) {
            id_Transm_Temp = timer.setInterval(tiempo, transmTemp);
          }
        }
      }
    break;
    case 'd':                               //Descarga de datos desde la EEProm - ( ESC d CR )
      parada();
//      descarga_E2();
      //rtc_write(rtc_m_st + 7, 0x02);        //Guarda en reloj código de descarga
//      ini_var();
    break;
    case 'e':                               //Transmite datos del equipo - ( ESC e CR ) - Responde con los datos básicos del proyecto
      datosEquipo();
    break;
    case 'h':                               //Transmite día y hora del RTC - ( ESC h CR ) - Responde con día y hora del equipo
      Serial.println(getFechaHora());
    break;
    case 'g':
      saludo();
    break;
    case 'k':
      id_Transm_Temp = timer.setInterval(50, transmTemp2);
    break;
  }
}

void in_Comando(char inChar) {
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

void serialManager(HardwareSerial &port) {
  if (port.available()) {
    in_Comando(port.read());
  }
}
