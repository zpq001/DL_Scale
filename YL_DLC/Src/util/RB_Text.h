//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Text.h
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
// $Revision: 1.52 $
//
//==================================================================================================

#ifndef _RB_Text__h
#define _RB_Text__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_TEXT) && (RB_CONFIG_USE_TEXT == RB_CONFIG_YES)

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

//! Text identifiers, used in RB_TEXT_GET(<text_identifier>)
typedef enum {
	RB_TEXT_ID_ITEM_NONE = -1,	//!< Dummy item, do not use

	// Rainbow internal text Id's, neutral
	// TID_UNDEF = Id 0
	#include "RB_TxtIdentifiersNeutral.inc"

	// User text Id's, neutral
	#ifdef RB_CONFIG_TEXT_IDENTIFIERS_USER_NEUTRAL
	#include RB_CONFIG_TEXT_IDENTIFIERS_USER_NEUTRAL
	#endif

	RB_TEXT_ID_NEUTRAL_LAST,

	// Rainbow internal text Id's, language dependent
	#ifdef RB_CONFIG_TEXT_IDENTIFIERS_DEFAULT
	#include RB_CONFIG_TEXT_IDENTIFIERS_DEFAULT
	#endif


	// User text Id's, language dependent
	#ifdef RB_CONFIG_TEXT_IDENTIFIERS_USER
	#include RB_CONFIG_TEXT_IDENTIFIERS_USER
	#endif

	RB_TEXT_ID_NUM_OF_ITEMS		//!< Number of configured items

} RB_DECL_TYPE RB_TEXT_tId;

#define RB_TEXT_ID_FIRST		(RB_TEXT_ID_NONE + 1)
#define RB_TEXT_ID_LAST			(RB_TEXT_ID_NUM_OF_ITEMS - 1)

//! Languages, according to XP0313
typedef enum {
	RB_CONFIG_TEXT_ENUM_LANGUAGES
	RB_TEXT_LANGUAGE_NUM_OF_ITEMS
} RB_DECL_TYPE RB_TEXT_tLanguage;



//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_TEXT_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_TEXT_Initialize(void);


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
RB_DECL_FUNC bool RB_TEXT_Subscribe(RB_OS_tMsgQueue* pMsgQueue);


//--------------------------------------------------------------------------------------------------
// RB_TEXT_Unsubscribe
//--------------------------------------------------------------------------------------------------
//! \brief	Caller unsubscribes to messages (events)
//!
//! \param	pMsgQueue	Message queue pointer, to cancel sending messages (events)
//! \return	true = OK, false = No such subscriber
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_TEXT_Unsubscribe(const RB_OS_tMsgQueue* pMsgQueue);


//--------------------------------------------------------------------------------------------------
// RB_TEXT_Get
//--------------------------------------------------------------------------------------------------
//! \brief	Return pointer to a text in the actual language.
//!
//! If a text is not defined, a string with "..." is returned.
//!
//! \param	textId		Text identifier of returned text
//! \return	Address of text
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC const char* RB_TEXT_Get(RB_TEXT_tId textId);


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
RB_DECL_FUNC const char* RB_TEXT_GetInLanguage(RB_TEXT_tId textId, RB_TEXT_tLanguage language);


//--------------------------------------------------------------------------------------------------
// RB_TEXT_SetExtTextTblPtr
//--------------------------------------------------------------------------------------------------
//! \brief	Set external text table pointers
//!
//! Allows setting of text table pointers, e.g. to an external memory.
//!
//! \param	pLanguageTable		Pointer language text table, NULL = use internal table
//! \param	pNeutralTable		Pointer neutral text table, NULL = use internal table
//! \return	Address of NULL terminated text
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_TEXT_SetExtTextTblPtr(const char* const * pLanguageTable, const char* const * pNeutralTable);


//--------------------------------------------------------------------------------------------------
// RB_TEXT_LanguageSet
//--------------------------------------------------------------------------------------------------
//! \brief	Set the actual language.
//!
//! \param	language	Language, in which text are returned.
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_TEXT_LanguageSet(RB_TEXT_tLanguage language);


//--------------------------------------------------------------------------------------------------
// RB_TEXT_LanguageGet
//--------------------------------------------------------------------------------------------------
//! \brief	Query the actual language.
//!
//! \return	RB_TEXT_tLanguage
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_TEXT_tLanguage RB_TEXT_LanguageGet(void);


#ifdef RB_CONFIG_TEXT_MT_SICS_LANGUAGE
//--------------------------------------------------------------------------------------------------
// RB_TEXT_LanguageGetFromMtSics
//--------------------------------------------------------------------------------------------------
//! \brief	Return the Rainbow internal language number from MT-SICS language numbering system.
//!
//! \param	MT_SICS_Lng		MT-SICS language number.
//! \return	RB_TEXT_tLanguage, i.e. Rainbow internal language number
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_TEXT_tLanguage RB_TEXT_LanguageGetFromMtSics(int MT_SICS_Lng);

#endif // RB_CONFIG_TEXT_MT_SICS_LANGUAGE


#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_TEXT
#endif // _RB_Text__h
