//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		srv/WP_Data.c
//! \ingroup	util
//! \brief		Common data storage for weighing package
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Martin Heusser
//
// $Date: 2016/11/10 08:58:42MEZ $
// $Revision: 1.71 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "WP_Data"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "WP_Data.h"

#if defined(WP_FX_BASIC) && defined(WP_FX_PARAM_ANY)
#include <string.h>

//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

//! Callback pointer for data storage
static WP_DATA_tStoreData DataStoreCallback = NULL;

//! Callback pointer for info requests
static WP_DATA_tInfoCallback RuntimeValueRequestCallback = NULL;

//==================================================================================================
//  G L O B A L   D A T A
//==================================================================================================

//! Global valid struct
WP_DATA_tValid WP_DATA_Valid;

//! Global parameter struct
WP_DATA_tData WP_DATA;


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================
//--------------------------------------------------------------------------------------------------
// WP_DATA_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialization of the weighing package data module
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_DATA_Initialize(void)
	{
	// Initialize all to zero
	RB_RESET_STRUCT(WP_DATA);
	RB_RESET_STRUCT(WP_DATA_Valid);
	}


//--------------------------------------------------------------------------------------------------
// WP_DATA_StoreParameter
//--------------------------------------------------------------------------------------------------
//! \brief	Store parameter in permanent memory
//!
//! \param	ident		identifier of parameter
//! \param	pData		pointer to data
//! \param	index0		1st index into 2D array (0 for normal arrays or simple values)
//! \param	index1		2nd index into 2D array (0 for simple values)
//! \return				Status (WP_OK, WP_RANGE_FAIL or WP_STORE_FAIL)
//--------------------------------------------------------------------------------------------------
WP_tStatus WP_DATA_StoreParameter(uint16_t ident, const void *pData, int index0, int index1)
	{
	if (DataStoreCallback)
		{
		return(DataStoreCallback(ident, pData, index0, index1));
		}
	return(WP_STORE_FAIL);
	}

//--------------------------------------------------------------------------------------------------
// WP_DATA_SetStoreDataCallback
//--------------------------------------------------------------------------------------------------
//! \brief	Set callback for storing data
//!
//! \param	pCallback		Pointer to callback function
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_DATA_SetStoreDataCallback(WP_DATA_tStoreData pCallback)
	{
	DataStoreCallback = pCallback;
	}

#if defined(WP_FX_SP_BLK70)
//--------------------------------------------------------------------------------------------------
// WP_DATA_GetGeoCodeCal
//--------------------------------------------------------------------------------------------------
//! \brief	Get Geo code data at calibration
//!
//! This function return either the high resolution code if enabled and valid or the low resolution
//! value. If neither of them are valid, the default value is returned.
//!
//! \param	pGeoCode	 out	Pointer to Geo code value to copy the data to (no copy if NULL)
//! \return				TRUE if data is valid, otherwise FALSE
//--------------------------------------------------------------------------------------------------
bool WP_DATA_GetGeoCodeCal(WP_DATA_tXP0947 *pGeoCode)
	{
	// Preset with low resolution value
	bool valid = (bool)WP_DATA_Valid.XP0377;
	WP_DATA_tXP0947 geoCode = (WP_DATA_tXP0947)WP_DATA.XP0377;
	#if defined(WP_FX_SP_BLK70_HIGHRES)
	if (WP_DATA_Valid.XP0947)
		{
		// Override with high resolution value
		valid = true;
		geoCode = WP_DATA.XP0947;
		}
	#endif
	if (pGeoCode)
		*pGeoCode = geoCode;

	return(valid);
	}
#endif

#if defined(WP_FX_SP_BLK70)
//--------------------------------------------------------------------------------------------------
// WP_DATA_SetGeoCodeCal
//--------------------------------------------------------------------------------------------------
//! \brief	Set Geo code data at calibration
//!
//! \param	geoCode		Geo code
//! \return				Status (WP_OK, WP_RANGE_FAIL or WP_STORE_FAIL)
//--------------------------------------------------------------------------------------------------
WP_tStatus WP_DATA_SetGeoCodeCal(WP_DATA_tXP0947 geoCode)
	{
	return(WP_DATA_StoreParameter(RB_PARAM_ID_XP(947), &geoCode, 0, 0));
	}
