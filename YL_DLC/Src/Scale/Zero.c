//==================================================================================================
//                                          
//==================================================================================================
//
//! \file	IND245/Scale/Zero.c
//! \brief	Handle Scale Zero Process
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

//--------------------------------------------------------------------------------------------------
// A P P L I C A T I O N
//--------------------------------------------------------------------------------------------------
#include "Scale.h"
#include "Zero.h"
//#include "MTSICS.h"
#include "ScaleConfig.h"
//#include "DemandPrint.h"
//#include "BRAMaddress.h"

ZERO g_zerodata;

static const uint8_t AZMDivisionTable[] = {5,10,30,100};
const uint8_t powerupZeroRange[]        = {0,4,20};
const uint8_t pushButtonZeroRange[]     = {0,4,40};
const uint16_t motionTimeOut[]          = {0,30,100,300};

extern long g_tempcounts[];
extern long current_lczerocounts[12];
//extern COMMANDSOURCE tareCommandSource  = COMMAND_NONE;
extern COMMANDSOURCE zeroCommandSource;
//extern COMMANDSOURCE printCommandSource = COMMAND_NONE;
/**---------------------------------------------------------------------
 * Name         : ZeroInit
 * Description  : ZERO maintains the zero of the scale.  There are
 *				  four possible ways of seting zero -- through
 *				  calibrated zero, pushbutton zero, power-up zero,
 *                an autozero.
 *               
 *				  AUTOZERO provides a zeroing  method through
 *				  automatic zero maintenance.  It moves the current zero
 *				  towards a new zero at a fixed rate.
 *               
 *				  AZM rate is fixed at .04 D/sec.
 *				 
 *				  The absolute range limit where autozeroing can be
 *				  applied is the PowerUpZero plus the PushButton Zero
 *				  Range.
 *				 
 * 				  The autozero window is range around the current zero
 *				  where autozero maintenance can be applied.
 *               
 * 				  Set number of divisions to surround center of zero
 *                for particular markets, as follows:
 *				  U.S. Handbook 44 is +/- .25 weight divisions.
 *	 			  Canada 		   is +/- .20 weight divisions.
 *				  Europe OIML/76   is +/- .25 weight divisions.
 * Prototype in : ZERO.h
 * \param    	: *this---pointer to ZERO struct
 * \return    	: none
 *---------------------------------------------------------------------*/
void ZERO_Init(ZERO *this)
{
	this->powerUpZeroDelayCycles = 0;
	this->zeroStatus = ZERO_STATUS_NONE;
	this->previousCounts = 0;
}

/*---------------------------------------------------------------------*
 * Name         : ZERO_InstallReInitialization
 * Prototype in : Zero.h
 * Description  : Callback function for re-initialize ZERO data members.
 *                This function will be installed from application.
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void ZERO_InstallReInitialization(ZERO *this, void (*pCallbackFunction)(void *))
{
	this->reInitializeDataMembers = pCallbackFunction;
}

void ZERO_InstallSavingCurrentZero(ZERO *this, void (*pCallbackFunction)(int))
{
    this->saveCurrentZero = pCallbackFunction;
}

/*---------------------------------------------------------------------*
 * Name         : ZERO_ReInitialization
 * Prototype in : Zero.h
 * Description  : Callback function for re-initialize ZERO data members.
 *                This function will be installed from application.
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void ZERO_ReInitialization(ZERO *this)
{
	this->reInitializeDataMembers(this);
}

/**---------------------------------------------------------------------
 * Name         : ZERO_Calibrate
 * Description  : calibrate zero weight parameters
 * Prototype in : ZERO.h
 * \param    	: *this---pointer to ZERO struct
 * \param	    : *pIncrArray---the increment table for all ranges
 * \param	    : capacityCounts---the capacity rawcounts
 * \param	    : *span---pointer to span factor table
 * \return    	: none
 *---------------------------------------------------------------------*/
