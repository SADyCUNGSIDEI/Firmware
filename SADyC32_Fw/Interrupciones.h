/*
	Trigger de evento PIN1
*/
void trigPin1(void);

/*
	Trigger de evento PIN2
*/
void trigPin2(void);

/*
	Trigger de activación de evento en el mismo PIN
*/
void trigSamePin(void);

/*
	Setea la medición de eventos en los dos pines indicados
	En el caso de que pin1 y pin2 sean los mismos activa el trigger 'trigSamePin'

		byte pin1:	Pin de medición 1
		byte pin2: 	Pin de medición 2
*/
void setMedicion(byte pin1, byte pin2);

/*
	Función que se ejecuta en el loop principal. Envía el tiempo que ocurrió entre eventos
*/
void checkMedicion(void);