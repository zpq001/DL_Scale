//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Stopwatch.c
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
// $Revision: 1.8 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Stopwatch"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Stopwatch.h"

#if defined(RB_CONFIG_USE_STOPWATCH) && (RB_CONFIG_USE_STOPWATCH == RB_CONFIG_YES)

#include "RB_Debug.h"
#include "RB_Ticker.h"

#if !defined(RB_CONFIG_TICKER_USE_MICROTIMER)
	#error RB_CONFIG_TICKER_USE_MICROTIMER must be defined to use this module
#endif

#ifdef NDEBUG
	//! For testing purposes, it is supported to enable the stopwatch even in a
	//! release configuration. The stopwatch may disable interrupts for a long
	//! time which significantly affects the runtime behavior. Therefore,
	//! the stopwatch shall be disabled for the final product.
	RB_BUILD_INFO("Attention: Time measurement is enabled and therefore, runtime behavior and interrupt handling may be affected due to additional critical sections")
#endif


//==================================================================================================
//  L O C A L   T Y P E S
//==================================================================================================

//! Stopwatch timer
typedef struct {
	const char *pName;			//!< Name of stopwatch timer
	uint32_t startTick;			//!< Starting time of measurement
	uint32_t countSumDeltaTime;	//!< Number of summands in sumDeltaTime
	uint32_t sumDeltaTime;		//!< Accumulated time in microseconds
	uint32_t maxTime;			//!< Maximal time in microseconds
	uint32_t minTime;			//!< Minimal time in microseconds
} tStopwatch;


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

//! Stopwatch timers
//! Define how RB_STOPWATCH_ITEM(id, name) within RB_CONFIG_STOPWATCH_ITEMTABLE must be expanded
#define RB_STOPWATCH_ITEM(id, name) {name, 0, 0, 0, 0, UINT32_MAX}
static tStopwatch Stopwatch[] = {RB_CONFIG_STOPWATCH_ITEMTABLE};
// clean up
#undef RB_STOPWATCH_ITEM

//! Scale factor from tick to microsecond
static double ScaleTick2Us;

//! Maximal value of tick counter
static uint32_t MaxTick;

//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_STOPWATCH_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize stopwatch timer
//--------------------------------------------------------------------------------------------------
void RB_STOPWATCH_Initialize(void)
{
	ScaleTick2Us = 1e6 / (double)RB_TICKER_GetMicroTickFrequency();
	MaxTick = RB_TICKER_GetMicroTickMaxCount();

	// Design decision:
	// - Stopwatch timers are only initialized by C runtime.
	// - Stopwatch timers are preserved in case of a soft restart.
}


//--------------------------------------------------------------------------------------------------
// RB_STOPWATCH_Start
//--------------------------------------------------------------------------------------------------
//! \brief	Start stopwatch timer
//!
//! \param[in]	id		Identifier of stopwatch timer
//--------------------------------------------------------------------------------------------------
void RB_STOPWATCH_Start(RB_STOPWATCH_tId id)
{
	tStopwatch *stopwatch;
	uint32_t *pStartTick;

	#ifndef NDEBUG
	if ((id < 0) || (id >= (int)RB_ARRAY_SIZE(Stopwatch))) {
		RB_DEBUG_FAIL("Invalid identifier");
		return;
	}
	#endif

	stopwatch = &Stopwatch[id];

	pStartTick = &(stopwatch->startTick);

	// RB_TICKER_GetMicroTickCount() shall be called as late as possible
	// to minimize the time measurement inaccuracy
	*pStartTick = RB_TICKER_GetMicroTickCount();
}


