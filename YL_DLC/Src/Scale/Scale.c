//==================================================================================================
//                                          
//==================================================================================================
//
//! \file	IND245/Scale/scale.c
//! \brief	Handle Scale Weight Process
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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "comm.h"

//--------------------------------------------------------------------------------------------------
// A P P L I C A T I O N
//--------------------------------------------------------------------------------------------------
#include "ScaleConfig.h"
#include "Scale.h"

//#include "Tare.h"
#include "UserParam.h"

#include "stm32f1xx_hal.h"
#include "RB_Format.h"
#include "RB_String.h"
//==================================================================================================
//  M A C R O   D E F I N E
//==================================================================================================
#define ERROR 0.0000001
//==================================================================================================
//  G L O B A L   V A R I A B L E S
//==================================================================================================



COMMANDSOURCE tareCommandSource  = COMMAND_NONE;
COMMANDSOURCE zeroCommandSource  = COMMAND_NONE;
COMMANDSOURCE printCommandSource = COMMAND_NONE;

extern bool totalzerodriftfalg ;
//extern SCALE scale; 
extern SCALE g_ScaleData;
const double incrTable[] =
{
	0.0001,         // 0
	0.0002,         // 1
	0.0005,         // 2
	0.001,			// 3
	0.002,			// 4
	0.005,			// 5
	0.01,			// 6
	0.02,			// 7
	0.05,			// 8
	0.1,			// 9
	0.2,			// 10
	0.5,			// 11
	1,				// 12
	2,				// 13
	5,				// 14
	10,				// 15
	20,				// 16
	50,				// 17
	100,			// 18
	200,			// 19
};

const double geoTable[] =
{
	977.039, // 0 
	977.237, // 1
	977.436, // 2
	977.635, // 3
	977.834, // 4
	978.033, // 5
	978.233, // 6
	978.432, // 7
	978.631, // 8
	978.831, // 9
	979.030, // 10
	979.230, // 11
	979.429, // 12
	979.629, // 13
	979.829, // 14
	980.029, // 15
	980.229, // 16
	980.429, // 17
	980.629, // 18
	980.830, // 19
	981.030, // 20
	981.230, // 21
	981.431, // 22
	981.631, // 23
	981.832, // 24
	982.033, // 25
	982.234, // 26
	982.435, // 27
	982.636, // 28
	982.837, // 29
	983.038, // 30
	983.239  // 31
};
//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

static const uint8_t noMotionInterval[] = {3, 5, 7, 10}; 

//==================================================================================================
//  G L O B A L   F U N C T I O N    D E C L A R A T I O N
//==================================================================================================
bool SCALE_AffirmWeightString(SCALE *this); 
float SCALE_GetProgrammableTareIncrement(SCALE *this); 
extern void CaculateLCoverload();
extern void Caculatezerodrift();
//extern void recordzerodrift();
extern void caculateprecentageload();
//==================================================================================================
//  S T A T I C   F U N C T I O N    D E C L A R A T I O N
//==================================================================================================
static uint8_t SCALE_CalcWeightStringLength(char *stringPtr);
//==================================================================================================
//  G L O B A L   F U N C T I O N    I M P L E M E N T A T I O N
//==================================================================================================
/********************************
//Inint Scale parameters
//
*********************************/

/*---------------------------------------------------------------------*
* Name         : getCalParams
* Prototype in : 
* Description  : Retrieve calibration parameters from EEPROM
* Paremeter    : Pointer to scale struct
* Return value : none
* Unit Testing Complete Date:   Tested By:  Test Unit Used:
*---------------------------------------------------------------------*/
static void getCalParams(SCALE *pScale)
{    
    double tempd = 0.0;
    int tempi = 0;
    uint8_t tempchar = 0;    
    CharOfUnionDouble chardouble;
    
    USER_PARAM_Get(BLK0_zeroCalCounts, (uint8_t *)(&tempi)); 
    pScale->zeroCalCounts = tempi;
    USER_PARAM_Get(BLK0_highCalCounts, (uint8_t *)(&tempi)); 
    pScale->highCalCounts = tempi;    
    USER_PARAM_Get(BLK0_highCalWeight, (uint8_t *)(chardouble.ucdoubel)); 
    pScale->highCalWeight = chardouble.tempd;
    
    USER_PARAM_Get(BLK0_ADJUST_K2, (uint8_t *)(&tempd));
    pScale->adjutk2 = tempd;
    
    USER_PARAM_Get(BLK0_midCalCounts , (uint8_t *)(&tempi)); 
    pScale->midCalCounts = tempi;
    USER_PARAM_Get(BLK0_midCalWeight , (uint8_t *)(&tempd)); 
    pScale->midCalWeight = tempd;
    
    USER_PARAM_Get(BLK0_calGeo       , (uint8_t *)(&tempchar)); 
    pScale->calGeo = tempchar;
    USER_PARAM_Get(BLK0_usrGeo       , (uint8_t *)(&tempchar)); 
    pScale->usrGeo = tempchar;
    
//    USER_PARAM_Get(BLK0_zeroCalCounts, (uint8_t *)&(pScale->zeroCalCounts)); 
//    
//    USER_PARAM_Get(BLK0_highCalCounts, (uint8_t *)&(pScale->highCalCounts)); 
//    
//    USER_PARAM_Get(BLK0_highCalWeight, (uint8_t *)&(pScale->highCalWeight)); 
//    
//    USER_PARAM_Get(BLK0_midCalCounts , (uint8_t *)&(pScale->midCalCounts)); 
//    
//    USER_PARAM_Get(BLK0_midCalWeight , (uint8_t *)&(pScale->midCalWeight)); 
//    
//    USER_PARAM_Get(BLK0_calGeo       , (uint8_t *)&(pScale->calGeo)); 
//    
//    USER_PARAM_Get(BLK0_usrGeo       , (uint8_t *)&(pScale->usrGeo)); 
    
    
    // 线性化
    USER_PARAM_Get(BLK0_setupLinearity, (uint8_t *)(&tempchar)); 
    pScale->setupLinearity = tempchar;
    if( pScale->setupLinearity == 0 )
        pScale->upScaleTestPoint = 1; 
    else
        pScale->upScaleTestPoint = 2; 
    
    
}

void InitScaleParamters(SCALE *Pscale)
{
    double  gravityCorrection; 
    double tmpd;
    uint8_t tmpuchar;
    // 获取当前零点
    // 获取当前量程 与分度
    USER_PARAM_Get(BLK0_setupRangeOneCapacity, (uint8_t *)(&tmpd)); 
    Pscale->setupCapacityValue = tmpd;
    Pscale->scaleCapacity =  Pscale->setupCapacityValue; 
    USER_PARAM_Get(BLK0_setupRangeOneIncrement, (uint8_t *)(&tmpd)); 
    Pscale->setupRangeOneIncrement = tmpd;
    Pscale->lowInc  = Pscale->setupRangeOneIncrement;
    Pscale->highInc = Pscale->lowInc; 
    // 获取当前的单位
    //    USER_PARAM_Get(BLK0_setupPrimaryUnit, &unit); 
    
    // calcalater span and liner factors
    getCalParams(Pscale);
    SCALE_CalcSpanFactor(Pscale); 
	SCALE_CalcLinearFactor(Pscale); 
    // 根据GEO CODE 优化得到每个单位多少counts数
    gravityCorrection = geoTable[Pscale->usrGeo] / geoTable[Pscale->calGeo]; 
    // Calculate Divisinon,oneD
    Pscale->countsPerCalUnit[0] *= gravityCorrection; 
	
	Pscale->capacityCounts = (int32_t)(Pscale->scaleCapacity * Pscale->countsPerCalUnit[0]); 
    
	SCALE_CalculateDivisinon(Pscale); 
    // 初始化稳态滤波器的 噪声灵敏度
    // 初始化稳态算法想参数主要灵敏度时间等
    
    USER_PARAM_Get(BLK0_setupMotionRange, (uint8_t *)&tmpuchar); 
    Pscale->motion->sensitivityInD = (SETUP_MOTIONRANGE)tmpuchar;
	USER_PARAM_Get(BLK0_setupnoMotionInterval, (uint8_t *)(&tmpuchar)); 
    Pscale->motion->sensitivityInterval = (SETUP_NOMOTIONINTERVAL)tmpuchar;
	MOTION_Init((Pscale->motion)); 
	/*convert time period from tenths of seconds to # readings*/
    //	MOTION_SetStabilityTimePeriod(&(Pscale->motion), (int)(CONFIG_WEIGHT_CYCLES_PER_SEC*0.1*noMotionInterval[this->motion.sensitivityInterval])); 
    MOTION_SetStabilityTimePeriod((Pscale->motion), (int)(CONFIG_WEIGHT_CYCLES_PER_SEC*0.1*5)); 
    MOTION_Calibrate((Pscale->motion), Pscale->oneD[0]); 
    
    // 设置开机清零或者为为零处理
    //皮重一些参数处理，是否保持上次的皮重状态等等
    
}

/**---------------------------------------------------------------------
* Name         : SCALE_Init
* Description  : Initialize scale wight process parameters
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \return    	: none
*---------------------------------------------------------------------*/

void InitScaleStruct(SCALE *Pscale)
{
    Pscale->motion = &g_motiondata;
    Pscale->zero = &g_zerodata;
    Pscale->tare = &g_taredata;
    Pscale->unit = &g_unitdata;
//    Pscale->filter = &g_filterdata;
//    Pscale->jfilter = &g_jfilterdata;
    g_zerodata.pScale = Pscale;
    g_taredata.pScale = Pscale;
}

void SCALE_Init(SCALE *Pscale)
{
    //	this->scaleType = 'A';
	Pscale->bZeroCommand = 0;
	Pscale->bTareCommand = 0;
	Pscale->programmableTare = -1.0;
	Pscale->numberRanges = ONE_RANGE;
	Pscale->currentRange = 0;
    // 
    InitScaleStruct(Pscale);
//    g_unitdata.p
    InitScaleParamters(Pscale);

//    FilterReInit((g_ScaleData.filter), (g_ScaleData.jfilter)); 
//    ZERO_InstallSavingCurrentZero((g_ScaleData.zero), saveCurrentZero); 

//    g_motiondata.
	// class initilization
    //	FILTER_Init(&(this->filter));
	MOTION_Init((Pscale->motion));
    //	this->zero.pScale = this;
	ZERO_Init((Pscale->zero));
    //	this->tare.pScale = this;
	TARE_Init((Pscale->tare));
    
}

/**---------------------------------------------------------------------
* Name         : SCALE_InstallReInitialization
* Description  : Callback function for re-initialize SCALE data members.
*                This function will be installed from application
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \return    	: none
*---------------------------------------------------------------------*/
void SCALE_InstallReInitialization(SCALE *this, void (*pCallbackFunction)(SCALE *, int))
{
	this->reInitializeDataMembers = pCallbackFunction;
}

/**---------------------------------------------------------------------
* Name         : SCALE_InstallZeroCommandCounter
* Description  : Callback function for installing SCALE zero command counter 
*                function.This function will be installed from application
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \param    	: *pCallbackFunction---function pointer to the installed rountie
* \return    	: none
*---------------------------------------------------------------------*/
void SCALE_InstallZeroCommandCounter(SCALE *this, void (*pCallbackFunction)(void))
{
	this->increaseZeroCommandCounter = pCallbackFunction;
}

