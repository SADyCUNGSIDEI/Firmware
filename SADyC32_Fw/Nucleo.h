void datosEquipo(void);
void setModo(byte mode);
byte getModo(void);
void setCanAnalog1(byte cant);;
byte getCanAnalog1(void);
void setCanAnalog2(byte cant);
byte getCanAnalog2(void);
void setCodigoInicio(byte codigo);
byte getCodigoInicio(void);
void setRegFlag(byte regFlag);
byte getRegFlag(void);
void getWifiParams(void);
void setTiempoReg(unsigned int tiempoReg);
void saludo(void);
void setPumem(unsigned long pmem);
unsigned long getPumem(void);
void setPwm(byte pin, int dtyCicle);
void setDac(byte pin, int valor);
void setSalDig(byte pinDig, bool estado);
void setInAmp(byte in_inAmp, byte amplif);
byte leerCanalesDigitales(void);
void parada(void);
void iniVar(void);
void vaciarAuxBuffer(void);