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

#define Xacc 0x04 //Accelerometer X-axis on ADC4 (PINC4 / pin 27)
#define Yacc 0x05 //Accelerometer Y-axis on ADC5 (PINC5 / pin 28)
#define ACCELEROMETER_BASELINE 500 //Empirically determined baseline

uint16_t abs16(int16_t x)
{
	if(x < 0) return -x;
	return x;
}

void led_out(uint8_t x)
{
	PORTC = x & ( (1 << PINC0) | (1 << PINC1) );
	PORTD = x & ( (1 << PIND2) | (1 << PIND3) | (1 << PIND4) | (1 << PIND5) | (1 << PIND6) | (1 << PIND7) );
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
void send_val_str(uint16_t val)
{
	send_char(val/100 + '0');
	val %= 100;
	send_char(val/10 + '0');
	val %= 10;
	send_char(val + '0');
	send_char('\n');
}

uint8_t buzzer_on = 0;
ISR(TIMER0_COMPA_vect)
{
	if(buzzer_on) {
		PORTB = PORTB ^ (1 << PINB0); //Flip buzzer output bit
	}
}

//Read Analog-to-Digital converter
uint16_t read_adc(uint8_t analog_channel)
{
	ADMUX = (ADMUX & 0xf0) | (analog_channel & 0x0f); //Input Channel Selection
	ADCSRA |= (1 << ADSC); //ADC Start Conversion
	while(!(ADCSRA & (1 << ADIF))); //Loop until ADC conversion completes
	uint16_t adc_value = ADCL; //Read ADC low byte
	adc_value = adc_value | (ADCH << 8); //Read ADC high byte
	return adc_value;
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
	ADMUX = (0 << REFS1) | (1 << REFS0); //Voltage Reference Selection: AV CC with external capacitor at AREF pin
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
		uint16_t x_adc_value = read_adc(Xacc);
		uint16_t y_adc_value = read_adc(Yacc);
		uint16_t x_diff = abs16(x_adc_value - ACCELEROMETER_BASELINE);
		uint16_t y_diff = abs16(y_adc_value - ACCELEROMETER_BASELINE);
		uint8_t diff = 0;
		//Send the more significant accelerometer reading over UART
		if(x_diff > y_diff)
		{
			send_val_str(x_adc_value);
			diff = x_diff;
		} else
		{
			send_val_str(y_adc_value);
			diff = y_diff;
		}
		diff = diff / 10; //Scale for buzzer frequency range
		buzzer_on = diff;
		OCR0A = 10 - diff; //Change buzzer frequency
		led_out(diff);
		_delay_ms(500);

	}
}