/**---------------------------------------------------------------------
* Name         : SCALE_InstallZeroFailureCounter
* Description  : Callback function for installing SCALE zero command failture 
*                counter function.This function will be installed from application
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \param    	: *pCallbackFunction---function pointer to the installed rountie
* \return    	: none
*---------------------------------------------------------------------*/
void SCALE_InstallZeroFailureCounter(SCALE *this, void (*pCallbackFunction)(void))
{
    this->increaseZeroFailureCounter = pCallbackFunction;
}

/**---------------------------------------------------------------------
* Name         : SCALE_ReInitialization
* Description  : Call the installed initialize function
* Prototype in : Scale.h
* \param    	: *this---pointer to SCALE struct
* \param	    : initMode---including normal init,calibration init and exit setup menu
* \return    	: none
*---------------------------------------------------------------------*/
void SCALE_ReInitialization(SCALE *this, int initMode)
{
    if (this->reInitializeDataMembers != NULL)
        this->reInitializeDataMembers(this, initMode);
}

/**---------------------------------------------------------------------
* Name         : SCALE_PostProcess
* Description  : Execute Weight Process:including Zero,Tare,Motion,Unit Switch
* Prototype in : Scale.h
* \param    	: *this---pointer to SCALE struct
* \param	    : filteredCounts---rawcounts retrieved from MELSI module
* \return    	: none
*---------------------------------------------------------------------*/
void SCALE_PostProcess(SCALE *this, long filteredCounts)
{
	uint8_t netMode;
	int32_t relCounts; 
	uint32_t overLoadTimes = 0; 
	bool bMotion, bCoz;
	double incrArray[3], incr;
    //	UNIT_tSymbol currUnit;
    //	float peakWeight = 0.0; 
    //	float currWeight = 0.0; 
	float roundedValue = 0.0; 
    //	float absNetWeight = 0.0; 
    SCALESTATUS scaleStatus; 
    
    
	/* process unit switch command*/
	SCALE_SwitchUnits(this);
	
	/* Multi range or interval process	*/
	SCALE_ProcessMultiRangeInterval(this);
    
	
	/*motion process*/
	MOTION_ProcessMotion((this->motion), filteredCounts);
	bMotion = MOTION_GetMotion((this->motion));
    
	/*power up zero capture*/
	ZERO_ProcessPowerupZero((this->zero), filteredCounts,bMotion);
    
	/* get tare mode*/
	netMode = TARE_GetTareMode((this->tare));
    
	/*zero command process*/
	if (this->bZeroCommand)
	{
	    //here use the error message to show the "Zeroing" information
    	ZERO_SetPushbuttonZero((this->zero), filteredCounts, netMode,bMotion);	
 		/*Increse ZeroCommandCounters lxw 2009-1-15 10:30 change*/
		if ((this->zero->zeroScaleStatus == ZERO_SUCCESS) 
		    || (this->zero->zeroScaleStatus == INVALID_ZERO_FUNCTION)
                || (this->zero->zeroScaleStatus == SCALE_IN_MOTION_DURING_ZERO)
                    || (this->zero->zeroScaleStatus ==ILLEGAL_SCALE_MODE_DURING_ZERO)
                        || (this->zero->zeroScaleStatus == SCALE_OUT_OF_POSITIVEZEROING_RANGE)
                            || (this->zero->zeroScaleStatus == SCALE_OUT_OF_NEGATIVEZEROING_RANGE)
                                || (this->zero->zeroScaleStatus == SCALE_OUT_OF_ZEROING_LIMIT))	
	    {
	     	if (this->increaseZeroCommandCounter)
	            this->increaseZeroCommandCounter();
	        zeroCommandSource = COMMAND_NONE; 
			this->bZeroCommand = 0;//2009-4-1 14:07 lxw add to make the 3s in motion valid
			if (this->zero->underzeroWait && (this->zero->zeroScaleStatus == ZERO_SUCCESS))
			    this->zero->underzeroWait = false; 
	    }
     	// if ((this->zero.zeroScaleStatus != ZERO_IN_PROGRESS) && (this->zero.zeroScaleStatus !=ZERO_SUCCESS))
		if (this->zero->zeroScaleStatus != ZERO_SUCCESS && this->zero->zeroScaleStatus != ZERO_IN_PROGRESS)
	    {
	    	if (this->increaseZeroFailureCounter)
				this->increaseZeroFailureCounter();
            
			displayZeroOperationErrorMessage();
	    }
        
  	}
    
 	/*adjust counts for current zero, include AZM, zero subtraction, COZ and under zero process*/
	relCounts = ZERO_ProcessZero((this->zero), filteredCounts, netMode, this->currentRange,bMotion);
    
	
    /*linearity compensation*/
	switch (this->upScaleTestPoint)
	{
    case 2: // 3 point calibration, use zero, mid and high
        SCALE_Linearity(this, relCounts, 0);   // 直接出重量
        break;
		
    case 1:
    default:
        /*this->fineGrossWeight = relCounts * this->weightPerCount[0];*/
        if (this->countsPerCalUnit[0] > NEAR_ZERO)
            this->fineGrossWeight = relCounts / this->countsPerCalUnit[0];
        break;
	}
    //
	if (this->unit->calUnitType != this->unit->currUnitType)
	{
		this->fineGrossWeight = UNIT_ConvertUnitType((this->unit), this->unit->calUnitType, this->unit->currUnitType, this->fineGrossWeight);		    
	}
	this->roundedGrossWeight = SCALE_RoundedWeight(this->fineGrossWeight, this->currInc);
    
    // over capacity check
	if ((this->fineGrossWeight > this->overCapWeight) || (this->fineGrossWeight > SCALECAPACITYLIMIT))
	{
	    if (this->bOverCapacity == 0)
	    {
            //	        accessBRAMParameters(STATISTICSOVERLOADSADDRESS, (uint8_t *)&overLoadTimes, 4, READOPERATION); 
	        overLoadTimes ++; 
            //	        accessBRAMParameters(STATISTICSOVERLOADSADDRESS, (uint8_t *)&overLoadTimes, 4, WRITEOPERATION); 
	    }
        this->bOverCapacity = 1;
    }
    else
    {
        if (this->bOverCapacity)
            this->bOverCapacity = 0; 
    }
    
	/*change range at COZ*/
	bCoz = ZERO_GetCenterOfZero((this->zero));
    //	if(bCoz&&(this->zerodriftmode != 0))  
    //	{
    //	  Caculatezerodrift(); //caculate zero drift
    //	}
    //	else if (this->zerodriftmode == 0)
    //	{
    //	   if(totalzerodriftfalg == true)
    //	   {
    //		  totalzerodriftfalg = false;	
    ////		  resetSystemInformation();
    //	   }
    //	}	  
    
	
	if (bCoz && (this->tare->fineTareWeight < NEAR_ZERO))
	{
        if(this->currentRange == 1) //在第二量程回到第一个量程事，只做一次清零
            ZERO_SetCurrentZero((this->zero), filteredCounts);
        this->currentRange = 0;
	}
    /*tare handling*/
	TARE_AutoTare((this->tare), &(this->fineGrossWeight), &(this->roundedGrossWeight),bMotion);
    
	if (this->bTareCommand)
	{
	    //here use the error message to show the "Taring" information
		if (TARE_IsPushbuttonTareEnabled((this->tare)) || (tareCommandSource == COMMAND_REMOTE))
		{/*Pushbutton tare is enabled or receive a MTSICS T command*/
			SCALE_GetIncrementArray(this, &incrArray[0]);      // Get increments array of all ranges
			TARE_TakePushbuttonTare((this->tare), PUSHBUTTON_TARE, this->fineGrossWeight, incrArray[0],bMotion);
            
		}
		else
		{
			/*Pushbutton tare is disabled*/
            //			this->bTareCommand = 0;
            
			//change tareScaleStatus to 0 is to clear previous status 
			this->tare->tareScaleStatus &= 0;
			this->tare->tareScaleStatus |= PUSHBUTTON_TARE_NOT_ENABLED;
            //			displayTareOperationErrorMessage();
		}
		if (this->tare->tareScaleStatus != TARE_IN_PROGRESS)
		{
			displayTareOperationErrorMessage(); 
			this->bTareCommand = 0; 
			tareCommandSource = COMMAND_NONE; 
		}
	}
    
	if (this->programmableTare > 0.0)
	{
	    if (this->market == MARKET_ARGENTINA)
	    {
	        scaleStatus = SCALE_CheckScaleStatus(this); 
	        if (scaleStatus == SCALE_IN_MOTION)
	        {
	            this->tare->tareScaleStatus = SCALE_IN_MOTION_DURING_TARE; 
	            displayTareOperationErrorMessage();
	            tareCommandSource = COMMAND_NONE; 
	            this->programmableTare = -1.0; 
	        }
	    }
        //		incr = SCALE_GetIncrementByWeight(this, this->programmableTare);      // Get increment by programmable tare value
        if (this->programmableTare > 0.0)
        {
    	    //here use the error message to show the "Taring" information
            incr = SCALE_GetProgrammableTareIncrement(this); 
    		TARE_SetKeyboardTare((this->tare), KEYBOARD_TARE, this->programmableTare, incr);
    		if (this->tare->tareScaleStatus != TARE_IN_PROGRESS)
    		{
    		    displayTareOperationErrorMessage();
    		    tareCommandSource = COMMAND_NONE; 
    			this->programmableTare = -1.0;
    		}
        }
	}
    
	if (this->bClearCommand)
	{
		TARE_PublicClearTare((this->tare), clearButton);
		this->bClearCommand = 0;
	}
    
	SCALE_AdjustTareAtMultiRangeZero(this, bMotion, bCoz);
	// make tare weight
    //    if (this->tare.tareMode == 'N')
    //	{
    if (this->numberRanges == TWO_RANGES)
    {
    	if (this->currentRange == 1)		
    	{
            if (this->tare->fineTareWeight > NEAR_ZERO)
            {
                roundedValue = SCALE_RoundedWeight(this->tare->fineTareWeight, this->currInc);
                
                if (roundedValue < NEAR_ZERO)
                    this->roundedTareWeight = this->currInc;
                else
                    this->roundedTareWeight = roundedValue;
            }
            else
                this->roundedTareWeight = 0.0; 
            
            if (this->tare->tareSource == KEYBOARD_TARE)
                this->tare->fineTareWeight = this->roundedTareWeight; 
    	}
    	else
    	    this->roundedTareWeight = SCALE_RoundedWeight(this->tare->fineTareWeight, this->currInc); 
    }
    else
        this->roundedTareWeight = SCALE_RoundedWeight(this->tare->fineTareWeight, this->currInc); 
    //    }
    //	this->roundedTareWeight = SCALE_RoundedWeight(this->tare.fineTareWeight, this->currInc); 
    
	this->fineNetWeight = this->fineGrossWeight - this->tare->fineTareWeight;
	this->roundedNetWeight = SCALE_RoundedWeight(this->fineNetWeight, this->currInc);
	
    
    if(this->market == MARKET_USA_NTEP ||this->market == MARKET_CANADA )
	{
        
        this->roundedGrossWeight = this->roundedNetWeight + this->roundedTareWeight; 
	}
	else  //其他情况
        this->roundedGrossWeight = this->roundedNetWeight + this->roundedTareWeight; 
	//在多分度下，OIML认证模式下，预置皮重下
    //	if((this->numberRanges == TWO_INTERVALS)&&(this->market == MARKET_EUROPEAN_OIML)&&(this->tare.tareSource == KEYBOARD_TARE))
    //	{
    //	  this->roundedNetWeight = this->fineGrossWeight - this->roundedTareWeight;
    //	  this->roundedNetWeight = SCALE_RoundedWeight(this->roundedNetWeight,this->currInc);
    //	}
	
	
    //	accessBRAMParameters(STATISTICSPEAKWEIGHTADDRESS, (uint8_t *)&peakWeight, 4, READOPERATION); 
    // 单位转换
    //	if (UNIT_GetCurrentUnitType(&(this->unit)) != PRIME_UNIT)
    //	    currWeight = UNIT_ConvertUnitType(&(this->unit), SECOND_UNIT, PRIME_UNIT, this->roundedGrossWeight);
    //	else
    //        currWeight = this->roundedGrossWeight; 
    //    if (currWeight > peakWeight)
    //        accessBRAMParameters(STATISTICSPEAKWEIGHTADDRESS, (uint8_t *)&currWeight, 4, WRITEOPERATION); 
	
	/* autoprint process*/
    //	AUTOPRINT_AutoPrint(&(this->autoPrint), this->roundedGrossWeight, this->unit.currUnitType,bMotion);
	
	// Make display strings of weights
    //	currUnit = UNIT_GetUnit(&(this->unit), this->unit.currUnitType); 
    //	SCALE_FormatDisplayWeight(this->grossString, &(this->roundedGrossWeight), this->currInc, this->currMiniDisplayInc, currUnit);
    SCALE_FormatDisplayWeight(this->grossString, &(this->roundedGrossWeight), this);
    //	if(scale.numberRanges == TWO_INTERVALS)
    //	  SCALE_FormatDisplayTare(this->tareString, &(this->roundedTareWeight), oldroundincr);
    //	else
    //	 SCALE_FormatDisplayWeight(this->tareString, &(this->roundedTareWeight), this->currInc, this->currMiniDisplayInc, currUnit);
    SCALE_FormatDisplayWeight(this->tareString, &(this->roundedTareWeight), this);
	
    //	SCALE_FormatDisplayWeight(this->netString, &(this->roundedNetWeight), this->currInc, this->currMiniDisplayInc, currUnit);
    SCALE_FormatDisplayWeight(this->netString, &(this->roundedNetWeight), this);
	SCALE_AffirmWeightString(this); 
	//caculate precentage of each loadcell
    //XHT_2018
    //	caculateprecentageload();
	//caculate the each loadcell whether it is overload
    //XHT_2018
    //	CaculateLCoverload();
    //	absNetWeight = fabs(this->roundedNetWeight); 
    
	
}

