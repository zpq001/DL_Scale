//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		WP_State.h
//! \ingroup	util
//! \brief		System-information.
//!
//! This module contains different state-informations of weighing package.
//! Rules to supervise all state changes of WP_State. The are three different methods to track the
//! correct state:
//! 1. Method: After detecting the WP_STATE as event source, get all states from WP_State.
//! 2. Method: Track all states locally and modify them according to the incoming events. These
//!            locally state will be correct only when no events are lost. Care should be taken when
//!            adjustments starts. In this case some states are reset but no event for change of
//!            WREQU or WSTATE change are sent.
//! 3. Method: Get the actual information whenever needed direct from WP_State without subscribe to
//!            its event.
//!
//! Method 1 is more rugged when an event is lost or in special abort or start situations. Method 1
//! can also be used with a short event queue where some events are lost.
//! Method 3 may be a favourite for periodic update of a display.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Pius Derungs, Martin Heusser
//
// $Date: 2017/09/26 13:15:22MESZ $
// $Revision: 1.100 $
//
//==================================================================================================

#ifndef _WP_State__h
#define _WP_State__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_WP) && (RB_CONFIG_USE_WP == RB_CONFIG_YES)

#include "WP_Typedefs.h"
#include <stddef.h>
#include "RB_Typedefs.h"
#include "RB_OS.h"


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

//! Definition of LFT seal breaking configuration bits (see also definition of XP0333)
#define WP_STATE_LFT_SEAL_CONFIG_BREAK_EXT_CAL_ALLOWED	0x01	//!< External calibration does not break seal
#define WP_STATE_LFT_SEAL_CONFIG_BREAK_INT_CAL_ALLOWED	0x02	//!< Internal calibration does not break seal

//! The notification event from the subscription is a mix of status and new value
//! High byte: WP_STATE_tEvent, i.e. the state which has changed
//! Low  byte: The new value of the changed state
//! See TS_WP_Balance.c testcase 33 to see an implementation
//!
//! Macros to separate event and new value
#define WP_STATE_EVENT(msgEvent)	((RB_OS_tEvent)(((uint16_t)(msgEvent)) & 0xff00u) >> 8)
#define WP_STATE_VALUE(msgEvent)	(((uint16_t)(msgEvent)) & 0x00ffu)

//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//! Definition of LFT modes (legal for trade)
//! Note: The numeration is compatible with previous products and is used by the 2nd display
typedef enum {
	WP_STATE_LFTMODE_NONE			= 0,	//!< Not legal for trade
	WP_STATE_LFTMODE_NOT_APPR		= 1,	//!< Value is not approvable
	WP_STATE_LFTMODE_1_D_APPR		= 2,	//!< e = d approved
	WP_STATE_LFTMODE_10_D_APPR		= 5,	//!< e = 10d approved
	WP_STATE_LFTMODE_100_D_APPR		= 8,	//!< e = 100d approved
	WP_STATE_LFTMODE_1000_D_APPR	= 11,	//!< e = 1000d approved
	WP_STATE_LFTMODE_10000_D_APPR	= 14,	//!< e = 10000d approved
	WP_STATE_LFTMODE_100000_D_APPR	= 17	//!< e = 100000d approved
} RB_DECL_TYPE WP_STATE_tLFTMode;

//! Definition of LFT type (legal for trade)
typedef enum {
	WP_STATE_LFTTYPE_NONE			= 0,	//!< Not legal for trade
	WP_STATE_LFTTYPE_OIML_R60		= 1,	//!< LFT according to OIML R60
	WP_STATE_LFTTYPE_OIML_R76		= 2,	//!< LFT according to OIML R76
	WP_STATE_LFTTYPE_NIST_HB44		= 4,	//!< LFT according to NIST HB44
	WP_STATE_LFTTYPE_OIML_R51		= 8		//!< LFT according to OIML R51
} RB_DECL_TYPE WP_STATE_tLFTType;

//! Definition of sealing category
typedef enum {
	WP_STATE_HB44_SEALING_CAT_NONE	= 0,	//!< No sealing category
	WP_STATE_HB44_SEALING_CAT_1		= 1,	//!< Sealing category 1
	WP_STATE_HB44_SEALING_CAT_2		= 2,	//!< Sealing category 2
	WP_STATE_HB44_SEALING_CAT_3		= 3		//!< Sealing category 3
} RB_DECL_TYPE WP_STATE_tHB44SealingCat;

//! Definition of accuracy class (legal for trade)
typedef enum {
	WP_STATE_ACCURACYCLASS_NONE			= 0,	//!< Not legal for trade
	WP_STATE_ACCURACYCLASS_1			= 1,	//!< accuracy class I
	WP_STATE_ACCURACYCLASS_2			= 2,	//!< accuracy class II
	WP_STATE_ACCURACYCLASS_3			= 3,	//!< accuracy class III
	WP_STATE_ACCURACYCLASS_4			= 4,	//!< accuracy class IV
	WP_STATE_ACCURACYCLASS_4L			= 5		//!< accuracy class IV L
} RB_DECL_TYPE WP_STATE_tAccuracyClass;

//! Definition of Balance Operating states
//! Note: Enumerations 1..4(WP_POSTPROC_WGH_STS_MAX) are used as array index for stability adaptation
//! inside the module WP_POSTPROC!
//! Note: All states > 4 use the stability specifications of the operating state 'weighing'(1)
typedef enum {
	WP_STATE_BAL_OP_UNDEFINED		= 0,	//!< Balance operating state is undefined
	WP_STATE_BAL_OP_WEIGHING		= 1,	//!< Balance is weighing
	WP_STATE_BAL_OP_TARING			= 2,	//!< Balance is taring
	WP_STATE_BAL_OP_ZEROING			= 3,	//!< Balance is zeroing
	WP_STATE_BAL_OP_ADJUSTING		= 4,	//!< Balance is adjusting
	WP_STATE_BAL_OP_INITZERO				//!< Balance is doing initial zero
} RB_DECL_TYPE WP_STATE_tBalOperatingState;

