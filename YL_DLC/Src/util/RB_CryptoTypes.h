//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_CryptoTypes.h
//! \ingroup	util
//! \brief		Provides crypto specific types
//!
//!	Provides crypto specific types to other Rainbow modules and the application. We need this,
//! because several module have to know crypto specific types.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author	Yves Schenker
//
// $Date: 2016/11/10 08:58:38MEZ $
// $Revision: 1.22 $
//
//==================================================================================================

#ifndef _RB_CryptoTypes__h
#define _RB_CryptoTypes__h


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

#if defined(RB_CONFIG_USE_CRYPTO) && (RB_CONFIG_USE_CRYPTO == RB_CONFIG_YES)

	//! Definition of the crypto device type. The resulting enum is composed from the user defined
	//! crypto devices in the RB_Config.h file and the last entry RB_CRYPTOTYPES_DEVICE_NUM_OF_ITEMS
	//! that counts the number of Crypto devices.
	#define RB_CRYPTO_ITEM(a) a
	typedef enum {
		RB_CRYPTOTYPES_DEVICE_ITEM_NONE = -1,	//!< Dummy item, do not use
		RB_CONFIG_CRYPTO_DEVICE_TABLE
		RB_CRYPTOTYPES_DEVICE_NUM_OF_ITEMS		//!< Number of configured items
	} RB_DECL_TYPE RB_CRYPTO_tDevice;
	#undef RB_CRYPTO_ITEM

#else

	//! If no Crypto is enabled we want to keep the Crypto XP coomands in the command table, in order
	//! to do so we need some definitions since they are needed by the Crypto XP0394, XP0395 command
	//! handlers.
	typedef enum {
		RB_CRYPTOTYPES_DEVICE_ITEM_NONE = -1,	//!< Dummy item, do not use
		RB_CONFIG_CRYPTO_DEVICE_0,
		RB_CRYPTOTYPES_DEVICE_NUM_OF_ITEMS		//!< Number of configured items
	} RB_DECL_TYPE RB_CRYPTO_tDevice;

#endif // (RB_CONFIG_USE_CRYPTO == RB_CONFIG_YES)

#define RB_CRYPTOTYPES_DEVICE_ITEM_FIRST		(RB_CRYPTOTYPES_DEVICE_ITEM_NONE + 1)		//!< Index of first configured item
#define RB_CRYPTOTYPES_DEVICE_ITEM_LAST			(RB_CRYPTOTYPES_DEVICE_NUM_OF_ITEMS - 1)	//!< Index of last configured item

#ifdef __cplusplus
}
#endif

#endif // _RB_CryptoTypes__h
