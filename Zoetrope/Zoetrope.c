/*
 * Zoetrope.c
 *
 * Created: 10/03/2015 12:43:00
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
//Inicializaci�n Interrupci�n externa INT1
void INT1_Init (void);
//Inicializaci�n TIMER1
void TIMER1_Init (void);
//Inicializaci�n ADC
void ADC_Init (void);
//Inicio conversi�n ADC
void startConversion (void);

//Bandera de solicitud de atenci�n a interrupci�n externa
bool FlagINT = false;

//Contadores veces pulsadores pulsados
int cont_INT0 = 0;
int cont_INT1 = 0;

void main(void)
{
	PORT_Init ();
	TIMER1_Init();
	ADC_Init();
	INT0_Init ();
	INT1_Init ();
	
	//Habilitaci�n de interrupciones
	sei();
	
	while(1)
	{
		if (cont_INT0 == 1 || cont_INT1 == 1)
		{
			//Nueva conversi�n
			startConversion();
		}
		//No interrupci�n externa INT0
		if (FlagINT == false)
		{
			//Habilitaci�n de interrupci�n externa INT0
			EIMSK |= (1 << INT0);
			//Habilitaci�n de interrupci�n externa INT1
			EIMSK |= (1 << INT1);
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
	DDRD |= (1 << PORTD4) | (1 << PORTD5);
	//Pin 9 (PORTB1) como salida
	DDRB |= (1 << PORTB1);
	//Pin 2 (PORTD2) como entrada - INT0
	PORTD |= (1 << PORTD2);
}

void TIMER1_Init (void)
{
	//Modo PWM r�pido. TOP 0xFF
	//Onda no invertida.
	//OC1A = 1 cuando TCNT1 = ICR1
	//OC1A = 0 cuando TCNT1 = OCR1A
	TCCR1A |= (1 << COM1A1) | (1 << WGM11);
	TCCR1B |= (1 << WGM13) | (1 << WGM12);
	//Se habilita el TIMER1. Sin preescalar
	TCCR1B |= (1 << CS10);
	//(frec. de 25 khz) Periodo onda cuadrada del PWM
	ICR1 = 639;
}

void ADC_Init(void)
{
	//Tensi�n de referencia a 5 V
	ADMUX |= (1 << REFS0);
	//Habilitaci�n del ADC
	//Habilitaci�n de interrupci�n por conversi�n del ADC completada
	//ADC preescalado a 128
	ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
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

void INT1_Init (void)
{
	//El flanco de subida en INT1 genera una petici�n de interrupci�n
	EICRA |= (1 << ISC11) | (1 << ISC10);
	//Habilitaci�n interrupci�n externa INT1
	EIMSK |= (1 << INT1);
	//limpieza alguna posible petici�n de interrupci�n externa INT1
	EIFR |= (1 << INTF1);
}

//Funci�n de interrupci�n externa INT0
ISR(INT0_vect)
{
	//Deshabilitaci�n de interrupci�n externa INT0
	EIMSK &= ~(1 << INT0);
	//limpieza alguna posible petici�n de interrupci�n externa INT0
	EIFR |= (1 << INTF0);
	cont_INT0++;
	//Primera vez que se pulsa el pulsador inicia giro izq.
	if (cont_INT0 == 1)
	{
		if (cont_INT1 == 1)
		{
			cont_INT1 = 0;
		}
		PORTD &= ~(1 << PORTD4);
		PORTD |= (1 << PORTD5);
	}
	//Segunda vez que se pulsa el pulsador paro motor
	if (cont_INT0 == 2)
	{
		OCR1A = 0;
		PORTD &= ~(1 << PORTD4);
		PORTD &= ~(1 << PORTD5);
		cont_INT0 = 0;
		cont_INT1 = 0;
	}
	//Atendida interrupci�n externa INT0
	FlagINT = true;
}

//Funci�n de interrupci�n externa INT1
ISR(INT1_vect)
{
	//Deshabilitaci�n de interrupci�n externa INT0
	EIMSK &= ~(1 << INT1);
	//limpieza alguna posible petici�n de interrupci�n externa INT0
	EIFR |= (1 << INTF1);
	cont_INT1++;
	//Primera vez que se pulsa el pulsador inicia giro der.
	if (cont_INT1 == 1)
	{
		if (cont_INT0 == 1)
		{
			cont_INT0 = 0;
		}
		PORTD &= ~(1 << PORTD5);
		PORTD |= (1 << PORTD4);
	}
	//Segunda vez que se pulsa el pulsador paro motor
	if (cont_INT1 == 2)
	{
		OCR1A = 0;
		PORTD &= ~(1 << PORTD4);
		PORTD &= ~(1 << PORTD5);
		cont_INT1 = 0;
		cont_INT0 = 0;
	}
	//Atendida interrupci�n externa INT0
	FlagINT = true;
}

void startConversion (void)
{
	//Comenzar conversi�n
	ADCSRA |= (1 << ADSC);
}

ISR(ADC_vect)
{
	//Variaci�n Duty Cycle onda cuadrada del PWM por potenci�metro
	OCR1A = ADC;
}