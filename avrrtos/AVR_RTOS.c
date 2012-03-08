/*********************************************************************************************************
*                                      The Real-Time Kernel For AVR
*                                             CORE FUNCTIONS
* File    : AVR_RTOS.C
* By      : 韩冬
* Version : V2.0
*********************************************************************************************************/
#include "AVR_RTOS.h"
/*********************************************************************************************************
函数名称：	OSTaskCreate
函数原型：	void OSTaskCreate(void (*Task)(void),INT8U *Stack,INT8U prio)
函数功能：	用户函数，建立任务 
入口参数：	*Task：任务函数地址；*Stack：任务堆栈指针；prio：任务优先级
*********************************************************************************************************/
void OSTaskCreate(void (*Task)(void),INT8U *Stack,INT8U prio)
{
	INT8U i;
	*Stack--=(INT16U)Task;				     /*将任务的地址低位压入堆栈                                 */
	*Stack--=(INT16U)Task>>8;		 	     /*将任务的地址高位压入堆栈	                                */
	*Stack--=0x00;							 /*R1 __zero_reg__				                            */
	*Stack--=0x00;							 /*R0 __tmp_reg__			                                */
	*Stack--=0x80;							 /*SREG 在任务中，开启全局中断	                            */
	for(i=0;i<14;i++)
    *Stack--=i;/*在 avr-libc 中的 FAQ中的 What registers are used by the C compiler?描述了寄存器的作用	*/
	TCB[prio].OSTaskStackTop = (INT16U)Stack;/*将人工堆栈的栈顶，保存到堆栈的数组中                     */
    TCB[prio].OSWaitTick     = 0;			 /* 初始化任务延时    	                                    */
	OSRdyTbl                |= 0x01<<prio;   /*任务就绪表已经准备好		      	                        */
}
/*********************************************************************************************************
函数名称：	OSInit
函数原型：	void OSInit (void)
函数功能：	用户函数，初始化任务时钟和一些系统全局变量，建立空闲任务	
*********************************************************************************************************/
void OSInit (void)
{
	OSRdyTbl      = 0;                            /*任务就绪表清零                                      */
	OSIntNesting  = 0;	                          /*中断嵌套计数器清零                                  */
	OSLockNesting = 0;
	OSIdleCtr     = 0;
#if OS_CPU_HOOKS_EN > 0
    OSInitHookBegin ();
#endif
	OSTaskCreate(IdleTask,&IdleStack[IDLE_STACK_SIZE-1],IDLE_TASK_PRIO); /*建立IDLE任务                 */
}
/*********************************************************************************************************
函数名称：	OSStart
函数原型：	void OSStart(void)
函数功能：	用户函数，开始任务调度,从空闲任务开始运行 	
*********************************************************************************************************/
void OSStart(void)
{
	OSPrioCur     = OS_TASKS;
	OSPrioHighRdy = OS_TASKS;
	SP=TCB[OS_TASKS].OSTaskStackTop+17;
	__asm__ __volatile__("reti"                    "\n\t");
}
/*********************************************************************************************************
函数名称：	OS_TASK_SW
函数原型：	void OS_TASK_SW(void)
函数功能：	进行任务调度 
有关说明：	内部函数，中断和任务都可以调用这个任务调度函数	
*********************************************************************************************************/
void OS_TASK_SW(void)
{
	__asm__ __volatile__("PUSH __zero_reg__         \n\t");	/*R1 					                    */
	__asm__ __volatile__("PUSH __tmp_reg__          \n\t");	/*R0					                    */
	__asm__ __volatile__("IN   __tmp_reg__,__SREG__ \n\t");	/*保存状态寄存器SREG               	        */
	__asm__ __volatile__("PUSH __tmp_reg__          \n\t"); 
	__asm__ __volatile__("CLR  __zero_reg__         \n\t");	/*R0重新清零			                    */
	__asm__ __volatile__("PUSH R18                  \n\t");
	__asm__ __volatile__("PUSH R19                  \n\t");
	__asm__ __volatile__("PUSH R20                  \n\t");
	__asm__ __volatile__("PUSH R21                  \n\t");
	__asm__ __volatile__("PUSH R22                  \n\t");
	__asm__ __volatile__("PUSH R23                  \n\t");
	__asm__ __volatile__("PUSH R24                  \n\t");
	__asm__ __volatile__("PUSH R25                  \n\t");
	__asm__ __volatile__("PUSH R26                  \n\t");
	__asm__ __volatile__("PUSH R27                  \n\t");
	__asm__ __volatile__("PUSH R30                  \n\t");  
	__asm__ __volatile__("PUSH R31                  \n\t");	

	__asm__ __volatile__("Int_OSSched:              \n\t");	/*当中断要求调度，直接进入这里		        */ 
    __asm__ __volatile__("CLI                       \n\t");                
	__asm__ __volatile__("PUSH R28                  \n\t");	/*R28与R29用于建立在堆栈上的指针	        */
	__asm__ __volatile__("PUSH R29                  \n\t");	/*入栈完成							        */	
	
	TCB[OSPrioCur].OSTaskStackTop=SP;					    /*将正在运行的任务的堆栈底保存		        */	
	OSPrioCur = OSPrioHighRdy ;							    /*运行当前就绪表中的最高优先级任务	        */	
	SP=TCB[OSPrioCur].OSTaskStackTop;

	__asm__ __volatile__("POP  R29                  \n\t"); /*出栈                                      */
	__asm__ __volatile__("POP  R28                  \n\t");
	__asm__ __volatile__("POP  R31                  \n\t");
	__asm__ __volatile__("POP  R30                  \n\t");
	__asm__ __volatile__("POP  R27                  \n\t");
	__asm__ __volatile__("POP  R26                  \n\t");
	__asm__ __volatile__("POP  R25                  \n\t");
	__asm__ __volatile__("POP  R24                  \n\t");
	__asm__ __volatile__("POP  R23                  \n\t");
	__asm__ __volatile__("POP  R22                  \n\t");
	__asm__ __volatile__("POP  R21                  \n\t");
	__asm__ __volatile__("POP  R20                  \n\t");
	__asm__ __volatile__("POP  R19                  \n\t");
	__asm__ __volatile__("POP  R18                  \n\t");
	__asm__ __volatile__("POP  __tmp_reg__          \n\t"); /*SERG 出栈并恢复	                        */
	__asm__ __volatile__("OUT  __SREG__,__tmp_reg__ \n\t");
	__asm__ __volatile__("POP  __tmp_reg__          \n\t");	/*R0 出栈			                        */
	__asm__ __volatile__("POP  __zero_reg__         \n\t");	/*R1 出栈			                        */	                                                      
	__asm__ __volatile__("RETI                      \n\t"); /*出栈完成,返回并开中断		                */
}
/*********************************************************************************************************
函数名称：	OSIntCtxSw
函数原型：	void OSIntCtxSw(void)
函数功能：	从中断退出并进行调度
有关说明：	当无中断嵌套并且中断中要求进行任务切换时才进行任务切换。
			因为从中断到运行下一个任务共调用了两次子函数，所以要弹出四个入栈的PC
*********************************************************************************************************/
void OSIntCtxSw(void)
{                                
	__asm__ __volatile__("POP  R31                  \n\t");	    /*去除因调用子程序而入栈的PC            */
	__asm__ __volatile__("POP  R31                  \n\t");
	__asm__ __volatile__("POP  R31                  \n\t");
	__asm__ __volatile__("POP  R31                  \n\t");
	__asm__ __volatile__("RJMP Int_OSSched          \n\t");	    /*重新调度					            */	
}
/*********************************************************************************************************
函数名称：	OSSched
函数原型：	void OSSched(void)
函数功能：	任务调度器，进行任务调度
有关说明：	内部函数，只有有更高优先级的任务就绪时才进行一次任务切换，否则不做切换，中断中不可调用此函数	
*********************************************************************************************************/
void OS_Sched (void)
{
    OS_ENTER_CRITICAL();
	if(OSIntNesting == 0){                                 /*判断是否由中断调用，是则直接推出           */   	
	    if(OSLockNesting == 0){                            /*判断调度是否上锁，是则直接推出             */
		    OS_SchedNew();						           /*找出就绪表中优先级最高的任务               */
	        if(OSPrioHighRdy != OSPrioCur){				   /*如果不是当前运行的任务，进行任务调度       */	    
		        OS_TASK_SW();							   /*调度任务                                   */    
		    }
		}
    }
	OS_EXIT_CRITICAL();
}
/*********************************************************************************************************
函数名称：	OSTaskSuspend
函数原型：	void OSTaskSuspend(INT8U prio)
函数功能：	挂起任务 
入口参数：	prio：任务优先级
有关说明：	用户函数，一个任务可以挂起本身也可以挂起其他任务	
*********************************************************************************************************/
#if OS_TASK_SUSPEND_EN >0
void OSTaskSuspend(INT8U prio)
{
	OS_ENTER_CRITICAL();
	TCB[prio].OSWaitTick=0;         
	OSRdyTbl &= ~(0x01<<prio);		                       /*从任务就绪表上去除标志位	                */
	if(OSPrioCur == prio){			                       /*当要挂起的任务为当前任务	                */	
		OS_EXIT_CRITICAL();
		OS_Sched();					                       /*重新调度					                */
		return;
	}
	OS_EXIT_CRITICAL();
}
#endif
/*********************************************************************************************************
函数名称：	OSTaskResume
函数原型：	void OSTaskResume(INT8U prio)
函数功能：	用户函数，恢复任务 可以让被OSTaskSuspend或 OSTimeDly暂停的任务恢复 
入口参数：	prio：任务优先级	
*********************************************************************************************************/
#if OS_TASK_RESUME_EN > 0
void OSTaskResume(INT8U prio)
{
	OS_ENTER_CRITICAL();
	OSRdyTbl |= 0x01<<prio;			                       /*从任务就绪表上重置标志位	                */
    TCB[prio].OSWaitTick=0;			                       /*将时间计时设为0,到时		                */
	if(OSPrioCur > prio){			                       /*当前任务的优先级低于重置位的任务的优先级	*/	
		OS_EXIT_CRITICAL();
		OS_Sched();					                       /*重新调度				             	    */
	    return;
	}
	OS_EXIT_CRITICAL();
}
#endif
/*********************************************************************************************************
函数名称：	IdleTask(void)
函数原型：	void IdleTask(void)
函数功能：	空闲任务，当其他任务没有就绪的时候运行
有关说明：	系统开始运行的时候是从运行空闲任务开始的。该任务不能删除
这里将timer0放在此处，因为该函数是第一个运行的函数，模仿UCos在第一个任务中启动系统节拍
*********************************************************************************************************/
void IdleTask(void)
{   
    TCN0Init();                                            /*初始化定时器                               */
	OS_Sched();
	for (;;){
    	OS_ENTER_CRITICAL();
        OSIdleCtr++;
#if OS_CPU_HOOKS_EN > 0
        OSTaskIdleHook();
#endif
        OS_EXIT_CRITICAL();	
	}
}
/*********************************************************************************************************
函数名称：	OS_SchedNew
函数原型：	void OS_SchedNew (void)
函数功能：	找出任务就绪表中的最高优先级的任务
*********************************************************************************************************/
void OS_SchedNew (void)
{   
	INT8U OSNextTaskPrio;
	OSNextTaskPrio = 0;
	while ((OSNextTaskPrio < OS_TASKS) && (!(OSRdyTbl & (0x01<<OSNextTaskPrio)))){
		OSNextTaskPrio++;                                 /*找出信号量等待列表中任务优先级最高的任务    */
	}
	OSPrioHighRdy  =  OSNextTaskPrio;
}
/*********************************************************************************************************
函数名称：	OSIntEnter
函数原型：	void OSIntEnter(void)
函数功能：	进入中断时调用此函数
有关说明：	OSIntEnter()和OSIntExit()要成对出现使用
*********************************************************************************************************/
void OSIntEnter(void)
{
	OS_ENTER_CRITICAL();
	if(OSIntNesting<255u){
		OSIntNesting++;     				             /*中断嵌套加1    			                    */
	}
	OS_EXIT_CRITICAL();
}
/*********************************************************************************************************
函数名称：	OSIntExit
函数原型：	void OSIntExit(void)
函数功能：	从中断中退出时调用此函数，如果中断让更高优先级的任务就绪就进行任务调度
有关说明：	OSIntEnter()和OSIntExit()要成对出现使用	
*********************************************************************************************************/
void OSIntExit(void)
{
	OS_ENTER_CRITICAL();
	if(OSIntNesting>0){
		OSIntNesting--;     				             /*中断嵌套减1			                        */
	}
	if(OSIntNesting == 0){
	    if(OSLockNesting == 0){
		    OS_SchedNew();				                 /*找出就绪表中优先级最高的任务                 */
		    if(OSPrioHighRdy != OSPrioCur){	
			    OS_EXIT_CRITICAL();	   	
			    OSIntCtxSw();					        /*进行任务调度            	                    */	
		        return;
		    }
	    }
	}
	OS_EXIT_CRITICAL();
}
/*********************************************************************************************************
函数名称：	OSSchedLock  &  OSSchedUnlock
函数原型：	void  OSSchedLock (void)  void  OSSchedUnlock (void)
函数功能：	任务调度上锁
有关说明：	OSSchedLock ()  OSSchedUnlock ()要成对出现使用	
*********************************************************************************************************/
#if OS_SCHED_LOCK_EN > 0
void  OSSchedLock (void)
{
    OS_ENTER_CRITICAL();
    if (OSIntNesting == 0) {                             /* 中断不得调用                                */
         if(OSLockNesting < 255u){          
             OSLockNesting++;                            /* 调度嵌套计数器+1                            */
         }
    }
    OS_EXIT_CRITICAL();
}

