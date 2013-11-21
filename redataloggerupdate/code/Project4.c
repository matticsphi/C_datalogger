/*	Scott Leonard
	Mattics Phi
	Project 4 - Sensor Project
*/

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>

#define F_CPU 16000000ULs
#define SET_FUNCTION 0x38
#define SET_DISPLAY 0x0F
#define SET_CGRAM 0x40
#define DISPLAY_CLEAR 0x01

/* Function Calls */
void printFunction(char*);
void callInstruction(int);
void setUpDisplay();
void Initialize_ADC0();
void printOutReading();
void calculateVoltage();
void calculateAmperage();

/* Global Variables */
char buf[10];
int numADC = 0;
int digitized = 0;
double wholeNum = 0;
double wholeNumAmp = 0;
double remainNum = 0;
double check = 0;

int main(void) 
{	
	Initialize_ADC0();							// Initializes the ADC
	UCSR0B = 0;									// TX/RX to write I/O
	DDRB = 0x03;								// enables portB output (PB1 PB0)
	DDRD = 0xFF;								// enables portD output (All of PD 7:0)
	setUpDisplay();								// function to set up display calls
	_delay_ms(10);
	
 	while (1) {
		if (PINB & 0x04) {						// button interface
			numADC++;
			if (numADC == 4) {					// Wraps around to the first button
				numADC = 0;
			}
		}
		printOutReading();						// Prints out to LCD
 	}
    return 0;
}

void printOutReading()
{
	_delay_ms(1000);
	ADCSRA = 0xC7;								// start conversion
	_delay_us(260);								// ensure max sampling rate not exceeded
	
	if (numADC == 0 || numADC == 1) {
		callInstruction(DISPLAY_CLEAR);
		digitized = ADC & 0x3FF;				// read voltage
		if (numADC == 0) {
			ADMUX = 0x01;						// changes to ADC1
			printFunction("V0: ");
		} else {
			ADMUX = 0x02;						// changes to ADC2
			printFunction("V1: ");
		}
		calculateVoltage();
		printFunction(itoa(wholeNum, buf, 10));	// prints out voltage to display
		printFunction(".");
		if (remainNum < 10) {
			printFunction(itoa(0, buf, 10));	// Buffers with 0's if remainder is less than 3 digits
			printFunction(itoa(0, buf, 10));	// so works up to three significant figures
		} else if (remainNum < 100) {
			printFunction(itoa(0, buf, 10));
		}
		printFunction(itoa(remainNum, buf, 10));
		printFunction(" V");
		callInstruction(0xC0);
	} else if (numADC == 2 || numADC == 3) {
		callInstruction(DISPLAY_CLEAR);
		digitized = ADC & 0x3FF;				// read current
		if (numADC == 2) {
			ADMUX = 0x03;						// Changes to ADC 3
			printFunction("C2: ");
		} else {
			ADMUX = 0x04;						// Changes to ADC 4
			printFunction("C3: ");
		}
		calculateAmperage();
		printFunction(itoa(wholeNum, buf, 10));
		printFunction(".");
		if (remainNum < 10) {
			printFunction(itoa(0, buf, 10));	// Buffers with 0's if remainder is less than 3 digits
			printFunction(itoa(0, buf, 10));	// so works up to three significant figures
		} else if (remainNum < 100) {
			printFunction(itoa(0, buf, 10));
		}
		printFunction(itoa(remainNum, buf, 10));
		printFunction(" A");
		callInstruction(0xC0);
		_delay_ms(1000);
	}
}

