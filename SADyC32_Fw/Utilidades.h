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
