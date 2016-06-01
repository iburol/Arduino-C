/*
 * Crystal_Ball.c
 *
 * Created: 18/03/2015 18:45:56
 *  Author: Ismael Burgos
 */ 

//Inclusi�n librer�a LCD.
#include "lcd.h"

//Manejo de interrupciones
#include <avr/interrupt.h>
//Manejo de funci�n Delay()
#include <util/delay.h>
//Manejo variables de tipo booleanas
#include <stdbool.h>


//Inicializaci�n Puertos E/S
void PORT_Init (void);
//Inicializaci�n Interrupci�n externa INT0
void INT0_Init (void);
//Inicializaci�n Timer1
void Timer1_Init (void);

//Bandera de solicitud de atenci�n a interrupci�n externa
bool FlagINT = false;
//Guarda resultado operaci�n random()
int respuesta = 0;

void main (void)
{
	//Inicializa el LCD
	lcdBegin();
	PORT_Init();
	Timer1_Init();
	INT0_Init();
	
	//Cursor del LCD situado en la primera posici�n de la l�nea superior
	lcdSetCursor(1,0);
	//Se imprime la cadena...
	lcdPrint ("Pregunta a la...");
	//Cursor del LCD situado en la primera posici�n de la l�nea inferior
	lcdSetCursor(2,0);
	//Se imprime la cadena...
	lcdPrint ("Bola 8 Magica!");
	
	//Habilitaci�n de interrupciones
	sei();

	while(1)
	{
		//Efecto rebote
		if (FlagINT == false)
		{
			//Habilitaci�n de interrupci�n externa INT0
			EIMSK |= (1 << INT0);
		}
		//Interrupci�n externa INT0
		else
		{
			//Espera de 40 ms
			_delay_ms(40);
			//Terminaci�n de atenci�n interrupci�n externa INT0
			FlagINT = false;
		}
	}
}

void PORT_Init (void)
{
	//Pin 2 (PORTD2) como entrada - INT0
	PORTD |= (1 << PORTD2);
}

//Timer1, �nicamente se activa para que comience la cuenta. 
//No nos interesa que haga nada m�s
void Timer1_Init (void)
{
	TCNT0 = 0;
	TCCR0B |= (1 << CS00);
}

void INT0_Init(void)
{
	//El flanco de subida en INT0 genera una petici�n de interrupci�n
	EICRA |= (1 << ISC01) | (1 << ISC00);
	//Habilitaci�n interrupci�n externa INT0
	EIMSK |= (1 << INT0);
	//limpieza alguna posible petici�n de interrupci�n externa INT0
	EIFR |= (1 << INTF0);
}

//Funci�n de interrupci�n externa INT0
ISR(INT0_vect)
{
	//Deshabilitaci�n de interrupci�n externa INT0
	EIMSK &= ~(1 << INT0);
	//limpieza alguna posible petici�n de interrupci�n externa INT0
	EIFR |= (1 << INTF0);
	
	//Generamos un n�mero aleatorio entre 0 y 7
	//utilizando como semilla de n�meros el reloj del sistema 
	srand (TCNT0);
	respuesta = random() % 8;
	
	//Se limpia la pantalla del LCD
	lcdClear();
	//Cursor del LCD situado en la primera posici�n de la l�nea superior
	lcdSetCursor(1,0);
	
	//Seg�n la variable respuesta se env�a al LCD una respuesta u otra
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