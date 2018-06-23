//==================================================================================================
//                                          
//==================================================================================================
//
//! \file	IND245/Scale/Tare.c
//! \brief	Handle Scale Tare Process
//!
//! (c) Copyright 2004-2006 Mettler-Toledo Laboratory & Weighing Technologies. All Rights Reserved.
//! \author Zhao YuPeng
//
// $Date: 		
// $State: 
// $Revision: 0.1
//
//==================================================================================================
//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================
 #include <stdio.h>
 #include <string.h>
//--------------------------------------------------------------------------------------------------
// R A I N B O W
//--------------------------------------------------------------------------------------------------
#include "RB_Format.h"
#include "RB_String.h"
//--------------------------------------------------------------------------------------------------
// A P P L I C A T I O N
//--------------------------------------------------------------------------------------------------
#include "ScaleConfig.h"
#include "Scale.h"
#include "Tare.h"
#include "UserParam.h"

extern COMMANDSOURCE tareCommandSource;
TARE g_taredata;

//*******************************************************************
// Tare.c contains the scale tare processing methods
// that are declared in the file Tare.h
//
// Weights & Measures Requirements
// Europe OIML/R76  (IT3)
//     Mulitple chaining may not lead to a reduction in tare.
//     Tare can only be cleared at gross zero.
//     Auto tare is not allowed.
//     Keyboard tare is not allowed.
//
// US  Handbook 44  (8142)
//     Tare can be taken only in gross mode.
//     Tare can only be cleared at gross zero.
//     Keyboard tare can be entered only at gross zero.
//
// Australia (Wade Long via John Moorman)
//     There are no specific requirements for tare interlocking in
//     Weights&Measures-approved applications where there is no direct
//     sale to public (DSP).
//     In Weights&Measures-approved applications where DSP is required,
//     the following rules apply.
//     Pushbutton Tare
//       o  When tare has been entered and in operation, recall of gross
//          weight is not permitted.
//       o  Chain taring is permitted, but it must be incremental.
//          That is, current tare cannot be replaced with a tare of lower
//          value.
//       o  Tare can only be cleared at gross zero.
//       o  Auto-clearing of tare at gross zero is permissible and should
//          be independently selectable.
//     Keyboard (Preset) Tare
//       o  These rules are in addition for those for Pushbutton Tare.
//       o  Keyboard Tare must be displayed in a second display -- one that
//          is clearly differentiated from the primary weight display.
//       o  Keyboard Tare may not be entered when there is already a tare
//          in effect (indicator is in the net mode).
//
// Canada  (Frank Lang via John Moorman)
//     Any auto tare must be identified, including printouts,
//     by "TARE" or "TR".
//     Tare,Net,Gross must be indentified, including printouts.
//     Appropriate lb/kg switching conversion
//     8142 interlocks as shown above.
//     External nomenclature must bilingual (French/English).
//	   Tare must be in rounded increment values.
//
// OK to tare Conditions
//     if weights & measures interlock,
//           tare can only be taken after powerup zero has been captured.
//     tare cannot be captured if scale is over capacity.
//     tare cannot be captured if scale is under zero.
//
// Tare is preserved across power-down/power-up in SharedData.
//
// The operation of multi-ranging is consistent for all markets,
// except Automatic Multi_Ranging is inhibited if (market = U.S. or
// Canada) and LegalforTrade ON.
//    Pushbutton (auto) Tare may be taken in any range
//     if Legal-for-Trade
//         Preset Tare Entries must be within low range and will
//         be rounded to the Low Increment Size (an error "OUT OF
//         RANGE" is given if entry too large).
//     if Not-Legal-for-Trade
//         Preset Tare Entries may be in any range and will be
//         rounded to the increment size for the range where they
//         fall.
//
// AUTO_CLEAR_TARE
// AUTO_CLEAR_TARE clears the tare when it falls below a setup
// threshold.
// AUTO_CLEAR_TARE also clears tare after print.
//
//
// AUTO_TARE
// AUTO_TARE automatically sets tare after the weight exceeds
// a threshold.  Once the weight falls below a reset threshold,
// AUTO_TARE is primed to take a new tare one the weight again
// exceeds the auto tare threshold.
//*******************************************************************

/**---------------------------------------------------------------------
 * Name         : TareInit
 * Description  : initialize TARE struct
 * Prototype in : Tare.h
 * \param    	: *this---pointer to TARE struct
 * \return    	: none
 *---------------------------------------------------------------------*/
void TARE_Init(TARE *this)
{
	this->tareTakenFlag             = 0;
	this->tareReEnabledToBeRetaken  = 0;
	this->autoClearTareArmed 		= 0;
	// this->reInitializeDataMembers(this);
	// TARE_SetTareMode(this, 'G');
	// this->fineTareWeight = 0.0;
	// this->powerUpTareFlag = 0;
	// this->programmableTareWeight = -1.0;
	this->bClearTareOnZero = 0;
	this->bTareWait = 0;
	if (this->onPowerUpTare == 1)         //rerest tare
	{
		TARE_ClearTare(this);
		this->tareChangedFlag = 5;
	}
}

