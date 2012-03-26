/*
 * =====================================================================================
 *
 *       Filename:  fish.c
 *
 *    Description:  for Fasil tesing
 *
 *        Version:  1.0
 *        Created:  12-03-08 12:34:58 PM
 *       Revision:  none
 *       Compiler:  avr-gcc
 *
 *         Author:  Huazhi (Hank) GONG, 
 *   Organization:  Bionik Labs
 *
 * =====================================================================================
 */
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdint.h>
#include "hserial.h"
#include <avr/interrupt.h>

// set the pin as high level for 
#define SETBIT(R, B) (R|=_BV(B))
// set the pin as low level
#define CLRBIT(R, B) (R&=~_BV(B))

// Use HIGH(PPXx)/LOW(PPXx) to set high/low on a pin
#define SETHIGH(P)		(*Ports[P] |= _BV(Bv[P]))
#define SETLOW(P)		(*Ports[P] &=~_BV(Bv[P]))

#define HIGH	1
#define LOW		0

// Use OUTPUT(PPXx)/INPUT(PPXx) to set a pin as output/input
#define OUTPUT(P)	(*Ddrs[P] |= _BV(Bv[P]))
#define INPUT(P)	(*Ddrs[P] &=~_BV(Bv[P]))

typedef struct _MotorPins
{
	uint8_t pwmPin;
	uint8_t enablePin;
	uint8_t brakePin;
	uint8_t directionPin;
	uint8_t countPin;
} MotorPins;

typedef struct _MotorStatus
{
	uint8_t index;			//motor index
	uint8_t status;			//status indicator: 0 means stop, 1 means forward running, 2 means reverse running
	uint8_t pwm;			//current pwm value
	uint8_t direction;		//not using
	uint8_t brake;			//not using
	uint32_t rounds;		//rounds counting
} MotorStatus;


//global arrays for motor pins setup and status
MotorPins motorPins[4];
MotorStatus motorStatus[4];


//Pins alias setup==========================================
enum PortDef 
{
	/*  8 PA pins */
	PPA0, PPA1, PPA2, PPA3, PPA4, PPA5, PPA6, PPA7,
	/*  8 PB pins */
	PPB0, PPB1, PPB2, PPB3, PPB4, PPB5, PPB6, PPB7,
	/*  8 PC pins */
	PPC0, PPC1, PPC2, PPC3, PPC4, PPC5, PPC6, PPC7,
	/*  8 PD pins */
	PPD0, PPD1, PPD2, PPD3, PPD4, PPD5, PPD6, PPD7,
	/*  8 PE pins */
	PPE0, PPE1, PPE2, PPE3, PPE4, PPE5, PPE6, PPE7,
	/*  8 PF pins */
	PPF0, PPF1, PPF2, PPF3, PPF4, PPF5, PPF6, PPF7,
	/*  6 PG pins */
	PPG0, PPG1, PPG2, PPG3, PPG4, PPG5,
	/*  8 PH pins */
	PPH0, PPH1, PPH2, PPH3, PPH4, PPH5, PPH6, PPH7,
	/*  8 PJ pins */
	PPJ0, PPJ1, PPJ2, PPJ3, PPJ4, PPJ5, PPJ6, PPJ7, 
	/*  8 PK pins */
	PPK0, PPK1, PPK2, PPK3, PPK4, PPK5, PPK6, PPK7,
	/*  8 PL pins */
	PPL0, PPL1, PPL2, PPL3, PPL4, PPL5, PPL6, PPL7, 
};

uint8_t Bv[] =
{
	PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7,
	PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7,
	PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7,
	PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7,
	PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7,
	PF0, PF1, PF2, PF3, PF4, PF5, PF6, PF7,
	PG0, PG1, PG2, PG3, PG4, PG5,
	PH0, PH1, PH2, PH3, PH4, PH5, PH6, PH7,
	PJ0, PJ1, PJ2, PJ3, PJ4, PJ5, PJ6, PJ7,
	PK0, PK1, PK2, PK3, PK4, PK5, PK6, PK7,
	PL0, PL1, PL2, PL3, PL4, PL5, PL6, PL7,
};

