/*
 * Love_O_Meter.c
 *
 * Created: 30/01/2015 16:48:37
 *  Author: Ismael Burgos
 */ 

#define F_CPU 16000000

//Definiciones específicas del dispositivo AVR
#include <avr/io.h>
//Manejo de interrupciones
#include <avr/interrupt.h>

//definición de la Temperatura base a 20 ºC
#define BaseLineTemp 20

//Inicialización ADC
void ADC_Init (void);
//Inicialización Puertos E/S
void PORT_Init (void);
//Inicio conversión ADC
void startConversion (void);

//Varible escalado lectura ADC a Voltios: 0... 1024 --> 0... 1,1
float voltage;
//Variable escalado Voltios a Temperatura 
float Temp;

void main(void)
{
	ADC_Init();
	PORT_Init();
	
	//Habilitación de interrupciones
	sei();
	
	while(1)
	{

	}
}

void PORT_Init (void)
{
	//Pines 2 (PORTD2), 3 (PORTD3) y 4 (PORTD4) como salida
	DDRD |= (1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4);	
}

void ADC_Init(void)
{
	ADMUX |= (1 << REFS1) | (1 << REFS0);
	ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
	
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
	//Escalado lectura ADC a Voltios: 0... 1024 --> 0... 1,1
	voltage = (ADC/1024.0) * 1.1;
	//Escalado Voltios a Temperatura 
	Temp = (voltage - 0.5) * 100;
	
	//Si Temp menor a 20 ºC
	if (Temp <= BaseLineTemp)
	{
		//Todos los LEDS OFF
		PORTD &= ~(1 << PORTD2);
		PORTD &= ~(1 << PORTD3);
		PORTD &= ~(1 << PORTD4);
	}
	//O si Temp entre 20 y 22 ºC
	else if (Temp > BaseLineTemp && Temp <= BaseLineTemp + 2)
	{
		//Primer LED ON
		PORTD |= (1 << PORTD2);
		PORTD &= ~(1 << PORTD3);
		PORTD &= ~(1 << PORTD4);
	}
	//O si Temp entre 22 y 24 ºC
	else if (Temp > BaseLineTemp + 2 && Temp <= BaseLineTemp + 4)
	{
		//Primero y segundo LEDs ON
		PORTD |= (1 << PORTD2);
		PORTD |= (1 << PORTD3);
		PORTD &= ~(1 << PORTD4);
	}
	//O si Temp mayor a 24 ºC
	else if (Temp > BaseLineTemp + 4)
	{
		//Todos los LEDS ON
		PORTD |= (1 << PORTD2);
		PORTD |= (1 << PORTD3);
		PORTD |= (1 << PORTD4);
	}
	
	//Nueva conversión
	startConversion();
}
