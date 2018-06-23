//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Timer.h
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
// $Revision: 1.59 $
//
//==================================================================================================

#ifndef _RB_Timer__h
#define _RB_Timer__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

// This module is mandatory and has no RB_CONFIG_USE, no check is needed here.

#include "RB_Typedefs.h"
#include "RB_OS.h"


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//! Timeout type (internal use only!)
typedef enum {
	RB_TIMER_CALLBACK_INT = 0,		//!< Timed callback during interrupt
	RB_TIMER_CALLBACK_MAIN,			//!< Timed callback in main program
	RB_TIMER_MESSAGE_INT			//!< Timed message during interrupt
} RB_DECL_TYPE RB_TIMER_tTimeoutType;

// Defined here for backward compatibility
#define RB_TIMER_MESSAGE	RB_TIMER_MESSAGE_INT

//! Timer list element
typedef struct _RB_TIMER_tElement
{
	RB_TIMER_tTimeoutType			type;			//!< Type: Callback | Message, Interrupt | Main
	uint32_t						milliseconds;	//!< Count will be decremented each timer interrupt
	uint32_t						period;			//!< Constant to reload count if periodic
	struct _RB_TIMER_tElement*		pNext;			//!< Address of next timer list element
	struct _RB_TIMER_tElement*		pExpNext;		//!< Address of next timer expired list element
	union {
		RB_tCallback				pCallback;		//!< Address of callback function
		RB_OS_tMsgQueue*			pMsgQueue;		//!< Address of message queue
	} ptr;
	RB_OS_tMessage					message;		//!< Message, when timed message
} RB_DECL_TYPE RB_TIMER_tElement;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
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
RB_DECL_FUNC void RB_TIMER_Initialize(void);


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
RB_DECL_FUNC bool RB_TIMER_SetTimeout(RB_TIMER_tElement* pTimerElement,
									  RB_tCallback pCallbackFunction,
									  uint32_t milliseconds,
									  bool periodic,
									  bool execOnTimeoutInterrupt);


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
RB_DECL_FUNC bool RB_TIMER_SetTimeoutMessage(RB_TIMER_tElement* pTimerElement,
											 RB_OS_tMsgQueue* pMsgQueue,
											 RB_OS_tSource source,
											 RB_OS_tEvent event,
											 uint32_t milliseconds,
											 bool periodic);


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
RB_DECL_FUNC bool RB_TIMER_CancelTimeout(RB_TIMER_tElement* pTimerElement);


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
RB_DECL_FUNC void RB_TIMER_ExecuteCallbackOnTimeout(void);


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
RB_DECL_FUNC void RB_TIMER_Ticker(void);


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
RB_DECL_FUNC const RB_TIMER_tElement* RB_TIMER_GetTimerElement(void);


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
RB_DECL_FUNC uint32_t RB_TIMER_GetSystemTime(void) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC uint32_t RB_TIMER_GetSystemTimeInSeconds(void) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC void RB_TIMER_BusyDelay(uint32_t ms);


#ifdef __cplusplus
}
#endif

#endif
