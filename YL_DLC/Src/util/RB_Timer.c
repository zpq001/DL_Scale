//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Timer.c
//! \ingroup	util
//! \brief		Timer utilities.
//!
//! This module supports a milliseconds counter and a seconds counter reflecting the time since
//! startup. The milliseconds counter will overflow after approximately 49 days. The seconds counter
//! will overflow after approximately 136 years.
//!
//! The module also supports timeout handling for single or repetitive callbacks or messages.
//! It contains two lists of timers. One list will be maintained on main level (for main level timer
//! callback), the other list will be maintained on interrupt level (for interrupt level timer
//! callback or event messages).
//!
//! Generally the timers can be manipulated (i.e. calling the functions RB_TIMER_SetTimeout,
//! RB_TIMER_SetTimeoutMessage, RB_TIMER_CancelTimeout) from main or interrupt level, but there are
//! some restrictions:
//! \li A main level timer must not be canceled or reloaded from interrupt level.
//!     Otherwise it could happen that the timer element is accessed after it has been changed or
//!     canceled, which could lead to invalid memory accesses.
//! \li For canceling or reloading timers from within a timer callback the same rule applies, it is
//!     permitted as long as no main level timer is modified from interrupt level.
//! \li Interrupt level timers must not be manipulated from a higher-than-normal interrupt level
//!     (e.g. FIQ on LPC2000, anything higher than RB_SYSCONTROL_IRQ_PRIO_NORMAL on LPC1000)
//! \li These limitations are caused by the fact that the callback functions cannot be called from
//!     within a critical section.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Peter Lutz, Matthias Klaey, Martin Heusser, Silvan Sturzenegger
//
// $Date: 2017/08/15 11:05:00MESZ $
// $Revision: 1.89 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Timer"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Timer.h"
// This module is mandatory and has no RB_CONFIG_USE, no check is needed here.

#include "RB_Config.h"
#include "RB_Ticker.h"


//==================================================================================================
//  L O C A L   D E F I N I T I O N S
//==================================================================================================

//! Check RB_CONFIG_TICKER_TICKS_PER_SEC is in range 50..1000 is alread done by RB_Ticker.h
//! Check if RB_CONFIG_TICKER_TICKS_PER_SEC results in a natural number of milliseconds. As a result
//! of this check, only values 1000, 500, 250, 200, 125, 100 or 50 are valid.
#if ((1000 / RB_CONFIG_TICKER_TICKS_PER_SEC) * RB_CONFIG_TICKER_TICKS_PER_SEC) != 1000
	#error RB_CONFIG_TICKER_TICKS_PER_SEC does not results in a natural number of milliseconds
#endif

//! Time of one tick in ms
#define MILLISECONDS_PER_TICK		(1000UL / (uint32_t)RB_CONFIG_TICKER_TICKS_PER_SEC)


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

//! List of pending timers on interrupt level
static RB_TIMER_tElement* pIntTimerList = NULL;
//! List of expired timers on interrupt level
static RB_TIMER_tElement* pIntExpList = NULL;
//! List of pending timers on main level
static RB_TIMER_tElement* pMainTimerList = NULL;
//! List of expired timers on main level
static RB_TIMER_tElement* pMainExpList = NULL;

//! Millisecond counter that counts up after power up and will overflow after approximately 49 days
static volatile uint32_t millisecondCount = (uint32_t)0;

//! Second counter that counts up after power up and will overflow after approximately 136 years
static volatile uint32_t secondCount = (uint32_t)0;

//! Second prescaler
static uint32_t secondPrescaler = (uint32_t)0;

//! Pointer to the currently active main timer element, used in RB_TIMER_GetTimerElement
static const RB_TIMER_tElement* pActiveMainTimer = NULL;
//! Pointer to the currently active interrupt timer element, used in RB_TIMER_GetTimerElement
static const RB_TIMER_tElement* pActiveIntTimer = NULL;
//! true, when callback function under interrupt is executed, used in RB_TIMER_GetTimerElement
static bool interruptActive = false;

//! Milliseconds of last execution of this function
static uint32_t lastMillisecondsOfExecCallback = (uint32_t)0;


//==================================================================================================
//  F O R W A R D   D E C L A R A T I O N S
//==================================================================================================

