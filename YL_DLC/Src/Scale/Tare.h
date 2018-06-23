#ifndef  _TARE_H
#define  _TARE_H

//#include "RB_Type.h"
//#include "RB_String.h"

//#include "IDNet.h"
//#include "Scale.h"

#include "Motion.h"
#include "Zero.h"
#include "Unit.h"


//#define		PUSHBUTTON_TARE	1
//#define 	KEYBOARD_TARE	2
//#define		AUTO_TARE		3


//#define plcPushButtonTare               0x239
//#define plcKeyboardEnteredTare          0x23a
//#define plcClearButton                  0x23b
//#define setupTare						0x23c

typedef enum
{
    NO_TARE = 0,
    PUSHBUTTON_TARE,
    KEYBOARD_TARE,
    AUTO_TARE,
    TARE_MEMORY
} TARE_SOURCE;

typedef enum
{
    NO_SUCCESSIVETARE = 0,
    INCREMENTAL_SUCCESSIVETARE,
    ALL_SUCCESSIVETAREENABLED,
} SUCCESSIVETARE_MODE;

typedef enum
{
	TARE_SUCCESS            				= 0,
	TARE_IN_PROGRESS						= 1,
	SCALE_IN_MOTION_DURING_TARE				= 2,
	PUSHBUTTON_TARE_NOT_ENABLED				= 3,
	PROGRAMMABLE_TARE_NOT_ENABLED			= 4,
	CHAIN_TARE_NOT_PERMITTED				= 5,
	ONLY_INCREMENTAL_CHAIN_TARE_PERMITTED	= 6, 
	TARE_NOT_IN_ROUNDED_INCREMENT_VALUE		= 7,
	TARE_VALUE_TOO_SMALL					= 8,
	TARING_WHEN_POWER_UP_ZERO_NOT_CAPTURED  = 9,
	TARING_OVER_CAPACITY					= 10,	
	TARING_UNDER_ZERO						= 11,
	TARE_VALUE_EXCEEDS_LIMIT				= 12,
	MUST_CLEAR_TARE_AT_GROSS_ZERO			= 13,
	INVALID_TARE_FUNCTION       			= 14    
} TARE_SCALE_tStatus;

struct TareData
{
//	SCALE   *pScale;
    struct ScaleData *pScale;
//    struct ZeroData    *pZero;
//    struct MotionData  *pMotion;
//    struct UnitData    *pUnit;
  
	TARE_SCALE_tStatus tareScaleStatus;
  
	double fineTareWeight;		// tare in current LEGAL_UNITS
	double fineStoredWeight;  	// stored weight
	double roundedStoredWeight;	// for net sign correction.
  // float programmableTareWeight;   // Application can set this value to initiate a programmable tare command, moved to SCALE class
	double primeAutoClearTareThresholdWeight;	// tare is automatically cleared
  									// when tare falls below this
  									// threshold.
	double primeAutoTareThresholdWeight;	// tare is taken when the weight
  								// exceeds this threshold.
	double primeAutoTareResetThresholdWeight;	// tare is primed to be retaken
  									// when weight falls below this
  									// threshold.
  
	double autoClearTareThresholdWeight;	// tare is automatically cleared
  									// when tare falls below this
  									// threshold.
	double autoTareThresholdWeight;	// tare is taken when the weight
  								// exceeds this threshold.
	double autoTareResetThresholdWeight;	// tare is primed to be retaken
										// when weight falls below this
										// threshold.
	uint8_t tareSource;			// keyboard, pushbutton, or autotare.
	char tareSourceString[3];// keyboard, pushbutton, or autotare.
	char storedWeightString[CONFIG_MAX_WT_DIGITS+2];
  