/**---------------------------------------------------------------------
* Name         : SCALE_Linearity
* Description  : This routine applies the linearity compensation factors
*				  to the uncompensated weight value.  The L_Linearity routine
*				  uses the following formula to to derive the linearity
*				  compensated weight
*					linearized_weight  = G * weight * ( 1 + weight * L )
* Prototype in : Scale.h
* \param    	: *this: pointer to scale struct 
* \param    	: relcounts: the difference between rawcounts and current zero counts
*				  factorIndex: determined by upScaleTestPoint
* \return    	: none
*---------------------------------------------------------------------*/
void SCALE_Linearity(SCALE *this, int32_t relCounts, uint32_t factorIndex)
{
	// this->fineGrossWeight = (this->linearityGain[factorIndex] * relCounts * (1.0 + relCounts * this->linearityFactor[factorIndex])) * this->weightPerCount[factorIndex];
	if (this->countsPerCalUnit[factorIndex] > NEAR_ZERO)
        this->fineGrossWeight = (this->linearityGain[factorIndex] * relCounts * (1.0 + relCounts * this->linearityFactor[factorIndex])) / this->countsPerCalUnit[factorIndex];
}

/**---------------------------------------------------------------------
* Name         : SCALE_ProcessToggleHighPrecisionWeight
* Description  : Process expand display
* Prototype in : Scale.h
* \param    	: *this: pointer to scale struct 
* \return    	: none
*---------------------------------------------------------------------*/
//void SCALE_ProcessToggleHighPrecisionWeight(SCALE *this)
//{
// 	/* Reset expand display mode*/
//    if (this->bLegalForTrade)
//	{
//		if (this->expandDisplayCycles > 0)
//			this->expandDisplayCycles--;
//        else
//		{
//			this->bExpandDisplay = false;
//		}	
//	}
//	
//	if (this->bToggleHighPrecision)
//	{
//      // if (this->scaleTypes == 'I' && IDNet_Approval_x10 == 1)    //if scale type is Idnet adn is calss ii scale with extend res
//      //    return;
////      	this->expandDisplayCycles = CONFIG_WEIGHT_CYCLES_PER_SEC * 5;	// 5 second
//      	 this->expandDisplayCycles = 25 * 5;	 //每秒25次，
//	  if (this->bLegalForTrade)
//      	{
//      	// if (bExpandDisplay == 0)
//      	//    Send_IDNet_U_command(IDNET_U1);
//      		this->bExpandDisplay = true;
//      	}
//      	else
//      	{
////      		if (this->scaleType == 'A' || this->scaleType == 'P')
////      		{
//  			if (this->bExpandDisplay)
//  			{
//  				this->bExpandDisplay = false;
//  	    	}		
//  			else
//  			{    
//  				this->bExpandDisplay = true;
//  	    	}
////      		}
//      	}
//      	this->bToggleHighPrecision = false;
//	}
//}

/**---------------------------------------------------------------------
* Name         : SCALE_ProcessMultiRangeInterval
* Description  : Process multi range and interval
* Prototype in : Scale.h
* \param    	: *this: pointer to scale struct 
* \return    	: none
*---------------------------------------------------------------------*/
void SCALE_ProcessMultiRangeInterval(SCALE *this)
{
	uint8_t oldRange = this->currentRange;
    //	double tmp;
	switch (this->numberRanges)
	{
    case ONE_RANGE:
        this->currInc = this->lowInc;
        this->currentRange = 0;         // range number start from 0
        this->currMiniDisplayInc = this->lowInc;
        break;
		
    case TWO_RANGES:
        if ((this->fineGrossWeight > this->lowHighThreshold) || (this->tare->fineTareWeight > this->lowHighThreshold))
        {
            this->currInc = this->highInc;
            this->currentRange = 1;
        }
        else if (this->currentRange == 1)
        {
            this->currInc = this->highInc;
        }			
        else if (this->currentRange == 0)
        {
            this->currInc = this->lowInc;
        }
        this->currMiniDisplayInc = this->lowInc;
        break;
        
    default:
        break;
	}
    
    
	//recaculate motion counts
	if (this->currentRange != oldRange)
		MOTION_Calibrate((this->motion), this->oneD[this->currentRange]);
}

/**---------------------------------------------------------------------
* Name         : SCALE_CalculateDivisinon
* Description  : Calculate the increment parameters for weight process
* Prototype in : 
* \param    	: *this: pointer to scale struct 
* \return    	: none
*---------------------------------------------------------------------*/
void SCALE_CalculateDivisinon(SCALE *this)
{
	double tmpCountsPerUnit = 0.0;
	double lowOneD,highOneD = 0.0;
	double capacity = 0.0;
	double low = 0.0;
	double hight = 0.0;
	double tempinc = 0.0;
	double tempcap = 0.0;
	UNIT_tSymbol dstUnit;
	uint8_t currIncType;
	double division = 0; 
    double tempdouble;
	
	if (this->currInc == this->lowInc)   //
		currIncType = 1;
	else
		currIncType = 2;
    
	tmpCountsPerUnit = this->countsPerCalUnit[0];
	if (this->unit->currUnitType != this->unit->calUnitType)
	{
	    dstUnit = UNIT_GetUnit((this->unit), this->unit->currUnitType);
	    
		tempcap = this->scaleCapacity;
		tempinc = this->highInc;		
	    division = (this->scaleCapacity / this->highInc); 
		division = (tempcap / tempinc);
        this->highInc = UNIT_ConvertUnitType((this->unit), this->unit->calUnitType, this->unit->currUnitType, this->highInc); 
        this->highInc = UNIT_RoundToNearest1_2_5(this->highInc, dstUnit);
        hight = division * this->highInc;
        this->scaleCapacity =(float)hight;
        this->scaleCapacity = division * this->highInc; 
        
        this->setupCapacityValue = this->scaleCapacity;
        tempcap = this->lowHighThreshold;
        tempinc = this->lowInc;	
        division = (this->lowHighThreshold / this->lowInc); 
        division = (tempcap / tempinc);
        this->lowInc = UNIT_ConvertUnitType((this->unit), this->unit->calUnitType, this->unit->currUnitType, this->lowInc); 
        this->lowInc  = UNIT_RoundToNearest1_2_5(this->lowInc, dstUnit);
        low = division * this->lowInc;
        this->lowHighThreshold = (float)low;
        this->lowHighThreshold = division * this->lowInc; 
        //	    if (this->numberRanges > ONE_RANGE)
        //	    {
        //		this->lowHighThreshold = UNIT_ConvertUnitType(&(this->unit), this->unit.calUnitType, this->unit.currUnitType, this->lowHighThreshold);
        //		this->highInc = UNIT_ConvertUnitType(&(this->unit), this->unit.calUnitType, this->unit.currUnitType, this->highInc); 
        //	    }
	    // the over capacity point is the same in spite of what current unit is
	    if (this->numberRanges > ONE_RANGE)
	    	this->overCapWeight = this->scaleCapacity + this->overCapDivisions * this->highInc;	
	    else
	    	this->overCapWeight = this->scaleCapacity + this->overCapDivisions * this->lowInc;	
		
		
        //		if (this->numberRanges > ONE_RANGE)	
        //			this->highInc = UNIT_RoundToNearest1_2_5(this->highInc, dstUnit);
        //		this->lowInc  = UNIT_RoundToNearest1_2_5(this->lowInc, dstUnit);
		// tmpWgtPerCount = UNIT_ConvertUnitType(&(this->unit), this->unit.calUnitType, this->unit.currUnitType, tmpWgtPerCount);
		tmpCountsPerUnit /= UNIT_ConvertUnitType((this->unit), this->unit->calUnitType, this->unit->currUnitType, 1.0);
	}
	else
	{
		if (this->numberRanges > ONE_RANGE)
		{
	      	USER_PARAM_Get(BLK0_setupRangeTwoCapacity, (uint8_t *)&capacity);
	      	this->scaleCapacity = capacity;
	      	this->setupCapacityValue = capacity;
	      	USER_PARAM_Get(BLK0_setupRangeOneCapacity, (uint8_t *)&capacity);
	      	this->lowHighThreshold = capacity;
			USER_PARAM_Get(BLK0_setupRangeTwoIncrement, (uint8_t *)&tempdouble);
            this->setupRangeTwoIncrement = tempdouble;
			this->highInc = this->setupRangeTwoIncrement;
			USER_PARAM_Get(BLK0_setupRangeOneIncrement, (uint8_t *)&tempdouble);
            this->setupRangeOneIncrement = tempdouble;
			this->lowInc = this->setupRangeOneIncrement;
	    }
		else
		{
			USER_PARAM_Get(BLK0_setupRangeOneCapacity, (uint8_t *)&(tempdouble));
            this->setupCapacityValue = tempdouble;
			this->scaleCapacity =  this->setupCapacityValue;
			this->lowHighThreshold = this->setupCapacityValue;
			USER_PARAM_Get(BLK0_setupRangeOneIncrement, (uint8_t *)&tempdouble);
            this->setupRangeOneIncrement = tempdouble;
			this->lowInc = this->setupRangeOneIncrement;
			this->highInc = this->lowInc; 
		}
	    /*the over capacity point is the same in spite of what current unit is*/
	    if (this->numberRanges > ONE_RANGE)
			this->overCapWeight = this->scaleCapacity + this->overCapDivisions * this->highInc;
		else
			this->overCapWeight = this->scaleCapacity + this->overCapDivisions * this->lowInc;
	}
	
	switch (currIncType)
	{
    case 1:
        this->currInc = this->lowInc;
        break;
		
    case 2:
        this->currInc = this->highInc;
        break;
        
    default:
        this->currInc = this->lowInc;
        break;
	}
	lowOneD  = this->lowInc  * tmpCountsPerUnit;
    //	midOneD  = this->midInc	 * tmpCountsPerUnit;
	highOneD = this->highInc * tmpCountsPerUnit;
    
	this->oneD[0] = lowOneD;
	this->oneD[1] = highOneD;
    
    
    //	if ((this->unit.currUnitType == SECOND_UNIT && this->unit.secUnits == UNIT_cus))
    //	{
    //		this->lowInc =  this->highInc = this->customUnitsIncr;
    //		this->oneD[0] = this->oneD[1] = this->oneD[2] = highOneD;
    //	}
    //xht remove
    
	if (this->numberRanges > ONE_RANGE)
      CalibrateStabilityFilter(highOneD);
//		JFILTER_CalibrateStabilityFilter((this->jfilter), highOneD);
	else
      CalibrateStabilityFilter(lowOneD);
//		JFILTER_CalibrateStabilityFilter((this->jfilter), lowOneD);
}

