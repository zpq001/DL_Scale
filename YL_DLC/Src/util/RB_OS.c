//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_OS.c
//! \ingroup	util
//! \brief		This module contains the operating system functionality.
//!
//! This operating system supports a "cooperative multitasking mode".
//! The cooperative multitasking is implemented due to hard RAM constraints. The advantage is,
//! that all task are running on the same stack. It is a kind of polling mode, but with some
//! realtime like features:
//!
//! The cooperative multitasking provides not a real task switching, but the tasks are called one
//! after the other from the main program level.
//!
//! A pending task is not called until a message is received.
//!
//! A delayed task is not called until the delay time is over.
//!
//! Task with lower priorities (higher number) are called only, if no tasks with higher
//! priorities are ready. Task priorities are in the range of 0..255, where 0 is the highest.
//!
//! SW design hints:
//! - If two tasks require same events, two separate message queues are required
//! - If a task requires the same events in two different locations:
//!   ... read the events in a single location and store it in a static variable
//!   ... or create a new logical message queue
//!
//! \attention RB_OS_MsgQueuePend must never be called from interrupt because it may suspend an
//!            arbitrary task! RB_OS_MsgQueuePend will always suspend the task if no message was
//!            available, which will be the task that was running when the interrupt happened!
//!            In case and interrupt has to read from a message queue, RB_OS_MsgQueueAccept must
//!            be used instead of RB_OS_MsgQueuePend!
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author	Werner Langenegger, Martin Heusser
//
// $Date: 2017/05/24 08:17:18MESZ $
// $Revision: 1.103 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_OS"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_OS.h"
// This module is mandatory and has no RB_CONFIG_USE, no check is needed here.

#include "RB_Config.h"
#include "RB_Debug.h"

#if defined(RB_CONFIG_USE_SYSVIEW) && (RB_CONFIG_USE_SYSVIEW == RB_CONFIG_YES)
	#include "SEGGER_SYSVIEW.h"
#endif


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

//! Queue of ready tasks
static RB_OS_tTask* m_pTaskQueue = NULL;

//! Running task
static RB_OS_tTask* m_pRunningTask = NULL;


//==================================================================================================
//  F O R W A R D   D E C L A R A T I O N S
//==================================================================================================

static RB_OS_tStatus TaskQueueInsert(RB_OS_tTask** ppQueue, RB_OS_tTask* pTask);
static RB_OS_tStatus TaskQueueRemove(RB_OS_tTask** ppQueue, const RB_OS_tTask* pTask);


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_OS_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialization of the operating system
//!
//! This function does nothing and may be called any time. The Rainbow operating system can only be
//! re-initialized with a full restart of the firmware \see RB_MAIN_Restart().
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
void RB_OS_Initialize(void)
{
}


