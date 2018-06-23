#ifndef _SCALE_H
#define _SCALE_H

#include "ScaleConfig.h"
#include "NotchIIRFilter.h"
#include "J_FILTER.H"
#include "filter.h"
#include "Motion.h"
#include "Zero.h"
#include "Tare.h"
#include "Unit.h"

#include "Cal.h"
#include "SetupParameterTable.h"

typedef enum
{
    SCALE_IN_GOOD_STATUS = 0, 
    SCALE_IN_MOTION, 
    SCALE_BAD_ZERO, 
    SCALE_OVERCAPACITY, 
    SCALE_UNDER_ZERO, 
    SCALE_IN_EXPAND
}SCALESTATUS;


typedef enum 
{
  NORMAL_INIT, 
  EXIT_SETUP, 
  CAL_INITZERO,
  CAL_INITSPAN
} INIT_MODE; 



typedef union
{
    double tempd;
    char  ucdoubel[8];
}CharOfUnionDouble;


typedef struct ScaleData
{
	uint8_t scaleType;            
                                  /* 
								  *'A' = Analog Scale, 'P' = Power Cell DigiTOL Scale,
                                  * 'I' = IDnet High-Precision Scale, 'D' = DigiNet High-
                                  * Precision Scale, 'S' = SICS Lab Balance,
                                  * 'U' = Summing, 'N' = None.
                                  */
	uint8_t scaleCellsNum;
    char scaleName[21];
	uint8_t upScaleTestPoint;     /*
                                  * 1, 2, 3, or 4. Typically, there is only one upscale
                                  * calibration point. For non-linear scale bases,
                                  * two additional calibration points can help
                                  * correct for the non-linearity. You may also
                                  * use these additional ¡°non-linearity¡± points to
                                  * see more weight resolution in the higher
                                  * ranges of a multi-ranging scale.
                                  */
	uint8_t overCapDivisions;
  /*scale commands*/
	uint8_t bZeroCommand;
	uint8_t bTareCommand;
	uint8_t bClearCommand;
	uint8_t bPrintCommand;
	uint8_t bSwitchUnits;
	uint8_t bSwitch2PriUnit;
	bool bToggleHighPrecision;
  
    /*scale status*/
	uint8_t bOverCapacity;
    // uint8_t bUnderZero;
  
	bool bExpandDisplay;				// 1= high-precision weight display to include an additional
								// decimal digit beyond the specified division size
	uint8_t currentRange;                // current weight range (0-3)
	SETUP_RANGE numberRanges;                // number of weight ranges(1~2)
	SETUP_APPROVAL market;                      // 1=OIML, 2=NTEP, 3=AUSTRALIA, 0=CANADA.
	bool bLegalForTrade;
  
	uint8_t usrGeo;
	uint8_t calGeo;
  
	uint8_t useCalFree;
//    uint8_t calFreeGeo;
  
	uint16_t expandDisplayCycles;        // expand display time represented by cycles
    
	int32_t capacityCounts;
	
	double currInc;                  // current increment size
	double currMiniDisplayInc;
	double customUnitsIncr;          // custom units increment size
	double lowInc;                   // multi-range parameters in cal unit
	double highInc;
	double lowHighThreshold;
	double scaleCapacity;
	double oneD[3];
	double overCapWeight;
	double programmableTare;

	#if CONFIG_MAX_UPSCALE_TEST_POINT == 1
    double countsPerCalUnit[1];
	#else
    double countsPerCalUnit[CONFIG_MAX_UPSCALE_TEST_POINT-1];
	#endif
    /*calibration values*/
	int32_t   zeroCalCounts; 
	int32_t   highCalCounts;
	double highCalWeight;
    double adjutk2;
  
	#if CONFIG_MAX_UPSCALE_TEST_POINT > 1
    int32_t   midCalCounts;
    double midCalWeight;
	#endif
	#if CONFIG_MAX_UPSCALE_TEST_POINT > 2
    int32_t   lowCalCounts;
    double lowCalWeight;
	#endif
	#if CONFIG_MAX_UPSCALE_TEST_POINT > 3
    int32_t   xlowCalCounts;
    double xlowCalWeight;
	#endif
	int32_t   zeroCounts2mv;        // factory calibration
	int32_t   spanCounts2mv;        // factory calibration value

	double        preload;
	UNIT_tSymbol preloadUnit;
	double        cellCapacity;
	UNIT_tSymbol cellCapUnit;
	double        ratedCellOutput;
      
  /*weight values*/
	double   fineGrossWeight;    // floating point gross weight.
	double   fineNetWeight;      // (fineGross - fineTare) 
  // float   fineTareWeight;     // floating point tare weight. this varible is in tare class now

	double   roundedGrossWeight; // floating point gross weight
					        // rounded to the nearest
						    // increment.
	double   roundedNetWeight;   // floating point net weight
						    // rounded to the nearest
						    // increment.
	double   roundedTareWeight;  // floating point tare weight
						    // rounded to nearest increment.

	char      grossString[12];    // printable gross weight string.	    
	char      netString[12];      // printable net weight string.
	char      tareString[12];     // printable tare weight string.

	double   linearityFactor[CONFIG_MAX_UPSCALE_TEST_POINT-1];    // second-order linearity
							// adjustment factor.
	double   linearityGain[CONFIG_MAX_UPSCALE_TEST_POINT-1];      // first-order linearity// adjustment factor.
   
//Scale:
    
//	uint8_t setupApproval;
	double setupRangeOneIncrement;
	double setupRangeTwoIncrement;
	double setupCapacityValue;
	uint8_t setupLinearity;
  
	uint8_t setupAutoZero;
	uint8_t setupAutoZeroRange;    
  
	uint8_t setupPowerupZero;
	uint8_t setupPushButtonTare;   

  
	uint8_t setupLowPassFilter;    
	uint8_t setupStabilityFilter;
	
	uint8_t bPrintWait;
	uint8_t printMotionWait; 
	uint32_t printWaitCount; 
	DEMANDPRINTSOURCE demandPrintSource; 

    /*serial*/
    //com1
	uint8_t setupCOM1BaudRate;
	uint8_t setupCOM1DateBits;
	uint8_t setupCOM1Parity;
    uint8_t setupCOM1FlowControl;
	uint8_t	setupCOM1InterFace;

	
	uint8_t  setupPassword;/*lxw add */
	double  password;/*lxw add */       
	uint8_t	inputRotation[11];         
	uint8_t setupWeightUnits;          
	
	  
	uint8_t setupByteOrder;
	char setupScaleName[21];
	char setupCertificateNo[21];
	double currentTareWeight; 
	uint8_t maintenanceRestorefromSDCard; 
//maintenance:   
    uint8_t zerodriftmode;
	double 	zerodriftthreshold;
	double   overloadthreshold;
  // class
//	struct MotionData      *motion;
//	struct ZeroData        *zero;
//	struct TareData        *tare;
//	struct UnitData        *unit;
//	struct FilterData      *filter;
//	struct JFilterData     *jfilter;
    
    
    MOTION      *motion;
	ZERO        *zero;
	TARE        *tare;
	UNIT       *unit;
//	FILTER      *filter;
//	JFILTER     *jfilter;
    

  //! Callback function for re-initialize data members
	void (*reInitializeDataMembers)(struct ScaleData *, int);
	void (*increaseZeroCommandCounter)(void);
	void (*increaseZeroFailureCounter)(void);
}SCALE;

