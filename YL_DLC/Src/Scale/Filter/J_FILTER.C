/*-----------------------------------------------------------------------*
 * Filename....: j_filter.cpp
 * Scope.......: Mayer Digital Filtering
 * Last Change.:
 *
 * Version      Date                      Description
 *   1.0        02/1/92 	          New
 *
 *          COPYRIGHT (C) 1992.  All Rights Reserved.
 *       Mettler Toledo Corporation, Worthington, Ohio,  USA.
 *------------------------------------------------------------------------*/
#include "j_filter.h"

//*****************************************************************
// 	Digital filtering coefficients
//*****************************************************************
#include "2pct4p.h"
#include "3pct4p.h"
#include "4pct4p.h"
#include "5pct4p.h"
#include "6pct4p.h"
#include "7pct4p.h"
#include "8pct4p.h"
#include "9pct4p.h"
#include "10pct4p.h"
#include "11pct4p.h"
#include "12pct4p.h"
#include "13pct4p.h"
#include "14pct4p.h"
#include "15pct4p.h"

#include "2pct6p.h"
#include "3pct6p.h"
#include "4pct6p.h"
#include "5pct6p.h"
#include "6pct6p.h"
#include "7pct6p.h"
#include "8pct6p.h"
#include "9pct6p.h"
#include "10pct6p.h"
#include "11pct6p.h"
#include "12pct6p.h"
#include "13pct6p.h"
#include "14pct6p.h"
#include "15pct6p.h"

#include "2pct8p.h"
#include "3pct8p.h"
#include "4pct8p.h"
#include "5pct8p.h"
#include "6pct8p.h"
#include "7pct8p.h"
#include "8pct8p.h"
#include "9pct8p.h"
#include "10pct8p.h"
#include "11pct8p.h"
#include "12pct8p.h"
#include "13pct8p.h"
#include "14pct8p.h"
#include "15pct8p.h"

//#include "sd_index.h"
#include "UserParam.h"
#include <stdlib.h>
//*****************************************************************
//	Filtering coefficients
//*****************************************************************

FILT_COEF filter_0 = 
{
	0,	/* # of cells */
	{	/* amplif    den coef 2      den coef 3 */
		{ 	1.0,		1.0,		1.0	},
		{ 	1.0,		1.0,		1.0	}
	}
};

/******************************************************************
 * Global data
 ******************************************************************/

FILT_COEF *		currentFilter ;      // pointer to current filter coefficient structure
FILT_COEF *	    standardFilter;
FILT_COEF *		fillnoiseFilter ;
double          hist[MAX_FILT_CELLS][4] ;    // filter history storage
double			filteredOutput ;

static long            	fillnoise_motion_buffer[FILLNOISE_MOTION_READINGS] ;
static int             	fillnoise_motion_ptr ;
static long	            fillnoise_motion_counts ;
static long            	fillnoise_zero_counts;
static unsigned char   	fillnoise_filter_switch;


static void SetLowPassFilterCornerFrequency(double freq,short poles,double weightUpdateRate,double *retFreq,short *retPoles );
static void InitFilter(double initCounts);
static double  MayerFilter(double * counts);

/*---------------------------------------------------------------------*
 * Name         : LOW_PASS_FILTER::LOW_PASS_FILTER
 * Prototype in : j_filter.h
 * Description  : constructor. Read filter frequency from shared memory.
 * Return value : None
 *---------------------------------------------------------------------*/
void StabilityFilterInit(double weightUpdateRate,double initialCounts)
{
	double	retFreq;
	short	retPoles;
	double  frequency;
	unsigned char	fillNoise = 1;  //default enable
	unsigned char poles = 8; //default

//    sd_get(&fillNoise, DI_cs0118);  // enable  disable
//    sd_get(&frequency, DI_cs0114); // LowPassFreq  defalut
//   	sd_get(&poles, DI_cs0115);      // low pass of poles
    
    USER_PARAM_Get(BLK0_setupLowPassFilter,   (uint8_t*)&(frequency)); 
//	sd_get(&LowPassFreq,DI_cs0114); // 截止频率0.1--9.9
    if(frequency<0.1||frequency>9.9)
    {
      frequency = 2.0;
      USER_PARAM_Set(BLK0_setupLowPassFilter,   (uint8_t*)&(frequency)); 
    }
    
	SetLowPassFilterCornerFrequency(frequency,poles,weightUpdateRate,&retFreq,&retPoles ); // 后两个输出
	fillnoise_filter_switch = fillNoise;
	fillnoise_motion_ptr = 0;
	fillnoiseFilter = &filter_2_8;
	fillnoise_motion_counts =
	fillnoise_zero_counts   = 0;
	InitFilter(initialCounts);
}

