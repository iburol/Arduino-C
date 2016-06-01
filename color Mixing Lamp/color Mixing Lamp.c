/*
 * color_Mixing_Lamp.c
 *
 * Created: 03/02/2015 12:59:17
 *  Author: Ismael Burgos
 */ 

#define F_CPU 16000000

//Definiciones espec�ficas del dispositivo AVR
#include <avr/io.h>
//Manejo de interrupciones
#include <avr/interrupt.h>

//Inicializaci�n ADC
void ADC_Init (void);
//Inicializaci�n TIMER1
void TIMER1_Init (void);
//Inicializaci�n TIMER0
void TIMER0_Init (void);
//Inicializaci�n Puertos E/S
void PORT_Init (void);
//Inicio conversi�n ADC
void startConversion (void);

//Variable selector de canal del ADC
int sel_canal = 0;

void main(void)
{
	ADC_Init();
	TIMER1_Init();
	TIMER0_Init();
	PORT_Init();
	
	//Habilitaci�n de interrupciones
	sei();
	
	while(1)
	{
		
	}
}

void PORT_Init (void)
{
	//Pines 6 (PORTD6) OC0A, 9 (PORTB1) OC1A y 10 (PORTB2) OC1B como salida
	DDRD |= (1 << PORTD6);
	DDRB |= (1 << PORTB1) | (1 << PORTB2);
}

void ADC_Init(void)
{
	//Tensi�n de referencia a 5 V
	ADMUX |= (1 << REFS0);
	//Habilitaci�n delADC
	//Habilitaci�n de interrupci�n por conversi�n del ADC completada
	//ADC preescalado a 128
	ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
	
	//Inicio conversi�n
	startConversion();
}

void TIMER0_Init (void)
{
	//Modo PWM r�pido. TOP 0xFF
	//Onda no invertida. 
	//OC0A = 1 cuando TCNT0 = TOP
	//OCR0A = 0 cuando TCNT0 = OCR0A
	TCCR0A |= (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
	//Se habilita el TIMER0. Sin preescalar
	TCCR0B |= (1 << CS00);
	OCR0A = 0;
}

void TIMER1_Init (void)
{
	//Modo PWM r�pido. TOP 0xFF
	//Onda no invertida.
	//OC1A/OC1B = 1 cuando TCNT1 = TOP
	//OCR1A/OCR1B = 0 cuando TCNT1 = OCR1A/OCR1B
	TCCR1A |= (1 << COM1A1)|(1 << COM1B1) | (1 << WGM10) | (1 << WGM10);
	//Se habilita el TIMER1. Sin preescalar
	TCCR1B |= (1 << CS10);
	OCR1A = 0;
	OCR1B = 0;
}

void startConversion (void)
{
	//Comenzar conversi�n
	ADCSRA |= (1 << ADSC);
}

ISR(ADC_vect)
{
	switch (sel_canal)
	{
		//Canal de conversi�n ADC0
		case 0:
			//Longitud de periodo de onda cuadrada PWM por salida OCR1A igual a dato del ADC0
			OCR1A = ADC;
			//Incremento sel_canal. Siguiente canal ADC1
			sel_canal++;
			//Limpieza bits selecci�n de canal registro ADMUX
			ADMUX &= 0xC8;
			//Configuraci�n siguiente canal. ADC1
			ADMUX |= (1 << REFS0) | (1 << MUX0);
			break;
		//Canal de conversi�n ADC1
		case 1:
			//Longitud de periodo de onda cuadrada PWMpor salida OCR1B igual a dato del ADC1
			OCR1B = ADC;
			//Incremento sel_canal. Siguiente canal ADC2
			sel_canal++;
			//Limpieza bits selecci�n de canal registro ADMUX
			ADMUX &= 0xC8;
			//Configuraci�n siguiente canal. ADC2
			ADMUX |= (1 << REFS0) | (1 << MUX1);
			break;
		//Canal de conversi�n ADC2
		case 2:
			//Longitud de periodo de onda cuadrada PWM por salida OC01A igual a dato del ADC2
			OCR0A = ADC;
			//Puesta a 0 sel_canal. Siguiente canal ADC0
			sel_canal = 0;
			//Limpieza bits selecci�n de canal registro ADMUX
			ADMUX &= 0xC8;
			//Configuraci�n siguiente canal. ADC0
			ADMUX |= (1 << REFS0);
			break;
		default:
			break;
	}
	
	//Nueva conversi�n
	startConversion();
}