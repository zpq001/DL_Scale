//==================================================================================================
//                                          
//==================================================================================================
//
//! \file	IND245/Scale/Motion.c
//! \brief	motion.c contains the weight processing methods
//!			MOTION scans the series of weight readings to
//! 		determine the points of "motion" and "no motion".
//!			"no motion" is a consecutive string of weight readings
//!			(constructable from 3 to 10) in which the delta from
//!			the lowest to highest weight is no greater than the
//!			motion sensitivity, which is also constructable.
//!			
//!			The Motion sensitivity is defined in divisions.
//!			Whenever there is a unit switch or whenever there is a
//!			there is a range change, the current LEGAL_UNIT object
//!			must send a calibrate message to MOTION so that the
//!			motion sensitivity counts can be re-figgered.
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
//--------------------------------------------------------------------------------------------------
// R A I N B O W
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// A P P L I C A T I O N
//--------------------------------------------------------------------------------------------------
#include "ScaleConfig.h"
#include "Motion.h"
//#include "MTSICS.h"
//#include "DemandPrint.h"
//==================================================================================================
//  M A C R O   D E F I N E
//==================================================================================================

//==================================================================================================
//  G L O B A L   V A R I A B L E S
//==================================================================================================
MOTION g_motiondata;
//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================
static const uint8_t motionRange[]={0,1,3};
//==================================================================================================
//  G L O B A L   F U N C T I O N    D E C L A R A T I O N
//==================================================================================================

//==================================================================================================
//  S T A T I C   F U N C T I O N    D E C L A R A T I O N
//==================================================================================================

//==================================================================================================
//  G L O B A L   F U N C T I O N    I M P L E M E N T A T I O N
//==================================================================================================

//==================================================================================================
//  S T A T I C   F U N C T I O N    I M P L E M E N T A T I O N
//==================================================================================================





/**---------------------------------------------------------------------
 * Name         : MotionInit
 * Description  : initialize motion variables,including buffer,state,counter
 * Prototype in : Motion.h
 * \param    	: *this---pointer to MOTION struct
 * \return    	: none
 *---------------------------------------------------------------------*/
void MOTION_Init(MOTION *this)
{
	int i;

	this->motionChangedFlag = true;
	this->inMotionFlag      = false;
	for (i = 0; i < MOTION_ENTRIES; i++)
		this->readingsBuffer[i] = -1000000;
	this->bufferWritePointer = 0;
}

/*---------------------------------------------------------------------*
 * Name         : MOTION_InstallReInitialization
 * Prototype in : Motion.h
 * Description  : Callback function for re-initialize MOTION data members.
 *                This function will be installed from application.
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
//void MOTION_InstallReInitialization(MOTION *this, void (*pCallbackFunction)(void *))
//{
//	this->reInitializeDataMembers = pCallbackFunction;
//}

/*---------------------------------------------------------------------*
 * Name         : GetMotionSensitivityD
 * Syntax       : float GetMotionSensitivityD(void)
 * Prototype in : Motion.h
 * Description  : return motion sensitivity in Divisions
 * Return value : motion sensitivity in Divisions
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
float MOTION_GetMotionSensitivityD(MOTION *this)
{
    return this->sensitivityInD;
}

/*---------------------------------------------------------------------*
 * Name         : SetMotionSensitivityD
 * Syntax       : void SetMotionSensitivityD(float D)
 * Prototype in : Motion.h
 * Description  : set motion sensitivity in Divisions
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void MOTION_SetMotionSensitivityD(MOTION *this, unsigned short D)
{
    this->sensitivityInD = (SETUP_MOTIONRANGE)D;
}

/*---------------------------------------------------------------------*
 * Name         : MOTION_SetStabilityTimePeriod
 * Syntax       : void SetStabilityTimePeriod(int cycles)
 * Prototype in : Motion.h
 * Description  : set motion stability time period in weight cycles
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void MOTION_SetStabilityTimePeriod(MOTION *this, int cycles)
{
	this->periodInCycles = cycles;
	if (this->periodInCycles > 0)
		this->periodInCycles++;
	if (this->periodInCycles >  MOTION_ENTRIES)
		this->periodInCycles = MOTION_ENTRIES;
}

/*---------------------------------------------------------------------*
 * Name         : MOTION_GetStabilityTimePeriod
 * Prototype in : Motion.h
 * Description  : get motion stability time period in weight cycles
 * Return value :
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
long MOTION_GetStabilityTimePeriod(MOTION *this)
{
    return this->periodInCycles;
}

/*---------------------------------------------------------------------*
 * Name         : MOTION_GetMotion
 * Prototype in : Motion.h
 * Description  : get motion flag
 * Return value : motion flag   TRUE  = motion detected
 *              :               FALSE = no motion
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
bool MOTION_GetMotion(MOTION *this)
{
	return this->inMotionFlag;
}

/*---------------------------------------------------------------------*
 * Name         : MOTION_GetMotionChanged
 * Prototype in : Motion.h
 * Description  : get motion changed flag
 * Return value : indication that motion flag changed polarity in the
 *              : last weight cycle.
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
bool MOTION_GetMotionChanged(MOTION *this)
{
	return this->motionChangedFlag;
}

/*---------------------------------------------------------------------*
 * Name         : MOTION_Calibrate
 * Prototype in : Motion.h
 * Description  : calculate motion sensitivity in counts
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void MOTION_Calibrate(MOTION *this, float countsPerD)
{
	this->sensitivityInCounts = (long)(motionRange[this->sensitivityInD] * countsPerD);
}

/**---------------------------------------------------------------------
 * Name         : MOTION_ProcessMotion
 * Description  : process weight cycle counts to determine if there
 *              : is motion on the scale
 * Prototype in : Motion.h
 * \param    	: *this---pointer to MOTION struct
 * \param    	: counts---current filtered rawcounts
 * \return    	: none
 *---------------------------------------------------------------------*/