void ZERO_Calibrate(ZERO *this, double *span, double *pIncrArray, int32_t capacityCounts)
{
	int32_t i;
	double centerOfZeroDivisions;
	double Rate_DivisionsPerSecond = 0.07; // rate at which the auto zeroing will move the current zero
									      // towards the current reading.
										
	centerOfZeroDivisions = (this->pScale->market == MARKET_CANADA && this->pScale->bLegalForTrade == 1) ? 0.20 : 0.25;
	
	if ((this->pScale->market == MARKET_EUROPEAN_OIML) || (this->pScale->market == MARKET_AUSTRALIA))
	{
	    i = (capacityCounts + 5) / 10;          // (int32_t)(capacityCounts * 0.1 + 0.5)
		this->powerUpZeroPositiveCounts = this->calibratedZeroCounts + i;
		this->powerUpZeroNegativeCounts = this->calibratedZeroCounts - i;
        i = (capacityCounts * 2 + 50) / 100;    // (int32_t)(capacityCounts * 0.02 + 0.5)
		this->pushbuttonZeroPositiveDelta = i;
		this->pushbuttonZeroNegativeDelta = i;
	}
	else
	{
		this->powerUpZeroPositiveCounts = this->calibratedZeroCounts + (capacityCounts * powerupZeroRange[this->powerupZero]/2 + 50) / 100;
		this->powerUpZeroNegativeCounts = this->calibratedZeroCounts - (capacityCounts * powerupZeroRange[this->powerupZero]/2 + 50) / 100;

		this->pushbuttonZeroPositiveDelta = (capacityCounts * pushButtonZeroRange[this->pushbuttonZero]/2 + 50) / 100;
		this->pushbuttonZeroNegativeDelta = (capacityCounts * pushButtonZeroRange[this->pushbuttonZero]/2 + 50) / 100;
	}

	// set the zeroing range
	ZERO_SetPowerUpZero(this, this->powerUpZeroCounts);
	// In a Multiple Range system, the return to zero is half a
	// divsion of the higher weighing ranges.
	for ( i = 0; i < 3; i++ )
	{
		if (i == 0)
		{
			this->centerOfZeroCounts[0] = (int32_t)(centerOfZeroDivisions * span[0]);
			this->cozLimitWeight[0] = centerOfZeroDivisions * pIncrArray[0];
		}
		else 
		{
			this->centerOfZeroCounts[i]   = (int32_t)(0.5 * span[i]);
			this->cozLimitWeight[i] = 0.5 * pIncrArray[i];
		}
		// AZM window (in d) x counts per d
		this->autoZeroWindowCounts[i] = (int32_t)(AZMDivisionTable[this->autoZeroWindowDivisions] * span[i] /10);
		this->autoZeroMotionCounts[i] = (this->autoZeroWindowCounts[0] + 5) / 10;
		this->adjustmentCountsPerCycle[i] = (int32_t)((Rate_DivisionsPerSecond / CONFIG_WEIGHT_CYCLES_PER_SEC) * span[i]);
		if (this->adjustmentCountsPerCycle[i] < 1)
		    this->adjustmentCountsPerCycle[i] = 1;
		this->autoZeroWindowCounts[i] -= this->adjustmentCountsPerCycle[i];
	}	
	


	if (this->underZeroBlanking != UNDERZEROBLANKING_DISABLE)
		this->underZeroCounts = -(int32_t)(20 * span[0]);

	this->autoZeroMotionCycles = 3;
}

/**---------------------------------------------------------------------
 * Name         : ZERO_SetPushbuttonZero
 * Description  : set push button zero
 * Prototype in : ZERO.h
 * \param    	: *this---pointer to ZERO struct
 * \param	    : counts---rawcounts retrieved from MELSI module
 * \param	    : bMotion---BOOL variable to denote the scale motion state
 * \return    	: none
 *---------------------------------------------------------------------*/
