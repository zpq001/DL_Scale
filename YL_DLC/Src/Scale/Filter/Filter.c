//****************************************************************************
// Name:         Fast Mettler Quantized Low Pass IIR Filter
//
// Description:
//
// Each cell of this filter provides a pair of identical real poles, located
// at z = (1-r), where r = a quantized fractional factor. For this filter,
// the quantized factors q and r are equal, and are quantized in units of
// 1/256. As the numerator gets smaller, the filter gets stiffer.
//
// The general formula being executed is as follows:
//
//      v(k) = v(k-1) + r * ( q * [ x(k) - y(k-1) +v(k-1)] - 2v(k-1) )
//      y(k) = y(k-1) + v(k)
//
//  where:     x(k)   = cell input
//             y(k)   = cell output
//             y(k-1) = cell previous output
//             v(k)   = cell output derrivative value
//             v(k-1) = cell previous output derrivative value
//
//  Input:     x  	filter input
//
//  Output:    same as input
//
//
//  Entry Points:       init_fast_filter
//                      lowpass_filter
//
//  Notes:
//      1. The low (fraction) word is 16 bits and high (integer) word 2
//		   is 32 bits. The valid input data range is an unsigned 32 bits.
//         This is OK since the MeLSI presents only positive 16-bits.
//         counts. Also, the minimum MeLSI output is several thousand counts
//         above zero since its comparator circuit will not operate with
//         signals having zero slope.
//      2. More resolution is available at the output than was presented
//         to the input. Output resolution is (almost) an unsigned 21 bits,
//         00000000h - 001fffe0h (2,097,120). It is theoretically possible
//         (though pretty unlikely in a real system) for the filter output
//         to go negative since the input is always positive and >> 0 by some
//         offset. Again, the fraction word is not meaningful at the output.
//
// Version        Date                   Description
//   1.0       05-Sept-2003                   New
//
//  Copyright (c) Mettler Toledo, Inc.  2003
//
//**************************************************************************
//#include "sd_index.h"

#include "UserParam.h"
#include <stdint.h>

#define MAX_NOTCH_SAMPLE	40  // 陷波器阶数，这里滑动平均缓冲长度，通知和采样频率配套使用，
#define MAX_FILTER_NO		28   //

// *****************************************
// ********** GLOBAL DEFAULTS **************
// *****************************************

//*********** notch filter types ***********
#define NO_NOTCH						0
#define COMB							1
#define AVERAGER						2



#define DEFAULT_LOWPASS_FILTER_FREQ		5//2 //低通截止頻率
#define DEFAULT_LOWPASS_FILTER_POLES	4 //8   //极点数,/2滤波器节数问题
#define DEFAULT_NOTCH_FILTER_TYPE		2   // 陷波方法，
#define DEFAULT_NOTCH_FILTER_FREQ		30.0 //陷波频率，这里是工频 50hz 或者60hz

#define MELSI_SAMPLING_FREQ				80.0


//xht
//#define DAGGER_EX                        1
//******************************************
//********** FILTER STRUCTURES *************
//******************************************

//***************************************************/
// P R O T O T Y P E S
//**************************************************/

void			initialize_filter(void);
double			execute_filter(unsigned long ATDreading);
void			init_fast_filter(void);
double			lowpass_filter(double);
void			init_notch_filter(char notch_type);
void			notch_filter(void);

static double			filcnt;				// working filter counts
//static double			FILreading;				// Filtered Weight Reading

//low-pass filtering pole history data
static double prev_y[5];
static double prev_v[5];

static char			filtno;				// filter number
static char			halfpoles;			// number of poles / 2
static double		numerator;			// QCOEFF filtering routine

//static short			runningReportingCount LRAM1;
static unsigned char	notch_filter_type;		// filter setup response
static unsigned char	head_ptr, tail_ptr; 	// fifo pointers
static unsigned char	notchSample;			// # samples in notch filter
static double	notch_sum ;				// sum of notch filter frequency
static unsigned char	kk;			// general-purpose index


static double	notchFifo[MAX_NOTCH_SAMPLE];// notch filter sample history

static char			lowpass_poles;				// filter setup response



/*-----------------------------------------------------------------------*
 *
 * Filename :  filter.c													 */
