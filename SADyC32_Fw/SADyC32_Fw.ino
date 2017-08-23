//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//IDEI - UNGS
//Proyecto SADyC32
//Se utiliza la placa microcontroladora Arduino Due (uC: ATSam3X8E)
//--------------------------------------------------------------------------------------------------
#include <SimpleTimer.h>      //Biblioteca para setear tareas controladas por timer
#include <Scheduler.h>        //Biblioteca para schedulear tareas independiente del lazo principal
#include <Wire.h>             //Biblioteca para manejo del bus I2C
#include <SPI.h>              //Biblioteca para manejo del bus SPI
#include <SD.h>               //Biblioteca para manejo de la memoria SD/MMC
#include <SerialFlash.h>      //Biblioteca para manejo de la memoria Flash 

#include "SADyC32.h"
#include "Salida.h"
#include "Reloj.h"
#include "Medicion.h"


SimpleTimer timer;            //Asignación del timer (timer) utilizado para las transmiciones sincrónicas
SimpleTimer timer1;           //Asignación del timer (timer1) utilizado para las transmiciones sincrónicas


#define WP1 30                //Salida que maneja el WRITE PROTECT protect de la memoria EEprom-I2C: 24AA1025
#define MEM_FIN 524200        //Fin de la EEProm (se reservan los 88 bytes del final)

#define WP 32                 //Salida que maneja el WRITE PROTECT de la memoria EEprom-SPI: S25FL127S
#define HOLD 33               //Salida que maneja el HOLD de la memoria EEprom-SPI: S25FL127S 
#define CS1 31                //Salida que maneja el CHIP SELECT de la memoria EEprom-SPI: S25FL127S

#define CS2 47                //Salida que maneja el CHIP SELECT de la memoria SD (placa nueva --> 34 ; Mikroe --> 47)
#define WP_SD 35              //Entrada para detectar el Write Protect de la memoria SD
#define CD 44                 //Entrada para detectar presencia del CHIP SD

#define RS485_DIR 43          //Salida que maneja la direccón de la comunicación RS232
#define RS485_Rx LOW          //Valor para que el RS485-tranceiver este en recepción
#define RS485_Tx HIGH         //Valor para que el RS485-tranceiver este en transmición


#define ESCAPE 0x1b           //Escape --> 0x1B - Definido "99" solo para pruebas en consola que no admiten "0x1B"
#define IN_END 0x0d           //Fin de mensajes
#define TIME_OUT 25000       //Time-out de recepción
#define VECT_END 30           //Tamaño vector para valores de los canales 
#define LARGO_ARCH 15         //Máximo largo de archivo


//--------------------------------------------------------------------------------------------------
//Seteo del bus SPI para manejo de la memoria SD/MMC
SPISettings Spi_mem_param(50000000, MSBFIRST, SPI_MODE0); 

//--------------------------------------------------------------------------------------------------
const byte mem_ad = 0xa0;       //Dirección de base de la memoria EEProm-I2C: 24AA1025     

//volatile => estan en RAM y no en registros. Muy importante en interrupciones.

boolean estadoEspera = false;

bool mem_full;                  //Indicador de memoria EEProm llena
volatile bool Wifi_ini;         //Indicador que la placa WiFi eta inicializada (True --> inicializada)
//------
volatile byte modo;
volatile byte reg_flag;
byte auxbuffer[VECT_END];
byte k_g = 0;
volatile byte pin1, pin2;
//------
int comando;
int i;
int id_Transm_Temp;
int id_Registro_Temp;
int vect_canales1[8];
int vect_canales2[4];
int vect_aux1[8];
int vect_aux2[4];
volatile unsigned int tiempo_reg;
//------
unsigned long comm0_out;
volatile unsigned long pumem;
volatile unsigned long Toma1, Toma2;
//------
String inString = "";    
//------
String archivo;


SADyC32 sadyc;
Reloj r;
Salida s;
Medicion * Medicion::instances [3] = { NULL, NULL, NULL };
Medicion m; 

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------      

