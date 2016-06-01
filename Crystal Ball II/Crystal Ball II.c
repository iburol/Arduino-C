/*
 * Crystal_Ball_II.c
 *
 * Created: 21/03/2015 15:03:07
 *  Author: Ismael Burgos
 */ 

#include "lcd.h"

#define PUNTO 46

//Manejo de interrupciones
#include <avr/interrupt.h>

//Inicialización ADC
void ADC_Init (void);
//Inicio conversión ADC
void startConversion (void);
//Conversión Datos Tensión
void Conv_Tension (void);
//Conversión Datos Resistencia
void Conv_Resistencia (void);

float Tension = 0;
int Resistencia = 0;

int Medida_actual = 0;
int Medida_anterior = 0;

void main (void)
{
	//Inicializa el LCD
	lcdBegin();
	ADC_Init ();
	
	//Habilitación de interrupciones
	sei();
	
	lcdSetCursor (1,0);
	lcdPrint ("R( )=");
	lcdSetCursor (1,2);
	lcdWrite (0xF4);
	lcdSetCursor (2,0);
	lcdPrint ("T(V)=");
	
	while(1)
	{
		if (Medida_actual != Medida_anterior)
		{
			Medida_anterior = Medida_actual;
			
			Conv_Tension ();
			Conv_Resistencia ();
			
			_delay_ms(100);
		}
		
		//Nueva conversión
		startConversion();
	}
}

void ADC_Init(void)
{
	//Tensión de referencia a 5 V
	//ADC de 8 bits. Valor en ADCH.
	ADMUX |= (1 << REFS0) | (1 << ADLAR);
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
	Medida_actual = ADCH;
}

//Escalado del valor obtenido por el ADC a Tensión real
//Descomposición del valor según las unidades que lo constituyen
//Guardado en el buffer en orden: Unidad, punto, Décima, Centésima
//Envío al LCD
void Conv_Tension (void)
{
	Tension = (ADCH * 5.0) / 256.0;
	char valor_tension [4];
	int dato_tension = Tension * 100;
	valor_tension [0] = '0' + (dato_tension / 100);
	valor_tension [1] = PUNTO;
	int xdato_tension = dato_tension % 100;
	valor_tension [2] = '0' + (xdato_tension / 10);
	valor_tension [3] = '0' + (xdato_tension % 10);
	for(int i = 0; i < 4; i++)
	{
		lcdSetCursor (2,5+i);
		lcdWrite (valor_tension [i]);
	}
}

//Escalado del valor obtenido por el ADC a valor Óhmico real
//Descomposición del valor según las unidades que lo constituyen
//Guardado en el buffer en orden: Unidad de Millar, Centena, Decena, Unidad
//Envío al LCD
void Conv_Resistencia (void)
{
	Resistencia = (ADCH * 10000.0) / 256.0;
	char valor_resistencia [4];
	valor_resistencia [0] = '0' + (Resistencia / 1000);
	int dato_resistencia = Resistencia % 1000;
	valor_resistencia [1] = '0' + (dato_resistencia / 100);
	int xdato_resistencia = dato_resistencia % 100;
	valor_resistencia [2] = '0' + (xdato_resistencia / 10);
	valor_resistencia [3] = '0' + (xdato_resistencia % 10);
	for(int i = 0; i < 4; i++)
	{
		lcdSetCursor (1,5+i);
		lcdWrite (valor_resistencia [i]);
	}
}