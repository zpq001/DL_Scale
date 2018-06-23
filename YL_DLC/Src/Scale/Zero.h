#ifndef  _ZERO_H
#define  _ZERO_H

#include"comm.h"

#include "SetupParameterTable.h"

typedef enum {
    POWERUP_ZERO_SUCCESS = 0,
    POWERUP_ZERO_IN_PROGRESS,
    POWERUP_ZERO_MOTION,
    POWERUP_ZERO_OVER,
    POWERUP_ZERO_UNDER
} POWERUP_ZERO_tStatus;

typedef enum {
    ZERO_SUCCESS = 0,
    ZERO_IN_PROGRESS,
    SCALE_IN_MOTION_DURING_ZERO,
    SCALE_OUT_OF_POSITIVEZEROING_RANGE,
    SCALE_OUT_OF_NEGATIVEZEROING_RANGE,
    ILLEGAL_SCALE_MODE_DURING_ZERO,
    INVALID_ZERO_FUNCTION,
    SCALE_OUT_OF_ZEROING_LIMIT,
} ZERO_SCALE_tStatus;

typedef enum {
    ZERO_STATUS_NONE      = 0,
    POWERUP_ZERO_CAPTURED = 0x20,
    UNDER_ZERO            = 0x40,
    CENTER_OF_ZERO        = 0x80
} ZERO_tStatus;

//typedef enum
//{
//	IDNET_OUT_OF_RANGE_NOT_APPLICABLE = 0,
//	IDNET_OUT_OF_RANGE_NEITHER = ' ',
//	IDNET_OUT_OF_RANGE_OVER = '+',
//	IDNET_OUT_OF_RANGE_UNDER = '-',
//	IDNET_CENTER_OF_ZERO = 'Z'
//}IDNET_OUT_OF_RANGE_STATUS;
//
//typedef enum
//{
//	IDNET_ZERO_NOT_APPLICABLE = 0,
//	IDNET_ZERO_APPLICABLE = 1,
//	IDNET_ZERO_ACCEPTED = 'B',
//	IDNET_ZERO_REJECTED = 'L',
//	IDNET_ZERO_OVER = '+',
//	IDNET_ZERO_UNDER = '-'
//}IDNET_ZERO_STATUS;

// class ZERO
struct ZeroData
{
    struct ScaleData *pScale;
//    SCALE  *pScale;
    int32_t calibratedZeroCounts;                   // raw weight reading counts for zero at calibration.
    int32_t powerUpZeroCounts;	                    // raw weight reading counts for zero at power-up.
    int32_t currentZeroCounts;                      // raw weight reading counts for the current zero.
    int32_t centerOfZeroCounts[3];                  // the number raw weight counts from current zero 
										        // at the scale is defined to be at zero.
    int32_t underZeroCounts;                        // counts per five divisions triggers the under zero flag.
                                                // static int32_t capacityCounts;
    POWERUP_ZERO_tStatus powerupZeroStatus;
    ZERO_SCALE_tStatus zeroScaleStatus;
    int32_t powerUpZeroPositiveCounts;              // weight converted to raw reading counts.
    int32_t powerUpZeroNegativeCounts;              // weight converted to raw reading counts.
//    uint8_t powerUpZeroPositivePercentCapacity;      // percent of scale capacity within which zero will be
										        // captured at power up when the weight is above calibrated
										        // zero. A value of zero disables this feature.
//    uint8_t powerUpZeroNegativePercentCapacity;	    // percent of scale capacity within which zero will be
										        // captured at power up when the weight is below calibrated
										        // zero. A value of zero disables this feature.
    int32_t pushbuttonZeroPositiveDelta;            // percent converted to raw reading counts.
    int32_t zeroLimitPositiveCounts;                // power up zero + pushbutton zero positive counts.
    SETUP_ZERORANGE_PUSHBUTTON  pushbuttonZero;                 // TRUE  = enabled,FALSE = not enabled
    SETUP_ZERORANGE_POWERUP  powerupZero;

