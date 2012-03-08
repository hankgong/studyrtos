/*********************************************************************************************************
*                                      The Real-Time Kernel For AVR
*                                             CORE FUNCTIONS
* File    : AVR_RTOS.C
* By      : ����
* Version : V2.0
*********************************************************************************************************/
#include "AVR_RTOS.h"
/*********************************************************************************************************
�������ƣ�	OSTaskCreate
����ԭ�ͣ�	void OSTaskCreate(void (*Task)(void),INT8U *Stack,INT8U prio)
�������ܣ�	�û��������������� 
��ڲ�����	*Task����������ַ��*Stack�������ջָ�룻prio���������ȼ�
*********************************************************************************************************/
void OSTaskCreate(void (*Task)(void),INT8U *Stack,INT8U prio)
{
	INT8U i;
	*Stack--=(INT16U)Task;				     /*������ĵ�ַ��λѹ���ջ                                 */
	*Stack--=(INT16U)Task>>8;		 	     /*������ĵ�ַ��λѹ���ջ	                                */
	*Stack--=0x00;							 /*R1 __zero_reg__				                            */
	*Stack--=0x00;							 /*R0 __tmp_reg__			                                */
	*Stack--=0x80;							 /*SREG �������У�����ȫ���ж�	                            */
	for(i=0;i<14;i++)
    *Stack--=i;/*�� avr-libc �е� FAQ�е� What registers are used by the C compiler?�����˼Ĵ���������	*/
	TCB[prio].OSTaskStackTop = (INT16U)Stack;/*���˹���ջ��ջ�������浽��ջ��������                     */
    TCB[prio].OSWaitTick     = 0;			 /* ��ʼ��������ʱ    	                                    */
	OSRdyTbl                |= 0x01<<prio;   /*����������Ѿ�׼����		      	                        */
}
/*********************************************************************************************************
�������ƣ�	OSInit
����ԭ�ͣ�	void OSInit (void)
�������ܣ�	�û���������ʼ������ʱ�Ӻ�һЩϵͳȫ�ֱ�����������������	
*********************************************************************************************************/
void OSInit (void)
{
	OSRdyTbl      = 0;                            /*�������������                                      */
	OSIntNesting  = 0;	                          /*�ж�Ƕ�׼���������                                  */
	OSLockNesting = 0;
	OSIdleCtr     = 0;
#if OS_CPU_HOOKS_EN > 0
    OSInitHookBegin ();
#endif
	OSTaskCreate(IdleTask,&IdleStack[IDLE_STACK_SIZE-1],IDLE_TASK_PRIO); /*����IDLE����                 */
}
/*********************************************************************************************************
�������ƣ�	OSStart
����ԭ�ͣ�	void OSStart(void)
�������ܣ�	�û���������ʼ�������,�ӿ�������ʼ���� 	
*********************************************************************************************************/
void OSStart(void)
{
	OSPrioCur     = OS_TASKS;
	OSPrioHighRdy = OS_TASKS;
	SP=TCB[OS_TASKS].OSTaskStackTop+17;
	__asm__ __volatile__("reti"                    "\n\t");
}
/*********************************************************************************************************
�������ƣ�	OS_TASK_SW
����ԭ�ͣ�	void OS_TASK_SW(void)
�������ܣ�	����������� 
�й�˵����	�ڲ��������жϺ����񶼿��Ե������������Ⱥ���	
*********************************************************************************************************/
void OS_TASK_SW(void)
{
	__asm__ __volatile__("PUSH __zero_reg__         \n\t");	/*R1 					                    */
	__asm__ __volatile__("PUSH __tmp_reg__          \n\t");	/*R0					                    */
	__asm__ __volatile__("IN   __tmp_reg__,__SREG__ \n\t");	/*����״̬�Ĵ���SREG               	        */
	__asm__ __volatile__("PUSH __tmp_reg__          \n\t"); 
	__asm__ __volatile__("CLR  __zero_reg__         \n\t");	/*R0��������			                    */
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

	__asm__ __volatile__("Int_OSSched:              \n\t");	/*���ж�Ҫ����ȣ�ֱ�ӽ�������		        */ 
    __asm__ __volatile__("CLI                       \n\t");                
	__asm__ __volatile__("PUSH R28                  \n\t");	/*R28��R29���ڽ����ڶ�ջ�ϵ�ָ��	        */
	__asm__ __volatile__("PUSH R29                  \n\t");	/*��ջ���							        */	
	
	TCB[OSPrioCur].OSTaskStackTop=SP;					    /*���������е�����Ķ�ջ�ױ���		        */	
	OSPrioCur = OSPrioHighRdy ;							    /*���е�ǰ�������е�������ȼ�����	        */	
	SP=TCB[OSPrioCur].OSTaskStackTop;

	__asm__ __volatile__("POP  R29                  \n\t"); /*��ջ                                      */
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
	__asm__ __volatile__("POP  __tmp_reg__          \n\t"); /*SERG ��ջ���ָ�	                        */
	__asm__ __volatile__("OUT  __SREG__,__tmp_reg__ \n\t");
	__asm__ __volatile__("POP  __tmp_reg__          \n\t");	/*R0 ��ջ			                        */
	__asm__ __volatile__("POP  __zero_reg__         \n\t");	/*R1 ��ջ			                        */	                                                      
	__asm__ __volatile__("RETI                      \n\t"); /*��ջ���,���ز����ж�		                */
}
/*********************************************************************************************************
�������ƣ�	OSIntCtxSw
����ԭ�ͣ�	void OSIntCtxSw(void)
�������ܣ�	���ж��˳������е���
�й�˵����	�����ж�Ƕ�ײ����ж���Ҫ����������л�ʱ�Ž��������л���
			��Ϊ���жϵ�������һ�����񹲵����������Ӻ���������Ҫ�����ĸ���ջ��PC
*********************************************************************************************************/
void OSIntCtxSw(void)
{                                
	__asm__ __volatile__("POP  R31                  \n\t");	    /*ȥ��������ӳ������ջ��PC            */
	__asm__ __volatile__("POP  R31                  \n\t");
	__asm__ __volatile__("POP  R31                  \n\t");
	__asm__ __volatile__("POP  R31                  \n\t");
	__asm__ __volatile__("RJMP Int_OSSched          \n\t");	    /*���µ���					            */	
}
/*********************************************************************************************************
�������ƣ�	OSSched
����ԭ�ͣ�	void OSSched(void)
�������ܣ�	����������������������
�й�˵����	�ڲ�������ֻ���и������ȼ����������ʱ�Ž���һ�������л����������л����ж��в��ɵ��ô˺���	
*********************************************************************************************************/
void OS_Sched (void)
{
    OS_ENTER_CRITICAL();
	if(OSIntNesting == 0){                                 /*�ж��Ƿ����жϵ��ã�����ֱ���Ƴ�           */   	
	    if(OSLockNesting == 0){                            /*�жϵ����Ƿ�����������ֱ���Ƴ�             */
		    OS_SchedNew();						           /*�ҳ������������ȼ���ߵ�����               */
	        if(OSPrioHighRdy != OSPrioCur){				   /*������ǵ�ǰ���е����񣬽����������       */	    
		        OS_TASK_SW();							   /*��������                                   */    
		    }
		}
    }
	OS_EXIT_CRITICAL();
}
/*********************************************************************************************************
�������ƣ�	OSTaskSuspend
����ԭ�ͣ�	void OSTaskSuspend(INT8U prio)
�������ܣ�	�������� 
��ڲ�����	prio���������ȼ�
�й�˵����	�û�������һ��������Թ�����Ҳ���Թ�����������	
*********************************************************************************************************/
#if OS_TASK_SUSPEND_EN >0
void OSTaskSuspend(INT8U prio)
{
	OS_ENTER_CRITICAL();
	TCB[prio].OSWaitTick=0;         
	OSRdyTbl &= ~(0x01<<prio);		                       /*�������������ȥ����־λ	                */
	if(OSPrioCur == prio){			                       /*��Ҫ���������Ϊ��ǰ����	                */	
		OS_EXIT_CRITICAL();
		OS_Sched();					                       /*���µ���					                */
		return;
	}
	OS_EXIT_CRITICAL();
}
#endif
/*********************************************************************************************************
�������ƣ�	OSTaskResume
����ԭ�ͣ�	void OSTaskResume(INT8U prio)
�������ܣ�	�û��������ָ����� �����ñ�OSTaskSuspend�� OSTimeDly��ͣ������ָ� 
��ڲ�����	prio���������ȼ�	
*********************************************************************************************************/
#if OS_TASK_RESUME_EN > 0
void OSTaskResume(INT8U prio)
{
	OS_ENTER_CRITICAL();
	OSRdyTbl |= 0x01<<prio;			                       /*����������������ñ�־λ	                */
    TCB[prio].OSWaitTick=0;			                       /*��ʱ���ʱ��Ϊ0,��ʱ		                */
	if(OSPrioCur > prio){			                       /*��ǰ��������ȼ���������λ����������ȼ�	*/	
		OS_EXIT_CRITICAL();
		OS_Sched();					                       /*���µ���				             	    */
	    return;
	}
	OS_EXIT_CRITICAL();
}
#endif
/*********************************************************************************************************
�������ƣ�	IdleTask(void)
����ԭ�ͣ�	void IdleTask(void)
�������ܣ�	�������񣬵���������û�о�����ʱ������
�й�˵����	ϵͳ��ʼ���е�ʱ���Ǵ����п�������ʼ�ġ���������ɾ��
���ｫtimer0���ڴ˴�����Ϊ�ú����ǵ�һ�����еĺ�����ģ��UCos�ڵ�һ������������ϵͳ����
*********************************************************************************************************/
void IdleTask(void)
{   
    TCN0Init();                                            /*��ʼ����ʱ��                               */
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
�������ƣ�	OS_SchedNew
����ԭ�ͣ�	void OS_SchedNew (void)
�������ܣ�	�ҳ�����������е�������ȼ�������
*********************************************************************************************************/
void OS_SchedNew (void)
{   
	INT8U OSNextTaskPrio;
	OSNextTaskPrio = 0;
	while ((OSNextTaskPrio < OS_TASKS) && (!(OSRdyTbl & (0x01<<OSNextTaskPrio)))){
		OSNextTaskPrio++;                                 /*�ҳ��ź����ȴ��б����������ȼ���ߵ�����    */
	}
	OSPrioHighRdy  =  OSNextTaskPrio;
}
/*********************************************************************************************************
�������ƣ�	OSIntEnter
����ԭ�ͣ�	void OSIntEnter(void)
�������ܣ�	�����ж�ʱ���ô˺���
�й�˵����	OSIntEnter()��OSIntExit()Ҫ�ɶԳ���ʹ��
*********************************************************************************************************/
void OSIntEnter(void)
{
	OS_ENTER_CRITICAL();
	if(OSIntNesting<255u){
		OSIntNesting++;     				             /*�ж�Ƕ�׼�1    			                    */
	}
	OS_EXIT_CRITICAL();
}
/*********************************************************************************************************
�������ƣ�	OSIntExit
����ԭ�ͣ�	void OSIntExit(void)
�������ܣ�	���ж����˳�ʱ���ô˺���������ж��ø������ȼ�����������ͽ����������
�й�˵����	OSIntEnter()��OSIntExit()Ҫ�ɶԳ���ʹ��	
*********************************************************************************************************/
void OSIntExit(void)
{
	OS_ENTER_CRITICAL();
	if(OSIntNesting>0){
		OSIntNesting--;     				             /*�ж�Ƕ�׼�1			                        */
	}
	if(OSIntNesting == 0){
	    if(OSLockNesting == 0){
		    OS_SchedNew();				                 /*�ҳ������������ȼ���ߵ�����                 */
		    if(OSPrioHighRdy != OSPrioCur){	
			    OS_EXIT_CRITICAL();	   	
			    OSIntCtxSw();					        /*�����������            	                    */	
		        return;
		    }
	    }
	}
	OS_EXIT_CRITICAL();
}
/*********************************************************************************************************
�������ƣ�	OSSchedLock  &  OSSchedUnlock
����ԭ�ͣ�	void  OSSchedLock (void)  void  OSSchedUnlock (void)
�������ܣ�	�����������
�й�˵����	OSSchedLock ()  OSSchedUnlock ()Ҫ�ɶԳ���ʹ��	
*********************************************************************************************************/
#if OS_SCHED_LOCK_EN > 0
void  OSSchedLock (void)
{
    OS_ENTER_CRITICAL();
    if (OSIntNesting == 0) {                             /* �жϲ��õ���                                */
         if(OSLockNesting < 255u){          
             OSLockNesting++;                            /* ����Ƕ�׼�����+1                            */
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
�������ƣ�	OSTimeTick (void)
����ԭ�ͣ�	OSTimeTick (void)
�������ܣ�	ϵͳ���ķ�����
�й�˵����	����Ϊ��Ҫ��ʱ���������������ʱ������ϵͳʱ��	
*********************************************************************************************************/
void  OSTimeTick (void)
{
	INT8U i;
#if OS_CPU_HOOKS_EN > 0
    OSTimeTickHook ();
#endif
	OS_ENTER_CRITICAL();
	for(i=0;i < OS_TASKS;i++){				             /*ˢ�¸�����ʱ��	        	                */	
		 if(TCB[i].OSWaitTick){
			 TCB[i].OSWaitTick--;
			 if(TCB[i].OSWaitTick == 0){		         /*������ʱ�ӵ�ʱ,�������ɶ�ʱ����ʱ�Ĳ���      */			
				 OSRdyTbl |= (0x01<<i);		             /*����ʱ���ĳ���������λ	                    */				
			 }
		 }
	}
	OS_EXIT_CRITICAL();
}
/*********************************************************************************************************
�������ƣ�	OSTimeDly
����ԭ�ͣ�	void OSTimeDly(INT16U ticks)
�������ܣ�	������ʱ 
��ڲ�����	��ʱ��ʱ�䣬ϵͳ���ĵĸ���
�й�˵����		
*********************************************************************************************************/
void OSTimeDly(INT16U ticks)
{
    if(OSIntNesting > 0){                               /*�ж� ���õ���                                 */
       return;
    }         
    if(ticks){                             	            /*����ʱ��Ч                                    */	
	   OS_ENTER_CRITICAL();
	   OSRdyTbl &= ~(0x01<<OSPrioCur);	                /*������Ӿ�������ȥ��                          */
	   TCB[OSPrioCur].OSWaitTick=ticks;
	   OS_EXIT_CRITICAL();
	   OS_Sched();                                       /*���µ���	                                    */
	}
}
/*********************************************************************************************************
�������ƣ�	OSTimeDlyHMSM
����ԭ�ͣ�	OSTimeDlyHMSM (INT8U hours, INT8U minutes, INT8U seconds, INT16U ms)
�������ܣ�	������ʱ 
��ڲ�����	��ʱ��ʱ�䣬Сʱ�������
�й�˵����		
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
�������ƣ�	OSSemCreate
����ԭ�ͣ�	void OSSemCreate(OS_SEM *pevent,INT8U cnt)
�������ܣ�	��ʼ���ź��� 
��ڲ�����	pevent���ź���ָ�룻cnt���ź�����ֵ	    
*********************************************************************************************************/
#if OS_SEM_CREATE_EN >0
INT8U OSSemCreate(OS_SEM *pevent,INT8U cnt)
{
	pevent->OSSemCnt=cnt;                               /*�����źż�������ֵ                            */
	pevent->OSSemPendTbl=0;
	return OS_ERR_NONE;				
}
#endif
/*********************************************************************************************************
�������ƣ�	OSSemPend
����ԭ�ͣ�	INT8U OSSemPend(OS_SEM *pevent,INT16U timeout,INT8U *perr) 
�������ܣ�	����ȴ��ź���,���� 
��ڲ�����	pevent:�ź���ָ�룻Timeout����ʱʱ��
���ڲ�����	��
�й�˵����	��Timeout==0ʱ��Ϊ���޵ȴ�	 �ú����������жϳ�����ʹ��
*********************************************************************************************************/
#if OS_SEM_PEND_EN >0
void  OSSemPend (OS_SEM *pevent, INT16U timeout, INT8U *perr)
{
    if(OSIntNesting>0){                                 /* ����Ƿ��жϵ���                           */
        *perr = OS_ERR_PEND_ISR;                        
        return;
    }
    if(OSLockNesting>0){                                /* �������Ƿ�����                           */
        *perr = OS_ERR_PEND_LOCKED;                     
        return;
    }
    OS_ENTER_CRITICAL();
	if(pevent->OSSemCnt>0){                              					
        pevent->OSSemCnt--;				                /*�ź�����������1                               */		
        OS_EXIT_CRITICAL();
		*perr = OS_ERR_NONE;
		return ;       
    }
	pevent->OSSemPendTbl |= 0x01<<OSPrioCur;            /*����ȴ��б�                                  */
	OSRdyTbl &= ~(0x01<<OSPrioCur);			            /*�������������ȥ��	                        */
	if(timeout ==0 ){        
        OS_EXIT_CRITICAL();	                            /*���µ���				                        */
		OS_Sched();
	    pevent->OSSemPendTbl &= ~(0x01<<OSPrioCur);
		*perr = OS_ERR_NONE;
		return ;	 
    }
	TCB[OSPrioCur].OSWaitTick = timeout;	            /*����ȴ���ʱ	        	                    */
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
�������ƣ�	OSSemAccept
����ԭ�ͣ�	INT8U OSSemAccept(OS_SEM *pevent) 
�������ܣ�	�޵ȴ�����һ���ź��� 
��ڲ�����	pevent:�ź���ָ�� �ú��������жϳ�����ʹ��
*********************************************************************************************************/
#if OS_SEM_ACCEPT_EN >0
INT8U OSSemAccept(OS_SEM *pevent)
{
    INT8U cnt;
    OS_ENTER_CRITICAL();
    cnt=pevent->OSSemCnt;
	if(cnt>0){				                           /*����ź�������������0	                        */	
		pevent->OSSemCnt--;				               /*�ź�����������1                                */		       
	}
	OS_EXIT_CRITICAL();
	return (cnt); 
}
#endif
/*********************************************************************************************************
�������ƣ�	OSSemPost
����ԭ�ͣ�	INT8U OSSemPost(OS_SEM *pevent) 
�������ܣ�	����һ���ź��������Դ�������жϷ��ͣ�
��ڲ�����	pevent���ź�����ָ��
�й�˵����	����������ڵȴ����ź����򽫸����������û������ȴ�������ǰ��ź���+1	
*********************************************************************************************************/
#if OS_SEM_POST_EN >0
INT8U OSSemPost(OS_SEM *pevent)
{
    INT8U i;
    OS_ENTER_CRITICAL();
    if(pevent->OSSemPendTbl != 0){                    /*���������ȴ����ź���                         */    	   
	    i=0;
	    while((i<OS_TASKS)&&(!(pevent->OSSemPendTbl&(0x01<<i)))){
		 	    i++;                                  /*�ҳ��ź����ȴ��б����������ȼ���ߵ�����        */
	       }
	    if(i<OS_TASKS){							      /*�����������Ҫ		     	                    */		   	
		    pevent->OSSemPendTbl &= ~(0x01<<i);	      /*�ӵȴ�����ȥ��		                            */
			OSRdyTbl |= 0x01<<i;				      /*�������					                    */
            if(OSPrioCur>i){                          /*����ȴ����źŵ��������ȼ����ڵ�ǰ����          */	     
			    OS_EXIT_CRITICAL(); 
                OS_Sched();                            /*���µ���                                        */
                return (OS_ERR_NONE);
			}			
	    }     
	}
    if(pevent->OSSemCnt<255u){
	    pevent->OSSemCnt++;				               /*ʹ�ź�����������1			          	        */
	    OS_EXIT_CRITICAL();
	    return (OS_ERR_NONE);
    }
	OS_EXIT_CRITICAL();
	return (OS_ERR_NONE);
}
#endif
/********************************************************************************************************
�������ƣ�	OSMboxCreate
����ԭ�ͣ�	INT8U OSMboxCreate(OS_MBOX *pevent);
�������ܣ�	��ʼ������ 
��ڲ�����	pevent������ָ��
*********************************************************************************************************/
#if OS_MBOX_CREATE_EN >0
INT8U OSMboxCreate(OS_MBOX *pevent)
{
	pevent->OSMboxPtr=(void *)0;                       /*��������ָ���ֵ                               */
	pevent->OSMboxPendTbl=0;                           /*��������ȴ��б�Ϊ��                           */
	pevent->OSMboxStatus=OS_MBOX_EMPTY;                /*��������״̬Ϊ��                               */
	return (OS_ERR_NONE);				
}
#endif
/*********************************************************************************************************
�������ƣ�	OSMboxPend
����ԭ�ͣ�	void *OSMboxPend(OS_MBOX *pevent,INT16U Timeout,INT8U *perr)
�������ܣ�	����ȴ��ź���,���� 
��ڲ�����	pevent:����ָ�룻 Timeout���ȴ�ʱ��;perr�����󷵻ش���
���ڲ�����	��
�й�˵����	��Timeout==0ʱ��Ϊ���޵ȴ�	 �ú����������жϳ�����ʹ��
*********************************************************************************************************/
#if OS_MBOX_PEND_EN >0
void *OSMboxPend(OS_MBOX *pevent,INT16U Timeout,INT8U *perr)
{ 
    void *pmsg;
	if(OSIntNesting>0){                                    /* ����Ƿ��жϵ���                        */
        *perr = OS_ERR_PEND_ISR;                        
        return ((void *)0);;
    }
    if(OSLockNesting>0){                                   /* �������Ƿ�����                        */
        *perr = OS_ERR_PEND_LOCKED;                     
        return ((void *)0);;
    }
    OS_ENTER_CRITICAL();
	if(pevent->OSMboxStatus==OS_MBOX_FULL){                /*�������������Ϣ                           */	                              					
        pmsg=pevent->OSMboxPtr;				               /*��ȡ��Ϣ                                   */
        pevent->OSMboxPtr=(void *)0;                       /*�����������                               */
		pevent->OSMboxStatus=OS_MBOX_EMPTY;	   	           /*�����־��Ϊ��                             */
		pevent->OSMboxPendTbl &= ~(0x01<<OSPrioCur);       /*�������ȴ��б�                           */
        OS_EXIT_CRITICAL(); 
		*perr = OS_ERR_NONE;
		return (pmsg);      
    }
	pevent->OSMboxPendTbl |= 0x01<<OSPrioCur;              /*��������ȴ��б�                           */
	OSRdyTbl &= ~(0x01<<OSPrioCur);			               /*�������������ȥ��	                        */
	if(Timeout==0){                                        /*�����ʱ�����õȴ���ֱ������������       */	            
        OS_EXIT_CRITICAL();	                               /*���µ���				                    */
		OS_Sched();
    }else{
        TCB[OSPrioCur].OSWaitTick = Timeout;	           /*����ȴ���ʱ	        	                */
	    OS_EXIT_CRITICAL();
        OS_Sched();			
	    if(TCB[OSPrioCur].OSWaitTick == 0){                /*�ж��Ƿ�ʱ                               */
		    pevent->OSMboxPendTbl &= ~(0x01<<OSPrioCur);   /*�������ȴ��б�                           */
            *perr = OS_ERR_TIMEOUT;
		    return ((void *)0);
	    }
	}
    pmsg=pevent->OSMboxPtr;                                /*��ȡ��������                               */
	pevent->OSMboxPtr=(void *)0;		                   /*�����������                               */		             
	pevent->OSMboxStatus=OS_MBOX_EMPTY;                    /*�����־��Ϊ��                             */
	pevent->OSMboxPendTbl &= ~(0x01<<OSPrioCur);           /*�������ȴ��б�                           */
	OS_EXIT_CRITICAL();	
	return (pmsg);					       
}
#endif
/*********************************************************************************************************
�������ƣ�	OSMboxAccept
����ԭ�ͣ�	void *OSMboxAccept(OS_MBOX *pevent) 
�������ܣ�	�޵ȴ�����һ������ 
��ڲ�����	pevent:����ָ��
*********************************************************************************************************/
#if OS_MBOX_ACCEPT_EN >0
void *OSMboxAccept(OS_MBOX *pevent)
{
    void *pmsg;
    OS_ENTER_CRITICAL();
	pmsg=pevent->OSMboxPtr;			                  /*ȡ����Ϣ                                        */	
    pevent->OSMboxPtr=(void *)0;
	pevent->OSMboxPendTbl &= ~(0x01<<OSPrioCur);      /*����ȴ��б�                                    */	
	OS_EXIT_CRITICAL();
	return (pmsg);
}
#endif
/*********************************************************************************************************
�������ƣ�	OSMboxPost
����ԭ�ͣ�	INT8U OSMboxPost(OS_MBOX *pevent,void *pmsg )
�������ܣ�	����һ�����䣬���Դ�������жϷ��ͣ�
��ڲ�����	pevent������ָ�� pmsg����������ָ��
�й�˵����	�����ж������Ƿ�Ϊ�գ���������Ϣ������
*********************************************************************************************************/
#if OS_MBOX_POST_EN >0
INT8U OSMboxPost(OS_MBOX *pevent,void *pmsg )
{
    INT8U i;
    OS_ENTER_CRITICAL();
    if(pevent->OSMboxStatus==OS_MBOX_EMPTY){
        pevent->OSMboxPtr=pmsg;
        pevent->OSMboxStatus=OS_MBOX_FULL;
        if(pevent->OSMboxPendTbl!=0){                /*���������ȴ����ź���                           */     
	        i=0;
	        while((i<OS_TASKS)&&(!(pevent->OSMboxPendTbl&(0x01<<i)))){
		        i++;                                 /*�ҳ��ź����ȴ��б����������ȼ���ߵ�����         */
	        }
	        if(i<OS_TASKS){					         /*�����������Ҫ		                            */	
		        pevent->OSMboxPendTbl &= ~(0x01<<i); /*�ӵȴ�����ȥ��		                            */
		        OSRdyTbl |= 0x01<<i;				 /*�������					                        */
                if(OSPrioCur>i){                     /*����ȴ����źŵ��������ȼ����ڵ�ǰ����           */  
			        OS_EXIT_CRITICAL();
                    OS_Sched();                      /*���µ���                                         */
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
�������ƣ�	TCN0Init
����ԭ�ͣ�	void TCN0Init(void)
�������ܣ�	��ʼ����ʱ��0
�й�˵����	��ʱ��T0������ʱ��	
*********************************************************************************************************/
void TCN0Init(void)
{
	TCCR0 = 0;
	TCCR0 = 0x05;			    /* 1024Ԥ��Ƶ		*/
	TIMSK |= (1<<TOIE0);		/* T0����ж�����	*/
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

