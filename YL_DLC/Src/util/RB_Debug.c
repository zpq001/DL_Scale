//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Debug.c
//! \ingroup	util
//! \brief		This module defines debugging macros and functions.
//!
//! Debugging output is configured by NDEBUG and optionally by RB_ENV_DEBUG_LEVEL. If NDEBUG is
//! defined (normally in release configurations) all debugging messages are dropped and does not
//! use any ROM size. If NDEBUG is not defined, the level of debug messages may be controlled by
//! RB_ENV_DEBUG_LEVEL and RB_MODULE_DEBUG_LEVEL:
//! - #define RB_x_DEBUG_LEVEL = 0 or RB_DEBUG_LEVEL_NONE:   Performs nothing
//! - #define RB_x_DEBUG_LEVEL = 1 or RB_DEBUG_LEVEL_ASSERT: Only perform asserts
//! - #define RB_x_DEBUG_LEVEL = 2 or RB_DEBUG_LEVEL_FAIL:   Perform asserts and output failures
//! - #define RB_x_DEBUG_LEVEL = 3 or RB_DEBUG_LEVEL_WARN:   Perform asserts and output failures and warnings
//! - #define RB_x_DEBUG_LEVEL = 4 or RB_DEBUG_LEVEL_INFO:   Perform asserts and output failures, warnings and infos
//!
//! In case both RB_ENV_DEBUG_LEVEL and RB_MODULE_DEBUG_LEVEL are defined, the higher value of these
//! is used. In case nothing is defined RB_DEBUG_LEVEL_INFO is used.
//! This module has no RB_CONFIG_USE switch.
//!
//! \attention	This header file must not directly depend on any RB_Config items
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Martin Heusser
//
// $Date: 2017/05/17 13:40:03MESZ $
// $Revision: 1.23 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Debug"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Debug.h"
// This module is mandatory (in case of !NDEBUG) and has no RB_CONFIG_USE, no check is needed here.

#include "RB_Config.h" // RB_Debug.h does not include RB_Config.h, so we do it here explicitly.

// Define some defaults if not already defined in RB_Config
#ifndef NDEBUG
	#ifndef RB_CONFIG_DEBUG_OUTPUT_EOL
		#define RB_CONFIG_DEBUG_OUTPUT_EOL	"\r\n"
	#endif
	#ifndef RB_CONFIG_DEBUG_OUTPUT_FILENAME
		#define RB_CONFIG_DEBUG_OUTPUT_FILENAME		RB_CONFIG_YES
	#endif
	#ifndef RB_CONFIG_DEBUG_OUTPUT_TIME
		#define RB_CONFIG_DEBUG_OUTPUT_TIME			RB_CONFIG_NO
	#endif
#endif

#if defined(RB_CONFIG_DEBUG_OUTPUT_LED) && (RB_CONFIG_DEBUG_OUTPUT_LED == RB_CONFIG_YES)
	#include "RB_Port.h"
#endif
#if defined(RB_CONFIG_DEBUG_OUTPUT_TIME) && (RB_CONFIG_DEBUG_OUTPUT_TIME == RB_CONFIG_YES)
	#include "RB_Timer.h"
#endif

#if defined(__ICCARM__)
	// Contains the prototype of __aeabi_assert which is overridden, see function header of
	// __aeabi_assert for more details
	#include <assert.h>
#endif

// Note: There is no RB_CONFIG_USE switch for the debugging module, NDEBUG is used instead.


//==================================================================================================
//  L O C A L   D E F I N I T I O N S
//==================================================================================================

// Sanity check for debug output LED configuration (port may be expander port and must not be tested)
#if defined(RB_CONFIG_DEBUG_OUTPUT_LED) && (RB_CONFIG_DEBUG_OUTPUT_LED == RB_CONFIG_YES)
	#if (RB_CONFIG_DEBUG_OUTPUT_LED_BIT == RB_PORT_BIT_NO)
		#error RB_CONFIG_DEBUG_OUTPUT_LED_BIT must not be configured to RB_PORT_NO when RB_CONFIG_DEBUG_OUTPUT_LED is set to RB_CONFIG_YES in RB_Config.h
	#endif
#endif


//==================================================================================================
//  L O C A L   C O N S T A N T S
//==================================================================================================

#ifndef NDEBUG
static const char* const standardPrefix[RB_DEBUG_LEVEL_INFO] = {
	"DCC== ", // RB_DEBUG_LEVEL_ASSERT
	"DCC-- ", // RB_DEBUG_LEVEL_FAIL
	"DCC-  ", // RB_DEBUG_LEVEL_WARN
	"DCC   ", // RB_DEBUG_LEVEL_INFO
	};
#endif


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

