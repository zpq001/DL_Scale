#ifndef _USER_PARAM_H
#define _USER_PARAM_H

//#include "RB_Type.h"
#include "comm.h"

#define MAX_BLOCK_NUM       8
#define MAX_APP_PARAM_SIZE  1024  //256   --21-nov-08 920
#define MAX_MFG_PARAM_SIZE  64

#define EE_APP_BASE         0
#define EE_BAK_BASE         0
#define EE_MFG_BASE         1024
//<<<<<<< UserParam.h
//#define EE_MFG_BASE         256
//
/*
=======
//#define EE_MFG_BASE         0
//>>>>>>> 1.5

//CAPACITY_INPUT MINUS_TOL_INPUT PLUS_TOL_INPUT SPILL_INPUT FINEFEED_INPUT
*/
//============================================================
// 		define data type
//============================================================
#define        DT_nn      0
#define        DT_Bl      DT_nn+1        /* Boolean field, 0 or 1 */
#define        DT_By      DT_nn+2        /* One byte integer */
#define        DT_US      DT_nn+3        /* Two byte unsigned integer */
#define        DT_UL      DT_nn+4        /* Four byte unsigned integer */
#define        DT_F       DT_nn+5        /* Single precision floating point */
#define        DT_D       DT_nn+6        /* Double precision floating point */
#define        DT_ABy     DT_nn+7        /* Array of By */
#define        DT_ABl     DT_nn+8        /* Array of Bl */
#define        DT_S       DT_nn+9        /* Unicode string */
#define        DT_AL      DT_nn+10       /* Array of UL */
#define        DT_Struct  DT_nn+11       /* Entire block */

//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//! Definition of return values
typedef enum 
{
	USER_PARAM_OK				= 0x00,	//!< No Error
	USER_PARAM_ID_NOT_FOUND,			//!< Identifier not found
	USER_PARAM_APP_READ_ERROR,			//!< Error during read of parameter
	USER_PARAM_BAK_READ_ERROR,
	USER_PARAM_BOTH_READ_ERROR,
	USER_PARAM_MFG_READ_ERROR,
	USER_PARAM_APP_WRITE_ERROR,     //!< Error during write of parameter
	USER_PARAM_BAK_WRITE_ERROR,
	USER_PARAM_BOTH_WRITE_ERROR,
	USER_PARAM_MFG_WRITE_ERROR,
	USER_PARAM_TYPE_ERROR,			//!< Error in type definition of parameter
	USER_PARAM_SIZE_ERROR, 			//!< Size of parameter is zero or odd
	USER_PARAM_TOO_MUCH_BLOCK,
	USER_PARAM_APP_BLOCK_CHECKSUM_ERROR,
	USER_PARAM_BAK_BLOCK_CHECKSUM_ERROR,
	USER_PARAM_BOTH_BLOCK_CHECKSUM_ERROR,
	USER_PARAM_MFG_BLOCK_CHECKSUM_ERROR
} USER_PARAM_tStatus;

typedef enum 
{
	STORAGE_APP,
	STORAGE_MFG
} USER_PARAM_tStorageType;

typedef struct 
{
  uint16_t paramId;
  uint16_t dataSize;
} USER_PARAM_tTableEntry;

typedef struct 
{
  uint16_t startIndex;
  uint16_t endIndex;
  uint16_t blockOffset;
  uint16_t blockSize;
} USER_PARAM_tBlockInfo;

