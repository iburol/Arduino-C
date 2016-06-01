/*
 * Serial_Communication.c
 *
 * Created: 26/04/2015 18:28:03
 *  Author: ASUS
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>

#define BUAD 9600
#define BRC ((F_CPU/16/BUAD) - 1)

void ADC_Init (void);
void USART_Init (void);
void TIMER1_Init (void);

uint8_t dato;

void main (void)
{
	USART_Init();
	ADC_Init();
	TIMER1_Init();
	
	sei();
	
	while(1)
	{
	
	}
}

//ADC configurado con disparo de inicio de conversión por TIMER1 modo compración
void ADC_Init(void)
{
	ADMUX = (1 << REFS0) | (1 << ADLAR);
	ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
	ADCSRB |= (1 << ADTS2) | (1 << ADTS0);
	DIDR0 |= (1 << ADC0D);
}

void USART_Init (void)
{
	UBRR0H = (BRC >> 8);
	UBRR0L = BRC;
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

//Timer1 a 0.01 s.
//100 muestras cada seg.
void TIMER1_Init (void)
{
	TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);
	OCR1A = 2499;
	TIMSK1 = (1 << OCIE1B);
}
 
//Se guarda el valor de la conversión en dato
//Habilitación de Interrupción de Registro de Datos Vacío del USART 
ISR(ADC_vect)
{
	dato = ADCH;	
	UCSR0B |= (1 << UDRIE0);
}

//Se envía el dato
//Deshabilitación de Interrupción de Registro de Datos Vacío del USART
ISR(USART_UDRE_vect)
{
	 UDR0 = dato;
	 UCSR0B &= ~(1 << UDRIE0);
}