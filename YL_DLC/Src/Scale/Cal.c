//#include "RB_Type.h"
//#include "RB_Timer.h"

#include "Scale.h"
//#include "Power.h"
#include "Cal.h"
#include <math.h>
#include "UserParam.h"

extern void HAL_Delay(uint32_t Delay);

const CAL_TYPE calSpanType[] = {CAL_HIGH, CAL_MID};
//int32_t tmpMidCalCounts = 0;
//float   tmpMidCalWeight = 0.0;

//static int32_t previousInCounts;
//static uint32_t calSum;
//static uint16_t calCounter;
//static uint16_t unstableCounter;
static CAL_STATUS calStatus;
//static bool calReiniTicker = false;

void CAL_Init(uint32_t inCounts)
{
//    calSum           = 0;
//    calCounter       = 0;
//    unstableCounter  = 0;
//    previousInCounts = inCounts;
    calStatus        = CAL_IN_PROGRESS;
    m_motionCounter  = 0;
//    calReiniTicker   = false;
    //XHT_2018
//    calTimeInterval  = RB_TIMER_GetSystemTime();
}

CAL_STATUS CAL_Calibrating(uint32_t inCounts, uint32_t *pCounts, CAL_CALLBACK calCallback)
{   
//    bool bMotion;
//    uint32_t ticker = RB_TIMER_GetSystemTime();
//    static uint32_t calMotionTicker = 0;
//
//    // PDX_GetWeight((long *)&inCounts);
//
//    bMotion = MOTION_Detect(inCounts, &previousInCounts, 150, CAPTURE_CYCLES_PER_SEC/3);
//    
//    if (calStatus == CAL_DYNAMIC)
//        bMotion = false;
//    if (bMotion)
//    {
//        if (calReiniTicker == true)
//        {
//            calReiniTicker  = false; 
//            calMotionTicker = 0; 
//            calTimeInterval = ticker; 
//        }
//            
////        unstableCounter++;
//
////        if (unstableCounter >= CAPTURE_TIMER)
//        if ((calMotionTicker + ticker - calTimeInterval)  >= 5000)
//        {
//            calStatus = CAL_DYNAMIC;
//            calTimeInterval = ticker;
//        }
//        
//        if (calStatus != CAL_DYNAMIC)
//            calStatus = CAL_IN_MOTION;
//            
//        calSum = 0;
//        calCounter = 0;
//        if (calCallback)
//            (*calCallback)(calStatus, (calMotionTicker + ticker - calTimeInterval) /1000); 
//    }
//    else
//    {
//        if (calReiniTicker == false)
//        {
//            calReiniTicker = true;
//            calMotionTicker += ticker - calTimeInterval;  
//            calTimeInterval = ticker;
//        }
//        
//        if (calStatus == CAL_IN_MOTION)
//            calStatus = CAL_IN_PROGRESS;
//
//        if ((ticker - calTimeInterval) >= 2000)
//        {
//            calSum += inCounts;
//            calCounter++;
//        }
//        if (calCallback)
//            (*calCallback)(calStatus, (ticker - calTimeInterval)/1000);
//
//        if ((ticker - calTimeInterval) >= 5000)
////        if (calCounter == CAPTURE_TIMER)
//        {
//            *pCounts = calSum / calCounter;
//            calMotionTicker = 0;
//            if (calStatus == CAL_DYNAMIC)
//                calStatus = CAL_DYNAMIC_OK;
//            else
//                calStatus = CAL_OK;
//        }
//    }
    
    return calStatus;
}

CAL_STATUS CAL_GetStatus(void)
{
    return calStatus;
}

void CAL_SetStatus(CAL_STATUS currStatus)
{
    calStatus = currStatus;
}

