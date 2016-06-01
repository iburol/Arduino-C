/*
 * Spaceship_Interface.c
 *
 * Created: 27/01/2015 11:34:14
 * Author: Ismael Burgos
 */ 

#define F_CPU 16000000

//Definiciones espec�ficas del dispositivo AVR
#include <avr/io.h>
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
void Timer1_Init (void);

//Contador interrupci�n por desbordamiento del TIMER1
int cont = 0;
//Bandera de solicitud de atenci�n a interrupci�n externa
bool FlagINT0 = false;
	
void main(void)
{
	PORT_Init ();
	INT0_Init ();
	Timer1_Init ();
	
	//Habilitaci�n de interrupciones
	sei();
		
	while(1)
	{
		//No interrupci�n externa INT0
		if (FlagINT0 == false)
		{
			//Habilitaci�n de interrupciones
			sei();
		}
		//Interrupci�n externa INT0
		else
		{
			//Deshabilitaci�n de interrupciones
			cli();
			//limpieza alguna posible petici�n de interrupci�n externa INT0
			EIFR |= (1 << INTF0);
			//Espera de 40 ms
			_delay_ms(40);
			//Habilitaci�n de la interrupci�n por desbordamiento
			TIMSK1 = (1 << TOIE1);
			//Terminaci�n de atenci�n interrupci�n externa INT0
			FlagINT0 = false;
		}	
	}
}

void PORT_Init (void)
{
	//Pines 11 (PORTD5), 10 (PORTD4) y 9 (PORTD3) como salida
	DDRD |= (1 << PORTD5) | (1 << PORTD4) | (1 << PORTD3);
	//Puesta a 1 primer LED rojo
	PORTD |= (1 << PORTD3);
	//Pin 2 (PORTD2) como entrada - INT0
	PORTD |= (1 << PORTD2);
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

void Timer1_Init (void)
{
	//Modo CTC TIMER, OCR1A como TOP.
	//Preescalado a 256
	TCCR1B |= (1 << WGM12) | (1 << CS12);
	//31249 ticks * 16 us / ticks = 0,5 s
	OCR1A = 31249;
	//Habilitaci�n de la interrupci�n por desbordamiento
	TIMSK1 = (1 << TOIE1);
}

ISR(TIMER1_COMPA_vect)
{
	//Incremento del contador
	cont++;
	//Si primera vez que se entra en interrupci�n
	if (cont == 1)
	{
		//Puesta a 0 primer LED rojo
		PORTD &= ~(1 << PORTD5);
		//Puesta a 1 segundo LED rojo
		PORTD |= (1 << PORTD4);
	}
	//Si segunda vez que se entra en interrupci�n
	if (cont == 2)
	{
		//Puesta a 0 segundo LED rojo
		PORTD &= ~(1 << PORTD4);
		//Puesta a 1 LED verde
		PORTD |= (1 << PORTD3);
		//Deshabilitaci�n de la interrupci�n por desbordamiento
		TIMSK1 &= ~(1 << TOIE1);
		//Limpiador a cero
		cont = 0;
	}
}

//Funci�n de interrupci�n externa INT0
ISR(INT0_vect)
{
	//Puesta a 0 LED verde
	PORTD &= ~(1 << PORTD3);
	//Puesta a 1 primer LED rojo
	PORTD |= (1 << PORTD5);
	//Atendida interrupci�n externa INT0
	FlagINT0 = true;
}