/***********************************************************************
filter percent choices is an indexed table, which gives the approximate
cutoff frequency in percent (cutoff_freq/sampling_freq) for each of the
low pass filters provided. Index 0 = no filter, higher indicies provide
stiffer filters. Cutoff frequency in Hz is provided in the comments,
assuming a sampling frequency of 366.0 Hz.
***********************************************************************/
extern const float filt_pct_choices[MAX_FILTER_NO+1]
= {
	  10.00,    /*  0  no filter, but 32X gain included. */
	   7.96,    /*  1  29.13 Hz */
	   6.96,    /*  2  25.47 Hz */
	   5.97,    /*  3  21.85 Hz */
	   4.97,    /*  4  18.19 Hz */
	   4.48,    /*  5  16.40 Hz */
	   3.98,    /*  6  14.57 Hz */
	   3.48,    /*  7  12.74 Hz */
	   2.98,    /*  8  10.91 Hz */
	   2.49,    /*  9   9.11 Hz */
	   2.24,    /* 10   8.20 Hz */
	   1.99,    /* 11   7.28 Hz */
	   1.74,    /* 12   6.39 Hz */
	   1.49,    /* 13   5.45 Hz */
	   1.24,    /* 14   4.54 Hz */
	   1.12,    /* 15   4.10 Hz */
	   0.99,    /* 16   3.62 Hz */
	   0.87,    /* 17   3.18 Hz */
	   0.75,    /* 18   2.75 Hz */
	   0.62,    /* 19   2.27 Hz */
	   0.56,    /* 20   2.05 Hz */
	   0.50,    /* 21   1.83 Hz */
	   0.44,    /* 22   1.61 Hz */
	   0.37,    /* 23   1.35 Hz */
	   0.31,    /* 24   1.13 Hz */
	   0.25,    /* 25   0.92 Hz */
	   0.19,    /* 26   0.70 Hz */
	   0.12,    /* 27   0.44 Hz */
	   0.06     /* 28   0.22 Hz */
};

//***************************************************************************
//Initialize filter
//
//	lowpass_poles    number of poles (0-10)
//	filtNo           filter number (0-28)
//	0				 initial filter output
//
//returns       nothing
//***************************************************************************


