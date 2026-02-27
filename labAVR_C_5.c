/*
 * LabAVR_C_5.c
 * Receive a number string on serial port.
 * Show the number in binary form through LEDs.
 * WIP
 *
 * Created: 2026-02-26 16:52:25
 * Author : aleksander
 */

#include <avr/io.h>

#define F_CPU 1000000UL
#include <util/delay.h>

//#include <util/setbaud.h> //TODO

#define BTN0 PINB1
#define BTN1 PINB2
#define BTN2 PINB6
#define BTN3 PINB7

/*volatile uint8_t outstate = 0;*/
void led_out(uint8_t x)
{
	PORTC = x;
	PORTD = x;
//	outstate = x;
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

void uart_get_char()
{
	//loop_until_bit_is_set(UCSR0A, RXC0);
	//return UDR0;
	if(bit_is_set(UCSR0A, RXC0))
	{
		led_out(UDR0);
	}
	// 	else if (0x00== outstate)
	// 	{
	// 		led_out(0xff);
	// 	}
	// 	else if (0xff == outstate)
	// 	{
	// 		led_out(0x00);
	// 	}
}

int main(void)
{
	//init
	DDRB = 0x00; //Configure PORTB as input
	PORTB = (1 << BTN0) | (1 << BTN1) | (1 << BTN2) | (1 << BTN3); //Activate pullup resistors
	DDRC = DDRD = 0xff; //configure PORTC, PORTD as outputs
	//init UART
	UBRR0H = 0;
	UBRR0L = 6; //Baude rate //6 = 9600, 12 = 4800, 25 = 2400
	//UBRR0 = 6; //Baude rate //6 = 9600, 12 = 4800, 25 = 2400
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	salute();
	//sei(); //enable interrupts

	while (1)
	{
		uart_get_char();
		//led_out(uart_get_char());
		//TODO: clean up
		if(
			bit_is_clear(PINB, BTN0) ||
			bit_is_clear(PINB, BTN1) ||
			bit_is_clear(PINB, BTN2) ||
			bit_is_clear(PINB, BTN3)
		) {
			salute();
		}
		_delay_ms(100);
	}
}