volatile uint8_t *Ports[] = 
{
	&PORTA,	&PORTA,	&PORTA,	&PORTA,	&PORTA,	&PORTA,	&PORTA,	&PORTA,
	&PORTB,	&PORTB,	&PORTB,	&PORTB,	&PORTB,	&PORTB,	&PORTB,	&PORTB,
	&PORTC, &PORTC, &PORTC, &PORTC, &PORTC, &PORTC, &PORTC, &PORTC, 
	&PORTD,	&PORTD,	&PORTD,	&PORTD,	&PORTD,	&PORTD,	&PORTD,	&PORTD,	
	&PORTE,	&PORTE,	&PORTE,	&PORTE,	&PORTE,	&PORTE,	&PORTE,	&PORTE,	
	&PORTF,	&PORTF,	&PORTF,	&PORTF,	&PORTF,	&PORTF,	&PORTF,	&PORTF,	
	&PORTG,	&PORTG,	&PORTG,	&PORTG,	&PORTG,	&PORTG,	
	&PORTH,	&PORTH,	&PORTH,	&PORTH,	&PORTH,	&PORTH,	&PORTH,	&PORTH,	
	&PORTJ,	&PORTJ,	&PORTJ,	&PORTJ,	&PORTJ,	&PORTJ,	&PORTJ,	&PORTJ,	
	&PORTK,	&PORTK,	&PORTK,	&PORTK,	&PORTK,	&PORTK,	&PORTK,	&PORTK,	
	&PORTL,	&PORTL,	&PORTL,	&PORTL,	&PORTL,	&PORTL,	&PORTL,	&PORTL,	
};

volatile uint8_t *Ddrs[] =
{
	&DDRA, &DDRA, &DDRA, &DDRA, &DDRA, &DDRA, &DDRA, &DDRA, 
	&DDRB, &DDRB, &DDRB, &DDRB, &DDRB, &DDRB, &DDRB, &DDRB, 
	&DDRC, &DDRC, &DDRC, &DDRC, &DDRC, &DDRC, &DDRC, &DDRC, 
	&DDRD, &DDRD, &DDRD, &DDRD, &DDRD, &DDRD, &DDRD, &DDRD, 
	&DDRE, &DDRE, &DDRE, &DDRE, &DDRE, &DDRE, &DDRE, &DDRE, 
	&DDRF, &DDRF, &DDRF, &DDRF, &DDRF, &DDRF, &DDRF, &DDRF, 
	&DDRG, &DDRG, &DDRG, &DDRG, &DDRG, &DDRG, 
	&DDRH, &DDRH, &DDRH, &DDRH, &DDRH, &DDRH, &DDRH, &DDRH, 
	&DDRJ, &DDRJ, &DDRJ, &DDRJ, &DDRJ, &DDRJ, &DDRJ, &DDRJ, 
	&DDRK, &DDRK, &DDRK, &DDRK, &DDRK, &DDRK, &DDRK, &DDRK, 
	&DDRL, &DDRL, &DDRL, &DDRL, &DDRL, &DDRL, &DDRL, &DDRL, 
};

enum PwmPort
{
	TIMER0A, TIMER0B,
	TIMER1A, TIMER1B,
	TIMER2A, TIMER2B,
	TIMER3A, TIMER3B, TIMER3C,
	TIMER4A, TIMER4B, TIMER4C,
	TIMER5A, TIMER5B, TIMER5C,
};

uint8_t Timers[] =
{
	-1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, TIMER2A, TIMER1A, TIMER1B, TIMER0A,
	-1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, TIMER3A, TIMER3B, TIMER3C, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, TIMER0B,
	-1, -1, -1, TIMER4A, TIMER4B, TIMER4C, TIMER2B, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, TIMER5A, TIMER5B, TIMER5C, -1, -1,
};

