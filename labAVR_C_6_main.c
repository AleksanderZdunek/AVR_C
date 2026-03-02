/*
 * LabAVR_C_6.c
 * Read value from potentiometer via ADC.
 * Send to UART.
 *
 * Created: 2026-03-02 12:02:21
 * Author : aleksander
 */

#include <avr/io.h>
#include <string.h>

#define F_CPU 1000000UL
#include <util/delay.h>

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

void send_char(uint8_t c)
{
	while(!( UCSR0A & (1 << UDRE0))); //Wait for empty transmit buffer
	UDR0 = c; //Put data into buffer, sends the data
}

//Send value as human readable decimal number string
void send_val_str(uint8_t val)
{
	send_char(val/100 + '0');
	val %= 100;
	send_char(val/10 + '0');
	val %= 10;
	send_char(val + '0');
	send_char('\n');
}

int main(void)
{
	//init
	//init LED outputs
	DDRC = (1 << PINC0) | (1 << PINC1); //configure PORTC outputs
	DDRD = (1 << PIND2) | (1 << PIND3) | (1 << PIND4) | (1 << PIND5) | (1 << PIND6) | (1 << PIND7); //configure PORTD outputs
	//init UART
	UBRR0 = 6; //Baude rate //6 = 9600, 12 = 4800, 25 = 2400
	UCSR0B = (1 << TXEN0);
	//init ADC (Analog Digital Converter)
	ADMUX = (0 << REFS1) | (1 << REFS0) //Voltage Reference Selection: AV CC with external capacitor at AREF pin
		| (1 << ADLAR) //ADC Left Adjust Result; So I don't have to deal with multibyte readings. 8-bit precision is probably good enough for my purpose
		| 0x02; //Input Channel Selections; Potentiometer at ADC2
	ADCSRA = (1 << ADEN) //ADC Control and Status Register A: ADC Enable
		| (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // ADC Prescaler Select Bits: Division factor 128

	salute();

	while (1)
	{
		ADCSRA |= (1 << ADSC); //ADC Start Conversion
		while(!(ADCSRA & (1 << ADIF))); //Loop until ADC conversion completes
		uint8_t adc_value = ADCH; //Read ADC high byte
		send_val_str(adc_value);
		led_out(adc_value);
		_delay_ms(1000);
	}
}