void CAL_AdjustCalParams(SCALE *pScale, int testpoint, double addLoad, int32_t calCounts)
{	
	int deltaHigh, deltaMid, deltaLow;
    CharOfUnionDouble chardouble;
  
	switch (testpoint)
	{
		case 0:
			deltaHigh = pScale->highCalCounts - pScale->zeroCalCounts;
			deltaMid  = pScale->midCalCounts  - pScale->zeroCalCounts;
			deltaLow  = pScale->lowCalCounts  - pScale->zeroCalCounts;
			//deltaXlow = pScale->xlowCalCounts - pScale->zeroCalCounts;
			pScale->zeroCalCounts = calCounts; 
			pScale->zero->calibratedZeroCounts = calCounts; 
			pScale->zero->currentZeroCounts = calCounts;
//			boolZeroCountsChange = true; 
			if (pScale->zero->powerupZeroStatus==POWERUP_ZERO_OVER || pScale->zero->powerupZeroStatus==POWERUP_ZERO_UNDER)
			    pScale->zero->powerupZeroStatus = POWERUP_ZERO_SUCCESS;//lxw 2009-2-16 8:54
			// ZERO_SetReCalibratedZeroCounts(&(pScale->zero), calCounts);
			switch (pScale->upScaleTestPoint)
			{
				case 1:
					pScale->highCalCounts = pScale->zeroCalCounts + deltaHigh;
					break;
				case 2:
					pScale->highCalCounts = pScale->zeroCalCounts + deltaHigh;
					pScale->midCalCounts  = pScale->zeroCalCounts + deltaMid;
					break;
				case 3:
					pScale->highCalCounts = pScale->zeroCalCounts + deltaHigh;
					pScale->midCalCounts  = pScale->zeroCalCounts + deltaMid;
					pScale->lowCalCounts  = pScale->zeroCalCounts + deltaLow;
					break;
				case 4:
					pScale->highCalCounts = pScale->zeroCalCounts + deltaHigh;
					pScale->midCalCounts  = pScale->zeroCalCounts + deltaMid;
					pScale->lowCalCounts  = pScale->zeroCalCounts + deltaLow;
//					pScale->xlowCalCounts = pScale->zeroCalCounts + deltaXlow;					
					break;
				default:
					break;
			}

            USER_PARAM_Set(BLK0_zeroCalCounts, (uint8_t *)(&calCounts)); 
			break;
			
		case 1:

//			pScale->highCalCounts = calCounts; 
//			pScale->highCalWeight = addLoad; 
//			if (!boolZeroCountsChange)
//			{
//			    pScale->zeroCalCounts = pScale->zero.currentZeroCounts; 
//			    pScale->zero.calibratedZeroCounts = pScale->zero.currentZeroCounts; 
//			}
            
            chardouble.tempd = addLoad;
            USER_PARAM_Set(BLK0_highCalWeight, (uint8_t *)(chardouble.ucdoubel)); 
//            HAL_Delay(50);
            USER_PARAM_Set(BLK0_highCalCounts, (uint8_t *)(&calCounts)); 
            
            pScale->highCalCounts = calCounts; 
			pScale->highCalWeight = addLoad; 
            
            
            
			break;
//		case CAL_XLOW:
////			pScale->xlowCalCounts = calCounts;
////			pScale->xlowCalWeight = addLoad;				
////			m_MaintLog.event = E_SPAN_CALIBRATION;
//			break;
//					
//		case CAL_LOW:
//			pScale->lowCalCounts = calCounts;
//			pScale->lowCalWeight = addLoad;				
////			m_MaintLog.event = E_SPAN_CALIBRATION;
//
//			break;
//		case CAL_MID:
////		    if (boolZeroCountsChange)
////		        pScale->midCalCounts = calCounts; 
////		    else
////		        pScale->midCalCounts = calCounts - pScale->zero.currentZeroCounts + pScale->zeroCalCounts; 
////		    pScale->midCalWeight = addLoad;
//			pScale->midCalCounts = calCounts;
//			pScale->midCalWeight = addLoad;
////			m_MaintLog.event = E_SPAN_CALIBRATION;
//			break;
			
		default:
			break;
	}
	
	if (testpoint != 0)
	    pScale->calGeo = pScale->usrGeo;	
}


void calculate_span(CAL_STAT *cal_stat)
{
	double 		  tmp = 0; 
	cal_stat->span2 = 0;
	cal_stat->span3 = 0;
	switch (cal_stat->testpoints)
	{
	case 1:
	case 2:
		tmp = ((double)cal_stat->high_weight);
		tmp /= (double)(cal_stat->high_counts - cal_stat->zero_counts);
		cal_stat->span = tmp;
		break;
	case 3:
		tmp = ((double)cal_stat->mid_weight);
		tmp /= (double)(cal_stat->mid_counts - cal_stat->zero_counts);
		cal_stat->span = tmp;
        
		tmp = ((double)cal_stat->high_weight);
		tmp /= (double)(cal_stat->high_counts - cal_stat->zero_counts);
		cal_stat->span2 = tmp;
		break;
	case 4:
		tmp = ((double)cal_stat->low_weight);
		tmp /= (double)(cal_stat->low_counts - cal_stat->zero_counts);
		cal_stat->span = tmp;
        
		tmp = ((double)cal_stat->mid_weight);
		tmp /= (double)(cal_stat->mid_counts - cal_stat->zero_counts);
		cal_stat->span2 = tmp;
        
		tmp = ((double)cal_stat->high_weight);
		tmp /= (double)(cal_stat->high_counts - cal_stat->zero_counts);
		cal_stat->span3 = tmp;
		break;
	default:
		break;
	}
}

