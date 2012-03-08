/*
 * hserial.h
 *
 *  Created on: 2011-10-20
 *      Author: bionik
 */

#ifndef HSERIAL_H_
#define HSERIAL_H_

#include <inttypes.h>
//#include "wiring.h"
#include <avr/io.h>
//#include "wiring_private.h"

#ifdef __cplusplus
extern "C"{
#endif

//#define SERIALPORT 0
#define RX_BUFFER_SIZE 128

FILE *my_stream;

typedef struct _ring_buffer {
	unsigned char buffer[RX_BUFFER_SIZE];
	int head;
	int tail;
} ring_buffer;

typedef struct _hserial {
	ring_buffer *rx_buffer;
    volatile uint8_t *ubrrh;
    volatile uint8_t *ubrrl;
    volatile uint8_t *ucsra;
    volatile uint8_t *ucsrb;
    volatile uint8_t *udr;
    uint8_t rxen;
    uint8_t txen;
    uint8_t rxcie;
    uint8_t udre;
} hserial;

/* The ATmega1280/2560 has 4 serial ports available
 * thus we check whether we are cross compiling to
 * this CPU and create as many serial ports as needed.
 */
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	hserial Serial[4];
#else
	hserial Serial[1];
#endif

void hserialInit(unsigned int port); 					// Initializes all available serial ports.
void hserialStart(unsigned long baud); 					// Sets up a serial port.
uint8_t hs_available(void);								// Checks whether data is available on the port
void hs_flush(void); 									// Flushes the contents on the serial port.
int hs_getChar(void);							// Gets next character (rx) on serial port.
int hs_writeChar(char c, FILE *f);					// Write a character (tx) on serial port.
void store_char(unsigned char c, ring_buffer *rx_buffer);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* HSERIAL_H_ */