/**---------------------------------------------------------------------
* Name         : SCALE_GetIncrementArray
* Description  : Retrieve current increment information
* Prototype in : 
* \param    	: *this: pointer to scale struct
* \param    	: *pIncr: pointer to current increment
* \return    	: none
*---------------------------------------------------------------------*/
void SCALE_GetIncrementArray(SCALE *this, double *pIncr)
{
	if (this->numberRanges == TWO_RANGES )    /*2 ranges|| this->numberRanges == TWO_INTERVALS*/
	{
		pIncr[0] = this->lowInc;
		pIncr[1] = this->highInc;
		pIncr[2] = this->highInc;
	}
	else
	{
		pIncr[0] = this->lowInc;
		pIncr[1] = this->lowInc;
		pIncr[2] = this->lowInc;
	}
}

/**---------------------------------------------------------------------
* Name         : SCALE_GetIncrementByWeight
* Description  : Retrieve the increment information by the weight value
* Prototype in : 
* \param    	: *this: pointer to scale struct
* \param    	: weight: weight value
* \return    	: increment according to weight paramenter
*---------------------------------------------------------------------*/
double SCALE_GetIncrementByWeight(SCALE *this, double weight)
{
	if (this->numberRanges == TWO_RANGES)    /*2 ranges || this->numberRanges == TWO_INTERVALS*/
	{
		if (weight > this->lowHighThreshold)
			return this->highInc;
		else 
			return this->lowInc;
	}
	else 	
		return this->lowInc;    
}

/**---------------------------------------------------------------------
* Name         : SCALE_GetCapacityByRange
* Description  : Retrieve the increment information by the range value
* Prototype in : 
* \param    	: *this: pointer to scale struct
* \param    	: range: weight range value
* \return    	: Capacity of the range
*---------------------------------------------------------------------*/
double SCALE_GetCapacityByRange(SCALE *this)
{
    //	if (this->numberRanges == TWO_INTERVALS)    // 2 ranges or 2 interval
    //	{
    //	        return this->lowHighThreshold;
    //	}
    //	else
	return this->scaleCapacity;
}


/**---------------------------------------------------------------------
* Name         : SCALE_RoundedWeight
* Description  : Retrieve the round weight value
* Prototype in : 
* \param    	: weight: raw weight value
* \param    	: incr: increment value
* \return    	: weight value after round operation
*---------------------------------------------------------------------*/
double SCALE_RoundedWeight(double weight, double incr) 
{
	double fVal;
	long lVal;
	
	fVal = weight / incr;
    
	if (fVal < 0) 
		fVal -= 0.50000005;
	else
		fVal += 0.50000005;
    
	lVal = (long)fVal;
	
	fVal = (double)(lVal * incr);
	
	return (fVal);
}


double RoundedWeight(double wgt,double tmpincr_f)
{
    double tmp_f;
    long tmp_d_i;
    
    tmp_f = wgt / tmpincr_f;
    if(tmp_f < 0)
        tmp_f -= 0.49999999999;
    else
        tmp_f += 0.50000000001;
    
    tmp_d_i = (long)tmp_f;
    
    tmp_f = (double)(tmp_d_i * tmpincr_f);
    
    return (tmp_f);
}


/**---------------------------------------------------------------------
* Name         : SCALE_CalcDP
* Description  : Calculates incr's number of decimal places
* Prototype in : 
* \param    	: digits: max digits in display (include negative sign)
* \param    	: incr: increment value
* \return    	: 
*---------------------------------------------------------------------*/
int32_t SCALE_CalcDP(int32_t digits, double incr) 
{
	int32_t i;
	float fPart, iPart, error = ERROR;
	
	for (i = 0; i < (digits - 3); i++, incr *= 10.0)
	{
		if (incr >= (1.0 - error))
		{
			// fpart = modf (incr, &ipart);
			iPart = (float)((int32_t)incr);
			fPart = incr - iPart;
			if ((fPart < error) || (fPart > (1.0 - error)))
				break;
		}
		if (error < 0.1)
			error *= 10.0;
	}
	return i;
}

/**---------------------------------------------------------------------
* Name         : SCALE_GetDp
* Description  : 
* Prototype in : 
* \param    	: incr: increment value
* \return    	: 
*---------------------------------------------------------------------*/
int32_t SCALE_GetDp(float incr)
{
    int32_t incrIndex;
    
    incrIndex = SCALE_GetIncrIndex(incr);
    
    if (incrIndex == -1)
        return 0;
    else
        return (CONFIG_MAX_DP-incrIndex/3);
    //        return (((CONFIG_MAX_DP-incrIndex/3)>0) ? (CONFIG_MAX_DP-incrIndex/3):(-1));
}

/**---------------------------------------------------------------------
* Name         : SCALE_SwitchUnits
* Description  : Performing unit switch.
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \return    	: none
*---------------------------------------------------------------------*/
void SCALE_SwitchUnits(SCALE *this)
{
    UNIT_tType oldUnitType = this->unit->currUnitType;
    double incrArray[3];
    double tmpCapacity = 0.0;
    double tmpInc   = 0.0;
    int32_t incrIndex;
    bool switchPermit = true;
    UNIT_tSymbol dstUnit;
    
    //  unit switch 
	if (this->bSwitchUnits)
	{
		if (oldUnitType == PRIME_UNIT)
		{
			if (this->unit->secUnits != UNIT_none)
			    this->unit->currUnitType = SECOND_UNIT;
		}
		else if (oldUnitType == SECOND_UNIT)
		{
			this->unit->currUnitType = PRIME_UNIT;
		}
        //		accessBRAMParameters(CURRENTUNITADDRESS, (uint8_t *)&this->unit.currUnitType, 1, WRITEOPERATION); 
		this->bSwitchUnits = 0;
	}
    
    if (oldUnitType != this->unit->currUnitType)
    {
        if (this->unit->currUnitType == SECOND_UNIT)
        {
            if (this->numberRanges == ONE_RANGE)
            {
                USER_PARAM_Get(BLK0_setupRangeOneCapacity, (uint8_t *)&tmpCapacity);
            }
            else
                USER_PARAM_Get(BLK0_setupRangeTwoCapacity, (uint8_t *)&tmpCapacity);
            
            tmpCapacity = UNIT_ConvertUnitType((this->unit), oldUnitType, this->unit->currUnitType, tmpCapacity);
            if (tmpCapacity > SCALECAPACITYLIMIT)
            {
                //                showCommanfFailMessage(TID_EXCEED_CAPACITY_LIMIT); 
                this->unit->currUnitType = PRIME_UNIT;
                //                accessBRAMParameters(CURRENTUNITADDRESS, (uint8_t *)&scale.unit.currUnitType, 1, WRITEOPERATION); 
                return;
            }
            
            dstUnit = UNIT_GetUnit((this->unit), this->unit->currUnitType); 
            tmpInc = this->lowInc;
            tmpInc = UNIT_ConvertUnitType((this->unit), oldUnitType, this->unit->currUnitType, tmpInc);
            tmpInc = UNIT_RoundToNearest1_2_5(tmpInc,dstUnit);
            incrIndex = SCALE_GetIncrIndex(tmpInc);
            if (incrIndex == -1)
                switchPermit = false;
            if (this->numberRanges == TWO_RANGES )//||this->numberRanges ==  TWO_INTERVALS
            {
                tmpInc = this->highInc;
                tmpInc = UNIT_ConvertUnitType((this->unit), this->unit->calUnitType, this->unit->currUnitType, tmpInc);
                tmpInc = UNIT_RoundToNearest1_2_5(tmpInc,dstUnit);
                incrIndex = SCALE_GetIncrIndex(tmpInc);
                if (incrIndex == -1)
                    switchPermit = false;
            }
            if (!switchPermit)
            {
                //                showCommanfFailMessage(TID_INVALID_INCREMENT_VALUE); 
                this->unit->currUnitType = PRIME_UNIT;
                //                accessBRAMParameters(CURRENTUNITADDRESS, (uint8_t *)&this->unit.currUnitType, 1, WRITEOPERATION); 
                return;
            }
        }
    	this->fineGrossWeight = UNIT_ConvertUnitType((this->unit), oldUnitType, this->unit->currUnitType, this->fineGrossWeight);
    	SCALE_CalculateDivisinon(this);
        SCALE_GetIncrementArray(this, &incrArray[0]);      // Get increments array of all ranges
    	MOTION_Calibrate((this->motion), this->oneD[this->currentRange]);
    	ZERO_Calibrate((this->zero), &this->oneD[0], &incrArray[0], this->capacityCounts);
    	TARE_Calibrate((this->tare), oldUnitType, this->unit->currUnitType, &incrArray[0]);
    	// fineTareWeight = TARE_FineTareWeight();
        //    	AUTOPRINT_Calibrate(&(this->autoPrint));
    }
}


/**---------------------------------------------------------------------
* Name         : SCALE_AdjustTareAtMultiRangeZero
* Description  : Adjust tare at multi-range zero
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \param    	: bMotion---BOOL variable to denote whether scale in motion state
* \param    	: bCoz---BOOL variable to denote whether scale in COZ state
* \return    	: none
*---------------------------------------------------------------------*/
void SCALE_AdjustTareAtMultiRangeZero(SCALE *this, bool bMotion, bool bCoz)
{
	float roundedValue, inc;
	
	inc = this->currInc;
	if (this->tare->tareMode == 'N')
	{
		if (this->currentRange == 1)		
		{
		    if ((bCoz && !bMotion) && this->tare->tareSource == KEYBOARD_TARE)
		    {
                if (this->bExpandDisplay)
                    inc *= 10;
                
    			roundedValue = SCALE_RoundedWeight(this->tare->fineTareWeight, inc);
    			
    			if (roundedValue < NEAR_ZERO)
    				this->tare->fineTareWeight = inc;
    			else
    				this->tare->fineTareWeight = roundedValue;
    			/*to save curent tare parameters    02-Mar-09  lxw*/
    			this->currentTareWeight=this->tare->fineTareWeight;//save for next power up lxw
                // XHT_2018
                //    			accessBRAMParameters(CURRENTTAREWEIGHTADDRESS, (uint8_t *)&(this->currentTareWeight), 4, WRITEOPERATION); 
		    }
            
		}
	}
}


