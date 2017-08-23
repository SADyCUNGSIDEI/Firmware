04/05/2017 - Ver: 0.5
Se agregan las rutinas básicas de manejo de la SD
Se agrega una segunda rutina de recepción por RS232
para recibir el nombre del archivo para la SD

05/05/2017 - Ver: 0.6
Se emprolija secciones de rutinas

11/05/2017 - Ver: 0.7
Se emprolijaron nombres de variables para que empiecen con minúscula

26/05/2017 - Ver: 0.8 
Se agregan rutinas de la memoria EEprom-SPI: S25FL127S (NO ESTAN PROBADAS!!!!!)
Se agrega timer (Timer1) para registro y comando "R" para iniciar el registro de datos

29/05/2017 - Ver: 0.9
Se agrega la configuración para la comunicación RS485 sobre puerto serial 2 (TX2, RX2)
Se agrega un evento serial-2 para la recepción por RS485 

01/06/2017 - Ver: 1.0
Se agrega manejo de salidas digitales con el comando "S"

06/06/2017 - Ver: 1.1
Se agrega seteo de ganancia de los inAmps
Se agrega el manejo de "modo" y se dividen los canales analógicos
en 8 y 4. Tanto el ADC como las trnsmisiones se adecuaron a esto.

08/06/2017 - Ver: 1.2
Se emprolijan comentarios de comandos

09/06/2017 - Ver: 1.3
Se modifica la rutina Transm_Temp para que si el tiempo=0 transmita los estados de los canales
una sola vez

11/06/2017 - Ver: 1.4
Se agregan las rutinas de registro en EEProm, de acuerdo a los canales y modo seteados.
Se resguardan las variables importantes en la ram del reloj

12/06/2017 - Ver: 1.5
Se agrega rutina de descarga de datos de la EEProm por RS232

13/06/2017 - Ver: 1.6
Se cambia la rutina del ADC para barrido completo de canales por cada ejecución de la misma

14/06/2017 - Ver: 1.7
Se eliminan variables no usadas 

15/06/2017 - Ver: 1.7.1
y se elimina doble definicion de MEM_FIN

16/06/2017 - Ver: 1.7.2
Se agrega seteo de modo de pines de entradas digitales
Se corrige la rutina "Adc_Srv" para que tome bien los canales superiores (del 8 al 11)(inAmp)

20/06/2017 - Ver: 1.7.3
Se agregan comentarios y se pasan mensajes a flash memory


20/06/2017 - Ver: 1.7.3
Se agregan comentarios y se pasan mensajes a flash memory

25/06/2017 - Ver: 1.7.4
Se agrega rutina de inicialización y comunicación serial para Wifi ESP8266 

04/07/2017 - Ver: 1.7.5
Se emprolija inicialización de Wifi con vector de comandos

16/08/2017 - Ver: 1.7.6
Se crea la clase Salida

23/08/2017 - Ver: 1.7.7
Se crean las clases: SADyC32, Reloj y Medicion. 
Se agregan las transmisiones temporales a la clase Salida