//! Definition of Balance Adjustment methods\n
//! Note 1: WP_STATE_ADJ_SCALE_CAL_EXT is also used as a marker for the first scale production method using external weights\n
//! Note 2: WP_STATE_ADJ_FACTORY_CAL_EXT is also used as a marker for the first factory method using external weights\n
//! Note 3: WP_STATE_ADJ_USER_TST_INT is also used as mark for the first method using internal weights\n
//! Note 4: WP_STATE_ADJ_FACTORY_CAL_INT is also used as mark for the first factory method using internal weights\n
typedef enum {
	WP_STATE_ADJ_UNDEFINED			= 0,		//!<  0 No method
	WP_STATE_ADJ_USER_TST_EXT,					//!<  1 External user verification
	WP_STATE_ADJ_USER_CAL_EXT,					//!<  2 External user calibration
	WP_STATE_ADJ_USER_CAL_UP_EXT,				//!<  3 External user calibration without repro zero
	WP_STATE_ADJ_USER_CAL_DOWN_EXT,				//!<  4 External user calibration without repro zero (weight acquisition downwards)
	WP_STATE_ADJ_USER_PRODZERO_CAL_EXT,			//!<  5 External user calibration which sets the factory zero
	WP_STATE_ADJ_USER_3PT_CLIN_EXT,				//!<  6 External user 3 point coarse linearisation
	WP_STATE_ADJ_USER_3PT_UP_LIN_EXT,			//!<  7 External user 3 point linearization (weight acquisition upwards)
	WP_STATE_ADJ_USER_5PT_UP_LIN_EXT,			//!<  8 External user 5 point linearization (weight acquisition upwards)
	WP_STATE_ADJ_USER_3PT_DOWN_LIN_EXT,			//!<  9 External user 3 point linearization (weight acquisition downwards)
	WP_STATE_ADJ_USER_PRODZERO_3PT_UP_LIN_EXT,	//!< 10 External user 3 point linearization which sets factory zero
	WP_STATE_ADJ_USER_PRODZERO_5PT_UP_LIN_EXT,	//!< 11 External user 5 point linearization which sets factory zero
	WP_STATE_ADJ_USER_3PT_LINADJUST_EXT,		//!< 12 External user 3 point linearization with only acquiring linearity load
	WP_STATE_ADJ_USER_ZEROADJ_EXT,				//!< 13 External user factory zero adjustment
	WP_STATE_ADJ_USER_SPANADJUST_EXT,			//!< 14 External user span calibration without acquiring zero load
	WP_STATE_ADJ_SCALE_CAL_EXT,					//!< 15 External scale production calibration (\e Note \e 1)
	WP_STATE_ADJ_SCALE_CAL_UP_EXT,				//!< 16 External scale production calibration without repro zero
	WP_STATE_ADJ_SCALE_CAL_DOWN_EXT,			//!< 17 External scale production calibration measuring downwards
	WP_STATE_ADJ_SCALE_3PT_FLIN_EXT,			//!< 18 External scale production 3 point linearization with unknown weights
	WP_STATE_ADJ_SCALE_3PT_UP_LIN_EXT,			//!< 19 External scale production 3 point linearization with known weights (weight acquisition upwards)
	WP_STATE_ADJ_SCALE_3PT_DOWN_LIN_EXT,		//!< 20 External scale production 3 point linearization with known weights (weight acquisition downwards)
	WP_STATE_ADJ_SCALE_5PT_FLIN_EXT,			//!< 21 External scale production 5 point linearization with unknown weights
	WP_STATE_ADJ_SCALE_5PT_UP_LIN_EXT,			//!< 22 External scale production 5 point linearization with known weights (weight acquisition upwards)
	WP_STATE_ADJ_SCALE_3PT_LINADJUST_EXT,		//!< 23 External scale production 3 point linearization with only acquiring linearity load
	WP_STATE_ADJ_SCALE_ZEROADJ_EXT,				//!< 24 External scale production zero adjustment
	WP_STATE_ADJ_SCALE_ZEROADJ2_EXT,			//!< 25 External scale production zero adjustment with recalculating customer linearization
	WP_STATE_ADJ_SCALE_SPANADJUST_EXT,			//!< 26 External scale production span adjustment without acquiring zero load
	WP_STATE_ADJ_SCALE_LOADHYST_EXT,			//!< 27 External scale production load hysteresis compensation adjustment
	WP_STATE_ADJ_SCALE_LOADHYST_LINCAL_EXT,		//!< 28 External scale production load hysteresis compensation adjustment combined with 3pt linearization and span/offset adjustment
	WP_STATE_ADJ_SCALE_LOADHYST_5PTLINCAL_EXT,	//!< 29 External scale production load hysteresis compensation adjustment combined with 5pt linearization and span/offset adjustment
	WP_STATE_ADJ_FACTORY_CAL_EXT,				//!< 30 External factory calibration (\e Note \e 2)
	WP_STATE_ADJ_FACTORY_CAL_UP_EXT,			//!< 31 External factory calibration without repro zero
	WP_STATE_ADJ_FACTORY_CAL_DOWN_EXT,			//!< 32 External factory calibration without repro zero (weight acquisition downwards)
	WP_STATE_ADJ_FACTORY_3PT_CLIN_EXT,			//!< 33 External factory 3 point coarse linearisation (not supported by RB)
	WP_STATE_ADJ_FACTORY_3PT_FLIN_EXT,			//!< 34 External factory 3 point fine linearisation
	WP_STATE_ADJ_FACTORY_5PT_FLIN_EXT,			//!< 35 External factory 5 point fine linearisation
	WP_STATE_ADJ_FACTORY_3PT_UP_LIN_EXT,		//!< 36 External factory 3 point linearization (weight acquisition upwards)
	WP_STATE_ADJ_FACTORY_3PT_DOWN_LIN_EXT,		//!< 37 External factory 3 point linearization (weight acquisition downwards)
	WP_STATE_ADJ_FACTORY_5PT_UP_LIN_EXT,		//!< 38 External factory 5 point linearization (weight acquisition upwards)
	WP_STATE_ADJ_FACTORY_ZEROADJ_EXT,			//!< 39 External factory zero adjustment
	WP_STATE_ADJ_FACTORY_SPANADJUST_EXT,		//!< 40 External factory span calibration without acquiring zero load
	WP_STATE_ADJ_PRESCALER_EXT,					//!< 41 External prescaler adjustment
	WP_STATE_ADJ_PRESCALEROFFSET_EXT,			//!< 42 External prescaler adjustment
	WP_STATE_ADJ_USER_TST_INT,					//!< 43 Internal user verification (\e Note \e 3)
	WP_STATE_ADJ_USER_CAL_INT,					//!< 44 Internal user calibration
	WP_STATE_ADJ_USER_3PT_CLIN_INT,				//!< 45 Internal user 3 point coarse linearisation (not supported by RB)
	WP_STATE_ADJ_USER_3PT_FLIN1_INT,			//!< 46 Internal user 3 point fine linearisation (mode1)
	WP_STATE_ADJ_USER_3PT_FLIN2_INT,			//!< 47 Internal user 3 point fine linearisation (mode2)
	WP_STATE_ADJ_USER_2x3PT_FLIN_INT,			//!< 48 Internal user 2x3 point fine linearisation
	WP_STATE_ADJ_USER_STD_CAL,					//!< 49 User standard calibration
	WP_STATE_ADJ_USER_STD_CAL_HYBRID,			//!< 50 User standard calibration for hybrid scales
	WP_STATE_ADJ_USER_STD_CAL_ADJ_CAL,			//!< 51 User standard calibration combined with internal user calibration
	WP_STATE_ADJ_PLACEMENT_TST_INT,				//!< 52 Internal scale placement verification
	WP_STATE_ADJ_PLACEMENT_CAL_INT,				//!< 53 Internal calibration adjustment with Scale Placement SP block
	WP_STATE_ADJ_PLACEMENT_GEOCAL_INT,			//!< 54 Internal geo adjustment with Scale Placement SP block
	WP_STATE_ADJ_SCALE_CAL_INT,					//!< 55 Internal scale production calibration
	WP_STATE_ADJ_FACTORY_CAL_INT,				//!< 56 Internal factory calibration (\e Note \e 4)
	WP_STATE_ADJ_FACTORY_3PT_CLIN_INT,			//!< 57 Internal factory 3 point coarse linearisation
	WP_STATE_ADJ_FACTORY_3PT_FLIN_INT,			//!< 58 Internal factory 3 point fine linearisation
	WP_STATE_ADJ_FACTORY_2x3PT_FLIN_INT,		//!< 59 Internal factory 2x3 point fine linearisation
	WP_STATE_ADJ_FACTORY_STD_CAL,				//!< 60 Factory standard calibration
	WP_STATE_ADJ_FACTORY_STD_CAL_HYBRID,		//!< 61 Factory standard calibration for hybrid scales
	WP_STATE_ADJ_FACTORY_STD_CAL_ADJ_CAL,		//!< 62 Factory standard calibration combined with internal factory calibration
	WP_STATE_ADJ_PRESCALER_INT,					//!< 63 Internal prescaler adjustment
	WP_STATE_ADJ_USER_RESET,					//!< 64 Resets user adjustment stage
	WP_STATE_ADJ_PLACEMENT_CAL_RESET,			//!< 65 Resets scale placement calibration adjustment
	WP_STATE_ADJ_PLACEMENT_GEOCAL_RESET,		//!< 66 Resets scale placement geocalibration adjustment
	WP_STATE_ADJ_PLACEMENT_GEOCODE_RESET,		//!< 67 Resets scale placement geocode
	WP_STATE_ADJ_SCALE_RESET,					//!< 68 Resets scale production and user adjustment stage
	WP_STATE_ADJ_FACTORY_RESET,					//!< 69 Resets factory, scale placement, scale production and user adjustment stage
	WP_STATE_ADJ_USER_LAST						//!< Must be last entry
} RB_DECL_TYPE WP_STATE_tBalAdjustMethod;