/**---------------------------------------------------------------------
* Name         : SCALE_GetIncrIndex
* Description  : Retrieve increment index in the increment table
* Prototype in : 
* \param    	: incr---increment value
* \return    	: increment index
*---------------------------------------------------------------------*/
int32_t SCALE_GetIncrIndex(float incr)
{
    int32_t i, start, end;
    float result;
    
    start = CONFIG_MIN_INCR_INDEX;
    end   = CONFIG_MAX_INCR_INDEX;
    
    while (end >= start)
    {
    	i = (start + end) >> 1;
    	result = incr - incrTable[i];
    	
    	if (result > NEAR_ZERO)
    		start = i + 1;
    	else if (result < -NEAR_ZERO)
    		end = i - 1;
    	else
    		return i;
    }
    
    return -1;
}

/**---------------------------------------------------------------------
* Name         : SCALE_FormatDisplayWeight
* Description  : Format the weight value to string to display according
*				  the current unit,increment
* Prototype in : 
* \param    	: *pWeightString---string buffer
* \param    	: *pWeight---wieght value
* \param    	: currIncr---current increment value
* \param    	: miniIncr---increment value
* \param    	: currUnit---current unit 
* \return    	: none
*---------------------------------------------------------------------*/
//void SCALE_FormatDisplayWeight(char *pWeightString, float *pWeight, float currIncr, float miniIncr, UNIT_tSymbol currUnit)
void SCALE_FormatDisplayWeight(char *pWeightString, double *pWeight, SCALE *Ptrscale)
{
	int i, j; 
	int32_t currDp, miniDp, incrIndex;
    
	currDp = SCALE_GetDp(Ptrscale->currInc);
	miniDp = SCALE_GetDp(Ptrscale->currMiniDisplayInc);
	
	//this if branch used to avoid error in expand display
	if (Ptrscale->bExpandDisplay)
	{
	    if (currDp == 0)
	    {
	        incrIndex = SCALE_GetIncrIndex(Ptrscale->currInc);
	        if ((incrIndex != 12) && (incrIndex != 13) && (incrIndex != 14))
	            currDp = 5;
	    }
	    
	    if (miniDp == 0)
	    {
	        incrIndex = SCALE_GetIncrIndex(Ptrscale->currInc);
	        if ((incrIndex != 12) && (incrIndex != 13) && (incrIndex != 14))
	            miniDp = 5;
	    }
	}
	//There is no decimal
    
	// create format string & zero string for lb-oz weight, if needed.
	// the source for the data is always "oz" units.
    // other units
	{
    	// if (MT_fabs(*weightPtr) < MIN_STEP_VAL)	// weight == 0.0
    	if (fabs(*pWeight) < (Ptrscale->currMiniDisplayInc-NEAR_ZERO))     // weight == 0.0
    	{
    		if (currDp <= 0)				// 1, 2, 5 or 10, 20, 50 or 100, 200
    			j = -currDp + 1;
    		else							// include dp
    			j = currDp + 2;
            
    		for (i = 0; i < j; i++)
    		{
    			pWeightString[i] = '0';
    		}
    		pWeightString[i] = 0;
            
    		if (currDp > 0)
    			pWeightString[1] = '.';
    	}
    	else
    	{
    		if (currDp > 0)
    		{
    			i = currDp;
    			j = CONFIG_MAX_WT_DIGITS + 1;
    		}
    		else
    		{
                //    			i = -1;
				i = 0;
				j = CONFIG_MAX_WT_DIGITS;
                //    			j = CONFIG_MAX_WT_DIGITS+1;// 16_Feb_09 lxw CONFIG_MAX_WT_DIGITS;
    		}
    		// sprintf (weightFormat, "%%%bd.%bdf", j, i);
    		// sprintf(weightString, weightFormat, *weightPtr);
            RB_FORMAT_Double((char *)pWeightString, i, *pWeight, j);
            
            // MT_ftoa(*weightPtr, weightString, i);
    	}
        //    	if (currDp < 0)
        //    	    currDp = -1;
        //    	if (miniDp < 1)
        //    	    miniDp = -1;    
        if (miniDp > 0)
        {
            //the lowInc has at least 1 decimal
            if (miniDp > currDp)
        	{   // to keep the display digits the same as the digits when mini display increment is used
        	    i = CONFIG_MAX_WT_DIGITS+1-(miniDp-currDp);
        	    RB_STRING_AlignRight((char *)pWeightString, i);
        	    while (i < (CONFIG_MAX_WT_DIGITS+1))
        	    {
        	        pWeightString[i] = ' ';
        	        i++;
        	    }
        	    pWeightString[CONFIG_MAX_WT_DIGITS+1] = '\0';
        	}
        	else
                RB_STRING_AlignRight((char *)pWeightString, CONFIG_MAX_WT_DIGITS+1);
        }
        else
        {
            //there is no decimal for 2 ranges
            RB_STRING_AlignRight((char *)pWeightString, CONFIG_MAX_WT_DIGITS+1);
        }
    }
}

void SCALE_FormatDisplayTare(char *pWeightString, double *pWeight, double currIncr)
{
    
    int32_t Dp,templong;
    Dp = SCALE_GetDp(currIncr);
    //   	if (scale.bExpandDisplay)
    //	{
    //		if(Dp < 5)
    //		  Dp+=1;
    //	}
	
    switch(Dp)
    {
    case 0:
        templong = (int32_t)*pWeight;
        snprintf(pWeightString,CONFIG_MAX_WT_DIGITS+1,"%d",templong);
        break;
    case 1:
        snprintf(pWeightString,CONFIG_MAX_WT_DIGITS+1,"%.1f",*pWeight);
        break;
    case 2:
        snprintf(pWeightString,CONFIG_MAX_WT_DIGITS+1,"%.2f",*pWeight);
        break;
    case 3:
        snprintf(pWeightString,CONFIG_MAX_WT_DIGITS+1,"%.3f",*pWeight);
        break;
    case 4:
        snprintf(pWeightString,CONFIG_MAX_WT_DIGITS+1,"%.4f",*pWeight);
        break;
    case 5:
        snprintf(pWeightString,CONFIG_MAX_WT_DIGITS+1,"%.5f",*pWeight);
        break;
    default:		 
        templong = (int32_t)*pWeight;
        snprintf(pWeightString,CONFIG_MAX_WT_DIGITS+1,"%d",templong);
        break;
    }
    RB_STRING_AlignRight((char *)pWeightString, CONFIG_MAX_WT_DIGITS+1);
}
/**---------------------------------------------------------------------
* Name         : SCALE_CalcWeightStringLength
* Description  : calculate the weight string length
* Prototype in : 
* \param    	: *stringPtr---pointer to weight string
* \return    	: string length
*---------------------------------------------------------------------*/
static uint8_t SCALE_CalcWeightStringLength(char *stringPtr)
{
    char * weightString = stringPtr; 
    uint8_t length = 0; 
    
    while(*weightString == ' ')
        weightString ++;
    
    length = strlen(weightString); 
    return length; 
}

/**---------------------------------------------------------------------
* Name         : SCALE_AffirmWeightString
* Description  : judge if the weight string too long to display
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \return    	: none
*---------------------------------------------------------------------*/
bool SCALE_AffirmWeightString(SCALE *this)
{
    bool error = false;
    bool weightStringInvalid ;
    uint8_t length = 0, limit;
    
    length = SCALE_CalcWeightStringLength(this->tareString);
    if (length > 7)
        error = true;
    if (error)
    {
        weightStringInvalid = true;
        this->bClearCommand = 1;
        //        return weightStringInvalid ;
    }    
    
    length = SCALE_CalcWeightStringLength(this->grossString); 
    if (this->roundedGrossWeight > -NEAR_ZERO)
        limit = 7;
    else
        limit = 8;
    if (length > limit)
        error = true;
    if (error)
    {
        weightStringInvalid = true;
        //        return weightStringInvalid;
    }
    
    length = SCALE_CalcWeightStringLength(this->netString); 
    if (this->roundedNetWeight > -NEAR_ZERO)
        limit = 7;
    else
        limit = 8;
    if (length > limit)
        error = true;
    if (error)
    {
        weightStringInvalid = true;
        //        return weightStringInvalid;
    }
    
    //    weightStringInvalid = false;
    return weightStringInvalid;
}

/**---------------------------------------------------------------------
* Name         : SCALE_GetRoundedGross
* Description  : Retrieve round gross value
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \return    	: round gross value
*---------------------------------------------------------------------*/
double SCALE_GetRoundedGross(SCALE *this)
{
    return this->roundedGrossWeight;
}

/**---------------------------------------------------------------------
* Name         : SCALE_GetDisplayMode
* Description  : Retrieve display mode:expend or normal
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \return    	: display mode:expend or normal
*---------------------------------------------------------------------*/
bool SCALE_GetDisplayMode(SCALE *this)
{
    return this->bExpandDisplay;
}

/**---------------------------------------------------------------------
* Name         : SCALE_GetLFT
* Description  : Retrieve trade mode
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \return    	: trade mode
*---------------------------------------------------------------------*/
bool SCALE_GetLFT(SCALE *this)
{
    return this->bLegalForTrade;
}

/**---------------------------------------------------------------------
* Name         : SCALE_GetOverCapacity
* Description  : Retrieve over capacity division
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \return    	: over capacity division
*---------------------------------------------------------------------*/
uint8_t SCALE_GetOverCapacity(SCALE *this)
{
    return this->bOverCapacity;
}

/**---------------------------------------------------------------------
* Name         : SCALE_CalcSpanFactor
* Description  : calculate span factor
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \return    	: none
*---------------------------------------------------------------------*/
void SCALE_CalcSpanFactor(SCALE *this)
{
    int i;
    
	for (i = 0; i < (CONFIG_MAX_UPSCALE_TEST_POINT-1); i++)
	    this->countsPerCalUnit[i] = 0;
    
	switch (this->upScaleTestPoint)
	{
    case 1:
    case 2:
        this->countsPerCalUnit[0] = (this->highCalCounts - this->zeroCalCounts) / this->highCalWeight;
        break;
        
    default:
        break;
	}
}

/**---------------------------------------------------------------------
* Name         : SCALE_CalcLinearFactor
* Description  : calculate linear factor
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \return    	: none
*---------------------------------------------------------------------*/
void SCALE_CalcLinearFactor(SCALE *this)
{
	int i;
	float wt1, wt2, R1;
    //	float wt12, wt22, R12;
    
	for (i = 0; i < (CONFIG_MAX_UPSCALE_TEST_POINT-1); i++)
	{
		this->linearityGain[i] = 0;
		this->linearityFactor[i] = 0;
	}
    
	switch (this->upScaleTestPoint)
	{
    case 2:
        wt1 = this->midCalWeight * this->countsPerCalUnit[0];
        wt2 = this->highCalWeight * this->countsPerCalUnit[0];
        R1  = this->midCalCounts - this->zeroCalCounts;
        this->linearityFactor[0] = (R1 - wt1) / (wt1 * wt2 - R1 * R1);
        this->linearityGain[0] = 1.0 / (1.0 + this->linearityFactor[0] * wt2);
        break;
        
    default:
        break;
	}
}