void calculateAmperage()
{
	if (digitized >= 603) {				// greater than 2.9 V
		wholeNumAmp = digitized / 58.8;
	} else if (digitized >= 591) {		// greater than 2.84 V
		wholeNumAmp = digitized / 68.6;
	} else if (digitized >= 582) {		// greater than 2.8 V
		wholeNumAmp = digitized / 77.4;
	} else if (digitized >= 571) {		// greater than 2.75 V
		wholeNumAmp = digitized / 89.8;
	} else if (digitized >= 554) {		// greater than 2.67 V
		wholeNumAmp = digitized / 126.9;
	} else if (digitized >= 550) {		// greater than 2.65 V
		wholeNumAmp = digitized / 149.2;
	} else if (digitized >= 538) {		// greater than 2.59 V
		wholeNumAmp = digitized / 228.4;
	} else if (digitized >= 536) {		// greater than 2.58 V
		wholeNumAmp = digitized / 269.8;
	} else if (digitized >= 532) {		// greater than 2.56 V
		wholeNumAmp = digitized / 332.5;
	} else if (digitized >= 529) {		// greater than 2.55 V
		wholeNumAmp = digitized / 392.6;
	} else {							// greater than 0 V
		if (check > 0) {
			wholeNumAmp = check;
		} else if (check == 0) {
			wholeNumAmp = 0;
		}
	}
	check = wholeNumAmp;
	
	remainNum = 1000 * (wholeNumAmp - floor(wholeNumAmp));
}

void calculateVoltage()
{
	if (digitized >= 842) {			// greater than 23.5 V
		wholeNum = digitized / 210.3;
		wholeNum *= 5.85;
	} else if (digitized >= 737) {	// greater than 20.6 V
		wholeNum = digitized / 210.57;
		wholeNum *= 5.88;
	} else if (digitized >= 634) {	// greater than 17.77 V
		wholeNum = digitized / 211.33;
		wholeNum *= 5.89;
	} else if (digitized >= 529) {	// greater than 14.83 V
		wholeNum = digitized / 211.6;
		wholeNum *= 5.92;
	} else if (digitized >= 414) {	// greater than 11.64 V
		wholeNum = digitized / 207.0;
		wholeNum *= 5.93;
	} else if (digitized >= 310) {	// greater than 8.78 V
		wholeNum = digitized / 206.67;
		wholeNum *= 5.82;
	} else if (digitized >= 207) {	// greater than 5.91 V
		wholeNum = digitized / 207.0;
		wholeNum *= 5.85;
	} else if (digitized >= 97) {	// greater than 2.85 V
		wholeNum = digitized / 194.0;
		wholeNum *= 5.7;
	} else if (digitized > 0) {		// greater than 0 V
		wholeNum = digitized / 230.77;
		wholeNum *= 18.46;
	} else {
		wholeNum = 0;
	}
	
	remainNum = 1000 * (wholeNum - floor(wholeNum));	// gets the decimal number up to 3 digits
	
}

/* Function to set up the display */
void setUpDisplay() 
{
	_delay_ms(20);
	callInstruction(SET_FUNCTION); // set function call
	_delay_us(37);
	callInstruction(SET_DISPLAY); // set display call
	_delay_us(37);
	callInstruction(DISPLAY_CLEAR);  // display clear call
	_delay_ms(1.52);
}

/* Function to call each instruction in the setup function */
void callInstruction(int functionCall)
{
	_delay_ms(1);
	PORTD = 0x00;
	_delay_ms(50);
	PORTB |= 1<<PB1; // set E high
	_delay_ms(50);
	PORTD = functionCall;
	_delay_ms(50);
	PORTB &= ~(1<<PB1); // E low
	_delay_ms(1);
}

/* Prints the input string to the LCD */
void printFunction(char* s)
{
	int i;
	
	for (i = 0; i < strlen(s); i++) {
		if (i == 16) { // check when it hits the 16th character (16 characters per row)
			callInstruction(0xC0); // sets DDRAM to 40 which outputs to second row of LCD
		}
		PORTB |= 1<<PB0; // Sets RS to high
		PORTD = s[i];
		_delay_ms(50);
		PORTB |= 1<<PB1; // set E high
		_delay_ms(50);
		PORTB &= ~(1<<PB1); // E low
		_delay_ms(50);
		PORTB &= ~(1<<PB0); // RS low
		_delay_ms(50);
	}
}

/* Initializes the ADC */
void Initialize_ADC0(void)
{
	ADCSRA = 0x87;    //Turn On ADC and set prescaler (CLK/128--62 kHz)
                      //MAX A/D conversion rate 5 kHz @ 62 kHz frequency    
    ADCSRB = 0x00;    //Set gain & turn off autotrigger
    ADMUX = 0x00;     //Set ADC channel ADC0 with 1X gain
}