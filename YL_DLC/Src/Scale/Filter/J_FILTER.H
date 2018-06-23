#ifndef _J_FILTER_H
#define _J_FILTER_H

#include "Zero.h"
#include "NotchIIRFilter.h"

#define MAX_FILT_CELLS              4
#define FILLNOISE_MOTION_READINGS   20

typedef const struct 
{     						            // filter coefficient structure
	char   ncells;						// number of 2 pole cells in filter
	double  coef[MAX_FILT_CELLS][3];     // coefficients for the filter
} 	FILT_COEF;

struct JFilterData
{
    FILT_COEF *		currentFilter;              // pointer to current filter coefficient structure
    FILT_COEF *	    standardFilter;
    FILT_COEF *		fillnoiseFilter;
    double           hist[MAX_FILT_CELLS][4];    // filter history storage
    float			filteredOutput;
    
    long            fillnoise_motion_buffer[FILLNOISE_MOTION_READINGS];
    int             fillnoise_motion_ptr;
    long	        fillnoise_motion_counts;
    long            fillnoise_zero_counts;
    int				filterPercent;   			// filter percentage
    float			weightReadingRate;          // specified weight update rate
    short			filterPoles;
    unsigned char   fillnoise_filter_switch;
    //! Callback function for re-initialize data members
    void (*reInitializeDataMembers)(void *);
};

typedef struct JFilterData JFILTER;

//extern JFILTER g_jfilterdata;
//
//void JFILTER_Init(JFILTER *this, float frequency, short poles,
//                  unsigned char fillNoise, float weightUpdateRate, float initialCounts);
//void JFILTER_CalibrateStabilityFilter(JFILTER *this, float span_factor);
//float JFILTER_FilterWeight(JFILTER *this, ZERO *pZero, long counts);
//void FilterReInit(FILTER *pFilter, JFILTER *pJFilter);

void StabilityFilterInit(double weightUpdateRate,double initialCounts);
void CalibrateStabilityFilter(double span_factor);
double  FilterWeight(double * counts);


#endif
