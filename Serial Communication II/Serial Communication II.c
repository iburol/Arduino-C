/*
 * Serial_Communication_II.c
 *
 * Created: 09/05/2015 15:06:01
 * Author: Ismael Burgos
 *
 * http://arduinoyansic.blogspot.com.es
 *
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>

#define BUAD 9600
#define BRC ((F_CPU/16/BUAD) - 1)

//Números de bytes a enviar
#define TX_Buffer 2

void ADC_Init (void);
void USART_Init (void);
void TIMER1_Init (void);

//Buffer de datos a enviar
uint8_t dato[TX_Buffer];
//Bytes enviados
int TX_i = 0;

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

//ADC con referencia de voltaje a 5 V
//ADC habilitado, disparo automático de la cnversión habilitada, interrupción por conversión
//completada habilitada y CLK/128
//Inicio de conversión por disparo por interrupción del TIMER1
void ADC_Init(void)
{
	ADMUX = (1 << REFS0);
	ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
	ADCSRB |= (1 << ADTS2) | (1 << ADTS0);
	DIDR0 |= (1 << ADC0D);
}

//Configuración básica USART
void USART_Init (void)
{
	UBRR0H = (BRC >> 8);
	UBRR0L = BRC;
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

//Timer1 a 0.01 s.
//100 muestras cada seg.
//Habilitada interrupcíón modo CTC
void TIMER1_Init (void)
{
	TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);
	OCR1A = 2499;
	TIMSK1 = (1 << OCIE1B);
}

//Byte alto del resultado de la conversión del ADC a la posición inicial del Buffer de datos
//Byte bajo a la siguiente posición
//Habilitación de interrupción por buffer de datos vacío del USART
ISR(ADC_vect)
{
	dato[0] = ADCL;
	dato[1] = ADCH;
	
	UCSR0B |= (1 << UDRIE0);
}

//Función de interrupcion del USART
//Si el contador de bytes enviados es inferior al tamaño del buffer de datos se sigue enviando bytes
//... en caso contrario, se limpia el contador de bytes enviados y se deshabilita la interrupción
//del USART
ISR(USART_UDRE_vect)
{
	if (TX_i < TX_Buffer)
	{
		UDR0 = dato[TX_i];
		TX_i++;
	}
	else
	{
		TX_i = 0;
		UCSR0B &= ~(1 << UDRIE0);
	}
}