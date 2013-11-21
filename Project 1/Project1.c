#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000ULs

void writeSetup();
void writeDisplaySetting(int);
void writeDisplayOutput(int);
void writeToDisplay(char*);

int main()
{
    UCSR0B = 0x00;	// TX/RX I/O
	_delay_ms(20);
	DDRD = 0xFF;	// PORTD 7:0 I/O
	DDRB = 0x03;	// PORTB 2:0 I/o
	writeSetup();	// sets up display (CALL ONCE)
	writeToDisplay("Hello World!");
	
// 	while (1)
// 	{
// 	}
	
	return 0;
}

void writeSetup()
{
	_delay_ms(20);
	writeDisplaySetting(0x38);
	_delay_ms(1);
	writeDisplaySetting(0x0F);
	_delay_ms(1);
	writeDisplaySetting(0x01);
	_delay_ms(2);
}

void writeDisplaySetting(int functCall) 
{
	PORTB = 0x02;	// sets E enabled
	_delay_us(0.18);
	PORTD = functCall;
	_delay_us(0.01);
	PORTB = 0x00;	// sets E off
	_delay_us(0.01);
	PORTD = 0x00;	// portd nothing
}

void writeToDisplay(char* sentence)
{
	int i;
	
	for (i = 0; i < strlen(sentence); i++) {
		writeDisplayOutput(sentence[i]);
	}
}

void writeDisplayOutput(int functCall)
{
	_delay_ms(9);
	PORTB = 0x03;	// sets E/RS enabled
	_delay_ms(10);
	PORTD = functCall;
	_delay_ms(5);
	PORTB = 0x00;	// sets E/RS off
	_delay_ms(5);
	PORTD = 0x00;	// portd nothing
}