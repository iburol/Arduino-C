/*
 * LCD.c
 *
 * Created: 15/03/2015 16:20:32
 *  Author: Ismael Burgos
 */ 

/***************************************************************************************************************
    This file is part of Library for AVR.
    Library for AVR is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Library for AVR is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Library for AVR.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************************************************/



/***
**   File       : lcd.h
**   Author     : Sriharsha
**   Website    : www.zuna.in
**   Email      : helpzuna@gmail.com
**   Description: This is include file for LCD library for AVR
***/
#ifndef __lcd_h__
#define __lcd_h__

#ifndef F_CPU
#define F_CPU 16000000UL           // Clock Frequency is 8 Mhz
#endif

#include <avr/io.h>               // Peripheral Address
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

// If you are using R/W Pin
//#define LCD_RW_ENABLED 1

// LCD Type (16x2,20x4)
#define _LCD_TYPE_ 2                   // 1 -> 20x2 , 2-> 16x2
#define LCD_MODE   2                   // 1-> 8-bit Mode,2-> 4-Bit Mode

// LCD Pin definitions
// RS of LCD Direction,OUTPUT and Bit Position
#define LCD_RS_PORT PORTC
#define LCD_RS_DIR  DDRC
#define LCD_RS_BIT  1 << 5

// EN of LCD Direction,OUTPUT and Bit Position
#define LCD_EN_PORT PORTC
#define LCD_EN_DIR  DDRC
#define LCD_EN_BIT  1 << 4

#ifdef LCD_RW_ENABLED
// RW of LCD Direction,OUTPUT and Bit Position
#define LCD_RW_PORT PORTB
#define LCD_RW_DIR  DDRB
#define LCD_RW_BIT  1 << 6
#endif

// If 8-bit mode
#if LCD_MODE == 1
// D0 of LCD Direction,OUTPUT and Bit Position
#define LCD_D0_PORT PORTA
#define LCD_D0_DIR  DDRA
#define LCD_D0_BIT  1 << 0

// D1 of LCD Direction,OUTPUT and Bit Position
#define LCD_D1_PORT PORTA
#define LCD_D1_DIR  DDRA
#define LCD_D1_BIT  1 << 1

// D2 of LCD Direction,OUTPUT and Bit Position
#define LCD_D2_PORT PORTA
#define LCD_D2_DIR  DDRA
#define LCD_D2_BIT  1 << 2

// D3 of LCD Direction,OUTPUT and Bit Position
#define LCD_D3_PORT PORTA
#define LCD_D3_DIR  DDRA
#define LCD_D3_BIT  1 << 3
#endif

// Default Mode 4 - Bit (It also includes 8-bit pin definitions)
// D4 of LCD Direction,OUTPUT and Bit Position
#define LCD_D4_PORT PORTD
#define LCD_D4_DIR  DDRD
#define LCD_D4_BIT  1 << 3

// D5 of LCD Direction,OUTPUT and Bit Position
#define LCD_D5_PORT PORTD
#define LCD_D5_DIR  DDRD
#define LCD_D5_BIT  1 << 4

// D6 of LCD Direction,OUTPUT and Bit Position
#define LCD_D6_PORT PORTD
#define LCD_D6_DIR  DDRD
#define LCD_D6_BIT  1 << 5

// D7 of LCD Direction,OUTPUT and Bit Position
#define LCD_D7_PORT PORTD
#define LCD_D7_DIR  DDRD
#define LCD_D7_BIT  1 << 6

// RS(Register Select) LOW and HIGH macros
#define LCD_COMMAND_MODE_RS  LCD_RS_PORT &= ~LCD_RS_BIT
#define LCD_DATA_MODE_RS     LCD_RS_PORT |=  LCD_RS_BIT

// Enable LOW and HIGH macros
#define LCD_ENABLE_LOW       LCD_EN_PORT &= ~LCD_EN_BIT
#define LCD_ENABLE_HIGH      LCD_EN_PORT |=  LCD_EN_BIT

// LCD standard commands
#define LCD_4BIT_1_LINE_5_x_7            0x20
#define LCD_4BIT_2_LINE_5_x_7            0x28
#define LCD_8BIT_1_LINE_5_x_7            0x30
#define LCD_8BIT_2_LINE_5_x_7            0x38
#define LCD_ENTRY_MODE                   0x06
#define LCD_DISPLAY_OFF_CURSOR_OFF       0x08
#define LCD_DISPLAY_ON_CURSOR_ON         0x0E
#define LCD_DISPLAY_ON_CURSOR_OFF        0x0C
#define LCD_DISPLAY_ON_CURSOR_BLINKING   0x0F
#define LCD_DISPLAY_SHIFT_ENTIRE_LEFT    0x18
#define LCD_DISPLAY_SHIFT_ENTIRE_RIGHT   0x1C
#define LCD_MOVE_CURSOR_LEFT_ONE_CHAR    0x10
#define LCD_MOVE_CURSOR_RIGHT_ONE_CHAR   0x14
#define LCD_CLEAR_ALSO_DDRAM             0x01

// LCD Row Addresses
#define LCD_ADDRESS_ROW1 0x80
#define LCD_ADDRESS_ROW2 0xC0
#define LCD_ADDRESS_ROW3 0x94
#define LCD_ADDRESS_ROW4 0xD4

// Delay macros
#define __delay_ms__(x)  _delay_ms((long) x/2)
#define __delay_us__(x)  _delay_us((long) x)

// Prototype Declaration
extern void lcdWrite(unsigned char);
extern void lcdPrint(unsigned char *);
extern void lcdBegin(void);
extern void lcdSetCursor(int,int);
extern void lcdClear(void);
extern void lcdWriteint(signed int);

extern void lcd_command(unsigned char);
extern void lcd_data(unsigned char);
extern void lcdEnable(void);
extern void lcd_port_init(void);
extern void lcdwrite4Bits(unsigned char);
extern void lcdwrite8Bits(unsigned char);
extern void lcdSendbyte(unsigned char);
extern void lcdWriteintU(unsigned int);

#endif