	uint8_t tareTakenFlag;		// TRUE  = tare taken,FALSE = tare clear
	uint8_t tareChangedFlag;	// TRUE = tare changed since,last weight cycle;
	uint8_t tareReEnabledToBeRetaken;
	uint8_t tareMode;			// 'G'  = Gross, 'N'	= Net
//	uint8_t tareEnabled;		// TRUE = enabled.
	uint8_t pushbuttonTareEnabled;	// TRUE = enabled.
	uint8_t keyboardTareEnabled;	// TRUE = enabled.
	uint8_t autoClearTareThresholdEnabled;	// TRUE = enabled
	uint8_t autoClearTareCheckMotion;		// TRUE = auto clear tare only,when there is no motion.
	uint8_t autoClearTareAfterPrintEnabled;// TRUE = clear tare after print
	uint8_t autoTareEnabled;				// TRUE = auto tare enabled
	uint8_t autoTareCheckMotion;			//  TRUE  = auto tare is taken, only when there is no motion.
	uint8_t netSignCorrection;
	uint8_t onPowerUpTare;			//=0:restart with current tare, 1: reset the tare to zero on power up
	uint8_t autoClearTareArmed; // TRUE = gross weight is above
  								// autoClear threshold.
  								// tare will be cleared next time
  								// gross weight falls below
  								// autoClear threshold.
  // uint8_t powerUpTareFlag;
	uint8_t bClearTareOnZero;
	SETUP_MOTIONTIMEOUT tareMotionWait;                  // 0 = no wait, 1-98 = wait N sec in motion, 99 = wait until stable
	uint8_t bTareWait;
	uint32_t tareWaitCount;
  //! Callback function for re-initialize data members
	void (*reInitializeDataMembers)(void *);
};

typedef struct TareData TARE;

extern const uint16_t motionTimeOut[];
extern TARE g_taredata;

void TARE_Init(TARE *this);
void TARE_InstallReInitialization(TARE *this, void (*pCallbackFunction)(void *));
void TARE_ReInitialization(TARE *this);
//void TARE_Calibrate(TARE *this, UNIT_tType oldUnitType, UNIT_tType currUnitType, double *pIncrArray);
void TARE_Calibrate(TARE *this, UNIT_tType oldUnitType, UNIT_tType currUnitType, double *pIncrArray);

void TARE_AutoTare(TARE *this, double *gwt, double *r_gwt, bool bMotion);
TARE_SCALE_tStatus TARE_CheckAutoClearTare(TARE *this);

//TARE_SCALE_tStatus TARE_CheckAutoClearTare(TARE *this);
TARE_SCALE_tStatus TARE_CheckAutoClearTare(TARE *this);
TARE_SCALE_tStatus TARE_TakeTare(TARE *this, double weight);

//TARE_SCALE_tStatus TARE_TakeTare(TARE *this, double weight);
//void TARE_FormatStoredWeight(TARE *this, double weight);
void TARE_FormatStoredWeight(TARE *this, double weight);

//TARE_SCALE_tStatus TARE_TakeNetSignCorrectedTare(TARE *this);
TARE_SCALE_tStatus TARE_TakeNetSignCorrectedTare(TARE *this);

void TARE_TakePushbuttonTare(TARE *this, int type, double weight, double incr,bool bMotion);
void TARE_SetKeyboardTare(TARE *this, int type, double weight, double incr);
TARE_SCALE_tStatus TARE_ClearTare(TARE *this);
TARE_SCALE_tStatus TARE_PublicClearTare(TARE *this, int type);
bool TARE_TareChanged(TARE *this);
void TARE_ClearTareChanged(TARE *this);
void TARE_SetTareChanged(TARE *this);
uint8_t * TARE_GetTareSource(TARE *this);
char * GetTareSourceString(TARE *this);
void TARE_SetTareSource(TARE *this, uint8_t source);
double TARE_GetFineTareWeight(TARE *this);
TARE_SCALE_tStatus TARE_PrintCompleteCheckClearTare(TARE *this);
bool TARE_IsKeyboardTareEnabled(TARE *this, int msgType);
bool TARE_IsPushbuttonTareEnabled(TARE *this);
int TARE_IsTareEnabled(TARE *this, int msgType);
void TARE_SetTareMode(TARE *this, uint8_t mode);
uint8_t TARE_GetTareMode(TARE *this);
void TARE_EnableMotionTare(TARE *this);
bool TARE_GetNetSignCorrection(TARE *this);
uint8_t TARE_GetClearTareOnZero(TARE *this);


#endif  // _TARE_H