//! Parameter Id's, which are used in getting or setting the parameters
typedef enum 
{
	BLOCK0 = 0,
	BLK0_MACAddress,
	//BLK0_setupSerialNumber,	
	BLK0_boardInfoReserved,
	/*F1.1 Approval*/
	BLK0_setupScaleName,
	BLK0_setupApproval,
	BLK0_setupCertificateNo,
	/*F1.2 Capacity*/
	BLK0_setupPrimaryUnit,
	BLK0_setupRanges,
	BLK0_setupRangeOneCapacity,	
	BLK0_setupRangeOneIncrement,
	BLK0_setupRangeTwoCapacity,	
	BLK0_setupRangeTwoIncrement,
	BLK0_setupBlankoverCapacity,
	/*F1.3 Calibration*/
	BLK0_calGeo,
	BLK0_usrGeo,
	BLK0_setupLinearity,
	BLK0_zeroCalCounts,
	BLK0_highCalWeight,
	BLK0_highCalCounts,
	BLK0_midCalWeight,
	BLK0_midCalCounts,
	BLK0_lowCalWeight,
	BLK0_lowCalCounts,
	BLK0_calibrationDate,
    BLK0_ADJUST_K2,
	/*F1.4 Zero*/
	BLK0_setupAutoZero,
	BLK0_setupAutoZeroRange,
	BLK0_setupUnderZeroBlanking,
	BLK0_setupCenterofZero,
	BLK0_setupPowerupZero,
	BLK0_setupPushButtonZero,
	/*F1.5 Tare*/
	BLK0_setupPushButtonTare,
	BLK0_setupKeyboardTare,
	BLK0_setupNetSignCorrection,
	BLK0_setupAutoTare,
	BLK0_setupAutoTareThreshold,
	BLK0_setupAutoTareResetThreshold,
	BLK0_setupAutoTareMotionCheck,
	BLK0_setupAutoClearTare,
	BLK0_setupClearafterPrint,
	BLK0_setupAutoClearTareThreshold,
	BLK0_setupAutoClearTareMotionCheck,
	BLK0_setupClearTareonZero,
	BLK0_setupTareInterlock,
	/*F1.6 Unit*/
	BLK0_setupSecondUnit,
	/*F1.7 Filter*/
	BLK0_setupLowPassFilter,
	BLK0_setupStabilityFilter,
	/*F1.8 Motion*/
	BLK0_setupMotionRange,
	BLK0_setupnoMotionInterval,
	BLK0_setupMotionTimeout,
	
	/*F1.9 Log or Print*/
	BLK0_setupMinimumWeight,
	BLK0_setupPrintInterLock,
	BLK0_setupFilterPols,
	BLK0_setupResetOn,
	BLK0_setupResetOnReturnWeight,
	BLK0_setupResetOnDeviationWeight,
	BLK0_setupThresholdWeight,
	BLK0_setupPrintMotionCheck,
	
	BLK0_upScaleTestPoint,
  
	BLK0_cellCapacity,
	BLK0_cellCapUnit,
	BLK0_preload,
	BLK0_preloadUnit,
	BLK0_checksum = 99,
  
	BLOCK1 = 100, 




	/*connections*/
	//COM1
	BLK1_setupCOM1Assignment,
	BLK1_setupCOM1Template,
	BLK1_setupCOM1AssignmentChecksum,
	BLK1_setupCOM1Assignment2,
	BLK1_setupCOM1Template2,
	BLK1_setupCOM1Assignment3,
	BLK1_setupCOM1Template3,
	//COM2
	BLK1_setupCOM2Assignment,
	BLK1_setupCOM2Template,
	BLK1_setupCOM2AssignmentChecksum,
	BLK1_setupCOM2Assignment2,
	BLK1_setupCOM2Template2,
	BLK1_setupCOM2Assignment3,
	BLK1_setupCOM2Template3,
	//Ethernet
	BLK1_setupEthernetAssignment,	
	BLK1_setupEthernetTemplate,	
	BLK1_setupEthernetAssignment2,	
	BLK1_setupEthernetTemplate2,	
	BLK1_setupEthernetAssignment3,	
	BLK1_setupEthernetTemplate3,	

	BLK1_setupEthernetPrintClientAssignment, 
	BLK1_setupEthernetPrintClientTemplate,	
	BLK1_setupEthernetPrintClientAssignment2, 
	BLK1_setupEthernetPrintClientTemplate2,	
	BLK1_setupEthernetPrintClientAssignment3, 
	BLK1_setupEthernetPrintClientTemplate3,	
	BLK1_setupEthernetPrintClientChecksum, 
	BLK1_setupEthernetChecksum,

	/*serial*/
	//COM1
	BLK1_setupCOM1BaudRate,
	BLK1_setupCOM1DateBits,
	BLK1_setupCOM1Parity,
	BLK1_setupCOM1FlowControl,
	//COM2
	BLK1_setupCOM2BaudRate,
	BLK1_setupCOM2DateBits,
	BLK1_setupCOM2Parity,
	BLK1_setupCOM2FlowControl,
	BLK1_setupCOM2InterFace,
	BLK1_setupCOM2Address,
	/*Network*/
    //Ethernet
    BLK1_setupIPDHCPClient, 
	BLK1_setupIPAddressSeg1,
	BLK1_setupIPAddressSeg2,
	BLK1_setupIPAddressSeg3,
	BLK1_setupIPAddressSeg4,  
	BLK1_setupSubnetMaskSeg1,
	BLK1_setupSubnetMaskSeg2,
	BLK1_setupSubnetMaskSeg3,
	BLK1_setupSubnetMaskSeg4,
	BLK1_setupGateway1,
	BLK1_setupGateway2,
	BLK1_setupGateway3,
	BLK1_setupGateway4,
	//Print Client
	BLK1_setupServerIPAddressSeg1,
  	BLK1_setupServerIPAddressSeg2,
  	BLK1_setupServerIPAddressSeg3,
  	BLK1_setupServerIPAddressSeg4,
	BLK1_setupServerPort,
//	//WiFi
	BLK1_setupWIFISSIDName,          
	BLK1_setupWIFIWST0,                     
	BLK1_setupWIFIWLPP,
	BLK1_setupWIFIWLKI,
	BLK1_setupWIFIReset,
	BLK1_checksum = 199,
    
    BLOCK2 = 200,
	BLK2_applicationType,
	BLK2_setupOutputTemplateFormat,
	BLK2_setupOutputTemplateScaleName,
	BLK2_setupScreenSave,     
	BLK2_setupBackLightTimeOut,    
	BLK2_setupAutoOffTimer, 
	BLK2_setupSystemline,
	BLK2_setupTareDisplay,

	BLK2_setupTimeFormat,     
	BLK2_setupDateFormat,    
	BLK2_setupDateFieldSeparator, 
//	BLK2_setupDisplayTD, 	
  
	BLK2_setupMenuLanguage, 
	BLK2_setupSetupLanguage,
  
	BLK2_setupTransactionCounter, 
	BLK2_setupEditCounter, 
  
	BLK2_setupProtection, 
	BLK2_setupAdminPassword,  
  
	BLK2_setupAlibiMemoryAccess, 
	BLK2_setupContrastAdjustAccess, 
	BLK2_setupTransCounterAccess, 
	BLK2_setupTimeDateAccess,
	BLK2_setupTotalMemoryAccess,
	BLK2_setupExpandAccess,
	BLK2_setupIDTotalAccess,
	BLK2_setupSerialTest,
    BLK2_KEYTIMEOUT,
    BLK2_setuppoweroffcontrol,
    BLK2_setupBlueToothReset,

	
	//Bluetooth connection
	BLK2_setupBTAssignment,
	BLK2_setupBTTemplate,
	BLK2_setupBTAssignmentChecksum,
	BLK2_setupBTAssignment2,
	BLK2_setupBTTemplate2,
	BLK2_setupBTAssignment3,
	BLK2_setupBTTemplate3,

	
	//	//Bluetooth
	BLK2_setupBlueToothName,
	BLK2_setupBlueToothPIN,
	BLK2_setupBlueToothUARTBAUDRATE,
	BLK2_setupBlueToothUARTSTOP,
	BLK2_setupBlueToothUARTPARITY,

	//barcode
	BLK2_setupLabelPrintBarcodePrefixlen,
	BLK2_setupLabelPrintBarcodeSuffixlen,
	BLK2_setupLabelPrintBarcodeDatalen,
	BLK2_setupLabelPrintBarcodeEndchar,
	//printlanguage
	BLK2_setupPrintLanguage,
	BLK2_setupTFTPserverIP,
	BLK2_setupAlibiTransactionPW,
    BLK2_checksum = 299,
    
	BLOCK3 = 300,
	BLK3_AlibiMemoryEnable,
	BLK3_setupIDExpand,
	BLK3_setupTotalizationMode,
	BLK3_setupTotalizationClearGT,
	BLK3_setupTotalizationSubTotal,
	BLK3_setupTotalizationClearST,
	BLK3_setupTotalizationConvertWeight,	
	
	BLK3_setupInput1Polarity,
	BLK3_setupInput1Assignment,  
	BLK3_setupInput2Polarity,
	BLK3_setupInput2Assignment,
	BLK3_setupOutput1Assignment,
	BLK3_setupOutput2Assignment,
	BLK3_setupOutput3Assignment,
	BLK3_setupOutput4Assignment,
	
	BLK3_setupFunctionkeyAssignment,
	BLK3_setupFunctionAutoStart,
	BLK3_setupAnimalWeighingOperationMode,
	BLK3_setupAnimalWeighingSamplingTime,
	BLK3_setupAnimalWeighingAutoStart,
	BLK3_setupAnimalWeighingdisplayLine2, 
	BLK3_setupAnimalWeighingStartThreshold,
	BLK3_setupAnimalWeighingAutoPrint,
	BLK3_setupAnimalWeighingPrintDelay,
	
	BLK3_setupOverUnderOperationSource,
	BLK3_setupOverUnderOperationTolType,	
	BLK3_setupOverUnderOperationTargetEdit,
	BLK3_setupOverUnderOperationHoldTimer,
	BLK3_setupOverUnderOperationMotionCheck,
	BLK3_setupOverUnderOperationAutoPrint,
	
	BLK3_setupOverUnderDisplayMode,	
	BLK3_setupOverUnderDisplayLine,
	BLK3_setupOverUnderDisplaySmarttrack,
	BLK3_setupOverUnderDisplayMotionblanking,		
	BLK3_setupOverUnderTargetTable,
	BLK3_setupOverUnderTotalizationTotalization,
	BLK3_setupOverUnderTotalizationCleartotals,	
	
	BLK3_setupOverUnderMenukeysActivetarget,
	BLK3_setupOverUnderMenukeysQuicksettarget,
	BLK3_setupOverUnderMenukeysTargettable,
	
	BLK3_setupOverUnderTargetValues,
	BLK3_setupOverunderAvtiveUnit,	
	BLK3_setupOverUnderTargetMinusTol,
	BLK3_setupOverUnderTargetPlusTol,
	BLK3_setupOverUnderTargetUnderLimit,
	BLK3_setupOverUnderTargetOverLimit,	
	BLK3_setupOverUnderTargetDiscription, 
	BLK3_setupOverUnderRecordIndex, 
	
	BLK3_setupPeakWeighingOperationMode,	
	BLK3_setupPeakWeighingUseMemory,
	BLK3_setupPeakWeighingAutoPrint,
	BLK3_setupPeakWeighingDispLine1,
	BLK3_setupPeakWeighingDispLine2,
	BLK3_setupPeakWeighingReports,
	BLK3_setupPeakWeighingHoldTimer, 
	/*Dynamic/animal Weighing*/
	BLK3_setupDynamicWeighingTimeInterval,
	BLK3_setupDynamicWeighingAccumulation,
	BLK3_setupDynamicWeighingAutoMemClear,
	BLK3_setupDynamicWeighingAutoPrint,
	BLK3_setupDynamicWeighingDispLine,
	/*Counting*/
	BLK3_setupCountingOperationPrompt,
	BLK3_setupCountingOperationOptimize,
	BLK3_setupCountingOperationAutoClearAPW,
	BLK3_setupCountingOperationTARGETPCS,
	BLK3_setupCountingDisplayLine1,
	BLK3_setupCountingDisplayLine2,
	BLK3_setupCountingMenuKeysIDTable,
	BLK3_setupCountingMenuKeysReports,
	BLK3_setupCountingMenuKeysSampleAPWSelect,
	BLK3_setupCountingIDMemoryIDTable,
	BLK3_setupCountingIDMemoryTotalization,
	BLK3_setupCountingIDMemoryClearOnPrint,
	BLK3_setupCountingIDMemoryUpdateAPW, 
		
	BLK3_setupCountingWeighingAutoRefOpti,
	BLK3_setupCountingWeighingAPWEntry,
	BLK3_setupCountingWeighingAccumulate,
	BLK3_setupCountingWeighingLastUsedID,
	BLK3_setupCountingWeighingIDLKUPTAB0,
	BLK3_setupCountingWeighingIDLKUPTAB1,
	BLK3_setupCountingWeighingIDLKUPTAB2,
	BLK3_setupCountingWeighingIDLKUPTAB3,
	BLK3_setupCountingWeighingIDLKUPTAB4,
	BLK3_setupCountingWeighingIDLKUPTAB5,
	BLK3_setupCountingWeighingIDLKUPTAB6,
	BLK3_setupVehicleOperationTemporaryID,
	BLK3_setupVehicleOperationAutoID,
	BLK3_setupVehicleOperationOperatorClearing,
	BLK3_setupVehicleOperationPermanentID,
	BLK3_setupVehicleOperationDescription,
	BLK3_setupVehicleOperationTotalization,
	BLK3_setupVehicleOperationClearTotals,
	BLK3_setupVehicleGeneralTempPrompt,
	BLK3_setupVehicleGeneralPermPrompt,
	BLK3_setupVehicleGeneralThresholdWt,
    BLK3_setupVehicleGeneralResetWt,
    BLK3_setupVehicleGeneralTransactionTable,
    BLK3_SERVICENUMBER,
	BLK3_checksum = 399,
  
	BLOCK4 = 400,
	BLK4_zeroCounts2mv,
	BLK4_spanCounts2mv,
//	BLK4_startupcounter, 
	BLK4_checksum = 499,
	
	BLOCK5 = 500,
	BLK5_sdWasPresent,
	BLK5_setupMaintenanceRestorefromSDCard,
	BLK5_checksum = 599
} USER_PARAM_tIdent;


