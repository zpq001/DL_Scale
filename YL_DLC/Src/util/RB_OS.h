//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_OS.h
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
//! \author		Werner Langenegger, Martin Heusser
//
// $Date: 2017/04/04 13:06:50MESZ $
// $Revision: 1.75 $
//
//==================================================================================================

#ifndef _RB_OS__h
#define _RB_OS__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

// This module is mandatory and has no RB_CONFIG_USE, no check is needed here.

#include "RB_Typedefs.h"
#include "RB_Identifiers.h"
#include "RB_Queue.h"


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

//! Macro for backward compatibility, was originally used to reserve an additional element in buffer.
//! It is no longer needed to increase message buffer size by one element.
//! #define MAX_NUMBER_OF_MESSAGES	5
//! RB_OS_tMessage MsgBuf[MAX_NUMBER_OF_MESSAGES];
//! RB_OS_MsgQueueCreate(&MsgQueue, MsgBuf, MAX_NUMBER_OF_MESSAGES, "example");
#define RB_OS_MSG_BUF_SIZE(n)			(n)

//! Maximum task priority
#define RB_OS_TASK_MAX_PRIO				0

//! Minimum task priority
#define RB_OS_TASK_MIN_PRIO				255

//! Default task priority, use this priority for all normal tasks
#define RB_OS_TASK_DEFAULT_PRIO			100

//! Background task priority, use this priority for backround tasks
#define RB_OS_TASK_BACKGROUND_PRIO		RB_OS_TASK_MIN_PRIO


//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//! Status, i.e. return values
typedef enum {
	RB_OS_OK = 0,
	RB_OS_TASK_NOT_FOUND,		//!< The task could not be found, use RB_OS_TaskCreate() first
	RB_OS_TASK_ALREADY_EXISTS,	//!< The task to be created, already exists
	RB_OS_NO_MSG_AVAILABLE,		//!< No message is currently available
	RB_OS_MSG_QUEUE_FULL,		//!< The message queue is full, message is not posted
	RB_OS_MSG_QUEUE_SIZE_ERR,	//!< The message queue size is too small, must be at least 1 elements
	RB_OS_NULL_POINTER			//!< NULL pointer exception
} RB_DECL_TYPE RB_OS_tStatus;


//! Message source type
typedef uint16_t RB_DECL_TYPE RB_OS_tSource;


//! Message event type
typedef uint16_t RB_DECL_TYPE RB_OS_tEvent;


//! Message
typedef struct {
	RB_OS_tSource		src;	//!< Source of message
	RB_OS_tEvent		evt;	//!< Event
} RB_DECL_TYPE RB_OS_tMessage;


//! Message queue control block
#define RB_OS_tMsgQueue	RB_QUEUE_tQueue


//! Task function
typedef RB_DECL_TYPE void (*RB_OS_tTaskfunction)(const void* pParam);


//! Task state
typedef enum {
	RB_OS_TERMINATED = 0,		//!< Terminated, before start and after termination
	RB_OS_READY,				//!< Scheduled for running
	RB_OS_RUNNING,				//!< Task is running
	RB_OS_DELAYED,				//!< Waiting for delay expiration
	RB_OS_SUSPENDED,			//!< Waiting for resume
	RB_OS_PENDING				//!< Waiting for message
} RB_DECL_TYPE RB_OS_tTaskState;


//! Task control block
typedef struct _RB_OS_tTask {
	struct _RB_OS_tTask*	next;			//!< Address of next task control block
	RB_OS_tTaskfunction		task;			//!< Address of task function
	const void*				argument;		//!< Task function argument
	RB_QUEUE_tQueue*		pMsgQueue;		//!< Pending message queue
	uint32_t				delay;			//!< Task is delayed [ms], if delay > 0
	RB_OS_tTaskState		state;			//!< State of task
	uint8_t					prio;			//!< Task priority
	uint8_t					paddingByte;	//!< Padding byte to even value
	uint16_t				runTime;		//!< Actual run time of task in milliseconds
	uint16_t				runPeak;		//!< Peak run time of task in milliseconds
	const char*				name;			//!< Task name
} RB_DECL_TYPE RB_OS_tTask;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
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
RB_DECL_FUNC void RB_OS_Initialize(void);


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
RB_DECL_FUNC void RB_OS_TaskReschedule(void);


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
RB_DECL_FUNC void RB_OS_TaskTicker(void);


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
RB_DECL_FUNC RB_OS_tStatus RB_OS_TaskCreate(RB_OS_tTask* pTcb, RB_OS_tTaskfunction taskFunction, const void* arg,
											uint8_t priority, const char* taskName);


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
RB_DECL_FUNC RB_OS_tStatus RB_OS_TaskDelete(RB_OS_tTask* pTcb);


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
RB_DECL_FUNC RB_OS_tStatus RB_OS_TaskDelay(RB_OS_tTask* pTcb, uint32_t delay) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC RB_OS_tStatus RB_OS_TaskSuspend(RB_OS_tTask* pTcb) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC RB_OS_tStatus RB_OS_TaskResume(RB_OS_tTask* pTcb) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC RB_OS_tStatus RB_OS_TaskSetPriority(RB_OS_tTask* pTcb, uint8_t priority);


//--------------------------------------------------------------------------------------------------
// RB_OS_TaskGetPriority
//--------------------------------------------------------------------------------------------------
//! \brief	Get the task priority
//!
//! \param[in]	pTcb			Task control block
//! \return		Task priority (returns RB_OS_TASK_MIN_PRIO if task does not exist)
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint8_t RB_OS_TaskGetPriority(const RB_OS_tTask* pTcb);


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
RB_DECL_FUNC RB_OS_tTask* RB_OS_GetTask(void) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_OS_GetTaskState
//--------------------------------------------------------------------------------------------------
//! \brief	Return task state.
//!
//! \param[in]	pTcb			Task control block
//! \return		Task state, i.e. terminated, ready, running, delayed, suspended, pending
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_OS_tTaskState RB_OS_GetTaskState(const RB_OS_tTask* pTcb) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_OS_GetTaskQueue
//--------------------------------------------------------------------------------------------------
//! \brief	Return task queue pointer. For debugging only.
//!
//! \return	Task queue pointer
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_OS_tTask* RB_OS_GetTaskQueue(void) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC RB_OS_tStatus RB_OS_MsgQueueCreate(RB_OS_tMsgQueue* pMsgQueue, RB_OS_tMessage* pMsgBuffer,
												size_t msgBufSize, const char* msgQueueName) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC RB_OS_tStatus RB_OS_MsgQueueDelete(RB_OS_tMsgQueue* pMsgQueue) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC RB_OS_tStatus RB_OS_MsgQueueAccept(RB_OS_tMsgQueue* pMsgQueue, RB_OS_tMessage* pMessage) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC RB_OS_tStatus RB_OS_MsgQueuePend(RB_OS_tMsgQueue* pMsgQueue, RB_OS_tMessage* pMessage);


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
RB_DECL_FUNC RB_OS_tStatus RB_OS_MsgQueuePost(RB_OS_tMsgQueue* pMsgQueue, const RB_OS_tMessage* pMessage) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC RB_OS_tStatus RB_OS_MsgQueuePostEvent(RB_OS_tMsgQueue* pMsgQueue, RB_OS_tSource src, RB_OS_tEvent evt) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif

#endif
