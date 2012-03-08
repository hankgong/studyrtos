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
 *       Compiler:  gcc
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
#include "test.h"

// set the pin as high level
#define SETBIT(R, B) (R|=_BV(B))
// set the pin as low level
#define CLRBIT(R, B) (R&=~_BV(B))


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init
 *  Description:  This function is used to setup the pin input/output
 * =====================================================================================
 */
void init(void)
{
	// 0x00 means all port set as input, this is the default mode
	// if you need to set any port as output, just set the corresponding port as bit 1
	DDRA = 0x00;
	
	DDRB = 0x00;
	SETBIT(DDRB, PB4);		// PB4=pwm3
	SETBIT(DDRB, PB5);		// PB5=pwm2
	SETBIT(DDRB, PB6);		// PB6=pwm1
	SETBIT(DDRB, PB7);		// PB7=pwm0

	DDRC = 0x00;

	DDRD = 0x00;
	SETBIT(DDRD, PD5);		// PD5=FR1
	SETBIT(DDRD, PD6);		// PD6=BR1
	SETBIT(DDRD, PD7);		// PD7=EN1

	DDRE = 0x00;
	SETBIT(DDRE, PE5);		// PE5=EN0
	SETBIT(DDRE, PE6);		// PE6=BR0
	SETBIT(DDRE, PE7);		// PE7=FR0

	DDRF = 0x00;
	SETBIT(DDRF, PF0);		// PF0=BR3
	SETBIT(DDRF, PF1);		// PF1=EN3
	SETBIT(DDRF, PF2);		// PF2=FR3

	DDRG = 0x00;

	DDRH = 0x00;

	DDRJ = 0x00;
	
	DDRK = 0x00;
	SETBIT(DDRK, PK0);		// PK0=FR2
	SETBIT(DDRK, PK1);		// PK1=BR2
	SETBIT(DDRK, PK2);		// PK2=EN2

	return;
}

// =========================start of code about serial communication====================
// The following code is used for serial input/output
// very standard code, dont' need to change it by yourself
static void serialInit(void);
static int cput(char, FILE *);
static int cget(FILE *);
static FILE O = FDEV_SETUP_STREAM(cput, NULL, _FDEV_SETUP_WRITE);
static FILE I = FDEV_SETUP_STREAM(NULL, cget, _FDEV_SETUP_READ);

void serialInit(void)
{
	uint8_t bitRate = (16000000/9600/16 -1);
	UBRR2H = (bitRate>>8) ;			//Here, 9600 is the baudrate, 16000000 is the CPU freq
	UBRR2L = bitRate;

	/*  set the farming to 8N1 */
	UCSR2C = (3 << UCSZ20);
	UCSR2B = (1 << RXEN2) | (1 << TXEN2);

	stdout = &O;
	stdin = &I;
	return;
}

static int cput(char c, FILE *f)
{
	loop_until_bit_is_set(UCSR2A, UDRE2);
	UDR2 = c;
	return 0;
}

static int cget(FILE *f)
{
	loop_until_bit_is_set(UCSR2A, RXC2);
	return UDR2;
}

// =========================end of code about serial communication======================


int main(void)
{
	//serialInit();
	
	init();
	hserialInit(2);
	hserialStart(19200);

	while (1)
	{
		printf("why???\n\r");
		if(hs_available() > 0) {
			printf("test here\n\r");
			switch(hs_getChar()) 
			{
				case 's': 
					{
						printf("really\n");
						break;
					}
				default:
					break;
			}
		}

		SETBIT(PORTK, PK0);
		CLRBIT(PORTK, PK1);
		SETBIT(PORTK, PK2);
		
		//printf("test me\n\r");
		_delay_ms(100);
	}
	return 1;
}