void init_fast_filter()
{
    // Initialize the quantized filter subroutine settings.
    // This is how we change the filter cutoff frequency.
    // Qval  = filtno;
    // Rval  = filtno;
	switch ( filtno )
	{
		case 1:	numerator = 0.5;	break;	// 128/256 = 1/2 = 0.5f, 		OP1 = (OP1+1) >> 1;
		case 2:	numerator = 0.4375;	break;	// 112/256 = 7/16 = 0.4375, 	OP1 *= 7;, 	OP1 = (OP1+8) >> 4;
		case 3:	numerator = 0.375;		break;	// 96/256 = 3/8 = 0.375, 		OP1 *= 3; 	OP1 = (OP1+4) >> 3;
		case 4:	numerator = 0.3125;		break;	// 80/256 = 5/16 = 0.3125, 		OP1 *= 5; 	OP1 = (OP1+8) >> 4;
		case 5:	numerator = 0.28125;		break;	// 72/256 = 9/32 = 0.28125, 	OP1 *= 9; 	OP1 = (OP1+16) >> 5;
		case 6:	numerator = 0.25;		break;	// 64/256 = 1/4 = 0.25, 		OP1 = (OP1+2) >> 2;
		case 7:	numerator = 0.21875;		break;	// 56/256 = 7/32 = 0.21875, 	OP1 *= 7; 	OP1 = (OP1+16) >> 5;
		case 8:	numerator = 0.1875;		break;	// 48/256 = 3/16 = 0.1875, 		OP1 *= 3; 	OP1 = (OP1+8) >> 4;
		case 9:	numerator = 0.15625;		break;	// 40/256 = 5/32 = 0.15625, 	OP1 *= 5; 	OP1 = (OP1+16) >> 5;
		case 10:numerator = 0.140625;		break;	// 36/256 = 9/64 = 0.140625,	OP1 *= 9; 	OP1 = (OP1+32) >> 6;
		case 11:numerator = 0.125;		break;	// 32/256 = 1/8 = 0.125,		OP1 = (OP1+4) >> 3;
		case 12:numerator = 0.109375;		break;	// 28/256 = 7/64 = 0.109375,	OP1 *= 7; 	OP1 = (OP1+32) >> 6;
		case 13:numerator = 0.09375;		break;	// 24/256 = 3/32 = 0.09375, 	OP1 *= 3; 	OP1 = (OP1+16) >> 5;
		case 14:numerator = 0.078125;		break;	// 20/256 = 5/64 = 0.078125,	OP1 *= 5; 	OP1 = (OP1+32) >> 6;
		case 15:numerator = 0.0703125;		break;	// 18/256 = 9/128 = 0.0703125, 	OP1 *= 18; 	OP1 = (OP1+128) >> 8;
		case 16:numerator = 0.0625;		break;	// 16/256 = 1/16 = 0.0625, 		OP1 = (OP1+8) >> 4;
		case 17:numerator = 0.0546875;		break;	// 14/256 = 7/128 = 0.0546875,	OP1 *= 14;	OP1 = (OP1+128) >> 8;
		case 18:numerator = 0.046875;		break;	// 12/256 = 3/64 = 0.046875,	OP1 *= 3;	OP1 = (OP1+32) >> 6;
		case 19:numerator = 0.0390625;		break;	// 10/256 = 5/128 = 0.0390625,	OP1 *= 10;	OP1 = (OP1+128) >> 8;
		case 20:numerator = 0.03515625;		break;	// 9/256 = 0.03515625,			OP1 *= 9;	OP1 = (OP1+128) >> 8;
		case 21:numerator = 0.03125;		break;	// 8/256 = 1/32 = 0.03125,		OP1 = (OP1+16) >> 5;
		case 22:numerator = 0.02734375;		break;	// 7/256 = 0.02734375,			OP1 *= 7;	OP1 = (OP1+128) >> 8;
		case 23:numerator = 0.0234375;		break;	// 6/256 = 3/128 = 0.0234375,	OP1 *= 6;	OP1 = (OP1+128) >> 8;
		case 24:numerator = 0.01953125;		break;	// 5/256 = 0.01953125,			OP1 *= 5;	OP1 = (OP1+128) >> 8;
		case 25:numerator = 0.015625;		break;	// 4/256 = 1/64 = 0.015625,		OP1 = (OP1+32) >> 6;
		case 26:numerator = 0.01171875;		break;	// 3/256 = 0.01171875,			OP1 *= 3;	OP1 = (OP1+128) >> 8;
		case 27:numerator = 0.0078125;		break;	// 2/256 = 1/128 = 0.0078125,	OP1 = (OP1+64) >> 7;
		case 28:numerator = 0.00390625;		break;	// 1/256 = 0.00390625,			OP1 = (OP1+128) >> 8;
		default:numerator = 1;	break;	// no-op
	}
		
    // Initialize the memory areas owned by each cell.
    // Set the previous output = the caller's output
    // Set the previous derivative = 0.

    for ( kk = 0; kk < 5; kk++ )
    {
		prev_y[kk] = 0;			//initial previous output
		prev_v[kk] = 0;			//initial previous derivative output
    }
	// Initialize number of halfpoles
	halfpoles = lowpass_poles / 2;
}



