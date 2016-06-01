/*
 * Keyboard_Instrument.c
 *
 * Created: 20/02/2015 9:11:26
 *  Author: Ismael Burgos
 */ 


#define F_CPU 16000000

//Definiciones específicas del dispositivo AVR
#include <avr/io.h>
//Manejo de interrupciones
#include <avr/interrupt.h>

//Inicialización ADC
void ADC_Init (void);
//Inicialización TIMER1
void TIMER1_Init (void);
//Inicialización Puertos E/S
void PORT_Init (void);
//Inicio conversión ADC
void startConversion (void);

uint16_t frecuencias [5] = {3817,3401,3030,2865,0};
uint16_t tono = 4;
uint16_t Muestreo = 0;


void main (void)
{
	PORT_Init ();
	TIMER1_Init ();
	ADC_Init ();
	
	//Habilitación de interrupciones
	sei();
	
	while(1)
	{

	}
}

void PORT_Init (void)
{
	//Pin 10 (PORTB2) OC1B como salida
	DDRB |= (1 << PORTB2);
}

void TIMER1_Init (void)
{
	//Modo PWM rápido. TOP OCR1A
	//Onda no invertida.
	//OC0B = 1 cuando TCNT0 = TOP
	//OCR0B = 0 cuando TCNT0 = OCR0B
	TCCR1A |= (1 << COM1B1) | (1 << WGM11) | (1 << WGM10);
	//Se habilita el TIMER1. Preescalado a 8
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11);
	//Inicios registros
	OCR1A = frecuencias [4];
	OCR1B = OCR1A / 2;
	TCNT1 = 0;
}

void ADC_Init(void)
{
	//Tensión de referencia a 5 V
	ADMUX |= (1 << REFS0);
	//Habilitación del ADC
	//Habilitación de interrupción por conversión del ADC completada
	//ADC preescalado a 128
	ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
	DIDR0 |= (1 << ADC0D);
	
	//Inicio conversión
	startConversion();
}

void startConversion (void)
{
	//Comenzar conversión
	ADCSRA |= (1 << ADSC);
}

ISR(ADC_vect)
{
	Muestreo = ADC;
		
	if (Muestreo == 1023)
	{
		tono = 0;
	}
	else if (Muestreo <= 1010 && Muestreo >= 990)
	{
		tono = 1;
	}
	else if (Muestreo >= 505 && Muestreo <= 515)
	{
		tono = 2;
	}
	else if (Muestreo >= 5 && Muestreo <= 10)
	{
		tono = 3;
	}
	else if (Muestreo == 0)
	{
		tono = 4;
	}
		
	OCR1A = frecuencias [tono];
	OCR1B = OCR1A / 2;
	
	//Nueva conversión
	startConversion();
}