#ifndef NDEBUG
static RB_DEBUG_tOutputFunction txFunc = NULL;
static RB_DEBUG_tOutputMutexCallback outputMutexCallback = NULL;
static int outputLevel = RB_DEBUG_LEVEL_INFO;
#endif


//==================================================================================================
//  F O R W A R D   D E C L A R A T I O N S
//==================================================================================================

#ifndef NDEBUG
static void OutputString(const char *pStr);
#if defined(RB_CONFIG_DEBUG_OUTPUT_FILENAME) && (RB_CONFIG_DEBUG_OUTPUT_FILENAME == RB_CONFIG_YES)
	static void OutputLineNumber(int value);
#endif
#if defined(RB_CONFIG_DEBUG_OUTPUT_TIME) && (RB_CONFIG_DEBUG_OUTPUT_TIME == RB_CONFIG_YES)
	static void OutputSystemTime(uint32_t value);
#endif
#endif


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_DEBUG_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the debug module
//!
//! Initialize will set debug output function and mutex callback to NULL and sets the debug output
//! level to RB_DEBUG_THRESHOLD.
//--------------------------------------------------------------------------------------------------
void RB_DEBUG_Initialize(void)
{
	#ifndef NDEBUG

	// Configure output pin if configured
	#if defined(RB_CONFIG_DEBUG_OUTPUT_LED) && (RB_CONFIG_DEBUG_OUTPUT_LED == RB_CONFIG_YES)
		#if RB_CONFIG_DEBUG_OUTPUT_LED_POLARITY == 1
		{
			RB_PORT_InitializePin(RB_CONFIG_DEBUG_OUTPUT_LED_PORT, RB_CONFIG_DEBUG_OUTPUT_LED_BIT, RB_PORT_OUTPUT, 0);
		}
		#else
		{
			RB_PORT_InitializePin(RB_CONFIG_DEBUG_OUTPUT_LED_PORT, RB_CONFIG_DEBUG_OUTPUT_LED_BIT, RB_PORT_OUTPUT, 1);
		}
		#endif // RB_CONFIG_DEBUG_OUTPUT_LED_POLARITY
	#endif // defined(RB_CONFIG_DEBUG_OUTPUT_LED)

	#endif // NDEBUG
}


//--------------------------------------------------------------------------------------------------
// RB_DEBUG_Shutdown
//--------------------------------------------------------------------------------------------------
//! \brief	Shutdown the debug module
//!
//! Shutdown will set debug output function and mutex callback to NULL and set the debug output
//! level to default.
//--------------------------------------------------------------------------------------------------
void RB_DEBUG_Shutdown(void)
{
	#ifndef NDEBUG
	txFunc = NULL;
	outputMutexCallback = NULL;
	outputLevel = RB_DEBUG_LEVEL_INFO;
	#endif // NDEBUG
}


//--------------------------------------------------------------------------------------------------
// RB_DEBUG_Message
//--------------------------------------------------------------------------------------------------
//! \brief	Output debug messages to debug output channel
//!
//! \param	pFile		Name of module
//! \param	lineNum		Line number
//! \param	pPrefix		Message prefix, zero terminated string
//! \param	pMessage	Message, zero terminated string
//! \param	level		Level of message
//--------------------------------------------------------------------------------------------------
void RB_DEBUG_Message(const char* pFile, int lineNum, const char* pPrefix, const char* pMessage, int level)
{
	#ifndef NDEBUG
	if ((level > outputLevel) || (level < RB_DEBUG_LEVEL_ASSERT) || (level > RB_DEBUG_LEVEL_INFO))
		return; // exit: message not desired or out of range

	// Lock output channel
	if (outputMutexCallback)
		outputMutexCallback(true);

	// Output "Level #Line Module: Message"
	if (txFunc)
	{
		OutputString("RB ");
		OutputString((pPrefix) ? pPrefix : standardPrefix[level - 1]);

		#if defined(RB_CONFIG_DEBUG_OUTPUT_TIME) && (RB_CONFIG_DEBUG_OUTPUT_TIME == RB_CONFIG_YES)
		OutputSystemTime(RB_TIMER_GetSystemTime());
		OutputString(" ");
		#endif

		#if defined(RB_CONFIG_DEBUG_OUTPUT_FILENAME) && (RB_CONFIG_DEBUG_OUTPUT_FILENAME == RB_CONFIG_YES)
		// Omit line number and module part when the line number is zero
		if (lineNum > 0)
		{
			OutputString("#");
			OutputLineNumber(lineNum);
			OutputString(" ");
			OutputString((pFile) ? pFile : "<unknown>");
			OutputString(": ");
		}
		#else
		RB_UNUSED(pFile);
		RB_UNUSED(lineNum);
		#endif

		OutputString((pMessage) ? pMessage : "<invalid>");
		OutputString(RB_CONFIG_DEBUG_OUTPUT_EOL);
	}

	// Release output channel
	if (outputMutexCallback)
		outputMutexCallback(false);

	// Activate LED if configured and message level is ASSERT, FAIL or WARN (See RB_Config.h)
	#if defined(RB_CONFIG_DEBUG_OUTPUT_LED) && (RB_CONFIG_DEBUG_OUTPUT_LED == RB_CONFIG_YES)
	if (level <= RB_DEBUG_LEVEL_WARN)
	{
		#if RB_CONFIG_DEBUG_OUTPUT_LED_POLARITY == 1
		RB_PORT_SetPin(RB_CONFIG_DEBUG_OUTPUT_LED_PORT, RB_CONFIG_DEBUG_OUTPUT_LED_BIT, 1);
		#else
		RB_PORT_SetPin(RB_CONFIG_DEBUG_OUTPUT_LED_PORT, RB_CONFIG_DEBUG_OUTPUT_LED_BIT, 0);
		#endif // RB_CONFIG_DEBUG_OUTPUT_LED_POLARITY
	}
	#endif // defined(RB_CONFIG_DEBUG_OUTPUT_LED)

	#else
	RB_UNUSED(pFile);
	RB_UNUSED(lineNum);
	RB_UNUSED(pPrefix);
	RB_UNUSED(pMessage);
	RB_UNUSED(level);
	#endif
}

