#ifndef _NOTCH_IIR_FILTER
#define _NOTCH_IIR_FILTER

//============================================================================
//  I N C L U D E D   F I L E S
//============================================================================
//#include "RB_Type.h"

#include <stdint.h>

#define MAX_NOTCH_SAMPLE	           80// 184
#define MAX_FILTER_NO		            28
#define DEFAULT_NOTCH_FILTER_FREQ		30.0

typedef struct
{
    uint32_t ul;	// long integer filter counts
    uint16_t df;	// fractional filter counts
} XLONG;

typedef enum
{
    NO_NOTCH = 0,
    COMB,
    AVERAGER
} NOTCH_TYPE;

struct FilterData
{
    uint32_t notch_filter_type;		        // filter setup response
    float notch_filter_frequency;		// filter setup response
    uint32_t head_ptr, tail_ptr; 	            // fifo pointers
    uint32_t notchSample;			            // # samples in notch filter
    uint32_t notch_sum;				        // sum of notch filter frequency
    uint16_t notchFifo[MAX_NOTCH_SAMPLE];     // notch filter sample history
    
    XLONG filcnt;		                        // working filter counts
    //low-pass filtering pole history data
    XLONG prev_y[5];
    XLONG prev_v[5];
    uint32_t widenShiftBits;
    uint32_t halfpoles;			            // number of poles / 2
    uint32_t numerator;			            // QCOEFF filtering routine
    // uint32_t kk;
    
    uint32_t notch_filter_enabled;
    uint32_t lowpass_filter_enabled;
    //! Callback function for re-initialize data members
    void (*reInitializeDataMembers)(void *);
};

typedef struct FilterData FILTER;

extern FILTER g_filterdata;

void FILTER_Init(FILTER *this);
void FILTER_InstallReInitialization(FILTER *this, void (*pCallbackFunction)(void *));
void FILTER_InitNotchFilter(FILTER *this, NOTCH_TYPE notch_type, uint32_t widenLeftShiftBits, float notch_frequency, float melsiSampleFreq);
void FILTER_InitIirFilter(FILTER *this, uint32_t lowpass_poles, float lowpass_frequency, float melsiSampleFreq);
uint32_t FILTER_Execute(FILTER *this, uint32_t ATDreading);

#endif // _NOTCH_IIR_FILTER
