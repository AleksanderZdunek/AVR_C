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
#include <string.h>

#define F_CPU 1000000UL
#include <util/delay.h>

#include <avr/interrupt.h>

//#include <util/setbaud.h> //TODO

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

//a fancy way of zeroing output LEDs
void clear_out()
{
	while(0x00 < PORTC)
	{
		led_out(PORTC >> 1);
		_delay_ms(50);
	}
}

//Input digit buffer
uint8_t buf[3] = {0};

//UART get char
ISR(USART_RX_vect)
{
	if(bit_is_set(UCSR0A, RXC0))
	{
		const uint8_t digit = UDR0;
		if(13 == digit)
		{
			led_out(buf[0] + 10*buf[1] + 100*buf[2]);
			memset(buf, 0, sizeof(buf));
		}
		else
		{
			//FIFO
			buf[2] = buf[1];
			buf[1] = buf[0];
			buf[0] = digit & 0x0f; //Only care about the lower 4 bits
		}
	}
}


ISR(PCINT0_vect)
{
	//I don't understand how to hold Pin Change interrupts correctly

	//PCIFR Pin Change Interrupt Flag Register
	//if(bit_is_set(PCIFR, PCIF0)) //For future reference
	//{
		//led_out(0xff);
	//}
	led_out(0xff);
}

int main(void)
{
	//init
	DDRB = 0x00; //Configure PORTB as input
	PORTB = (1 << BTN0) | (1 << BTN1) | (1 << BTN2) | (1 << BTN3); //Activate pullup resistors
	DDRC = DDRD = 0xff; //configure PORTC, PORTD as outputs
	PCICR = (1 << PCIE0); //PCICR Pin Change Interrupt Control Register - Enable Pin Change Interrupt 0
	PCMSK0 = (1 << PCINT7); //PCMSK Pin Change Mask Register 0 - Enable interrupt on PINB7
	//init UART
	UBRR0 = 6; //Baude rate //6 = 9600, 12 = 4800, 25 = 2400
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	salute();
	sei(); //enable interrupts

	while (1)
	{
		//Reset input buffer
		if(bit_is_clear(PINB, BTN0)) {
			clear_out();
			memset(buf, 0, sizeof(buf));
		}
		_delay_ms(100);
	}
}