//! Definition of Balance Adjust Processing states
typedef enum {
	WP_STATE_BAL_APROC_UNDEFINED	= 0,		//!<  0 Adjust processing state is undefined
	WP_STATE_BAL_APROC_DONE,					//!<  1 Adjust process is successfully done
	WP_STATE_BAL_APROC_ABORT,					//!<  2 Adjust process is aborted (see abort codes)
	WP_STATE_BAL_APROC_CANCEL,					//!<  3 Adjust process is canceled by user
	WP_STATE_BAL_APROC_PUSH_ZERO,				//!<  4 Adjust process is waiting for pushZero
	WP_STATE_BAL_APROC_PUSH_LOAD,				//!<  5 Adjust process is waiting for pushLoad
	WP_STATE_BAL_APROC_PRE_LOAD,				//!<  6 Adjust process is waiting for preLoad
	WP_STATE_BAL_APROC_1ST_ZERO,				//!<  7 Adjust process is waiting for first zero
	WP_STATE_BAL_APROC_2ND_ZERO,				//!<  8 Adjust process is waiting for second zero
	WP_STATE_BAL_APROC_3RD_ZERO,				//!<  9 Adjust process is waiting for third zero
	WP_STATE_BAL_APROC_4TH_ZERO,				//!< 10 Adjust process is waiting for fourth zero
	WP_STATE_BAL_APROC_1ST_LOAD,				//!< 11 Adjust process is waiting for first load
	WP_STATE_BAL_APROC_2ND_LOAD,				//!< 12 Adjust process is waiting for second load
	WP_STATE_BAL_APROC_3RD_LOAD,				//!< 13 Adjust process is waiting for third load
	WP_STATE_BAL_APROC_4TH_LOAD,				//!< 14 Adjust process is waiting for fourth load
	WP_STATE_BAL_APROC_5TH_LOAD,				//!< 15 Adjust process is waiting for fifth load
	WP_STATE_BAL_APROC_6TH_LOAD,				//!< 16 Adjust process is waiting for sixth load
	WP_STATE_BAL_APROC_7TH_LOAD					//!< 17 Adjust process is waiting for seventh load
} RB_DECL_TYPE WP_STATE_tBalAdjProcState;

