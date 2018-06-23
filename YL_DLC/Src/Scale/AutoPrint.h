#ifndef  _AUTO_PRINT_H
#define  _AUTO_PRINT_H

#include "Unit.h"

#define PRINT_THRESHOLD_NOT_ENABLED 0
#define	WAITING_FOR_PRINT_RESET		1
#define PRINT_RESET					2
#define	READY_TO_PRINT				3

#define AUTOPRINT_RESET_ONDEVIATION		0
#define AUTOPRINT_RESET_ONRESET     	1


#define OKtoPrint                        0x0218
#define OKtoPrintResponse                0x1218


typedef enum
{
	PRINTING_COMPLETED_SUCCESSFULLY			= 0,
	PRINTING_OPERATION_SUCCESS				= 0,
	PRINTING_IN_PROGRESS					= 1,
	PRINTING_CONNECTION_NOT_FOUND			= 2,
	PRINTING_BUSY							= 3,
	PRINTING_ERROR							= 4,
	PRINTING_NOT_READY_TO_PRINT				= 5,
	PRINTING_SCALE_IN_MOTION				= 6,
	PRINTING_SCALE_OVERCAPACITY				= 7,
	PRINTING_SCALE_UNDER_ZERO				= 8,
	PRINTING_REQUEST_ARMED					= 9,
	PRINTING_READY_TO_PRINT					= 10,
	PRINTING_SCALE_IN_EXPAND				= 11,
	PRINTING_SCALE_BAD_ZERO					= 12,
	INVALID_PRINT_FUNCTION_PARAMETER		= 13,
	CANNOT_ACCESS_PRINT_SD_TRIGGER			= 14,
	PRINTING_SCALE_TEMPLATE_MISMATCH        = 15,
    PRINTING_SCALE_ETHERNET_NOSOCKET        = 16,
    PRINTING_SD_CARD_NOT_INSTALLED          = 17,
} PRINT_SCALE_tStatus;

struct AutoPrintData
{
    struct ScaleData   *pScale;
    
    float primaryThresholdWeight;		    // threshold weight in primary units.
    float primaryResetThresholdWeight;	    // reset weight in primary units.
    float primaryDeviationThresholdWeight;  //deviation weight in primary uints
    float primaryMinPrintWeight; 
    float currentThresholdWeight;		    // threshold weight in current units.
    float currentResetThresholdWeight;	    // reset weight in current unit
    float currentDeviationThresholdWeight;  //deviation weight in current uints
    float currentMinPrintWeight; 
    float oldPrintWeight;
    UNIT_tType oldPrintUnitType;
    unsigned char checkMotion;              // TRUE = check motion
    unsigned char autoPrintEnabled;                // TRUE = auto print enabled
    unsigned char printInterlockEnabled;
    unsigned char demandPrintRequest;       // TRUE = request outstanding
    SETUP_AUTOPRINT_RESETMODE autoPrintResetMode;
    unsigned char thresholdState;	        // 1 = PRINT_WAITING_FOR_RESET
    								        // 2 = PRINT_RESET
    								        // 3 = READY_TO_PRINT
    unsigned char thresholdEnabled;
    unsigned char waitCount;
    //! Callback function for re-initialize data members
    void (*reInitializeDataMembers)(void *);
};

typedef struct AutoPrintData AUTO_PRINT;


void AUTOPRINT_Init(AUTO_PRINT *this);
void AUTOPRINT_InstallReInitialization(AUTO_PRINT *this, void (*pCallbackFunction)(void *));
void AUTOPRINT_ReInitialization(AUTO_PRINT *this);
void AUTOPRINT_Calibrate(AUTO_PRINT *this);
int AUTOPRINT_AutoPrint(AUTO_PRINT *this, float weight, UNIT_tType currUnitType,bool bMotion);

// PRINT_SCALE_tStatus AUTOPRINT_DemandPrint(AUTO_PRINT *this);
// unsigned char PRINT_THRESHOLD_StoreDemandPrintRequest(void);

#endif
