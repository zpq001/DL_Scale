//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		WP_State.c
//! \ingroup	util
//! \brief		Informationboard about system and balance states.
//!
//! This module contains different state informations.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Pius Derungs, Martin Heusser
//
// $Date: 2017/09/26 13:15:22MESZ $
// $Revision: 1.81 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "WP_State"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "WP_State.h"

#if defined(WP_FX_BASIC)

#include "WP_Data.h"

#if !defined(RB_CONFIG_WP_STATE_NUM_OF_SUBSCRIBER)
	#define RB_CONFIG_WP_STATE_NUM_OF_SUBSCRIBER	2
#endif

#if defined(RB_CONFIG_USE_SUBSCRIPTION) && (RB_CONFIG_USE_SUBSCRIPTION == RB_CONFIG_YES)
	#if !defined(RB_CONFIG_WP_STATE_NUM_OF_SUBSCRIBER)
		#error RB_CONFIG_WP_STATE_NUM_OF_SUBSCRIBER must be defined in RB_Config.h
	#endif
	#if defined(RB_CONFIG_WP_STATE_NUM_OF_SUBSCRIBER) && (RB_CONFIG_WP_STATE_NUM_OF_SUBSCRIBER > 0)
		#define SUPPORT_SUBSCRIBE
	#endif
#endif

#ifdef SUPPORT_SUBSCRIBE
	#include "RB_Subscription.h"
	#include <string.h>
#endif

#include "RB_Debug.h"


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================
#ifndef NDEBUG
	//! Uncomment the following line to get more information of sent events
	//#define OUTPUT_STATE_EVENT_INFO		1
#endif

#ifdef SUPPORT_SUBSCRIBE
//! Event subscriptions
static RB_OS_tMsgQueue* SubscriberList[RB_CONFIG_WP_STATE_NUM_OF_SUBSCRIBER];
static RB_SUBSCRIPTION_tCD SubscriptionControlData;
static const RB_SUBSCRIPTION_tCB SubscriptionControlBlock = {SubscriberList,
		RB_ARRAY_SIZE(SubscriberList),
		RB_IDENTIFIERS_SRC_WP_STATE,
		&SubscriptionControlData,
		RB_MODULE_NAME,
		NULL};
#endif

//! Fatal error code
static uint16_t FatalError;

//! WP_State configuration parameter
WP_STATE_tTDParams StateParam;

//! Balance states info
static WP_STATE_tBalState BalState;


//! User calibration and test info
static struct {
//	WP_STATE_tBalUserAdjWeight	AdjSelect;	//!< Adjustment Weight selection (internal or external)
	WP_STATE_tBalUserAdjWeight	TestSelect;	//!< Test Weight selection (internal or external)
	} UserCalInfo;


#if defined(WP_FX_LFT_SEAL_BREAK)
	static bool	LFTseal_IsBroken;		//!< IsBroken flag
	static uint16_t	LFTseal_Increment;	//!< Increment for LFT break counter
#endif

//==================================================================================================
//  F O R W A R D   D E C L A R A T I O N S
//==================================================================================================

#ifdef SUPPORT_SUBSCRIBE
static void SendEvent(WP_STATE_tEvent event, uint16_t newValue);
#endif


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// WP_STATE_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialization of the module STATUS
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_Initialize(void)
	{
	FatalError = 0;
	UserCalInfo.TestSelect = WP_STATE_BAL_USER_ADJ_WGT_EXTERNAL;
	BalState.reqWeight = WP_FLT_ZERO;
	WP_STATE_ResetBalStates();

#if defined(WP_FX_LFT_SEAL_BREAK)
	LFTseal_IsBroken = false;
	LFTseal_Increment = 1u;
#endif

#ifdef SUPPORT_SUBSCRIBE
	RB_SUBSCRIPTION_Create(&SubscriptionControlBlock);
#endif
	}


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetTDParams
//--------------------------------------------------------------------------------------------------
//! \brief	Set typedefinition parameters for the module STATE
//!
//! \param	pUnitParams		Pointer to a set of typedefinition parameters for the module STATE
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetTDParams(const WP_STATE_tTDParams* pParams)
	{
	StateParam = *pParams;
	}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetTDParams
//--------------------------------------------------------------------------------------------------
//! \brief	Get the typedefinition parameterset of the module STATE
//!
//! \return	Pointer to the typedefinition parameterset
//--------------------------------------------------------------------------------------------------
const WP_STATE_tTDParams* WP_STATE_GetTDParams(void)
	{
	return(&StateParam);
	}


