//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Stopwatch.h
//! \ingroup	util
//! \brief		Measure execution time of C-code statements
//!
//! Start and stop a stopwatch timer to measure the execution time of C-code statements.
//! The number of available stopwatch timers is configurable.
//!
//!
//! Usage 1: Measure time with disabled interrupts (interrupts would distort the measurement).
//!
//!         RB_STOPWATCH_START_IN_SUPER_CRITICAL_SECTION(RB_CONFIG_STOPWATCH_XY);
//!         // < C-code whose execution time is measured >
//!         RB_STOPWATCH_STOP_IN_SUPER_CRITICAL_SECTION(RB_CONFIG_STOPWATCH_XY);
//!
//!
//! Usage 2: Measure time with enabled interrupts (interrupts will distort the measurement).
//!
//!         RB_STOPWATCH_START(RB_CONFIG_STOPWATCH_XY);
//!         // < C-code whose execution time is measured >
//!         RB_STOPWATCH_STOP(RB_CONFIG_STOPWATCH_XY);
//!
//! The MT-SICS command ZZ21 shows the measured times of stopwatch timers.
//! The MT-SICS command ZZ22 resets the measured times of stopwatch timers.
//! The MT-SICS command ZZ01 0 shows information about the underlying microticker.
//!
//! Required configuration in RB_Config.h:
//! -------------------------------------
//! Configure here the identifier and name for each stopwatch timer. Each line must have the syntax
//! RB_STOPWATCH_ITEM(id, name) followed by comma and backslash. An empty line must be present at the end
//! of the table definition.\n
//! \b Tableparameters
//! \li \c id       Stopwatch identifier
//! \li \c name     Descriptive name of stopwatch timer, must be a string
//!
//! \#define RB_CONFIG_STOPWATCH_ITEMTABLE										<backslash>
//!		/*				  Identifier					Name */					<backslash>
//!		RB_STOPWATCH_ITEM(RB_CONFIG_STOPWATCH_RB_INIT,	"Rainbow init"),		<backslash>
//!		RB_STOPWATCH_ITEM(RB_CONFIG_STOPWATCH_SP,		"Signalprocessing"),	<backslash>
//!		RB_STOPWATCH_ITEM(RB_CONFIG_STOPWATCH_WP,		"Postprocessing"),		<backslash>
//!		RB_STOPWATCH_ITEM(RB_CONFIG_STOPWATCH_XY,		"myName"),				<backslash>
//!
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Peter Hertach
//
// $Date: 2017/06/14 17:50:20MESZ $
// $Revision: 1.5 $
//
//==================================================================================================

#ifndef _RB_Stopwatch__h
#define _RB_Stopwatch__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

#if defined(RB_CONFIG_USE_STOPWATCH) && (RB_CONFIG_USE_STOPWATCH == RB_CONFIG_YES)

	//! Attention:
	//! The stopwatch timer must be enclosed by a critical section in order to
	//! ensure that no interrupt occurs during the time measurement (an interrupt
	//! would distort the time measurement). Note that if the interrupts are
	//! disabled too long then some interrupts may be missed.

	//! Start stopwatch timer (interrupts are disabled during time measurement)
	#define RB_STOPWATCH_START_IN_SUPER_CRITICAL_SECTION(id) \
		{ \
			RB_ENTER_SUPER_CRITICAL_SECTION; \
			RB_STOPWATCH_Start(id) /* force trailing semicolon */

	//! Stop stopwatch timer, use only paired with RB_STOPWATCH_START_IN_SUPER_CRITICAL_SECTION
	#define RB_STOPWATCH_STOP_IN_SUPER_CRITICAL_SECTION(id) \
			RB_STOPWATCH_Stop(id); \
			RB_LEAVE_SUPER_CRITICAL_SECTION; \
		} do {} while (0) /* force trailing semicolon */

	//! Start stopwatch timer (interrupts may occur during time measurement)
	#define RB_STOPWATCH_START(id) \
		{ \
			RB_STOPWATCH_Start(id) /* force trailing semicolon */

	//! Stop stopwatch timer, use only paired with RB_STOPWATCH_START
	#define RB_STOPWATCH_STOP(id) \
			RB_STOPWATCH_Stop(id); \
		} do {} while (0) /* force trailing semicolon */

