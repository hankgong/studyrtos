/*
 * =====================================================================================
 *
 *       Filename:  test1.c
 *
 *    Description:  Test1
 *
 *        Version:  1.0
 *        Created:  12-02-28 04:51:19 PM
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

void fun1(void)
{
	unsigned char i = 0;
	while(1) 
	{
		PORTB = i++;
		PORTC = 0x01<<(i%8);
	}
}

int main(void)
{
	fun1();
}