/*---------------------------------------------------------------------*
 * Name         : LOW_PASS_FILTER::CalibrateFillnoiseFilter
 * Prototype in : j_filter.h
 * Description  : set fillnoise sensitivity counts to half a division
 * Return value : None
 *---------------------------------------------------------------------*/
void CalibrateStabilityFilter( double span_factor ) 
{
	fillnoise_motion_counts = 
	fillnoise_zero_counts   =  (long)(.5 * span_factor);
}
/*---------------------------------------------------------------------*
 * Name         : LOW_PASS_FILTER::FilterWeight
 * Prototype in : j_filter.h
 * Description:	: If weight is in zero range, apply stability filter.
 *    			: Else if scale is not in motion, apply stability filter.
 *              : Else apply standard filter.
 *				: When switching filters, set filter history to
 *				: current raw count.
 *
 * Return value : Filtered floating point weight
 *---------------------------------------------------------------------*/
double FilterWeight(double * counts)
{
	int i;

	if ( fillnoise_filter_switch ) 
	{
		// FILLNOISE MOTION DETECTION
		// method for motion detection calculates difference between
		// maximum and minimum readings in a motion buffer of 'n' elements
		long max_reading, min_reading;
		long lcounts = (long)(*counts);

		if (++fillnoise_motion_ptr >= FILLNOISE_MOTION_READINGS)
			fillnoise_motion_ptr = 0;
		fillnoise_motion_buffer[fillnoise_motion_ptr] = lcounts;
		// get minimum reading and maximum reading in motion buffer
		min_reading = max_reading = fillnoise_motion_buffer[0];
		for (i = 1; i < FILLNOISE_MOTION_READINGS; i++) 
		{
			if ( fillnoise_motion_buffer[i] > max_reading )
				max_reading = fillnoise_motion_buffer[i];
			else if ( fillnoise_motion_buffer[i] < min_reading )
				min_reading = fillnoise_motion_buffer[i];
		}
		if(labs(lcounts-(long)ZERO_GetCurrentZero(&g_zerodata))<fillnoise_zero_counts
			|| max_reading-min_reading < fillnoise_motion_counts ) 
		{
			if ( currentFilter == standardFilter ) 
			{
				currentFilter = fillnoiseFilter;
				InitFilter(*counts);
			}
		}
		else if ( currentFilter != standardFilter ) 
		{
				currentFilter = standardFilter;
				InitFilter(*counts);
		}

	}
	return MayerFilter(counts);	//run the selected filter
}
/*---------------------------------------------------------------------*
 * Name         : LOW_PASS_FILTER::InitFilter
 * Prototype in : j_filter.h
 * Description  : reset filter history to specified counts
 * Return value : None
 *---------------------------------------------------------------------*/
static void InitFilter( double initCounts ) 
{
	int i,j;
	for ( i=0; i < MAX_FILT_CELLS; i++ )
		for ( j = 0; j < 4; j++ )
			hist[i][j] = initCounts; 		//clear the filter memory
}