//--------------------------------------------------------------------------------------------------
// RB_DEBUG_GetLevel
//--------------------------------------------------------------------------------------------------
//! \brief	This function reads the actual debug level
//!
//! \return	Value of debug level (RB_DEBUG_LEVEL_NONE .. RB_DEBUG_LEVEL_INFO)
//--------------------------------------------------------------------------------------------------
int RB_DEBUG_GetLevel(void)
{
	#ifndef NDEBUG
	return(outputLevel);
	#else
	return(RB_DEBUG_LEVEL_NONE);
	#endif
}


//--------------------------------------------------------------------------------------------------
// RB_DEBUG_SetLevel
//--------------------------------------------------------------------------------------------------
//! \brief	This function sets the actual debug level
//!
//! The debug level may be set at runtime within zero to the value defined as RB_DEBUG_LEVEL_INFO.
//! By default the debug level is set to RB_DEBUG_LEVEL_INFO upon start up and can be decreased at
//! runtime to reduce the amount of messages.
//!
//! \param	level	The debug level to set (RB_DEBUG_LEVEL_NONE .. RB_DEBUG_LEVEL_INFO)
//! \return
//!     - RB_DEBUG_OK			Level set
//!     - RB_DEBUG_OUTOFRANGE	Level outside valid range
//--------------------------------------------------------------------------------------------------
RB_DEBUG_tResult RB_DEBUG_SetLevel(int level)
{
	#ifndef NDEBUG
	if ((level >= RB_DEBUG_LEVEL_NONE) && (level <= RB_DEBUG_LEVEL_INFO))
	{
		outputLevel = level;
		return(RB_DEBUG_OK);
	}
	return(RB_DEBUG_OUTOFRANGE);
	#else
	RB_UNUSED(level);
	return(RB_DEBUG_OUTOFRANGE);
	#endif
}


//--------------------------------------------------------------------------------------------------
// RB_DEBUG_GetOutputFunction
//--------------------------------------------------------------------------------------------------
//! \brief	This function reads the actual output function
//!
//! \return	Output function, may be NULL
//--------------------------------------------------------------------------------------------------
RB_DEBUG_tOutputFunction RB_DEBUG_GetOutputFunction(void)
{
	#ifndef NDEBUG
	return(txFunc);
	#else
	return(NULL);
	#endif
}


//--------------------------------------------------------------------------------------------------
// RB_DEBUG_SetOutputFunction
//--------------------------------------------------------------------------------------------------
//! \brief	This function sets the actual output function
//!
//! The output function for debugging messages must be set at start up or later during runtime.
//! The output function defaults to NULL.
//!
//! \param	outputFunction	Output function, may be NULL
//--------------------------------------------------------------------------------------------------
void RB_DEBUG_SetOutputFunction(RB_DEBUG_tOutputFunction outputFunction)
{
	#ifndef NDEBUG
	txFunc = outputFunction;
	#else
	RB_UNUSED(outputFunction);
	#endif
}


