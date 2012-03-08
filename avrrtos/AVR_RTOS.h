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
#define F_CPU                       16000000        /*定义系统晶振频率         */
#define OS_TASKS			        3				/*设定运行任务的数量       */
#define OS_TICKS_PER_SEC            100             /*设定每秒节拍数           */
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
#define IDLE_STACK_SIZE	            32				/*空闲任务堆栈空间	   	   */
#define IDLE_TASK_PRIO              OS_TASKS        /*定义最低优先级任务号     */
#define OS_TIME_DLY_HMSM_EN         0
#define OS_SCHED_LOCK_EN            0

/*数据格式定义                                                                 */
typedef unsigned char               INT8U;          
typedef signed char                 INT8S;		
typedef unsigned int                INT16U;		
typedef signed int                  INT16S;		
typedef unsigned long               INT32U;		
typedef signed long                 INT32S;	    

/*防止被编译器占用                                                             */
register INT8U OSRdyTbl             asm("r2");		 /*任务运行就绪表	       */
register INT8U OSPrioCur		    asm("r3");       /*正在运行的任务	       */
register INT8U OSIntNesting		    asm("r4");       /*中断嵌套计数器          */
register INT8U OSPrioHighRdy		asm("r5");		 /*更高优先级的任务	       */
register INT8U OSLockNesting        asm("r6");       /*调度锁定计数器          */
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
	 
volatile INT32U  OSIdleCtr;                          /*Idle计数器              */
INT8U    IdleStack[IDLE_STACK_SIZE];                 /*建立IDLE任务堆栈        */

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


/*进入临界代码区*/ 
#define		OS_ENTER_CRITICAL()          \
asm volatile(                            \
"IN   __tmp_reg__,__SREG__"    "\n\t"    \
"PUSH __tmp_reg__"             "\n\t"    \
"CLR  __zero_reg__"            "\n\t"    \
"CLI"                          "\n\t"    \
)  \

/*退出临界代码区*/
#define		OS_EXIT_CRITICAL()           \
asm volatile(                            \
"POP  __tmp_reg__"            "\n\t"     \
"OUT  __SREG__,__tmp_reg__"   "\n\t"     \
"CLR  __zero_reg__"           "\n\t"     \
)   \

/*任务控制块数据结构*/
typedef struct TaskCtrBlk
{
	INT16U	OSTaskStackTop;		/*保存任务的堆栈顶		                 */
	INT16U	OSWaitTick;			/*任务延时时钟		 	                 */
} OS_TCB;
OS_TCB		TCB[OS_TASKS+1];    /*定义任务控制块,加一是因为有一个空闲任务*/

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
/*信号量数据结构*/
typedef struct SemBlk
{
	INT8U OSSemCnt;			    /*信号量计数器            */ 
	INT8U OSSemPendTbl;		    /*等待信号量的任务列表	  */
} OS_SEM;

INT8U OSSemCreate(OS_SEM *pevent,INT8U cnt);
void  OSSemPend (OS_SEM *pevent, INT16U timeout, INT8U *perr);
INT8U OSSemAccept(OS_SEM *pevent);
INT8U OSSemPost(OS_SEM *pevent);

typedef struct MboxBlk
{
	void  *OSMboxPtr;			/*邮箱指针                */ 
	INT8U OSMboxPendTbl;		/*等待邮箱的任务列表	  */
	INT8U OSMboxStatus;         /*邮箱状态                */
} OS_MBOX;

INT8U OSMboxCreate(OS_MBOX *pevent);
void *OSMboxPend(OS_MBOX *pevent,INT16U Timeout,INT8U *perr);
void *OSMboxAccept(OS_MBOX *pevent);
INT8U OSMboxPost(OS_MBOX *pevent,void *pmsg );

#endif
