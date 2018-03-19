#include "Arduino.h"
#include "Globales.h"
#include "Nucleo.h"
#include "Comunicacion.h"
#include "Memorias.h"
#include "Adquisicion.h"
#include "Reloj.h"
#include "Wifi.h"
#include <DueTimer.h>              //Biblioteca para setear tareas controladas por timer

//Rutina que setea los canales elegidos en el ADC para que solo barra esos.
void setADCChan(void) {
  if((modo == 1) || (modo == 2)) {
    mask_Can1 = (0x00FF << (8-cantCanAnalog1)) & 0x00FF;
    REG_ADC_CHER = mask_Can1;                                 //Canales habilitados  
    REG_ADC_CHDR != mask_Can1;                                //Canales deshabilitados 
  }
  if((modo == 3) || (modo == 4)) {
    mask_Can2 = (0x3C00 >> (4-cantCanAnalog2)) & 0x3C00;
    REG_ADC_CHER = mask_Can2;                                 //Canales habilitados  
    REG_ADC_CHDR != mask_Can2;                                //Canales deshabilitados 
  }
}

//Rutina que lee el ADC en secuencia
//Cada vez que ejecuta lee todos los canales seteados
void adcSrv(void) {
  if((modo == 1) || (modo == 2)) {
    while((ADC->ADC_ISR & mask_Can1) != mask_Can1);           //Espera hasta terminar todas las conversiones. Sincronismo. 
    for(int i = 0; i < cantCanAnalog1; i++){
      vectCanales1[i] = ADC->ADC_CDR[7 - i];
    }
  }
  if((modo == 3) || (modo == 4)) {
    while((ADC->ADC_ISR & mask_Can2) != mask_Can2);           //Espera hasta terminar todas las conversiones. Sincronismo. 
    for(int i = 0; i < cantCanAnalog2; i++){
      vectCanales2[i] = ADC->ADC_CDR[10 + i];
    }
  }
}

void setTiempoMuestreoMin(void) {
  if(modo == 1 || modo == 2) {
    t_Muestro_Min = (unsigned long)(20000000 * cantCanAnalog1 / delta_B);
  }
  if(modo == 3 || modo == 4) {
    t_Muestro_Min = (unsigned long)(20000000 * cantCanAnalog2 / delta_B);
  }
}

//Transmite por RS232 los valores de todos los canales digitales y los canales
//analógicos leídos. 
void transmTemp(void) {
  byte aux;

  adcSrv();

  if ((modo == 1) || (modo == 2)) {
    for (byte i = 0; i < cantCanAnalog1; i++) {
      aux = byte(vectCanales1[i] >> 8);
      Serial.write(aux);
      Serial.write(vectCanales1[i]);
    }
  }
  if ((modo == 3) || (modo == 4)) {
    for (byte i = 0; i < cantCanAnalog2; i++) {
      aux = byte(vectCanales2[i] >> 8);
      Serial.write(aux);    
      Serial.write(vectCanales2[i]);
    }
  }
}

void transmisionTemporizada(unsigned long tiempo, boolean wifi) {
  if (tiempo <= 10000000 && modo != 0) {
    if (tiempo == 0) {
      if (wifi) {
        transmTempWifi();
      }
      else {
        transmTemp();
      }
    }
    else {
      setTiempoMuestreoMin();
      if ((modo == 1 || modo == 3) && tiempo >= t_Muestro_Min) {
        if (wifi) {
          Timer5.attachInterrupt(transmTempWifi);                      //Timer de transmisiones temporizadas
          Timer5.start(tiempo * 1000000);
        }
        else {
          Timer3.attachInterrupt(transmTemp);                      //Timer de transmisiones temporizadas
          Timer3.start(tiempo);
        }
      }
    }
  }
}

unsigned int getTiempoReg(void) {
  unsigned int tiempoReg = 0;
  tiempoReg = tiempoReg + (unsigned int)rtcRead(RTC_M_ST + 9);
  tiempoReg = tiempoReg + ((unsigned int)rtcRead(RTC_M_ST + 10) << 8);
  return tiempoReg;
}

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

void RegistroTempE2(void) {
  serialOutput("Rutina de registro", true);

  adcSrv();
  
  if (modo == 2) {
    for (byte i = 0; i < cantCanAnalog1; i++) {
      grabaDatoE2(byte(vectCanales1[i]));       //Graba valores de los canales analógicos inAmp seteados
      grabaDatoE2(byte(vectCanales1[i] >> 8));
    }
  }
  if (modo == 4) {
    for (byte i = 0; i < cantCanAnalog2; i++) {
      grabaDatoE2(byte(vectCanales2[i]));       //Graba valores de los canales analógicos inAmp seteados
      grabaDatoE2(byte(vectCanales2[i] >> 8));
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
      Timer4.attachInterrupt(RegistroTempE2);        //Timer para registro en E2
      Timer4.start(tiempoReg * 1000000);
    }
  }
}