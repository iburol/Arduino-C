/*
 * Serial_Communication_IV.c
 *
 * Created: 15/06/2015 11:17:51
 * Author: Ismael Burgos
 *
 * http://arduinoyansic.blogspot.com
 *
 */ 


#define F_CPU 16000000

//Definiciones espec�ficas del dispositivo AVR
#include <avr/io.h>
//Manejo de interrupciones
#include <avr/interrupt.h>

//Definici�n de la longitud del periodo de la onda cuadrada del PWM, 20 ms
#define PERIODO 19999
//Definici�n de los valores m�nimo y m�ximo de maniobra del servo.
//Seg�n el libro de Arduino estos valores est�n comprendidos entre 1 y 2 ms,
//pero si se quiere que el servo recorra los 180� estos deben de estar
//comprendidos entre 0,49 y 2,325 ms
#define NIV_MIN 980
#define NIV_MAX 4650
//Definici�n cada cuanto se incrementa en un grado por escal�n del ADC
#define INC_GRADO (NIV_MAX - NIV_MIN) / 180

#define BUAD 9600
#define BRC ((F_CPU/16/BUAD) - 1)

//Inicializaci�n TIMER1
void TIMER1_Init (void);
//Inicializaci�n Puertos E/S
void PORT_Init (void);
//Inicializaci�n USART
void USART_Init (void);

float Grados = 0;
//Variable que guarda el valor del Duty Cycle del PWM a enviar al servo
int Margen_Operacion = 0;

//Definici�n tama�o Buffer
#define TAM_BUFFER	3
//Definici�n Buffer
unsigned char Buffer[TAM_BUFFER];
uint8_t Pos_Buffer = 0;


void main (void)
{
	PORT_Init ();
	TIMER1_Init ();
	USART_Init();
	
	//Habilitaci�n de interrupciones
	sei();
	
	while(1)
	{

	}
}

void PORT_Init (void)
{
	//Pin 15 (PORTB1) como salida
	DDRB |= (1 << PORTB1);
}

void TIMER1_Init (void)
{
	//Modo PWM r�pido. TOP ICR1
	//Onda no invertida.
	//OC1A = 1 cuando TCNT0 = TOP
	//OCR1A = 0 cuando TCNT0 = OC1A/
	TCCR1A |= (1 << COM1A1) | (1 << WGM11);
	//Se habilita el TIMER1. Preescalado a 8
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11);
	//Inicializamos registros
	ICR1 = PERIODO;
	OCR1A = 0;
	TCNT1 = 0;
}

//Configuraci�n b�sica USART para recibir mensajes
void USART_Init (void)
{
	UBRR0H = (BRC >> 8);
	UBRR0L = BRC;
	UCSR0B = (1 << RXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

ISR(USART_RX_vect)
{
	//Lectura del byte entrante y almacenaje en la posici�n Pos_Buffer
	Buffer[Pos_Buffer] = UDR0;
	Pos_Buffer++;	
	if(Pos_Buffer >= TAM_BUFFER)
	{
		Pos_Buffer = 0;
		int centenas = Buffer[0];
		int decenas = Buffer[1];
		int unidades = Buffer[2];
		//Se le resta 48 para pasar el numero de ASCII a Decimal. El 0 en ASCII es 48.
		Grados = (centenas - 48)*100 + (decenas - 48)*10 + unidades - 48;
		//Margen de operacion igual al valorm�nimo de maniobra del servor m�s su incremento le�do por ADC
		Margen_Operacion = NIV_MIN + (Grados * INC_GRADO);
		//Porcentaje de Duty Cycle de la onda de periodo de 20 ms enviado al servo
		OCR1A = Margen_Operacion;
	}
}