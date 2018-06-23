//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Debug.h
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
// $Date: 2016/11/10 08:58:39MEZ $
// $Revision: 1.16 $
//
//==================================================================================================

#ifndef _RB_Debug__h
#define _RB_Debug__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

// This module is mandatory (in case of !NDEBUG) and has no RB_CONFIG_USE, no check is needed here.

#include "RB_Sysdefs.h"			// Needed for RB_DECL_FUNC and RB_DECL_TYPE


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// Configuration of Debug Output messages
//
// RB_DEBUG_INFO, _WARN, _FAIL, _ASSERT macros are defined according to RB_ENV_DEBUG_LEVEL
// which is defined in preprocessor configuration.
//--------------------------------------------------------------------------------------------------

//! Debug output levels (defined with #define to be preprocessor compatible)
#define RB_DEBUG_LEVEL_NONE    0    //!< No messages
#define RB_DEBUG_LEVEL_ASSERT  1    //!< Only perform asserts
#define RB_DEBUG_LEVEL_FAIL    2    //!< Perform asserts and output failures
#define RB_DEBUG_LEVEL_WARN    3    //!< Perform asserts and output failures and warnings
#define RB_DEBUG_LEVEL_INFO    4    //!< Perform asserts and output failures, warnings and infos

//! Check configuration for valid values
#if defined(RB_ENV_DEBUG_LEVEL) && ((RB_ENV_DEBUG_LEVEL < RB_DEBUG_LEVEL_NONE) || (RB_ENV_DEBUG_LEVEL > RB_DEBUG_LEVEL_INFO))
	#error Definition of RB_ENV_DEBUG_LEVEL not valid (must be 0 .. 4 or RB_DEBUG_LEVEL_NONE .. RB_DEBUG_LEVEL_INFO)
#endif
#if defined(RB_MODULE_DEBUG_LEVEL) && ((RB_MODULE_DEBUG_LEVEL < RB_DEBUG_LEVEL_NONE) || (RB_MODULE_DEBUG_LEVEL > RB_DEBUG_LEVEL_INFO))
	#error Definition of RB_MODULE_DEBUG_LEVEL not valid (must be 0 .. 4 or RB_DEBUG_LEVEL_NONE .. RB_DEBUG_LEVEL_INFO)
#endif

//! Determine final debug level based on RB_ENV_DEBUG_LEVEL and RB_MODULE_DEBUG_LEVEL
#if defined(RB_ENV_DEBUG_LEVEL) && defined(RB_MODULE_DEBUG_LEVEL)
	// Both defined, use the higher value
	#if (RB_ENV_DEBUG_LEVEL > RB_MODULE_DEBUG_LEVEL)
		#define RB_DEBUG_THRESHOLD	RB_ENV_DEBUG_LEVEL
	#else
		#define RB_DEBUG_THRESHOLD	RB_MODULE_DEBUG_LEVEL
	#endif

#elif defined(RB_ENV_DEBUG_LEVEL)
	// Only RB_ENV_DEBUG_LEVEL defined
	#define RB_DEBUG_THRESHOLD		RB_ENV_DEBUG_LEVEL

#elif defined(RB_MODULE_DEBUG_LEVEL)
	// Only RB_MODULE_DEBUG_LEVEL defined
	#define RB_DEBUG_THRESHOLD		RB_MODULE_DEBUG_LEVEL

#else
	// Nothing defined, use RB_DEBUG_LEVEL_INFO
	#define RB_DEBUG_THRESHOLD		RB_DEBUG_LEVEL_INFO
#endif

#ifdef NDEBUG
	#define RB_DEBUG_ASSERT(e,m)
	#define RB_DEBUG_FAIL(msg)
	#define RB_DEBUG_WARN(msg)
	#define RB_DEBUG_INFO(msg)
#else
	//! Define module name if not already defined. The module name must be defined in C-files before
	//! including any header file. This name is used instead of the predefined __FILE__ to provide
	//! user-friendlier messages.
	#ifndef RB_MODULE_NAME
		#define RB_MODULE_NAME 0
	#endif

	//! Define RB_DEBUG_ASSERT
	#if (RB_DEBUG_THRESHOLD >= RB_DEBUG_LEVEL_ASSERT)
		#define RB_DEBUG_ASSERT(exp,msg)	(/*lint --e(920) */(exp) ? (void)0 : RB_DEBUG_Message(RB_MODULE_NAME, __LINE__, NULL, msg, RB_DEBUG_LEVEL_ASSERT))
	#else
		#define RB_DEBUG_ASSERT(exp,msg)
	#endif

	//! Define RB_DEBUG_FAIL
	#if (RB_DEBUG_THRESHOLD >= RB_DEBUG_LEVEL_FAIL)
		#define RB_DEBUG_FAIL(msg)	RB_DEBUG_Message(RB_MODULE_NAME, __LINE__, NULL, msg, RB_DEBUG_LEVEL_FAIL)
	#else
		#define RB_DEBUG_FAIL(msg)
	#endif

	//! Define RB_DEBUG_WARN
	#if (RB_DEBUG_THRESHOLD >= RB_DEBUG_LEVEL_WARN)
		#define RB_DEBUG_WARN(msg)	RB_DEBUG_Message(RB_MODULE_NAME, __LINE__, NULL, msg, RB_DEBUG_LEVEL_WARN)
	#else
		#define RB_DEBUG_WARN(msg)
	#endif

	//! Define RB_DEBUG_INFO
	#if (RB_DEBUG_THRESHOLD >= RB_DEBUG_LEVEL_INFO)
		#define RB_DEBUG_INFO(msg)	RB_DEBUG_Message(RB_MODULE_NAME, __LINE__, NULL, msg, RB_DEBUG_LEVEL_INFO)
	#else
		#define RB_DEBUG_INFO(msg)
	#endif

