/*
 * Light_Theremin.c
 *
 * Created: 14/02/2015 12:04:37
 *  Author: Ismael Burgos
 */ 


#define F_CPU 16000000

//Definiciones espec�ficas del dispositivo AVR
#include <avr/io.h>
//Manejo de interrupciones
#include <avr/interrupt.h>

//Definici�n de T_MAX con el n� de ticks de reloj m�ximo
#define T_MAX 39999
//Definici�n de T_MIN con el n� de ticks de reloj m�nimo
#define T_MIN 499
//Definici�n de VAR_T_CUENTA con el n� de ticks de reloj por escal�n del ADC
#define VAR_T_CUENTA (T_MAX - T_MIN) / 1024

//Inicializaci�n ADC
void ADC_Init (void);
//Inicializaci�n TIMER1
void TIMER1_Init (void);
//Inicializaci�n Puertos E/S
void PORT_Init (void);
//Inicio conversi�n ADC
void startConversion (void);

void main (void)
{
	PORT_Init ();
	TIMER1_Init ();
	ADC_Init ();
	
	//Habilitaci�n de interrupciones
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
	//Modo PWM r�pido. TOP OCR1A
	//Onda no invertida.
	//OC0B = 1 cuando TCNT0 = TOP
	//OCR0B = 0 cuando TCNT0 = OCR0B
	TCCR1A |= (1 << COM1B1) | (1 << WGM11) | (1 << WGM10);
	//Se habilita el TIMER1. Preescalado a 8
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11);
	//Inicios registros
	OCR1A = T_MAX;
	OCR1B = OCR1A / 2;
	TCNT1 = 0;
}

void ADC_Init(void)
{
	//Tensi�n de referencia a 5 V
	ADMUX |= (1 << REFS0);
	/Habilitaci�n delADC
	//Habilitaci�n de interrupci�n por conversi�n del ADC completada
	//ADC preescalado a 128
	ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
	DIDR0 |= (1 << ADC0D);
	
	//Inicio conversi�n
	startConversion();
}

void startConversion (void)
{
	//Comenzar conversi�n
	ADCSRA |= (1 << ADSC);
}

ISR(ADC_vect)
{
	//OCR1A igual T_MAX - (VAR_T_CUENTA * ADC)
	OCR1A = T_MAX - (VAR_T_CUENTA * ADC);
	//Longitud de periodo de onda cuadrada PWM por salida OCR1B igual a OCR1A / 2
	OCR1B = OCR1A / 2;
	
	//Nueva conversi�n
	startConversion();
}