void ZERO_SetPushbuttonZero(ZERO *this, int32_t counts, uint8_t netMode,bool bMotion)
{
//    MTSICS_PENDING * pendingPtr = NULL; 
//    
//    if (MTSICSCommandPending[0].command[0] == PENDING_Z)
//        pendingPtr = &MTSICSCommandPending[0];
//    else if (MTSICSCommandPending[1].command[0] == PENDING_Z)
//        pendingPtr = &MTSICSCommandPending[1];
//    else if (MTSICSCommandPending[2].command[0] == PENDING_Z)
//        pendingPtr = &MTSICSCommandPending[2];
//    else if (MTSICSCommandPending[3].command[0] == PENDING_Z)
//        pendingPtr = &MTSICSCommandPending[3];
        
	this->zeroScaleStatus = ZERO_IN_PROGRESS;
	if (netMode == 'N')
	{
		if (!this->bClearTareOnZero)
		{

		    this->zeroScaleStatus = ILLEGAL_SCALE_MODE_DURING_ZERO;
			return;
		}
	}
	if (bMotion)
	{
		if (this->zeroMotionWait == 0)   //no wait
	  	{
		  	this->zeroScaleStatus = SCALE_IN_MOTION_DURING_ZERO; 

		}
	  	else if (this->zeroMotionWait < 99)
		{
			if (this->bZeroWait)
			{
	  			if (this->zeroWaitCount > 0)
	  			    this->zeroWaitCount--;
	  			else
	    		{
		    		this->bZeroWait = 0;
			    	this->zeroScaleStatus = SCALE_IN_MOTION_DURING_ZERO; 
      
				}
	    	}
			else
	    	{
		 	   this->bZeroWait = 1;
		 	   this->zeroWaitCount = CONFIG_WEIGHT_CYCLES_PER_SEC * motionTimeOut[this->zeroMotionWait]/10;
	    	}
		}
	}
	else
	{
		this->bZeroWait = 0;
		this->zeroWaitCount = 0;
		if ((bool)this->pushbuttonZero || (zeroCommandSource == COMMAND_REMOTE))
		{
			// see if within zero range		
			if (counts > (this->powerUpZeroCounts + this->pushbuttonZeroPositiveDelta))
			{
				this->zeroScaleStatus = SCALE_OUT_OF_POSITIVEZEROING_RANGE;	
   
			}
			else if (counts < (this->powerUpZeroCounts - this->pushbuttonZeroNegativeDelta))
			{
				this->zeroScaleStatus = SCALE_OUT_OF_NEGATIVEZEROING_RANGE;	 
  
			}
			else
			{
				ZERO_SetCurrentZero(this, counts);
				this->zeroStatus |= CENTER_OF_ZERO;
    	
				if (!(this->zeroStatus & POWERUP_ZERO_CAPTURED))	
				{
					this->powerupZeroStatus = POWERUP_ZERO_SUCCESS;
					this->zeroStatus |= POWERUP_ZERO_CAPTURED;
					if ((counts < this->powerUpZeroPositiveCounts) && (counts > this->powerUpZeroNegativeCounts))
					{
							// this->zeroStatus |= POWERUP_ZERO_CAPTURED;    
						ZERO_SetPowerUpZero(this, counts);
					}
				}
				this->zeroScaleStatus = ZERO_SUCCESS;
    		}
		}
		else
		{
			this->zeroScaleStatus = INVALID_ZERO_FUNCTION;
	
		}
	}
}

/**---------------------------------------------------------------------
 * Name         : ZERO_ProcessPowerupZero
 * Description  : process zero at each weight cycle
 * Prototype in : ZERO.h
 * \param    	: *this---pointer to ZERO struct
 * \param	    : counts---rawcounts retrieved from MELSI module
 * \param	    : bMotion---BOOL variable to denote the scale motion state
 * \return    	: none
 *---------------------------------------------------------------------*/
void ZERO_ProcessPowerupZero(ZERO *this, int32_t counts,bool bMotion)
{    
	if ((this->zeroStatus & POWERUP_ZERO_CAPTURED) == 0)
	{
    	this->powerupZeroStatus = POWERUP_ZERO_SUCCESS;
		if (this->powerUpZeroDelayCycles < CONFIG_WEIGHT_CYCLES_PER_SEC*4)
		{
			this->powerUpZeroDelayCycles++;
			if (bMotion)
				this->powerupZeroStatus = POWERUP_ZERO_MOTION;
			else
				this->powerupZeroStatus = POWERUP_ZERO_IN_PROGRESS;
		}
		else
		{
			if (this->powerUpZeroSet == 0)    //restart whith current zero, and powerup zero = calibrated zero counts
				this->zeroStatus |= POWERUP_ZERO_CAPTURED;
			else
			{
				if ((bool)this->powerupZero == false)
				{
					ZERO_SetCurrentZero(this, this->calibratedZeroCounts);
					this->zeroStatus |= POWERUP_ZERO_CAPTURED;
				}
				else if (!bMotion)
				{
					if (counts >= this->powerUpZeroPositiveCounts)
						this->powerupZeroStatus = POWERUP_ZERO_OVER;
					else if (counts <= this->powerUpZeroNegativeCounts)
						this->powerupZeroStatus = POWERUP_ZERO_UNDER;
					else // in powerup zero range
					{
						this->zeroStatus |= POWERUP_ZERO_CAPTURED;
						ZERO_SetCurrentZero(this, counts);
						ZERO_SetPowerUpZero(this, counts);
					}
				}
				else // in motion
				{
					// powerupZeroStatus = POWERUP_ZERO_MOTION;
					if (counts >= this->powerUpZeroPositiveCounts)
						this->powerupZeroStatus = POWERUP_ZERO_OVER;
					else
						this->powerupZeroStatus = POWERUP_ZERO_UNDER;
				}
			}
		}
	}
}

static int32_t GetAbsCounts(int32_t counts)
{
    if (counts < 0)
        return (-counts);
    else
        return counts;
}