/**---------------------------------------------------------------------
* Name         : SCALE_GetWeight
* Description  : retrieve scale gross/tare/net weight value
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \param    	: *pWeight---pointer to WEIGHT struct
* \return    	: none
*---------------------------------------------------------------------*/
void SCALE_GetWeight(SCALE *this, WEIGHT *pWeight)
{
	pWeight->roundedGross = this->roundedGrossWeight;
	pWeight->roundedNet   = this->roundedNetWeight;
	pWeight->roundedTare  = this->roundedTareWeight;
}

/**---------------------------------------------------------------------
* Name         : SCALE_GetWeightString
* Description  : retrieve scale gross/tare/net weight string
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \param    	: *pWeight---pointer to weight string buffer
* \return    	: none
*---------------------------------------------------------------------*/
void SCALE_GetWeightString(SCALE *this, WEIGHT_STRING *pWeightString)
{
	pWeightString->pGrossString = this->grossString;
	pWeightString->pNetString   = this->netString;
	pWeightString->pTareString  = this->tareString;
}

/**---------------------------------------------------------------------
* Name         : SCALE_CheckScaleStatus
* Description  : get scale status
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \return    	: current scale status
*---------------------------------------------------------------------*/
SCALESTATUS SCALE_CheckScaleStatus(SCALE *this)
{
    bool bMotion, bPowerUpZeroCaptured, bUnderZero; 
    bool bExpandDisplay, bLegalForTrade, bOverCapacity; 
    
    bPowerUpZeroCaptured = ZERO_GetPowerUpZeroCaptured((this->zero));
	if (!bPowerUpZeroCaptured)
	{
		return SCALE_BAD_ZERO;
	}
    
    bOverCapacity = SCALE_GetOverCapacity(this);
	if (bOverCapacity)
		return SCALE_OVERCAPACITY;
    
    bUnderZero = ZERO_GetUnderZero((this->zero));
	if (bUnderZero)
		return SCALE_UNDER_ZERO;
    
	bExpandDisplay = SCALE_GetDisplayMode(this);
	bLegalForTrade = SCALE_GetLFT(this);
	if (bExpandDisplay && bLegalForTrade)
		return SCALE_IN_EXPAND;
    
    bMotion = MOTION_GetMotion((this->motion));
	if (bMotion)
		return SCALE_IN_MOTION;
    
    return SCALE_IN_GOOD_STATUS; 
}

/**---------------------------------------------------------------------
* Name         : SCALE_ShowScaleStatus
* Description  : show scale status on screen
* Prototype in : 
* \param    	: scaleStatus---scale status
* \return    	: current scale status
*---------------------------------------------------------------------*/
void SCALE_ShowScaleStatus(SCALESTATUS scaleStatus)
{
    //    switch (scaleStatus)
    //    {
    //        case SCALE_BAD_ZERO:
    //            showErrorMessage((char *)getTextString(TID_POWER_UP_ZERO_NOT_CAPTURED), NORMAL_INTERVAL); 
    //            break; 
    //        
    //        case SCALE_OVERCAPACITY:
    //            showErrorMessage((char *)getTextString(TID_SCALE_OVER_CAPACITY), NORMAL_INTERVAL); 
    //            break; 
    //        
    //        case SCALE_UNDER_ZERO:
    //            showErrorMessage((char *)getTextString(TID_SCALE_UNDER_ZERO), NORMAL_INTERVAL); 
    //            break; 
    //        
    //        case SCALE_IN_EXPAND:
    //            showErrorMessage((char *)getTextString(TID_SCALE_IN_EXPAND_MODE), NORMAL_INTERVAL); 
    //            break; 
    //        
    //        case SCALE_IN_MOTION:
    //            showErrorMessage((char *)getTextString(TID_MOTION), NORMAL_INTERVAL); 
    //            break; 
    //    }
}

/**---------------------------------------------------------------------
* Name         : SCALE_GetProgrammableTareIncrement
* Description  : get increment to round keyboard tare value
* Prototype in : 
* \param    	: *this---pointer to SCALE struct
* \return    	: current scale status
*---------------------------------------------------------------------*/
float SCALE_GetProgrammableTareIncrement(SCALE *this)
{
    
    if (this->currentRange == 0)
    {
        if (this->programmableTare < this->lowHighThreshold)
            return this->lowInc; 
        else
            return this->highInc; 
    }
    //    else if(this->numberRanges == TWO_INTERVALS)
    //	{
    //	  if ((this->market == MARKET_USA_NTEP) || (this->market == MARKET_CANADA))
    //	    return this->currInc;
    //	  else
    //		return this->lowInc; 		    
    //	}
	else
        return this->highInc;
    
}


double Calculate_fineGrossweight(int relCounts,CAL_STAT * cal_stat)
{
    double fineGrossWeight;
    double GravityCorrection;
    double m_WeightPerCount;
    GravityCorrection = geoTable[cal_stat->Cal_grvty_code]/geoTable[cal_stat->Local_grvty_code];
    m_WeightPerCount = (cal_stat->span*GravityCorrection);
    
    switch(cal_stat->testpoints)
    {
    case 2://3 point calibration ,use zero,mid and high
        
        fineGrossWeight = (cal_stat->scale_factor * relCounts * (1.0 + relCounts * cal_stat->linear_factor)) * m_WeightPerCount;
        break;
        
    case 3:	//4 point calibration,use zero,low,mid amd high
        if(relCounts > (cal_stat->mid_counts - cal_stat->zero_counts))
        {
            m_WeightPerCount = (cal_stat->span2*GravityCorrection);
            fineGrossWeight = (cal_stat->scale_factor2 * relCounts * (1.0 + relCounts * cal_stat->linear_factor2)) * m_WeightPerCount;
        }
        else
        {
            fineGrossWeight = (cal_stat->scale_factor * relCounts * (1.0 + relCounts * cal_stat->linear_factor)) * m_WeightPerCount;
        }
        break;
        
        
    case 4:	//5 point calibraiton,use zero,xlow,low,mid,high
        if(relCounts > (cal_stat->mid_counts - cal_stat->zero_counts))
        {
            m_WeightPerCount = (cal_stat->span3*GravityCorrection);
            fineGrossWeight = (cal_stat->scale_factor3 * relCounts * (1.0 + relCounts * cal_stat->linear_factor3)) * m_WeightPerCount;
        }
        else if(relCounts > (cal_stat->low_counts - cal_stat->zero_counts))
        {
            m_WeightPerCount = (cal_stat->span2*GravityCorrection);
            fineGrossWeight = (cal_stat->scale_factor2 * relCounts * (1.0 + relCounts * cal_stat->linear_factor2)) * m_WeightPerCount;
        }
        else
        {
            
            fineGrossWeight = (cal_stat->scale_factor * relCounts * (1.0 + relCounts * cal_stat->linear_factor)) * m_WeightPerCount;
        }
        break;
        
    default:
        fineGrossWeight = relCounts * m_WeightPerCount;
        break;
    }
    return fineGrossWeight;
}


int CheckIncrIsValid(double data)
{
	int i;
    
	// check incr, whether it is in incr table
	for (i = 0; i<CONFIG_MAX_INCR_INDEX; i++)
	{
		if (data == incrTable[i])
			break;
	}
    
	if (i >= CONFIG_MAX_INCR_INDEX)
		return -1;
    
	return 0;
}