//typedef struct ScaleData SCALE;

typedef struct {
    float roundedGross;
    float roundedNet;
    float roundedTare;
} WEIGHT;

typedef struct {
    char *pGrossString;
    char *pNetString;
    char *pTareString;
} WEIGHT_STRING;

// defined in Unit.c
//void UNIT_SwitchUnits(SCALE *this);
//float UNIT_ConvertUnits(UNIT_tSymbol srcUnit, UNIT_tSymbol dstUnit, const float weight);
//float UNIT_ConvertUnitType(SCALE *this, UNIT_tType srcType, UNIT_tType dstType, const float weight);
//float RoundToNearest1_2_5(float inc, unsigned char switchUnit);
//signed char GetIncrIndex(float incr);
//signed char GetSwitchIncrIndex(unsigned char primincr);


typedef enum
{
    COMMAND_NONE = 0, 
    COMMAND_LOCAL,
    COMMAND_REMOTE
}COMMANDSOURCE;


extern SCALE g_ScaleData;

extern const double incrTable[];
extern const double geoTable[];

void InitScaleStruct(SCALE *Pscale);
void SCALE_Init(SCALE *this);
void SCALE_InstallReInitialization(SCALE *this, void (*pCallbackFunction)(SCALE *, int));
void SCALE_InstallZeroCommandCounter(SCALE *this, void (*pCallbackFunction)(void));
void SCALE_InstallZeroFailureCounter(SCALE *this, void (*pCallbackFunction)(void));
void SCALE_ReInitialization(SCALE *this, int initMode);
void SCALE_PostProcess(SCALE *this, const long filteredCounts);
void SCALE_Linearity(SCALE *this, int32_t relCounts, uint32_t factorIndex);
void SCALE_ProcessMultiRangeInterval(SCALE *this);
void SCALE_ProcessToggleHighPrecisionWeight(SCALE *this);
void SCALE_CalculateDivisinon(SCALE *this);
void SCALE_GetIncrementArray(SCALE *this, double *pIncr);
double SCALE_GetIncrementByWeight(SCALE *this, double weight);
double SCALE_GetCapacityByRange(SCALE *this);

