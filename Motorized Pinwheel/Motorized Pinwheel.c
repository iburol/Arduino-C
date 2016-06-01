/*
 * Motorized_Pinwheel.c
 *
 * Created: 06/03/2015 19:05:10
 *  Author: Ismael Burgos
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

//Bandera de solicitud de atenci�n a interrupci�n externa
bool FlagINT0 = false;

void main(void)
{
	PORT_Init ();
	INT0_Init ();
	
	//Habilitaci�n de interrupciones
	sei();
	
	while(1)
	{
		//No interrupci�n externa INT0
		if (FlagINT0 == false)
		{
			//Habilitaci�n de interrupci�n externa INT0
			EIMSK |= (1 << INT0);
		}
		//Interrupci�n externa INT0
		else
		{
			//Deshabilitaci�n de interrupci�n externa INT0
			EIMSK &= ~(1 << INT0);
			//limpieza alguna posible petici�n de interrupci�n externa INT0
			EIFR |= (1 << INTF0);
			//Espera de 40 ms
			_delay_ms(40);
			//Terminaci�n de atenci�n interrupci�n externa INT0
			FlagINT0 = false;
		}
	}
}

void PORT_Init (void)
{
	//Pin 9 (PORTB1) como salida
	DDRB |= (1 << PORTB1);
	//Motor iniciado como apagado
	PORTB &= ~(1 << PORTB1);
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

//Funci�n de interrupci�n externa INT0
ISR(INT0_vect)
{
	cli ();
	//Inversi�n estado de la salida
	PORTB ^= (1 << PORTB1);
	//Atendida interrupci�n externa INT0
	FlagINT0 = true;
}