//--------------------------------------------------------------------------------------------------
// RB_OS_TaskReschedule
//--------------------------------------------------------------------------------------------------
//! \brief	Reschedule the tasks.
//!
//! In "cooperative multitasking mode", this function must be called from main program, as often
//! as possible.
//!
//! \attention This function is called from DoMainLoop() within RB_Main.c.
//!
//! \attention Must not be called anywhere outside of RB_Main.
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
void RB_OS_TaskReschedule(void)
{
	// Get first ready task
	m_pRunningTask = m_pTaskQueue;

	// For all task in queue
	while (m_pRunningTask != NULL)
	{

		switch (m_pRunningTask->state)
		{
			case RB_OS_PENDING:
				// If message queue is empty then keep task pending otherwise run task
				if (RB_QUEUE_IsEmpty(m_pRunningTask->pMsgQueue))
				{
					break;
				}
				//lint -fallthrough
				// Intentionally fall through if there is a message in the message queue for the task
			case RB_OS_READY:
			case RB_OS_RUNNING:
				m_pRunningTask->state = RB_OS_RUNNING;
				m_pRunningTask->runTime = 0u;
				#if defined(RB_CONFIG_SYSVIEW_RECORD_RBOS_TASKS) && (RB_CONFIG_SYSVIEW_RECORD_RBOS_TASKS == RB_CONFIG_YES)
				#if defined(RB_CONFIG_SYSVIEW_RBOS_TASK_PRIORITY_THRESHOLD) && (RB_CONFIG_SYSVIEW_RBOS_TASK_PRIORITY_THRESHOLD < RB_OS_TASK_MIN_PRIO)
				if (m_pRunningTask->prio <= RB_CONFIG_SYSVIEW_RBOS_TASK_PRIORITY_THRESHOLD)
				#endif
				{
					SEGGER_SYSVIEW_OnTaskStartExec((unsigned)m_pRunningTask);
				}
				#endif
				m_pRunningTask->task(m_pRunningTask->argument);		// Call taskFunction
				#if defined(RB_CONFIG_SYSVIEW_RECORD_RBOS_TASKS) && (RB_CONFIG_SYSVIEW_RECORD_RBOS_TASKS == RB_CONFIG_YES)
				#if defined(RB_CONFIG_SYSVIEW_RBOS_TASK_PRIORITY_THRESHOLD) && (RB_CONFIG_SYSVIEW_RBOS_TASK_PRIORITY_THRESHOLD < RB_OS_TASK_MIN_PRIO)
				if (m_pRunningTask->prio <= RB_CONFIG_SYSVIEW_RBOS_TASK_PRIORITY_THRESHOLD)
				#endif
				{
					SEGGER_SYSVIEW_OnTaskStopReady((unsigned)m_pRunningTask, m_pRunningTask->state);
				}
				#endif
				if (m_pRunningTask->runTime > m_pRunningTask->runPeak)
				{
					m_pRunningTask->runPeak = m_pRunningTask->runTime;
				}
				if (m_pRunningTask->state == RB_OS_RUNNING)
				{
					m_pRunningTask->state = RB_OS_READY;
					#if defined(RB_CONFIG_SYSVIEW_RECORD_RBOS_TASKS) && (RB_CONFIG_SYSVIEW_RECORD_RBOS_TASKS == RB_CONFIG_YES)
					#if defined(RB_CONFIG_SYSVIEW_RECORD_RBOS_TASKS_READY) && (RB_CONFIG_SYSVIEW_RECORD_RBOS_TASKS_READY == RB_CONFIG_YES)
					#if defined(RB_CONFIG_SYSVIEW_RBOS_TASK_PRIORITY_THRESHOLD) && (RB_CONFIG_SYSVIEW_RBOS_TASK_PRIORITY_THRESHOLD < RB_OS_TASK_MIN_PRIO)
					if (m_pRunningTask->prio <= RB_CONFIG_SYSVIEW_RBOS_TASK_PRIORITY_THRESHOLD)
					#endif
					{
						SEGGER_SYSVIEW_OnTaskStartReady((unsigned)m_pRunningTask);
					}
					#endif
					#endif
				}
				// Deleted task are not inserted into queue again
				if (m_pRunningTask->state != RB_OS_TERMINATED)
				{
					// Remove and insert to allow round robin
					TaskQueueRemove(&m_pTaskQueue, m_pRunningTask);
					// Insert task in priority order
					TaskQueueInsert(&m_pTaskQueue, m_pRunningTask);
				}
				return;

			case RB_OS_TERMINATED:
			case RB_OS_SUSPENDED:
			case RB_OS_DELAYED:
			default:
				break;
		} // switch

		// Get next task in queue
		m_pRunningTask = m_pRunningTask->next;
	}
}


//--------------------------------------------------------------------------------------------------
// RB_OS_TaskTicker
//--------------------------------------------------------------------------------------------------
//! \brief	Countdown of the task delays.
//!
//! This function must be called from the ticker interrupt every tick.
//! The tick time can be configured by setting RB_CONFIG_TICKER_TICKS_PER_SEC in RB_Config.h
//!
//! \attention This function is called at interrupt level by RB_Ticker.
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
void RB_OS_TaskTicker(void)
{
	RB_OS_tTask* pTask;

	// Increment performance counter
	if (m_pRunningTask)
		m_pRunningTask->runTime += (uint16_t)(1000 / RB_CONFIG_TICKER_TICKS_PER_SEC);

	// Get first ready task
	pTask = m_pTaskQueue;

	// For all task in queue
	while (pTask != NULL)
	{
		// If task is delayed
		if (pTask->state == RB_OS_DELAYED)
		{
			if (pTask->delay == 0)
			{
				// Set ready
				pTask->state = RB_OS_READY;
			}
			else if (pTask->delay > (1000 / RB_CONFIG_TICKER_TICKS_PER_SEC))
			{
				// Count down, prevent wrap-around of pTask->delay
				pTask->delay -= (1000 / RB_CONFIG_TICKER_TICKS_PER_SEC);
			}
			else
			{
				pTask->delay = 0;
			}
		}

		// Get next task in queue
		pTask = pTask->next;
	}
}