//! Definition of Balance Weight Request state
typedef enum {
	WP_STATE_BAL_WREQU_UNDEFINED	= 0,		//!<  0 Weight requesting state is undefined
	WP_STATE_BAL_WREQU_WAIT_USER,				//!<  1 Weight requests user interaction
	WP_STATE_BAL_WREQU_OUT_RANGE,				//!<  2 Weight is out of range for weight request
	WP_STATE_BAL_WREQU_IN_RANGE,				//!<  3 Weight is in range but unstable for weight request
	WP_STATE_BAL_WREQU_STABLE,					//!<  4 Weight is ready for weight request (in range and stable)
	WP_STATE_BAL_WREQU_ABORT					//!<  5 Weight request is aborted (UNDER-/OVERLOAD)
} RB_DECL_TYPE WP_STATE_tBalWRequState;

//! Definition of Balance Weight State
typedef enum {
	WP_STATE_BAL_WGTSTS_UNDEFINED	= 0,		//!<  0 Adjustment weight is undefined
	WP_STATE_BAL_WGTSTS_BELOW_TOL,				//!<  1 Adjustment weight is below tolerances
	WP_STATE_BAL_WGTSTS_ABOVE_TOL,				//!<  2 Adjustment weight is above tolerances
	WP_STATE_BAL_WGTSTS_IN_TOL					//!<  3 Adjustment weight is within tolerances
} RB_DECL_TYPE WP_STATE_tBalWgtState;

//! Definition of Balance Abort codes
typedef enum {
	WP_STATE_BAL_ACODE_UNDEFINED	= 0,		//!<  0 Abort code is undefined
	WP_STATE_BAL_ACODE_TIMEOUT,					//!<  1 Adjust process is aborted by timeout
	WP_STATE_BAL_ACODE_OVERLOAD,				//!<  2 Adjust process is aborted by overload
	WP_STATE_BAL_ACODE_UNDERLOAD,				//!<  3 Adjust process is aborted by underload
	WP_STATE_BAL_ACODE_OP_ERROR					//!<  4 Adjust process is aborted by system failure
} RB_DECL_TYPE WP_STATE_tBalAbortCode;

//! Definition of the actual balance user adjustment mode
typedef enum {
	WP_STATE_BAL_USER_ADJ_MODE_MANUAL	= 0,	//!<  0 Manual user adjustment
	WP_STATE_BAL_USER_ADJ_MODE_AUTOMATIC		//!<  1 Automatic user adjustment
} RB_DECL_TYPE WP_STATE_tBalUserAdjMode;

//! Definition of the actual balance user adjustment mode
typedef enum {
	WP_STATE_BAL_USER_ADJ_WGT_INTERNAL	= 0,	//!<  0 Use internal weight
	WP_STATE_BAL_USER_ADJ_WGT_EXTERNAL			//!<  1 Use external weight
} RB_DECL_TYPE WP_STATE_tBalUserAdjWeight;

//! Definition of the event which is sent when state changed
typedef enum {
	WP_STATE_EVENT_SYS_OPERATING_MODE_CHANGED	= 0,	//!< 0 = System Operating Mode changed
	WP_STATE_EVENT_LFT_MODE_CHANGED,					//!< 1 = LFT mode changed (Legal For Trade)
	WP_STATE_EVENT_BAL_OPERATING_STATE_CHANGED,			//!< 2 = Balance Operating State changed
	WP_STATE_EVENT_BAL_ADJ_METHOD_CHANGED,				//!< 3 = Balance Adjustment Method changed
	WP_STATE_EVENT_BAL_ADJ_PROC_STATE_CHANGED,			//!< 4 = Balance Adjust Processing State changed
	WP_STATE_EVENT_BAL_BAL_WREQU_STATE_CHANGED,			//!< 5 = Balance Weight Request State changed
	WP_STATE_EVENT_BAL_BAL_WEIGHT_STATE_CHANGED,		//!< 6 = Balance Weight State changed
	WP_STATE_EVENT_BAL_STATE_CHANGED					//!< 7 = Balance state was changed
} RB_DECL_TYPE WP_STATE_tEvent;

