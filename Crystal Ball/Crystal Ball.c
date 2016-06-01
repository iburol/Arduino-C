/*
 * Crystal_Ball.c
 *
 * Created: 18/03/2015 18:45:56
 *  Author: Ismael Burgos
 */ 

//Inclusión librería LCD.
#include "lcd.h"

//Manejo de interrupciones
#include <avr/interrupt.h>
//Manejo de función Delay()
#include <util/delay.h>
//Manejo variables de tipo booleanas
#include <stdbool.h>


//Inicialización Puertos E/S
void PORT_Init (void);
//Inicialización Interrupción externa INT0
void INT0_Init (void);
//Inicialización Timer1
void Timer1_Init (void);

//Bandera de solicitud de atención a interrupción externa
bool FlagINT = false;
//Guarda resultado operación random()
int respuesta = 0;

void main (void)
{
	//Inicializa el LCD
	lcdBegin();
	PORT_Init();
	Timer1_Init();
	INT0_Init();
	
	//Cursor del LCD situado en la primera posición de la línea superior
	lcdSetCursor(1,0);
	//Se imprime la cadena...
	lcdPrint ("Pregunta a la...");
	//Cursor del LCD situado en la primera posición de la línea inferior
	lcdSetCursor(2,0);
	//Se imprime la cadena...
	lcdPrint ("Bola 8 Magica!");
	
	//Habilitación de interrupciones
	sei();

	while(1)
	{
		//Efecto rebote
		if (FlagINT == false)
		{
			//Habilitación de interrupción externa INT0
			EIMSK |= (1 << INT0);
		}
		//Interrupción externa INT0
		else
		{
			//Espera de 40 ms
			_delay_ms(40);
			//Terminación de atención interrupción externa INT0
			FlagINT = false;
		}
	}
}

void PORT_Init (void)
{
	//Pin 2 (PORTD2) como entrada - INT0
	PORTD |= (1 << PORTD2);
}

//Timer1, únicamente se activa para que comience la cuenta. 
//No nos interesa que haga nada más
void Timer1_Init (void)
{
	TCNT0 = 0;
	TCCR0B |= (1 << CS00);
}

void INT0_Init(void)
{
	//El flanco de subida en INT0 genera una petición de interrupción
	EICRA |= (1 << ISC01) | (1 << ISC00);
	//Habilitación interrupción externa INT0
	EIMSK |= (1 << INT0);
	//limpieza alguna posible petición de interrupción externa INT0
	EIFR |= (1 << INTF0);
}

//Función de interrupción externa INT0
ISR(INT0_vect)
{
	//Deshabilitación de interrupción externa INT0
	EIMSK &= ~(1 << INT0);
	//limpieza alguna posible petición de interrupción externa INT0
	EIFR |= (1 << INTF0);
	
	//Generamos un número aleatorio entre 0 y 7
	//utilizando como semilla de números el reloj del sistema 
	srand (TCNT0);
	respuesta = random() % 8;
	
	//Se limpia la pantalla del LCD
	lcdClear();
	//Cursor del LCD situado en la primera posición de la línea superior
	lcdSetCursor(1,0);
	
	//Según la variable respuesta se envía al LCD una respuesta u otra
	switch(respuesta)
	{
		case 0:
			lcdPrint ("Si");
			break;
		case 1:
			lcdPrint ("Probablemente");
			break;
		case 2:
			lcdPrint ("Cierto");
			break;
		case 3:
			lcdPrint ("Buenas");
			lcdSetCursor(2,0);
			lcdPrint ("perspectivas");
			break;
		case 4:
			lcdPrint ("Inseguro");
			break;
		case 5:
			lcdPrint ("Pregunta de");
			lcdSetCursor(2,0);
			lcdPrint ("nuevo");
			break;
		case 6:
			lcdPrint ("Dudoso");
			break;
		case 7:
			lcdPrint ("No");
			break;
		default:
			break;
	}
}