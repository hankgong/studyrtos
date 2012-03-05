/*
 * =====================================================================================
 *
 *       Filename:  regs.c
 *
 *    Description:  regs allocation and usage
 *
 *        Version:  1.0
 *        Created:  12-03-05 01:48:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <avr/io.h>

register unsigned char a asm("r20");

unsigned char add(unsigned char b, unsigned char c, unsigned char d)
{
	return b+c*d;
}


int main(void)
{
	//unsigned char a = 0;
	while(1)
	{
		a++;
		PORTB=add(a,a,a);
	}
}
