// Voltmeter for the power supply module
// 10-Sep-2015

#define _XTAL_FREQ 8000000

#include <htc.h>
#include <stdio.h>

#include "led.h"


/* Config Bits for PIC18F4320 */
__CONFIG(1, RCIO ); //RC oscillator
__CONFIG(2, BORDIS & WDTDIS ); //disable Brown-out Reset  //disable  Watchdog Timer


typedef signed char int8_t; //stdint
typedef unsigned char uint8_t;
typedef signed int int16_t;
typedef unsigned int uint16_t;


#define TMR0_LOAD 64911 //preload Timer 0 for overflow ~200Hz //65536-64911=625 125kHz/625=200
#define VOLT_CONT 32 //voltage divider coeff.

char strBuff[4]; //string buffer
uint16_t volt=0; //voltage


///////////////////////////////////////////////////////////////////////////////////////////////////
void interrupt handler(void)
{
if (TMR0IF) //the timer 0 overflow interrupt
	{
	TMR0IF=0;
	TMR0=TMR0_LOAD;

	led_scan(); //dynamic lighting
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void delay_ms(uint16_t val)  //delay milliseconds  //correct for 8MHz speed
{
while(val--) __delay_ms(1);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void main(void)
{
IRCF2=1; IRCF1=1; IRCF0=1; //8MHz internal RC oscillator

ADCON0 = 0b000001; //Channel 0  //A/D converter module is enabled
ADCON1 = 0b001110; //Voltage Reference  //A/D Port Configuration Control bits - pin AN0
ADCON2 = 0b10000101; //A/D Result Format Select bit  //000=0TAD  //110=FOSC/64, 010=FOSC/32, 101=FOSC/16

T0CON=0b10010011; //prescaler 011 - 1:16  Fosc/4=2000, 2000/16=125kHz
TMR0=TMR0_LOAD; //preload
TMR0IE=1; //timer overflow intterrupt enable

PEIE=1; //peripheral interrupt enable
GIE=1; //global interrupts enable

led_init();

sbuff[0]=0xff; //light all segments
sbuff[1]=0xff;
sbuff[2]=0xff;

delay_ms(1400);

led_clear();

for(;;)
	{
	GODONE=1;
	while(GODONE);
	volt=ADRES*10/VOLT_CONT;
	if(volt>999) volt=999;
	
	sprintf(strBuff,"%2u%1u",volt/10,volt%10);
	led_print(0,strBuff);
	
	led_dot(1,1);
	
	delay_ms(200);
	}
}
