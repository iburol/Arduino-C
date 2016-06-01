/*
 * Serial_Communication_III.c
 *
 * Created: 17/05/2015 12:07:58
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

//Tama�o del Buffer definido
#define TX_Buffer	64

void USART_Init (void);
//Intro cadena a Buffer
void AnexarBuffer(char caracter);
//Lectura de la cadena de caracteres
void LecturaCadena(char caracter[]);

//Buffer de datos a enviar
char dato[TX_Buffer];
//Contador de posici�n en el Buffer
uint8_t PosTX_i = 0;
//Contador Byte enviado
uint8_t TX_i = 0;
//Tama�o del Buffer a enviar
uint8_t TX_Max = 0;


void main (void)
{
	USART_Init();
	
	sei();
	
	LecturaCadena("�Hola Mundo!\n");
	
	LecturaCadena("Bienvenidos a Arduino y ANSI C\n");
	
	UCSR0B |= (1 << UDRIE0);
	
	while(1)
	{
		
	}
}

void USART_Init (void)
{
	UBRR0H = (BRC >> 8);
	UBRR0L = BRC;
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

///Funci�n encargada de la lectura de la cadena de caracteres
void LecturaCadena(char caracter[])
{
	for(uint8_t cont = 0; cont < strlen(caracter); cont++)
	{
		AnexarBuffer(caracter[cont]);
	}
}

//Funci�n en cargada de introducir la cadena le�da en el Buffer de datos
void AnexarBuffer(char caracter)
{
	dato[PosTX_i] = caracter;
	PosTX_i++;
	TX_Max++;
	
	if(PosTX_i >= TX_Buffer)
	{
		PosTX_i = 0;
	}
}

//Se env�a el dato 
//Si el Byte enviado es menor que el tama�o m�ximo del Buffer se env�a el siguiente caracter
//... en caso contrario se limpian ambos
//Deshabilitaci�n de Interrupci�n de Registro de Datos Vac�o del USART
ISR(USART_UDRE_vect)
{
	if (TX_i < TX_Max)
	{
		UDR0 = dato[TX_i];
		TX_i++;
	}
	else
	{
		TX_i = 0;
		TX_Max = 0;
		UCSR0B &= ~(1 << UDRIE0);
	}
}