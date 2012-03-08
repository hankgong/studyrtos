/********************************************************************************************************
*                                          The Real-Time Kernel
*                                           Configuration File 
* File    : AVR_RTOS.H
* By      : Handong
* Version : V2.0
*********************************************************************************************************/
#ifndef _AVR_RTOS_H_
#define _AVR_RTOS_H_
/*********************************************************************************************************
*                                           INCLUDE HEADER FILES
*********************************************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/signal.h> 

/*********************************************************************************************************
*                                             SYSTEM SETTING
*********************************************************************************************************/
#define F_CPU                       16000000        /*����ϵͳ����Ƶ��         */
#define OS_TASKS			        3				/*�趨�������������       */
#define OS_TICKS_PER_SEC            100             /*�趨ÿ�������           */
#define OS_TASK_RESUME_EN           1
#define OS_CPU_HOOKS_EN             1
#define OS_TASK_SUSPEND_EN          1
#define OS_SEM_CREATE_EN            1
#define OS_SEM_PEND_EN              1
#define SE_SEM_ACCEPT_EN            1
#define OS_SEM_POST_EN              1
#define OS_MBOX_CREATE_EN           0
#define OS_MBOX_PEND_EN             0 
#define OS_MBOX_ACCEPT_EN           0
#define OS_MBOX_POST_EN             0
#define IDLE_STACK_SIZE	            32				/*���������ջ�ռ�	   	   */
#define IDLE_TASK_PRIO              OS_TASKS        /*����������ȼ������     */
#define OS_TIME_DLY_HMSM_EN         0
#define OS_SCHED_LOCK_EN            0

/*���ݸ�ʽ����                                                                 */
typedef unsigned char               INT8U;          
typedef signed char                 INT8S;		
typedef unsigned int                INT16U;		
typedef signed int                  INT16S;		
typedef unsigned long               INT32U;		
typedef signed long                 INT32S;	    

/*��ֹ��������ռ��                                                             */
register INT8U OSRdyTbl             asm("r2");		 /*�������о�����	       */
register INT8U OSPrioCur		    asm("r3");       /*�������е�����	       */
register INT8U OSIntNesting		    asm("r4");       /*�ж�Ƕ�׼�����          */
register INT8U OSPrioHighRdy		asm("r5");		 /*�������ȼ�������	       */
register INT8U OSLockNesting        asm("r6");       /*��������������          */
//register INT8U tempR7               asm("r7");
//register INT8U tempR8               asm("r8");
//register INT8U tempR9               asm("r9");
//register INT8U tempR10              asm("r10");
//register INT8U tempR11              asm("r11");
//register INT8U tempR12              asm("r12");
//register INT8U tempR13              asm("r13");
//register INT8U tempR14              asm("r14");
//register INT8U tempR15              asm("r15");
//register INT8U tempR16			  asm("r16");
//register INT8U tempR17              asm("r17");
	 
volatile INT32U  OSIdleCtr;                          /*Idle������              */
INT8U    IdleStack[IDLE_STACK_SIZE];                 /*����IDLE�����ջ        */

#define OS_FALSE                    0u
#define OS_TRUE                     1u

#define OS_ERR_NONE                 0u
#define OS_ERR_TIMEOUT              1u
#define OS_ERR_NO_SEM               2u
#define OS_ERR_MBOX_FULL            3u
#define OS_MBOX_EMPTY               4u
#define OS_MBOX_FULL                5u
#define OS_ERR_PEND_LOCKED          6u
#define OS_ERR_PEND_ISR             7u


/*�����ٽ������*/ 
#define		OS_ENTER_CRITICAL()          \
asm volatile(                            \
"IN   __tmp_reg__,__SREG__"    "\n\t"    \
"PUSH __tmp_reg__"             "\n\t"    \
"CLR  __zero_reg__"            "\n\t"    \
"CLI"                          "\n\t"    \
)  \

/*�˳��ٽ������*/
#define		OS_EXIT_CRITICAL()           \
asm volatile(                            \
"POP  __tmp_reg__"            "\n\t"     \
"OUT  __SREG__,__tmp_reg__"   "\n\t"     \
"CLR  __zero_reg__"           "\n\t"     \
)   \

/*������ƿ����ݽṹ*/
typedef struct TaskCtrBlk
{
	INT16U	OSTaskStackTop;		/*��������Ķ�ջ��		                 */
	INT16U	OSWaitTick;			/*������ʱʱ��		 	                 */
} OS_TCB;
OS_TCB		TCB[OS_TASKS+1];    /*����������ƿ�,��һ����Ϊ��һ����������*/

void OSTaskCreate(void (*Task)(void),INT8U *Stack,INT8U prio);
void OSStart(void);
void OS_TASK_SW(void);
void OS_Sched(void);
void OSIntCtxSw(void);
void OSTaskSuspend(INT8U prio);
void OSTaskResume(INT8U prio);
void OSTimeDly(INT16U ticks);
void TCN0Init(void);
void IdleTask(void);
void OSInit(void);
void OS_SchedNew (void);
void OSIntEnter(void);
void OSIntExit(void);
void OSTimeTick (void);
void OSTimeTickHook (void);
void OSTaskIdleHook (void);
void OSInitHookBegin (void);
void OSSchedLock (void);
void OSSchedUnlock (void);
/*�ź������ݽṹ*/
typedef struct SemBlk
{
	INT8U OSSemCnt;			    /*�ź���������            */ 
	INT8U OSSemPendTbl;		    /*�ȴ��ź����������б�	  */
} OS_SEM;

INT8U OSSemCreate(OS_SEM *pevent,INT8U cnt);
void  OSSemPend (OS_SEM *pevent, INT16U timeout, INT8U *perr);
INT8U OSSemAccept(OS_SEM *pevent);
INT8U OSSemPost(OS_SEM *pevent);

typedef struct MboxBlk
{
	void  *OSMboxPtr;			/*����ָ��                */ 
	INT8U OSMboxPendTbl;		/*�ȴ�����������б�	  */
	INT8U OSMboxStatus;         /*����״̬                */
} OS_MBOX;

INT8U OSMboxCreate(OS_MBOX *pevent);
void *OSMboxPend(OS_MBOX *pevent,INT16U Timeout,INT8U *perr);
void *OSMboxAccept(OS_MBOX *pevent);
INT8U OSMboxPost(OS_MBOX *pevent,void *pmsg );

#endif
