/*
 * Blink.c
 *
 * Created: 26/01/2015 19:58:42
 * Author: Ismael Burgos
 */ 

#define F_CPU 16000000UL

//Definiciones específicas del dispositivo AVR
#include <avr/io.h>
//Manejo de interrupciones
#include <avr/interrupt.h>

//Inicialización Puertos E/S
void PORT_Init (void);
//Inicialización TIMER1
void Timer1_Init (void);		

//Cuerpo principal
void main(void)
{
	PORT_Init ();
	Timer1_Init ();
	
	//Habilitación de interrupciones
	sei();
	
	while(1)
	{
		
	}
}

void PORT_Init (void)
{
	//Pin 13 (PORTB5) como salida
	DDRB |= (1 << PINB5);
}

void Timer1_Init (void)
{
	//Modo CTC TIMER, OCR1A como TOP.
	//Preescalado a 256
	TCCR1B |= (1 << WGM12) | (1 << CS12);
	//31249 ticks * 16 us / ticks = 0,5 s
	OCR1A = 31249;
	//Habilitación interrupcion por comparación, TCNT1 = OCR1A
	TIMSK1 = (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect)
{
	//Operación XOR. Se invierte el valor del pin
	PORTB ^= (1 << PINB5);
}