/*---------------------------------------------------------------------*
 * Name         : TARE_InstallReInitialization
 * Prototype in : Tare.h
 * Description  : Callback function for re-initialize TARE data members.
 *                This function will be installed from application.
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void TARE_InstallReInitialization(TARE *this, void (*pCallbackFunction)(void *))
{
    this->reInitializeDataMembers = pCallbackFunction;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_ReInitialization
 * Prototype in : Tare.h
 * Description  : Callback function for re-initialize TARE data members.
 *                This function will be installed from application.
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void TARE_ReInitialization(TARE *this)
{
    this->reInitializeDataMembers(this);
}

/*---------------------------------------------------------------------*
 * Name         : TARE_Calibrate
 * Prototype in : Tare.h
 * Description  : Calibrate tare thresholds and current tare weight.
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void TARE_Calibrate(TARE *this, UNIT_tType oldUnitType, UNIT_tType currUnitType, double *pIncrArray)
{
	// uint8_t   clearTareFlag = 0;
    double tempdoule = 0;
	UNIT *pUnit;
	
	pUnit = (this->pScale->unit);

    this->autoTareThresholdWeight = UNIT_ConvertUnitType(pUnit, pUnit->calUnitType, currUnitType, this->primeAutoTareThresholdWeight);
    this->autoTareResetThresholdWeight = UNIT_ConvertUnitType(pUnit, pUnit->calUnitType, currUnitType, this->primeAutoTareResetThresholdWeight);
    this->autoClearTareThresholdWeight = UNIT_ConvertUnitType(pUnit, pUnit->calUnitType, currUnitType, this->primeAutoClearTareThresholdWeight);

	
	this->fineTareWeight = UNIT_ConvertUnitType(pUnit, oldUnitType, currUnitType, this->fineTareWeight);
    tempdoule = this->fineTareWeight; 
	this->pScale->currentTareWeight = tempdoule;
    
    // XHT_2018 remove
//	accessBRAMParameters(CURRENTTAREWEIGHTADDRESS, (uint8_t *)&((this->pScale->currentTareWeight)), 4, WRITEOPERATION); 
    
	if (this->fineTareWeight >= 0.5 * pIncrArray[0])
		this->tareTakenFlag = 1;
	else
		TARE_ClearTare(this); // clearTareFlag = 1;
	
	if (this->netSignCorrection == 1)
		this->fineStoredWeight = UNIT_ConvertUnitType(pUnit, oldUnitType, currUnitType, this->fineStoredWeight);

	TARE_FormatStoredWeight(this, this->fineStoredWeight);

	// if (clearTareFlag == 1)
	//	TARE_ClearTare(this);
	this->tareChangedFlag = 5;
}

/**---------------------------------------------------------------------
 * Name         : TARE_AutoTare
 * Description  : process tare at each weight cycle
 * Prototype in : Tare.h
 * \param    	: *this---pointer to TARE struct
 * \param    	: *gwt---gross weight
 * \param    	: *r_gwt---rounded gross weight
 * \param    	: bMotion---denote if the scale is in motion state
 * \return    	: none
 *---------------------------------------------------------------------*/
void TARE_AutoTare(TARE *this, double *gwt, double *r_gwt, bool bMotion)
{
//	if (this->tareEnabled)
//	{
	if (this->tareTakenFlag)
	{
		// tare is already taken, waiting to re-arm
		// for taking tare again.
		if (TARE_CheckAutoClearTare(this) == TARE_SUCCESS)
			return;
		if (this->autoTareEnabled && (!bMotion || !this->autoTareCheckMotion))
		{
			if (*r_gwt < this->autoTareResetThresholdWeight)
			{
				// if weight falls below the reset weight,
				// re-enable tare to be taken again when weight
				// exceeds the autotare threshold.
				this->tareReEnabledToBeRetaken = 1;
				this->tareTakenFlag = 0;
			}
		}
	}
	else if (this->tareReEnabledToBeRetaken) // tare is now armed to be retaken
	{
		if (TARE_CheckAutoClearTare(this) == TARE_SUCCESS)
			return;
		if (this->autoTareEnabled && !bMotion)
		{
			if (*r_gwt > this->autoTareThresholdWeight)
			{
				TARE_TakeTare(this, *gwt);
				TARE_SetTareSource(this, AUTO_TARE);
				// g_CurTareID = 0;
				this->tareReEnabledToBeRetaken = 0;
			}
		}
	}
	else
	{
		// tare has not already been taken, waiting
		// to be armed so that auto tare can be taken.
		if (this->autoTareEnabled)
		{
			if (!bMotion || !this->autoTareCheckMotion)
			{
				if (*r_gwt < this->autoTareResetThresholdWeight)
					this->tareReEnabledToBeRetaken = 1;
			}
		}
	}
//	}	// if tare enabled
}