//! Definition of LFT seal breaking resons
typedef enum {
	WP_STATE_LFT_SEAL_BREAK				= 0,	//!< Break seal unconditionally
	WP_STATE_LFT_SEAL_BREAK_EXT_CAL		= 1,	//!< Break seal caused by external calibration or linearisation
	WP_STATE_LFT_SEAL_BREAK_INT_CAL		= 2,	//!< Break seal caused by intern calibration or linearisation
	WP_STATE_LFT_SEAL_BREAK_FW_CHANGE	= 3		//!< Break seal caused by firmware change
} RB_DECL_TYPE WP_STATE_tLFTBreakReason;


//! Balance states info
typedef struct {
	WP_STATE_tBalOperatingState	state;		//!< State (weighing, zeroing, tareing, ...)
	WP_STATE_tBalAdjustMethod	method;		//!< Adjust method (user-cal, factory-cal, ...)
	WP_STATE_tBalAdjProcState	adjState;	//!< Adjust Processing state (first zero, third load, ...)
	WP_STATE_tBalAbortCode		abortCode;	//!< Abort code (timeout, overload, ...)
	WP_STATE_tBalWRequState		reqState;	//!< Weight Requesting state (out of range, within range, ...)
	WP_STATE_tBalWgtState		wgtState;	//!< State of the requested weight (above, below, within range)
	WP_tFloat					reqWeight;	//!< Requested weight
	} RB_DECL_TYPE WP_STATE_tBalState;