void setup() {
  Serial.begin(115200);                 //Para comunicación serial (TX0, RX0) vía conector USB

  Serial1.begin(115200);                 //Para comunicación serial (TX1, RX1) vía conector RS232
    
  pinMode(RS485_DIR, OUTPUT);     
  digitalWrite(RS485_DIR, RS485_Rx);    //Prepara para recepción por RS485
  Serial2.begin(4800);                  //Para comunicación por RS485 (TX2, RX2)
  
  Serial3.begin(115200);                //Para comunicación por WiFi (TX3, RX3) (ESP8266)

  Wire.begin();
  
  pinMode(TST, OUTPUT);
  digitalWrite(TST, LOW);

  pinMode(DIG36, OUTPUT);                 
  pinMode(DIG37, OUTPUT);
  pinMode(DIG38, OUTPUT);
  pinMode(DIG39, OUTPUT);
  pinMode(DIG40, OUTPUT);
  pinMode(DIG41, OUTPUT);

  pinMode(DIN0, INPUT);
  pinMode(DIN1, INPUT);
  pinMode(DIN2, INPUT);
  pinMode(DIN3, INPUT);
  pinMode(DIN4, INPUT);
  pinMode(DIN5, INPUT);
  pinMode(DIN6, INPUT);
  pinMode(DIN7, INPUT);

  pinMode(A8, INPUT_PULLUP);                 

  pinMode(WP1, OUTPUT);                 
  digitalWrite(WP1, HIGH);
  
  pinMode(CS1, OUTPUT);
  digitalWrite(CS1, HIGH);
  pinMode(HOLD, OUTPUT);
  digitalWrite(HOLD, HIGH);
  pinMode(WP, OUTPUT);
  digitalWrite(WP, HIGH);

  pinMode(CS2, OUTPUT);
  digitalWrite(CS2, HIGH);
  pinMode(WP_SD, INPUT);
  pinMode(CD, INPUT);
 
  if (!SD.begin(CS2)) {
    Serial.println(F("Tarjeta SD no presente, sin formato  o fallada"));
//    return;
  } else {
    Serial.println(F("card initialized."));
  }

  if (!SerialFlash.begin(CS1)) { 
    Serial.println(F("No se puede acceder a la memoria Flash por SPI")); 
  } else {
    Serial.println(F("Flash encontrada e inicializada."));
  }
  
  analogReference(AR_DEFAULT);                            //VER COMO SE SETEA LA REFERENCIA EXTERNA !!!!!!!
  REG_ADC_MR = (REG_ADC_MR & 0xFFF0FFFF) | 0x00020000;    //Para convertir en 3,97uS aprox. en vez de 40uS.
  analogReadResolution(MAX_RESOL);
  analogWriteResolution(MAX_RESOL);

  //rtc_write(rtc_m_st + 7, 0x00);                              //Guarda en reloj código de inicio
  ini_var();
  Scheduler.startLoop(Adc_Srv);
  
  sadyc.setModo(r.getMemModo());
  sadyc.setCanAnalog1(r.getMemCanAnalog1());
  sadyc.setCanAnalog2(r.getMemCanAnalog2());
  sadyc.saludo();
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
void loop() {
  timer.run();

  if (comm0_out != 0) {
    digitalWrite(TST, HIGH);
    comm0_out--;
  }
  else {
    digitalWrite(TST, LOW);
    estadoEspera = false;
    vaciarAuxBuffer();
  }

  m.checkMedicion();

  yield();
}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//---------------------------     RUTINAS     ------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Inicialización de todas las variables para una correcta ejecución del programa

void ini_var(void){
  
  /*pumem = 0;
  pumem = pumem + (unsigned long)rtc_read(rtc_m_st + 1);
  pumem = pumem + ((unsigned long)rtc_read(rtc_m_st + 2) << 8);  
  pumem = pumem + ((unsigned long)rtc_read(rtc_m_st + 3) << 16);  
  pumem = pumem + ((unsigned long)rtc_read(rtc_m_st + 4) << 24);  */
  
  //reg_flag = rtc_read(rtc_m_st + 8);
  tiempo_reg = 0;
/*  tiempo_reg = tiempo_reg + (unsigned int)rtc_read(rtc_m_st + 9);
  tiempo_reg = tiempo_reg + ((unsigned int)rtc_read(rtc_m_st + 10) << 8);  */


//  contCan_analog1 = 0;
//  contCan_analog1 = 0;
  inString = "";
  Wifi_ini = false;

  archivo.reserve(LARGO_ARCH);

  ini_var_def();                        //COMENTAR PARA VERSION DEFINITIVA !!!!!!!

  if(reg_flag == 1) {
    graba_ini_E2();                                             //Almacena en EEProm encabezado por vuelta de alimentación
    id_Registro_Temp = timer.setInterval(tiempo_reg * 1000, Registro_Temp_E2);
  }
/*  if(ini_Wifi()){
    Serial.println("WiFi ESP8266 inicializada");
    Wifi_ini = true;
  }else{
    Serial.println("Wifi ESP8266 no encontrada");
  }*/
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Inicialización de todas las variables para una correcta ejecución del programa

void ini_var_def(void){
  //cantCan_analog1 = 8;
  //cantCan_analog2 = 4;
  //modo = 1;
  reg_flag = 0;
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
bool ini_Wifi(void){
  char c;
  int i = 0;
  int index = 0;
  String ordenes[]= {  
    "ATE0",
    "AT+CWAUTOCONN=0",
    "AT+CWQAP",
    "AT+CWJAP_CUR=\"WiFi-Arnet-5c53\",\"5AC5C63220\"",    //Nombre de red y password
    "AT+CIPSTA?",
    "AT+CIPMUX=1",
    "AT+CIPSERVER=1,80",
    "END"                                                // "END" --> Para reconocer el fin de los comandos AT
  };
  String s ="";
  
  while(ordenes[index] != "END"){  
    Serial3.println(ordenes[index]);
    if(index == 3) delay(7500);
    delay(500);
    index++;
    s="";
    while(Serial3.available()){
      c = Serial3.read();
      Serial.print(c);
      if(c != '\n') s = s + c;
      else s="";
      if (s.startsWith("OK")) i++;
//      if(c == 'O') i++;
//      if(c == 'K') i++;
    }
    Serial.println("....................");
  }  
  if(i >= 7) {
    return true;
  }else{
    return false;
  }
}

//Rutina que lee el ADC en secuencia
//Cada vez que ejecuta lee el siguiente canal hasta el último seteado

void Adc_Srv(){
  s.adcSrv(sadyc);
  yield();
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Máquina de estados que recibe datos por RS232 (Terminal arduino)
//Valida la recepción de un comando

void serialEvent() {
  while (Serial.available()) {
    in_Comando(Serial.read());
  }
}
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//Máquina de estados que recibe datos por RS232 (Propio de la placa)
//Valida la recepción de un comando

void serialEvent1() {
  while (Serial1.available()) {
    in_Comando(Serial1.read());
  }
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Máquina de estados que recibe datos por RS485

void serialEvent2() {
  while (Serial2.available()) {
    in_Comando(Serial2.read());
  }
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Máquina de estados que recibe datos por WiFi (Placa ESP8266)

void serialEvent3() {
  char inChar;
  inChar = Serial3.read();
  
  while (Serial3.available()) {
    if(Wifi_ini){
      in_Comando(inChar);
    }else{
      Serial3.print(inChar);
    }
  }
}
//--------------------------------------------------------------------------------------------------

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
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
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

      test_eeprom(data);
    break;
    case '2':                               //Test de la comunicación RS232 - ( ESC 2 Char CR ) - Char => cialquier caracter ASCII
//      Serial.print(inChar);      
    break;
    case 'A':                               //Recibe cantidad de canales analógicos - ( ESC A Cantidad CR) - Cantidad => 1 a 8
      aux = inString.toInt();
      sadyc.setCanAnalog1(aux);
      r.setMemCanAnalog1(aux);
    break;
    case 'B':                               //Recibe cantidad de canales analógicos tipo inAmp - ( ESC B Cantidad CR) - Cantidad => 1 a 4
      aux = inString.toInt();
      sadyc.setCanAnalog1(aux);
      r.setMemCanAnalog2(aux);
    break;
    case 'C':                               //Seteo de la amplificación de los inAmp - (ESC C Entrada Amplificación CR) - Entrada => 09 a 12 - Amplificación => 0 a 3
      byte in_inAmp, amplif;
    
      in_inAmp = parseValor(1,2);
      amplif = parseValor(3,1);

      sadyc.setInAmp(in_inAmp, amplif);
    break;
    case 'D':                               //Medición de tiempo entre eventos - ( ESC D Pin1 Pin2 CR ) - Pin1 y Pin2 => patas válidas, puede ser la misma
        pin1 = parseValor(1, 2);           //Patas válidas:  46, 47, 49, 50, 51, 52, 53
        pin2 = parseValor(3, 2);
        
        m.setMedicion(pin1, pin2);
    break;
    case 'E':                               //Recibe el modo de trabajo del equipo - ( ESC E Modo CR ) - Modo => 1 a 5
        aux = inString.toInt();
        sadyc.setModo(aux);
        r.setMemModo(aux);
    break;
    case 'F':                               //Borrado EEProm - ( ESC F CR )
     // if(modo == 0) {
        borrado_E2();
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
    
      r.setFechaHora(hora, minuto, segundo, dia, mes, anio);
    break;
    case 'M':                               //Generación de PWM - ( ESC M Pin Duty CR) - Pin => 02 al 13 - Duty => 0000 a 4095
      int pinPwm;
      int dtyCicle;

      pinPwm = parseValor(1, 2);
      dtyCicle = parseValor(3, 4);

      s.setPwm(pinPwm, dtyCicle);
    break;
    case 'N':                               //Generación de señales analógicas en DACs - ( ESC N Pin Steps CR ) - Pin => 00 al 01 - Steps => 0000 a 4095
      int pinDac;
      int valor;

      pinDac = parseValor(1, 2);
      valor = parseValor(3, 4);

      s.setDac(pinDac, valor);
    break;
    case 'O':                               //Transmite datos de un archivo en memoria SD - ( ESC O Archivo CR ) - Archivo en formato --> 15.3 máximo
        archivo = inString;
        leo_arch_sd(archivo);
    break;
    case 'P':                               //Parada de los modos del equipo - ( ESC P CR )
      parada();
    break;
    case 'Q':                               //Graba un dato fijo en un archivo en memoria SD - ( ESC Q Archivo CR ) - Archivo en formato --> 15.3 máximo
      archivo = inString;
      grabo_dato_sd(archivo, 'a');
    break;
    case 'R':                               //Registro temporizado de los canales analógicos seteados - ( ESC R Tiempo CR ) - Tiempo => 1 a 3600 segundos
      tiempo = inString.toInt();
      registro_temporizado(tiempo);
    break;
    case 'S':                               //Seteo de la salida digital - (ESC S Salida Estado CR) - Salida => 2 a 9 - Estado 0 o 1
      byte pinDig;
      bool estado;

      pinDig = parseValor(1,1);
      estado = parseValor(2,1);

      s.setSalDig(pinDig, estado);
    break;
    case 'T':                               //Transmisión temporizada de los canales analógicos seteados - ( ESC T Tiempo CR ) - Tiempo => 1 a 10000 milisegundos || Tiempo=0 -> una transmisión      
      tiempo = inString.toInt();
      if (tiempo <= 10000 && sadyc.getModo() != 0) {
        if (tiempo == 0){
          s.transTemp(sadyc);
        }
        else {
          if(modo == 1 || sadyc.getModo() == 3) {
            id_Transm_Temp = timer.setInterval(tiempo, Transm_Temp);
          }
        }
      }
    break;
    case 'd':                               //Descarga de datos desde la EEProm - ( ESC d CR )
      parada();
      descarga_E2();
      //rtc_write(rtc_m_st + 7, 0x02);        //Guarda en reloj código de descarga
      ini_var();
    break;
    case 'e':                               //Transmite datos del equipo - ( ESC e CR ) - Responde con los datos básicos del proyecto
      sadyc.datosEquipo();
    break;
    case 'h':                               //Transmite día y hora del RTC - ( ESC h CR ) - Responde con día y hora del equipo
      Serial.println(r.getFechaHora());
    break;
    case 'g':
      sadyc.saludo();
    break;
  }
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Transmite por RS232 los valores de todos los canales digitales y los canales analógicos leídos.

void Transm_Temp(void) {
  s.transTemp(sadyc);
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Testea cuantos chips de memoria EEProm hay instalados, escribiendo y lledendo en cada uno el dato ingresado por el usuario

void test_eeprom(byte data){
  unsigned long aux;
  byte i;

    aux = 32000;          
    for(i=1;i<=2;i++){
      e2prom_write(aux, data);
      aux = aux + 65536;
    }
    delay(10);
    aux = 32000;          
    for(i=1;i<=2;i++){
      data = e2prom_read(aux);
      Serial.print(F("Block "));
      Serial.print(i);
      Serial.print(F(" - Memoria "));
      if(aux<100000){Serial.print(" ");}
      Serial.print(aux);
      Serial.print(F(" --> "));
      if(data<100){Serial.print(" ");}
      if(data<10){Serial.print(" ");}
      Serial.println(data);
      aux = aux + 65536;
    }  
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que graba un datos en la EEProm y guarda el nuevo pumem en la ram del reloj
//Data --> dato a grabar - Se asume que "pumem" apunta al primer valor libre
void graba_dato_E2(byte data) {

  if(mem_full == 0){
      e2prom_write(pumem, data);
      pumem++;
      if(pumem >= MEM_FIN) {
        mem_full = 1;
        parada();
      }
      //rtc_write(rtc_m_st + 1, byte(pumem>>0));
      //rtc_write(rtc_m_st + 2, byte(pumem>>8));
      //rtc_write(rtc_m_st + 3, byte(pumem>>16));
      //rtc_write(rtc_m_st + 4, byte(pumem>>24));
  }
}

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que borra la EEProm e inicializa el puntero de registro "pumem" a cero
void borrado_E2(void) {
    parada();
    e2prom_erase();
    pumem = 0;
    //rtc_write(rtc_m_st + 1, byte(pumem>>0));
    //rtc_write(rtc_m_st + 2, byte(pumem>>8));
    //rtc_write(rtc_m_st + 3, byte(pumem>>16));
    //rtc_write(rtc_m_st + 4, byte(pumem>>24));
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que graba los datos de inicialización de registro en la EEProm
void graba_ini_E2(void) {
  int i;
  char buf[6];

  sprintf(buf,"%04d",tiempo_reg);
//  get_time();
  graba_dato_E2(0xff);
  graba_dato_E2(0xff);
  for(i=1;i<=10;i++){
    graba_dato_E2(auxbuffer[i]);
  }
  graba_dato_E2(0x32);
  graba_dato_E2(0x30);
  graba_dato_E2(auxbuffer[11]);
  graba_dato_E2(auxbuffer[12]);
  for(i=0;i<=3;i++){
    graba_dato_E2(buf[i]);
  }
  graba_dato_E2(modo);
  //graba_dato_E2(cantCan_analog1);
  //graba_dato_E2(cantCan_analog2);
  graba_dato_E2(0x08);
  //graba_dato_E2(rtc_read(rtc_m_st + 7));
  graba_dato_E2(0xff);
  graba_dato_E2(0xff);
}

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina de registro temporizado en EEProm (I2C) de los canales y el modo seteados
//Pumem apunta a la primera posición libre de la memoria
void Registro_Temp_E2(void) {
  int i;

  Serial.println(F("Rutina de registro"));

  //analog_foto();

  graba_dato_E2(leo_can_dig());                       //Garaba estado de los 8 canales digitales
/*
  if(modo == 2) {
    for(i=0;i<cantCan_analog1;i++){
      graba_dato_E2(byte(vect_aux1[i]));       //Graba valores de los canales analógicos seteados
      graba_dato_E2(byte(vect_aux1[i] >> 8));
    }
  }
  if(modo == 4) {
    for(i=0;i<cantCan_analog2;i++){
      graba_dato_E2(byte(vect_aux2[i]));       //Graba valores de los canales analógicos inAmp seteados
      graba_dato_E2(byte(vect_aux2[i] >> 8));
    }
  }*/
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------

void registro_temporizado(int tiempo_reg) {
    if(modo == 2 || modo == 4) {
    if(tiempo_reg>=1 && tiempo_reg<=3600) {
      parada();
      //rtc_write(rtc_m_st + 7, 0x01);                              //Guarda en reloj código de inicio
      //rtc_write(rtc_m_st + 9, byte(tiempo_reg>>0));               //Guarda en reloj tiempo de registro
      //rtc_write(rtc_m_st + 10, byte(tiempo_reg>>8));              //Guarda en reloj tiempo de registro
      reg_flag = 1;
      //rtc_write((rtc_m_st + 8), reg_flag);                        //Guarda en reloj flag de registro
      graba_ini_E2();                                             //Almacena en EEProm encabezado de inicio de registro
      id_Registro_Temp = timer.setInterval(tiempo_reg * 1000, Registro_Temp_E2);
    }          
  }
}
//--------------------------------------------------------------------------------------------------
//Rutina que lee los canales digitales de entrada y los unifica en un solo byte "digi_in"
byte leo_can_dig(void){
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

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que descarga los datos grabados en la EEProm y los trnsmite por RS232
void descarga_E2(void){
  unsigned long aux_p;

  Serial.write(byte(pumem>>24));
  Serial.write(byte(pumem>>16));
  Serial.write(byte(pumem>>8));
  Serial.write(byte(pumem>>0));

  for(aux_p=0;aux_p<pumem;aux_p++){
    Serial.write(e2prom_read(aux_p));
  }
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//                         Rutinas para memoria SD
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que agrega un dato en un archivo en la memoria SD
//archivo --> nombre del archivo en donde se desea guardar el dato.
//data --> dato a guardar

bool grabo_dato_sd(String archivo, byte data){
  String dataString = "";

  dataString += String(data);
  File dataFile = SD.open(archivo, FILE_WRITE);

  if (dataFile) {
    dataFile.print(dataString);
    dataFile.close();
    Serial.println(dataString);
    return true;
  }
  else {
    Serial.print("Error al abrir ");
    Serial.println(archivo);
    return false;
  }
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que lee un archivo de la memoria SD y lo manda por RS232
//archivo --> nombre del archivo en donde se desea guardar el dato.

bool leo_arch_sd(String archivo){
  
  File dataFile = SD.open(archivo, FILE_READ);

  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
    return true;

  } else {
    Serial.print(F("Error al abrir "));
    Serial.println(archivo);
    return false;
  }
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//                    Rutinas para memoria FLASH EEPROM (SPI) - S25FL127S
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que agrega un dato en un archivo en la memoria Flash
//archivo --> nombre del archivo en donde se desea guardar el dato. Si no existe lo crea.
//data --> dato a guardar

bool grabo_dato_flash(char *archivo, byte data){
  char buff[256];

  buff[0] = data;
  if (SerialFlash.exists(archivo) == false) {
        SerialFlash.create(archivo, 256);
  }

  SerialFlashFile S_File = SerialFlash.open(archivo);
  if (S_File) {
    S_File.write(buff, 1);
    S_File.close();
    Serial.println(buff);
    return true;
  } else {
    Serial.print(F("Error al abrir "));
    Serial.println(archivo);
    return false;
  }
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que lee un archivo de la memoria Flash y lo manda por RS232
//archivo --> nombre del archivo en donde se desea guardar el dato.
//cant_datos --> cantidad de datos a leer (0 --> todos)

bool leo_arch_flash(char *archivo){

  if (SerialFlash.exists(archivo)) {
    SerialFlashFile S_File = SerialFlash.open(archivo);
  
    if (S_File) {
      Serial.println(archivo);
      char buffer[256];
      S_File.read(buffer, 256);
      Serial.print(buffer);
      S_File.close();
      return true;
    } else {
      Serial.print(F("Error al abrir "));
      Serial.println(archivo);
      return false;
    }
  } else {
    Serial.print(archivo);
    Serial.println(F(" no existe"));
    return false;
  }    
}
 
//--------------------------------------------------------------------------------------------------
//Rutina que borra toda la Flash

void borro_flash(void) {

  SerialFlash.eraseAll();
  
  while (SerialFlash.ready() == false) {
     // Loop de espera hasta que se borre. Puede tardar de 30 seg. a 2 min.
  }
}
 
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//                    Rutinas para memoria EEPROM (I2C) - 24AA1025
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que escribe un dato en la E2
//e2addr --> dirección completa de puntero a banco de memoria en 4 bytes
//data --> datos a guardar

void e2prom_write(unsigned long e2_addr, byte data){
  byte memaddl, memaddh,i; 
  
  memaddl = (byte) (e2_addr & 0xff);
  memaddh = (byte) ((e2_addr >> 8) & 0xff);
  i = (byte) ((e2_addr >> 13) & 0x08);
  i = i | mem_ad;
  i = i >> 1;
  digitalWrite(WP1, LOW);
  Wire.beginTransmission(i);
  Wire.write(memaddh);                
  Wire.write(memaddl);                
  Wire.write(data);                
  Wire.endTransmission(true);
  delay(10);    
  digitalWrite(WP1, HIGH);
}  
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rutina que lee un dato de la E2
//e2addr --> dirección completa de puntero a banco de memoria en 4 bytes
//Devuelve byte leído

byte e2prom_read(unsigned long e2_addr){
  byte memaddl, memaddh, i, c; 
  
  memaddl = (byte) (e2_addr & 0xff);
  memaddh = (byte) ((e2_addr >> 8) & 0xff);
  i = (byte) ((e2_addr >> 13) & 0x08);
  i = i | mem_ad;
  i = i >> 1;
  Wire.beginTransmission(i);
  Wire.write(memaddh);                
  Wire.write(memaddl);                
  Wire.endTransmission(false);    
  Wire.requestFrom((int)i, 1, true); 
  c = Wire.read();
  return c;    
}  
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Borrado rápido de la memoria E2

void e2prom_erase(void){
  byte memaddl, memaddh, i, data, k; 
  unsigned long e2_addr;
  
  data = 0xff;
  e2_addr = 0;
  digitalWrite(WP, LOW);
  do{
    memaddl = (byte) (e2_addr & 0xff);
    memaddh = (byte) ((e2_addr >> 8) & 0xff);
    i = (byte) ((e2_addr >> 13) & 0x08);
    i = i | mem_ad;
    i = i >> 1;
    Wire.beginTransmission(i);
    Wire.write(memaddh);                
    Wire.write(memaddl);
    for(k=1;k<=64;k++){                
       Wire.write(data);
       e2_addr++;                
    }
    Wire.endTransmission(true);    
    delay(10);
  }while(e2_addr < MEM_FIN);
  digitalWrite(WP, HIGH);
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//                           Rutinas utilitarias
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rota "value" a la izquierda "shift" posiciones 
byte rotl_b(byte value, byte shift) {
    if ((shift < 1) || (shift > 7))
      return value;
    return (value << shift) | (value >> (8 - shift));
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Rota "value" a la derecha "shift" posiciones 
byte rotr_b(byte value, byte shift) {
    if ((shift < 1) || (shift > 7))
      return value;
    return (value >> shift) | (value << (8 - shift));
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

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//                             FIN DEL PROGRAMA
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------


