/*
 * Digital_Hourglass.c
 *
 * Created: 01/03/2015 16:30:13
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
//Inicialización TIMER1
void Timer1_Init (void);
//Inicialización TIMER0
void Timer0_Init (void);

//Bandera de solicitud de atención a interrupción externa
bool FlagINT0 = false;
int ContSeg = 0;
int ContMin = 0;
uint16_t frec = 127;
uint8_t INICIO = 0b00000000;
uint8_t DIEZ_MIN = 0b00100000;
uint8_t FIN = 0b00111111;

void main(void)
{
	PORT_Init ();
	INT0_Init ();
	Timer1_Init ();
	Timer0_Init();
	
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
	//Pines 8 - 13 (PORTB0 - 5) como salida
	DDRB |= (1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB3) | (1 << PORTB4) | (1 << PORTB5);
	//Pin 5 (PORTD5) OC1B como salida
	DDRD |= (1 << PORTD5);
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

void Timer1_Init (void)
{
	//Modo CTC TIMER, OCR1A como TOP.
	//Preescalado a 256
	TCCR1B |= (1 << WGM12) | (1 << CS12);
	//62499 ticks * 16 us / ticks = 1 s
	OCR1A = 62499;
	//Habilitación de la interrupción por desbordamiento
	TIMSK1 |= (1 << OCIE1A);
	TCNT1 = 0;
}

void Timer0_Init (void)
{
	//Modo PWM rápido. TOP OCR1A
	//Onda no invertida.
	//OC0B = 1 cuando TCNT0 = BOTTON
	//OCR0B = 0 cuando TCNT0 = OCR0B
	TCCR0A |= (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
	//Se habilita el TIMER1. Preescalado a 8
	TCCR0B |= (1 << WGM02);
	//Inicios registros
	OCR0A = frec;
	OCR0B = OCR1A / 2;
	TCNT0 = 0;
}

ISR(TIMER1_COMPA_vect)
{
	ContSeg++;
	if (ContSeg == 60)
	{
		ContMin++;
		ContSeg = 0;
	}
	switch (ContMin)
	{
		case 0:
			PORTB = INICIO;
			break;
		case 1:
			PORTB = DIEZ_MIN;
			break;
		case 2:
			PORTB = DIEZ_MIN >> 1;
			break;
		case 3:
			PORTB = DIEZ_MIN >> 2;
			break;
		case 4:
			PORTB = DIEZ_MIN >> 3;
			break;
		case 5:
			PORTB = DIEZ_MIN >> 4;
			break;
		case 6:
			PORTB = FIN;
			//Habilitación del TIMER0
			TCCR0B |= (1 << CS02);
			//Deshabilitación de la interrupción por desbordamiento
			TIMSK1 &= ~(1 << OCIE1A);
			//Deshabilitación del TIMER1
			TCCR1B = 0b00001000;
			break;
		default:
			break;
	}
}

//Función de interrupción externa INT0
ISR(INT0_vect)
{
	PORTB = 0b00000000;
	if (ContMin == 6)
	{
		//Habilitación de la interrupción por desbordamiento
		TIMSK1 |= (1 << OCIE1A);
		//Habilitación del TIMER1
		TCCR1B = 0b00001100;
	}
	ContMin = 0;
	ContSeg = 0;
	TCNT0 = 0;
	TCNT1 = 0;
	//Deshabilitación del TIMER0
	TCCR0B = 0b00001000;
	//Atendida interrupción externa INT0
	FlagINT0 = true;
}