/**---------------------------------------------------------------------
 * Name         : ZERO_ProcessZero
 * Description  : process zero at each weight cycle
 * Prototype in : ZERO.h
 * \param    	: *this---pointer to ZERO struct
 * \param	    : counts---rawcounts retrieved from MELSI module
 * \param	    : bMotion---BOOL variable to denote the scale motion state
 * \param	    : netMode---denote scale on gross mode or netmode
 * \param	    : range---denote which range the scale is in
 * \return    	: the difference counts between the current rawcounts and current.
 *                zero counts after AZM operation
 *---------------------------------------------------------------------*/
int32_t ZERO_ProcessZero(ZERO *this, int32_t counts, uint8_t netMode, uint8_t range,bool bMotion)
{
	int32_t  newZero;
	int32_t  absCounts;   //absolute value of counts relative to zero
	int32_t  relCounts;
    
	relCounts = counts - this->currentZeroCounts;
	absCounts = GetAbsCounts(relCounts);
	
	/*	AZM MOTION DETECTION*/
	if ((bool)this->enableAZM && ((this->enableAZM == AZM_GROSS_NET) || netMode == 'G'))
	{
		if (absCounts <= this->autoZeroWindowCounts[range])
		{
			if ((counts - this->previousCounts) < this->autoZeroMotionCounts[range]) 
			{
				if (--this->autoZeroMotionCycles == 0)
				{
					this->autoZeroMotionCycles++;
					/*AZM HANDLING*/
					if (!bMotion)
					{
						if (relCounts < 0)  // Weight < 0
						{
							if (relCounts + this->adjustmentCountsPerCycle[range] > 0) 
							{   //adjustment causes wt. to go positive
								ZERO_SetCurrentZero(this, counts);
								relCounts = counts - this->currentZeroCounts;
                                absCounts = GetAbsCounts(relCounts);
							}
							else
							{
							    newZero = this->currentZeroCounts - this->adjustmentCountsPerCycle[range];
								if (newZero > this->zeroLimitNegativeCounts)
								{
									ZERO_SetCurrentZero(this, newZero);
								    relCounts = counts - this->currentZeroCounts;
                                    absCounts = GetAbsCounts(relCounts);
								}
							}
						}
						else // Weight >= 0
						{
							if (relCounts - this->adjustmentCountsPerCycle[range] < 0) 
							{   //adjustment causes wt. to go negative
								ZERO_SetCurrentZero(this, counts);
								relCounts = counts - this->currentZeroCounts;
                                absCounts = GetAbsCounts(relCounts);
							}
							else
							{
							    newZero = this->currentZeroCounts + this->adjustmentCountsPerCycle[range];
								if (newZero < this->zeroLimitPositiveCounts)	
								{
									ZERO_SetCurrentZero(this, newZero);
								    relCounts = counts - this->currentZeroCounts;
                                    absCounts = GetAbsCounts(relCounts);
								}
							}
						}
					}  /* if motion->GetMotion*/
				}  /* if autoZeroMotionCycles == 0*/
			}  /* if counts - previousCounts < autoZeroMotionCounts*/
			else
				this->autoZeroMotionCycles = 3;
		}  // if absCounts <= autoZeroWindowCounts
		else
			this->autoZeroMotionCycles = 3;
	}  // if enable AZM
		
	/* CENTER OF ZERO HANDLING */
	if (absCounts > this->centerOfZeroCounts[range])
		this->zeroStatus &= ~CENTER_OF_ZERO;
	else if (!bMotion)
		this->zeroStatus |= CENTER_OF_ZERO;
			
	// UNDER ZERO HANDLING
	if (this->underZeroBlanking != UNDERZEROBLANKING_DISABLE)
	{
    	if (relCounts < this->underZeroCounts)
    	{
    		this->zeroStatus |= UNDER_ZERO;
    		if (this->underZeroBlanking == UNDERZEROBLANKING_20DWITHZERO)
    		    this->underzeroWait = true; 
    	}
    	else if (this->underZeroBlanking == UNDERZEROBLANKING_20DWITHZERO)
    	{
    	    if (!this->underzeroWait)
    	        this->zeroStatus &= ~UNDER_ZERO; 
    	}
        else
    		this->zeroStatus &= ~UNDER_ZERO;
	}

	this->previousCounts = counts;
	return relCounts;
}