void MOTION_ProcessMotion(MOTION *this, long counts)
{
	bool oldMotion;
	long maxReading, minReading;
	int  i;


    
	if (this->periodInCycles)
	{
		if (++this->bufferWritePointer == this->periodInCycles)
			this->bufferWritePointer = 0;
		this->readingsBuffer[this->bufferWritePointer] = counts;
		// get minimum reading and
		// maximum reading in motion buffer
		minReading = maxReading = this->readingsBuffer[0];
		for (i=1; i <  this->periodInCycles; i++) 
		{
			if (this->readingsBuffer[i] > maxReading)
				maxReading = this->readingsBuffer[i];
			else if (this->readingsBuffer[i] < minReading)
				minReading = this->readingsBuffer[i];
		}
		// set motion flag

		oldMotion = this->inMotionFlag;
		if (this->sensitivityInCounts > 0)
		{
			if ((maxReading - minReading) > this->sensitivityInCounts)
				this->inMotionFlag = true;
			else
				this->inMotionFlag = false;
		}
		else
		{
			this->inMotionFlag = false;
		}

	}
    

    	
	// set motion changed flag
	if (oldMotion != this->inMotionFlag)
		this->motionChangedFlag = true;
	else
		this->motionChangedFlag = false;
}

/**---------------------------------------------------------------------
 * Name         : MTSICSPending_S_Process
 * Description  : process MTSICS S command
 * Prototype in : Motion.c
 * \param    	: connection---port number to send
 * \param    	: this---motion struct
 * \return    	: none
 *---------------------------------------------------------------------*/
//static void MTSICSPending_S_Process(MOTION *this,MTSICS_PENDING * pendingPtr)
//{
//    char * ut;
//	char pbuff[50] = {0};
//	char netString[12] = {0};
//	char *wgtString; 
//	
//    if (scale.bZeroCommand || scale.bTareCommand || scale.bClearCommand || scale.bPrintCommand)
//    {
//        printSendoutString(pendingPtr->connection,"S I\r\n");
//        pendingPtr->command[0] = PENDING_NONE;
//    }
//    else if (scale.bOverCapacity)
//    {
//        printSendoutString(pendingPtr->connection,"S +\r\n");
//        pendingPtr->command[0] = PENDING_NONE;
//    }
//    else if ((ZERO_GetUnderZero(&(scale.zero))))
//    {
//        printSendoutString(pendingPtr->connection,"S -\r\n");
//        pendingPtr->command[0] = PENDING_NONE;
//    }
//    else
//    { 
//        if (!this->inMotionFlag)//stable
//    	{
//    		ut = UNIT_GetUnitStringbyUnitType(&(scale.unit),scale.unit.currUnitType);
//            
//            RB_STRING_strncatmax(pbuff,"S S ",sizeof(pbuff));
//            RB_STRING_strncpymax(netString, scale.netString, sizeof(netString));
//            wgtString = netString; 
//            while (*wgtString != 0)
//                wgtString++; 
//            if (*(wgtString - 1) == ' ')
//                *(wgtString - 1) = 0x00; 
//            RB_STRING_AlignRight(netString, 10); 
//            RB_STRING_strncatmax(pbuff,netString,sizeof(pbuff)); 
//            RB_STRING_strncatmax(pbuff," ",sizeof(pbuff)); 
//            RB_STRING_strncatmax(pbuff,ut,sizeof(pbuff)); 
//            if (RB_DEVICE_GetFreeSendBufferSpace(pendingPtr->connection) > MTSICSFREEBUFFERLIMIT)
//            {
//                printSendoutString(pendingPtr->connection,(char *)pbuff);
//                RB_DEVICE_SendEndOfLine(pendingPtr->connection);
//                pendingPtr->command[0] = PENDING_NONE;
//                pendingPtr->waitCount  = 0;
//            }
//    	}
//    	else
//    	{
//    		if (pendingPtr->waitCount > 0)
//    			pendingPtr->waitCount --;
//    		else if (pendingPtr->waitCount == 0)
//    		{
//    			printSendoutString(pendingPtr->connection,"S I\r\n");
//                pendingPtr->command[0] = PENDING_NONE;
//                pendingPtr->waitCount  = 0;
//    		}
//    	}
//    }
//}

// For simple motion detection
//2009-4-27 14:38 change static uint32_t motionCounter && bool  bMotion to int32_t m_motionCounter&& bool m_bMotion
// and move from the (in)program MOTION_Detect to the out
int32_t m_motionCounter = 0;
bool m_bMotion=false;
bool MOTION_Detect(int32_t currentCounts, int32_t *pPreviousCounts, int32_t motionRange, int32_t settlingCounter)
{
	int32_t x;
	x = currentCounts - *pPreviousCounts;
	*pPreviousCounts = currentCounts;
	if (x < 0)
		x = -x;
	if (x > motionRange)
	{
		m_motionCounter = settlingCounter;
		m_bMotion = true;
	}
	else if (m_motionCounter > 0)
		m_motionCounter--;
	else
		m_bMotion = false;
	
	return m_bMotion;
}