#endif

#if defined(WP_FX_SP_BLK70)
//--------------------------------------------------------------------------------------------------
// WP_DATA_GetGeoCodeUse
//--------------------------------------------------------------------------------------------------
//! \brief	Get Geo code data at use
//!
//! This function return either the high resolution code if enabled and valid or the low resolution
//! value. If neither of them are valid, the default value is returned.
//!
//! \param	pGeoCode	 out	Pointer to Geo code value to copy the data to (no copy if NULL)
//! \return				TRUE if data is valid, otherwise FALSE
//--------------------------------------------------------------------------------------------------
bool WP_DATA_GetGeoCodeUse(WP_DATA_tXP0948 *pGeoCode)
	{
	// Preset with low resolution value
	bool valid = (bool)WP_DATA_Valid.XP0390;
	WP_DATA_tXP0948 geoCode = (WP_DATA_tXP0948)WP_DATA.XP0390;
	#if defined(WP_FX_SP_BLK70_HIGHRES)
	if (WP_DATA_Valid.XP0948)
		{
		// Override with high resolution value
		valid = true;
		geoCode = WP_DATA.XP0948;
		}
	#endif
	if (pGeoCode)
		*pGeoCode = geoCode;

	return(valid);
	}
#endif

#if defined(WP_FX_SP_BLK70)
//--------------------------------------------------------------------------------------------------
// WP_DATA_SetGeoCodeUse
//--------------------------------------------------------------------------------------------------
//! \brief	Set Geo code data at use
//!
//! \param	geoCode		Geo code
//! \return				Status (WP_OK, WP_RANGE_FAIL or WP_STORE_FAIL)
//--------------------------------------------------------------------------------------------------
WP_tStatus WP_DATA_SetGeoCodeUse(WP_DATA_tXP0948 geoCode)
	{
	return(WP_DATA_StoreParameter(RB_PARAM_ID_XP(948), &geoCode, 0, 0));
	}
#endif


//--------------------------------------------------------------------------------------------------
// WP_DATA_GetCellNominal
//--------------------------------------------------------------------------------------------------
//! \brief	Get cell nominal
//!
//! Returns the cell nominal which corresponds to XP0308 or 1.0
//!
//! \return		cell nominal in Gram
//--------------------------------------------------------------------------------------------------
WP_tFloat WP_DATA_GetCellNominal(void)
	{
	if (RuntimeValueRequestCallback)
		return(RuntimeValueRequestCallback(WP_DATA_RVR_CELLNOMINAL));
	return(WP_FLT_ONE);
	}


//--------------------------------------------------------------------------------------------------
// WP_DATA_GetSumOfBuiltinWeights
//--------------------------------------------------------------------------------------------------
//! \brief	Get sum of internal weights
//!
//! Return the sum of internal weights from Signalprocessing or XP0368 or 0.6 * cell nominal
//!
//! \return		sum of internal weights in Gram
//--------------------------------------------------------------------------------------------------
WP_tFloat WP_DATA_GetSumOfBuiltinWeights(void)
	{
	if (RuntimeValueRequestCallback)
		return(RuntimeValueRequestCallback(WP_DATA_RVR_SUMOFINTCALWEIGHT));
	return(WP_FLT_ZERO);
	}


//--------------------------------------------------------------------------------------------------
// WP_DATA_SetRuntimeValueRequestCallback
//--------------------------------------------------------------------------------------------------
//! \brief	Set callback function for runtime value request
//!
//! \attention This function is reserved for WP_Balance only
//!
//! \param  callback       Callback function
//--------------------------------------------------------------------------------------------------
void WP_DATA_SetRuntimeValueRequestCallback(WP_DATA_tInfoCallback callback)
	{
	RuntimeValueRequestCallback = callback;
	}


//--------------------------------------------------------------------------------------------------
#endif // WP_FX_BASIC && WP_FX_PARAM_ANY