//--------------------------------------------------------------------------------------------------
// RB_OS_TaskCreate
//--------------------------------------------------------------------------------------------------
//! \brief	Create a task
//!
//! Create a task. The task control block has to be supplied by the caller.
//! The taskFunction() will be added to a scheduler, which calls this
//! function as often as possible, --> "cooperative multitasking mode"
//!
//! \param[out]	pTcb			Task control block
//! \param[in]	taskFunction	Start address of task function
//! \param[in]	arg				Argument of task function
//! \param[in]	priority		Task priority, 0 = high .. 255 = low
//! \param[in]	taskName		Task name, for debugging purpose only
//! \return		RB_OS_tStatus
//!		- RB_OS_OK
//!		- RB_OS_TASK_ALREADY_EXISTS		i.e. the task already exists
//!		- RB_OS_NULL_POINTER			Argument pTcb or taskFunction is NULL (debug only)
//!
//! \par Example code:
//! \code
//! void InitFunction(void)
//! {
//!		RB_OS_tStatus sts;
//!		RB_OS_tTask mainTask;
//!
//!		sts = RB_OS_TaskCreate(&mainTask, MainTaskFunction, NULL, RB_OS_TASK_DEFAULT_PRIO, RB_MODULE_NAME);
//!
//!		// other initializations
//! }
//!
//! void MainTaskFunction(const void* arg)       // signature of task function must be (const void*)
//! {
//!		// main task function
//! }
//! \endcode
//--------------------------------------------------------------------------------------------------
RB_OS_tStatus RB_OS_TaskCreate(RB_OS_tTask* pTcb, RB_OS_tTaskfunction taskFunction, const void* arg,
							   uint8_t priority, const char* taskName)
{
	const RB_OS_tTask* pTask;

	#ifndef NDEBUG
	// Sanity checks
	if (pTcb == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	if (taskFunction == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	#endif

	// Check if task already exists in queue
	pTask = m_pTaskQueue;
	while (pTask != NULL)
	{
		if (pTask == pTcb)
		{
			// Task already exists, return error
			RB_DEBUG_WARN("Task already exists");
			return RB_OS_TASK_ALREADY_EXISTS;
		}
		pTask = pTask->next;
	}

	// Fill task control block
	pTcb->next		= NULL;
	pTcb->task		= taskFunction;
	pTcb->delay		= 0;
	pTcb->state		= RB_OS_READY;
	pTcb->pMsgQueue	= NULL;
	pTcb->prio		= priority;
	pTcb->argument	= arg;
	pTcb->name		= taskName;

	// Insert task in front of ready queue
	TaskQueueInsert(&m_pTaskQueue, pTcb);

	return RB_OS_OK;
}


//--------------------------------------------------------------------------------------------------
// RB_OS_TaskDelete
//--------------------------------------------------------------------------------------------------
//! \brief	Delete a task
//!
//! \param[in,out]	pTcb			Task control block
//! \return			RB_OS_tStatus
//!		- RB_OS_OK
//!		- RB_OS_TASK_NOT_FOUND	i.e. the task could not be found in the task queue
//!		- RB_OS_NULL_POINTER	Argument pTcb is NULL (debug only)
//--------------------------------------------------------------------------------------------------
RB_OS_tStatus RB_OS_TaskDelete(RB_OS_tTask* pTcb)
{
	#if defined(RB_CONFIG_SYSVIEW_RECORD_RBOS_TASKS) && (RB_CONFIG_SYSVIEW_RECORD_RBOS_TASKS == RB_CONFIG_YES)
	SEGGER_SYSVIEW_TASKINFO Info;
	#endif

	#ifndef NDEBUG
	// Sanity checks
	if (pTcb == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	#endif

	#if defined(RB_CONFIG_SYSVIEW_RECORD_RBOS_TASKS) && (RB_CONFIG_SYSVIEW_RECORD_RBOS_TASKS == RB_CONFIG_YES)
	// --------------------------------------------------------------------------------------------
	// Give names in SystemView Timeline to task if this task is deleted before
	// the 5 second periodical update of tasks in the message queue is done.
	// --------------------------------------------------------------------------------------------
	#if defined(RB_CONFIG_SYSVIEW_RBOS_TASK_PRIORITY_THRESHOLD) && (RB_CONFIG_SYSVIEW_RBOS_TASK_PRIORITY_THRESHOLD < RB_OS_TASK_MIN_PRIO)
	if (pTcb->prio <= RB_CONFIG_SYSVIEW_RBOS_TASK_PRIORITY_THRESHOLD)
	#endif
	{
		Info.TaskID = (U32)pTcb;
		Info.sName = pTcb->name;
		Info.Prio = pTcb->prio;
		Info.StackBase = 0;
		Info.StackSize = 0;
		SEGGER_SYSVIEW_SendTaskInfo(&Info);
	}
	#endif

	pTcb->state = RB_OS_TERMINATED;
	return TaskQueueRemove(&m_pTaskQueue, pTcb);
}


//--------------------------------------------------------------------------------------------------
// RB_OS_TaskDelay
//--------------------------------------------------------------------------------------------------
//! \brief	Delay a task
//!
//! The task resumes execution after the specified delay time. If in "cooperative multitasking mode",
//! the scheduler will not call the taskFunction any more, until the delay time is over.
//!
//! \param[in,out]	pTcb			Task control block
//! \param[in]		delay			Delay time in ms, 0..2^32-1
//! \return			RB_OS_tStatus
//!		- RB_OS_OK
//!		- RB_OS_NULL_POINTER	Argument pTcb is NULL (debug only)
//--------------------------------------------------------------------------------------------------
RB_OS_tStatus RB_OS_TaskDelay(RB_OS_tTask* pTcb, uint32_t delay) RB_ATTR_THREAD_SAFE
{
	bool taskNotFound = false;
	#ifndef NDEBUG
	// Sanity checks
	if (pTcb == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	#endif

	// Check if task is deleted
	RB_ENTER_CRITICAL_SECTION;
	if (pTcb->state == RB_OS_TERMINATED)
	{
		taskNotFound = true; // Task already is deleted
	}
	else
	{
		pTcb->delay = delay;
		pTcb->state = RB_OS_DELAYED;
	}
	RB_LEAVE_CRITICAL_SECTION;

	if (taskNotFound)
	{
		return RB_OS_TASK_NOT_FOUND;
	}
	else
	{
		return RB_OS_OK;
	}
}


//--------------------------------------------------------------------------------------------------
// RB_OS_TaskSuspend
//--------------------------------------------------------------------------------------------------
//! \brief	Suspend a task
//!
//! \param[in,out]	pTcb			Task control block
//! \return			RB_OS_tStatus
//!		- RB_OS_OK
//!		- RB_OS_NULL_POINTER	Argument pTcb is NULL (debug only)
//--------------------------------------------------------------------------------------------------
RB_OS_tStatus RB_OS_TaskSuspend(RB_OS_tTask* pTcb) RB_ATTR_THREAD_SAFE
{
	bool taskNotFound = false;
	#ifndef NDEBUG
	// Sanity checks
	if (pTcb == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	#endif

	RB_ENTER_CRITICAL_SECTION;
	// Check if task is deleted
	if (pTcb->state == RB_OS_TERMINATED)
	{
		taskNotFound = true; // Task already is deleted
	}
	else
	{
		pTcb->state = RB_OS_SUSPENDED;
	}
	RB_LEAVE_CRITICAL_SECTION;

	if (taskNotFound)
	{
		return RB_OS_TASK_NOT_FOUND;
	}
	else
	{
		return RB_OS_OK;
	}
}


//--------------------------------------------------------------------------------------------------
// RB_OS_TaskResume
//--------------------------------------------------------------------------------------------------
//! \brief	Resume a suspended task
//!
//! \param[in,out]	pTcb			Task control block
//! \return			RB_OS_tStatus
//!		- RB_OS_OK
//!		- RB_OS_NULL_POINTER	Argument pTcb is NULL (debug only)
//--------------------------------------------------------------------------------------------------
RB_OS_tStatus RB_OS_TaskResume(RB_OS_tTask* pTcb) RB_ATTR_THREAD_SAFE
{
	bool taskNotFound = false;
	#ifndef NDEBUG
	// Sanity checks
	if (pTcb == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	#endif

	RB_ENTER_CRITICAL_SECTION;
	// Check if task is deleted
	if (pTcb->state == RB_OS_TERMINATED)
	{
		taskNotFound = true; // Task already is deleted
	}
	else
	{
		pTcb->state = RB_OS_READY;
	}
	RB_LEAVE_CRITICAL_SECTION;

	if (taskNotFound)
	{
		return RB_OS_TASK_NOT_FOUND;
	}
	else
	{
		return RB_OS_OK;
	}
}


//--------------------------------------------------------------------------------------------------
// RB_OS_TaskSetPriority
//--------------------------------------------------------------------------------------------------
//! \brief	Set the task priority
//!
//! The task will run with the new priority when the next task reschedule occurs.
//!
//! \param[in,out]	pTcb			Task control block
//! \param[in]		priority		Task priority, 0 = high .. 255 = low
//! \return			RB_OS_tStatus
//!		- RB_OS_OK
//!		- RB_OS_ILLEGAL_PRIORITY	i.e. priority is not in legal range
//!		- RB_OS_NULL_POINTER		Argument pTcb is NULL (debug only)
//--------------------------------------------------------------------------------------------------
RB_OS_tStatus RB_OS_TaskSetPriority(RB_OS_tTask* pTcb, uint8_t priority)
{
	#ifndef NDEBUG
	// Sanity checks
	if (pTcb == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	#endif

	// Check if task is deleted
	if (pTcb->state == RB_OS_TERMINATED)
	{
		return RB_OS_TASK_NOT_FOUND;	// Task is deleted
	}

	// Change task priority
	pTcb->prio = priority;

	// Remove and insert into task queue again in new priority order
	TaskQueueRemove(&m_pTaskQueue, pTcb);
	TaskQueueInsert(&m_pTaskQueue, pTcb);

	return RB_OS_OK;
}


//--------------------------------------------------------------------------------------------------
// RB_OS_TaskGetPriority
//--------------------------------------------------------------------------------------------------
//! \brief	Get the task priority
//!
//! \param[in]	pTcb			Task control block
//! \return		Task priority (returns RB_OS_TASK_MIN_PRIO if task does not exist)
//--------------------------------------------------------------------------------------------------
uint8_t RB_OS_TaskGetPriority(const RB_OS_tTask* pTcb)
{
	#ifndef NDEBUG
	// Sanity checks
	if (pTcb == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_TASK_DEFAULT_PRIO);
	}
	#endif

	return pTcb->prio;
}


//--------------------------------------------------------------------------------------------------
// RB_OS_GetTask
//--------------------------------------------------------------------------------------------------
//! \brief	Return the pointer of the actual running task.
//!
//! If called from a normal running task, the result will be always the TCB of the calling task. If
//! called from interrupt or other special task, the resulting pointer may also be NULL.
//!
//! \return	Task pointer of the actual running task (may be NULL)
//--------------------------------------------------------------------------------------------------
RB_OS_tTask* RB_OS_GetTask(void) RB_ATTR_THREAD_SAFE
{
	return m_pRunningTask;
}


//--------------------------------------------------------------------------------------------------
// RB_OS_GetTaskState
//--------------------------------------------------------------------------------------------------
//! \brief	Return task state.
//!
//! \param[in]	pTcb			Task control block
//! \return		Task state, i.e. terminated, ready, running, delayed, suspended, pending
//--------------------------------------------------------------------------------------------------
RB_OS_tTaskState RB_OS_GetTaskState(const RB_OS_tTask* pTcb) RB_ATTR_THREAD_SAFE
{
	return(pTcb ? pTcb->state : RB_OS_TERMINATED);
}


//--------------------------------------------------------------------------------------------------
// RB_OS_GetTaskQueue
//--------------------------------------------------------------------------------------------------
//! \brief	Return task queue pointer. For debugging only.
//!
//! \return	Task queue pointer
//--------------------------------------------------------------------------------------------------
RB_OS_tTask* RB_OS_GetTaskQueue(void) RB_ATTR_THREAD_SAFE
{
	return m_pTaskQueue;
}


//--------------------------------------------------------------------------------------------------
// RB_OS_MsgQueueCreate
//--------------------------------------------------------------------------------------------------
//! \brief	Create and initialize a message queue
//!
//! \param[out]	pMsgQueue		Message queue control block
//! \param[in]	pMsgBuffer		Message buffer, i.e. array of RB_OS_tMessage
//! \param[in]	msgBufSize		Number of messages, i.e. size of message buffer array
//! \param[in]	msgQueueName	Name of message queue, for debugging purpose only
//! \return		RB_OS_tStatus
//!		- RB_OS_OK				Successful
//!		- RB_OS_NULL_POINTER	Argument pMsgQueue or pMsgBuffer is NULL (debug only)
//!
//! \par Example code:
//! \code
//! #define EXPL_MESSAGES	5
//! RB_OS_tStatus EXPL_Sts;
//! RB_OS_tMessage EXPL_MsgBuf[EXPL_MESSAGES];
//! RB_OS_tMsgQueue EXPL_MsgQueue;
//!
//! EXPL_Sts = RB_OS_MsgQueueCreate(&EXPL_MsgQueue, EXPL_MsgBuf, EXPL_MESSAGES, RB_MODULE_NAME);
//! \endcode
//--------------------------------------------------------------------------------------------------
RB_OS_tStatus RB_OS_MsgQueueCreate(RB_OS_tMsgQueue* pMsgQueue, RB_OS_tMessage* pMsgBuffer,
								   size_t msgBufSize, const char* msgQueueName) RB_ATTR_THREAD_SAFE
{
	#ifndef NDEBUG
	// Sanity checks
	if (pMsgQueue == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	if (pMsgBuffer == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	#endif

	RB_QUEUE_Initialize(pMsgQueue, pMsgBuffer, (uint16_t)msgBufSize, (uint16_t)sizeof(RB_OS_tMessage), 0, 0);
	RB_QUEUE_SetName(pMsgQueue, msgQueueName);
	return(RB_OS_OK);
}


//--------------------------------------------------------------------------------------------------
// RB_OS_MsgQueueDelete
//--------------------------------------------------------------------------------------------------
//! \brief	Delete a message queue
//!
//! Attention! Do not delete a message queue that is still in use somewhere in the system.
//!
//! \param[in,out]	pMsgQueue		Message queue control block
//! \return			RB_OS_tStatus
//!		- RB_OS_OK				Successful
//!		- RB_OS_NULL_POINTER	Argument pMsgQueue is NULL (debug only)
//--------------------------------------------------------------------------------------------------
RB_OS_tStatus RB_OS_MsgQueueDelete(RB_OS_tMsgQueue* pMsgQueue) RB_ATTR_THREAD_SAFE
{
	#ifndef NDEBUG
	// Sanity checks
	if (pMsgQueue == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	#endif

	RB_QUEUE_Clear(pMsgQueue);
	return RB_OS_OK;
}


//--------------------------------------------------------------------------------------------------
// RB_OS_MsgQueueAccept
//--------------------------------------------------------------------------------------------------
//! \brief	Receive a message, return with RB_OS_NO_MSG_AVAILABLE if no message is available.
//!
//! The task is not suspended as in RB_OS_MsgQueuePend, i.e. you may do anything else until you
//! receive a message.
//!
//! \param[in,out]	pMsgQueue	Message queue control block
//! \param[out]		pMessage	Message
//! \return			RB_OS_tStatus
//!		- RB_OS_OK					Message received, i.e. copied to pMsg
//!		- RB_OS_NO_MSG_AVAILABLE	No message available
//!		- RB_OS_NULL_POINTER		Argument pMsgQueue or pMessage is NULL (debug only)
//!
//! \par Example code:
//! \code
//! RB_OS_tStatus EXPL_Sts;
//! RB_OS_tMsgQueue EXPL_MsgQueue;
//! RB_OS_tMessage EXPL_Message;
//!
//! EXPL_Sts = RB_OS_MsgQueueAccept(&EXPL_MsgQueue, &EXPL_Message);
//! if (EXPL_Sts == RB_OS_NO_MSG_AVAILABLE)
//! 	// Do anything
//! 	...
//! else {
//! 	// Process message
//! 	...
//! }
//!
//! \endcode
//--------------------------------------------------------------------------------------------------
RB_OS_tStatus RB_OS_MsgQueueAccept(RB_OS_tMsgQueue* pMsgQueue, RB_OS_tMessage* pMessage) RB_ATTR_THREAD_SAFE
{
	#ifndef NDEBUG
	// Sanity checks
	if (pMsgQueue == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	if (pMessage == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	#endif

	return(RB_QUEUE_Get(pMsgQueue, pMessage) ? RB_OS_OK : RB_OS_NO_MSG_AVAILABLE);
}


//--------------------------------------------------------------------------------------------------
// RB_OS_MsgQueuePend
//--------------------------------------------------------------------------------------------------
//! \brief	Pend for a message, wait if no message is available.
//!
//! The task resumes execution when a message is received. If in "cooperative multitasking mode",
//! the scheduler will not call the taskFunction any more, until a message is received.
//!
//! \attention RB_OS_MsgQueuePend must never be called from interrupt because it may suspend an
//!            arbitrary task! RB_OS_MsgQueuePend will always suspend the task if no message was
//!            available, which will be the task that was running when the interrupt happened!
//!            In case and interrupt has to read from a message queue, RB_OS_MsgQueueAccept must
//!            be used instead of RB_OS_MsgQueuePend!
//!
//! \param[in,out]	pMsgQueue	Message queue control block
//! \param[out]		pMessage	Message
//! \return			RB_OS_tStatus
//!		- RB_OS_OK					Message received, i.e. copied to pMsg
//!		- RB_OS_NO_MSG_AVAILABLE	No message available, only in "cooperative multitasking mode"
//!		- RB_OS_NULL_POINTER		Argument pMsgQueue or pMessage is NULL (debug only)
//!
//! \par Example code:
//! \code
//! RB_OS_tStatus EXPL_Sts;
//! RB_OS_tMsgQueue EXPL_MsgQueue;
//! RB_OS_tMessage EXPL_Message;
//!
//! EXPL_Sts = RB_OS_MsgQueuePend(&EXPL_MsgQueue, &EXPL_Message);
//! if (EXPL_Sts == RB_OS_NO_MSG_AVAILABLE)
//! 	return;
//! else {
//! 	// Process message
//! 	...
//! }
//!
//! \endcode
//--------------------------------------------------------------------------------------------------
RB_OS_tStatus RB_OS_MsgQueuePend(RB_OS_tMsgQueue* pMsgQueue, RB_OS_tMessage* pMessage)
{
	bool success;

	#ifndef NDEBUG
	// Sanity checks
	if (pMsgQueue == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	if (pMessage == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	#endif

	success = RB_QUEUE_Get(pMsgQueue, pMessage);
	if (success)
		return(RB_OS_OK);

	// Set task to pending state
	m_pRunningTask->pMsgQueue = pMsgQueue;
	m_pRunningTask->state     = RB_OS_PENDING;

	// Fill message
	pMessage->src = 0;
	pMessage->evt = 0;

	return(RB_OS_NO_MSG_AVAILABLE);
}


//--------------------------------------------------------------------------------------------------
// RB_OS_MsgQueuePost
//--------------------------------------------------------------------------------------------------
//! \brief	Post a message, if message queue is not full, i.e. no waiting if queue is full.
//!
//! \param[in,out]	pMsgQueue	Message queue control block
//! \param[in]		pMessage	Message, the message is copied and can be reused again
//! \return			RB_OS_tStatus
//!		- RB_OS_OK					Message placed in queue
//!		- RB_OS_MSG_QUEUE_FULL		Message queue is full, nothing done
//!		- RB_OS_NULL_POINTER		Argument pMsgQueue or pMessage is NULL (debug only)
//--------------------------------------------------------------------------------------------------
RB_OS_tStatus RB_OS_MsgQueuePost(RB_OS_tMsgQueue* pMsgQueue, const RB_OS_tMessage* pMessage) RB_ATTR_THREAD_SAFE
{
	bool success;

	#ifndef NDEBUG
	// Sanity checks
	if (pMsgQueue == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	if (pMessage == NULL)
	{
		RB_DEBUG_FAIL("NULL pointer");
		return(RB_OS_NULL_POINTER);
	}
	#endif

	success = RB_QUEUE_Put(pMsgQueue, pMessage);
	return((success) ? RB_OS_OK : RB_OS_MSG_QUEUE_FULL);
}


//--------------------------------------------------------------------------------------------------
// RB_OS_MsgQueuePostEvent
//--------------------------------------------------------------------------------------------------
//! \brief	Post a message, if message queue is not full, i.e. no waiting if queue is full.
//!
//! Same functionality as RB_OS_MsgQueuePost, but the message is composed inside this function.
//!
//! \param[in,out]	pMsgQueue	Message queue control block
//! \param[in]		src			Source of message
//! \param[in]		evt			Event occurred
//! \return			RB_OS_tStatus
//!		- RB_OS_OK					Message placed in queue
//!		- RB_OS_MSG_QUEUE_FULL		Message queue is full, nothing done
//!		- RB_OS_NULL_POINTER		Argument pMsgQueue is NULL (debug only)
//--------------------------------------------------------------------------------------------------
RB_OS_tStatus RB_OS_MsgQueuePostEvent(RB_OS_tMsgQueue* pMsgQueue, RB_OS_tSource src, RB_OS_tEvent evt) RB_ATTR_THREAD_SAFE
{
	RB_OS_tMessage message;
	message.src = src;
	message.evt = evt;
	return(RB_OS_MsgQueuePost(pMsgQueue, &message));
}


//==================================================================================================
//  L O C A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// TaskQueueInsert
//--------------------------------------------------------------------------------------------------
//! \brief	Insert a task in priority order into task queue.
//!
//! \param[in,out]	ppQueue			Address of queue
//! \param[in]		pTask			Task to insert
//! \return			RB_OS_tStatus
//!		- Always RB_OS_OK
//--------------------------------------------------------------------------------------------------
static RB_OS_tStatus TaskQueueInsert(RB_OS_tTask** ppQueue, RB_OS_tTask* pTask)
{
	RB_OS_tTask* p_post; // the one after insertion
	RB_OS_tTask* p_prev; // the one before

	pTask->next = NULL;
	p_prev = NULL;
	p_post = *ppQueue;
	while (p_post) // not the last link
	{
		if (p_post->prio <= pTask->prio)
		{   // lets go to the next link
			p_prev = p_post;
			p_post = p_post->next;
		}
		else
		{   // insert and leave loop
			pTask->next = p_post;
			break;
		}
	}
	if (p_prev == NULL)
		*ppQueue = pTask; // inserted at the beginning
	else
		p_prev->next = pTask;

	return RB_OS_OK;
}


//--------------------------------------------------------------------------------------------------
// TaskQueueRemove
//--------------------------------------------------------------------------------------------------
//! \brief	Remove a task from task queue.
//!
//! \param[in,out]	ppQueue			Address of queue
//! \param[in]		pTask			Task to remove
//! \return			RB_OS_tStatus
//!		- RB_OS_OK
//!		- RB_OS_TASK_NOT_FOUND, i.e. the task could not be found in the task queue
//--------------------------------------------------------------------------------------------------
static RB_OS_tStatus TaskQueueRemove(RB_OS_tTask** ppQueue, const RB_OS_tTask* pTask)
{
	RB_OS_tTask* p1;
	RB_OS_tTask* p2;

	p2 = NULL;
	p1 = *ppQueue;
	while (p1 != pTask)
	{
		if (p1 == NULL)
		{
			return RB_OS_TASK_NOT_FOUND;
		}
		p2 = p1;
		p1 = p1->next;
	}
	if (p2 == NULL)
		*ppQueue = pTask->next;
	else
		p2->next = pTask->next;

	return RB_OS_OK;
}


//--------------------------------------------------------------------------------------------------