//***************************************************************************
//Notch filter initialization code.
//
//input:	requested frequency of the first notch, in Hz
//			filter type (NONE, COMB, AVERAGER)
//			MELSI_SAMPLING_FREQ		A/D sample rate, in Hz
//
//Output:	actual_freq      actual frequency of the first notch, in Hz.
//			(ie., what you actually get!)
//
//****************************************************************************
void init_notch_filter(char notch_type)
{
  // 目前固定
	double 	notch_frequency = 30.0;
	short 	tempSample;
	float 	notch_filter_frequency;		// filter setup response

	notch_filter_type = DEFAULT_NOTCH_FILTER_TYPE;
	notch_filter_frequency = DEFAULT_NOTCH_FILTER_FREQ;
	notchSample = (short)(((float)MELSI_SAMPLING_FREQ / notch_filter_frequency ) + 0.5);

    tail_ptr = 0;
	head_ptr = tail_ptr + notchSample;
	for (kk=0; kk < MAX_NOTCH_SAMPLE; kk++)
		notchFifo[kk] = 0;
	notch_sum = 0;	

//	sd_get(&notch_frequency,DI_cs0116); // 虑的干扰波 通常工频干扰25 30 
	if(notch_frequency<1)
		notch_filter_type = NO_NOTCH;

    switch (notch_type )	
    {

		case NO_NOTCH:
	    	return;				// SUCCESS;

		case COMB:
	    	tempSample = (short)((0.5 * (MELSI_SAMPLING_FREQ/notch_frequency+1)) + 0.5);
	    	if ((tempSample < MAX_NOTCH_SAMPLE) && (tempSample > 0))	
	    	{
				notch_filter_type = COMB;
				notchSample = tempSample;
		    	notch_filter_frequency = MELSI_SAMPLING_FREQ/(2.0*(float)(tempSample-1));
				head_ptr = tail_ptr + notchSample;
				return;			// SUCCESS;
	    	}
	    	else
				return;			// FREQUENCY_OUT_OF_RANGE;


		case AVERAGER:
	    	if ( notch_frequency < (float)MELSI_SAMPLING_FREQ/MAX_NOTCH_SAMPLE )
				return;			// FREQUENCY_OUT_OF_RANGE;
			
			tempSample = (short)((MELSI_SAMPLING_FREQ / notch_frequency) + 0.5);
	    	if ( notchSample < MAX_NOTCH_SAMPLE )	
	    	{
				notch_filter_type = AVERAGER;
				notchSample = tempSample;
		    	notch_filter_frequency = (float)MELSI_SAMPLING_FREQ / tempSample;
				head_ptr = tail_ptr + notchSample;
				return;			// SUCCESS;
	    	}
	    	else
				return;			// FREQUENCY_OUT_OF_RANGE;

		default:
	    	return;				// UNKNOWN_FILTER_TYPE;
    }
}


/*------------------------------------------------------------------------*
 * Name:     low_pass::initialize
 * Purpose:  initialize filter
 *	cornerFrequency = weightUpdateRate * filterPercentage
 * Returns:  nothing
 * ----------------------------------------------------------------------*/
void initialize_filter(void)
{
	float	tempfloat;
	double 			LowPassFreq;
	unsigned char 	LowPassPoles = 8;


	lowpass_poles = DEFAULT_LOWPASS_FILTER_POLES;
    //******************************************************************
    //Initialize the Mettler Fast Low Pass IIR Filter
    //
    //Input:  	cutoff_freq     requested -3db corner frequency
    //			poles           requested number of poles
    //			sampling_freq   sampling frequency in Hz
    //			initial_output  the filter output to start with
    //
    //Output: actual_freq -3db corner frequency closest to the requested
    //			(ie. what you are going to get!)
    //
    //*******************************************************************
    USER_PARAM_Get(BLK0_setupLowPassFilter,   (uint8_t*)&(LowPassFreq)); 
//	sd_get(&LowPassFreq,DI_cs0114); // 截止频率0.1--9.9
    if(LowPassFreq<0.1||LowPassFreq>9.9)
    {
      LowPassFreq = 2.0;
      USER_PARAM_Set(BLK0_setupLowPassFilter,   (uint8_t*)&(LowPassFreq)); 
    }
     USER_PARAM_Get(BLK0_setupFilterPols,   (uint8_t*)&(LowPassPoles)); 
    if(LowPassPoles<1||LowPassPoles>8)
    {
      LowPassPoles = 8;
      USER_PARAM_Set(BLK0_setupFilterPols,   (uint8_t*)&(LowPassPoles)); 
    }
//	sd_get(&LowPassPoles,DI_cs0115); // 阶数1 2 4 8
//	if(LowPassFreq!=0)
	if(LowPassFreq> 0 && LowPassFreq <9.9)//change 9.9Hz to no filter according the new requement in Test Direct ,2007-1-25 8:50
   	{
		tempfloat = 100.0 * LowPassFreq/ MELSI_SAMPLING_FREQ;
		for ( filtno = MAX_FILTER_NO; filtno > 0; filtno-- )
		{
			if (tempfloat <= filt_pct_choices[filtno])
				break;
		}

		if (filtno != 0)
		{
			if ( filtno < MAX_FILTER_NO)
			{
				if ((filt_pct_choices[filtno]-tempfloat) > (tempfloat-filt_pct_choices[filtno+1]))
					filtno++;
			}
			
			if( LowPassPoles < 2 )
				lowpass_poles = 2;
			else if(LowPassPoles > 10 )
				lowpass_poles = 10;
			else
				lowpass_poles = LowPassPoles;
		}
	}
	else
		filtno = 0;			//no filter, but 32X gain included.
	init_fast_filter();	
	init_notch_filter(DEFAULT_NOTCH_FILTER_TYPE);
}