void calculate_linear(CAL_STAT * cal_stat)
{

    
	double   wt1;
	double	 wt2;
	double   R1;
	double	linear_factor;
	double	scale_factor;
    
	double   wt12;
	double	 wt22;
	double   R12;
	double	linear_factor2;
	double	scale_factor2;
    

    
	cal_stat->linear_factor = 0;
	cal_stat->linear_factor2 = 0;
	cal_stat->linear_factor3 = 0;
    
	cal_stat->scale_factor = 0;
	cal_stat->scale_factor2 = 0;
	cal_stat->scale_factor3 = 0;
    
	switch (cal_stat->testpoints)
	{
	case 2:
		wt1 = (((double)cal_stat->mid_weight)) / cal_stat->span;
		wt2 = (((double)cal_stat->high_weight)) / cal_stat->span;
		R1 = (double)(cal_stat->mid_counts - cal_stat->zero_counts);
		linear_factor = (R1 - wt1) / (wt1*wt2 - R1*R1);
		scale_factor = 1.0 / (1.0 + linear_factor * wt2);
		cal_stat->linear_factor = linear_factor;
		cal_stat->scale_factor = scale_factor;
		break;
        
	case 3:
		if (cal_stat->low_counts >= cal_stat->mid_counts)
			break;
		wt1 = (((double)cal_stat->low_weight)) / cal_stat->span;
		wt2 = (((double)cal_stat->mid_weight)) / cal_stat->span;
		R1 = (double)(cal_stat->low_counts - cal_stat->zero_counts);
        
		linear_factor = (R1 - wt1) / (wt1*wt2 - R1*R1);
        
		scale_factor = 1.0 / (1.0 + linear_factor * wt2);
		cal_stat->linear_factor = linear_factor;
		cal_stat->scale_factor = scale_factor;
        
		wt12 = (((double)cal_stat->mid_weight)) / cal_stat->span2;
		wt22 = (((double)cal_stat->high_weight)) / cal_stat->span2;
		R12 = (double)(cal_stat->mid_counts - cal_stat->zero_counts);
		linear_factor2 = (R12 - wt12) / (wt12*wt22 - R12*R12);
		scale_factor2 = 1.0 / (1.0 + linear_factor2 * wt22);
		cal_stat->linear_factor2 = linear_factor2;
		cal_stat->scale_factor2 = scale_factor2;
		break;
        
	case 4:
		if ((cal_stat->xlow_counts >= cal_stat->low_counts) || (cal_stat->low_counts >= cal_stat->mid_counts))
			break;
        
		wt1 = (((double)cal_stat->xlow_weight)) / cal_stat->span;
		wt2 = (((double)cal_stat->low_weight)) / cal_stat->span;
		R1 = (double)(cal_stat->xlow_counts - cal_stat->zero_counts);
        
		linear_factor = (R1 - wt1) / (wt1*wt2 - R1*R1);
		scale_factor = 1.0 / (1.0 + linear_factor * wt2);
		cal_stat->linear_factor = linear_factor;
		cal_stat->scale_factor = scale_factor;
        
		wt1 = (((double)cal_stat->low_weight)) / cal_stat->span2;
		wt2 = (((double)cal_stat->mid_weight)) / cal_stat->span2;
		R1 = (double)(cal_stat->low_counts - cal_stat->zero_counts);
        
		linear_factor = (R1 - wt1) / (wt1*wt2 - R1*R1);
		scale_factor = 1.0 / (1.0 + linear_factor * wt2);
		cal_stat->linear_factor2 = linear_factor;
		cal_stat->scale_factor2 = scale_factor;
        
		wt12 = (((double)cal_stat->mid_weight)) / cal_stat->span3;
		wt22 = (((double)cal_stat->high_weight)) / cal_stat->span3;
		R12 = (double)(cal_stat->mid_counts - cal_stat->zero_counts);
		linear_factor2 = (R12 - wt12) / (wt12*wt22 - R12*R12);
		scale_factor2 = 1.0 / (1.0 + linear_factor2 * wt22);
		cal_stat->linear_factor3 = linear_factor2;
		cal_stat->scale_factor3 = scale_factor2;
		break;
        
	default:
		break;
	}
}



