/*
 * Mood_Cue.c
 *
 * Created: 08/02/2015 15:19:12
 *  Author: Ismael Burgos
 */ 

#define F_CPU 16000000

//Definiciones específicas del dispositivo AVR
#include <avr/io.h>
//Manejo de interrupciones
#include <avr/interrupt.h>

//Definición de la longitud del periodo de la onda cuadrada del PWM, 20 ms
#define PERIODO 19999
//Definición de los valores mínimo y máximo de maniobra del servo.
//Según el libro de Arduino estos valores están comprendidos entre 1 y 2 ms, 
//pero si se quiere que el servo recorra los 180º estos deben de estar 
//comprendidos entre 0,49 y 2,325 ms
#define NIV_MIN 980
#define NIV_MAX 4650
//Definición cada cuanto se incrementa en un grado por escalón del ADC
#define INC_GRADO (NIV_MAX - NIV_MIN) / 180

//Inicialización ADC
void ADC_Init (void);
//Inicialización TIMER1
void TIMER1_Init (void);
//Inicialización Puertos E/S
void PORT_Init (void);
//Inicio conversión ADC
void startConversion (void);

////Varible escalado lectura ADC a Grados: 0... 1024 --> 0... 180
float Grados = 0;
//Variable que guarda el valor del Duty Cycle del PWM a enviar al servo
int Margen_Operacion = 0;

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
	//Pin 15 (PORTB1) como salida
	DDRB |= (1 << PORTB1);
}

void TIMER1_Init (void)
{
	//Modo PWM rápido. TOP ICR1
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

void ADC_Init(void)
{
	//Tensión de referencia a 5 V
	ADMUX |= (1 << REFS0);
	//Habilitación de interrupción por conversión del ADC completada
	//ADC preescalado a 128
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
	//Escalado lectura ADC a Grados: 0... 1024 --> 0... 180
	Grados = (ADC * 180.0) / 1024.0;
	//Margen de operacion igual al valormínimo de maniobra del servor más su incremento leído por ADC
	Margen_Operacion = NIV_MIN + (Grados * INC_GRADO);
	//Porcentaje de Duty Cycle de la onda de periodo de 20 ms enviado al servo
	OCR1A = Margen_Operacion;
	
	//Nueva conversión
	startConversion();
}