static void AddTimerToIntTimerList(RB_TIMER_tElement* pTimerElement);
static void AddTimerToMainTimerList(RB_TIMER_tElement* pTimerElement);
static bool RemoveTimerFromIntTimerList(const RB_TIMER_tElement* pTimerElement);
static bool RemoveTimerFromMainTimerList(const RB_TIMER_tElement* pTimerElement);


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_TIMER_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initializes the timer module
//!
//! \attention	This module cannot be re-initialized because owners of timers can not be informed
//!				about lost timers. Therefore RB_TIMER_Initialize will do nothing.
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
void RB_TIMER_Initialize(void)
{
	// Nothing to do, see description above
}


//--------------------------------------------------------------------------------------------------
// RB_TIMER_SetTimeout
//--------------------------------------------------------------------------------------------------
//! \brief		Set a timeout function call in/every x milliseconds
//!
//! Set a timeout function to be called in x milliseconds.
//! The timer list element has to be supplied by the caller.
//! Choose whether the timeout should be periodic.
//! Choose whether your function should be executed on timer interrupt service routine or executed
//! by RB_TIMER_ExecuteCallbackOnTimeout() called periodically from RB_MAIN_Run().
//!
//! \attention	This function can be called on main() level or on interrupt level. But a main level
//!				timer should not be reloaded from interrupt level, otherwise it is possible that the
//!				callback function is called after the timer has been modified.
//!
//! \attention	Keep the processing time of the callback function to an absolute minimum
//!				if you set up the timer for interrupt level - other interrupts cannot run
//!				while the timer interrupt is running.
//!
//! \param		pTimerElement			Timer list element
//! \param		pCallbackFunction		Address of callback function
//! \param		milliseconds			Timeout time
//! \param		periodic				true if timeout is periodic
//! \param		execOnTimeoutInterrupt	true if callback function is to be executed on interrupt, see comments above
//! \return		true if the setting was successful
//--------------------------------------------------------------------------------------------------
bool RB_TIMER_SetTimeout(RB_TIMER_tElement* pTimerElement,
						 RB_tCallback pCallbackFunction,
						 uint32_t milliseconds,
						 bool periodic,
						 bool execOnTimeoutInterrupt)
{
	if ((pTimerElement == NULL) || (pCallbackFunction == NULL))
		return(false);

	RB_TIMER_CancelTimeout(pTimerElement); // Prevent any additional actions on this timer if it was used before

	if (milliseconds < MILLISECONDS_PER_TICK)
		milliseconds = MILLISECONDS_PER_TICK; // Correct times below minimum

	// Fill timer element
	pTimerElement->ptr.pCallback = pCallbackFunction;
	pTimerElement->milliseconds = milliseconds;
	pTimerElement->period = ((periodic) ? milliseconds : 0uL);

	if (execOnTimeoutInterrupt)
	{
		pTimerElement->type = RB_TIMER_CALLBACK_INT;
		AddTimerToIntTimerList(pTimerElement);
	}
	else
	{
		// Correct time by the elapsed time since last execution of RB_TIMER_ExecuteCallbackOnTimeout.
		// This correction is needed only for main timer when a task has taken a long time to execute
		// before setting the timeout.
		// No correction is needed for interrupt timer since they are handled each ticker interval.
		uint32_t correction = millisecondCount - lastMillisecondsOfExecCallback;
		if (correction != 0)
		{
			if (pTimerElement->milliseconds <= (UINT32_MAX - correction)) // prevent overflow
				pTimerElement->milliseconds += correction;
		}

		pTimerElement->type = RB_TIMER_CALLBACK_MAIN;
		AddTimerToMainTimerList(pTimerElement);
	}

	return (true);
}


