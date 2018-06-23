//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Text.c
//! \ingroup	util
//! \brief		Functions for the localized text and language handling.
//!
//! This module contains the functions for localized text and language handling.
//!
//! These Websites can be used for text translation:
//!	http://translate.google.com/?hl=de&tab=wT#en/ko/November
//! http://translate.reference.com/translate?query=Dezember&src=de&dst=zh-CN
//!	http://macchiato.com/unicode/convert.html	UTF-8 converter tool
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger
//
// $Date: 2016/12/19 14:27:34MEZ $
// $Revision: 1.65 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Text"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_TEXT) && (RB_CONFIG_USE_TEXT == RB_CONFIG_YES)

#include "RB_Text.h"

#include "RB_String.h"

#if !defined(RB_CONFIG_TEXT_NUM_OF_SUBSCRIBER)
	#define RB_CONFIG_TEXT_NUM_OF_SUBSCRIBER	2
#endif

#if defined(RB_CONFIG_USE_SUBSCRIPTION) && (RB_CONFIG_USE_SUBSCRIPTION == RB_CONFIG_YES)
	#if !defined(RB_CONFIG_TEXT_NUM_OF_SUBSCRIBER)
		#error RB_CONFIG_TEXT_NUM_OF_SUBSCRIBER must be defined in RB_Config.h
	#endif
	#if defined(RB_CONFIG_TEXT_NUM_OF_SUBSCRIBER) && (RB_CONFIG_TEXT_NUM_OF_SUBSCRIBER > 0)
		#define SUPPORT_SUBSCRIBE
	#endif
#endif

#ifdef SUPPORT_SUBSCRIBE
	#include "RB_Subscription.h"
#endif

#include "RB_Debug.h"


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

#ifdef SUPPORT_SUBSCRIBE
//! Event subscriptions
static RB_OS_tMsgQueue* SubscriberList[RB_CONFIG_TEXT_NUM_OF_SUBSCRIBER];
static RB_SUBSCRIPTION_tCD SubscriptionControlData;
static const RB_SUBSCRIPTION_tCB SubscriptionControlBlock = {SubscriberList,
		RB_ARRAY_SIZE(SubscriberList),
		RB_IDENTIFIERS_SRC_TEXT,
		&SubscriptionControlData,
		RB_MODULE_NAME,
		NULL};
#endif

//! Text table, contains all neutral text
static const char* const textTableNeutral[] = {
	#ifdef RB_CONFIG_TEXT_LANGUAGE_N_DEFAULT
	#include RB_CONFIG_TEXT_LANGUAGE_N_DEFAULT
	#endif

	#ifdef RB_CONFIG_TEXT_LANGUAGE_N_USER
	#include RB_CONFIG_TEXT_LANGUAGE_N_USER
	#endif
};

//! Text table, contains all text in all languages
static const char* const textTable[RB_CONFIG_TEXT_NUM_OF_LANGUAGES][(int)RB_TEXT_ID_NUM_OF_ITEMS - ((int)RB_TEXT_ID_NEUTRAL_LAST + 1)] = {
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_0_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_0_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_0_USER
		#include RB_CONFIG_TEXT_LANGUAGE_0_USER
		#endif
	},
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 1
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_1_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_1_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_1_USER
		#include RB_CONFIG_TEXT_LANGUAGE_1_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 2
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_2_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_2_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_2_USER
		#include RB_CONFIG_TEXT_LANGUAGE_2_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 3
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_3_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_3_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_3_USER
		#include RB_CONFIG_TEXT_LANGUAGE_3_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 4
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_4_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_4_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_4_USER
		#include RB_CONFIG_TEXT_LANGUAGE_4_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 5
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_5_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_5_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_5_USER
		#include RB_CONFIG_TEXT_LANGUAGE_5_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 6
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_6_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_6_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_6_USER
		#include RB_CONFIG_TEXT_LANGUAGE_6_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 7
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_7_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_7_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_7_USER
		#include RB_CONFIG_TEXT_LANGUAGE_7_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 8
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_8_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_8_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_8_USER
		#include RB_CONFIG_TEXT_LANGUAGE_8_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 9
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_9_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_9_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_9_USER
		#include RB_CONFIG_TEXT_LANGUAGE_9_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 10
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_10_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_10_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_10_USER
		#include RB_CONFIG_TEXT_LANGUAGE_10_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 11
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_11_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_11_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_11_USER
		#include RB_CONFIG_TEXT_LANGUAGE_11_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 12
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_12_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_12_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_12_USER
		#include RB_CONFIG_TEXT_LANGUAGE_12_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 13
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_13_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_13_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_13_USER
		#include RB_CONFIG_TEXT_LANGUAGE_13_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 14
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_14_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_14_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_14_USER
		#include RB_CONFIG_TEXT_LANGUAGE_14_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 15
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_15_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_15_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_15_USER
		#include RB_CONFIG_TEXT_LANGUAGE_15_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 16
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_16_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_16_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_16_USER
		#include RB_CONFIG_TEXT_LANGUAGE_16_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 17
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_17_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_17_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_17_USER
		#include RB_CONFIG_TEXT_LANGUAGE_17_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 18
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_18_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_18_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_18_USER
		#include RB_CONFIG_TEXT_LANGUAGE_18_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 19
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_19_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_19_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_19_USER
		#include RB_CONFIG_TEXT_LANGUAGE_19_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 20
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_20_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_20_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_20_USER
		#include RB_CONFIG_TEXT_LANGUAGE_20_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 21
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_21_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_21_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_21_USER
		#include RB_CONFIG_TEXT_LANGUAGE_21_USER
		#endif
	},