#else // RB_CONFIG_USE_STOPWATCH

	//! Start stopwatch timer (interrupts are disabled during time measurement)
	#define RB_STOPWATCH_START_IN_SUPER_CRITICAL_SECTION(id)

	//! Stop stopwatch timer, use only paired with RB_STOPWATCH_START_IN_SUPER_CRITICAL_SECTION
	#define RB_STOPWATCH_STOP_IN_SUPER_CRITICAL_SECTION(id)

	//! Start stopwatch timer (interrupts may occur during time measurement)
	#define RB_STOPWATCH_START(id)

	//! Stop stopwatch timer, use only paired with RB_STOPWATCH_START
	#define RB_STOPWATCH_STOP(id)

#endif // RB_CONFIG_USE_STOPWATCH


//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

#if defined(RB_CONFIG_USE_STOPWATCH) && (RB_CONFIG_USE_STOPWATCH == RB_CONFIG_YES)

#ifndef RB_CONFIG_STOPWATCH_ITEMTABLE
#error Configuration error: RB_CONFIG_STOPWATCH_ITEMTABLE not defined.
#endif

//! Stopwatch identifiers
//! Define how RB_STOPWATCH_ITEM(id, name) within RB_CONFIG_STOPWATCH_ITEMTABLE must be expanded
#define RB_STOPWATCH_ITEM(id, name) id
typedef enum {
	RB_STOPWATCH_ITEM_NONE = -1,	//!< Dummy item, do not use
	RB_CONFIG_STOPWATCH_ITEMTABLE
	RB_STOPWATCH_NUM_OF_ITEMS		//!< Number of configured items
} RB_DECL_TYPE RB_STOPWATCH_tId;
// clean up
#undef RB_STOPWATCH_ITEM

#define RB_STOPWATCH_ITEM_FIRST		(RB_STOPWATCH_ITEM_NONE + 1)	//!< Index of first configured item
#define RB_STOPWATCH_ITEM_LAST		(RB_STOPWATCH_NUM_OF_ITEMS - 1)	//!< Index of last configured item

//! Time information
typedef struct {
	const char *pName;  //!< Name of stopwatch timer
	double average;		//!< Average time in milliseconds
	double maximum;		//!< Maximum time in milliseconds
	double minimum;		//!< Minimal time in milliseconds
	uint32_t count;		//!< Number of summands in average time
} RB_DECL_TYPE RB_STOPWATCH_tTime;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_STOPWATCH_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize stopwatch timer
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_STOPWATCH_Initialize(void);

//--------------------------------------------------------------------------------------------------
// RB_STOPWATCH_Start
//--------------------------------------------------------------------------------------------------
//! \brief	Start stopwatch timer
//!
//! \param[in]	id		Identifier of stopwatch timer
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_STOPWATCH_Start(RB_STOPWATCH_tId id);

//--------------------------------------------------------------------------------------------------
// RB_STOPWATCH_Stop
//--------------------------------------------------------------------------------------------------
//! \brief	Stop stopwatch timer
//!
//! \param[in]	id		Identifier of stopwatch timer
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_STOPWATCH_Stop(RB_STOPWATCH_tId id);

//--------------------------------------------------------------------------------------------------
// RB_STOPWATCH_Get
//--------------------------------------------------------------------------------------------------
//! \brief	Get measured times of a stopwatch timer
//!
//! \param[in]	id		Identifier of stopwatch timer
//! \param[out]	pTime	Time information
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_STOPWATCH_Get(RB_STOPWATCH_tId id, RB_STOPWATCH_tTime *pTime);

//--------------------------------------------------------------------------------------------------
// RB_STOPWATCH_Reset
//--------------------------------------------------------------------------------------------------
//! \brief	Reset measured times of a stopwatch timer
//!
//! \param[in]	id		Identifier of stopwatch timer
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_STOPWATCH_Reset(RB_STOPWATCH_tId id);


#endif // RB_CONFIG_USE_STOPWATCH

#ifdef __cplusplus
}
#endif

#endif // _RB_Stopwatch__h