//--------------------------------------------------------------------------------------------------
// RB_TIMER_SetTimeoutMessage
//--------------------------------------------------------------------------------------------------
//! \brief		Set a timeout message, i.e. the message is sent in/every x milliseconds
//!
//! Send a message in x milliseconds. The message is sent in timer interrupt.
//! The timer list element has to be supplied by the caller.
//! Choose whether the message is sent once or periodically.
//!
//! \attention	This function can be called on main() level and on interrupt level
//!
//! \param		pTimerElement			Timer list element
//! \param		pMsgQueue				Message queue to send event
//! \param		source					message source
//! \param		event					message event
//! \param		milliseconds			Timeout time
//! \param		periodic				true if timeout is periodic
//! \return		true if the setting was successful
//--------------------------------------------------------------------------------------------------
bool RB_TIMER_SetTimeoutMessage(RB_TIMER_tElement* pTimerElement,
								RB_OS_tMsgQueue* pMsgQueue,
								RB_OS_tSource source,
								RB_OS_tEvent event,
								uint32_t milliseconds,
								bool periodic)
{
	if ((pTimerElement == NULL) || (pMsgQueue == NULL))
		return (false);

	RB_TIMER_CancelTimeout(pTimerElement); // Prevent any additional actions on this timer if it was used before

	if (milliseconds < MILLISECONDS_PER_TICK)
		milliseconds = MILLISECONDS_PER_TICK; // Correct times below minimum

	// Fill timer element
	pTimerElement->milliseconds = milliseconds;
	pTimerElement->period = ((periodic) ? milliseconds : 0uL);
	pTimerElement->type = RB_TIMER_MESSAGE_INT;
	pTimerElement->ptr.pMsgQueue = pMsgQueue;
	pTimerElement->message.src = source;
	pTimerElement->message.evt = event;

	AddTimerToIntTimerList(pTimerElement);

	return (true);
}


//--------------------------------------------------------------------------------------------------
// RB_TIMER_CancelTimeout
//--------------------------------------------------------------------------------------------------
//! \brief		Cancel a timeout function call or a timed event
//!
//! \attention	This function can be called on main() level or on interrupt level. But a main level
//!				timer should not be canceled from interrupt level, otherwise it is possible that the
//!				callback function is called after the timer is already canceled.
//!
//! \attention	Canceling a timeout message where the message is already sent, will not delete the
//!				message itself.
//!
//! \param		pTimerElement	Timer list element
//! \return		true if cancel was successful
//--------------------------------------------------------------------------------------------------
bool RB_TIMER_CancelTimeout(RB_TIMER_tElement* pTimerElement)
{
	bool ret = false;

	if (pTimerElement == NULL)
		return(false);

	// Prevent any additional actions on this timer
	RB_ENTER_CRITICAL_SECTION;
	pTimerElement->milliseconds = 0;
	pTimerElement->period = 0;
	pTimerElement->ptr.pCallback = NULL;
	pTimerElement->ptr.pMsgQueue = NULL;
	RB_LEAVE_CRITICAL_SECTION;

	// Remove from lists. CancelTimeout may also be called with uninitialized or corrupted timer
	// elements. Since no data of timer element is valid, the element must be removed from both lists.
	ret = RemoveTimerFromIntTimerList(pTimerElement);
	if (!ret)
		ret = RemoveTimerFromMainTimerList(pTimerElement);
	return(ret);
}


