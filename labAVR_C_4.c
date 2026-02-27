/*
 * LabAVR_C_4.c
 * Count number of depressed buttons
 *
 * Created: 2026-02-26 16:21:23
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
		led_out(
			bit_is_clear(PINB, BTN0) +
			bit_is_clear(PINB, BTN1) +
			bit_is_clear(PINB, BTN2) +
			bit_is_clear(PINB, BTN3)
		);
		_delay_ms(100);
	}
}
