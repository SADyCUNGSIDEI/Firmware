void eeWrite(unsigned long e2_addr, byte data);
byte eeRead(unsigned long eeAddr);
void eeErase(void);
void eeTest(byte data);

void grabaDatoE2(byte data);
void descargaE2(void);
void borrarE2(void);

bool grabarDatoSD(String archivo, byte data);
bool leerArchSD(String archivo);

bool grabarDatoFlash(char *archivo, byte data);
bool leerArchFlash(char *archivo);
void borrarFlash(void);