//! WARNING C-Syntax: NO character is allowed after the backspace '\' !!!  toleranceType
//! Parameter definitions, which are used in the parameter table
#define USER_PARAM_DEFINITIONS						\
	/* paramId              dataSize */          	\
	/* Scale block start */                      	\
	{BLK0_MACAddress,                       12},    \
	/*{BLK0_setupSerialNumber,				16},	*/\
	{BLK0_boardInfoReserved,                36},    \
	/*F1.1*/										\
	{BLK0_setupScaleName,					21},	\
	{BLK0_setupApproval,					1},		\
	{BLK0_setupCertificateNo,				21},	\
	/*F1.2 Capacity and Increment*/						\
	{BLK0_setupPrimaryUnit,					1},		\
	{BLK0_setupRanges,						1},		\
	{BLK0_setupRangeOneCapacity,			8},		\
	{BLK0_setupRangeOneIncrement,			8},		\
	{BLK0_setupRangeTwoCapacity,			8},		\
	{BLK0_setupRangeTwoIncrement,			8},		\
	{BLK0_setupBlankoverCapacity,			1},		\
	/*F1.3*/											\
	{BLK0_calGeo,							1},		\
	{BLK0_usrGeo,							1},		\
	{BLK0_setupLinearity,					1},		\
	{BLK0_zeroCalCounts,					4},		\
	{BLK0_highCalWeight,					8},		\
	{BLK0_highCalCounts,					4},		\
	{BLK0_midCalWeight,						8},		\
	{BLK0_midCalCounts,						4},		\
	{BLK0_lowCalWeight,						8},		\
	{BLK0_lowCalCounts,						4},		\
	{BLK0_calibrationDate,                  12},    \
    {BLK0_ADJUST_K2,                        8},    \
	/*F1.4 Zero*/										\
	{BLK0_setupAutoZero,					1},		\
	{BLK0_setupAutoZeroRange,				1},		\
	{BLK0_setupUnderZeroBlanking,			1},		\
	{BLK0_setupCenterofZero,				1},		\
	{BLK0_setupPowerupZero,					1},		\
	{BLK0_setupPushButtonZero,				1},		\
	/*F1.5 Tare*/										\
	{BLK0_setupPushButtonTare,				1},		\
	{BLK0_setupKeyboardTare,				1},		\
	{BLK0_setupNetSignCorrection,			1},		\
	{BLK0_setupAutoTare,					1},		\
	{BLK0_setupAutoTareThreshold,			8},		\
	{BLK0_setupAutoTareResetThreshold,		8},		\
	{BLK0_setupAutoTareMotionCheck,			1},		\
	{BLK0_setupAutoClearTare,           	1},		\
	{BLK0_setupClearafterPrint,           	1},		\
	{BLK0_setupAutoClearTareThreshold,		8},		\
	{BLK0_setupAutoClearTareMotionCheck,	1},		\
	{BLK0_setupClearTareonZero,				1},		\
	{BLK0_setupTareInterlock,				1},		\
	/*F1.6 Second Unit*/								\
	{BLK0_setupSecondUnit,					1},		\
	 /*F1.7 Filter*/									\
	{BLK0_setupLowPassFilter,				8},		\
	{BLK0_setupStabilityFilter,				1},		\
	/*F1.8 Motion and Stability*/						\
	{BLK0_setupMotionRange,					1},		\
	{BLK0_setupnoMotionInterval,			1},		\
	{BLK0_setupMotionTimeout,				1},		\
	/*F1.9 Log or Print*/								\
	{BLK0_setupMinimumWeight,				8},		\
	{BLK0_setupPrintInterLock,				1},		\
	{BLK0_setupFilterPols,					1},		\
	{BLK0_setupResetOn,						1},		\
	{BLK0_setupResetOnReturnWeight,			8},		\
	{BLK0_setupResetOnDeviationWeight,		8},		\
	{BLK0_setupThresholdWeight,				8},		\
	{BLK0_setupPrintMotionCheck,			1},		\
	{BLK0_upScaleTestPoint,					1},		\
	{BLK0_cellCapacity,						8},		\
	{BLK0_cellCapUnit,						8},		\
	{BLK0_preload,							8},		\
	{BLK0_preloadUnit,						4},		\
	{BLK0_checksum,							2},		\
	/*Block 1 start*/								\
	/*--COM1--*/                                    \
	{BLK1_setupCOM1Assignment,       		1},		\
	{BLK1_setupCOM1Template,       			1},		\
	{BLK1_setupCOM1AssignmentChecksum,      1},		\
	{BLK1_setupCOM1Assignment2,             1},		\
	{BLK1_setupCOM1Template2,               1},		\
	{BLK1_setupCOM1Assignment3,             1},		\
	{BLK1_setupCOM1Template3,               1},		\
	/*--COM2--*/                                    \
	{BLK1_setupCOM2Assignment,       		1},		\
	{BLK1_setupCOM2Template,       			1},		\
	{BLK1_setupCOM2AssignmentChecksum,      1},		\
    {BLK1_setupCOM2Assignment2,            1},		\
	{BLK1_setupCOM2Template2,               1},		\
	{BLK1_setupCOM2Assignment3,             1},		\
	{BLK1_setupCOM2Template3,               1},		\
	/*--Ethernet--*/                                \
	{BLK1_setupEthernetAssignment,       	1},		\
	{BLK1_setupEthernetTemplate,       		1},		\
    {BLK1_setupEthernetAssignment2,	        1},		\
	{BLK1_setupEthernetTemplate2,	        1},		\
	{BLK1_setupEthernetAssignment3,	        1},		\
	{BLK1_setupEthernetTemplate3,	        1},		\
	{BLK1_setupEthernetPrintClientAssignment,   1},    \
	{BLK1_setupEthernetPrintClientTemplate,	    1},    \
	{BLK1_setupEthernetPrintClientAssignment2,	1},    \
	{BLK1_setupEthernetPrintClientTemplate2,	1},    \
	{BLK1_setupEthernetPrintClientAssignment3,	1},    \
	{BLK1_setupEthernetPrintClientTemplate3,	1},    \
	{BLK1_setupEthernetPrintClientChecksum,	    1},    \
	{BLK1_setupEthernetChecksum,				1},	\
  	/*--COM1--*/                                    \
	{BLK1_setupCOM1BaudRate,       			1},		\
	{BLK1_setupCOM1DateBits,       			1},		\
	{BLK1_setupCOM1Parity,					1},		\
	{BLK1_setupCOM1FlowControl,				1},		\
	/*--COM2--*/                                    \
	{BLK1_setupCOM2BaudRate,       			1},		\
	{BLK1_setupCOM2DateBits,       			1},		\
	{BLK1_setupCOM2Parity,					1},		\
	{BLK1_setupCOM2FlowControl,				1},		\
	{BLK1_setupCOM2InterFace,				1},		\
	{BLK1_setupCOM2Address,                 1},   \
	/*++++++++++++Network+++++++++++*/              \
	/*--Ethernet--*/                                \
	{BLK1_setupIPDHCPClient,  				1},		\
	{BLK1_setupIPAddressSeg1, 				1},		\
	{BLK1_setupIPAddressSeg2, 				1},		\
	{BLK1_setupIPAddressSeg3, 				1},		\
	{BLK1_setupIPAddressSeg4, 				1},		\
	{BLK1_setupSubnetMaskSeg1,				1},		\
	{BLK1_setupSubnetMaskSeg2,				1},		\
	{BLK1_setupSubnetMaskSeg3,				1},		\
	{BLK1_setupSubnetMaskSeg4,				1},		\
	{BLK1_setupGateway1,      				1},		\
	{BLK1_setupGateway2,      				1},		\
	{BLK1_setupGateway3,      				1},		\
	{BLK1_setupGateway4,      				1},		\
	/*--PrintClient--*/                             \
	{BLK1_setupServerIPAddressSeg1,			1},		\
	{BLK1_setupServerIPAddressSeg2,			1},		\
	{BLK1_setupServerIPAddressSeg3,			1},		\
	{BLK1_setupServerIPAddressSeg4,			1},		\
	{BLK1_setupServerPort,					4},		\
	/*--WiFi--*/                                    \
	{BLK1_setupWIFISSIDName,                21},    \
	{BLK1_setupWIFIWST0,                     1},    \
	{BLK1_setupWIFIWLPP,                    27},    \
    {BLK1_setupWIFIWLKI,                     1},    \
	{BLK1_setupWIFIReset,					 1},	\
	{BLK1_checksum,					        2},		\
	/* Serial block start */						\
    {BLK2_applicationType,					1},		\
    {BLK2_setupOutputTemplateFormat,        1},		\
    {BLK2_setupOutputTemplateScaleName,		1},		\
	{BLK2_setupScreenSave,      	1},		\
	{BLK2_setupBackLightTimeOut,	1},		\
	{BLK2_setupAutoOffTimer,    	1},		\
	{BLK2_setupSystemline,      	1},		\
	{BLK2_setupTareDisplay,      	1},		\
	{BLK2_setupTimeFormat,         	1},		\
	{BLK2_setupDateFormat,         	1},		\
	{BLK2_setupDateFieldSeparator, 	1},		\
	{BLK2_setupMenuLanguage,    	1},		\
	{BLK2_setupSetupLanguage,    	1},		\
	{BLK2_setupTransactionCounter, 	1},		\
	{BLK2_setupEditCounter,         1},		\
  	{BLK2_setupProtection,          1},		\
	{BLK2_setupAdminPassword,     	7},		\
  	{BLK2_setupAlibiMemoryAccess,	1},		\
  	{BLK2_setupContrastAdjustAccess,1},		\
	{BLK2_setupTransCounterAccess,	1},		\
	{BLK2_setupTimeDateAccess,		1},		\
	{BLK2_setupTotalMemoryAccess,   1},		\
	{BLK2_setupExpandAccess,        1},     \
	{BLK2_setupIDTotalAccess,		1},		\
	{BLK2_setupSerialTest,			1},		\
    {BLK2_KEYTIMEOUT,               1},     \
	{BLK2_setuppoweroffcontrol,		1},		\
	{BLK2_setupBlueToothReset,		1},		\
	/*--Bluetooth connection--*/								\
	{BLK2_setupBTAssignment,       		1},		\
	{BLK2_setupBTTemplate,       			1},		\
	{BLK2_setupBTAssignmentChecksum,      1},		\
	{BLK2_setupBTAssignment2,             1},		\
	{BLK2_setupBTTemplate2,               1},		\
	{BLK2_setupBTAssignment3,             1},		\
	{BLK2_setupBTTemplate3,               1},		\
	/*--BlueTooth--*/                              \
    {BLK2_setupBlueToothName,               11},    \
	{BLK2_setupBlueToothPIN,      			11},    \
    {BLK2_setupBlueToothUARTBAUDRATE,		1},		\
	{BLK2_setupBlueToothUARTSTOP,			1},		\
	{BLK2_setupBlueToothUARTPARITY,			1},		\
	{BLK2_setupLabelPrintBarcodePrefixlen,	1},		\
	{BLK2_setupLabelPrintBarcodeSuffixlen,	1},		\
	{BLK2_setupLabelPrintBarcodeDatalen,	1},		\
	{BLK2_setupLabelPrintBarcodeEndchar,	1},		\
	{BLK2_setupPrintLanguage,				1},		\
	{BLK2_setupTFTPserverIP,				4},		\
    {BLK2_setupAlibiTransactionPW,            8},     \
	{BLK2_checksum,                 2},		\
	{BLK3_AlibiMemoryEnable,        1},		\
	{BLK3_setupIDExpand,			1},		\
	{BLK3_setupTotalizationMode,         			1},		\
	{BLK3_setupTotalizationClearGT,           		1},		\
	{BLK3_setupTotalizationSubTotal,         		1},		\
	{BLK3_setupTotalizationClearST,            		1},		\
	{BLK3_setupTotalizationConvertWeight,			1},		\
	{BLK3_setupInput1Polarity,						1},		\
	{BLK3_setupInput1Assignment,      				1},		\
	{BLK3_setupInput2Polarity,       				1},		\
	{BLK3_setupInput2Assignment,       				1},		\
	{BLK3_setupOutput1Assignment,					1},		\
	{BLK3_setupOutput2Assignment,       			1},		\
	{BLK3_setupOutput3Assignment,       			1},		\
	{BLK3_setupOutput4Assignment,       			1},		\
	{BLK3_setupFunctionkeyAssignment,       		1},		\
	{BLK3_setupFunctionAutoStart,                   1},		\
	{BLK3_setupAnimalWeighingOperationMode,         1},     \
	{BLK3_setupAnimalWeighingSamplingTime,          4},     \
	{BLK3_setupAnimalWeighingAutoStart,             1},     \
	{BLK3_setupAnimalWeighingdisplayLine2,          1},     \
	{BLK3_setupAnimalWeighingStartThreshold,        4},     \
	{BLK3_setupAnimalWeighingAutoPrint,             1},     \
	{BLK3_setupAnimalWeighingPrintDelay,            1},     \
	{BLK3_setupOverUnderOperationSource,			1},		\
	{BLK3_setupOverUnderOperationTolType,			1},		\
	{BLK3_setupOverUnderOperationTargetEdit,  		1},		\
	{BLK3_setupOverUnderOperationHoldTimer,  	 	4},		\
	{BLK3_setupOverUnderOperationMotionCheck,		1},		\
	{BLK3_setupOverUnderOperationAutoPrint,			1},		\
	{BLK3_setupOverUnderDisplayMode,				1},		\
	{BLK3_setupOverUnderDisplayLine,				1},		\
	{BLK3_setupOverUnderDisplaySmarttrack,			1},		\
	{BLK3_setupOverUnderDisplayMotionblanking,		1},		\
	{BLK3_setupOverUnderTargetTable,                1},		\
	{BLK3_setupOverUnderTotalizationTotalization,	1},		\
	{BLK3_setupOverUnderTotalizationCleartotals,	1},		\
	{BLK3_setupOverUnderMenukeysActivetarget,		1},		\
	{BLK3_setupOverUnderMenukeysQuicksettarget,	    1},		\
	{BLK3_setupOverUnderMenukeysTargettable,		1},		\
	{BLK3_setupOverUnderTargetValues,	    		4},		\
	{BLK3_setupOverunderAvtiveUnit,                 1},     \
	{BLK3_setupOverUnderTargetMinusTol,    		    4},		\
	{BLK3_setupOverUnderTargetPlusTol,     		    4},		\
	{BLK3_setupOverUnderTargetUnderLimit,    		4},		\
	{BLK3_setupOverUnderTargetOverLimit,     		4},		\
	{BLK3_setupOverUnderTargetDiscription, 		    21},	\
	{BLK3_setupOverUnderRecordIndex,                1},		\
	{BLK3_setupPeakWeighingOperationMode,			1},		\
	{BLK3_setupPeakWeighingUseMemory,				1},		\
	{BLK3_setupPeakWeighingAutoPrint,				1},		\
	{BLK3_setupPeakWeighingDispLine1,      			1},		\
	{BLK3_setupPeakWeighingDispLine2,	  			1},		\
	{BLK3_setupPeakWeighingReports,                 1},     \
	{BLK3_setupPeakWeighingHoldTimer,               4},     \
	{BLK3_setupDynamicWeighingTimeInterval,	        2},		\
	{BLK3_setupDynamicWeighingAccumulation,	        1},		\
	{BLK3_setupDynamicWeighingAutoMemClear,	        1},		\
	{BLK3_setupDynamicWeighingAutoPrint,	        1},		\
	{BLK3_setupDynamicWeighingDispLine,		        1},		\
	{BLK3_setupCountingOperationPrompt,             1},     \
	{BLK3_setupCountingOperationOptimize,           1},     \
	{BLK3_setupCountingOperationAutoClearAPW,       1},     \
	{BLK3_setupCountingOperationTARGETPCS,			4},		\
	{BLK3_setupCountingDisplayLine1,                1},     \
	{BLK3_setupCountingDisplayLine2,                1},     \
	{BLK3_setupCountingMenuKeysIDTable,             1},     \
	{BLK3_setupCountingMenuKeysReports,             1},     \
	{BLK3_setupCountingMenuKeysSampleAPWSelect,     1},     \
	{BLK3_setupCountingIDMemoryIDTable,             1},     \
	{BLK3_setupCountingIDMemoryTotalization,        1},     \
	{BLK3_setupCountingIDMemoryClearOnPrint,        1},     \
    {BLK3_setupCountingIDMemoryUpdateAPW,           1},     \
	{BLK3_setupCountingWeighingAutoRefOpti,	        1},		\
	{BLK3_setupCountingWeighingAPWEntry,	        1},		\
	{BLK3_setupCountingWeighingAccumulate,	        1},		\
	{BLK3_setupCountingWeighingLastUsedID,	        16},	\
	{BLK3_setupCountingWeighingIDLKUPTAB0,	        2},		\
	{BLK3_setupCountingWeighingIDLKUPTAB1,	        2},		\
	{BLK3_setupCountingWeighingIDLKUPTAB2,	        2},		\
	{BLK3_setupCountingWeighingIDLKUPTAB3,	        2},		\
	{BLK3_setupCountingWeighingIDLKUPTAB4,	        2},		\
	{BLK3_setupCountingWeighingIDLKUPTAB5,	        2},		\
	{BLK3_setupCountingWeighingIDLKUPTAB6,	        2},		\
	{BLK3_setupVehicleOperationTemporaryID,         1},     \
	{BLK3_setupVehicleOperationAutoID,              1},     \
	{BLK3_setupVehicleOperationOperatorClearing,    1},     \
	{BLK3_setupVehicleOperationPermanentID,         1},     \
	{BLK3_setupVehicleOperationDescription,         1},     \
	{BLK3_setupVehicleOperationTotalization,        1},     \
	{BLK3_setupVehicleOperationClearTotals,         1},     \
	{BLK3_setupVehicleGeneralTempPrompt,            1},     \
	{BLK3_setupVehicleGeneralPermPrompt,            1},     \
	{BLK3_setupVehicleGeneralThresholdWt,           4},     \
    {BLK3_setupVehicleGeneralResetWt,               4},     \
    {BLK3_setupVehicleGeneralTransactionTable,      1},     \
    {BLK3_SERVICENUMBER,                            21},    \
    {BLK3_checksum,							2},		\
	/* MFG eeprom block start */						\
	{BLK4_zeroCounts2mv, 			 		4},		\
	{BLK4_spanCounts2mv,  					4},		\
	/*{BLK4_startupcounter,                   4},		\*/\
	{BLK4_checksum,       					2},		\
	/* block3 start */                           		\
	{BLK5_sdWasPresent, 					1},		\
	{BLK5_setupMaintenanceRestorefromSDCard,1},     \
	{BLK5_checksum,       					2},		\

//! WARNING C-Syntax: NO character is allowed after the backspace '\' !!!  BLK1_targetSource
//! Storage type definitions for blocks. Note that blocks with STORAGE_BAK type should put ahead of those with STORAGE_APP type consecutively
#define BLOCK_STORAGE_TYPE_DEFINITIONS			                \
        STORAGE_APP,                    \
        STORAGE_APP,                    \
        STORAGE_APP,                    \
        STORAGE_APP,                    \
        STORAGE_MFG,                    \
        STORAGE_MFG,                    \

#define EEPROM_USEDBLOCKS	6
#define MFGPARAPOSITION		(1 << 3)

int USER_PARAM_Initialize(void);
USER_PARAM_tStatus USER_PARAM_Get(USER_PARAM_tIdent ident, uint8_t* param);
USER_PARAM_tStatus USER_PARAM_Set(USER_PARAM_tIdent ident, uint8_t* param);
USER_PARAM_tStatus USER_PARAM_GetBlock(int32_t blockIndex, uint8_t* param);
USER_PARAM_tStatus USER_PARAM_SetBlock(int32_t blockIndex, uint8_t* param);

#endif // _USER_PARAM_H