#endif
#if RB_CONFIG_TEXT_NUM_OF_LANGUAGES > 22
	{
		#ifdef RB_CONFIG_TEXT_LANGUAGE_22_DEFAULT
		#include RB_CONFIG_TEXT_LANGUAGE_22_DEFAULT
		#endif

		#ifdef RB_CONFIG_TEXT_LANGUAGE_22_USER
		#include RB_CONFIG_TEXT_LANGUAGE_22_USER
		#endif
	},
#endif
};

#ifdef RB_CONFIG_TEXT_MT_SICS_LANGUAGE
//! Table for conversion from MT-SICS language number to Rainbow language number
static const RB_TEXT_tLanguage lngMtSicsTbl[] = {
	RB_CONFIG_TEXT_MT_SICS_LANGUAGE
};
#endif // RB_CONFIG_TEXT_MT_SICS_LANGUAGE

//! Actually set language
static RB_TEXT_tLanguage RB_TEXT_Language = (RB_TEXT_tLanguage)0;

//! External text table pointer, initially set to internal table, address of one languages table
static const char* const * textExtTableLngPtr = &(textTable[0][0]);

//! Text table language pointer, initially set to internal table, address of one language table
static const char* const * textTableLanguagePtr = &(textTable[0][0]);

//! Text table neutral pointer, initially set to internal table
static const char* const * textTableNeutralPtr = &(textTableNeutral[0]);


//==================================================================================================
//  F O R W A R D   D E C L A R A T I O N S
//==================================================================================================


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_TEXT_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_TEXT_Initialize(void)
{
#ifdef SUPPORT_SUBSCRIBE
	RB_SUBSCRIPTION_Create(&SubscriptionControlBlock);
#endif
}


//--------------------------------------------------------------------------------------------------
// RB_TEXT_Subscribe
//--------------------------------------------------------------------------------------------------
//! \brief	Caller subscribes to messages (events)
//!
//! The number of possible subscriber is defined in RB_Config.h (RB_CONFIG_TEXT_NUM_OF_SUBSCRIBER)
//! Notify will return the actual language, when RB_TEXT_LanguageSet() is called.
//!
//! \param	pMsgQueue	Message queue pointer, to send messages (events)
//! \return	true = OK, false = Too many subscriber -> increase number of subscriber
//--------------------------------------------------------------------------------------------------
bool RB_TEXT_Subscribe(RB_OS_tMsgQueue* pMsgQueue)
{
#ifdef SUPPORT_SUBSCRIBE
	if (RB_SUBSCRIPTION_Attach(&SubscriptionControlBlock, pMsgQueue)) {
		return true;
	}
	RB_DEBUG_WARN("Too many subscriber");
#else
	RB_UNUSED(pMsgQueue);
#endif
	return false;
}


//--------------------------------------------------------------------------------------------------
// RB_TEXT_Unsubscribe
//--------------------------------------------------------------------------------------------------
//! \brief	Caller unsubscribes to messages (events)
//!
//! \param	pMsgQueue	Message queue pointer, to cancel sending messages (events)
//! \return	true = OK, false = No such subscriber
//--------------------------------------------------------------------------------------------------
bool RB_TEXT_Unsubscribe(const RB_OS_tMsgQueue* pMsgQueue)
{
#ifdef SUPPORT_SUBSCRIBE
	if (RB_SUBSCRIPTION_Detach(&SubscriptionControlBlock, pMsgQueue)) {
			return true;
	}
	RB_DEBUG_WARN("No such subscriber");
#else
	RB_UNUSED(pMsgQueue);
#endif
	return false;
}


//--------------------------------------------------------------------------------------------------
// RB_TEXT_Get
//--------------------------------------------------------------------------------------------------
//! \brief	Return pointer to a text in the actual language.
//!
//! If a text is not defined, a string with "..." is returned.
//!
//! \param	textId		Text identifier of returned text
//! \return	Address of NULL terminated text
//--------------------------------------------------------------------------------------------------
const char* RB_TEXT_Get(RB_TEXT_tId textId)
{
	if (textId < RB_TEXT_ID_NEUTRAL_LAST)
		return textTableNeutralPtr[textId];

	if (textId == RB_TEXT_ID_NEUTRAL_LAST)
		return "...";

	if (textId < RB_TEXT_ID_NUM_OF_ITEMS)
		return textTableLanguagePtr[textId - (RB_TEXT_ID_NEUTRAL_LAST+1)];

	return "...";
}