    int32_t pushbuttonZeroNegativeDelta;            // percent converted to raw
										        // reading counts.
    int32_t zeroLimitNegativeCounts;                // power up zero - pushbutton
										        // zero negative counts.
    ZERO_tStatus zeroStatus;                    // zero status flags
										        // CENTER_OF_ZERO           0x80
										        // UNDER_ZERO               0x40
										        // POWER_UP_ZERO_CAPTURED   0x20
    int32_t previousCounts;                         // last reading counts
    SETUP_AZMDIVISION autoZeroWindowDivisions;                // divisions from current zero
										        // in which auto zero maintenance
										        // is applied.
										        // 00 implies no auto zeroing
    SETUP_UNDERZEROBLANKING underZeroBlanking;
    bool                  underzeroWait; 
    int32_t autoZeroWindowCounts[3];                // converted to raw reading
										        // counts.
    int32_t adjustmentCountsPerCycle[3];            // number of counts that current
										        // zero can be adjusted at
										        // each reading to meet the
										        // specified rate.
    int32_t autoZeroMotionCounts[3];                // maximum number of counts for
										        // AZM to be in "no motion."
    uint32_t autoZeroMotionCycles;                   // number of cycles for count
										        // delta must be within the
										        // motion counts for there to
										        // be "no motion"
//    uint8_t underZeroDivisions;
    SETUP_MOTIONTIMEOUT zeroMotionWait;                          // 0 = no wait, 1-98 = wait N sec in motion, 99 = wait until stable
    // uint8_t market;                                  // U=USA, E=European, A=Australia, C=Canada.
    // uint8_t legalForTrade;
    SETUP_AZMMODE enableAZM;			                    // flag to temporarily disable
										        // AZM.  TRUE = enabled.
    uint32_t powerUpZeroDelayCycles;
    int32_t relCounts;   		                    //counts relative to current zero
    float cozLimitWeight[3];	                // weight limit for center of zero
    uint32_t zeroWaitCount;
    uint8_t bZeroWait;
    uint8_t powerUpZeroSet;
    uint8_t bClearTareOnZero;
    //! Callback function for re-initialize data members
    void (*reInitializeDataMembers)(void *);
    void (*saveCurrentZero)(int);
};

typedef struct ZeroData ZERO;

extern ZERO g_zerodata;

void ZERO_Init(ZERO *this);
void ZERO_InstallReInitialization(ZERO *this, void (*pCallbackFunction)(void *));
void ZERO_InstallSavingCurrentZero(ZERO *this, void (*pCallbackFunction)(int));
void ZERO_ReInitialization(ZERO *this);
void ZERO_Calibrate(ZERO *this, double *span, double *pIncrArray, int32_t capacityCounts);
void ZERO_SetPushbuttonZero(ZERO *this, int32_t counts, uint8_t netMode,bool bMotion);
void ZERO_ProcessPowerupZero(ZERO *this, int32_t counts,bool bMotion);
int32_t ZERO_ProcessZero(ZERO *this, int32_t counts, uint8_t netMode, uint8_t range,bool bMotion);
int32_t ZERO_GetCurrentZero(ZERO *this);
void ZERO_SetCurrentZero(ZERO *this, int32_t counts);
void ZERO_SetReCalibratedZeroCounts(ZERO *this, int32_t counts);
void ZERO_SetCalibratedZeroCounts(ZERO *this, int32_t counts);
int32_t ZERO_GetCalibratedZeroCounts(ZERO *this);
void ZERO_SetPowerUpZero(ZERO *this, int32_t counts);
bool ZERO_GetCenterOfZero(ZERO *this);
bool ZERO_GetUnderZero(ZERO *this);
bool ZERO_GetPowerUpZeroCaptured(ZERO *this);
POWERUP_ZERO_tStatus ZERO_GetPowerUpZeroStatus(ZERO *this);
ZERO_SCALE_tStatus ZERO_GetZeroScaleStatus(ZERO *this);

#endif // #ifndef  _ZERO_H