//--------------------------------------------------------------------------------------------------
// RB_DEBUG_SetOutputMutexCallback
//--------------------------------------------------------------------------------------------------
//! \brief	This function sets the callback for output mutex
//!
//! The callback function will be called with the argument lock=TRUE before the first character of a
//! debug message is sent to the output function. After completion of output the mutex callback is
//! called again with the argument lock=FALSE. Systems with RTOS behavior should implement a mutex
//! to protect debug messages against different tasks.
//! The mutex callback must be set before the output function is set.
//! The mutex callback function defaults to NULL.
//!
//! \param	callback	Mutex callback function, may be NULL
//--------------------------------------------------------------------------------------------------
void RB_DEBUG_SetOutputMutexCallback(RB_DEBUG_tOutputMutexCallback callback)
{
	#ifndef NDEBUG
	outputMutexCallback = callback;
	#else
	RB_UNUSED(callback);
	#endif
}


//--------------------------------------------------------------------------------------------------
// RB_DEBUG_GetOutputMutexCallback
//--------------------------------------------------------------------------------------------------
//! \brief	This function gets the callback for output mutex
//!
//! See description of RB_DEBUG_SetOutputMutexCallback.
//!
//! \return	Mutex callback function, may be NULL
//--------------------------------------------------------------------------------------------------
RB_DEBUG_tOutputMutexCallback RB_DEBUG_GetOutputMutexCallback(void)
{
	#ifndef NDEBUG
	return(outputMutexCallback);
	#else
	return(NULL);
	#endif
}


//==================================================================================================
//  L O C A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

#ifndef NDEBUG
//--------------------------------------------------------------------------------------------------
// OutputString
//--------------------------------------------------------------------------------------------------
//! \brief	Send string to debug output
//!
//! No output is generated if no output function is available.
//!
//! \param	pStr	pointer to string (may be NULL)
//--------------------------------------------------------------------------------------------------
static void OutputString(const char *pStr)
{
	if (txFunc && pStr)
		while (*pStr)
			txFunc(*pStr++);
}


#if defined(RB_CONFIG_DEBUG_OUTPUT_FILENAME) && (RB_CONFIG_DEBUG_OUTPUT_FILENAME == RB_CONFIG_YES)
//--------------------------------------------------------------------------------------------------
// OutputLineNumber
//--------------------------------------------------------------------------------------------------
//! \brief	Convert a line number to string and send it to debug output
//!
//! This conversion is used only for line numbers in the range 0 to 9999. The output always has four
//! characters with leading zeros, i.e. 0052. No output is generated if no output function is
//! available.
//!
//! \param	value	value to be printed (0 .. 9999)
//--------------------------------------------------------------------------------------------------
static void OutputLineNumber(int value)
{
	if (txFunc && (value >= 0) && (value < 10000))
	{
		int tmp = 1000;
		while(tmp > 0)
		{
			txFunc((char)(value / tmp) + (char)'0');
			value = value % tmp;
			tmp /= 10;
		}
	}
}
#endif


#if defined(RB_CONFIG_DEBUG_OUTPUT_TIME) && (RB_CONFIG_DEBUG_OUTPUT_TIME == RB_CONFIG_YES)
//--------------------------------------------------------------------------------------------------
// OutputSystemTime
//--------------------------------------------------------------------------------------------------
//! \brief	Convert system ticks to string and send it to debug output
//!
//! This conversion is used only for time stamps. The output always has the form ssss.mmm indicating
//! seconds and milliseconds with leading zeros, i.e. "0057.121". No output is generated if no output
//! function is available. Output overflows after 10000 seconds.
//!
//! \param	value	value to be printed (0 .. 10000000)
//--------------------------------------------------------------------------------------------------
static void OutputSystemTime(uint32_t value)
{
	if (txFunc)
	{
		uint32_t tmp = 1000uL * 1000uL;
		while(tmp > 0)
		{
			if (tmp == 100uL)
				txFunc('.');
			txFunc((char)(value / tmp) + (char)'0');
			value = value % tmp;
			tmp /= 10;
		}
	}
}
#endif


#if defined(__ICCARM__)
//--------------------------------------------------------------------------------------------------
// __aeabi_assert
//--------------------------------------------------------------------------------------------------
//! \brief	Override the IAR assert function
//!
//! The assert macro calls the IAR library function __aeabi_assert, which prints out the failed
//! assertion on stderr. Since the __write function which is required to print the message is not
//! available when semihosting is deactivated in the IAR options linking fails.
//! This implementation overrides the IAR library function and prints the failed assertion using
//! RB_DEBUG_Message.
//! This function is only necessary with the IAR compiler.
//!
//! \param	pMessage	Message to print, zero terminated string
//! \param	pFile		Filename, zero terminated string
//! \param	lineNum		Line number
//--------------------------------------------------------------------------------------------------
void __aeabi_assert(const char *pMessage, const char *pFile, int lineNum)
{
	RB_DEBUG_Message(pFile, lineNum, NULL, pMessage, RB_DEBUG_LEVEL_FAIL);
}
#endif
#endif // NDEBUG

//--------------------------------------------------------------------------------------------------
