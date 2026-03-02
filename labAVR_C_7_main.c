/*
 * LabAVR_C_7.c
 * Read data from accelerometer
 * Send accelerometer data over UART.
 * Generate a tone from accelerometer data.
 * WIP
 *
 * Created: 2026-03-02 15:58:13
 * Author : aleksander
 */

#include <avr/io.h>
#include <string.h>

#define F_CPU 1000000UL
#include <util/delay.h>

#include <avr/interrupt.h>

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

void send_char(uint8_t c)
{
	while(!( UCSR0A & (1<<UDRE0))); //Wait for empty transmit buffer
	UDR0 = c; //Put data into buffer, sends the data
}

/*
	Send value as human readable decimal number string
*/
void send_val_str(uint8_t val)
{
	send_char(val/100 + '0');
	val %= 100;
	send_char(val/10 + '0');
	val %= 10;
	send_char(val + '0');
	send_char('\n');
}

ISR(TIMER0_COMPA_vect)
{
	PORTB = PORTB ^ (1 << PINB0); //Flip summer output bit
}

int main(void)
{
	//init
	DDRB = (1 << PINB0); //Summer output on PINB0, rest if PORTB is input
	PORTB = (1 << BTN0) | (1 << BTN1) | (1 << BTN2) | (1 << BTN3); //Activate pullup resistors
	//init LED outputs
	DDRC = (1 << PINC0) | (1 << PINC1); //configure PORTC outputs
	DDRD = (1 << PIND2) | (1 << PIND3) | (1 << PIND4) | (1 << PIND5) | (1 << PIND6) | (1 << PIND7); //configure PORTD outputs
	//init UART
	UBRR0 = 12; //Baude rate //6 = 9600, 12 = 4800, 25 = 2400
	UCSR0B = (1 << TXEN0);
	//init ADC (Analog Digital Converter)
	ADMUX = (0 << REFS1) | (1 << REFS0) //Voltage Reference Selection: AV CC with external capacitor at AREF pin
		| (1 << ADLAR) //ADC Left Adjust Result; So I don't have to deal with multibyte readings. 8-bit precision is probably good enough for my purpose
		| 0x02; //Input Channel Selections; Potentiometer at ADC2
	ADCSRA = (1 << ADEN) //ADC Control and Status Register A: ADC Enable
		| (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // ADC Prescaler Select Bits: Division factor 128
	//init Timer
	TCCR0A = (1 << WGM01); //Timer/Counter Control Register A, Waveform Generator Mode: CTC (Clear timer on match)
	//TCCR0B = (1 << CS02) | (0 << CS01) | (1 << CS00); //Timer/Counter Control Register B clock selection bits: clk_io/1024
	TCCR0B = (1 << CS02) | (0 << CS01) | (0 << CS00); //clk_io/256
	TIMSK0 = (1 << OCIE0A); //Timer/Counter Interrupt Mask Register: Timer/Counter0 Output Compare Match A Interrupt Enable
	OCR0A = 255; //Clear Timer on Match value

	salute();
	sei();

	while (1)
	{
		ADCSRA |= (1 << ADSC); //ADC Start Conversion
		while(!(ADCSRA & (1 << ADIF))); //Loop until ADC conversion completes
		uint8_t adc_value = ADCH; //Read ADC high byte
		send_val_str(adc_value);
		OCR0A = adc_value; //Change summer frequency
		led_out(adc_value);
		_delay_ms(1000);

	}
}