void  OSSchedUnlock (void)
{
    OS_ENTER_CRITICAL();
    if(OSLockNesting > 0){                           
        OSLockNesting--;                              
        if(OSLockNesting == 0){                     
            if(OSIntNesting == 0){                   
                OS_EXIT_CRITICAL();
                OS_Sched();                           
            }else{
                  OS_EXIT_CRITICAL();
            }
        }else{
              OS_EXIT_CRITICAL();
        }
    }else{
          OS_EXIT_CRITICAL();
    }
}
#endif
/*********************************************************************************************************
函数名称：	OSTimeTick (void)
函数原型：	OSTimeTick (void)
函数功能：	系统节拍服务函数
有关说明：	用来为需要延时的任务进行任务延时并更新系统时间	
*********************************************************************************************************/
void  OSTimeTick (void)
{
	INT8U i;
#if OS_CPU_HOOKS_EN > 0
    OSTimeTickHook ();
#endif
	OS_ENTER_CRITICAL();
	for(i=0;i < OS_TASKS;i++){				             /*刷新各任务时钟	        	                */	
		 if(TCB[i].OSWaitTick){
			 TCB[i].OSWaitTick--;
			 if(TCB[i].OSWaitTick == 0){		         /*当任务时钟到时,必须是由定时器减时的才行      */			
				 OSRdyTbl |= (0x01<<i);		             /*将延时到的程序就需表置位	                    */				
			 }
		 }
	}
	OS_EXIT_CRITICAL();
}
/*********************************************************************************************************
函数名称：	OSTimeDly
函数原型：	void OSTimeDly(INT16U ticks)
函数功能：	任务延时 
入口参数：	延时的时间，系统节拍的个数
有关说明：		
*********************************************************************************************************/
void OSTimeDly(INT16U ticks)
{
    if(OSIntNesting > 0){                               /*中断 不得调用                                 */
       return;
    }         
    if(ticks){                             	            /*当延时有效                                    */	
	   OS_ENTER_CRITICAL();
	   OSRdyTbl &= ~(0x01<<OSPrioCur);	                /*把任务从就绪表中去掉                          */
	   TCB[OSPrioCur].OSWaitTick=ticks;
	   OS_EXIT_CRITICAL();
	   OS_Sched();                                       /*重新调度	                                    */
	}
}
/*********************************************************************************************************
函数名称：	OSTimeDlyHMSM
函数原型：	OSTimeDlyHMSM (INT8U hours, INT8U minutes, INT8U seconds, INT16U ms)
函数功能：	任务延时 
入口参数：	延时的时间，小时分秒毫秒
有关说明：		
*********************************************************************************************************/
#if OS_TIME_DLY_HMSM_EN > 0
INT8U  OSTimeDlyHMSM (INT8U hours, INT8U minutes, INT8U seconds, INT16U ms)
{
    INT32U ticks;
    INT16U loops;                                                
    ticks = ((INT32U)hours * 3600L + (INT32U)minutes * 60L + (INT32U)seconds) * OS_TICKS_PER_SEC
          + OS_TICKS_PER_SEC * ((INT32U)ms + 500L / OS_TICKS_PER_SEC) / 1000L;
    loops = (INT16U)(ticks >> 16);               
    ticks = ticks & 0xFFFFL;                     
    OSTimeDly((INT16U)ticks);
    while (loops > 0) {
        OSTimeDly((INT16U)32768u);
        OSTimeDly((INT16U)32768u);
        loops--;
    }
    return OS_ERR_NONE;
}
#endif
/*********************************************************************************************************
函数名称：	OSSemCreate
函数原型：	void OSSemCreate(OS_SEM *pevent,INT8U cnt)
函数功能：	初始化信号量 
入口参数：	pevent：信号量指针；cnt：信号量初值	    
*********************************************************************************************************/
#if OS_SEM_CREATE_EN >0
INT8U OSSemCreate(OS_SEM *pevent,INT8U cnt)
{
	pevent->OSSemCnt=cnt;                               /*定义信号计数器初值                            */
	pevent->OSSemPendTbl=0;
	return OS_ERR_NONE;				
}
#endif
/*********************************************************************************************************
函数名称：	OSSemPend
函数原型：	INT8U OSSemPend(OS_SEM *pevent,INT16U timeout,INT8U *perr) 
函数功能：	任务等待信号量,挂起 
入口参数：	pevent:信号量指针；Timeout：超时时间
出口参数：	无
有关说明：	当Timeout==0时，为无限等待	 该函数不能在中断程序中使用
*********************************************************************************************************/
#if OS_SEM_PEND_EN >0
void  OSSemPend (OS_SEM *pevent, INT16U timeout, INT8U *perr)
{
    if(OSIntNesting>0){                                 /* 检查是否被中断调用                           */
        *perr = OS_ERR_PEND_ISR;                        
        return;
    }
    if(OSLockNesting>0){                                /* 检查调度是否被锁定                           */
        *perr = OS_ERR_PEND_LOCKED;                     
        return;
    }
    OS_ENTER_CRITICAL();
	if(pevent->OSSemCnt>0){                              					
        pevent->OSSemCnt--;				                /*信号量计数器减1                               */		
        OS_EXIT_CRITICAL();
		*perr = OS_ERR_NONE;
		return ;       
    }
	pevent->OSSemPendTbl |= 0x01<<OSPrioCur;            /*加入等待列表                                  */
	OSRdyTbl &= ~(0x01<<OSPrioCur);			            /*从任务就绪表中去除	                        */
	if(timeout ==0 ){        
        OS_EXIT_CRITICAL();	                            /*重新调度				                        */
		OS_Sched();
	    pevent->OSSemPendTbl &= ~(0x01<<OSPrioCur);
		*perr = OS_ERR_NONE;
		return ;	 
    }
	TCB[OSPrioCur].OSWaitTick = timeout;	            /*定义等待超时	        	                    */
	OS_EXIT_CRITICAL();
	OS_Sched();	
	if(TCB[OSPrioCur].OSWaitTick == 0){
		pevent->OSSemPendTbl &= ~(0x01<<OSPrioCur);
		*perr = OS_ERR_TIMEOUT;
        return ;			 
 	}      
}
#endif
/*********************************************************************************************************
函数名称：	OSSemAccept
函数原型：	INT8U OSSemAccept(OS_SEM *pevent) 
函数功能：	无等待请求一个信号量 
入口参数：	pevent:信号量指针 该函数能在中断程序中使用
*********************************************************************************************************/
#if OS_SEM_ACCEPT_EN >0
INT8U OSSemAccept(OS_SEM *pevent)
{
    INT8U cnt;
    OS_ENTER_CRITICAL();
    cnt=pevent->OSSemCnt;
	if(cnt>0){				                           /*如果信号量计数器大于0	                        */	
		pevent->OSSemCnt--;				               /*信号量计数器减1                                */		       
	}
	OS_EXIT_CRITICAL();
	return (cnt); 
}
#endif
/*********************************************************************************************************
函数名称：	OSSemPost
函数原型：	INT8U OSSemPost(OS_SEM *pevent) 
函数功能：	发送一个信号量，可以从任务或中断发送，
入口参数：	pevent：信号量的指针
有关说明：	如果有任务在等待该信号量则将该任务就绪，没有任务等待则仅仅是把信号量+1	
*********************************************************************************************************/
#if OS_SEM_POST_EN >0
INT8U OSSemPost(OS_SEM *pevent)
{
    INT8U i;
    OS_ENTER_CRITICAL();
    if(pevent->OSSemPendTbl != 0){                    /*如果有任务等待该信号量                         */    	   
	    i=0;
	    while((i<OS_TASKS)&&(!(pevent->OSSemPendTbl&(0x01<<i)))){
		 	    i++;                                  /*找出信号量等待列表中任务优先级最高的任务        */
	       }
	    if(i<OS_TASKS){							      /*如果有任务需要		     	                    */		   	
		    pevent->OSSemPendTbl &= ~(0x01<<i);	      /*从等待表中去除		                            */
			OSRdyTbl |= 0x01<<i;				      /*任务就绪					                    */
            if(OSPrioCur>i){                          /*如果等待该信号的任务优先级大于当前任务          */	     
			    OS_EXIT_CRITICAL(); 
                OS_Sched();                            /*重新调度                                        */
                return (OS_ERR_NONE);
			}			
	    }     
	}
    if(pevent->OSSemCnt<255u){
	    pevent->OSSemCnt++;				               /*使信号量计数器加1			          	        */
	    OS_EXIT_CRITICAL();
	    return (OS_ERR_NONE);
    }
	OS_EXIT_CRITICAL();
	return (OS_ERR_NONE);
}
#endif
/********************************************************************************************************
函数名称：	OSMboxCreate
函数原型：	INT8U OSMboxCreate(OS_MBOX *pevent);
函数功能：	初始化邮箱 
入口参数：	pevent：邮箱指针
*********************************************************************************************************/
#if OS_MBOX_CREATE_EN >0
INT8U OSMboxCreate(OS_MBOX *pevent)
{
	pevent->OSMboxPtr=(void *)0;                       /*定义邮箱指针初值                               */
	pevent->OSMboxPendTbl=0;                           /*定义邮箱等待列表为空                           */
	pevent->OSMboxStatus=OS_MBOX_EMPTY;                /*定义邮箱状态为空                               */
	return (OS_ERR_NONE);				
}
#endif
/*********************************************************************************************************
函数名称：	OSMboxPend
函数原型：	void *OSMboxPend(OS_MBOX *pevent,INT16U Timeout,INT8U *perr)
函数功能：	任务等待信号量,挂起 
入口参数：	pevent:邮箱指针； Timeout：等待时间;perr：错误返回代码
出口参数：	无
有关说明：	当Timeout==0时，为无限等待	 该函数不能在中断程序中使用
*********************************************************************************************************/
#if OS_MBOX_PEND_EN >0
void *OSMboxPend(OS_MBOX *pevent,INT16U Timeout,INT8U *perr)
{ 
    void *pmsg;
	if(OSIntNesting>0){                                    /* 检查是否被中断调用                        */
        *perr = OS_ERR_PEND_ISR;                        
        return ((void *)0);;
    }
    if(OSLockNesting>0){                                   /* 检查调度是否被锁定                        */
        *perr = OS_ERR_PEND_LOCKED;                     
        return ((void *)0);;
    }
    OS_ENTER_CRITICAL();
	if(pevent->OSMboxStatus==OS_MBOX_FULL){                /*如果邮箱中有信息                           */	                              					
        pmsg=pevent->OSMboxPtr;				               /*获取信息                                   */
        pevent->OSMboxPtr=(void *)0;                       /*清除邮箱内容                               */
		pevent->OSMboxStatus=OS_MBOX_EMPTY;	   	           /*邮箱标志设为空                             */
		pevent->OSMboxPendTbl &= ~(0x01<<OSPrioCur);       /*清除邮箱等待列表                           */
        OS_EXIT_CRITICAL(); 
		*perr = OS_ERR_NONE;
		return (pmsg);      
    }
	pevent->OSMboxPendTbl |= 0x01<<OSPrioCur;              /*加入邮箱等待列表                           */
	OSRdyTbl &= ~(0x01<<OSPrioCur);			               /*从任务就绪表中去除	                        */
	if(Timeout==0){                                        /*如果超时则永久等待，直到被被任务唤醒       */	            
        OS_EXIT_CRITICAL();	                               /*重新调度				                    */
		OS_Sched();
    }else{
        TCB[OSPrioCur].OSWaitTick = Timeout;	           /*定义等待超时	        	                */
	    OS_EXIT_CRITICAL();
        OS_Sched();			
	    if(TCB[OSPrioCur].OSWaitTick == 0){                /*判断是否超时                               */
		    pevent->OSMboxPendTbl &= ~(0x01<<OSPrioCur);   /*清除邮箱等待列表                           */
            *perr = OS_ERR_TIMEOUT;
		    return ((void *)0);
	    }
	}
    pmsg=pevent->OSMboxPtr;                                /*获取邮箱内容                               */
	pevent->OSMboxPtr=(void *)0;		                   /*清除邮箱内容                               */		             
	pevent->OSMboxStatus=OS_MBOX_EMPTY;                    /*邮箱标志设为空                             */
	pevent->OSMboxPendTbl &= ~(0x01<<OSPrioCur);           /*清除邮箱等待列表                           */
	OS_EXIT_CRITICAL();	
	return (pmsg);					       
}
#endif
/*********************************************************************************************************
函数名称：	OSMboxAccept
函数原型：	void *OSMboxAccept(OS_MBOX *pevent) 
函数功能：	无等待请求一个邮箱 
入口参数：	pevent:邮箱指针
*********************************************************************************************************/
#if OS_MBOX_ACCEPT_EN >0
void *OSMboxAccept(OS_MBOX *pevent)
{
    void *pmsg;
    OS_ENTER_CRITICAL();
	pmsg=pevent->OSMboxPtr;			                  /*取得信息                                        */	
    pevent->OSMboxPtr=(void *)0;
	pevent->OSMboxPendTbl &= ~(0x01<<OSPrioCur);      /*清除等待列表                                    */	
	OS_EXIT_CRITICAL();
	return (pmsg);
}
#endif
/*********************************************************************************************************
函数名称：	OSMboxPost
函数原型：	INT8U OSMboxPost(OS_MBOX *pevent,void *pmsg )
函数功能：	发送一个邮箱，可以从任务或中断发送，
入口参数：	pevent：邮箱指针 pmsg：邮箱内容指针
有关说明：	首先判断邮箱是否为空，是则发送消息并调度
*********************************************************************************************************/
#if OS_MBOX_POST_EN >0
INT8U OSMboxPost(OS_MBOX *pevent,void *pmsg )
{
    INT8U i;
    OS_ENTER_CRITICAL();
    if(pevent->OSMboxStatus==OS_MBOX_EMPTY){
        pevent->OSMboxPtr=pmsg;
        pevent->OSMboxStatus=OS_MBOX_FULL;
        if(pevent->OSMboxPendTbl!=0){                /*如果有任务等待该信号量                           */     
	        i=0;
	        while((i<OS_TASKS)&&(!(pevent->OSMboxPendTbl&(0x01<<i)))){
		        i++;                                 /*找出信号量等待列表中任务优先级最高的任务         */
	        }
	        if(i<OS_TASKS){					         /*如果有任务需要		                            */	
		        pevent->OSMboxPendTbl &= ~(0x01<<i); /*从等待表中去除		                            */
		        OSRdyTbl |= 0x01<<i;				 /*任务就绪					                        */
                if(OSPrioCur>i){                     /*如果等待该信号的任务优先级大于当前任务           */  
			        OS_EXIT_CRITICAL();
                    OS_Sched();                      /*重新调度                                         */
                    return (OS_ERR_NONE);
				}			
	        }     
	    }
		OS_EXIT_CRITICAL(); 
        return (OS_ERR_NONE);
    }
	OS_EXIT_CRITICAL();
	return (OS_ERR_MBOX_FULL);
}
#endif
/********************************************************************************************************
函数名称：	TCN0Init
函数原型：	void TCN0Init(void)
函数功能：	初始化定时器0
有关说明：	定时器T0是任务时钟	
*********************************************************************************************************/
void TCN0Init(void)
{
	TCCR0 = 0;
	TCCR0 = 0x05;			    /* 1024预分频		*/
	TIMSK |= (1<<TOIE0);		/* T0溢出中断允许	*/
	TCNT0 = (256-F_CPU/1024/OS_TICKS_PER_SEC);
}
SIGNAL(SIG_OVERFLOW0)
{    
	OSIntEnter();
    TCNT0 = (256-F_CPU/1024/OS_TICKS_PER_SEC);	
    OSTimeTick();	
	OSIntExit();
}
#if OS_CPU_HOOKS_EN > 0
void  OSTimeTickHook (void){}
void  OSTaskIdleHook (void){}
void  OSInitHookBegin (void){}
#endif

