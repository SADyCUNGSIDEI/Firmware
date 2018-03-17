#include "Arduino.h"
#include "Globales.h"
#include "Nucleo.h"
#include "Comunicacion.h"
#include "Memorias.h"
#include "Adquisicion.h"

//Transmite por RS232 los valores de todos los canales digitales y los canales
//analógicos leídos. Primero copia el vector de canales analógicos
//a modo de congelamiento temporal de datos
void transmTemp(void) {
  byte i, aux;
  int vectAux1[8];
  int vectAux2[4];

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

void transmisionTemporizada(int tiempo, boolean wifi) {
  if (tiempo <= 10000 && modo != 0) {
    if (tiempo == 0) {
      if (wifi) {
        // transmTempWifi();
      }
      else {
        transmTemp();
      }
    }
    else {
      if (modo == 1 || modo == 3) {
        if (wifi) {
          // idTransmTempWifi = timer.setInterval(tiempo, transmTempWifi);
        }
        else {
          idTransmTemp = timer.setInterval(tiempo, transmTemp);  
        }
      }
    }
  }
}

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

void registroTemporizado(int tiempoReg) {
  if(modo == 2 || modo == 4) {
    if (tiempoReg >= 1 && tiempoReg <= 3600) {
      parada();
      setCodigoInicio(0x01);                         //Guarda en reloj código de inicio
      setTiempoReg(tiempoReg);                       //Guarda en reloj tiempo de registro
      setRegFlag(1);                                 //Guarda en reloj flag de registro
      // grabaIniE2();                                  //Almacena en EEProm encabezado de inicio de registro
      idRegistroTemp = timer.setInterval(tiempoReg * 1000, RegistroTempE2);
    }
  }
}