//***************************************************************************
// notch_filter run time code
//
//Input:   unsigned integer raw weight, from the A/D
//
//Returns: notch-filtered weight
//		   shifted left 5 to widen A to D count
//****************************************************************************/
void notch_filter (void)
{
	double ultemp;
	
	// save short filter counts in notch filter array
    notchFifo[head_ptr] = filcnt;
    head_ptr++;
    if ( head_ptr >= (unsigned char)MAX_NOTCH_SAMPLE )
		head_ptr = 0;

	switch (notch_filter_type)
	{

		case COMB:
			// shift right 1 to divide by 2
	    	// widen ATD counts by left-shifting 5 bits (32X)
#ifdef DAGGER_EX
	    	filcnt = (filcnt+ notchFifo[tail_ptr] + 1) *32;
#else	    	
	    	filcnt = (filcnt + notchFifo[tail_ptr] + 1) *16;
#endif	    	
	    	break;

		case AVERAGER:
			// calculate average filter value	
			// widen ATD counts by left-shifting 5 bits (32X)
		    notch_sum = notch_sum - notchFifo[tail_ptr] + filcnt;
#ifdef DAGGER_EX
			ultemp  = notch_sum *64;
#else
			ultemp  = notch_sum *32;
#endif			
			filcnt = ultemp / notchSample;
	    	break;

		default:
	    	// widen ATD counts by left-shifting 5 bits (32X)
#ifdef DAGGER_EX
		    filcnt*=64;
#else	    	
		    filcnt*=32;
#endif		
		    return;
    }

    tail_ptr++;
    if ( tail_ptr >= (unsigned char)MAX_NOTCH_SAMPLE )
		tail_ptr = 0;
}




//***************************************************************************

// filter runtime code starts here


double lowpass_filter( double x) 
{
    int i;
    double v,y;

    for ( i = 0; i < halfpoles; i++ )
    {
		v = (numerator)*(x-prev_y[i]+prev_v[i]);   	
		v = prev_v[i]+(numerator)*(v-prev_v[i]-prev_v[i]);
		y = prev_y[i] + v;
		prev_v[i] = v;
	    prev_y[i] = y;
		x = y;
    }
    return x;
}

//double Test_lowpass_filter( double x) 
//{
//    int i;
//    double v,y;
//
//    for ( i = 0; i < 2; i++ )
//    {
//		v = (0.0378047541709)*(x-prev_y[i]+prev_v[i]);   	
//		v = prev_v[i]+(0.0378047541709)*(v-prev_v[i]-prev_v[i]);
//		y = prev_y[i] + v;
//		prev_v[i] = v;
//	    prev_y[i] = y;
//		x = y;
//    }
//    return x;
//}





/*------------------------------------------------------------------------*
 * Name         : filter_execute
 * Prototype in : exalc.h
 * Description  : execute filters and span adjust
 * Return value : none
 *------------------------------------------------------------------------*/

 double execute_filter(unsigned long ATDreading)
{
//	unsigned long ltemp;


	// The assembly language mettler fast filter gets a fixed point number input:
	// IIII.FF
	// where: IIII    unsigned 32 bit integer
	//     	    FF    16 bit fraction
	// Melsi read routine returns 16 bits of A to D data.
	// It sets the upper two bytes of long IIII to 0.
	// Set decimal fraction FF to zero here
	filcnt	= ATDreading;


   	// notch filter widens ATD counts by five (32X)
	notch_filter();
	return lowpass_filter(filcnt);
//    FILreading = filcnt;Test_lowpass_filter(filcnt);//
}



