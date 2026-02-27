/*
 * LabAVR_C_3.c
 * Move LED left or right with buttons
 *
 * Created: 2026-02-26 15:46:13
 * Author : aleksander
 */

#include <avr/io.h>

#define F_CPU 1000000UL
#include <util/delay.h>

#define BTN0 PINB1
#define BTN1 PINB2
#define BTN2 PINB6
#define BTN3 PINB7

void led_out(uint8_t x)
{
	PORTC = x;
	PORTD = x;
}

void salute()
{
	for(uint8_t i = 0x80; i; i >>= 1)
	{
		led_out(i);
		_delay_ms(50);
	}
	led_out(0);
}

int main(void)
{
	//init
	DDRB = 0x00; //Configure PORTB as input
	PORTB = (1 << BTN0) | (1 << BTN1) | (1 << BTN2) | (1 << BTN3); //Activate pullup resistors
	DDRC = DDRD = 0xff; //configure PORTC, PORTD as outputs
	salute();

	uint8_t x = 1 << 3;
	led_out(x); //Initially lights LED3

	while (1)
	{
		if(bit_is_clear(PINB, BTN1))
		{
			switch(x)
			{
				case 0x80:
					x = 0x01;
					break;
				default:
					x <<= 1;
					break;
			}
			led_out(x);
		} else if(bit_is_clear(PINB, BTN0))
		{
			switch(x)
			{
				case 0x01:
					x = 0x80;
					break;
				default:
					x >>= 1;
					break;
			}
			led_out(x);
		}
		_delay_ms(100);
	}
}