/*---------------------------------------------------------------------*
 * Name         : TARE_CheckAutoClearTare
 * Prototype in : Tare.h
 * Description  : After gross weight is above autoClear threshold,
 *				: tare will be cleared next time gross weight falls below
 *				: autoClear threshold.
 * Return value :
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
TARE_SCALE_tStatus TARE_CheckAutoClearTare(TARE *this)
{
    bool bMotion = MOTION_GetMotion((this->pScale->motion));
    
	if (this->autoClearTareThresholdEnabled)
	{
		if (this->autoClearTareArmed) 
		{
			if (!bMotion || !this->autoClearTareCheckMotion)
			{
				if (this->pScale->roundedGrossWeight < this->autoClearTareThresholdWeight) 
				{			
					TARE_ClearTare(this);
					return TARE_SUCCESS;
				}
			}
		}
		else if (!bMotion && (this->pScale->roundedGrossWeight >= this->autoClearTareThresholdWeight))
				this->autoClearTareArmed = 1;
	}
	return INVALID_TARE_FUNCTION;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_TakeTare
 * Prototype in : TARE_Process.h
 * Description  : take tare
 * Return value : status
 *				:	SUCCESS
 *				: 	TARING_WHEN_POWER_UP_ZERO_NOT_CAPTURED
 *				:	TARING_OVER_CAPACITY
 *				:	TARING_UNDER_ZERO
 *				:	TARE_VALUE_EXCEEDS_LIMIT
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
TARE_SCALE_tStatus TARE_TakeTare(TARE *this, double weight)
{   
	if (!ZERO_GetPowerUpZeroCaptured((this->pScale->zero)))
	{
		return	TARING_WHEN_POWER_UP_ZERO_NOT_CAPTURED;
	}
	if (this->pScale->bOverCapacity)
	{
		return	TARING_OVER_CAPACITY;
	}
	if (ZERO_GetUnderZero((this->pScale->zero)))
	{
		return	TARING_UNDER_ZERO;
	}
	if (weight > SCALE_GetCapacityByRange(this->pScale))
	{
		return	TARE_VALUE_EXCEEDS_LIMIT;
	}
	/* 
	*if net sign correction mode,
	*store the weight in the stored weight field of shared data.
	*/
	if (this->netSignCorrection == 1) 
	{
		TARE_FormatStoredWeight(this, weight);
	}
    this->tareTakenFlag = 1;
    this->tareMode = 'N';
    if (this->pScale->market == MARKET_CANADA && this->pScale->bLegalForTrade) 
    {   // in Canada, tare must be rounded to nearest increment
//        if (selectedApplet == APPLCONFIG_APPL_COUNTINGWEIGHING)
//            this->fineTareWeight = weight; 
//        else
		  this->fineTareWeight = weight; 
//    	    this->fineTareWeight = SCALE_RoundedWeight(weight, SCALE_GetIncrementByWeight(this->pScale, weight));
    }
    else
    	this->fineTareWeight = weight;
	this->tareChangedFlag = 5;
	/*Save current tare parameters for restart   03-march-09  lxw*/
	 this->pScale->currentTareWeight = this->fineTareWeight;//save for next power up lxw
     //XHT_2018 remove
//	accessBRAMParameters(CURRENTTAREWEIGHTADDRESS, (uint8_t *)&((this->pScale->currentTareWeight)), 4, WRITEOPERATION); 
//	accessBRAMParameters(CURRENTTAREMODEADDRESS, (uint8_t *)&(this->tareMode), 1, WRITEOPERATION); 
//	accessBRAMParameters(CURRENTTARESOURCEADDRESS, (uint8_t *)&(this->tareSource), 1, WRITEOPERATION); 
    
	return TARE_SUCCESS;
}


