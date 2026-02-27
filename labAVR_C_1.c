/*
 * LabAVR_C_1.c
 * Light up LEDs corresponding to button presses
 *
 * Created: 2026-02-23 10:46:55
 * Author : aleksander
 */

#include <avr/io.h>

#define BTN0 PINB1
#define BTN1 PINB2
#define BTN2 PINB6
#define BTN3 PINB7

void led_out(uint8_t x)
{
	PORTC = x;
	PORTD = x;
}

int main(void)
{
	//init
	DDRB = 0x00; //Configure PORTB as input
	PORTB = (1 << BTN0) | (1 << BTN1) | (1 << BTN2) | (1 << BTN3); //Activate pullup resistors
	DDRC = DDRD = 0xff; //configure PORTC, PORTD as outputs

	while (1)
    {
		uint8_t x =
			(bit_is_clear(PINB, BTN3) << 3) |
			(bit_is_clear(PINB, BTN2) << 2) |
			(bit_is_clear(PINB, BTN1) << 1) |
			(bit_is_clear(PINB, BTN0) << 0) ;
		led_out(x);
    }
}