/*---------------------------------------------------------------------*
 * Name         : ZERO_GetCurrentZero
 * Prototype in : Zero.h
 * Description  : return current zero counts
 * Return value : current zero counts
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
int32_t ZERO_GetCurrentZero(ZERO *this)
{
	return this->currentZeroCounts;
}

/*---------------------------------------------------------------------*
 * Name         : ZERO_SetCurrentZero
 * Prototype in : Zero.h
 * Description  : Set current zero counts
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void ZERO_SetCurrentZero(ZERO *this, int32_t counts)
{ 
	this->currentZeroCounts = counts;
	if (this->saveCurrentZero)
	    this->saveCurrentZero(counts);	
//XHT_2018
//	memcpy(current_lczerocounts,g_tempcounts,4*scale.scaleCellsNum); // as current zero counts
//	memcpy((char *)LOADCELL_CURZEORCOUNTS,current_lczerocounts, sizeof(current_lczerocounts));

}

/*---------------------------------------------------------------------*
 * Name         : ZERO_SetReCalibratedZeroCounts
 * Prototype in : Zero.h
 * Description  : set calibrated zero counts after a new calibration
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void ZERO_SetReCalibratedZeroCounts(ZERO *this, int32_t counts)
{
	this->zeroStatus |= POWERUP_ZERO_CAPTURED;
	this->calibratedZeroCounts = counts;
	ZERO_SetCurrentZero(this, counts);
	ZERO_SetPowerUpZero(this, counts);
	
}

/*---------------------------------------------------------------------*
 * Name         : ZERO_SetCalibratedZeroCounts
 * Prototype in : Zero.h
 * Description  : set calibrated zero counts
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void ZERO_SetCalibratedZeroCounts(ZERO *this, int32_t counts)
{
   this->calibratedZeroCounts = counts;
   ZERO_SetPowerUpZero(this, counts);
}

/*---------------------------------------------------------------------*
 * Name         : ZERO_GetCalibratedZeroCounts
 * Prototype in : Zero.h
 * Description  : get calibrated zero counts
 * Return value : calibrated zero counts
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
int32_t ZERO_GetCalibratedZeroCounts(ZERO *this)
{
	return this->calibratedZeroCounts;
}

/*---------------------------------------------------------------------*
 * Name         : ZERO_SetPowerUpZero
 * Prototype in : Zero.h
 * Description  : set power up zero
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void ZERO_SetPowerUpZero(ZERO *this, int32_t counts)
{
// John Moorman method
	this->powerUpZeroCounts       = counts;
	this->zeroLimitPositiveCounts = counts + this->pushbuttonZeroPositiveDelta;
	this->zeroLimitNegativeCounts = counts - this->pushbuttonZeroNegativeDelta;
}

/*---------------------------------------------------------------------*
 * Name         : ZERO_GetCenterOfZero
 * Prototype in : Zero.h
 * Description  : get center of zero indication
 * Return value : TRUE  = scale within center of zero
 *              : FALSE = not at center of zero
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
bool ZERO_GetCenterOfZero(ZERO *this)
{
	if (this->zeroStatus & CENTER_OF_ZERO)
		 return true;
	else
		 return false;
}

/*---------------------------------------------------------------------*
 * Name         : ZERO_GetUnderZero
 * Syntax       : bool GetUnderZero(void)
 * Prototype in : Zero.h
 * Description  : under zero flag is set when weight > five divisions
 *              : below zero.
 * Return value : TRUE  =  scale 5 or more divisions below zero
 *              : FALSE =  NOT!
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
bool ZERO_GetUnderZero(ZERO *this)
{
	if (this->zeroStatus & UNDER_ZERO)
		return true;
	else
		return false;
}

/*---------------------------------------------------------------------*
 * Name         : ZERO_GetPowerUpZeroCaptured
 * Prototype in : Zero.h
 * Description  : return indication that power up zero has been captured
 * Return value : TRUE = power up zero captured
 *              : FALSE = power up zero not captured
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
bool ZERO_GetPowerUpZeroCaptured(ZERO *this)
{
	if (this->zeroStatus & POWERUP_ZERO_CAPTURED)
		 return true;
	else
		 return false;
}

/*---------------------------------------------------------------------*
 * Name         : ZERO_GetPowerUpZeroStatus
 * Prototype in : Zero.h
 * Description  : return indication that power up zero has been captured
 * Return value : power up zero operation status
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
POWERUP_ZERO_tStatus ZERO_GetPowerUpZeroStatus(ZERO *this)
{
    return this->powerupZeroStatus;
}

/*---------------------------------------------------------------------*
 * Name         : ZERO_GetZeroScaleStatus
 * Prototype in : Zero.h
 * Description  : return the status of execution for zero scale command
 * Return value : power up zero operation status
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
ZERO_SCALE_tStatus ZERO_GetZeroScaleStatus(ZERO *this)
{
    return this->zeroScaleStatus;
}