//--------------------------------------------------------------------------------------------------
// WP_STATE_Subscribe
//--------------------------------------------------------------------------------------------------
//! \brief	Caller subscribes to messages (events)
//!
//! The number of possible subscriber is defined in RB_Config.h (RB_CONFIG_WP_STATE_NUM_OF_SUBSCRIBER)
//!
//! \param	pMsgQueue	Message queue pointer, to send messages (events)
//! \return	true = OK, false = Too many subscriber -> increase number of subscriber
//--------------------------------------------------------------------------------------------------
bool WP_STATE_Subscribe(RB_OS_tMsgQueue* pMsgQueue)
{
#ifdef SUPPORT_SUBSCRIBE
	return RB_SUBSCRIPTION_Attach(&SubscriptionControlBlock, pMsgQueue);
#else
	RB_UNUSED(pMsgQueue);
	return false;
#endif
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_Unsubscribe
//--------------------------------------------------------------------------------------------------
//! \brief	Caller unsubscribes to messages (events)
//!
//! \param	pMsgQueue	Message queue pointer, to cancel sending messages (events)
//! \return	true = OK, false = No such subscriber
//--------------------------------------------------------------------------------------------------
bool WP_STATE_Unsubscribe(const RB_OS_tMsgQueue* pMsgQueue)
{
#ifdef SUPPORT_SUBSCRIBE
	return RB_SUBSCRIPTION_Detach(&SubscriptionControlBlock, pMsgQueue);
#else
	RB_UNUSED(pMsgQueue);
	return false;
#endif
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetMsgLostCount
//--------------------------------------------------------------------------------------------------
//! \brief	Get the number of lost messages and reset counter
//!
//! \return	Number of lost messages
//--------------------------------------------------------------------------------------------------
uint16_t WP_STATE_GetMsgLostCount(void)
{
#ifdef SUPPORT_SUBSCRIBE
	return RB_SUBSCRIPTION_GetMsgLostCount(&SubscriptionControlBlock);
#else
	return 0u;
#endif
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetLFTMode
//--------------------------------------------------------------------------------------------------
//! \brief	Set the LFT mode
//!
//! \param	mode  LFT mode
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetLFTMode(WP_STATE_tLFTMode mode)
{
#ifdef SUPPORT_SUBSCRIBE
	// Send event on value change
	if (StateParam.LFTMode != mode) {
		StateParam.LFTMode = mode;
		SendEvent(WP_STATE_EVENT_LFT_MODE_CHANGED, mode);
	}
#else
	StateParam.LFTMode = mode;
#endif
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetLFTMode
//--------------------------------------------------------------------------------------------------
//! \brief	Get the LFT mode
//!
//! This function returns the LFT mode having regard to the system operating mode
//!
//! \return	LFT mode
//--------------------------------------------------------------------------------------------------
WP_STATE_tLFTMode WP_STATE_GetLFTMode(void)
{
	return(StateParam.LFTMode);
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetLFTValue
//--------------------------------------------------------------------------------------------------
//! \brief	Get the LFT value, i.e. e = <value> d
//!
//! This function returns the LFT value, regarding the system operating mode
//!
//! \return	LFT value
//!		- WP_STATE_LFTMODE_NONE				returns	1
//!		- WP_STATE_LFTMODE_NOT_APPR			returns	1
//!		- WP_STATE_LFTMODE_1_D_APPR			returns	1
//!		- WP_STATE_LFTMODE_10_D_APPR		returns	10
//!		- WP_STATE_LFTMODE_100_D_APPR		returns	100
//!		- WP_STATE_LFTMODE_1000_D_APPR		returns	1000
//!		- WP_STATE_LFTMODE_10000_D_APPR		returns	10000
//!		- WP_STATE_LFTMODE_100000_D_APPR	returns	100000
//--------------------------------------------------------------------------------------------------
uint32_t WP_STATE_GetLFTValue(void)
{
	uint32_t result = 1u;
	switch (WP_STATE_GetLFTMode()) {
		case WP_STATE_LFTMODE_NONE:          result = 1u;      break;
		case WP_STATE_LFTMODE_NOT_APPR:      result = 1u;      break;
		case WP_STATE_LFTMODE_1_D_APPR:      result = 1u;      break;
		case WP_STATE_LFTMODE_10_D_APPR:     result = 10u;     break;
		case WP_STATE_LFTMODE_100_D_APPR:    result = 100u;    break;
		case WP_STATE_LFTMODE_1000_D_APPR:   result = 1000u;   break;
		case WP_STATE_LFTMODE_10000_D_APPR:  result = 10000u;  break;
		case WP_STATE_LFTMODE_100000_D_APPR: result = 100000u; break;
	}
	return (result);
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetAdjustConfig
//--------------------------------------------------------------------------------------------------
//! \brief	Get adjust configuration
//!
//! This function returns the adjust configuration value, regarding the system operating mode. The
//! bits are encoded according to XP0333 description (see SICS-P manual).
//!
//! \return	adjust configuration
//--------------------------------------------------------------------------------------------------
uint16_t WP_STATE_GetAdjustConfig(void)
	{
	return(StateParam.AdjustConfig);
	}


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetLFTType
//--------------------------------------------------------------------------------------------------
//! \brief	Set the LFT type
//!
//! \param	lftType  LFT type
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetLFTType(WP_STATE_tLFTType lftType)
	{
	StateParam.LFTType = lftType;
	}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetLFTType
//--------------------------------------------------------------------------------------------------
//! \brief	Get the LFT type
//!
//! \return	LFT type
//--------------------------------------------------------------------------------------------------
WP_STATE_tLFTType WP_STATE_GetLFTType(void)
	{
	return(StateParam.LFTType);
	}


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetAccuracyClass
//--------------------------------------------------------------------------------------------------
//! \brief	Set the accuracy class
//!
//! \param	accClass  accuracy class
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetAccuracyClass(WP_STATE_tAccuracyClass accClass)
	{
	StateParam.AccuracyClass = accClass;
	}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetAccuracyClass
//--------------------------------------------------------------------------------------------------
//! \brief	Get the accuracy class
//!
//! \return	accuracy class
//--------------------------------------------------------------------------------------------------
WP_STATE_tAccuracyClass WP_STATE_GetAccuracyClass(void)
	{
	return(StateParam.AccuracyClass);
	}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetDeclaredAccuracyClass
//--------------------------------------------------------------------------------------------------
//! \brief	Get the declared accuracy class for a given range
//!
//! Unlike the function WP_STATE_GetAccuracyClass this function does not return the runtime accuracy
//! class, but reads the declared accuracy class from XP0911.
//!
//! \param	range	Return the accuracy class for this range
//!					0 = total range
//!					RB_CONFIG_WP_RANGES_MAX-1 = smallest fine range
//! \return	accuracy class of the given range
//!			WP_STATE_ACCURACYCLASS_NONE if range is invalid or XP0911 not available
//--------------------------------------------------------------------------------------------------
WP_STATE_tAccuracyClass WP_STATE_GetDeclaredAccuracyClass(int range)
	{
	#if defined(WP_FX_CNFPAR_STD)
	if (range >= 0 && range < RB_CONFIG_WP_RANGES_MAX)
		{
		return((WP_STATE_tAccuracyClass)WP_DATA.XP0911[range]);
		}
	else
		{
		return(WP_STATE_ACCURACYCLASS_NONE);
		}
	#else
	RB_UNUSED(range);
	return(WP_STATE_ACCURACYCLASS_NONE);
	#endif
	}


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetHB44SealingCat
//--------------------------------------------------------------------------------------------------
//! \brief	Set the HB44 sealing category
//!
//! \param	sealingCat	HB44 sealing category
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetHB44SealingCat(WP_STATE_tHB44SealingCat sealingCat)
{
	StateParam.HB44SealingCat = sealingCat;
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetHB44SealingCat
//--------------------------------------------------------------------------------------------------
//! \brief	Get the LFT type (LegalForTrade)
//!
//! \return	HB44 sealing category, WP_STATE_HB44_SEALING_CAT_NONE if not avaialble
//--------------------------------------------------------------------------------------------------
WP_STATE_tHB44SealingCat WP_STATE_GetHB44SealingCat(void)
{
	return(StateParam.HB44SealingCat);
}


#if defined(WP_FX_LFT_SEAL_BREAK)
//--------------------------------------------------------------------------------------------------
// WP_STATE_BreakLFTSeal
//--------------------------------------------------------------------------------------------------
//! \brief	Break LFT seal
//!
//! This function will manage break of LFT behavior. If LFT seal is broken the LFT-Counter
//! is incremented and permanently stored.
//! Breaking LFT occurs on modifying weighing relevant parameters on LFT enabled balances. This covers
//! calibration, linearisation and enabling XP- or XA-commands (required to modify parameters).
//! The state if the seal is actually broken can be read with WP_STATE_IsLFTseal_IsBroken(). The actual
//! value of LFT-Counter can be read with WP_STATE_GetLFTBreakCounter().
//!
//! \param	reason		Reason for breaking seal (WP_STATE_tLFTBreakReason)
//--------------------------------------------------------------------------------------------------
void WP_STATE_BreakLFTSeal(WP_STATE_tLFTBreakReason reason)
	{
	if (!LFTseal_IsBroken)
		{
		bool breakSeal = false;
		switch (reason)
			{
			case WP_STATE_LFT_SEAL_BREAK:
			case WP_STATE_LFT_SEAL_BREAK_FW_CHANGE:
				// other reasons (i.e. XP0000), mark as broken
				breakSeal = true;
				break;

			case WP_STATE_LFT_SEAL_BREAK_EXT_CAL:
				// external calibration, break if  not allowed
				if ((StateParam.AdjustConfig & WP_STATE_LFT_SEAL_CONFIG_BREAK_EXT_CAL_ALLOWED) == 0x00)
					{
					breakSeal = true;
					}
				break;

			case WP_STATE_LFT_SEAL_BREAK_INT_CAL:
				// internal calibration, break if not allowed
				if ((StateParam.AdjustConfig & WP_STATE_LFT_SEAL_CONFIG_BREAK_INT_CAL_ALLOWED) == 0x00)
					{
					breakSeal = true;
					}
				break;

			default:
				RB_DEBUG_WARN("Unknown reason");
				break;
			}

		if (breakSeal)
			{
			uint16_t LFTBreakCounter = WP_STATE_GetLFTBreakCounter();
			RB_DEBUG_INFO("LFT seal broken");
			LFTseal_IsBroken = true;
			LFTBreakCounter += LFTseal_Increment;
			#if defined(RB_CONFIG_WP_PARAM_USE_CONFIG_PARAM) && (RB_CONFIG_WP_PARAM_USE_CONFIG_PARAM == RB_CONFIG_YES)
			if (WP_DATA_StoreParameter(RB_PARAM_ID_XP(323u), &LFTBreakCounter, 0, 0) != WP_OK)
				{
				RB_DEBUG_FAIL("LFT break count could not be stored");
				}
			#endif
			}
		}
	}
#endif


#if defined(WP_FX_LFT_SEAL_BREAK)
//--------------------------------------------------------------------------------------------------
// WP_STATE_SetLFTBreakCounterIncrement
//--------------------------------------------------------------------------------------------------
//! \brief	Set the increment for LFT break counter
//!
//! The default increment is set to one in initialization of module. Since breaking LFT can only be
//! done one time after each startup, this function must be called with a good seed value between
//! 1 and 65535.
//!
//! \param	increment  Increment for LFT break counter
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetLFTBreakCounterIncrement(uint16_t increment)
	{
	LFTseal_Increment = increment;
	}
#endif


#if defined(WP_FX_LFT_SEAL_BREAK)
//--------------------------------------------------------------------------------------------------
// WP_STATE_IsLFTSealBroken
//--------------------------------------------------------------------------------------------------
//! \brief	Return actual state of LFT seal break
//!
//! \return				State of LFT seal break
//--------------------------------------------------------------------------------------------------
bool WP_STATE_IsLFTSealBroken(void)
	{
	return(LFTseal_IsBroken);
	}
#endif


#if defined(WP_FX_LFT_SEAL_BREAK)
//--------------------------------------------------------------------------------------------------
// WP_STATE_GetLFTBreakCounter
//--------------------------------------------------------------------------------------------------
//! \brief	Read LFT break counter
//!
//! \return				LFT break counter
//--------------------------------------------------------------------------------------------------
uint16_t WP_STATE_GetLFTBreakCounter(void)
	{
	#if defined(RB_CONFIG_WP_PARAM_USE_CONFIG_PARAM) && (RB_CONFIG_WP_PARAM_USE_CONFIG_PARAM == RB_CONFIG_YES)
	return(WP_DATA.XP0323);
	#else
	return(0);
	#endif
	}
#endif


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetFatalError
//--------------------------------------------------------------------------------------------------
//! \brief	Set the fatal error
//!
//! \param	err  Error number
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetFatalError(uint16_t err)
	{
	if (StateParam.AcceptFatalError)
		{
		FatalError = err;
		}
	}

//--------------------------------------------------------------------------------------------------
// WP_STATE_GetFatalError
//--------------------------------------------------------------------------------------------------
//! \brief	Get the fatal error number. Zero is defined as no error
//!
//! \return	Error number
//--------------------------------------------------------------------------------------------------
uint16_t WP_STATE_GetFatalError(void)
	{
	return(FatalError);
	}

//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalOpState
//--------------------------------------------------------------------------------------------------
//! \brief	Get balance operating state
//!
//! \return	Balance operating state
//--------------------------------------------------------------------------------------------------
WP_STATE_tBalOperatingState WP_STATE_GetBalOpState(void)
{
	return (BalState.state);
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalAdjustMethode
//--------------------------------------------------------------------------------------------------
//! \brief	Get method of adjustment
//!
//! \return	Adjustment method
//--------------------------------------------------------------------------------------------------
WP_STATE_tBalAdjustMethod WP_STATE_GetBalAdjustMethode(void)
{
	return (BalState.method);
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalAProcState
//--------------------------------------------------------------------------------------------------
//! \brief	Get adjust-processing state
//!
//! \return	Balance adjust processing state
//--------------------------------------------------------------------------------------------------
WP_STATE_tBalAdjProcState WP_STATE_GetBalAProcState(void)
{
	return (BalState.adjState);
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalWRequState
//--------------------------------------------------------------------------------------------------
//! \brief	Get balance weight requesting state
//!
//! \return	Balance weight requesting state
//--------------------------------------------------------------------------------------------------
WP_STATE_tBalWRequState WP_STATE_GetBalWRequState(void)
{
	return (BalState.reqState);
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalWeightState
//--------------------------------------------------------------------------------------------------
//! \brief	Get balance weight state
//!
//! Get state of the requested weight
//!
//! \return	Weight actually requested
//--------------------------------------------------------------------------------------------------
WP_STATE_tBalWgtState WP_STATE_GetBalWeightState(void)
{
    return (BalState.wgtState);
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalWRequWeight
//--------------------------------------------------------------------------------------------------
//! \brief	Get balance weight requesting value
//!
//! \return	Weight actually requested
//--------------------------------------------------------------------------------------------------
WP_tFloat WP_STATE_GetBalWRequWeight(void)
{
	return (BalState.reqWeight);
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalAbortCode
//--------------------------------------------------------------------------------------------------
//! \brief	Get the balance weight requesting state
//!
//! \return	Balance abort code
//--------------------------------------------------------------------------------------------------
WP_STATE_tBalAbortCode WP_STATE_GetBalAbortCode(void)
{
	return (BalState.abortCode);
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalUserAdjMode
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance user adjustment mode
//!
//! \param	mode  	Balance user adjustment mode
//! \return	 State (see type WP_tStatus)
//--------------------------------------------------------------------------------------------------
WP_tStatus WP_STATE_SetBalUserAdjMode(WP_STATE_tBalUserAdjMode mode)
{
	#if defined(WP_FX_FACT)
	WP_DATA_tFACTEnable enable = (WP_DATA_tFACTEnable)mode;
	return WP_DATA_StoreParameter(RB_PARAM_M93, &enable, 0, 0);
	#else
	RB_UNUSED(mode);
	return(WP_STORE_FAIL);
	#endif
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalUserAdjMode
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance user adjustment mode
//!
//! \return	Balance user adjustment mode
//--------------------------------------------------------------------------------------------------
WP_STATE_tBalUserAdjMode WP_STATE_GetBalUserAdjMode(void)
{
	#if defined(WP_FX_FACT)
	return (WP_STATE_tBalUserAdjMode)WP_DATA.M93;
	#else
	return WP_STATE_BAL_USER_ADJ_MODE_MANUAL;
	#endif
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalUserAdjWeight
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance user adjustment weight (internal or external)
//!
//! \param	weight  	Balance user adjustment weight
//! \return	 State (see type WP_tStatus)
//--------------------------------------------------------------------------------------------------
WP_tStatus WP_STATE_SetBalUserAdjWeight(WP_STATE_tBalUserAdjWeight weight)
{
	WP_DATA_tAdjustmentWeightType awt = (WP_DATA_tAdjustmentWeightType)weight;
	return WP_DATA_StoreParameter(RB_PARAM_C0, &awt, 0, 0);
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalUserAdjWeight
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance user adjustment weight (internal or external)
//!
//! \return	Balance user adjustment weight
//--------------------------------------------------------------------------------------------------
WP_STATE_tBalUserAdjWeight WP_STATE_GetBalUserAdjWeight(void)
{
	return (WP_STATE_tBalUserAdjWeight)WP_DATA.C0;
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalUserTstWeight
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance user test weight (internal or external)
//!
//! \param	weight  	Balance user test weight
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetBalUserTstWeight(WP_STATE_tBalUserAdjWeight weight)
{
	UserCalInfo.TestSelect = weight;
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalUserTstWeight
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance user test weight (internal or external)
//!
//! \return	Balance user test weight
//--------------------------------------------------------------------------------------------------
WP_STATE_tBalUserAdjWeight WP_STATE_GetBalUserTstWeight(void)
{
	return UserCalInfo.TestSelect;
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_ResetBalStates
//--------------------------------------------------------------------------------------------------
//! \brief	Reset the state information of BALANCE states
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_ResetBalStates(void)
{
	BalState.state = WP_STATE_BAL_OP_UNDEFINED;
	BalState.method = WP_STATE_ADJ_UNDEFINED;
	BalState.adjState = WP_STATE_BAL_APROC_UNDEFINED;
	BalState.abortCode = WP_STATE_BAL_ACODE_UNDEFINED;
	BalState.reqState = WP_STATE_BAL_WREQU_UNDEFINED;
    BalState.wgtState = WP_STATE_BAL_WGTSTS_UNDEFINED;
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalanceState
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance state
//!
//! This function should only be used by the WP_BALANCE. The event WP_STATE_EVENT_BAL_STATE_CHANGED
//! will be posted if subscription is enabled and any part of state is changed.
//!
//! \param	pNewState  Pointer to new state
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetBalanceState(const WP_STATE_tBalState *pNewState)
{
#ifdef SUPPORT_SUBSCRIBE
	if (memcmp(&BalState, pNewState, sizeof(WP_STATE_tBalState)) != 0)
		{
		BalState = *pNewState;
		SendEvent(WP_STATE_EVENT_BAL_STATE_CHANGED, (uint16_t)0);
		#ifdef OUTPUT_STATE_EVENT_INFO
		RB_DEBUG_INFO("EVENT_BAL_STATE_CHANGED");
		#endif
		}
#else
	BalState = *pNewState;
#endif
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalanceState
//--------------------------------------------------------------------------------------------------
//! \brief	Get balance state
//!
//! Copies the whole balance state to the given pointer
//!
//! \param	pState  Pointer to copy the state to
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_GetBalanceState(WP_STATE_tBalState *pState)
{
	*pState = BalState;
}
//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalOpState
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance operating state
//!
//! \param	state  Balance operating state
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetBalOpState(WP_STATE_tBalOperatingState state)
{
#ifdef SUPPORT_SUBSCRIBE
	// Send event on value change
	if (BalState.state != state) {
		BalState.state = state;
		SendEvent(WP_STATE_EVENT_BAL_OPERATING_STATE_CHANGED, (uint16_t)state);
		#ifdef OUTPUT_STATE_EVENT_INFO
		switch (state)
			{
			case WP_STATE_BAL_OP_UNDEFINED  : RB_DEBUG_INFO("BAL_OP_UNDEFINED")	; break;
			case WP_STATE_BAL_OP_WEIGHING	: RB_DEBUG_INFO("BAL_OP_WEIGHING")	; break;
			case WP_STATE_BAL_OP_TARING		: RB_DEBUG_INFO("BAL_OP_TARING")	; break;
			case WP_STATE_BAL_OP_ZEROING	: RB_DEBUG_INFO("BAL_OP_ZEROING")	; break;
			case WP_STATE_BAL_OP_ADJUSTING  : RB_DEBUG_INFO("BAL_OP_ADJUSTING")	; break;
			case WP_STATE_BAL_OP_INITZERO	: RB_DEBUG_INFO("BAL_OP_INITZERO")	; break;
			default					    	: RB_DEBUG_FAIL("BAL_OP_unknown")	; break;
			}
		#endif
	}
#else
	BalState.state = state;
#endif
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalAdjustMethode
//--------------------------------------------------------------------------------------------------
//! \brief	Set method for adjustment
//!
//! \param	method  Adjustment method
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetBalAdjustMethode(WP_STATE_tBalAdjustMethod method)
{
#ifdef SUPPORT_SUBSCRIBE
	// Send event on value change
	if (BalState.method != method) {
		BalState.method = method;
		SendEvent(WP_STATE_EVENT_BAL_ADJ_METHOD_CHANGED, (uint16_t)method);
		#ifdef OUTPUT_STATE_EVENT_INFO
		switch (method)
			{
			case WP_STATE_ADJ_UNDEFINED                     : RB_DEBUG_INFO("ADJ__UNDEFINED")					; break;
			case WP_STATE_ADJ_USER_TST_EXT                  : RB_DEBUG_INFO("ADJ_USER_TST_EXT")					; break;
			case WP_STATE_ADJ_USER_CAL_EXT                  : RB_DEBUG_INFO("ADJ_USER_CAL_EXT")					; break;
			case WP_STATE_ADJ_USER_CAL_UP_EXT               : RB_DEBUG_INFO("ADJ_USER_CAL_UP_EXT")				; break;
			case WP_STATE_ADJ_USER_CAL_DOWN_EXT             : RB_DEBUG_INFO("ADJ_USER_CAL_DOWN_EXT")			; break;
			case WP_STATE_ADJ_USER_PRODZERO_CAL_EXT         : RB_DEBUG_INFO("ADJ_USER_PRODZERO_CAL_EXT")		; break;
			case WP_STATE_ADJ_USER_3PT_CLIN_EXT             : RB_DEBUG_INFO("ADJ_USER_3PT_CLIN_EXT")			; break;
			case WP_STATE_ADJ_USER_3PT_UP_LIN_EXT           : RB_DEBUG_INFO("ADJ_USER_3PT_UP_LIN_EXT ")			; break;
			case WP_STATE_ADJ_USER_5PT_UP_LIN_EXT           : RB_DEBUG_INFO("ADJ_USER_5PT_UP_LIN_EXT ")			; break;
			case WP_STATE_ADJ_USER_3PT_DOWN_LIN_EXT         : RB_DEBUG_INFO("ADJ_USER_3PT_DOWN_LIN_EXT")		; break;
			case WP_STATE_ADJ_USER_PRODZERO_3PT_UP_LIN_EXT  : RB_DEBUG_INFO("ADJ_USER_PRODZERO_3PT_UP_LIN_EXT")	; break;
			case WP_STATE_ADJ_USER_PRODZERO_5PT_UP_LIN_EXT  : RB_DEBUG_INFO("ADJ_USER_PRODZERO_5PT_UP_LIN_EXT")	; break;
			case WP_STATE_ADJ_USER_3PT_LINADJUST_EXT        : RB_DEBUG_INFO("ADJ_USER_3PT_LINADJUST_EXT")		; break;
			case WP_STATE_ADJ_USER_ZEROADJ_EXT              : RB_DEBUG_INFO("ADJ_USER_ZEROADJ_EXT")				; break;
			case WP_STATE_ADJ_USER_SPANADJUST_EXT           : RB_DEBUG_INFO("ADJ_USER_SPANADJUST_EXT ")			; break;
			case WP_STATE_ADJ_SCALE_CAL_EXT                 : RB_DEBUG_INFO("ADJ_SCALE_CAL_EXT")				; break;
			case WP_STATE_ADJ_SCALE_CAL_UP_EXT              : RB_DEBUG_INFO("ADJ_SCALE_CAL_UP_EXT")				; break;
			case WP_STATE_ADJ_SCALE_CAL_DOWN_EXT            : RB_DEBUG_INFO("ADJ_SCALE_CAL_DOWN_EXT")			; break;
			case WP_STATE_ADJ_SCALE_3PT_FLIN_EXT            : RB_DEBUG_INFO("ADJ_SCALE_3PT_FLIN_EXT")			; break;
			case WP_STATE_ADJ_SCALE_3PT_UP_LIN_EXT          : RB_DEBUG_INFO("ADJ_SCALE_3PT_UP_LIN_EXT")			; break;
			case WP_STATE_ADJ_SCALE_3PT_DOWN_LIN_EXT        : RB_DEBUG_INFO("ADJ_SCALE_3PT_DOWN_LIN_EXT")		; break;
			case WP_STATE_ADJ_SCALE_5PT_FLIN_EXT            : RB_DEBUG_INFO("ADJ_SCALE_5PT_FLIN_EXT")			; break;
			case WP_STATE_ADJ_SCALE_5PT_UP_LIN_EXT          : RB_DEBUG_INFO("ADJ_SCALE_5PT_UP_LIN_EXT")			; break;
			case WP_STATE_ADJ_SCALE_3PT_LINADJUST_EXT       : RB_DEBUG_INFO("ADJ_SCALE_3PT_LINADJUST_EXT")		; break;
			case WP_STATE_ADJ_SCALE_ZEROADJ_EXT             : RB_DEBUG_INFO("ADJ_SCALE_ZEROADJ_EXT")			; break;
			case WP_STATE_ADJ_SCALE_ZEROADJ2_EXT            : RB_DEBUG_INFO("ADJ_SCALE_ZEROADJ2_EXT")			; break;
			case WP_STATE_ADJ_SCALE_SPANADJUST_EXT          : RB_DEBUG_INFO("ADJ_SCALE_SPANADJUST_EXT")			; break;
			case WP_STATE_ADJ_SCALE_LOADHYST_EXT            : RB_DEBUG_INFO("ADJ_SCALE_LOADHYST_EXT")			; break;
			case WP_STATE_ADJ_SCALE_LOADHYST_LINCAL_EXT     : RB_DEBUG_INFO("ADJ_SCALE_LOADHYST_LINCAL_EXT")	; break;
			case WP_STATE_ADJ_SCALE_LOADHYST_5PTLINCAL_EXT  : RB_DEBUG_INFO("ADJ_SCALE_LOADHYST_5PTLINCAL_EXT")	; break;
			case WP_STATE_ADJ_FACTORY_CAL_EXT               : RB_DEBUG_INFO("ADJ_FACTORY_CAL_EXT")				; break;
			case WP_STATE_ADJ_FACTORY_CAL_UP_EXT            : RB_DEBUG_INFO("ADJ_FACTORY_CAL_UP_EXT")			; break;
			case WP_STATE_ADJ_FACTORY_CAL_DOWN_EXT          : RB_DEBUG_INFO("ADJ_FACTORY_CAL_DOWN_EXT")			; break;
			case WP_STATE_ADJ_FACTORY_3PT_CLIN_EXT          : RB_DEBUG_INFO("ADJ_FACTORY_3PT_CLIN_EXT")			; break;
			case WP_STATE_ADJ_FACTORY_3PT_FLIN_EXT          : RB_DEBUG_INFO("ADJ_FACTORY_3PT_FLIN_EXT")			; break;
			case WP_STATE_ADJ_FACTORY_5PT_FLIN_EXT          : RB_DEBUG_INFO("ADJ_FACTORY_5PT_FLIN_EXT")			; break;
			case WP_STATE_ADJ_FACTORY_3PT_UP_LIN_EXT        : RB_DEBUG_INFO("ADJ_FACTORY_3PT_UP_LIN_EXT")		; break;
			case WP_STATE_ADJ_FACTORY_3PT_DOWN_LIN_EXT      : RB_DEBUG_INFO("ADJ_FACTORY_3PT_DOWN_LIN_EXT ")	; break;
			case WP_STATE_ADJ_FACTORY_5PT_UP_LIN_EXT        : RB_DEBUG_INFO("ADJ_FACTORY_5PT_UP_LIN_EXT")		; break;
			case WP_STATE_ADJ_FACTORY_ZEROADJ_EXT           : RB_DEBUG_INFO("ADJ_FACTORY_ZEROADJ_EXT ")			; break;
			case WP_STATE_ADJ_FACTORY_SPANADJUST_EXT        : RB_DEBUG_INFO("ADJ_FACTORY_SPANADJUST_EXT")		; break;
			case WP_STATE_ADJ_PRESCALER_EXT                 : RB_DEBUG_INFO("ADJ_PRESCALER_EXT")				; break;
			case WP_STATE_ADJ_PRESCALEROFFSET_EXT           : RB_DEBUG_INFO("ADJ_PRESCALEROFFSET_EXT ")			; break;
			case WP_STATE_ADJ_USER_TST_INT                  : RB_DEBUG_INFO("ADJ_USER_TST_INT")					; break;
			case WP_STATE_ADJ_USER_CAL_INT                  : RB_DEBUG_INFO("ADJ_USER_CAL_INT")					; break;
			case WP_STATE_ADJ_USER_3PT_CLIN_INT             : RB_DEBUG_INFO("ADJ_USER_3PT_CLIN_INT")			; break;
			case WP_STATE_ADJ_USER_3PT_FLIN1_INT            : RB_DEBUG_INFO("ADJ_USER_3PT_FLIN1_INT")			; break;
			case WP_STATE_ADJ_USER_3PT_FLIN2_INT            : RB_DEBUG_INFO("ADJ_USER_3PT_FLIN2_INT")			; break;
			case WP_STATE_ADJ_USER_2x3PT_FLIN_INT           : RB_DEBUG_INFO("ADJ_USER_2x3PT_FLIN_INT ")			; break;
			case WP_STATE_ADJ_USER_STD_CAL                  : RB_DEBUG_INFO("ADJ_USER_STD_CAL")					; break;
			case WP_STATE_ADJ_PLACEMENT_CAL_INT             : RB_DEBUG_INFO("ADJ_PLACEMENT_CAL_INT")			; break;
			case WP_STATE_ADJ_PLACEMENT_GEOCAL_INT          : RB_DEBUG_INFO("ADJ_PLACEMENT_GEOCAL_INT")			; break;
			case WP_STATE_ADJ_FACTORY_CAL_INT               : RB_DEBUG_INFO("ADJ_FACTORY_CAL_INT")				; break;
			case WP_STATE_ADJ_FACTORY_3PT_CLIN_INT          : RB_DEBUG_INFO("ADJ_FACTORY_3PT_CLIN_INT")			; break;
			case WP_STATE_ADJ_FACTORY_3PT_FLIN_INT          : RB_DEBUG_INFO("ADJ_FACTORY_3PT_FLIN_INT")			; break;
			case WP_STATE_ADJ_FACTORY_2x3PT_FLIN_INT        : RB_DEBUG_INFO("ADJ_FACTORY_2x3PT_FLIN_INT")		; break;
			case WP_STATE_ADJ_FACTORY_STD_CAL               : RB_DEBUG_INFO("ADJ_FACTORY_STD_CAL")				; break;
			case WP_STATE_ADJ_PRESCALER_INT                 : RB_DEBUG_INFO("ADJ_PRESCALER_INT")				; break;
			case WP_STATE_ADJ_USER_RESET                    : RB_DEBUG_INFO("ADJ_USER_RESET")					; break;
			case WP_STATE_ADJ_PLACEMENT_CAL_RESET           : RB_DEBUG_INFO("ADJ_PLACEMENT_CAL_RESET ")			; break;
			case WP_STATE_ADJ_PLACEMENT_GEOCAL_RESET        : RB_DEBUG_INFO("ADJ_PLACEMENT_GEOCAL_RESET")		; break;
			case WP_STATE_ADJ_PLACEMENT_GEOCODE_RESET       : RB_DEBUG_INFO("ADJ_PLACEMENT_GEOCODE_RESET")		; break;
			case WP_STATE_ADJ_SCALE_RESET                   : RB_DEBUG_INFO("ADJ_SCALE_RESET")					; break;
			case WP_STATE_ADJ_FACTORY_RESET                 : RB_DEBUG_INFO("ADJ_FACTORY_RESET")				; break;
			default                                         : RB_DEBUG_FAIL("ADJ__unknown")						; break;
			}
		#endif
	}
#else
	BalState.method = method;
#endif
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalAProcState
//--------------------------------------------------------------------------------------------------
//! \brief	Set Adjust processing state
//!
//! \param	state  Balance adjust processing state
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetBalAProcState(WP_STATE_tBalAdjProcState state)
{
#ifdef SUPPORT_SUBSCRIBE
	// Send event on value change
	if (BalState.adjState != state) {
		BalState.adjState = state;
		SendEvent(WP_STATE_EVENT_BAL_ADJ_PROC_STATE_CHANGED, (uint16_t)state);
		#ifdef OUTPUT_STATE_EVENT_INFO
		switch (state)
			{
			case WP_STATE_BAL_APROC_UNDEFINED	: RB_DEBUG_INFO("BAL_APROC_UNDEFINED")	; break;
			case WP_STATE_BAL_APROC_DONE		: RB_DEBUG_INFO("BAL_APROC_DONE")		; break;
			case WP_STATE_BAL_APROC_ABORT		: RB_DEBUG_INFO("BAL_APROC_ABORT")		; break;
			case WP_STATE_BAL_APROC_CANCEL		: RB_DEBUG_INFO("BAL_APROC_CANCEL")		; break;
			case WP_STATE_BAL_APROC_PUSH_ZERO	: RB_DEBUG_INFO("BAL_APROC_PUSH_ZER")	; break;
			case WP_STATE_BAL_APROC_PUSH_LOAD	: RB_DEBUG_INFO("BAL_APROC_PUSH_LOAD")	; break;
			case WP_STATE_BAL_APROC_PRE_LOAD	: RB_DEBUG_INFO("BAL_APROC_PRE_LOAD")	; break;
			case WP_STATE_BAL_APROC_1ST_ZERO	: RB_DEBUG_INFO("BAL_APROC_1ST_ZERO")	; break;
			case WP_STATE_BAL_APROC_2ND_ZERO	: RB_DEBUG_INFO("BAL_APROC_2ND_ZERO")	; break;
			case WP_STATE_BAL_APROC_3RD_ZERO	: RB_DEBUG_INFO("BAL_APROC_3RD_ZERO")	; break;
			case WP_STATE_BAL_APROC_4TH_ZERO	: RB_DEBUG_INFO("BAL_APROC_4TH_ZERO")	; break;
			case WP_STATE_BAL_APROC_1ST_LOAD	: RB_DEBUG_INFO("BAL_APROC_1ST_LOAD")	; break;
			case WP_STATE_BAL_APROC_2ND_LOAD	: RB_DEBUG_INFO("BAL_APROC_2ND_LOAD")	; break;
			case WP_STATE_BAL_APROC_3RD_LOAD	: RB_DEBUG_INFO("BAL_APROC_3RD_LOAD")	; break;
			case WP_STATE_BAL_APROC_4TH_LOAD	: RB_DEBUG_INFO("BAL_APROC_4TH_LOAD")	; break;
			case WP_STATE_BAL_APROC_5TH_LOAD	: RB_DEBUG_INFO("BAL_APROC_5TH_LOAD")	; break;
			case WP_STATE_BAL_APROC_6TH_LOAD	: RB_DEBUG_INFO("BAL_APROC_6TH_LOAD")	; break;
			case WP_STATE_BAL_APROC_7TH_LOAD	: RB_DEBUG_INFO("BAL_APROC_7TH_LOAD")	; break;
			default					    		: RB_DEBUG_FAIL("BAL_APROC_unknown")	; break;
			}
		#endif
	}
#else
	BalState.adjState = state;
#endif
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalAbortCode
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance abort code
//!
//! \param	code  Balance abort code
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetBalAbortCode(WP_STATE_tBalAbortCode code)
{
	BalState.abortCode = code;
}

//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalWRequStatus
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance weight requesting state
//!
//! \param	state  Balance weight requesting state
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetBalWRequState(WP_STATE_tBalWRequState state)
{
#ifdef SUPPORT_SUBSCRIBE
	// Send event on value change
	if (BalState.reqState != state) {
		BalState.reqState = state;
		SendEvent(WP_STATE_EVENT_BAL_BAL_WREQU_STATE_CHANGED, (uint16_t)state);
		#ifdef OUTPUT_STATE_EVENT_INFO
		switch (state)
			{
			case WP_STATE_BAL_WREQU_UNDEFINED	: RB_DEBUG_INFO("BAL_WREQU_UNDEFINED")	; break;
			case WP_STATE_BAL_WREQU_WAIT_USER	: RB_DEBUG_INFO("BAL_WREQU_WAIT_USER")	; break;
			case WP_STATE_BAL_WREQU_OUT_RANGE	: RB_DEBUG_INFO("BAL_WREQU_OUT_RANGE")	; break;
			case WP_STATE_BAL_WREQU_IN_RANGE	: RB_DEBUG_INFO("BAL_WREQU_IN_RANGE")	; break;
			case WP_STATE_BAL_WREQU_STABLE		: RB_DEBUG_INFO("BAL_WREQU_STABLE")		; break;
			case WP_STATE_BAL_WREQU_ABORT		: RB_DEBUG_INFO("BAL_WREQU_ABORT")		; break;
			default					    		: RB_DEBUG_FAIL("BAL_WREQU_unknown")	; break;
			}
		#endif
	}
#else
	BalState.reqState = state;
#endif
}

//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalWeightStatus
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance weight status
//!
//! Get state of the requested weight
//!
//! \param	state  Balance weight state
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetBalWeightState(WP_STATE_tBalWgtState state)
{
#ifdef SUPPORT_SUBSCRIBE
	// Send event on value change
    if (BalState.wgtState != state) {
        BalState.wgtState = state;
		SendEvent(WP_STATE_EVENT_BAL_BAL_WEIGHT_STATE_CHANGED, (uint16_t)state);
		#ifdef OUTPUT_STATE_EVENT_INFO
		switch (state)
			{
			case WP_STATE_BAL_WGTSTS_UNDEFINED	: RB_DEBUG_INFO("BAL_WGTSTS_UNDEFINED")	; break;
			case WP_STATE_BAL_WGTSTS_BELOW_TOL	: RB_DEBUG_INFO("BAL_WGTSTS_BELOW_TOL")	; break;
			case WP_STATE_BAL_WGTSTS_ABOVE_TOL	: RB_DEBUG_INFO("BAL_WGTSTS_ABOVE_TOL")	; break;
			case WP_STATE_BAL_WGTSTS_IN_TOL   	: RB_DEBUG_INFO("BAL_WGTSTS_IN_TOL")	; break;
			default					    		: RB_DEBUG_FAIL("BAL_WGTSTS_unknown")	; break;
			}
		#endif
	}
#else
	BalState.wgtState = state;
#endif
}


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalWRequWeight
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance weight requesting reference
//!
//! \param	value  Requested weight
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_STATE_SetBalWRequWeight(WP_tFloat value)
{
	BalState.reqWeight = value;
}


//--------------------------------------------------------------------------------------------------


//==================================================================================================
//  L O C A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

#ifdef SUPPORT_SUBSCRIBE
//--------------------------------------------------------------------------------------------------
// SendEvent
//--------------------------------------------------------------------------------------------------
//! \brief	Send event to all subscriber
//!
//! \param	event		Event
//! \param	newValue	New value of enum
//! \return	none
//--------------------------------------------------------------------------------------------------
static void SendEvent(WP_STATE_tEvent event, uint16_t newValue)
{
	// Send new state to subscriber
	RB_SUBSCRIPTION_Notify(&SubscriptionControlBlock, (RB_OS_tEvent)((uint16_t)event * 256u + newValue));
}
#endif


#endif // WP_FX_BASIC
