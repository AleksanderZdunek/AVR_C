/*
 * LabAVR_C_2.c
 * Blink LED2 while BTN2 is depressed.
 *
 * Created: 2026-02-26 14:38:38
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

	while (1)
	{
		if(bit_is_clear(PINB, BTN2))
		{
			led_out(0x04);
			_delay_ms(100);
			led_out(0x00);
			_delay_ms(100);
		}
		if(bit_is_clear(PINB, BTN0))
		{
			salute();
		}
	}
}