#endif


//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//!	Function result
typedef	enum
{
	RB_DEBUG_OK		= 0,	//!< OK
	RB_DEBUG_OUTOFRANGE		//!< Parameter out of valid range
} RB_DECL_TYPE RB_DEBUG_tResult;

//! Prototype of output function used for debug output
typedef RB_DECL_TYPE void (*RB_DEBUG_tOutputFunction)(char ch);

//! Prototype of output mutex callback used for debug output
typedef RB_DECL_TYPE void (*RB_DEBUG_tOutputMutexCallback)(bool lock);


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_DEBUG_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the debug module
//!
//! Initialize will set debug output function and mutex callback to NULL and sets the debug output
//! level to default.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_DEBUG_Initialize(void);


//--------------------------------------------------------------------------------------------------
// RB_DEBUG_Shutdown
//--------------------------------------------------------------------------------------------------
//! \brief	Shutdown the debug module
//!
//! Shutdown will set debug output function and mutex callback to NULL and sets the debug output
//! level to default.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_DEBUG_Shutdown(void);


//--------------------------------------------------------------------------------------------------
// RB_DEBUG_Message
//--------------------------------------------------------------------------------------------------
//! \brief	Output debug messages to debug output channel
//!
//! \param	pFile		Name of module
//! \param	lineNum		Line number (type of __LINE__ is int according to multiple websites)
//! \param	pPrefix		Message prefix, zero terminated string
//! \param	pMessage	Message, zero terminated string
//! \param	level		Level of message
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_DEBUG_Message(const char* pFile, int lineNum, const char* pPrefix, const char* pMessage, int level);

//--------------------------------------------------------------------------------------------------
// RB_DEBUG_GetLevel
//--------------------------------------------------------------------------------------------------
//! \brief	This function reads the actual debug level
//!
//! \return	Value of debug level (RB_DEBUG_LEVEL_NONE .. RB_DEBUG_LEVEL_INFO)
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int RB_DEBUG_GetLevel(void);


//--------------------------------------------------------------------------------------------------
// RB_DEBUG_SetLevel
//--------------------------------------------------------------------------------------------------
//! \brief	This function sets the actual debug level
//!
//! The debug level may be set at runtime within zero to the value defined as RB_DEBUG_THRESHOLD.
//! By default the debug level is set to RB_DEBUG_THRESHOLD upon start up and can be decreased at
//! runtime to reduce the amount of messages.
//! \param	level	The debug level to set (RB_DEBUG_LEVEL_NONE .. RB_DEBUG_LEVEL_INFO)
//! \return
//!     - RB_DEBUG_OK			Level set
//!     - RB_DEBUG_OUTOFRANGE	Level outside valid range
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_DEBUG_tResult RB_DEBUG_SetLevel(int level);


//--------------------------------------------------------------------------------------------------
// RB_DEBUG_GetOutputFunction
//--------------------------------------------------------------------------------------------------
//! \brief	This function reads the actual output function
//!
//! \return	Output function, may be NULL
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_DEBUG_tOutputFunction RB_DEBUG_GetOutputFunction(void);


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
RB_DECL_FUNC void RB_DEBUG_SetOutputFunction(RB_DEBUG_tOutputFunction outputFunction);


//--------------------------------------------------------------------------------------------------
// RB_DEBUG_SetOutputMutexCallback
//--------------------------------------------------------------------------------------------------
//! \brief	This function sets the callback for output mutex
//!
//! The callback function will be called with the argument lock=TRUE before the first character of a
//! debug message is sent to the output function. After completion of output the mutex callback is
//! called again with the argument lock=FALSE. Systems with RTOS behavior should implement a mutex to
//! protect debug messages against different tasks.
//! The mutex callback must be set before the output function is set.
//! The mutex callback function defaults to NULL.
//!
//! \param	callback	Mutex callback function, may be NULL
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_DEBUG_SetOutputMutexCallback(RB_DEBUG_tOutputMutexCallback callback);


//--------------------------------------------------------------------------------------------------
// RB_DEBUG_GetOutputMutexCallback
//--------------------------------------------------------------------------------------------------
//! \brief	This function gets the callback for output mutex
//!
//! See description of RB_DEBUG_SetOutputMutexCallback.
//!
//! \return	Mutex callback function, may be NULL
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_DEBUG_tOutputMutexCallback RB_DEBUG_GetOutputMutexCallback(void);


#ifdef __cplusplus
}
#endif

#endif // _RB_Debug__h
