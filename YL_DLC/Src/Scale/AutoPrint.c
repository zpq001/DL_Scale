/*-----------------------------------------------------------------------*
 * Filename....: AutoPrint.c
 * Scope.......: Process print thresholds
 *	Version Author	Date		Description
 *
 *	COPYRIGHT (C) 2004.	All Rights Reserved.	MTCN
 *------------------------------------------------------------------------*/
#include <stdlib.h>
#include <math.h>

#include "ScaleConfig.h"
#include "Scale.h"
#include "AutoPrint.h"
#include "ApplMain.h"
#include "overunder.h"
#include "RB_includeAll.h"
#include "UserParam.h"


/*---------------------------------------------------------------------*
 * Name         : PRINT_THRESHOLD_INIT
 * Syntax       : PRINT_THRESHOLD_INIT
 * Prototype in : autoprint_process.h
 * Description  : constructor
 * Return value : none
 *---------------------------------------------------------------------*/
void AUTOPRINT_Init(AUTO_PRINT *this)
{
	this->thresholdState = WAITING_FOR_PRINT_RESET; 
	if (this->primaryThresholdWeight > .001)
		this->thresholdEnabled = 1; 
	else
		this->thresholdEnabled = 0; 

	this->demandPrintRequest = 0; 
	this->oldPrintUnitType = PRIME_UNIT; 
	this->oldPrintWeight = 0; 
	// this->reInitializeDataMembers(this);
}

/*---------------------------------------------------------------------*
 * Name         : PRINT_InstallReInitialization
 * Prototype in : Print.h
 * Description  : Callback function for re-initialize PRINT data members.
 *                This function will be installed from application.
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void AUTOPRINT_InstallReInitialization(AUTO_PRINT *this, void (*pCallbackFunction)(void *))
{
    this->reInitializeDataMembers = pCallbackFunction;
}

/*---------------------------------------------------------------------*
 * Name         : PRINT_ReInitialization
 * Prototype in : Print.h
 * Description  : Callback function for re-initialize PRINT data members.
 *                This function will be installed from application.
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void AUTOPRINT_ReInitialization(AUTO_PRINT *this)
{
    this->reInitializeDataMembers(this);
}

/*---------------------------------------------------------------------*
 * Name         : AUTOPRINT_Calibrate
 * Syntax       : AUTOPRINT_Calibrate (char current )
 * Prototype in : autoprint_process.h
 * Description  : calculate the weight conversion factors
 * Return value : none
 *---------------------------------------------------------------------*/
void AUTOPRINT_Calibrate(AUTO_PRINT *this)
{
    SCALE *pScale = this->pScale;
    UNIT *pUnit = &(pScale->unit);
    
	this->currentThresholdWeight = UNIT_ConvertUnitType(pUnit, pUnit->calUnitType, pUnit->currUnitType, this->primaryThresholdWeight);
	this->currentResetThresholdWeight = UNIT_ConvertUnitType(pUnit, pUnit->calUnitType, pUnit->currUnitType, this->primaryResetThresholdWeight);
	this->currentDeviationThresholdWeight = UNIT_ConvertUnitType(pUnit, pUnit->calUnitType,pUnit->currUnitType, this->primaryDeviationThresholdWeight);
	this->currentMinPrintWeight = UNIT_ConvertUnitType(pUnit, pUnit->calUnitType, pUnit->currUnitType, this->primaryMinPrintWeight);
	
	// this->autoPrintResetMode = (this->currentDeviationThresholdWeight>0) ? AUTOPRINT_RESET_ONDEVIATION : AUTOPRINT_RESETON_RETURN;
    if (this->autoPrintResetMode == AUTOPRINT_RESETON_DEVIATION)
    {
		if (this->oldPrintUnitType == PRIME_UNIT)
			this->oldPrintWeight = UNIT_ConvertUnitType(pUnit, PRIME_UNIT, pUnit->currUnitType, this->oldPrintWeight);
		else
			this->oldPrintWeight = UNIT_ConvertUnitType(pUnit, this->oldPrintUnitType, pUnit->currUnitType, this->oldPrintWeight);        
    }

	if (this->primaryThresholdWeight > .001)
		this->thresholdEnabled = 1;
	else
		this->thresholdEnabled = 0;
}