//--------------------------------------------------------------------------------------------------
// RB_STOPWATCH_Stop
//--------------------------------------------------------------------------------------------------
//! \brief	Stop stopwatch timer
//!
//! \param[in]	id		Identifier of stopwatch timer
//--------------------------------------------------------------------------------------------------
void RB_STOPWATCH_Stop(RB_STOPWATCH_tId id)
{
	uint32_t tick;
	tStopwatch *stopwatch;
	uint32_t deltaTick;
	uint32_t deltaTime;
	uint32_t sum;

	// RB_TICKER_GetMicroTickCount() shall be called as early as possible
	// to minimize the time measurement inaccuracy
	// Attention: Unit of RB_TICKER_GetMicroTickCount() is not microseconds
	tick = RB_TICKER_GetMicroTickCount();

	#ifndef NDEBUG
	if ((id < 0) || (id >= (int)RB_ARRAY_SIZE(Stopwatch))) {
		RB_DEBUG_FAIL("Invalid identifier");
		return;
	}
	#endif

	stopwatch = &Stopwatch[id];

	if (tick >= stopwatch->startTick) {
		deltaTick = tick - stopwatch->startTick;
	} else {
		// Consider wrap-around of micro ticker
		deltaTick = MaxTick - stopwatch->startTick;
		deltaTick += tick + 1;
	}

	// Convert to microseconds
	deltaTime = (uint32_t)(ScaleTick2Us * deltaTick);

	// Prevent overflow while summing
	stopwatch->countSumDeltaTime++;
	sum = stopwatch->sumDeltaTime + deltaTime;
	if ((stopwatch->countSumDeltaTime > 0) && (sum >= stopwatch->sumDeltaTime)) {
		// No overflow
		stopwatch->sumDeltaTime = sum;
	} else {
		// Overflow occurs, restart summing
		stopwatch->countSumDeltaTime = 1;
		stopwatch->sumDeltaTime = deltaTime;
	}

	// Maximal time
	if (stopwatch->maxTime < deltaTime) {
		stopwatch->maxTime = deltaTime;
	}

	// Minimal time
	if (stopwatch->minTime > deltaTime) {
		stopwatch->minTime = deltaTime;
	}
}


//--------------------------------------------------------------------------------------------------
// RB_STOPWATCH_Get
//--------------------------------------------------------------------------------------------------
//! \brief	Get measured times of a stopwatch timer
//!
//! \param[in]	id		Identifier of stopwatch timer
//! \param[out]	pTime	Time information
//--------------------------------------------------------------------------------------------------
void RB_STOPWATCH_Get(RB_STOPWATCH_tId id, RB_STOPWATCH_tTime *pTime)
{
	tStopwatch *stopwatch;
	uint32_t count;
	uint32_t sum;
	uint32_t maximum;
	uint32_t minimum;

	#ifndef NDEBUG
	if ((id < 0) || (id >= (int)RB_ARRAY_SIZE(Stopwatch))) {
		RB_DEBUG_FAIL("Invalid identifier");
		return;
	}
	#endif

	stopwatch = &Stopwatch[id];

	RB_ENTER_SUPER_CRITICAL_SECTION;
	count = stopwatch->countSumDeltaTime;
	sum = stopwatch->sumDeltaTime;
	maximum = stopwatch->maxTime;
	minimum = stopwatch->minTime;
	RB_LEAVE_SUPER_CRITICAL_SECTION;

	// Name of stopwatch timer
	pTime->pName = stopwatch->pName;

	// Calculate average time in milliseconds
	if (count) {
		pTime->average = (double)sum / (count * 1000.0);
	} else {
		pTime->average = 0.0;
	}

	// Maximum time in milliseconds
	pTime->maximum = (double)maximum / 1000.0;

	// Minimal time in milliseconds
	pTime->minimum = (double)minimum / 1000.0;

	// Number of summands in average time
	pTime->count = count;
}


//--------------------------------------------------------------------------------------------------
// RB_STOPWATCH_Reset
//--------------------------------------------------------------------------------------------------
//! \brief	Reset measured times of a stopwatch timer
//!
//! \param[in]	id		Identifier of stopwatch timer
//--------------------------------------------------------------------------------------------------
void RB_STOPWATCH_Reset(RB_STOPWATCH_tId id)
{
	tStopwatch *stopwatch;

	#ifndef NDEBUG
	if ((id < 0) || (id >= (int)RB_ARRAY_SIZE(Stopwatch))) {
		RB_DEBUG_FAIL("Invalid identifier");
		return;
	}
	#endif

	stopwatch = &Stopwatch[id];

	RB_ENTER_SUPER_CRITICAL_SECTION;
	stopwatch->countSumDeltaTime = 0;
	stopwatch->sumDeltaTime = 0;
	stopwatch->maxTime = 0;
	stopwatch->minTime = UINT32_MAX;
	RB_LEAVE_SUPER_CRITICAL_SECTION;
}


#endif // RB_CONFIG_USE_STOPWATCH