//--------------------------------------------------------------------------------------------------
// RB_TIMER_ExecuteCallbackOnTimeout
//--------------------------------------------------------------------------------------------------
//! \brief		Calls main level timeout functions and handles the timer list
//!
//! Calls timeout functions if a timeout has been set.
//! The appropriate element in the main timer list will be removed or reloaded.
//! Makes sure that timer elements are only checked once per tick time. Thus this function can be
//! called by \see RB_MAIN_Run() as often as possible.
//!
//! \attention	This function is called by RB_MAIN_Run() on main level
//!
//! \return		None
//--------------------------------------------------------------------------------------------------
void RB_TIMER_ExecuteCallbackOnTimeout(void)
{
	RB_TIMER_tElement* pMainTimer = NULL;
	RB_TIMER_tElement* pMainPrev = NULL;
	uint32_t milliseconds = millisecondCount; // Copy volatile timer once
	uint32_t delta; // Time since last call

	// Calculate the amount of time that elapsed since the last execution of this function.
	delta = milliseconds - lastMillisecondsOfExecCallback;
	lastMillisecondsOfExecCallback = milliseconds;

	// Check if walk through timer list is needed
	if (delta == 0)
		return; // Execute only once when millisecondCount has changed

	// Handle main timers (needs critical section for list manipulation by interrupts)
	RB_ENTER_CRITICAL_SECTION;
	pMainTimer = pMainTimerList;
	// Phase 1: Decrement all main timers by delta and remove them from list if expired
	while (pMainTimer)
	{
		if (pMainTimer->milliseconds > delta)
		{
			// Non-expired timers:
			pMainTimer->milliseconds -= delta; // Does not expire
			pMainPrev = pMainTimer;
			pMainTimer = pMainTimer->pNext; // Step to next element
			continue;
		}

		// Expired timers: Remove from list, add to expired list
		pMainTimer->milliseconds = 0uL;
		if (pMainPrev)
			pMainPrev->pNext = pMainTimer->pNext;
		else
			pMainTimerList = pMainTimer->pNext;
		pMainTimer->pExpNext = pMainExpList; // Use alternate next pointer
		pMainExpList = pMainTimer;
		pMainTimer = pMainTimer->pNext; // Step to next element
	}
	RB_LEAVE_CRITICAL_SECTION;

	// Phase 2: Walk through the expired list and execute callbacks
	while (pMainExpList)
	{
		RB_tCallback callback = pMainExpList->ptr.pCallback;
		// Check for periodic: Re-attach to main timer list
		if (pMainExpList->period)
		{
			pMainExpList->milliseconds = pMainExpList->period; // reload timer
			pMainExpList->pNext = pMainTimerList; // Prepends main timer list
			pMainTimerList = pMainExpList;
		}

		pActiveMainTimer = pMainExpList;	// used in RB_TIMER_GetTimerElement()
		// Call callback function
		if (callback)
		{
			callback();
		}
		// Step to next timer. If pMainExpList is not the same as the active timer, then the
		// active timer was canceled during the callback and the list is already updated.
		if (pMainExpList == pActiveMainTimer)
		{
			pMainExpList = pMainExpList->pExpNext;
		}
		pActiveMainTimer = NULL;
	}
}


//--------------------------------------------------------------------------------------------------
// RB_TIMER_Ticker
//--------------------------------------------------------------------------------------------------
//! \brief		Handler for milliseconds, seconds and interrupt timers
//!
//! Increase millisecondCount by configured tick time based on RB_CONFIG_TICKER_TICKS_PER_SEC every
//! invocation. Update secondCount and handle all interrupt timers.
//!
//! \attention	This system function is called by RB_TICKER_Interrupt() on interrupt level.
//!
//! \attention	Never call this function from main level.
//!
//! \return		None
//--------------------------------------------------------------------------------------------------
void RB_TIMER_Ticker(void)
{
	RB_TIMER_tElement* pIntTimer = pIntTimerList;
	RB_TIMER_tElement* pIntPrev = NULL;

	// Handle milliseconds and seconds counter
	millisecondCount += MILLISECONDS_PER_TICK;
	secondPrescaler  += MILLISECONDS_PER_TICK;
	if (secondPrescaler >= 1000uL)
	{
		secondPrescaler -= 1000uL;
		secondCount++;
	}

	// Handle interrupt timers
	// Phase 1: Decrement all interrupt timers by MILLISECONDS_PER_TICK and remove them from list if expired
	while (pIntTimer)
	{
		if (pIntTimer->milliseconds > MILLISECONDS_PER_TICK)
		{
			// Non-expired timers:
			pIntTimer->milliseconds -= MILLISECONDS_PER_TICK; // Does not expire
			pIntPrev = pIntTimer;
			pIntTimer = pIntTimer->pNext; // Step to next element
			continue;
		}

		// Expired timers: Remove from list, add to expired list
		pIntTimer->milliseconds = 0uL;
		if (pIntPrev)
			pIntPrev->pNext = pIntTimer->pNext; // Remove intermediate element
		else
			pIntTimerList = pIntTimer->pNext; // Remove first element
		pIntTimer->pExpNext = pIntExpList; // Use alternate next pointer
		pIntExpList = pIntTimer;
		pIntTimer = pIntTimer->pNext;
	}

	// Phase 2: Walk through the expired list and execute callbacks or send messages
	while (pIntExpList)
	{
		RB_tCallback callback = pIntExpList->ptr.pCallback;
		if (pIntExpList->period)
		{
			pIntExpList->milliseconds = pIntExpList->period; // reload timer
			pIntExpList->pNext = pIntTimerList; // Prepends interrupt timer list
			pIntTimerList = pIntExpList;
		}

		// Send Message or call callback
		if (pIntExpList->type == RB_TIMER_MESSAGE_INT)
		{
			// Send message
			if (pIntExpList->ptr.pMsgQueue)
				RB_OS_MsgQueuePost(pIntExpList->ptr.pMsgQueue, &pIntExpList->message);
			pIntExpList = pIntExpList->pExpNext;
		}
		else
		{
			interruptActive = true;						// used in RB_TIMER_GetTimerElement()
			pActiveIntTimer = pIntExpList;	// used in RB_TIMER_GetTimerElement()
			// Call callback function
			if (callback)
			{
				callback();
			}
			// Step to next timer. If pIntExpList is not the same as the active timer, then
			// the active timer was canceled during the callback and the list is already updated.
			if (pIntExpList == pActiveIntTimer)
			{
				pIntExpList = pIntExpList->pExpNext;
			}
			pActiveIntTimer = NULL;
			interruptActive = false;
		}
	}
}