/*---------------------------------------------------------------------*
 * Name         : PRINT_AutoPrint(void)
 * Syntax       : unsigned char PRINT_THRESHOLD_AutoPrint()
 * Prototype in : autoprint_process.h
 * Description  : print processing at each weight cycle
 * Return value : 1 = OK for demand print
 * 				: 1 = OK for auto print
 *              : 0 = NOT
 *---------------------------------------------------------------------*/
int AUTOPRINT_AutoPrint(AUTO_PRINT *this, float weight, UNIT_tType currUnitType,bool bMotion)
{
	// unsigned char status=1;
    bool bPowerUpZeroCaptured, bUnderZero;
    bool bExpandDisplay, bLegalForTrade, bOverCapacity;
    SCALE *pScale;
    
    pScale = this->pScale;
    bPowerUpZeroCaptured = ZERO_GetPowerUpZeroCaptured(&(pScale->zero));
	if (!bPowerUpZeroCaptured)
		return PRINTING_SCALE_BAD_ZERO;

   // USER_PARAM_Get(BLK1_targetLatch, &targetLatch);by zyp for debug
    
//	if (powerupZeroStatus != POWERUP_ZERO_SUCCESS)
//		return 0;

//	if (this->demandPrintRequest) 
//	{
//		if (PRINT_DemandPrint() == PRINT_SUCCESS) 
//		{
//			this->demandPrintRequest = 0;
//			return OKtoPrintResponse;				//printtest
//		}
//	}
	
	bOverCapacity = SCALE_GetOverCapacity(pScale);
	bUnderZero = ZERO_GetUnderZero(&(pScale->zero));
	bExpandDisplay = SCALE_GetDisplayMode(pScale);
	bLegalForTrade = SCALE_GetLFT(pScale);
	if (pScale->autoPrint.printInterlockEnabled 
	        || (pScale->autoPrint.autoPrintEnabled && ((selectedApplet == APPLCONFIG_APPL_WEIGHING) || 
	                (selectedApplet == APPLCONFIG_APPL_COUNTINGWEIGHING) || (selectedApplet == APPLCONFIG_APPL_OVERUNDER))))
	{
		if (this->autoPrintResetMode == AUTOPRINT_RESETON_RETURN)
		{
			switch (this->thresholdState) 
			{
				case WAITING_FOR_PRINT_RESET:
					if ((!(this->checkMotion) || !bMotion) && (weight < this->currentResetThresholdWeight))
					{
						 this->thresholdState = PRINT_RESET;
				    }
					break;

				case PRINT_RESET:
					if (this->autoPrintEnabled && this->thresholdEnabled && !bMotion && !bOverCapacity
					    	&& !bUnderZero && (!bExpandDisplay || !bLegalForTrade) 
					    		&& (weight > this->currentThresholdWeight) && (weight > this->currentMinPrintWeight))
					{
						if (++this->waitCount > 3)
    					{
							this->waitCount=0;
							// sd_set(&status,DI_wc0103);
							
                            remotePrintCommandProcess(); 
							return OKtoPrint;		// printtest READY_TO_AUTO_PRINT
						}    
//					    if (targetLatch)
//					    {
//					        if (sp_sd.latched)
//					        {
//        //						thresholdState = WAITING_FOR_PRINT_RESET;       // may be change state after print operation complete
//        						if (++this->waitCount > 3)
//        						{
//        							this->waitCount=0;
//        							// sd_set(&status,DI_wc0103);
//        							pScale->bPrintCommand = 1;
//        							return OKtoPrint;		// printtest READY_TO_AUTO_PRINT
//        						}
//        					}
//        				}
//    					else
//    					{
//    					    float nowwt;
//    					    
//    					    nowwt = SETPT_GetAbsSourceValue(pScale, sp_sd.sourcetype);
//    					    if (sp_status.feeding == 0 && sp_status.fastfeed == 0 && nowwt > m_finefeed_target)
//    					    {
//        //						thresholdState = WAITING_FOR_PRINT_RESET;       // may be change state after print operation complete
//        						if (++this->waitCount > 3)
//        						{
//        							this->waitCount=0;
//        							// sd_set(&status,DI_wc0103);
//        							pScale->bPrintCommand = 1;
//        							return OKtoPrint;		// printtest READY_TO_AUTO_PRINT
//        						}    					        
//    					    }
//    					}
					}
					else
						this->waitCount=0;
					break;
			}
		}
		else if (this->autoPrintResetMode == AUTOPRINT_RESETON_DEVIATION)
		{
			switch (this->thresholdState) 
			{
//				case PRINT_THRESHOLD_NOT_ENABLED:
//					break;

				case WAITING_FOR_PRINT_RESET:
					if ((!this->checkMotion || !bMotion)
						&& (fabs(weight - this->oldPrintWeight) > this->currentDeviationThresholdWeight))
							this->thresholdState = PRINT_RESET;
					break;

				case PRINT_RESET:
					if (this->autoPrintEnabled && !bMotion && !bOverCapacity && !bUnderZero
					    	&& (!bExpandDisplay || bLegalForTrade) && weight > this->currentMinPrintWeight)
					{
//						thresholdState = WAITING_FOR_PRINT_RESET;
						if (++this->waitCount>3)
						{
							this->waitCount=0;
							this->oldPrintWeight = weight;
							this->oldPrintUnitType = currUnitType;
							// sd_set(&status,DI_wc0103);
							// to do, set print scale command
							
							
                            remotePrintCommandProcess(); 
                            printCommandSource = COMMAND_LOCAL; 
							return OKtoPrint;		// printtest READY_TO_AUTO_PRINT
						}
					}
					else
						this->waitCount=0;
					break;
			}
		}
	}
	return 0;
}


