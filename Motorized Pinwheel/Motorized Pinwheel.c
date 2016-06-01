/*
 * Motorized_Pinwheel.c
 *
 * Created: 06/03/2015 19:05:10
 *  Author: Ismael Burgos
 */ 

#define F_CPU 16000000

//Definiciones específicas del dispositivo AVR
#include <avr/io.h>
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

//Bandera de solicitud de atención a interrupción externa
bool FlagINT0 = false;

void main(void)
{
	PORT_Init ();
	INT0_Init ();
	
	//Habilitación de interrupciones
	sei();
	
	while(1)
	{
		//No interrupción externa INT0
		if (FlagINT0 == false)
		{
			//Habilitación de interrupción externa INT0
			EIMSK |= (1 << INT0);
		}
		//Interrupción externa INT0
		else
		{
			//Deshabilitación de interrupción externa INT0
			EIMSK &= ~(1 << INT0);
			//limpieza alguna posible petición de interrupción externa INT0
			EIFR |= (1 << INTF0);
			//Espera de 40 ms
			_delay_ms(40);
			//Terminación de atención interrupción externa INT0
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
	cli ();
	//Inversión estado de la salida
	PORTB ^= (1 << PORTB1);
	//Atendida interrupción externa INT0
	FlagINT0 = true;
}