//--------------------------------------------------------------------------------------------------
// RB_TIMER_GetTimerElement
//--------------------------------------------------------------------------------------------------
//! \brief		Returns the address of the currently active timer element
//!
//! Returns the timer element pointer to identify the currently executed callback function.
//! This function may only be called in the callback function, otherwise a NULL pointer returns.
//!
//! \attention	This function may only be called in the timer callback function!
//!
//! \return		Address of timer element of the currently executed callback function, otherwise NULL
//--------------------------------------------------------------------------------------------------
const RB_TIMER_tElement* RB_TIMER_GetTimerElement(void)
{
	return((interruptActive) ? pActiveIntTimer : pActiveMainTimer);
}


//--------------------------------------------------------------------------------------------------
// RB_TIMER_GetSystemTime
//--------------------------------------------------------------------------------------------------
//! \brief		Returns a free running millisecond counter
//!
//! Returns a free running millisecond counter that is started at power up.
//! The increment is defined as 1000 / RB_CONFIG_TICKER_TICKS_PER_SEC.
//!
//! \attention	The counter maximum value is 4'294'967'295d (0xFFFFFFFF)
//! \attention	After this value an overflow will reset the counter to 0
//! \attention	This happens approx. every 49 days or 4'294'967'295d milliseconds after power up
//!
//! \return		Milliseconds since power on
//--------------------------------------------------------------------------------------------------
uint32_t RB_TIMER_GetSystemTime(void) RB_ATTR_THREAD_SAFE
{
	return(millisecondCount);
}


//--------------------------------------------------------------------------------------------------
// RB_TIMER_GetSystemTimeInSeconds
//--------------------------------------------------------------------------------------------------
//! \brief		Returns a free running second counter
//!
//! Returns a free running second counter that is started at power up.
//!
//! \note		The counter will overflow after 136 years
//!
//! \return		Seconds since power on
//--------------------------------------------------------------------------------------------------
uint32_t RB_TIMER_GetSystemTimeInSeconds(void) RB_ATTR_THREAD_SAFE
{
	return(secondCount);
}


//--------------------------------------------------------------------------------------------------
// RB_TIMER_BusyDelay
//--------------------------------------------------------------------------------------------------
//! \brief		This function does a busy delay. Used for exception info display during debugging.
//!
//! \attention	This functions does a busy delay, i.e. no other task can run during the delay!
//!
//! \param		ms		Delay time in milliseconds
//! \return		None
//--------------------------------------------------------------------------------------------------
void RB_TIMER_BusyDelay(uint32_t ms)
{
	for (;ms > 0; ms--)
		RB_BusyWaitMicroSeconds(1000); // Let the underlying system wait for a millisecond
}


//==================================================================================================
//  L O C A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// AddTimerToIntTimerList
//--------------------------------------------------------------------------------------------------
//! \brief	Add timer to interrupt list
//!
//! \param		pTimerElement	Timer list element
//--------------------------------------------------------------------------------------------------
static void AddTimerToIntTimerList(RB_TIMER_tElement* pTimerElement)
{
	// Add element to interrupt timer list as first element (needs critical section)
	// Note: If corrections are made also correct in AddTimerToMainTimerList
	RB_ENTER_CRITICAL_SECTION;
	pTimerElement->pNext = pIntTimerList;
	pIntTimerList = pTimerElement;
	RB_LEAVE_CRITICAL_SECTION;
}


