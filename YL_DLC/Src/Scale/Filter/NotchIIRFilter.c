/*-----------------------------------------------------------------------*
 *
 * Filename :  NotchIIRFilter.c
 *
 * Scope :  Excalibur filter control functions
 *
 * Function(s)	initialize_filter
 *				execute_filter
 *
 * Version        Date                   Description
 *   1.0       05-Sept-2003                   New
 *
 *      COPYRIGHT (C) 2003.  All Rights Reserved.
 *      Mettler Toledo, Worthington, Ohio,  USA.
 *
 *------------------------------------------------------------------------*/
#include "NotchIIRFilter.h"

//==================================================================================================
//  L O C A L   D E F I N I T I O N S
//==================================================================================================
//#define MAX_NOTCH_SAMPLE	            184
//#define MAX_FILTER_NO		            28
//#define DEFAULT_NOTCH_FILTER_FREQ		30.0

/***********************************************************************
filter percent choices is an indexed table, which gives the approximate
cutoff frequency in percent (cutoff_freq/sampling_freq) for each of the
low pass filters provided. Index 0 = no filter, higher indicies provide
stiffer filters. Cutoff frequency in Hz is provided in the comments,
assuming a sampling frequency of 366.0 Hz.
***********************************************************************/
const float filt_pct_choices[MAX_FILTER_NO+1] =
{
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

//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================
//static uint32_t notch_filter_type;		        // filter setup response
//static float notch_filter_frequency;		// filter setup response
//static uint32_t head_ptr, tail_ptr; 	            // fifo pointers
//static uint32_t notchSample;			            // # samples in notch filter
//static uint32_t notch_sum;				        // sum of notch filter frequency
//static uint16_t notchFifo[MAX_NOTCH_SAMPLE];     // notch filter sample history
//
//XLONG filcnt;		                        // working filter counts
////low-pass filtering pole history data
//static XLONG prev_y[5];
//static XLONG prev_v[5];
//static uint32_t widenShiftBits;
//static uint32_t halfpoles;			            // number of poles / 2
//static uint32_t numerator;			            // QCOEFF filtering routine
//static uint32_t kk;
//
//static uint32_t notch_filter_enabled = 0;
//static uint32_t lowpass_filter_enabled = 0;

//==================================================================================================
//  F O R W A R D   D E F I N I T I O N S
//==================================================================================================
static void notch_filter(FILTER *this);
static void init_fast_filter(FILTER *this, uint32_t filtno, uint32_t lowpass_poles);
static void lowpass_filter(FILTER *this);
static void add6b(XLONG *destination, XLONG *source);
static void sub6b(XLONG *destination, XLONG *source);
static void mult6b(XLONG *destination, uint32_t numerator);
static void twocom(XLONG *destination);
// static void addround(XLONG *destination, XLONG *source);

//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

/*------------------------------------------------------------------------*
 * Name:     FILTER_Init
 * Purpose:  initialize filter
 *	cornerFrequency = weightUpdateRate * filterPercentage
 * Returns:  nothing
 * ----------------------------------------------------------------------*/
void FILTER_Init(FILTER *this)
{
    // FILTER_InitNotchFilter(this, AVERAGER, 6, 30.0, 366.0);
    // FILTER_InitIirFilter(this, 8, 2.0, 366.0);
    // this->reInitializeDataMembers(this);
}

/*---------------------------------------------------------------------*
 * Name         : FILTER_InstallReInitialization
 * Prototype in : Motion.h
 * Description  : Callback function for re-initialize MOTION data members.
 *                This function will be installed from application.
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void FILTER_InstallReInitialization(FILTER *this, void (*pCallbackFunction)(void *))
{
	this->reInitializeDataMembers = pCallbackFunction;
}

/*------------------------------------------------------------------------*
 * Name:     FILTER_InitIirFilter
 * Purpose:  initialize iir filter
 *	cornerFrequency = weightUpdateRate * filterPercentage
 * Returns:  nothing
 * ----------------------------------------------------------------------*/
void FILTER_InitIirFilter(FILTER *this, uint32_t lowpass_poles, float lowpass_frequency, float melsiSampleFreq)
{
	float	tempfloat;
	uint32_t filtno;
    
    this->lowpass_filter_enabled = 1;
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
    if ((lowpass_frequency > 0.00001) && (lowpass_frequency < 9.89999))
    {
		tempfloat = 100.0 * lowpass_frequency / melsiSampleFreq;
		for (filtno = MAX_FILTER_NO; filtno > 0; filtno--)
		{
			if (tempfloat <= filt_pct_choices[filtno])
			    break;
		}

		if (filtno != 0)
		{
			if ( filtno < MAX_FILTER_NO)
				if ((filt_pct_choices[filtno]-tempfloat) > (tempfloat-filt_pct_choices[filtno+1]))
					filtno++;
			
			if (lowpass_poles < 2)
				lowpass_poles = 2;
			else if (lowpass_poles > 10) 
				lowpass_poles = 10;
		}
		else
		    lowpass_poles = 2;
	}
	else
	{
	    filtno = 0;			//no filter, but 32X gain included.
	    lowpass_poles = 2;
	}
	
	// widenShiftBits = widenLeftShiftBits;
	// init_notch_filter(notch_type, notch_frequency, melsiSampleFreq);
	init_fast_filter(this, filtno, lowpass_poles);
}

/*------------------------------------------------------------------------*
 * Name         : FILTER_Execute
 * Prototype in : exalc.h
 * Description  : execute filters and span adjust
 * Return value : Filtered Weight Reading
 *------------------------------------------------------------------------*/
uint32_t FILTER_Execute(FILTER *this, uint32_t ATDreading)
{
	// The assembly language mettler fast filter gets a fixed point number input:
	// IIII.FF
	// where: IIII    unsigned 32 bit integer
	//     	    FF    16 bit fraction
	// Melsi read routine returns 16 bits of A to D data.
	// It sets the upper two bytes of long IIII to 0.
	// Set decimal fraction FF to zero here
	this->filcnt.ul = ATDreading;
	this->filcnt.df = 0;

   	// notch filter widens ATD counts by five (32X)
   	if (this->notch_filter_enabled)
	    notch_filter(this);
	if (this->lowpass_filter_enabled)
	    lowpass_filter(this);

    return this->filcnt.ul;
}

//***************************************************************************
//Notch filter initialization code.
//
//input:	requested frequency of the first notch, in Hz
//			filter type (NONE, COMB, AVERAGER)
//			melsiSampleFreq		A/D sample rate, in Hz
//
//Output:	actual_freq      actual frequency of the first notch, in Hz.
//			(ie., what you actually get!)
//
//****************************************************************************
void FILTER_InitNotchFilter(FILTER *this, NOTCH_TYPE notch_type, uint32_t widenLeftShiftBits,
                       float notch_frequency, float melsiSampleFreq)
{
	uint32_t tempSample, kk;

    this->notch_filter_enabled = 1;
    this->widenShiftBits = widenLeftShiftBits;
    
	this->notch_filter_type = AVERAGER;   // DEFAULT NOTCH FILTER TYPE
	if (notch_frequency < 1.0)
		this->notch_filter_type = NO_NOTCH;

	this->notch_filter_frequency = DEFAULT_NOTCH_FILTER_FREQ;
	this->notchSample = (uint32_t)((melsiSampleFreq / this->notch_filter_frequency) + 0.5);

    this->tail_ptr = 0;
    this->head_ptr = this->tail_ptr + this->notchSample;
	for (kk = 0; kk < MAX_NOTCH_SAMPLE; kk++)
		this->notchFifo[kk] = 0;
	this->notch_sum = 0;

    switch (notch_type)
    {
	case NO_NOTCH:
	    return;				// SUCCESS;

	case COMB:
	    tempSample = (uint32_t)((0.5 * (melsiSampleFreq / notch_frequency + 1)) + 0.5);
	    this->notch_filter_frequency = melsiSampleFreq / (2.0 * (float)(tempSample - 1));

	    if ((tempSample < MAX_NOTCH_SAMPLE) && (tempSample > 0))
	    {
			this->notch_filter_type = COMB;
			this->notchSample = tempSample;
			this->head_ptr = this->tail_ptr + this->notchSample;
			return;			// SUCCESS;
	    }
	    else 
			return;			// FREQUENCY_OUT_OF_RANGE;

	case AVERAGER:
	    if (notch_frequency < (melsiSampleFreq / MAX_NOTCH_SAMPLE))
			return;			// FREQUENCY_OUT_OF_RANGE;

		tempSample = (uint32_t)((melsiSampleFreq / notch_frequency) + 0.5);
	    this->notch_filter_frequency = melsiSampleFreq / tempSample;

	    if (tempSample < MAX_NOTCH_SAMPLE)
	    {
			this->notch_filter_type = AVERAGER;
			this->notchSample = tempSample;
			this->head_ptr = this->tail_ptr + this->notchSample;
			return;			// SUCCESS;
	    }
	    else 
			return;			// FREQUENCY_OUT_OF_RANGE;

	default:
	    return;				// UNKNOWN_FILTER_TYPE;
    }
}

//***************************************************************************
// notch_filter run time code
//
//Input:   unsigned integer raw weight, from the A/D
//
//Returns: notch-filtered weight
//		   shifted left 5 to widen A to D count
//****************************************************************************/
static void notch_filter(FILTER *this)
{
	uint32_t ultemp;
	uint16_t ustemp;
	
	// save short filter counts in notch filter array
	ustemp = (uint16_t)(this->filcnt.ul & 0x0000ffff);
    this->notchFifo[this->head_ptr] = ustemp;
    this->head_ptr++;
    if (this->head_ptr >= MAX_NOTCH_SAMPLE)
		this->head_ptr = 0;

    switch (this->notch_filter_type)
    {
	case COMB:
		// shift right 1 to divide by 2
	    // widen ATD counts by left-shifting 5 bits (32X)
	    this->filcnt.ul = (this->filcnt.ul + this->notchFifo[this->tail_ptr] + 1) << (this->widenShiftBits - 1);
	    break;

	case AVERAGER:
		// calculate average filter value	
		// widen ATD counts by left-shifting 5 bits (32X)
	    this->notch_sum = this->notch_sum - this->notchFifo[this->tail_ptr] + ustemp;
		ultemp  = this->notch_sum << this->widenShiftBits;
		this->filcnt.ul = ultemp / this->notchSample;
	    break;

	default:
	    // widen ATD counts by left-shifting 5 bits (32X)
	    this->filcnt.ul <<= this->widenShiftBits;
	    return;
    }

    this->tail_ptr++;
    if (this->tail_ptr >= MAX_NOTCH_SAMPLE)
		this->tail_ptr = 0;
}

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

//***************************************************************************
// Initialize filter
//
//	lowpass_poles    number of poles (0-10)
//	filtNo           filter number (0-28)
//	0				 initial filter output
//
// returns       nothing
//***************************************************************************
static void init_fast_filter(FILTER *this, uint32_t filtno, uint32_t lowpass_poles)
{
    uint32_t kk;
    
    // Initialize the quantized filter subroutine settings.
    // This is how we change the filter cutoff frequency.
    // Qval  = filtno;
    // Rval  = filtno;
	switch (filtno)
	{
	case 1:
		// 128/256 = 1/2 = 0.5f
		// OP1 = (OP1+1) >> 1;
		this->numerator = 128;
		break;
	case 2:
		// 112/256 = 7/16 = 0.4375
		// OP1 *= 7;
		// OP1 = (OP1+8) >> 4;
		this->numerator = 112;
		break;
	case 3:
		// 96/256 = 3/8 = 0.375
		// OP1 *= 3;
		// OP1 = (OP1+4) >> 3;
		this->numerator = 96;
		break;
	case 4:
		// 80/256 = 5/16 = 0.3125
		// OP1 *= 5;
		// OP1 = (OP1+8) >> 4;
		this->numerator = 80;
		break;
	case 5:
		// 72/256 = 9/32 = 0.28125
		// OP1 *= 9;
		// OP1 = (OP1+16) >> 5;
		this->numerator = 72;
		break;
	case 6:
		// 64/256 = 1/4 = 0.25
		// OP1 = (OP1+2) >> 2;
		this->numerator = 64;
		break;
	case 7:
		// 56/256 = 7/32 = 0.21875
		//  OP1 *= 7;
		//  OP1 = (OP1+16) >> 5;
		this->numerator = 56;
		break;
	case 8:
		// 48/256 = 3/16 = 0.1875
		// OP1 *= 3;
		// OP1 = (OP1+8) >> 4;
		this->numerator = 48;
		break;
	case 9:
		// 40/256 = 5/32 = 0.15625
		// OP1 *= 5;
		// OP1 = (OP1+16) >> 5;
		this->numerator = 40;
		break;
	case 10:
		// 36/256 = 9/64 = 0.140625
		// OP1 *= 9;
		// OP1 = (OP1+32) >> 6;
		this->numerator = 36;
		break;
	case 11:
		// 32/256 = 1/8 = 0.125
		// OP1 = (OP1+4) >> 3;
		this->numerator = 32;
		break;
	case 12:
		// 28/256 = 7/64 = 0.109375
		// OP1 *= 7;
		// OP1 = (OP1+32) >> 6;
		this->numerator = 28;
		break;
	case 13:
		// 24/256 = 3/32 = 0.09375
		// OP1 *= 3;
		// OP1 = (OP1+16) >> 5;
		this->numerator = 24;
		break;
	case 14:
		// 20/256 = 5/64 = 0.078125
		// OP1 *= 5;
		// OP1 = (OP1+32) >> 6;
		this->numerator = 20;
		break;
	case 15:
		// 18/256 = 9/128 = 0.0703125
		// OP1 *= 18;
		// OP1 = (OP1+128) >> 8;
		this->numerator = 18;
		break;
	case 16:
		// 16/256 = 1/16 = 0.0625
		// OP1 = (OP1+8) >> 4;
		this->numerator = 16;
		break;
	case 17:
		// 14/256 = 7/128 = 0.0546875
		// OP1 *= 14;
		// OP1 = (OP1+128) >> 8;
		this->numerator = 14;
		break;
	case 18:
		// 12/256 = 3/64 = 0.046875
		// OP1 *= 3;
		// OP1 = (OP1+32) >> 6;
		this->numerator = 12;
		break;
	case 19:
		// 10/256 = 5/128 = 0.0390625
		// OP1 *= 10;
		// OP1 = (OP1+128) >> 8;
		this->numerator = 10;
		break;
	case 20:
		// 9/256 = 0.03515625
		// OP1 *= 9;
		// OP1 = (OP1+128) >> 8;
		this->numerator = 9;
		break;
	case 21:
		// 8/256 = 1/32 = 0.03125
		// OP1 = (OP1+16) >> 5;
		this->numerator = 8;
		break;
	case 22:
		// 7/256 = 0.02734375
		// OP1 *= 7;
		// OP1 = (OP1+128) >> 8;
		this->numerator = 7;
		break;
	case 23:
		// 6/256 = 3/128 = 0.0234375
		// OP1 *= 6;
		// OP1 = (OP1+128) >> 8;
		this->numerator = 6;
		break;
	case 24:
		// 5/256 = 0.01953125
		// OP1 *= 5;
		// OP1 = (OP1+128) >> 8;
		this->numerator = 5;
		break;
	case 25:
		// 4/256 = 1/64 = 0.015625
		// OP1 = (OP1+32) >> 6;
		this->numerator = 4;
		break;
	case 26:
		// 3/256 = 0.01171875
		// OP1 *= 3;
		// OP1 = (OP1+128) >> 8;
		this->numerator = 3;
		break;
	case 27:
		// 2/256 = 1/128 = 0.0078125
		// OP1 = (OP1+64) >> 7;
		this->numerator = 2;
		break;
	case 28:
		// 1/256 = 0.00390625
		// OP1 = (OP1+128) >> 8;
		this->numerator = 1;
		break;
	default:
		// no-op
		this->numerator = 255;
		break;
	}

    // Initialize the memory areas owned by each cell.
    // Set the previous output = the caller's output
    // Set the previous derivative = 0.
    for (kk = 0; kk < 5; kk++)
    {
		this->prev_y[kk].ul = 0;			//initial previous output
		this->prev_y[kk].df = 0;			//initial previous output
		this->prev_v[kk].ul = 0;	  		//previous derivative
		this->prev_v[kk].df = 0;			//initial previous derivative output
    }
	
	// Initialize number of halfpoles
	this->halfpoles = lowpass_poles / 2;
}

//***************************************************************************
// filter runtime code starts here

//long long lowpass_filter(long long x) {
//    int i;
//    long v;
//
//    for ( i = 0; i < poles/2; i++ ) {
//		v = (*CoefficientRtn[Qval])(x-fil.prev_y[i]+fil.prev_v[i]);   	
//		v = fil.prev_v[i]+(*CoefficientRtn[Rval])(v-fil.prev_v[i]-fil.prev_v[i]); 
//		y = fil.prev_y[i] + v;
//		fil.prev_v[i] = v;
//		fil.prev_y[i] = y;
//		x = y;
//    }
//    return x + (long long)0x80000000;
//}
//***************************************************************************
// Filter testing
//
// test 1 digit increment and 0xffff increment & make sure the 
// filter passes it through.  There are a finite number of steps
// to reach the final value. 
//
//***************************************************************************
static void lowpass_filter(FILTER *this)
{
    uint32_t kk;
    
    for (kk = 0; kk < this->halfpoles; kk++)
    {
		sub6b(&(this->filcnt), &(this->prev_y[kk]));		    // op1 = op1 - y
		add6b(&(this->filcnt), &(this->prev_v[kk]));		    // op1 = op1 + v
		if ((long)(this->filcnt.ul) >= 0)                       // positive counts
			mult6b(&(this->filcnt), this->numerator);           // source & result in op1
		else 
		{					
			twocom(&(this->filcnt));			                // negative counts
			mult6b(&(this->filcnt), this->numerator);           // source & result in op1
			twocom(&(this->filcnt));
		}

		// v = prev_v[kk]+(*CoefficientRtn[Rval])(op1-prev_v[kk]-prev_v[kk]); 
		sub6b(&(this->filcnt), &(this->prev_v[kk]));		    // op1 = op1 - v
		sub6b(&(this->filcnt), &(this->prev_v[kk]));		    // op1 = op1 - v
		if ((long)(this->filcnt.ul) >= 0)                       // positive counts
			mult6b(&(this->filcnt), this->numerator);           // source & result in op1
		else 
		{					
			twocom(&(this->filcnt));			                // negative counts
			mult6b(&(this->filcnt), this->numerator);           // source & result in op1
			twocom(&(this->filcnt));
		}
		add6b(&(this->prev_v[kk]), &(this->filcnt));		    // v = v + op1
		add6b(&(this->prev_y[kk]), &(this->prev_v[kk]));	    // y = y + v
		this->filcnt.ul = this->prev_y[kk].ul;
		this->filcnt.df = this->prev_y[kk].df;
    }
    // add rounding: filcnt = op1 + .5
	// addround(&filcnt, &filcnt);
    this->filcnt.df = this->filcnt.df + 0x8000;
    if (this->filcnt.df < this->filcnt.df)
        this->filcnt.ul++;
}

// calculation subroutines
static void add6b(XLONG *destination, XLONG *source) 
{
    destination->df = destination->df + source->df;
    if (destination->df < source->df)
    	destination->ul++;
    destination->ul = destination->ul + source->ul;
}

static void sub6b(XLONG *destination, XLONG *source) 
{
    if (destination->df < source->df)
    	destination->ul--;
    destination->df = destination->df - source->df;
    destination->ul = destination->ul - source->ul;
}

static void mult6b(XLONG *destination, uint32_t numerator) 
{
	uint32_t temp1, temp2;
	
	temp1 = (uint32_t)(destination->df) * numerator + 0x00000080;
	temp2 = (uint32_t)(destination->ul) * numerator;
	temp2 = temp2 + (temp1 >> 16);
	destination->ul = temp2 >> 8;
	destination->df = ((temp2 & 0x000000ff) << 8) + ((temp1 & 0x0000ffff) >> 8);
}

static void twocom(XLONG *destination)
{
	destination->ul = ~destination->ul;
	destination->df = ~destination->df + 1;
	if (destination->df == 0)
		destination->ul++;
}

// static void addround(XLONG *destination, XLONG *source) 
// {
//    destination->df = source->df + 0x8000;
//    destination->ul = source->ul;
//    if (destination->df < source->df)
//    	destination->ul++;
// }