double SCALE_RoundedWeight(double weight, double incr);
int32_t SCALE_CalcDP(int32_t digits, double incr);
void SCALE_SwitchUnits(SCALE *this);
void SCALE_AdjustTareAtMultiRangeZero(SCALE *this, bool bMotion, bool bCoz);
void SCALE_FormatDisplayWeight(char *pWeightString, double *pWeight, SCALE *Ptrscale);
void SCALE_FormatDisplayTare(char *pWeightString, double *pWeight, double currIncr);
double SCALE_GetRoundedGross(SCALE *this);
bool SCALE_GetDisplayMode(SCALE *this);
bool SCALE_GetLFT(SCALE *this);
// int32_t SCALE_GetDp(int32_t incrIndex);
uint8_t SCALE_GetOverCapacity(SCALE *this);
void SCALE_CalcSpanFactor(SCALE *this);
void SCALE_CalcLinearFactor(SCALE *this);
void SCALE_BuildContinousOutputString(SCALE *this, char *pOutString);
int32_t SCALE_GetIncrIndex(float incr);
int32_t SCALE_GetDp(float incr);
void SCALE_GetWeight(SCALE *this, WEIGHT *pWeight);
void SCALE_GetWeightString(SCALE *this, WEIGHT_STRING *pWeightString);
extern SCALESTATUS SCALE_CheckScaleStatus(SCALE *this); 
extern void SCALE_ShowScaleStatus(SCALESTATUS scaleStatus); 

//xht add this functions
double Calculate_fineGrossweight(int relcounts,CAL_STAT * cal_stat);
double Calculate_RoundWeight(double fineweight,double incr);
double RoundedWeight(double wgt,double tmpincr_f);

int CheckIncrIsValid(double data);
void ResetScaleParameters();
//void reinitializaeSetupMenuParameters(int initMode);
void reInitializeScaleParameters(SCALE *this, int initMode);
void CAL_AdjustCalParams(SCALE *pScale, int testpoint, double addLoad, int32_t calCounts);
void InitScaleParamters(SCALE *Pscale);

#endif