//! Definition of TD-parameters for the module STATE
typedef struct {
	WP_STATE_tLFTMode			LFTMode;				//!< LFT mode (none, e=d, e=10d, ...)
	WP_STATE_tLFTType			LFTType;				//!< LFT type (none, OIML-R60, OIML-R76, ...)
	WP_STATE_tHB44SealingCat	HB44SealingCat;			//!< HB44 Sealing Category (none, 1, ...)
	WP_STATE_tAccuracyClass		AccuracyClass;			//!< accuracy class (none, I, II, III, IV, ...)
	bool						AcceptFatalError;		//!< True if fatal errors are handled
	uint16_t					AdjustConfig;			//!< Adjust configuration (XP0333)
	uint8_t						weighingModeMask;		//!< Mask for weighing modes
	uint8_t						weighingDefault;		//!< Default weighing mode
	uint8_t						environmentModeMask;	//!< Mask for environment modes
	uint8_t						environmentDefault;		//!< Default environment mode
	uint8_t						readabilityMask;		//!< Mask for readability (XP0928, XP0959)
	uint8_t						readabilityDefault;		//!< Default for readability (XP0928, XP0959)
	uint16_t					dispResolutionDecMask;	//!< Mask for increased display resolutions (XP0962)
	uint16_t					dispResolutionIncMask;	//!< Mask for decreased display resolutions (XP0962)
} RB_DECL_TYPE WP_STATE_tTDParams;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// WP_STATE_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialization of the module STATUS
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_Initialize(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetTDParams
//--------------------------------------------------------------------------------------------------
//! \brief	Set typedefinition parameters for the module STATE
//!
//! \param	pUnitParams		Pointer to a set of typedefinition parameters for the module STATE
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetTDParams(const WP_STATE_tTDParams* pParams);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetTDParams
//--------------------------------------------------------------------------------------------------
//! \brief	Get the typedefinition parameterset of the module STATE
//!
//! \return	Pointer to the typedefinition parameterset
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC const WP_STATE_tTDParams* WP_STATE_GetTDParams(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_Subscribe
//--------------------------------------------------------------------------------------------------
//! \brief	Caller subscribes to messages (events)
//!
//! The number of possible subscriber is defined in RB_Config.h (RB_CONFIG_WP_STATE_NUM_OF_SUBSCRIBER)
//!
//! \param	pMsgQueue	Message queue pointer, where events are sent to
//! \return	true = OK, false = Too many subscriber -> increase number of subscriber
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool WP_STATE_Subscribe(RB_OS_tMsgQueue* pMsgQueue);


//--------------------------------------------------------------------------------------------------
// WP_STATE_Unsubscribe
//--------------------------------------------------------------------------------------------------
//! \brief	Caller unsubscribes to messages (events)
//!
//! \param	pMsgQueue	Message queue pointer to cancel subscription
//! \return	true = OK, false = No such subscriber
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool WP_STATE_Unsubscribe(const RB_OS_tMsgQueue* pMsgQueue);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetMsgLostCount
//--------------------------------------------------------------------------------------------------
//! \brief	Get the number of lost messages and reset counter
//!
//! \return	Number of lost messages
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint16_t WP_STATE_GetMsgLostCount(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetLFTMode
//--------------------------------------------------------------------------------------------------
//! \brief	Set the LFT mode (LegalForTrade)
//!
//! \param	mode  LFT mode
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetLFTMode(WP_STATE_tLFTMode mode);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetLFTMode
//--------------------------------------------------------------------------------------------------
//! \brief	Get the LFT mode (LegalForTrade)
//!
//! This function returns the LegalForTrade mode having regard to the system operating mode
//!
//! \return	LFT mode
//!		- WP_STATE_LFTMODE_NONE
//!		- WP_STATE_LFTMODE_NOT_APPR
//!		- WP_STATE_LFTMODE_1_D_APPR
//!		- WP_STATE_LFTMODE_10_D_APPR
//!		- WP_STATE_LFTMODE_100_D_APPR
//!		- WP_STATE_LFTMODE_1000_D_APPR
//!		- WP_STATE_LFTMODE_10000_D_APPR
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_STATE_tLFTMode WP_STATE_GetLFTMode(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetLFTValue
//--------------------------------------------------------------------------------------------------
//! \brief	Get the LFT value, i.e. e = <value> d
//!
//! This function returns the LegalForTrade value, regarding the system operating mode
//!
//! \return	LFT value
//!		- WP_STATE_LFTMODE_NONE				returns	1
//!		- WP_STATE_LFTMODE_NOT_APPR			returns	1
//!		- WP_STATE_LFTMODE_1_D_APPR			returns	1
//!		- WP_STATE_LFTMODE_10_D_APPR		returns	10
//!		- WP_STATE_LFTMODE_100_D_APPR		returns	100
//!		- WP_STATE_LFTMODE_1000_D_APPR		returns	1000
//!		- WP_STATE_LFTMODE_10000_D_APPR		returns	10000
//!		- WP_STATE_LFTMODE_100000_D_APPR	returns	100000
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint32_t WP_STATE_GetLFTValue(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetAdjustConfig
//--------------------------------------------------------------------------------------------------
//! \brief	Get adjust configuration
//!
//! This function returns the adjust configuration value, regarding the system operating mode. The
//! bits are encoded according to XP0333 description (see SICS-P manual).
//!
//! \return	adjust configuration
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint16_t WP_STATE_GetAdjustConfig(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetLFTType
//--------------------------------------------------------------------------------------------------
//! \brief	Set the LFT type (LegalForTrade)
//!
//! \param	lftType  LFT type
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetLFTType(WP_STATE_tLFTType lftType);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetLFTType
//--------------------------------------------------------------------------------------------------
//! \brief	Get the LFT type (LegalForTrade)
//!
//! \return	LFT type
//!		- WP_STATE_LFTTYPE_NONE
//!		- WP_STATE_LFTTYPE_OIML_R60
//!		- WP_STATE_LFTTYPE_OIML_R76
//!		- WP_STATE_LFTTYPE_NIST_HB44
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_STATE_tLFTType WP_STATE_GetLFTType(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetAccuracyClass
//--------------------------------------------------------------------------------------------------
//! \brief	Set the accuracy class (LegalForTrade)
//!
//! \param	accClass  accuracy class
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetAccuracyClass(WP_STATE_tAccuracyClass accClass);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetAccuracyClass
//--------------------------------------------------------------------------------------------------
//! \brief	Get the accuracy class (LegalForTrade)
//!
//! \return	accuracy class
//!		- WP_STATE_ACCURACYCLASS_NONE
//!		- WP_STATE_ACCURACYCLASS_1
//!		- WP_STATE_ACCURACYCLASS_2
//!		- WP_STATE_ACCURACYCLASS_3
//!		- WP_STATE_ACCURACYCLASS_4
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_STATE_tAccuracyClass WP_STATE_GetAccuracyClass(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetDeclaredAccuracyClass
//--------------------------------------------------------------------------------------------------
//! \brief	Get the declared accuracy class for a given range
//!
//! Unlike the function WP_STATE_GetAccuracyClass this function does not return the runtime accuracy
//! class, but reads the declared accuracy class from XP0911.
//!
//! \param	range	Return the accuracy class for this range
//!					0 = total range
//!					RB_CONFIG_WP_RANGES_MAX-1 = smallest fine range
//! \return	accuracy class of the given range
//!			WP_STATE_ACCURACYCLASS_NONE if range is invalid or XP0911 not available
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_STATE_tAccuracyClass WP_STATE_GetDeclaredAccuracyClass(int range);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetHB44SealingCat
//--------------------------------------------------------------------------------------------------
//! \brief	Set the HB44 sealing category
//!
//! \param	sealingCat	HB44 sealing category
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetHB44SealingCat(WP_STATE_tHB44SealingCat sealingCat);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetHB44SealingCat
//--------------------------------------------------------------------------------------------------
//! \brief	Get the LFT type (LegalForTrade)
//!
//! \return	HB44 sealing category, WP_STATE_HB44_SEALING_CAT_NONE if not avaialble
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_STATE_tHB44SealingCat WP_STATE_GetHB44SealingCat(void);


#if defined(WP_FX_LFT_SEAL_BREAK)
//--------------------------------------------------------------------------------------------------
// WP_STATE_BreakLFTSeal
//--------------------------------------------------------------------------------------------------
//! \brief	Break LFT seal
//!
//! This function will manage break of LFT behavior. If LFT seal is broken the LFT-BreakCounter
//! is incremented and permanently stored.
//! Breaking LFT occurs on modifying weighing relevant parameters on LFT enabled balances. This covers
//! calibration, linearisation and enabling XP- or XA-commands (required to modify parameters).
//! The state if the seal is actually broken can be read with WP_STATE_IsLFTSealBroken(). The actual
//! value of LFT-BreakCounter can be read with WP_STATE_GetLFTBreakCounter().
//!
//! \param	reason		Reason for breaking seal (WP_STATE_tLFTBreakReason)
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_BreakLFTSeal(WP_STATE_tLFTBreakReason reason);
#endif


#if defined(WP_FX_LFT_SEAL_BREAK)
//--------------------------------------------------------------------------------------------------
// WP_STATE_IsLFTSealBroken
//--------------------------------------------------------------------------------------------------
//! \brief	Return actual state of LFT seal break
//!
//! \return				State of LFT seal break
//!                     - TRUE: Seal is broken
//!                     - FALSE: Seal is not broken or non-LFT balance
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool WP_STATE_IsLFTSealBroken(void);
#endif


#if defined(WP_FX_LFT_SEAL_BREAK)
//--------------------------------------------------------------------------------------------------
// WP_STATE_SetLFTBreakCounterIncrement
//--------------------------------------------------------------------------------------------------
//! \brief	Set the increment for LFT break counter
//!
//! The default increment is set to one in initialization of module. Since breaking LFT can only be
//! done one time after each startup, this function must be called with a good seed value between
//! 1 and 65535.
//!
//! \param	increment  Increment for LFT break counter
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetLFTBreakCounterIncrement(uint16_t increment);
#endif


#if defined(WP_FX_LFT_SEAL_BREAK)
//--------------------------------------------------------------------------------------------------
// WP_STATE_GetLFTBreakCounter
//--------------------------------------------------------------------------------------------------
//! \brief	Read LFT break counter
//!
//! \return				LFT break counter
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint16_t WP_STATE_GetLFTBreakCounter(void);
#endif

//--------------------------------------------------------------------------------------------------
// WP_STATE_SetFatalError
//--------------------------------------------------------------------------------------------------
//! \brief	Set the fatal error
//!
//! \param	err  Error number (zero is defined as no error, other error codes are product specific)
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetFatalError(uint16_t err);

//--------------------------------------------------------------------------------------------------
// WP_STATE_GetFatalError
//--------------------------------------------------------------------------------------------------
//! \brief	Get the fatal error number. Zero is defined as no error
//!
//! \return	Error number  (zero is defined as no error, other error codes are product specific)
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint16_t WP_STATE_GetFatalError(void);

//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalUserAdjMode
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance user adjustment mode
//!
//! \return	Balance user adjustment mode
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_STATE_tBalUserAdjMode WP_STATE_GetBalUserAdjMode(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalUserAdjWeight
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance user adjustment weight (internal or external)
//!
//! \return	Balance user adjustment weight
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_STATE_tBalUserAdjWeight WP_STATE_GetBalUserAdjWeight(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalUserTstWeight
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance user test weight (internal or external)
//!
//! \return	Balance user test weight
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_STATE_tBalUserAdjWeight WP_STATE_GetBalUserTstWeight(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalOpState
//--------------------------------------------------------------------------------------------------
//! \brief	Get balance operating state
//!
//! \return	Balance operating state
//!		- WP_STATE_BAL_OP_UNDEFINED
//!		- WP_STATE_BAL_OP_WEIGHING
//!		- WP_STATE_BAL_OP_TARING
//!		- WP_STATE_BAL_OP_ZEROING
//!		- WP_STATE_BAL_OP_ADJUSTING
//!		- WP_STATE_BAL_OP_INITZERO
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_STATE_tBalOperatingState WP_STATE_GetBalOpState(void) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalAdjustMethode
//--------------------------------------------------------------------------------------------------
//! \brief	Get method of adjustment
//!
//! \return	Adjustment method
//!		- WP_STATE_ADJ_UNDEFINED
//!		- WP_STATE_ADJ_USER_TST_EXT
//!		- WP_STATE_ADJ_USER_CAL_EXT
//!		- WP_STATE_ADJ_USER_3PT_CLIN_EXT
//!		- WP_STATE_ADJ_FACTORY_CAL_EXT
//!		- WP_STATE_ADJ_FACTORY_3PT_CLIN_EXT
//!		- WP_STATE_ADJ_FACTORY_3PT_FLIN_EXT
//!		- WP_STATE_ADJ_FACTORY_5PT_FLIN_EXT
//!		- WP_STATE_ADJ_USER_TST_INT
//!		- WP_STATE_ADJ_USER_CAL_INT
//!		- WP_STATE_ADJ_USER_3PT_CLIN_INT
//!		- WP_STATE_ADJ_USER_3PT_FLIN1_INT
//!		- WP_STATE_ADJ_USER_3PT_FLIN2_INT
//!		- WP_STATE_ADJ_USER_2x3PT_FLIN_INT
//!		- WP_STATE_ADJ_USER_STD_CAL
//!		- WP_STATE_ADJ_FACTORY_CAL_INT
//!		- WP_STATE_ADJ_FACTORY_3PT_CLIN_INT
//!		- WP_STATE_ADJ_FACTORY_3PT_FLIN_INT
//!		- WP_STATE_ADJ_FACTORY_2x3PT_FLIN_INT
//!		- WP_STATE_ADJ_FACTORY_STD_CAL
//!		- WP_STATE_ADJ_USER_3PT_INDLIN_EXT
//!		- WP_STATE_ADJ_USER_3PT_PINDLIN_EXT
//!		- WP_STATE_ADJ_USER_5PT_PINDLIN_EXT
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_STATE_tBalAdjustMethod WP_STATE_GetBalAdjustMethode(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalAProcState
//--------------------------------------------------------------------------------------------------
//! \brief	Get adjust-processing state
//!
//! \return	Balance adjust processing state
//!		- WP_STATE_BAL_APROC_UNDEFINED
//!		- WP_STATE_BAL_APROC_DONE
//!		- WP_STATE_BAL_APROC_ABORT
//!		- WP_STATE_BAL_APROC_CANCEL
//!		- WP_STATE_BAL_APROC_PUSH_ZERO
//!		- WP_STATE_BAL_APROC_PUSH_LOAD
//!		- WP_STATE_BAL_APROC_PRE_LOAD
//!		- WP_STATE_BAL_APROC_1ST_ZERO
//!		- WP_STATE_BAL_APROC_2ND_ZERO
//!		- WP_STATE_BAL_APROC_3RD_ZERO
//!		- WP_STATE_BAL_APROC_4TH_ZERO
//!		- WP_STATE_BAL_APROC_1ST_LOAD
//!		- WP_STATE_BAL_APROC_2ND_LOAD
//!		- WP_STATE_BAL_APROC_3RD_LOAD
//!		- WP_STATE_BAL_APROC_4TH_LOAD
//!		- WP_STATE_BAL_APROC_5TH_LOAD
//!		- WP_STATE_BAL_APROC_6TH_LOAD
//!		- WP_STATE_BAL_APROC_7TH_LOAD
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_STATE_tBalAdjProcState WP_STATE_GetBalAProcState(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalWRequState
//--------------------------------------------------------------------------------------------------
//! \brief	Get balance weight requesting state
//!
//! \return	Balance weight requesting state
//!		- WP_STATE_BAL_WREQU_UNDEFINED
//!		- WP_STATE_BAL_WREQU_OUT_RANGE
//!		- WP_STATE_BAL_WREQU_IN_RANGE
//!		- WP_STATE_BAL_WREQU_STABLE
//!		- WP_STATE_BAL_WREQU_ABORT
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_STATE_tBalWRequState WP_STATE_GetBalWRequState(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalWeightState
//--------------------------------------------------------------------------------------------------
//! \brief	Get balance weight state
//!
//! Get state of the requested weight
//!
//! \return	Weight actually requested
//!		- WP_STATE_BAL_WGTSTS_UNDEFINED
//!		- WP_STATE_BAL_WGTSTS_BELOW_TOL
//!		- WP_STATE_BAL_WGTSTS_ABOVE_TOL
//!		- WP_STATE_BAL_WGTSTS_IN_TOL
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_STATE_tBalWgtState WP_STATE_GetBalWeightState(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalWRequWeight
//--------------------------------------------------------------------------------------------------
//! \brief	Get balance weight requesting value
//!
//! \return	Weight actually requested
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tFloat WP_STATE_GetBalWRequWeight(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalAbortCode
//--------------------------------------------------------------------------------------------------
//! \brief	Get abortion code of balance operation
//!
//! \return	Balance abort code
//!		- WP_STATE_BAL_ACODE_UNDEFINED
//!		- WP_STATE_BAL_ACODE_TIMEOUT
//!		- WP_STATE_BAL_ACODE_OVERLOAD
//!		- WP_STATE_BAL_ACODE_UNDERLOAD
//!		- WP_STATE_BAL_ACODE_OP_ERROR
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_STATE_tBalAbortCode WP_STATE_GetBalAbortCode(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalUserAdjMode
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance user adjustment mode
//!
//! \param	mode	Balance user adjustment mode
//! \return	State (see type WP_tStatus)
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tStatus WP_STATE_SetBalUserAdjMode(WP_STATE_tBalUserAdjMode mode);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalUserAdjWeight
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance user adjustment weight (internal or external)
//!
//! \param	weight		Balance user adjustment weight
//! \return	State (see type WP_tStatus)
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tStatus WP_STATE_SetBalUserAdjWeight(WP_STATE_tBalUserAdjWeight weight);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalUserTstWeight
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance user test weight (internal or external)
//!
//! \param	weight		Balance user test weight
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetBalUserTstWeight(WP_STATE_tBalUserAdjWeight weight);


//==================================================================================================
//! The following functions should be used only by the module WP_BALANCE for updating
//! the information of the BALANCE-states
//--------------------------------------------------------------------------------------------------
// WP_STATE_ResetBalStates
//--------------------------------------------------------------------------------------------------
//! \brief	Reset the state information of BALANCE states
//!
//! This function should only be used by the WP_BALANCE
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_ResetBalStates(void);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalanceState
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance state
//!
//! This function should only be used by the WP_BALANCE. The event WP_STATE_EVENT_BAL_STATE_CHANGED
//! will be posted if subscription is enabled and any part of state is changed.
//!
//! \param	pNewState  Pointer to new state
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetBalanceState(const WP_STATE_tBalState *pNewState);


//--------------------------------------------------------------------------------------------------
// WP_STATE_GetBalanceState
//--------------------------------------------------------------------------------------------------
//! \brief	Get balance state
//!
//! Copies the whole balance state to the given pointer
//!
//! \param	pState  Pointer to copy the state to
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_GetBalanceState(WP_STATE_tBalState *pState);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalOpState
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance operating state
//!
//! This function should only be used by the WP_BALANCE
//!
//! \param	state  Balance operating state
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetBalOpState(WP_STATE_tBalOperatingState state);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalAdjustMethode
//--------------------------------------------------------------------------------------------------
//! \brief	Set method for adjustment
//!
//! This function should only be used by the WP_BALANCE
//!
//! \param	method  Adjustment method
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetBalAdjustMethode(WP_STATE_tBalAdjustMethod method);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalAProcState
//--------------------------------------------------------------------------------------------------
//! \brief	Set Adjust processing state
//!
//! This function should only be used by the WP_BALANCE
//!
//! \param	state  Balance adjust processing state
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetBalAProcState(WP_STATE_tBalAdjProcState state);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalWRequStatus
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance weight requesting state
//!
//! This function should only be used by the WP_BALANCE
//!
//! \param	state  Balance weight requesting state
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetBalWRequState(WP_STATE_tBalWRequState state);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalWRequWeight
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance weight requesting reference
//!
//! This function should only be used by the WP_BALANCE
//!
//! \param	value  Requested weight
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetBalWRequWeight(WP_tFloat value);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalWeightStatus
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance weight status
//!
//! This function should only be used by the WP_BALANCE
//!
//! Get state of the requested weight
//!
//! \param	state  Balance weight state
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetBalWeightState(WP_STATE_tBalWgtState state);


//--------------------------------------------------------------------------------------------------
// WP_STATE_SetBalAbortCode
//--------------------------------------------------------------------------------------------------
//! \brief	Set balance abort code
//!
//! This function should only be used by the WP_BALANCE
//!
//! \param	code  Balance abort code
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_STATE_SetBalAbortCode(WP_STATE_tBalAbortCode code);


#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_WP
#endif // _WP_State__h