void ResetScaleParameters()
{
    uint8_t param; 
	uint16_t iparam; 
	double dtmep =0.0; 
	int l = 0; 
	UNIT_tSymbol unit; 
	char name[21] = {0}; 
    //	char capacity1[10] = "50.0"; 
    //	char capacity2[10] = "100.0"; 
    //	uint8_t MACAddress[] = {0x00, 0x00, 0x00, 0x10, 0x52, 0xFF, 0x01, 0x14, 0x6}; 
    //    uint8_t boardInfoReserved[54] = {0}; 
    char dateString[12] = {0}; 
    
	// Tare
	param = 1; 
	USER_PARAM_Set(BLK0_setupPushButtonTare,                (uint8_t *)&param); 
//    HAL_Delay(20);
	param = 1; 
	USER_PARAM_Set(BLK0_setupKeyboardTare,                  (uint8_t *)&param); 
//    HAL_Delay(20);
	param = 0; 
	USER_PARAM_Set(BLK0_setupNetSignCorrection,             (uint8_t *)&param); 
//    HAL_Delay(20);
    
	// Filter
	dtmep = 2.0; 
	USER_PARAM_Set(BLK0_setupLowPassFilter,                 (uint8_t *)&param); 
//    HAL_Delay(20);
	param = 0; 
	USER_PARAM_Set(BLK0_setupStabilityFilter,               (uint8_t *)&param); 
//    HAL_Delay(20);
    /*these two parameters reset will be deleted when manufactured formally*/
    //    USER_PARAM_Set(BLK0_MACAddress,                         (uint8_t *)&MACAddress); 
    //    USER_PARAM_Set(BLK0_boardInfoReserved,                  (uint8_t *)&boardInfoReserved); 
    /***********************************************************************/
    
    
    memset(name, 0, sizeof(name));
    USER_PARAM_Set(BLK0_setupCertificateNo,             (uint8_t *)name); 
//    HAL_Delay(20);
    param = 0; 
    USER_PARAM_Set(BLK0_setupApproval,                  (uint8_t *)&param); 	
//    HAL_Delay(20);
    /*F1.2 Capacity and Increment*/
    unit = UNIT_kg; /*kg is the default unit*/
    USER_PARAM_Set(BLK0_setupPrimaryUnit,               (uint8_t *)&unit); 
//    HAL_Delay(20);
    //		param = 0; /*PrimaryUnit is the default current unit*/
    //		accessBRAMParameters(CURRENTUNITADDRESS, (uint8_t *)&param, 1, WRITEOPERATION); 
    
    param = 0; /*default 1 range*/
    USER_PARAM_Set(BLK0_setupRanges, (uint8_t *)&param); 
//    HAL_Delay(20);
    dtmep = 50.0; /*set the default capacity for range one*/
    USER_PARAM_Set(BLK0_setupRangeOneCapacity,          (uint8_t *)&dtmep); 
    //		USER_PARAM_Set(BLK0_setupRangeOneCapacityString,    (uint8_t *)capacity1); 
//    HAL_Delay(20);
    dtmep = 0.01; /*set the default increment 0.01 for range one*/
    USER_PARAM_Set(BLK0_setupRangeOneIncrement,         (uint8_t *)&dtmep); 
//    HAL_Delay(20);
    dtmep = 100.0; /*set the default capacity for range two*/
    USER_PARAM_Set(BLK0_setupRangeTwoCapacity,          (uint8_t *)&dtmep); 
    //    	USER_PARAM_Set(BLK0_setupRangeTwoCapacityString,    (uint8_t *)capacity2); 
//    HAL_Delay(20);
    dtmep = 0.02; /*set the default increment 0.02 for range one*/
    USER_PARAM_Set(BLK0_setupRangeTwoIncrement,         (uint8_t *)&dtmep); 
//    HAL_Delay(20);
    param = 5; 
    USER_PARAM_Set(BLK0_setupBlankoverCapacity,         (uint8_t *)&param); 
//    HAL_Delay(20);
    // calibration
    param = 0; 
    USER_PARAM_Set(BLK0_setupLinearity,                 (uint8_t *)&param); 
//    HAL_Delay(20);
    l = 0x00; 
    USER_PARAM_Set(BLK0_zeroCalCounts,                  (uint8_t *)&l); 
//    HAL_Delay(50);
    l = 250000; 
    USER_PARAM_Set(BLK0_midCalCounts,                   (uint8_t *)&l); 
//    HAL_Delay(20);
    dtmep = 25.0; 
    USER_PARAM_Set(BLK0_midCalWeight,                   (uint8_t *)&dtmep); 
//    HAL_Delay(50);
    l = 500000; 
    USER_PARAM_Set(BLK0_highCalCounts,                  (uint8_t *)&l); 
//    HAL_Delay(20);
    dtmep = 50.0; 
    USER_PARAM_Set(BLK0_highCalWeight,                  (uint8_t *)&dtmep); 
    
//    HAL_Delay(50);
    USER_PARAM_Set(BLK0_calibrationDate,                (uint8_t *)dateString); 
    
    dtmep = 1.0;
    USER_PARAM_Set(BLK0_ADJUST_K2,                    (uint8_t *)&dtmep); 
    
    
//    HAL_Delay(50);
    param = 12; 
    USER_PARAM_Set(BLK0_calGeo,                         (uint8_t *)&param); 
//     HAL_Delay(20);
    USER_PARAM_Set(BLK0_usrGeo,                         (uint8_t *)&param); 
//    HAL_Delay(20);
    /* calculated calibration*/
    dtmep = 50.0; 
    USER_PARAM_Set(BLK0_cellCapacity,                   (uint8_t *)&dtmep); 
//    HAL_Delay(50);
    unit = UNIT_kg; 
    USER_PARAM_Set(BLK0_cellCapUnit,                    (uint8_t *)&unit); 
//    HAL_Delay(20);
    dtmep = 0.0; 
    USER_PARAM_Set(BLK0_preload,                        (uint8_t *)&dtmep); 
//    HAL_Delay(20);
    unit = UNIT_kg; 
    USER_PARAM_Set(BLK0_preloadUnit,                    (uint8_t *)&unit); 
//    HAL_Delay(20);
    /*F1.4 Zero*/
    param = 1; /*default auto zero enable*/
    USER_PARAM_Set(BLK0_setupAutoZero,                  (uint8_t *)&param); 
//    HAL_Delay(20);
    param = 0; /*default auto zero range:+/- 0.5d*/
    USER_PARAM_Set(BLK0_setupAutoZeroRange,             (uint8_t *)&param); 
//    HAL_Delay(20);
    param = 1; /*default under zero blanking:5d*/
    USER_PARAM_Set(BLK0_setupUnderZeroBlanking,         (uint8_t *)&param); 
//    HAL_Delay(20);
    param = 0; 	/*default powerup zero disable*/
    USER_PARAM_Set(BLK0_setupPowerupZero,               (uint8_t *)&param); 
//    HAL_Delay(20);
    param = 1; 	/*default pushbutton zero enable*/
    USER_PARAM_Set(BLK0_setupPushButtonZero,            (uint8_t *)&param); 
//    HAL_Delay(20);
    /*F1.5 Tare*/
    param = 0; /*default auto tare disable*/
    USER_PARAM_Set(BLK0_setupAutoTare,                  (uint8_t *)&param); 
//    HAL_Delay(20);
    dtmep = 0.0; /*default auto tare threshold*/
    USER_PARAM_Set(BLK0_setupAutoTareThreshold,         (uint8_t *)&dtmep); 
//    HAL_Delay(20);
    dtmep = 0.0; /*default auto tare reset threshold*/
    USER_PARAM_Set(BLK0_setupAutoTareResetThreshold,    (uint8_t *)&dtmep); 
//    HAL_Delay(20);
    param = 0; /*default auto tare motion check enable*/
    USER_PARAM_Set(BLK0_setupAutoTareMotionCheck,       (uint8_t *)&param); 
//    HAL_Delay(20);
    param = 0; /*default autoclear tare disable*/
    USER_PARAM_Set(BLK0_setupAutoClearTare,             (uint8_t *)&param); 
//    HAL_Delay(20);
    param = 0; /*default clear tare after print disable*/
    USER_PARAM_Set(BLK0_setupClearafterPrint,           (uint8_t *)&param); 
//    HAL_Delay(20);  
    dtmep = 0.0; /*default autoclear tare threshold*/
    USER_PARAM_Set(BLK0_setupAutoClearTareThreshold,    (uint8_t *)&dtmep); 
//    HAL_Delay(20);
    param = 0; /*default autoclear tare motion check enable*/
    USER_PARAM_Set(BLK0_setupAutoClearTareMotionCheck,  (uint8_t *)&param); 
//    HAL_Delay(20);
    /*F1.6 Second Unit*/
    unit = UNIT_none; 
    USER_PARAM_Set(BLK0_setupSecondUnit,                (uint8_t *)&param); 
//    HAL_Delay(20);
    /*F1.8 Motion and Stability*/
    iparam = 1; /*default motion range 1d*/
    USER_PARAM_Set(BLK0_setupMotionRange,               (uint8_t *)&iparam); 
//    HAL_Delay(20);
    param = 0; 
    USER_PARAM_Set(BLK0_setupnoMotionInterval,          (uint8_t *)&param); 
//    HAL_Delay(20);
    param = 1; /*default motion timeout 3 seconds*/
    USER_PARAM_Set(BLK0_setupMotionTimeout,             (uint8_t *)&param); 
//    HAL_Delay(20);
    /*F1.9 Log or Print*/
    param = 0; 
    USER_PARAM_Set(BLK0_setupPrintInterLock,            (uint8_t *)&param); //disable
//     HAL_Delay(20);
    param = 8; 
    USER_PARAM_Set(BLK0_setupFilterPols,                 (uint8_t *)&param); //disable
//     HAL_Delay(20);
    USER_PARAM_Set(BLK0_setupPrintMotionCheck,          (uint8_t *)&param); //disable
//     HAL_Delay(20);
    param = 1; 
    USER_PARAM_Set(BLK0_setupResetOn,                   (uint8_t *)&param); //reset on return mode
//    HAL_Delay(20);
    dtmep = 0.0; 
    USER_PARAM_Set(BLK0_setupMinimumWeight,             (uint8_t *)&dtmep); 
//    HAL_Delay(20);
    USER_PARAM_Set(BLK0_setupResetOnReturnWeight,       (uint8_t *)&dtmep); 
//    HAL_Delay(20);
    USER_PARAM_Set(BLK0_setupResetOnDeviationWeight,    (uint8_t *)&dtmep); 
//     HAL_Delay(20);
    USER_PARAM_Set(BLK0_setupThresholdWeight,           (uint8_t *)&dtmep); 
//    HAL_Delay(20);
    //		dtmep = 0.0; 
    //        accessBRAMParameters(CURRENTTAREWEIGHTADDRESS, (uint8_t *)&dtmep, 4, WRITEOPERATION); 
    TARE *this = (g_ScaleData.tare); 
    TARE_SetTareSource(this, NO_TARE); //2009-4-13 9:24 PUSHBUTTON_TARE
    TARE_SetTareMode(this, 'G'); //2009-3-24 10:40
	
}