//Pins alias setup==========================================

void digitalWrite(enum PortDef pin, int val)
{
	if (val== HIGH)
	{
		SETHIGH(pin);
	} 
	else if (val == LOW)
	{
		SETLOW(pin);
	}
}


// PWM output function, val is in range (0, 255) 
void analogWrite(enum PortDef pin, int val)
{
	OUTPUT(pin);
	
	if (val <= -1)
	{
		digitalWrite(pin, LOW);
	} 
	else if (val > 255)
	{
		digitalWrite(pin, HIGH);
	}
	else
	{
		switch(Timers[pin])
		{
			case TIMER0A:
				SETBIT(TCCR0A, COM0A1);
				OCR0A = val;
				break;
			case TIMER0B:
				SETBIT(TCCR0A, COM0B1);
				OCR0B = val;
				break;
			case TIMER1A:
				SETBIT(TCCR1A, COM1A1);
				OCR1A = val;
				break;
			case TIMER1B:
				SETBIT(TCCR1A, COM1B1);
				OCR1B = val;
				break;
			case TIMER2A:
				SETBIT(TCCR2A, COM2A1);
				OCR2A = val;
				break;
			case TIMER2B:
				SETBIT(TCCR2A, COM2B1);
				OCR2B = val;
			default:
				break;
		}
	}
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sysInit
 *  Description:  Everything about configuration registers
 * =====================================================================================
 */
void sysInit(void)
{
	//enable the global interrupt, it should be before every thing
	sei();

	// 0x00 means all port set as input, this is the default mode
	DDRA = 0x00;
	DDRB = 0x00;
	DDRC = 0x00;
	DDRD = 0x00;
	DDRE = 0x00;
	DDRF = 0x00;
	DDRG = 0x00;
	DDRH = 0x00;
	DDRJ = 0x00;
	DDRK = 0x00;

	//configuration for pwm timers
	SETBIT(TCCR0B, CS01);
	SETBIT(TCCR0B, CS00);
	SETBIT(TCCR0A, WGM00);

	SETBIT(TCCR1B, CS11);
	SETBIT(TCCR1B, CS10);
	SETBIT(TCCR1A, WGM10);

	SETBIT(TCCR2B, CS22);
	SETBIT(TCCR2A, WGM20);

	//configuration for counting registers
	//ICP1
	SETBIT(TIMSK1, ICIE1);

	SETBIT(TCCR1B, ICNC1);
	SETBIT(TCCR1B, ICES1);
	SETBIT(TCCR1B, CS10);

	//ICP3
	SETBIT(TIMSK3, ICIE3);

	SETBIT(TCCR3B, ICNC3);
	SETBIT(TCCR3B, ICES3);
	SETBIT(TCCR3B, CS30);
	
	//ICP4
	SETBIT(TIMSK4, ICIE4);

	SETBIT(TCCR4B, ICNC4);
	SETBIT(TCCR4B, ICES4);
	SETBIT(TCCR4B, CS40);

	//ICP5
	SETBIT(TIMSK5, ICIE5);

	SETBIT(TCCR5B, ICNC5);
	SETBIT(TCCR5B, ICES5);
	SETBIT(TCCR5B, CS50);

	return;
}

//interrupt handling for ICP capturing
ISR(TIMER1_CAPT_vect, ISR_NOBLOCK)
{
	motorStatus[3].rounds++;
}

ISR(TIMER3_CAPT_vect, ISR_NOBLOCK)
{
	motorStatus[2].rounds++;
}

ISR(TIMER4_CAPT_vect, ISR_NOBLOCK)
{
	motorStatus[1].rounds++;
}

ISR(TIMER5_CAPT_vect, ISR_NOBLOCK)
{
	motorStatus[0].rounds++;
}

/*  All PIN setup here */
void motorInit(void) 
{
	motorPins[0].pwmPin			= PPB7;
	motorPins[0].enablePin		= PPE5;
	motorPins[0].brakePin		= PPE3;
	motorPins[0].directionPin	= PPE4;
	motorPins[0].countPin		= PPL1;

	motorPins[1].pwmPin			= PPB6;
	motorPins[1].enablePin		= PPD3;
	motorPins[1].brakePin		= PPD5;
	motorPins[1].directionPin	= PPD2;
	motorPins[1].countPin		= PPL0;

	motorPins[2].pwmPin			= PPB5;
	motorPins[2].enablePin		= PPK2;
	motorPins[2].brakePin		= PPK1;
	motorPins[2].directionPin	= PPK0;
	motorPins[2].countPin		= PPE7;

	motorPins[3].pwmPin			= PPB4;
	motorPins[3].enablePin		= PPF1;
	motorPins[3].brakePin		= PPF0;
	motorPins[3].directionPin	= PPF2;
	motorPins[3].countPin		= PPD4;

	for (int i = 0; i < 4; i++) 
	{
		/*  Pin setup */
		OUTPUT(motorPins[i].pwmPin);
		OUTPUT(motorPins[i].enablePin);
		OUTPUT(motorPins[i].brakePin);
		OUTPUT(motorPins[i].directionPin);
		INPUT(motorPins[i].countPin);

		/*  motor status initialization */
		motorStatus[i].index = i;
		motorStatus[i].status = 0;
		motorStatus[i].pwm = 125;
		motorStatus[i].direction = 1;
		motorStatus[i].brake = 1;
		motorStatus[i].rounds = 0;
	}
	return;
}

int main(void)
{
	sysInit();

	motorInit();
	hserialInit(2);
	hserialStart(19200);

	printf("Ready to go...\n\r");

	while (1)
	{
		if(hs_available() > 0) {
			switch(hs_getChar()) 
			{
				case '1': 
					if(motorStatus[0].status == 0)
					{
						digitalWrite(motorPins[0].enablePin, HIGH);
						digitalWrite(motorPins[0].brakePin, HIGH);
						digitalWrite(motorPins[0].directionPin, HIGH);
						analogWrite(motorPins[0].pwmPin, motorStatus[0].pwm);
						motorStatus[0].status = 1;
						motorStatus[0].rounds = 0;
					} 
					else if (motorStatus[0].status == 1)
					{
						digitalWrite(motorPins[0].enablePin, HIGH);
						digitalWrite(motorPins[0].brakePin, HIGH);
						digitalWrite(motorPins[0].directionPin, LOW);
						analogWrite(motorPins[0].pwmPin, motorStatus[0].pwm);
						motorStatus[0].status = 2;
						motorStatus[0].rounds = 0;
					} 
					else if (motorStatus[0].status == 2)
					{
						digitalWrite(motorPins[0].enablePin, LOW);
						digitalWrite(motorPins[0].brakePin, LOW);
						digitalWrite(motorPins[0].directionPin, LOW);
						analogWrite(motorPins[0].pwmPin, 0);
						motorStatus[0].status = 0;
					}
					break;
				case '2':
					if(motorStatus[1].status == 0)
					{
						digitalWrite(motorPins[1].enablePin, HIGH);
						digitalWrite(motorPins[1].brakePin, HIGH);
						digitalWrite(motorPins[1].directionPin, HIGH);
						analogWrite(motorPins[1].pwmPin, motorStatus[1].pwm);
						motorStatus[1].status = 1;
						motorStatus[1].rounds = 0;
					} 
					else if (motorStatus[1].status == 1)
					{
						digitalWrite(motorPins[1].enablePin, HIGH);
						digitalWrite(motorPins[1].brakePin, HIGH);
						digitalWrite(motorPins[1].directionPin, LOW);
						analogWrite(motorPins[1].pwmPin, motorStatus[1].pwm);
						motorStatus[1].status = 2;
						motorStatus[1].rounds = 0;
					} 
					else if (motorStatus[1].status == 2)
					{
						digitalWrite(motorPins[1].enablePin, LOW);
						digitalWrite(motorPins[1].brakePin, LOW);
						digitalWrite(motorPins[1].directionPin, LOW);
						analogWrite(motorPins[1].pwmPin, 0);
						motorStatus[1].status = 0;
					}
					break;
				case '3':
					if(motorStatus[2].status == 0)
					{
						digitalWrite(motorPins[2].enablePin, HIGH);
						digitalWrite(motorPins[2].brakePin, HIGH);
						digitalWrite(motorPins[2].directionPin, HIGH);
						analogWrite(motorPins[2].pwmPin, motorStatus[2].pwm);
						motorStatus[2].status = 1;
						motorStatus[2].rounds = 0;
					} 
					else if (motorStatus[2].status == 1)
					{
						digitalWrite(motorPins[2].enablePin, HIGH);
						digitalWrite(motorPins[2].brakePin, HIGH);
						digitalWrite(motorPins[2].directionPin, LOW);
						analogWrite(motorPins[2].pwmPin, motorStatus[2].pwm);
						motorStatus[2].status = 2;
						motorStatus[2].rounds = 0;
					} 
					else if (motorStatus[2].status == 2)
					{
						digitalWrite(motorPins[2].enablePin, LOW);
						digitalWrite(motorPins[2].brakePin, LOW);
						digitalWrite(motorPins[2].directionPin, LOW);
						analogWrite(motorPins[2].pwmPin, 0);
						motorStatus[2].status = 0;
					}
					break;
				case '4':
					if(motorStatus[3].status == 0)
					{
						digitalWrite(motorPins[3].enablePin, HIGH);
						digitalWrite(motorPins[3].brakePin, HIGH);
						digitalWrite(motorPins[3].directionPin, HIGH);
						analogWrite(motorPins[3].pwmPin, motorStatus[0].pwm);
						motorStatus[3].status = 1;
						motorStatus[3].rounds = 0;
					} 
					else if (motorStatus[3].status == 1)
					{
						digitalWrite(motorPins[3].enablePin, HIGH);
						digitalWrite(motorPins[3].brakePin, HIGH);
						digitalWrite(motorPins[3].directionPin, LOW);
						analogWrite(motorPins[3].pwmPin, motorStatus[0].pwm);
						motorStatus[3].status = 2;
						motorStatus[3].rounds = 0;
					} 
					else if (motorStatus[3].status == 2)
					{
						digitalWrite(motorPins[3].enablePin, LOW);
						digitalWrite(motorPins[3].brakePin, LOW);
						digitalWrite(motorPins[3].directionPin, LOW);
						analogWrite(motorPins[3].pwmPin, 0);
						motorStatus[3].status = 0;
					}
					break;
				case ']':
					for (int i = 0; i <4; i++)
						if (motorStatus[i].status != 0)
						{
							motorStatus[i].pwm = (motorStatus[i].pwm<=250)?(motorStatus[i].pwm+5):(255);
							analogWrite(motorPins[i].pwmPin, motorStatus[i].pwm);
						}
					break;
				case '[':
					for (int i = 0; i <4; i++)
						if (motorStatus[i].status != 0)
						{
							motorStatus[i].pwm = (motorStatus[i].pwm>5)?(motorStatus[i].pwm-5):(0);
							analogWrite(motorPins[i].pwmPin, motorStatus[i].pwm);
						}
					break;
				default:
					break;
			}
		}
		
		for (int i = 0; i < 4; i++) 
		{
			if(motorStatus[i].status == 1) 
				printf("Motor %d pwm [%d] FW %lu |", i, motorStatus[i].pwm, motorStatus[i].rounds);
			else if (motorStatus[i].status == 2)
				printf("Motor %d pwm [%d] RVS %lu |", i, motorStatus[i].pwm, motorStatus[i].rounds);
		}

		if(motorStatus[0].status!=0 || motorStatus[1].status!=0 || motorStatus[2].status!=0 || motorStatus[3].status!=0) 
			printf("\n\r");

		_delay_ms(150);
	}
	return 1;
}