/*---------------------------------------------------------------------*
 * Name         : LOW_PASS_FILTER::SetLowPassFilterCornerFrequency
 * Syntax       : void LOW_PASS_FILTER::SetLowPassFilterCornerFrequency(
 *										double	freq,
 *										short	poles,
 *										double	weightUpdateRate,
 *										UCHAR 	*retFreq)
 * Prototype in : j_filter.h
 * Description  : cornerFrequency = weightUpdateRate * filterPercentage
 *				: Select the set of filter coefficients that most closely
 *				: matches the requested -3db corner frequency.
 * Return value : Return the selected corner frequency of the filter.
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
static void SetLowPassFilterCornerFrequency(double 	freq,
										short   poles,
										double	weightUpdateRate,
										double *retFreq,
										short *	retPoles )
{
	FILT_COEF *	coefs[3][16] =
		{  &filter_0,   &filter_2_4, &filter_2_4, &filter_3_4,
		   &filter_4_4, &filter_5_4, &filter_6_4, &filter_7_4,
		   &filter_8_4, &filter_9_4, &filter_10_4,&filter_11_4,
		   &filter_12_4,&filter_13_4,&filter_14_4,&filter_15_4,

		   &filter_0,   &filter_2_6, &filter_2_6, &filter_3_6,
		   &filter_4_6, &filter_5_6, &filter_6_6, &filter_7_6,
		   &filter_8_6, &filter_9_6, &filter_10_6,&filter_11_6,
		   &filter_12_6,&filter_13_6,&filter_14_6,&filter_15_6,

		   &filter_0,   &filter_2_8, &filter_2_8, &filter_3_8,
		   &filter_4_8, &filter_5_8, &filter_6_8, &filter_7_8,
		   &filter_8_8, &filter_9_8, &filter_10_8,&filter_11_8,
		   &filter_12_8,&filter_13_8,&filter_14_8,&filter_15_8 };

	double	corner;
	int 	ii,jj;

	if ( freq < .001 ) 
	{
		standardFilter = currentFilter = &filter_0;
		*retPoles	   = 0;
		*retFreq	   = 0.0;
	}
	else 
	{
		corner = freq;
		for ( ii = 0; ii < 16; ii++ )
			if (corner < weightUpdateRate*((double)ii+.49)*.01) break;
		if ( ii > 15) ii = 15;
		if ( poles < 5 ) {
			jj 		  = 0;
			*retPoles = 4;
		}
		else if ( poles < 7 ) {
			jj 		  = 1;
			*retPoles = 6;
		}
		else {
			jj 		  = 2;
			*retPoles = 8;
		}
		standardFilter = currentFilter = coefs[jj][ii];
		*retFreq = weightUpdateRate * (double)ii * .01;
	}
	//printf("FilterPercent=%d, Poles=%d, Corner=%4.1f, Rate*Percent=%4.1f, coefs=%p\n",
	//	ii,jj,corner,weightUpdateRate*ii*.01, standardFilter );
//	return SUCCESS;
}
/*---------------------------------------------------------------------*
 * Name         : LOW_PASS_FILTER::MayerFilter
 * Prototype in : j_filter.h
 * Description:	: If weight is in zero range, apply standard filter.
 *    			: Else if scale is not in motion, apply standard filter.
 *              : Else apply Fillnoise filter.
 *				: When switching filters, set filter history to
 *				: current raw count.
 *
 * Description  : Performs a standard digital floating point filter in
 *              : multiples of 2 poles.
 * Return value : Filtered floating point weight
 *---------------------------------------------------------------------*/
static double MayerFilter( double * cellin )	
{
	int  			c, loop_end;
	const double *	coefp;
	double * 		histp;
	double * 		cellout = &filteredOutput;

	if ((loop_end = (int)currentFilter->ncells) != 0) 
	{
											// filtering turned on
		coefp  = currentFilter->coef[0];	// pointer to coefficients
		histp  = hist[0];					// pointer to filter history

		for (c=0; c < loop_end; c++)  
		{      // compute 2 pole filter cell
			*cellout = (coefp[0] * (*cellin + histp[1] + histp[0] + histp[0])) -
			 		   (histp[2] * coefp[1]) - (histp[3] * coefp[2]);

			// shift history data */
			memcpy(&histp[1],&histp[0],8);	// histp[1] = histp[0]; 
			memcpy(&histp[0],cellin,8);		// histp[0] = *cellin;
			memcpy(&histp[3],&histp[2],8);	// histp[3] = histp[2];
			memcpy(&histp[2],cellout,8);	// histp[2] = cellout;
			memcpy(cellin,cellout,8);		// cellin = cellout;
			coefp += 3;                     // point to next cell data */
			histp += 4;
		}
	}
	else 
		memcpy( cellout, cellin, 8 );

	return *cellout;
}