TARE_SCALE_tStatus Pre_TARE_TakeTare(TARE *this, double weight)
{   
	if (!ZERO_GetPowerUpZeroCaptured((this->pScale->zero)))
	{
		return	TARING_WHEN_POWER_UP_ZERO_NOT_CAPTURED;
	}
	if (this->pScale->bOverCapacity)
	{
		return	TARING_OVER_CAPACITY;
	}
	if (ZERO_GetUnderZero((this->pScale->zero)))
	{
		return	TARING_UNDER_ZERO;
	}
//    if ( this->pScale->market == MARKET_SRILANKA )
//    {
//        if ( weight > SCALE_GetCapacityByRange(this->pScale)*0.5 )
//        {
//            return	TARE_VALUE_EXCEEDS_LIMIT;
//        }
//    }
//	else 
//   去皮不超过最大量程
    if ((weight - SCALE_GetCapacityByRange(this->pScale))> NEAR_ZERO)
	{
		return	TARE_VALUE_EXCEEDS_LIMIT;
	}
	/* 
	*if net sign correction mode,
	*store the weight in the stored weight field of shared data.
	*/
	if (this->netSignCorrection == 1) 
	{
		TARE_FormatStoredWeight(this, weight);
	}
    this->tareTakenFlag = 1;
    this->tareMode = 'N';
    if (this->pScale->market == MARKET_CANADA && this->pScale->bLegalForTrade || (this->pScale->market == MARKET_USA_NTEP) ) 
    {   // in Canada, tare must be rounded to nearest increment 
//        if (selectedApplet == APPLCONFIG_APPL_COUNTINGWEIGHING)
//            this->fineTareWeight = weight; 
//        else
//		{		
    	    this->fineTareWeight = SCALE_RoundedWeight(weight, SCALE_GetIncrementByWeight(this->pScale, weight));
//		}
    }
    else
    	this->fineTareWeight = weight;
	this->tareChangedFlag = 5;
	/*Save current tare parameters for restart   03-march-09  lxw*/
	 this->pScale->currentTareWeight = this->fineTareWeight;
     //save for next power up lxw
     // XHT_2018 remove
//	accessBRAMParameters(CURRENTTAREWEIGHTADDRESS, (uint8_t *)&((this->pScale->currentTareWeight)), 4, WRITEOPERATION); 
//	accessBRAMParameters(CURRENTTAREMODEADDRESS, (uint8_t *)&(this->tareMode), 1, WRITEOPERATION); 
//	accessBRAMParameters(CURRENTTARESOURCEADDRESS, (uint8_t *)&(this->tareSource), 1, WRITEOPERATION); 
    
	return TARE_SUCCESS;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_FormatStoredWeight
 * Prototype in : TARE_Process.h
 * Description  : Take Net Sign Corrected Tare
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void TARE_FormatStoredWeight(TARE *this, double weight)
{
    double incr = SCALE_GetIncrementByWeight(this->pScale, weight);
    
    this->fineStoredWeight = weight;
    this->roundedStoredWeight = SCALE_RoundedWeight(weight, incr);
    // L_Select_Format(format, MAX_WT_DIGITS, currentUnits_GetIncrement_Weight(weight));
    // sprintf(storedWeightString,format,roundedStoredWeight);
    RB_FORMAT_Double(this->storedWeightString, SCALE_CalcDP(CONFIG_MAX_WT_DIGITS, incr), (float64)(this->roundedStoredWeight), CONFIG_MAX_WT_DIGITS);
    RB_STRING_AlignRight(this->storedWeightString, CONFIG_MAX_WT_DIGITS);
}

/*---------------------------------------------------------------------*
 * Name         : TARE_TakeNetSignCorrectedTare
 * Prototype in : TARE_Process.h
 * Description  : Take Net Sign Corrected Tare
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
TARE_SCALE_tStatus TARE_TakeNetSignCorrectedTare(TARE *this) 
{
	TARE_SCALE_tStatus status;

	if (this->netSignCorrection == 1 && this->roundedStoredWeight != 0.0)
	{
		this->netSignCorrection = 0;
		status = TARE_TakeTare(this, this->fineStoredWeight);
		if (status == TARE_SUCCESS  && this->pScale->fineGrossWeight < this->fineStoredWeight)
		{
			TARE_SetTareSource(this, PUSHBUTTON_TARE);
		}
		this->netSignCorrection = 1;
		return status;
	}
	else
		return INVALID_TARE_FUNCTION;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_TakePushbuttonTare
 * Syntax       : STATUS TARE_TakePushbuttonTare( float weight );
 * Prototype in : TARE_Process.h
 * Description  : take pushbutton tare
 * Return value : status
 *      		:  return values from TakeTare()
 *      		:  PUSHBUTTON_TARE_NOT_ENABLED
 *				:  TARE_VALUE_TOO_SMALL
 *      		:  SCALE_IN_MOTION
 *      		:  TARE_ALEADY_TAKEN_CHAIN_TARE_NOT_PERMITTED
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
// unsigned char TARE_ProcessTareCommand(TARE *this, float weight) 
// {
//	unsigned char status=TARE_IN_PROGRESS;
//	unsigned char btare_command;
//
//	sd_get(&btare_command,DI_wc0101);
//	if (btare_command)
//	{
//		set_wx_tare(status);	//tare in process
//		if (GetScaleType()=='I'&&TARE_IsPushbuttonTareEnabled(pushButtonTare))
//		{
//			unsigned char terminaltare;//Do IDNET tare in dagger
//			unsigned char IdnetControlPanelstatus;
//			sd_get(&terminaltare,DI_ct0114);
////			if ( terminaltare == 1 || LegalForTrade) 
//			if ( terminaltare == 0 || LegalForTrade) 
//			{
//				IdnetControlPanelstatus =GetIdnetControlPanel();
//				if (IdnetControlPanelstatus == 0 ) 
//				{
//					RequestIDNetPushbuttonTare();
//					SetIdnetControlPanel(TARE_COMMAND);
//				}
//				else if (IdnetControlPanelstatus!=TARE_COMMAND)
//				{
//					status  = INVALID_TARE_FUNCTION_PARAMETER;//ILLEGAL_FUNCTION;
//					set_wx_tare(status);	//tare in process
//					reset_wc_tare();	
//				}
//				return status;
//			}
//		}					
//		status =TARE_TakePushbuttonTare(weight,pushButtonTare,IDNET_TARE_NOT_APPLICABLE);
//	}		
//	return status;
// }

/**---------------------------------------------------------------------
 * Name         : TARE_TakePushbuttonTare
 * Description  : process pushbutton tare at each weight cycle
 * Prototype in : Tare.h
 * \param    	: *this---pointer to TARE struct
 * \param    	: type---tare operation type
 * \param    	: weight---gross weight
 * \param    	: incr---increment value
 * \param    	: bMotion---denote if the scale is in motion state
 * \return    	: none
 *---------------------------------------------------------------------*/
void TARE_TakePushbuttonTare(TARE *this, int type, double weight, double incr,bool bMotion)
{
	double halfIncrement;
//	char *ut;
//	char pbuff[30] = {0};
//	char tareString[12] = {0};
//
//	float tmpRoundedTareWeight = 0.0;
//	UNIT_tSymbol currUnit;
    


    this->tareScaleStatus = TARE_IN_PROGRESS;
	
	halfIncrement = 0.5 * incr;
	
	if (bMotion)
	{ 
		if (this->tareMotionWait == 0)
		{
			this->tareScaleStatus = SCALE_IN_MOTION_DURING_TARE;

		}
		else if (this->tareMotionWait != 99)
		{
			if (this->bTareWait)
			{
    			if (--this->tareWaitCount == 0)
	    		{
		    		this->bTareWait = 0;
			    	this->tareScaleStatus = SCALE_IN_MOTION_DURING_TARE;

				}
			}
        	else
		    {
				this->bTareWait = 1;
			    this->tareWaitCount = CONFIG_WEIGHT_CYCLES_PER_SEC * motionTimeOut[this->tareMotionWait]/10;
			}
		}
	}
	
	
	if (weight < halfIncrement) 
	{
		if (!ZERO_GetPowerUpZeroCaptured((this->pScale->zero)))
		{
			this->tareScaleStatus = TARING_WHEN_POWER_UP_ZERO_NOT_CAPTURED;

		}
		else if (weight < -halfIncrement)
		{
			this->tareScaleStatus = TARING_UNDER_ZERO;

		}
		else
		{
//		    if (selectedApplet == APPLCONFIG_APPL_PEAKWEIGHING && m_PeakApplet_Status == PEAKAPPLET_WAITTOSTART)
//		    {
//		        this->tareScaleStatus = TARE_SUCCESS; 
//		    }
//		    else
//		    {
    			this->tareScaleStatus = TARE_VALUE_TOO_SMALL; 

//		    }
		}
	    
	}
	if (!bMotion)
	{
		switch (this->tareScaleStatus)
		{
			case TARE_IN_PROGRESS:
				this->tareScaleStatus = TARE_TakeTare(this, weight);
				switch (this->tareScaleStatus)
				{
					case TARE_SUCCESS:

						TARE_SetTareSource(this, PUSHBUTTON_TARE); 
						this->bTareWait = 0;
						break;
					
					case TARE_VALUE_EXCEEDS_LIMIT:
					case TARING_OVER_CAPACITY:

					    this->bTareWait = 0;
						break;
					
					case TARING_UNDER_ZERO:

					    this->bTareWait = 0;
						break;
					
					case TARING_WHEN_POWER_UP_ZERO_NOT_CAPTURED:

					    this->bTareWait = 0;
					    break;
					    
					default:
						break;
				}
	//			
	//			if ((this->tareScaleStatus = TARE_TakeTare(this, weight)) == TARE_SUCCESS)
	//				TARE_SetTareSource(this, PUSHBUTTON_TARE);
				break;
			
			case SCALE_IN_MOTION_DURING_TARE:

			    this->bTareWait = 0;
				break;
			
			default:
				break;
		}
	}
//	if ((this->tareScaleStatus == TARE_IN_PROGRESS) && (!bMotion))
//	{
//		if ((this->tareScaleStatus = TARE_TakeTare(this, weight)) == TARE_SUCCESS)
//			TARE_SetTareSource(this, PUSHBUTTON_TARE);
//	}		
}

/*---------------------------------------------------------------------*
 * Name         : TARE:: SetKeyboardTare
 * Syntax       : STATUS TARE:: SetKeyboardTare( float weight);
 * Prototype in : TARE_Process.h
 * Description  : Set Keyboard Tare
 *              : keyboard tare is additive.
 * Return value : status
 *      		:  return values from TakeTare()
 *      		:  KEYBOARD_TARE_NOT_ENABLED
 *				:  TARE_VALUE_TOO_SMALL
 *      		:  TARE_ALEADY_TAKEN_CHAIN_TARE_NOT_PERMITTED
 *				:  TARE_NOT_IN_ROUNDED_INCREMENT_VALUE
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
//
//unsigned char TARE_ProcessKeyboardTareCommand(char currentunit) 
//{
//	float weight,incr;
//	unsigned char status=TARE_IN_PROGRESS;
////	unsigned char bAdditiveTareEnable;
//
//// in Europe, preset tare cannot exceed the the limit of the first range in a
//// Multiple Range system.  Tare must be processed by Mettler High Precision
//// Base when in legal for trade mode and Multiple Range.
//
//	sd_get(&weight,DI_wk0104);
//	if (weight>=0)
//	{
//		set_wx_tare(status);	//tare in process
//		if (GetScaleType()=='I'&&TARE_IsKeyboardTareEnabled(keyboardEnteredTare)==true)
//		{
//			unsigned char terminaltare;//Do IDNET tare in dagger
//			unsigned char IdnetControlPanelstatus;
//			sd_get(&terminaltare,DI_ct0114);
//			if ( terminaltare == 1 || LegalForTrade) 
//			if ( terminaltare == 0 || LegalForTrade) 
//			{
//				IdnetControlPanelstatus =GetIdnetControlPanel();
//				if (IdnetControlPanelstatus == 0 ) 
//				{
//					incr = currentUnits_GetIncrement_Weight(weight);
//					weight=RoundedWeight(weight,incr); 	//rounded preset tare 
//					weight =ConvertToThisUnits(currentunit,PRIME_UNIT,weight);
//					IDNET_RequestIDNetKeyboardTare(weight);
//					SetIdnetControlPanel(KEYBOARDTARE_COMMAND);
//				}
//				else if (IdnetControlPanelstatus!=KEYBOARDTARE_COMMAND)
//				{
//					status  = INVALID_TARE_FUNCTION_PARAMETER;//ILLEGAL_FUNCTION;
//					set_wx_tare(status);	//tare in process
//					weight = -1;
//					sd_set(&weight,DI_wk0104);
//				}
//				return status;
//			}	
//		}
//		status =TARE_SetKeyboardTare(weight,keyboardEnteredTare,IDNET_TARE_NOT_APPLICABLE);
//	}
//	return status;
//}

void TARE_SetKeyboardTare(TARE *this, int type, double weight, double incr) 
{
	// float 	incr;
	double temp = 0.0;
	double roundedWeight= 0.0;
    double incrArray[3];

//    char pbuff[30] = {0};
//    char tareString[12] = {0};
//    char *ut;
//    int32_t dp;



    
// in Europe, preset tare cannot exceed the the limit of the first range in a
// Multiple Range system.  Tare must be processed by Mettler High Precision
// Base when in legal for trade mode and Multiple Range.
//		if ( market == 'E' || market == 'A' )
//			if ( weight > currentUnits->GetCapacityWeight(1) )
//				return	TARE_VALUE_EXCEEDS_LIMIT;
    this->tareScaleStatus = TARE_IN_PROGRESS;

//	if (type != plcKeyboardEnteredTare)
//	{
//	if (!this->keyboardTareEnabled || !this->tareEnabled)
    if (!this->keyboardTareEnabled)
	{
		if (tareCommandSource == COMMAND_LOCAL)
		    this->tareScaleStatus = PROGRAMMABLE_TARE_NOT_ENABLED;
	}
//	}
	// incr = SCALE_GetIncrementByWeight(this->pScale, weight);
	// In the Canadian market, keyboard tare must be
	// entered in a rounded increment value.

	if (this->pScale->market == MARKET_CANADA && this->pScale->bLegalForTrade) 
	{
//	    if (selectedApplet != APPLCONFIG_APPL_COUNTINGWEIGHING)
//	    {
    		roundedWeight = SCALE_RoundedWeight(weight, incr);
    		temp = roundedWeight - weight;
    		if (temp < 0.0)
    		    temp = -temp;
//    		if (temp > 0.0001 * incr)
			if (temp > NEAR_ZERO)
    		{
    			this->tareScaleStatus = TARE_NOT_IN_ROUNDED_INCREMENT_VALUE;

    		}
//	    }
	}

	if (weight < 0.5 * incr)
	{
	    SCALE_GetIncrementArray(this->pScale, &incrArray[0]);      // Get increments array of all ranges
	    
		if (!ZERO_GetPowerUpZeroCaptured((this->pScale->zero)))
		{
			this->tareScaleStatus = TARING_WHEN_POWER_UP_ZERO_NOT_CAPTURED;

		}
		else if (weight < -0.5 * incrArray[0]) 
		{
			this->tareScaleStatus = TARING_UNDER_ZERO;

		}
		else
		{
			TARE_ClearTare(this);    	        
            this->tareScaleStatus = TARE_SUCCESS;

		}
	}
    
//    if (selectedApplet == APPLCONFIG_APPL_COUNTINGWEIGHING)
//        temp = weight; 
//    else
        temp = SCALE_RoundedWeight(weight, incr); 	//rounded preset tare
    
	if (this->tareScaleStatus == TARE_IN_PROGRESS)
	{
	    this->tareScaleStatus = Pre_TARE_TakeTare(this, temp);
	    switch (this->tareScaleStatus)
	    {
	        case TARE_SUCCESS:
	            TARE_SetTareSource(this, KEYBOARD_TARE);

	            break;
	        
			case TARE_VALUE_EXCEEDS_LIMIT:
			case TARING_OVER_CAPACITY:

				break;
			
			case TARING_UNDER_ZERO:

				break;
			
			case TARING_WHEN_POWER_UP_ZERO_NOT_CAPTURED:

			    break;
	    }
	}
}

/*---------------------------------------------------------------------*
 * Name         : TARE_ClearTare
 * Prototype in : TARE_Process.h
 * Description  : Clear Tare
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
TARE_SCALE_tStatus TARE_ClearTare(TARE *this)
{
	this->fineTareWeight        = 0.0;
	this->tareTakenFlag 	 	= 0;
	this->tareChangedFlag       = 5;
	this->autoClearTareArmed    = 0;
	TARE_SetTareSource(this, PUSHBUTTON_TARE);
	TARE_SetTareMode(this, 'G');
	TARE_FormatStoredWeight(this, 0.0);
	// g_CurTareID =0;
//	this->pScale->currentTareWeight = this->fineTareWeight;
    //save for next power up lxw
    //XHT_2018
//	accessBRAMParameters(CURRENTTAREWEIGHTADDRESS, (uint8_t *)&((this->pScale->currentTareWeight)), 4, WRITEOPERATION); 
	return TARE_SUCCESS;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_PublicClearTare
 * Syntax       : unsigned char TARE_PublicClearTare()
 * Prototype in : TARE_Process.h
 * Description  : Clear Tare
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
//
//unsigned char TARE_ProcessClearCommand(void)
//{
//	unsigned char bclear_command;
//	unsigned char status=0;	
//	
//	sd_get(&bclear_command,DI_wc0102);
//	if (bclear_command) 
//	{
//		set_wx_clear(1);	//clear tare command in process;
//		if ( GetScaleType() == 'I' &&TARE_IsTareEnabled(clearButton) == true)
//		{
//
//			unsigned char terminaltare;//Do IDNET tare in dagger
//			unsigned char IdnetControlPanelstatus;
//			sd_get(&terminaltare,DI_ct0114);
////			if ( terminaltare == 1 || LegalForTrade) 
//			if ( terminaltare == 0 || LegalForTrade) 
//			{
//				IdnetControlPanelstatus =GetIdnetControlPanel();
//				if (IdnetControlPanelstatus == 0 ) 
//				{
//					RequestIDNetClearTare();
//					SetIdnetControlPanel(CLEAR_COMMAND);
//				}
//				else if (IdnetControlPanelstatus!=CLEAR_COMMAND)
//				{
//					status  = INVALID_TARE_FUNCTION_PARAMETER;//ILLEGAL_FUNCTION;
//					set_wx_clear(status); // clear command fail!
//					reset_wc_clear();
//				}
//				return status;
//			}	
//		}	
//		status = TARE_PublicClearTare(clearButton);
//	}    	
//	return status;
// }

TARE_SCALE_tStatus TARE_PublicClearTare(TARE *this, int type)
{
//	if (type == plcClearButton || this->tareEnabled)
//    if (this->tareEnabled)
    return TARE_ClearTare(this);
//	else
//		return PROGRAMMABLE_TARE_NOT_ENABLED;
}

/*---------------------------------------------------------------------*
 * Name         : TareChanged
 * Prototype in : TARE_Process.h
 * Description  : return indication whether tare changed
 * Return value : TRUE = Tare changed
 *              : FALSE= Same tare
 *---------------------------------------------------------------------*/
bool TARE_TareChanged(TARE *this) 
{
	if (this->tareChangedFlag)
	    return true;
	else
	    return false;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_ClearTareChanged
 * Prototype in : Tare.h
 * Description  : clear indication whether tare changed
 * Return value : none
 *---------------------------------------------------------------------*/
void TARE_ClearTareChanged(TARE *this) 
{
	if (this->tareChangedFlag != 0)
		this->tareChangedFlag--;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_SetTareChanged
 * Prototype in : Tare.h
 * Description  : set indication that tare changed
 * Return value : none
 *---------------------------------------------------------------------*/
void TARE_SetTareChanged(TARE *this) 
{
	this->tareChangedFlag = 5;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_TareSource
 * Syntax       : BOOL TARE_TareSource()
 * Prototype in : TARE_Process.h
 * Description  : return tare source
 * Return value : tare source
 *  Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
uint8_t * TARE_GetTareSource(TARE *this) 
{
	return &this->tareSource;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_TareSource
 * Prototype in : Tare.h
 * Description  : return tare source string
 * Return value : pointer to tare source string
 *  Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
char * GetTareSourceString(TARE *this) 
{
	return this->tareSourceString;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_SetTareSource
 * Prototype in : Tare.h
 * Description  : set tare source
 * Paremeters   : source : 'G' = gross
 *				         : 'N' = net
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void TARE_SetTareSource(TARE *this, uint8_t source)
{
	this->tareSource = source;
    //XHT_2018
//    accessBRAMParameters(CURRENTTARESOURCEADDRESS, (uint8_t *)&(this->tareSource), 1, WRITEOPERATION); 
	switch (source)
	{
		case AUTO_TARE:
		case PUSHBUTTON_TARE:
			RB_STRING_strncpymax(this->tareSourceString, "T", 3);
			break;
			
		case KEYBOARD_TARE:
			RB_STRING_strncpymax(this->tareSourceString, "PT", 3);
			break;

		case TARE_MEMORY:
			RB_STRING_strncpymax(this->tareSourceString, "M", 3);
			break;
	    
	    case NO_TARE:
	    default:
	        RB_STRING_strncpymax(this->tareSourceString, " ", 3);
			break;
	}
}

/*---------------------------------------------------------------------*
 * Name         : GetFineTareWeight
 * Prototype in : TARE_Process.h
 * Description  : return fine tare weight
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
double TARE_GetFineTareWeight(TARE *this)
{
	return this->fineTareWeight;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_PrintComplete_CheckClearTare
 * Syntax       : void TARE_PrintComplete_CheckClearTare()
 * Prototype in : Tare.h
 * Description  : print has completed.  if auto clear tare after print
 *				: complete, then clear tare.
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
TARE_SCALE_tStatus TARE_PrintCompleteCheckClearTare(TARE *this)
{
	if (this->autoClearTareAfterPrintEnabled == 1)
		TARE_ClearTare(this);
	return TARE_SUCCESS;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_IsKeyboardTareEnabled
 * Prototype in : Tare.h
 * Description  : return enable status of keyboard tare
 * Return value : TRUE or FALSE
 *---------------------------------------------------------------------*/
bool TARE_IsKeyboardTareEnabled(TARE *this, int msgType)
{
//	if ((this->keyboardTareEnabled && this->tareEnabled) || msgType == plcKeyboardEnteredTare)
    if (this->keyboardTareEnabled)
		return true;
	else
		return false;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_IsPushbuttonTareEnabled
 * Prototype in : TARE_Process.h
 * Description  : return enable status of pushbutton tare
 * Return value : TRUE or FALSE
 *---------------------------------------------------------------------*/
bool TARE_IsPushbuttonTareEnabled(TARE *this)
{
	if (this->pushbuttonTareEnabled)
		return true;
	else
		return false;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_IsTareEnabled
 * Prototype in : Tare.h
 * Description  : return enable status of tare
 * Return value : TRUE or FALSE
 *---------------------------------------------------------------------*/
int TARE_IsTareEnabled(TARE *this, int msgType)
{
//	if (this->tareEnabled || msgType==plcPushButtonTare ||
//		 msgType==plcKeyboardEnteredTare || msgType==plcClearButton)
//	if (this->tareEnabled)
    return true;
//	else
//		return false;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_SetTareMode
 * Prototype in : Tare.h
 * Description  : set tare mode
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void TARE_SetTareMode(TARE *this, uint8_t mode) 
{
    this->tareMode = mode;
    //XHT_2018
//    accessBRAMParameters(CURRENTTAREMODEADDRESS, (uint8_t *)&(this->tareMode), 1, WRITEOPERATION); 
}

/*---------------------------------------------------------------------*
 * Name         : TARE_GetTareMode
 * Prototype in : Tare.h
 * Description  : get tare mode
 * Return value : 'G' = gross
 *				: 'N' = net
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
uint8_t TARE_GetTareMode(TARE *this)
{
	return this->tareMode;
}

/*---------------------------------------------------------------------*
 * Name         : TARE_GetNetSignCorrection
 * Prototype in : Tare.h
 * Description  : get net sign correction flag
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
bool TARE_GetNetSignCorrection(TARE *this)
{
	if (this->netSignCorrection)
	    return true;
	else
	    return false;
}

/**---------------------------------------------------------------------
 * Name         : TARE_GetClearTareOnZero
 * Description  : get clear tare on zero mode
 * Prototype in : Tare.h
 * \param    	: *this---pointer to TARE struct
 * \return    	: true---clear tare function when scale return to zero point is enabled
 *---------------------------------------------------------------------*/
uint8_t TARE_GetClearTareOnZero(TARE *this)
{
    return this->bClearTareOnZero;
}
