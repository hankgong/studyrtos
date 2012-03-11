/*
 * hserial.c
 *
 *  Created on: 2011-10-20
 *      Author: bionik
 */

#include <inttypes.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "hserial.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// function prototypes
//void hs_parseStr(const int port, const char *str);

static int SERIALPORT=0;

// Initialize rx_buffer arrays accordingly.
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	ring_buffer rx_buffer[4] = { { { 0 }, 0, 0 },
								 { { 0 }, 0, 0 },
								 { { 0 }, 0, 0 },
								 { { 0 }, 0, 0 } };
#else
	ring_buffer rx_buffer[1] =  { { { 0 }, 0, 0 } };
#endif

// serial macros
#if defined(__AVR_ATmega1280__ ) || defined(__AVR_ATmega2560__)

	SIGNAL(SIG_USART0_RECV)
	{
		unsigned char c = UDR0;
		store_char(c, &rx_buffer[0]);
	}

	SIGNAL(SIG_USART1_RECV)
	{
		unsigned char c = UDR1;
		store_char(c, &rx_buffer[1]);
	}

	SIGNAL(SIG_USART2_RECV)
	{
		unsigned char c = UDR2;
		store_char(c, &rx_buffer[2]);
	}

	SIGNAL(SIG_USART3_RECV)
	{
		unsigned char c = UDR3;
		store_char(c, &rx_buffer[3]);
	}

#else
	#if defined(__AVR_ATmega8__)
	SIGNAL(SIG_UART_RECV)
	#else
	SIGNAL(USART_RX_vect)
	#endif
	{
	#if defined(__AVR_ATmega8__)
		unsigned char c = UDR;
	#else
		unsigned char c = UDR0;
	#endif
		store_char(c, &rx_buffer[0]);
	}
#endif

// Initialize serial objects according to processor type.
void hserialInit(unsigned int port){
	#if defined(__AVR_ATmega8__)
		Serial[0].rx_buffer = &rx_buffer[0];
		Serial[0].ubrrh = &UBRRH;
		Serial[0].ubrrl = &UBRRL;
		Serial[0].ucsra = &UCSRA;
		Serial[0].ucsrb = &UCSRB;
		Serial[0].udr = &UDR;
		Serial[0].rxen = RXEN;
		Serial[0].txen = TXEN;
		Serial[0].rxcie = RXCIE;
		Serial[0].udre = UDRE;
	#else
		Serial[0].rx_buffer = &rx_buffer[0];
		Serial[0].ubrrh = &UBRR0H;
		Serial[0].ubrrl = &UBRR0L;
		Serial[0].ucsra = &UCSR0A;
		Serial[0].ucsrb = &UCSR0B;
		Serial[0].udr = &UDR0;
		Serial[0].rxen = RXEN0;
		Serial[0].txen = TXEN0;
		Serial[0].rxcie = RXCIE0;
		Serial[0].udre = UDRE0;
	#endif

	#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
		Serial[1].rx_buffer = &rx_buffer[1];
		Serial[1].ubrrh = &UBRR1H;
		Serial[1].ubrrl = &UBRR1L;
		Serial[1].ucsra = &UCSR1A;
		Serial[1].ucsrb = &UCSR1B;
		Serial[1].udr = &UDR1;
		Serial[1].rxen = RXEN1;
		Serial[1].txen = TXEN1;
		Serial[1].rxcie = RXCIE1;
		Serial[1].udre = UDRE1;

		Serial[2].rx_buffer = &rx_buffer[2];
		Serial[2].ubrrh = &UBRR2H;
		Serial[2].ubrrl = &UBRR2L;
		Serial[2].ucsra = &UCSR2A;
		Serial[2].ucsrb = &UCSR2B;
		Serial[2].udr = &UDR2;
		Serial[2].rxen = RXEN2;
		Serial[2].txen = TXEN2;
		Serial[2].rxcie = RXCIE2;
		Serial[2].udre = UDRE2;

		Serial[3].rx_buffer = &rx_buffer[3];
		Serial[3].ubrrh = &UBRR3H;
		Serial[3].ubrrl = &UBRR3L;
		Serial[3].ucsra = &UCSR3A;
		Serial[3].ucsrb = &UCSR3B;
		Serial[3].udr = &UDR3;
		Serial[3].rxen = RXEN3;
		Serial[3].txen = TXEN3;
		Serial[3].rxcie = RXCIE3;
		Serial[3].udre = UDRE3;
	#endif

	SERIALPORT = port;
	my_stream = fdevopen(hs_writeChar, hs_getChar);
	/*FILE output=FDEV_SETUP_STREAM(hs_writeChar, NULL, _FDEV_SETUP_WRITE);*/
	/*FILE input =FDEV_SETUP_STREAM(NULL, hs_getChar, _FDEV_SETUP_READ);*/
	stdout = my_stream;
	stdin  = my_stream;
}

void hserialStart(unsigned long baud){
	*Serial[SERIALPORT].ubrrh = ((F_CPU / 16 + baud / 2) / baud - 1) >> 8;
	*Serial[SERIALPORT].ubrrl = ((F_CPU / 16 + baud / 2) / baud - 1);
	sbi(*Serial[SERIALPORT].ucsrb, Serial[SERIALPORT].rxen);
	sbi(*Serial[SERIALPORT].ucsrb, Serial[SERIALPORT].txen);
	sbi(*Serial[SERIALPORT].ucsrb, Serial[SERIALPORT].rxcie);
}

uint8_t hs_available(void){
	return (RX_BUFFER_SIZE + Serial[SERIALPORT].rx_buffer->head -
			Serial[SERIALPORT].rx_buffer->tail) % RX_BUFFER_SIZE;
}

int hs_getChar(void){
	/*
	 * If the head isn't ahead of the tail,
	 * we don't have any characters
	 */
	if (Serial[SERIALPORT].rx_buffer->head == Serial[SERIALPORT].rx_buffer->tail) {
		return -1;
	} else {
		unsigned char c =
			Serial[SERIALPORT].rx_buffer->buffer[Serial[SERIALPORT].rx_buffer->tail];
		Serial[SERIALPORT].rx_buffer->tail =
			(Serial[SERIALPORT].rx_buffer->tail + 1) % RX_BUFFER_SIZE;
		return c;
	}
}

int hs_writeChar(char c, FILE *f){
	while (!((*Serial[SERIALPORT].ucsra) & (1 << Serial[SERIALPORT].udre)));
	*(Serial[SERIALPORT].udr) = c;
	return 0;
}

void hs_flush(){
	Serial[SERIALPORT].rx_buffer->head = Serial[SERIALPORT].rx_buffer->tail;
}

inline void store_char(unsigned char c, ring_buffer *rx_buffer){
	int i = (rx_buffer->head + 1) % RX_BUFFER_SIZE;
	// if we should be storing the received character into the location
	// just before the tail (meaning that the head would advance to the
	// current location of the tail), we're about to overflow the buffer
	// and so we don't write the character or advance the head.
	if (i != rx_buffer->tail) {
		rx_buffer->buffer[rx_buffer->head] = c;
		rx_buffer->head = i;
	}
}