/*---------------------------------------------------------------------*
 * Name         : PRINT_THRESHOLD_DemandPrint
 * Syntax       : unsigned char PRINT_THRESHOLD_DemandPrint(void)
 * Prototype in : autoprint_process.h
 * Description  : process demand print request
 * Return value : status        SUCCESS
 *              :               NOT_READY_TO_PRINT
 *              :               SCALE_IN_MOTION
 *---------------------------------------------------------------------*/
/*unsigned char PRINT_THRESHOLD_DemandPrint(void) 
{

	if ( MOTION_GetMotion() )
		return PRINTING_SCALE_IN_MOTION;

	if ( currentUnits_OverCapacity() )
		return PRINTING_SCALE_OVERCAPACITY;

	if ( ZERO_GetUnderZero() )
		return PRINTING_SCALE_UNDER_ZERO;
		
	if (GetdisplayMode()&&GetLegalFlag()==1)//approved and expand
		return PRINTING_SCALE_IN_EXPAND;
	

	if ( printInterlockEnabled ) 
	{
		switch (thresholdState) 
		{
			case PRINT_RESET:
				if ( currentUnits_GetGrossWeightDouble() > currentThresholdWeight ) 
				{
					thresholdState = WAITING_FOR_PRINT_RESET;
					return SUCCESS;    // READY_TO_PRINT
				}
				else
					return PRINTING_NOT_READY_TO_PRINT;
					
			case PRINT_THRESHOLD_NOT_ENABLED:
				return SUCCESS;
				
			default:
				return PRINTING_NOT_READY_TO_PRINT;
		}
	}

	else 
	{          // check minimum print threshold, if enabled.
		if (thresholdState == PRINT_THRESHOLD_NOT_ENABLED )  
		{
			return SUCCESS;
		}
		else 
		{
			if ( currentUnits_GetGrossWeightDouble()> currentThresholdWeight )  
			{
				return SUCCESS;
			}
			else
				return PRINTING_NOT_READY_TO_PRINT;
		}

	}
}*/


/*---------------------------------------------------------------------*
 * Name         : PRINT_THRESHOLD_StoreDemandPrintRequest
 * Prototype in : autoprint_process.h
 * Description  : Store request until the print conditions are
 * 				: ready.  Return autoprint request when print
 *				: conditions are ready.
 * Return value : status        SUCCESS
 *              :               PRINT_REQUEST_ARMED
 *              :               FAILURE_STATUS
 *---------------------------------------------------------------------*/
/*unsigned char PRINT_THRESHOLD_StoreDemandPrintRequest(void) 
{

	unsigned char status;

	status = PRINT_THRESHOLD_DemandPrint();
	if ( status == PRINTING_SCALE_IN_MOTION||
		 status == PRINTING_NOT_READY_TO_PRINT )    	
	 {
		if ( thresholdState != WAITING_FOR_PRINT_RESET ) 
		{  
			demandPrintRequest =1;
			status = PRINTING_REQUEST_ARMED;
		}
	}
	return status;
}*/