//--------------------------------------------------------------------------------------------------
// RB_TEXT_GetInLanguage
//--------------------------------------------------------------------------------------------------
//! \brief	Return pointer to a text in the given language.
//!
//! If a text is not defined, a string with "..." is returned.
//!
//! \param	   textId          Text identifier of returned text
//! \param	   language        Language, in which text is returned.
//! \return	Address of NULL terminated text
//--------------------------------------------------------------------------------------------------
const char* RB_TEXT_GetInLanguage(RB_TEXT_tId textId, RB_TEXT_tLanguage language)
{
	if ((int)language >= 0 && (int)language < RB_CONFIG_TEXT_NUM_OF_LANGUAGES) {
		if (textId < RB_TEXT_ID_NEUTRAL_LAST)
			return textTableNeutralPtr[textId];

		if (textId == RB_TEXT_ID_NEUTRAL_LAST)
			return "...";

		if (textId < RB_TEXT_ID_NUM_OF_ITEMS)
			return textTable[language][textId - (RB_TEXT_ID_NEUTRAL_LAST+1)];

		return "...";
	}
	else {
		return "...";
	}
}


//--------------------------------------------------------------------------------------------------
// RB_TEXT_SetExtTextTblPtr
//--------------------------------------------------------------------------------------------------
//! \brief	Set external text table pointers
//!
//! Allows setting of text table pointers, e.g. to an external memory.
//! Setting NULL restores internal table pointer
//!
//! \param	pLanguageTable		Pointer language text table, NULL = use internal table
//! \param	pNeutralTable		Pointer neutral text table, NULL = use internal table
//! \return	Address of NULL terminated text
//--------------------------------------------------------------------------------------------------
void RB_TEXT_SetExtTextTblPtr(const char* const * pLanguageTable, const char* const * pNeutralTable)
{
	if (pLanguageTable) {
		textExtTableLngPtr   = pLanguageTable;
		textTableLanguagePtr = textExtTableLngPtr;
	}
	else {
		textExtTableLngPtr = &(textTable[0][0]);
		textTableLanguagePtr = &(textTable[0][0]);
	}

	if (pNeutralTable) {
		textTableNeutralPtr  = pNeutralTable;
	}
	else {
		textTableNeutralPtr = &(textTableNeutral[0]);
	}
}


//--------------------------------------------------------------------------------------------------
// RB_TEXT_LanguageSet
//--------------------------------------------------------------------------------------------------
//! \brief	Set the actual language.
//!
//! \param	language	Language, in which text are returned.
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_TEXT_LanguageSet(RB_TEXT_tLanguage language)
{
	if ((int)language >= 0 && (int)language < RB_CONFIG_TEXT_NUM_OF_LANGUAGES)
		RB_TEXT_Language = language;
	else
		RB_TEXT_Language = (RB_TEXT_tLanguage)0;

	// Check if not set to external table
	if (textExtTableLngPtr == &(textTable[0][0])) {
		// Internal table, table switching allowed
		textTableLanguagePtr = &(textTable[RB_TEXT_Language][0]);
	}
	else {
		// External table, table switching not possible
		textTableLanguagePtr = textExtTableLngPtr;
	}

#ifdef SUPPORT_SUBSCRIBE
	// Send actual language to subscriber
	//lint -e571 // Warning: Suspicious cast
	RB_SUBSCRIPTION_Notify(&SubscriptionControlBlock, (RB_OS_tEvent)RB_TEXT_Language);
#endif
}


//--------------------------------------------------------------------------------------------------
// RB_TEXT_LanguageGet
//--------------------------------------------------------------------------------------------------
//! \brief	Query the actual language.
//!
//! \return	RB_TEXT_tLanguage
//--------------------------------------------------------------------------------------------------
RB_TEXT_tLanguage RB_TEXT_LanguageGet(void)
{
	return RB_TEXT_Language;
}


#ifdef RB_CONFIG_TEXT_MT_SICS_LANGUAGE
//--------------------------------------------------------------------------------------------------
// RB_TEXT_LanguageGetFromMtSics
//--------------------------------------------------------------------------------------------------
//! \brief	Return the Rainbow internal language number from MT-SICS language numbering system.
//!
//! \param	MT_SICS_Lng		MT-SICS language number.
//! \return	RB_TEXT_tLanguage, i.e. Rainbow internal language number
//--------------------------------------------------------------------------------------------------
RB_TEXT_tLanguage RB_TEXT_LanguageGetFromMtSics(int MT_SICS_Lng)
{
	return (RB_TEXT_tLanguage)lngMtSicsTbl[MT_SICS_Lng];
}


#endif // RB_CONFIG_TEXT_MT_SICS_LANGUAGE

#endif // RB_CONFIG_USE_TEXT == RB_CONFIG_YES