//--------------------------------------------------------------------------------------------------
// AddTimerToMainTimerList
//--------------------------------------------------------------------------------------------------
//! \brief	Add timer to main list
//!
//! \param		pTimerElement	Timer list element
//--------------------------------------------------------------------------------------------------
static void AddTimerToMainTimerList(RB_TIMER_tElement* pTimerElement)
{
	// Add element to main timer list as first element (needs critical section)
	// Note: If corrections are made also correct in AddTimerToIntTimerList
	RB_ENTER_CRITICAL_SECTION;
	pTimerElement->pNext = pMainTimerList;
	pMainTimerList = pTimerElement;
	RB_LEAVE_CRITICAL_SECTION;
}


//--------------------------------------------------------------------------------------------------
// RemoveTimerFromIntTimerList
//--------------------------------------------------------------------------------------------------
//! \brief	Remove timer from interrupt list
//!
//! \param		pTimerElement	Timer list element
//! \return		true if remove was successful (timer found in list)
//--------------------------------------------------------------------------------------------------
static bool RemoveTimerFromIntTimerList(const RB_TIMER_tElement* pTimerElement)
{
	// Note: If corrections are made also correct in RemoveTimerFromMainTimerList
	bool ret = false;
	RB_TIMER_tElement* pT;
	RB_TIMER_tElement* pPrev;
	RB_ENTER_CRITICAL_SECTION;
	// Remove from pending timer list
	pT = pIntTimerList;
	pPrev = NULL;
	while (pT)
	{
		if (pT == pTimerElement)
		{
			if (pPrev)
				pPrev->pNext = pT->pNext; // Remove intermediate element
			else
				pIntTimerList = pT->pNext; // Remove first element
			ret = true;
			break; // leave loop
		}
		pPrev = pT;
		pT = pT->pNext;
	}
	// Remove from expired timer list
	pT = pIntExpList;
	pPrev = NULL;
	while (pT)
	{
		if (pT == pTimerElement)
		{
			if (pPrev)
				pPrev->pExpNext = pT->pExpNext; // Remove intermediate element
			else
				pIntExpList = pT->pExpNext; // Remove first element
			ret = true;
			break; // leave loop
		}
		pPrev = pT;
		pT = pT->pExpNext;
	}
	RB_LEAVE_CRITICAL_SECTION;
	return(ret);
}


//--------------------------------------------------------------------------------------------------
// RemoveTimerFromMainTimerList
//--------------------------------------------------------------------------------------------------
//! \brief	Remove timer from main list
//!
//! \param		pTimerElement	Timer list element
//! \return		true if remove was successful (timer found in list)
//--------------------------------------------------------------------------------------------------
static bool RemoveTimerFromMainTimerList(const RB_TIMER_tElement* pTimerElement)
{
	// Note: If corrections are made also correct in RemoveTimerFromIntTimerList
	bool ret = false;
	RB_TIMER_tElement* pT;
	RB_TIMER_tElement* pPrev;
	RB_ENTER_CRITICAL_SECTION;
	// Remove from pending timer list
	pT = pMainTimerList;
	pPrev = NULL;
	while (pT)
	{
		if (pT == pTimerElement)
		{
			if (pPrev)
				pPrev->pNext = pT->pNext; // Remove intermediate element
			else
				pMainTimerList = pT->pNext; // Remove first element
			ret = true;
			break; // leave loop
		}
		pPrev = pT;
		pT = pT->pNext;
	}
	// Remove from expired timer list
	pT = pMainExpList;
	pPrev = NULL;
	while (pT)
	{
		if (pT == pTimerElement)
		{
			if (pPrev)
				pPrev->pExpNext = pT->pExpNext; // Remove intermediate element
			else
				pMainExpList = pT->pExpNext; // Remove first element
			ret = true;
			break; // leave loop
		}
		pPrev = pT;
		pT = pT->pExpNext;
	}
	RB_LEAVE_CRITICAL_SECTION;
	return(ret);
}


//--------------------------------------------------------------------------------------------------
