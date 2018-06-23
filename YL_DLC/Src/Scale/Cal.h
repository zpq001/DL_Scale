#ifndef _CAL_H
#define _CAL_H

//#include "scale.h"
#include "Unit.h"


#define CAPTURE_CYCLES_PER_SEC      30
#define CAPTURE_TIMER               (5*CAPTURE_CYCLES_PER_SEC)

// definition for CalFREE
// A/D output counts
#define AD_OUTPUT_ZERO              0
#define AD_OUTPUT_FULL              1000000
#define MAX_AD_OUTPUT               1010000//1510000 2009-4-1 10:41 lxw change 

// A/D gain, 500000 counts/mV/V
#define AD_GAIN_2mv                 500000.0
#define AD_GAIN_3mv                 333333.333333


typedef enum {
    CAL_OK = 0,
    CAL_IN_PROGRESS,
    CAL_DONE,
    CAL_IN_MOTION,
    CAL_DYNAMIC,
    CAL_DYNAMIC_OK,
    CAL_FAIL
} CAL_STATUS;

typedef void (*CAL_CALLBACK)(CAL_STATUS, int);

typedef enum {
    CAL_ZERO = 0,
    CAL_XLOW,
    CAL_LOW,
    CAL_MID,
    CAL_HIGH,
    CAL_FREE,
    CAL_STEP
} CAL_TYPE;

typedef enum {
    GAIN_JUMPER_2mv = 0,
    GAIN_JUMPER_3mv
} GAIN_JUMPER;


typedef struct 
{

    int  testpoints;
	int 	zero_counts;	

	int		high_counts;		
	int		mid_counts;
	int		low_counts;
	int		xlow_counts;

	double 	high_weight;	
	double	mid_weight;
	double	low_weight;
	double	xlow_weight;

	double 	span;	
	double	span2;
	double	span3;
	double	linear_factor;	
	double	scale_factor;	
	double	linear_factor2;
	double	scale_factor2;
	double	linear_factor3;
	double	scale_factor3;
	
	// ---------------------------------------	
	
	int 	flag;
	int 	zero_flag;
    
	uint8_t 	Cal_grvty_code; // calibration GEO CODE
	uint8_t  	Local_grvty_code; // local GEO CODE
} CAL_STAT, *PCAL_STAT;


extern const CAL_TYPE calSpanType[];
extern uint32_t calTimeInterval;
extern int32_t tmpMidCalCounts;
extern float   tmpMidCalWeight;

void CAL_Init(uint32_t inCounts);
CAL_STATUS CAL_Calibrating(uint32_t inCounts, uint32_t *pCounts, CAL_CALLBACK calCallback);
//void CAL_AdjustCalParams(SCALE *pScale, int testpoint, double addLoad, int32_t calCounts);
CAL_STATUS CAL_GetStatus(void);
void CAL_SetStatus(CAL_STATUS currStatus);
//void CAL_CalcStepCalParams(SCALE *pScale, uint32_t stepNum, float *pStepCalWeight, uint32_t *pStepCalCounts);
//bool16 CAL_CalcCalFreeParams(SCALE *pScale, GAIN_JUMPER gainJumper);
CAL_STATUS CAL_FactoryCalibrating(uint32_t inCounts, uint32_t *pCounts, CAL_CALLBACK calCallback);

void calculate_span(CAL_STAT *cal_stat);
void calculate_linear(CAL_STAT * cal_stat);


#endif
