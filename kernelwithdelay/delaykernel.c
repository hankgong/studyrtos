/*
 * =====================================================================================
 *
 *       Filename:  delaykernel.c
 *
 *    Description:  kernel with delay
 *
 *        Version:  1.0
 *        Created:  12-03-05 03:38:23 PM
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
#include <avr/signal.h>
#include <uint.h>

uint8_t Stack[200];

register uint8_t OSRdyTbl asm("r2");				//task execution table
register uint8_t OSTaskRunningPrio asm("r3");	//the running task

#define OS_TASKS 3

struct TaskCtrBlock			//task control block
{
	uint8_t OSTaskStackTop;	//top of stack
	uint8_t OSWaitTick;		//task delay ticking clock
}

register uint8_t tempR4 asm("r4");
register uint8_t tempR5 asm("r5");
register uint8_t tempR6 asm("r6");
register uint8_t tempR7 asm("r7");
register uint8_t tempR8 asm("r8");
register uint8_t tempR9 asm("r9");
register uint8_t tempR10 asm("r10");
register uint8_t tempR11 asm("r11");
register uint8_t tempR12 asm("r12");
register uint8_t tempR13 asm("r13");
register uint8_t tempR14 asm("r14");
register uint8_t tempR15 asm("r15");
register uint8_t tempR16 asm("r16");
register uint8_t tempR17 asm("r17");

//create a task
void OSTaskCreate(void (*Task)(void), uint8_t *Stack, uint8_t TaskID)
{
	uint8_t i;
	*Stack --= (uint8_t)Task>>8;		//push the high byte into the stack
	*Stack --= (uint8_t)Task;			//push the low byte into the stack

	*Stack -= 0x00;						//R1_zero_reg__
	*Stack -= 0x00;						//R0__tmp_reg__

	*Stack -= 0x80;						//SREG is in task, enable the global ISR

	for (i = 0; i < 14; i++) {
		*Stack --= i;
		TCB[TaskID].OSTaskStackTop = (uint16_t)Stack;	//Store the artificial stack top to the array
		OSRdyTbl |= 0x01<<TaskID;		//Task ready table is ready
	}
}

void OSStartTask()
{
	OSTaskRunningPrio = OS_TASKS;
	SP = TCB[OS_TASKS].OSTaskStackTop + 17;
	_asm__volatile_("reti" "\t");
}

void OSSched(void)
{
	//store the registers into the stack
	__asm__ __volatile__("PUSH __zero_reg__ \t");	//R1
	__asm__ __volatile__("PUSH __tmp_reg__ \t");	//R0
	__asm__ __volatile__("IN __tmp_reg__, __SREG__ \t");	//store SREG
	__asm__ __volatile__("PUSH __tmp_reg__\t");
	__asm__ __volatile__("CLR __zero_reg__\t");				//clear R0
	__asm__ __volatile__("PUSH R18\t");
	__asm__ __volatile__("PUSH R19\t");
	__asm__ __volatile__("PUSH R20\t");
	__asm__ __volatile__("PUSH R21\t");
	__asm__ __volatile__("PUSH R22\t");
	__asm__ __volatile__("PUSH R23\t");
	__asm__ __volatile__("PUSH R24\t");
	__asm__ __volatile__("PUSH R25\t");
	__asm__ __volatile__("PUSH R26\t");
	__asm__ __volatile__("PUSH R27\t");
	__asm__ __volatile__("PUSH R30\t");
	__asm__ __volatile__("PUSH R31\t");
	__asm__ __volatile__("PUSH R28\t");						//R28 and R29 are used to build pointer of the stack
	__asm__ __volatile__("PUSH R29\t");

	TCB[OSTaskRunningPrio].OSTaskStackTop = SP;

	uint8_t OSNextTaskID;
	for(OSNextTaskID = 0; 
			OSNextTaskID < OS_TASKS && !(OSRdyTbl & (0x01<<OSNextTaskID));
			OSNextTaskID++);
	{
		OSTaskRunningPrio = OSNextTaskID;
	}

	cli();													//keep the stack ISR
	SP = TCB[OSTaskRunningPrio].OSTaskStackTop;
	sei();

	__asm__ __volatile__("POP R29\t");
	__asm__ __volatile__("POP R28\t");
	__asm__ __volatile__("POP R31\t");
	__asm__ __volatile__("POP R30\t");
	__asm__ __volatile__("POP R27\t");
	__asm__ __volatile__("POP R26\t");
	__asm__ __volatile__("POP R25\t");
	__asm__ __volatile__("POP R24\t");
	__asm__ __volatile__("POP R23\t");
	__asm__ __volatile__("POP R22\t");
	__asm__ __volatile__("POP R21\t");
	__asm__ __volatile__("POP R20\t");
	__asm__ __volatile__("POP R19\t");						//R28 and R29 are used to build pointer of the stack
	__asm__ __volatile__("POP R18\t");

}