void reInitializeScaleParameters(SCALE *Pscale, int initMode)
{
	static bool bReInit = false; 
	UNIT_tType oldUnitType; 
	double gravityCorrection; 
	double incrArray[3]; 
	double capacity; 
	uint8_t unit; 

	if (initMode == CAL_INITZERO || initMode == CAL_INITSPAN)
	{
		ZERO_SetReCalibratedZeroCounts((Pscale->zero), Pscale->zeroCalCounts); 
		Pscale->tare->tareMode = 'G'; //2009-4-13 9:18  lxw TARE_SetTareMode(&(this->tare), 'G'); 
		Pscale->tare->fineTareWeight = 0.0; 
		Pscale->currentRange = 0; 
	    // xht remove		
//		if (boolZeroCountsChange == true)
//		{
			TARE_SetTareMode((Pscale->tare), 'G'); //2009-4-13 9:18  lxw  if zero cal has been done the tareweight would be canceled
			Pscale->zero->currentZeroCounts = Pscale->zeroCalCounts; 	//2009-4-7 8:09  to change the zeroCalCounts as currentzerocounts
//			accessBRAMParameters(CURRENTZEROCOUNTSADDRESS, (uint8_t *)&(this->zero.currentZeroCounts), 4, WRITEOPERATION); 
//	   	}
	}
	if ((initMode == EXIT_SETUP))//2009-3-11 15:41 lxw change the current zero counts meanwhile&& (boolZeroCountsChange == true)
	{
		USER_PARAM_Get(BLK0_zeroCalCounts, (uint8_t *)&(Pscale->zeroCalCounts)); 
	 	Pscale->zero->currentZeroCounts = Pscale->zeroCalCounts; 	
//	 	accessBRAMParameters(CURRENTZEROCOUNTSADDRESS, (uint8_t *)&(this->zero.currentZeroCounts), 4, WRITEOPERATION); 
	}
	// to do, calFree init
  
	/*Initialize members of SCALE*/
//	this->scaleType = 'A'; 
	USER_PARAM_Get(BLK0_setupScaleName, (uint8_t *)Pscale->setupScaleName); 
	USER_PARAM_Get(BLK0_setupCertificateNo, (uint8_t *)Pscale->setupCertificateNo); 

	USER_PARAM_Get(BLK0_setupApproval, (uint8_t *)&Pscale->market); 
	if (Pscale->market)
		Pscale->bLegalForTrade = true; 
	else
		Pscale->bLegalForTrade = false; 
	
	/*F1.2 Capacity and Increment*/
	USER_PARAM_Get(BLK0_setupBlankoverCapacity, (uint8_t *)(&Pscale->overCapDivisions)); 

	Pscale->expandDisplayCycles = 255; 
  
	getCalParams(Pscale); //, initMode
	gravityCorrection = geoTable[Pscale->usrGeo] / geoTable[Pscale->calGeo]; 
  
	
	USER_PARAM_Get(BLK0_setupRanges, (uint8_t *)(&Pscale->numberRanges)); 
	if (Pscale->numberRanges > ONE_RANGE)
	{
      	USER_PARAM_Get(BLK0_setupRangeTwoCapacity, (uint8_t *)&capacity); 
      	Pscale->scaleCapacity = capacity; 
      	Pscale->setupCapacityValue = capacity; 
//      	USER_PARAM_Get(BLK0_setupRangeTwoCapacityString, (uint8_t *)scaleCapacityTwoStr); 
      	USER_PARAM_Get(BLK0_setupRangeOneCapacity, (uint8_t *)&capacity); 
      	Pscale->lowHighThreshold = capacity; 
//      	USER_PARAM_Get(BLK0_setupRangeOneCapacityString, (uint8_t *)scaleCapacityOneStr); 
		USER_PARAM_Get(BLK0_setupRangeTwoIncrement, (uint8_t *)&Pscale->setupRangeTwoIncrement); 
//		Pscale->highInc = incrTable[Pscale->setupRangeTwoIncrement]; 
        Pscale->highInc = Pscale->setupRangeTwoIncrement; 
		USER_PARAM_Get(BLK0_setupRangeOneIncrement, (uint8_t *)&Pscale->setupRangeOneIncrement); 
//		Pscale->lowInc = incrTable[Pscale->setupRangeOneIncrement]; 
        Pscale->lowInc = Pscale->setupRangeOneIncrement; 
    }
	else
	{
		USER_PARAM_Get(BLK0_setupRangeOneCapacity, (uint8_t *)&(Pscale->setupCapacityValue)); 
		Pscale->scaleCapacity =  Pscale->setupCapacityValue; 
		Pscale->lowHighThreshold = Pscale->setupCapacityValue; //lxw 2010.06.03 for the Tare in One Range
//		USER_PARAM_Get(BLK0_setupRangeOneCapacityString, (uint8_t *)scaleCapacityOneStr); 
//		USER_PARAM_Get(BLK0_setupRangeTwoCapacityString, (uint8_t *)scaleCapacityTwoStr); 
		USER_PARAM_Get(BLK0_setupRangeOneIncrement, (uint8_t *)&Pscale->setupRangeOneIncrement); 
		USER_PARAM_Get(BLK0_setupRangeTwoIncrement, (uint8_t *)&Pscale->setupRangeTwoIncrement);
//		Pscale->lowInc = incrTable[Pscale->setupRangeOneIncrement]; 
        Pscale->lowInc = Pscale->setupRangeOneIncrement; 
		Pscale->highInc = Pscale->lowInc; 
	}
	Pscale->currInc = Pscale->lowInc; 

  // init UNIT data members
	USER_PARAM_Get(BLK0_setupPrimaryUnit, &unit); 
	Pscale->unit->priUnits = (UNIT_tSymbol)unit; 
	USER_PARAM_Get(BLK0_setupSecondUnit, &unit); 
	Pscale->unit->secUnits = (UNIT_tSymbol)unit; 
	
	// there is no custom unit
//	this->unit.customUnitFactor = 1.0; 
	Pscale->unit->calUnitType = PRIME_UNIT; 
	
//	accessBRAMParameters(CURRENTUNITADDRESS, (uint8_t *)&(this->unit.currUnitType), 1, READOPERATION); 
	oldUnitType = Pscale->unit->currUnitType; 
	if ((Pscale->unit->currUnitType == SECOND_UNIT) && (Pscale->unit->secUnits == UNIT_none))
		Pscale->unit->currUnitType = PRIME_UNIT; 


	/*calculate span and linear factors*/
	SCALE_CalcSpanFactor(Pscale); 
	SCALE_CalcLinearFactor(Pscale); 

	Pscale->countsPerCalUnit[0] *= gravityCorrection; 
	
	Pscale->capacityCounts = (int32_t)(Pscale->scaleCapacity * Pscale->countsPerCalUnit[0]); 

	SCALE_CalculateDivisinon(Pscale); 

	// Get motion range setting
	USER_PARAM_Get(BLK0_setupMotionRange, (uint8_t *)&Pscale->motion->sensitivityInD); 
	USER_PARAM_Get(BLK0_setupnoMotionInterval, (uint8_t *)(&Pscale->motion->sensitivityInterval)); 
	MOTION_Init((Pscale->motion)); 
	/*convert time period from tenths of seconds to # readings*/
	MOTION_SetStabilityTimePeriod((Pscale->motion), (int)(CONFIG_WEIGHT_CYCLES_PER_SEC*0.1*noMotionInterval[Pscale->motion->sensitivityInterval])); 
	if (initMode == EXIT_SETUP)
		MOTION_Calibrate((Pscale->motion), Pscale->oneD[Pscale->currentRange]); 
	else
		MOTION_Calibrate((Pscale->motion), Pscale->oneD[0]); 

	/*F1.4 Zero*/
	Pscale->zero->calibratedZeroCounts = Pscale->zeroCalCounts; 
	USER_PARAM_Get(BLK0_setupPowerupZero, (uint8_t *)&Pscale->zero->powerupZero); 
	if (Pscale->zero->powerupZero == POWERUP_ZERO_DISABLE)
	{
		/*Fix powerup zero to restart*/
		Pscale->zero->powerUpZeroSet = 0; //1; 2009-3-11 10:54  lxw
//		accessBRAMParameters(CURRENTZEROCOUNTSADDRESS, (uint8_t *)&(Pscale->zero.currentZeroCounts), 4, READOPERATION); 
        USER_PARAM_Get(BLK0_zeroCalCounts, (uint8_t *)(&Pscale->zero->currentZeroCounts)); 
        
        if (Pscale->zero->currentZeroCounts < -999999)
		  	ZERO_SetCurrentZero((Pscale->zero), Pscale->zero->calibratedZeroCounts); 
		 
		Pscale->tare->onPowerUpTare = 0; 
	}
	else
	{
		Pscale->zero->powerUpZeroSet = 1; 
		if (bReInit)
		{
//			accessBRAMParameters(CURRENTZEROCOUNTSADDRESS, (uint8_t *)&(Pscale->zero.currentZeroCounts), 4, READOPERATION); 
          USER_PARAM_Get(BLK0_zeroCalCounts, (uint8_t *)(&Pscale->zero->currentZeroCounts)); 
			ZERO_SetCurrentZero((Pscale->zero), Pscale->zero->currentZeroCounts); 
		}
		else
			ZERO_SetCurrentZero((Pscale->zero), Pscale->zero->calibratedZeroCounts); 		
		Pscale->tare->onPowerUpTare = 1; //1; 2009-2-26 11:29 lxw  //=0:restart with current tare, 1: reset the tare to zero on power up
	}
	USER_PARAM_Get(BLK0_setupPushButtonZero, (uint8_t *)&Pscale->zero->pushbuttonZero); 
	if (!((initMode == CAL_INITZERO || initMode == CAL_INITSPAN)))
	    USER_PARAM_Get(BLK0_setupAutoZero, (uint8_t *)&Pscale->zero->enableAZM); 
	    
	USER_PARAM_Get(BLK0_setupAutoZeroRange, (uint8_t *)(&Pscale->zero->autoZeroWindowDivisions)); 
//	USER_PARAM_Get(BLK0_setupUnderZeroBlanking, (uint8_t *)(&this->zero.underZeroDivisions)); 
    USER_PARAM_Get(BLK0_setupUnderZeroBlanking, (uint8_t *)(&Pscale->zero->underZeroBlanking)); 
    Pscale->zero->underzeroWait = false; 

	//0--restart whith current zero, 1--reset to calibrated zero
	// Timeout value for zero, tare and print will be 3 seconds
	USER_PARAM_Get(BLK0_setupMotionTimeout, (uint8_t *)&(Pscale->zero->zeroMotionWait)); 
	Pscale->tare->tareMotionWait = Pscale->zero->zeroMotionWait; 
	Pscale->printMotionWait     = Pscale->zero->zeroMotionWait; 
	
	Pscale->zero->bZeroWait = 0; 
	
	if (Pscale->zero->zeroStatus & POWERUP_ZERO_CAPTURED)  // exit setup mode
	{
		Pscale->zero->zeroStatus = POWERUP_ZERO_CAPTURED; 
	}
	else
	{
		
		ZERO_SetCalibratedZeroCounts((Pscale->zero), Pscale->zeroCalCounts); 
	}

	SCALE_GetIncrementArray(Pscale, &incrArray[0]); // Get increments array of all ranges
	ZERO_Calibrate((Pscale->zero), Pscale->oneD, &incrArray[0], Pscale->capacityCounts); 
	
	// init tare parameters
//	this->tare.tareEnabled = 1; 
	USER_PARAM_Get(BLK0_setupPushButtonTare, &(Pscale->tare->pushbuttonTareEnabled)); 
	USER_PARAM_Get(BLK0_setupKeyboardTare, &(Pscale->tare->keyboardTareEnabled)); 
	USER_PARAM_Get(BLK0_setupNetSignCorrection, &(Pscale->tare->netSignCorrection)); 
	
	/*initialize autotare parameters*/
	USER_PARAM_Get(BLK0_setupAutoTare, &(Pscale->tare->autoTareEnabled)); 
	USER_PARAM_Get(BLK0_setupAutoTareThreshold, (uint8_t *)&(Pscale->tare->primeAutoTareThresholdWeight)); 
	USER_PARAM_Get(BLK0_setupAutoTareResetThreshold, (uint8_t *)&(Pscale->tare->primeAutoTareResetThresholdWeight)); 
	USER_PARAM_Get(BLK0_setupAutoTareMotionCheck, &(Pscale->tare->autoTareCheckMotion)); 
	
	/*initialize autocleartare parameters*/
	USER_PARAM_Get(BLK0_setupAutoClearTare, &(Pscale->tare->autoClearTareThresholdEnabled)); 
	USER_PARAM_Get(BLK0_setupClearafterPrint, (uint8_t *)&(Pscale->tare->autoClearTareAfterPrintEnabled)); 
	USER_PARAM_Get(BLK0_setupAutoClearTareThreshold, (uint8_t *)&(Pscale->tare->primeAutoClearTareThresholdWeight)); 
	USER_PARAM_Get(BLK0_setupAutoClearTareMotionCheck, &(Pscale->tare->autoClearTareCheckMotion)); 

	if (!bReInit || initMode == CAL_INITZERO || initMode == CAL_INITSPAN || ((initMode == EXIT_SETUP) )) //&& (boolZeroCountsChange == true)
	{
		Pscale->tare->tareMode = 'G'; 
		Pscale->tare->fineTareWeight   = 0.0; 
		Pscale->tare->fineStoredWeight = 0.0; 
		Pscale->tare->tareSource = NO_TARE; 
		Pscale->tare->tareTakenFlag      = 0; 
		Pscale->tare->tareChangedFlag    = 5; 
		Pscale->tare->autoClearTareArmed = 0; 
		if ((initMode == EXIT_SETUP) )//&& boolZeroCountsChange
		{
//			boolZeroCountsChange = false; 
			Pscale->currentTareWeight = Pscale->tare->fineTareWeight; 
//			accessBRAMParameters(CURRENTTAREWEIGHTADDRESS, (uint8_t *)&(Pscale->currentTareWeight), 4, WRITEOPERATION); 
//			accessBRAMParameters(CURRENTTAREMODEADDRESS,   (uint8_t *)&(Pscale->tare.tareMode),     1, WRITEOPERATION); 
//			accessBRAMParameters(CURRENTTARESOURCEADDRESS, (uint8_t *)&(Pscale->tare.tareSource),   1, WRITEOPERATION); 
		}
	}
	if (Pscale->tare->onPowerUpTare == 0)
	{
		/*to restart  the last tareweight   2009-3-3 11:01 lxw*/
//		accessBRAMParameters(CURRENTTAREWEIGHTADDRESS, (uint8_t *)&(Pscale->currentTareWeight), 4, READOPERATION); 
		Pscale->tare->fineTareWeight = Pscale->currentTareWeight; 
//		accessBRAMParameters(CURRENTTAREMODEADDRESS,   (uint8_t *)&(Pscale->tare.tareMode),   1, READOPERATION); 
//		accessBRAMParameters(CURRENTTARESOURCEADDRESS, (uint8_t *)&(Pscale->tare.tareSource), 1, READOPERATION); 
		TARE_SetTareSource((Pscale->tare), Pscale->tare->tareSource); 

	}
	
	Pscale->tare->bTareWait = 0; 

	if (Pscale->tare->tareMode == 'G')
		Pscale->tare->fineTareWeight = 0.0; 

	TARE_Calibrate((Pscale->tare), oldUnitType, Pscale->unit->currUnitType, &incrArray[0]); 
  


  // init print timeout parameters
	Pscale->bPrintWait = 0; 
    Pscale->demandPrintSource = PRINTSOURCE_COM0; 
  // set ReInitialization flag
	bReInit = true; 
}
