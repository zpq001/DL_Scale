//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/WP_Data.h
//! \ingroup	util
//! \brief		Common parameter data for weighing package
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Martin Heusser
//
// $Date: 2017/11/02 07:36:13MEZ $
// $Revision: 1.137 $
//
//==================================================================================================

#ifndef _WP_Data__h
#define _WP_Data__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_WP) && (RB_CONFIG_USE_WP == RB_CONFIG_YES)

#include "WP_Typedefs.h"

//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================


// Definition of counts for repetitive parameters
#define WP_DATA_N_XP0327	(RB_CONFIG_WP_STAB_DEF_MAX)

#if defined(WP_FX_HYST_CORRECTION)
	#define WP_DATA_N_XP0329	(RB_CONFIG_WP_RANGES_MAX)
	#define WP_DATA_N_XP0354	(RB_CONFIG_WP_RANGES_MAX)
#endif

#if defined(WP_FX_S_CURVE)
	#define WP_DATA_N_XP0331	(RB_CONFIG_WP_RANGES_MAX)
#endif

#if defined(WP_FX_RANGES)
	#define WP_DATA_N_XP0338	(RB_CONFIG_WP_RANGES_MAX-1)
#endif
#define WP_DATA_N_XP0340		2
#define WP_DATA_N_XP0369		(RB_CONFIG_WP_STAB_DEF_MAX)
#define WP_DATA_N_XP063X		9
#define WP_DATA_N_XP0909		(RB_CONFIG_WP_RANGES_MAX)
#define WP_DATA_N_XP0911		(RB_CONFIG_WP_RANGES_MAX)
#define WP_DATA_N_XP0919		(RB_CONFIG_WP_RANGES_MAX)
#define WP_DATA_N_XP0922		(RB_CONFIG_WP_RANGES_MAX)
#define WP_DATA_N_XP0949		(RB_CONFIG_WP_MAX_SCALE_CONFIGURATIONS)

#define WP_DATA_N_M21			RB_CONFIG_WP_UNIT_MAX_ACTIVATED
#define WP_DATA_N_M22			RB_CONFIG_WP_UNIT_MAX_CUSTOM_UNITS

#define WP_DATA_N_USTB			3

#if defined(WP_FX_PROFACT)
	#define WP_DATA_N_XP0940	(RB_CONFIG_WP_FACT_NUMBER_OF_TRIGGER_DAYTIMES)
	#define WP_DATA_N_M17		(RB_CONFIG_WP_FACT_NUMBER_OF_TRIGGER_DAYTIMES)
#endif

#if defined(WP_FX_MIN_WEIGH)
	#if !defined(RB_CONFIG_WP_NUM_OF_MIN_WEIGH_METHODS)
		#define RB_CONFIG_WP_NUM_OF_MIN_WEIGH_METHODS	3
	#endif
	#if (RB_CONFIG_WP_NUM_OF_MIN_WEIGH_METHODS < 1)
		#error RB_CONFIG_WP_NUM_OF_MIN_WEIGH_METHODS must be 1 or more
	#endif
	#define WP_DATA_N_XM1004	(RB_CONFIG_WP_NUM_OF_MIN_WEIGH_METHODS)
	#define WP_DATA_M_XM1005	(RB_CONFIG_WP_NUM_OF_MIN_WEIGH_METHODS)
	#define WP_DATA_N_XM1005	3
	#define WP_DATA_M_XM1006	(RB_CONFIG_WP_NUM_OF_MIN_WEIGH_METHODS)
	#define WP_DATA_N_XM1006	4
#endif

// Check correct setting for reduced configuration setup
#if defined(WP_FX_CNFPAR_RED)
	#if RB_CONFIG_WP_MAX_SCALE_CONFIGURATIONS < 1
		#error RB_CONFIG_WP_MAX_SCALE_CONFIGURATIONS must be at least 1
	#endif
	#if RB_CONFIG_WP_MAX_SCALE_CONFIGURATIONS > 100
		#error RB_CONFIG_WP_MAX_SCALE_CONFIGURATIONS must be maximal 100
	#endif
#endif

// Default values for filter tables used in XP0502, XP0348, XP0371, M01, M02
#if defined(WP_FX_DSP)
	#ifndef SP_CONFIG_USE_SIGNALPROCESSING_V2
		#ifndef SP_DEFAULT_FILTER_TABLE_NR
		#define SP_DEFAULT_FILTER_TABLE_NR 0
		#endif
		#ifndef SP_DEFAULT_FILTER_SET
		#define SP_DEFAULT_FILTER_SET 13
		#endif
	#endif
	#define WP_SP_DEFFILTTAB		(SP_DEFAULT_FILTER_TABLE_NR)
	#define WP_SP_DEFFILTSET		(SP_DEFAULT_FILTER_SET)
	#if (WP_SP_DEFFILTSET < 11) || (WP_SP_DEFFILTSET > 99)
		#error Wrong value of SP_DEFAULT_FILTER_SET
	#endif
	#define WP_SP_DEF_M01				((WP_SP_DEFFILTSET / 10) - 1)
	#define WP_SP_DEF_M02				((WP_SP_DEFFILTSET % 10) - 1)
	#define WP_SP_DEF_XP0348			{(1u << WP_SP_DEF_M01),WP_SP_DEF_M01}
	#define WP_SP_DEF_XP0371			{(1u << WP_SP_DEF_M02),WP_SP_DEF_M02}
#endif

#if defined(WP_FX_CALWGT)
	#if defined(RB_CONFIG_CALWEIGHT_NUMBER)
		#define WP_CALWGT_NR			(RB_CONFIG_CALWEIGHT_NUMBER)
	#else
		#define WP_CALWGT_NR			0
	#endif
#endif

#ifdef SP_CONFIG_USE_SIGNALPROCESSING_INTERFACE_V2
	#define WP_SP_SET_FILTER_PARAMETER(table, mode, speed) (SP_SetFilterParameter(table, mode, speed) == 0)
	#define WP_SP_SET_FIXED_FILTER(fc) (SP_SetFixedFilter(fc, -1) == 0)
#else
	#define WP_SP_SET_FILTER_PARAMETER(table, mode, speed) (SP_SIGNALPROC_SetFilter((SP_SIGNALPROC_tFilterSettingRow)(mode), (SP_SIGNALPROC_tFilterSettingColumn)(speed)) != 0)
	#define WP_SP_SET_FIXED_FILTER(fc) (SP_SIGNALPROC_SetFixFilter(fc, SP_SIGNALPROC_GetMaxFiltOrder()) != 0)
#endif
#define WP_FCUT_ENABLED(fcut) (((fcut) >= 0.001F) && ((fcut) <= 20.0F))

//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//! Definition of info callback parameters (used only internal)
typedef enum {
	WP_DATA_RVR_CELLNOMINAL = 0,			//!< Cell nominal
	WP_DATA_RVR_SUMOFINTCALWEIGHT			//!< Sum of internal weights
} RB_DECL_TYPE WP_DATA_RVR_tSelect;

//! Definition of parameter XP0312, adjustment load (external adjustment)
typedef float32 RB_DECL_TYPE WP_DATA_tXP0312;	//!< Adjustment load [defUnit]

//! Definition of parameter XP0315, initial zero setting range
typedef struct {
	float32 			lowerLimit;		//!< Lower limit in the definition unit
	float32 			upperLimit;		//!< Upper limit in the definition unit
	} RB_DECL_TYPE WP_DATA_tXP0315;

//! Definition of parameter XP0317, definition unit
typedef uint8_t RB_DECL_TYPE WP_DATA_tXP0317;	//!< Definition unit

//! Definition of parameter XP0320, selectable units for unit 1 -> Main unit
typedef struct {
	uint32_t		unitMask;		//!< Unit mask for unit-1
	int16_t			factory;		//!< Factory setting for unit-1
	} RB_DECL_TYPE WP_DATA_tXP0320;

//! Definition of parameter XP0321, selectable units for unit 2 -> Alternative unit
typedef struct {
	uint32_t		unitMask;		//!< Unit mask for unit-2
	int16_t			factory; 		//!< Factory setting for unit-2
	} RB_DECL_TYPE WP_DATA_tXP0321;

//! Definition of parameter XP0323, LFT break counter
typedef uint16_t RB_DECL_TYPE WP_DATA_tXP0323;	//!< LFT break counter


//! Definition of parameter XP0324, adjustment loads (VariCal)
typedef struct {
	float32			min;			//!< Smallest load in the definition unit
	float32			max;			//!< Biggest load in the definition unit
	float32			increment;		//!< Load increment in the definition unit
	} RB_DECL_TYPE WP_DATA_tXP0324;

//! Definition of parameter XP0325, adjustment tolerances
typedef struct {
	uint16_t		adjustTol;		//!< Tolerance in thousandth part of nominal load
	uint16_t		adjustRepro;	//!< Delta of zero load in digits
	} RB_DECL_TYPE WP_DATA_tXP0325;

//! Definition of parameter XP0326, adjustment parameters (FACT)
typedef struct {
	uint16_t 		quantity;		//!< Adjustment quantity following power on within time_1
	uint16_t 		time_1;			//!< Time following power on in hours
	float32			temperature;	//!< Temperature criterion in degree Celsius
	uint16_t 		time_2;			//!< Time criterion in hours
	} RB_DECL_TYPE WP_DATA_tXP0326;

//! Definition of stability parameter values for XP0327
typedef struct {
	float32			stabWidth;		//!< Stability signal width in digits
	float32			stabTime;		//!< Stability observation time in seconds
	} RB_DECL_TYPE WP_DATA_tXP0327;

//! Definition of display hysteresis values for XP0329
typedef float32 RB_DECL_TYPE WP_DATA_tXP0329;	//!< Width in digits

//! Definition of parameter XP0330, auto zero
typedef struct {
	float32			drift;			//!< Drift in digits/second
	float32			maxCorr;		//!< Maximum correction in digits
	uint16_t		setting;		//!< Index for auto zero setting, see manual
	} RB_DECL_TYPE WP_DATA_tXP0330;

//! Definition of parameter XP0331, S-curve
typedef struct {
	float32			width;			//!< Effective range in digits
	float32			strength;		//!< Zero setting range in digits
	} RB_DECL_TYPE WP_DATA_tXP0331;

//! Definition of parameter XP0333, adjustment configuration
typedef uint16_t RB_DECL_TYPE WP_DATA_tXP0333;	//!< Adjustment configuration

//! Definition of parameter XP0334, units with reduced resolution
typedef struct {
	uint32_t		unitMask;		//!< Unit mask for reduced units
	uint16_t		reduction;		//!< Reduction, number of decimals
	} RB_DECL_TYPE WP_DATA_tXP0334;

//! Definition of parameter XP0335,legal for trade brackets
typedef uint16_t RB_DECL_TYPE WP_DATA_tXP0335;	//!< LFT configuration

//! Definition of parameter XP0337, switch on range
typedef struct {
	float32			lowerLimit;		//!< Lower limit in the definition unit
	float32			upperLimit;		//!< Upper limit in the definition unit
	} RB_DECL_TYPE WP_DATA_tXP0337;

//! Definition of parameter XP0338, fine range definition (one line only)
typedef	struct {
	float32			limit;			//!< Upper limit in definition unit
	uint16_t		dispIncrement;	//!< Display increment
	uint16_t		anchor;			//!< Anchor	of zero	value
	} RB_DECL_TYPE WP_DATA_tXP0338;

//! Definition of parameter XP0339, total range definition
typedef struct {
	int32_t			dp;				//!< Number of decimal places
	float32			lowerLimit;		//!< Lower limit in the definition unit
	float32			upperLimit;		//!< Upper limit in the definition unit
	uint16_t		dispIncrement;	//!< Display increment
	uint16_t		anchor;			//!< Anchor of zero value
	} RB_DECL_TYPE WP_DATA_tXP0339;

//! Definition of parameter XP0340, timeout
typedef uint16_t RB_DECL_TYPE WP_DATA_tXP0340;	//!< Timeout

//! Definition of parameter XP0344, number of internal loads
typedef uint16_t RB_DECL_TYPE WP_DATA_tXP0344;	//!< Number of internal loads

//! Definition of parameter XP0348, selectable weighing modes and default
typedef struct {
	uint8_t			modeMask;		//!< Mask for modes
	uint8_t			defMode;		//!< Default mode
	} RB_DECL_TYPE WP_DATA_tXP0348;

//! Definition of adaptive display hysteresis values for XP0354
typedef float32 RB_DECL_TYPE WP_DATA_tXP0354;	//!< Width in digits

//! Definition of parameter XP0355, gradation of the stability parameters
typedef struct {
	float32			widthFactor;		//!< Factor for stability width
	float32			timeFactor;			//!< Factor for stability time
	} RB_DECL_TYPE WP_DATA_tXP0355;

//! Definition of parameter XP0368, sum of internal loads
typedef float32 RB_DECL_TYPE WP_DATA_tXP0368;	//!< Sum of internal loads

//! Definition of stability parameter XP0369 for fine range
typedef struct {
	float32			stabWidth;		//!< Stability signal width in digits
	float32			stabTime;		//!< Stability observation time in seconds
	} RB_DECL_TYPE WP_DATA_tXP0369;

//! Definition of parameter XP0370, zero setting range
typedef struct {
	float32			lowerLimit;		//!< Lower limit in the definition unit
	float32			upperLimit;		//!< Upper limit in the definition unit
	} RB_DECL_TYPE WP_DATA_tXP0370;

//! Definition of parameter XP0371, selectable environment conditions
typedef struct {
	uint16_t		modeMask;		//!< Mask for modes
	uint16_t		defMode;		//!< Default mode
	} RB_DECL_TYPE WP_DATA_tXP0371;

//! Definition of parameter XP0372, selectable value release settings
typedef struct {
	uint16_t		modeMask;		//!< Mask for modes
	uint16_t		defMode;		//!< Default mode
	} RB_DECL_TYPE WP_DATA_tXP0372;

//! Definition of parameter XP0373, range lock mode
typedef uint16_t RB_DECL_TYPE WP_DATA_tXP0373;	//!< Range lock mode

//! Definition of parameter XP0377, GEO point of calibration
typedef int8_t RB_DECL_TYPE WP_DATA_tXP0377;	//!< GEO code

//! Definition of parameter XP0387, Stability Beep Option
typedef struct {
	uint32_t		selectableBeeps;	//!< Mask for selectable beeps
	uint16_t		factory;			//!< Factory beep setting
	} RB_DECL_TYPE WP_DATA_tXP0387;

//! Definition of parameter XP0389, AD-Converter specification
typedef struct {
	uint16_t		adc;			//!< ADC parameter
	uint16_t		fRAR;			//!< Frequency of RAR (Sample input)
	uint16_t		fSP;			//!< Frequency of Signal processing (Sample output)
	uint16_t		wRAR;			//!< Width of RAR
	uint16_t		diff;			//!< Differential mode
	} RB_DECL_TYPE WP_DATA_tXP0389;

//! Definition of parameter XP0390, GEO point of use
typedef int8_t RB_DECL_TYPE WP_DATA_tXP0390;	//!< GEO code

//! Definition of parameter XP0397, Zero behaviour
typedef uint8_t RB_DECL_TYPE WP_DATA_tXP0397;	//!< Zero behaviour

//! Definition of parameter XP0398, Tare behaviour
typedef uint8_t RB_DECL_TYPE WP_DATA_tXP0398;	//!< Tare behaviour

//! Definition of parameter XP0399, power compensation
typedef struct {
	float32			offset;			//!< Offset
	float32			x1;				//!< x^1
	float32			x2;				//!< x^2
	float32			x3;				//!< x^3
	} RB_DECL_TYPE WP_DATA_tXP0399;

//! Definition of default filter settings
typedef struct {
	int16_t			tableNr;		//!< Filter table number
	int16_t			setNr;			//!< Filter set number
} RB_DECL_TYPE WP_DATA_tXP0502;

//! Definition of parameter XP0610, Approval mode enable/disable
typedef uint8_t RB_DECL_TYPE WP_DATA_tXP0610;	//!< Approval enable

//! Definition of parameter XP0620, step control mode enable/disable
typedef bool16 RB_DECL_TYPE WP_DATA_tXP0620;	//!< step control enable

//! Definition of parameter XP0630, default method for XP0621-XP0629 commands
typedef uint8_t	RB_DECL_TYPE WP_DATA_tXP0630;

//! Definition of parameter XP9630, default method for XP0632-XP063x commands (second array)
typedef WP_DATA_tXP0630	RB_DECL_TYPE WP_DATA_tXP9630;

//! Definition of parameter XP0631, disabled method for XP0621-XP0629 commands
typedef uint32_t RB_DECL_TYPE WP_DATA_tXP0631;

//! Definition of parameter XP0631, disabled method for XP0632-XP063x commands (second array)
typedef WP_DATA_tXP0631	RB_DECL_TYPE WP_DATA_tXP9631;

//! Definition of parameter XP0901, tare setting range
typedef struct {
	float32			lowerLimit;		//!< Lower limit in the definition unit
	float32			upperLimit;		//!< Upper limit in the definition unit
	} RB_DECL_TYPE WP_DATA_tXP0901;

//! Definition of parameter XP0902, pretare setting range
typedef struct {
	float32			lowerLimit;		//!< Lower limit in the definition unit
	float32			upperLimit;		//!< Upper limit in the definition unit
	} RB_DECL_TYPE WP_DATA_tXP0902;

//! Definition of parameter XP0903, approval type
typedef uint32_t RB_DECL_TYPE WP_DATA_tXP0903;	//!< Approval type

//! Definition of parameter XP0904, units with reduced resolution
typedef struct {
	uint32_t		unitMask;		//!< Unit mask for reduced units
	float32			reduction;		//!< Reduction factor
	} RB_DECL_TYPE WP_DATA_tXP0904;

//! Definition of parameter XP0907, Gross calculation setting
typedef uint8_t RB_DECL_TYPE WP_DATA_tXP0907;

//! Definition of parameter XP0909, minimum load definitions
typedef float32 RB_DECL_TYPE WP_DATA_tXP0909;

//! Definition of parameter XP0910, declared maximum capacity
typedef float32 RB_DECL_TYPE WP_DATA_tXP0910;

//! Definition of parameter XP0911, accuracy class
typedef uint8_t RB_DECL_TYPE WP_DATA_tXP0911;

//! Definition of parameter XP0912, enable/disable AutoTare factory default
typedef bool16 RB_DECL_TYPE WP_DATA_tXP0912;

//! Definition of parameter XP0913, AutoTare configuration factory default
typedef struct {
	float32 	AutoTareThreshold;					//!< AutoTareThreshold in definition unit
	float32		AutoRetriggerTareThreshold;			//!< AutoRetriggerTareThreshold in definition unit
	bool16		AutoTareStabilityCheck;				//!< AutoTare stability check (true=check stability, false=ignore stability)
	bool16		AutoRetriggerTareStabilityCheck;	//!< AutoTare stability check (true=check stability, false=ignore stability)
	} RB_DECL_TYPE WP_DATA_tXP0913;

//! Definition of parameter XP0914, enable/disable AutoClearTare factory default
typedef bool16 RB_DECL_TYPE WP_DATA_tXP0914;

//! Definition of parameter XP0915, AutoClearTare configuration factory default
typedef struct {
	float32 	AutoClearTareThreshold;			//!< AutoClearTareThreshold in definition unit
	bool16		AutoClearTareStabilityCheck;	//!< AutoClearTare stability check (true=check stability, false=ignore stability)
	} RB_DECL_TYPE WP_DATA_tXP0915;

//! Definition of parameter XP0916, Tare Reduction Mode
typedef uint8_t RB_DECL_TYPE WP_DATA_tXP0916;

//! Definition of parameter XP0918, Definition unit for 2nd range detector
typedef uint8_t RB_DECL_TYPE WP_DATA_tXP0918;

//! Definition of parameter XP0919, range definition for 2nd range detector (one line only)
typedef struct {
	float32			lowerLimit;		//!< Lower limit in the definition unit
	float32			upperLimit;		//!< Upper limit in the definition unit
	} RB_DECL_TYPE WP_DATA_tXP0919;

//! Definition of parameter XP0920, unit mask for 2nd range detector
typedef uint32_t RB_DECL_TYPE WP_DATA_tXP0920;	//!< Unit mask

//! Definition of parameter XP0921, maximum capacity for 2nd range detector
typedef float32 RB_DECL_TYPE WP_DATA_tXP0921;

//! Definition of parameter XP0922, minimum load for 2nd range detector
typedef float32 RB_DECL_TYPE WP_DATA_tXP0922;

//! Definition of parameter XP0928, Readability Control
typedef uint8_t RB_DECL_TYPE WP_DATA_tXP0928;

//! Definition of parameter XP0929, Under load behaviour
typedef uint8_t RB_DECL_TYPE WP_DATA_tXP0929;

//! Definition of parameter XP0931, FACT adjustment procedure description
typedef struct {
	uint8_t			adjustType;		//!< Adjust type
	uint8_t			adjustMethode;	//!< Adjust method
	} RB_DECL_TYPE WP_DATA_tXP0931;

//! Definition of parameter XP0932, One Time Adjustment Configuration
typedef struct {
	uint8_t			state;
	uint8_t			maxCount;
} RB_DECL_TYPE WP_DATA_tXP0932;

//! Definition of parameter XP0932, One Time Adjustment Counter
typedef uint8_t RB_DECL_TYPE WP_DATA_tXP0933;

//! Definition of parameter XP0935, Customer standard adjustment tolerances
typedef float32 RB_DECL_TYPE WP_DATA_tXP0935;	//!< Tolerance in thousandth part of internal load

//! Definition of parameter XP0937, Initial FACT disable/enable
typedef uint16_t RB_DECL_TYPE WP_DATA_tXP0937;		//!< 0 = disabled, 1 = enabled

//! Definition of parameter XP0938, Default for M82
typedef float32 RB_DECL_TYPE WP_DATA_tXP0938;		//!< Upper limit in the definition unit

//! Definition of parameter XP0940, ProFACT Time Factory Settings
typedef struct {
	uint8_t			hours;			//!< Hours (0..23)
	uint8_t			minutes;		//!< Minutes (0..59)
	uint8_t			seconds;		//!< Seconds (0..59)
	uint8_t			enable;			//!< Enable (0..1)
	} RB_DECL_TYPE WP_DATA_tXP0940;

//! Definition of parameter XP0941, ProFACT Weekday Factory Settings
typedef uint8_t RB_DECL_TYPE WP_DATA_tXP0941; //!< Weekday mask (Monday=2^0, Tuesday=2^1,..)

//! Definition of parameter XP0943, maximum adjustment pre-load
typedef float32 RB_DECL_TYPE WP_DATA_tXP0943;

//! Definition of parameter XP0947, GeoCode at Point of Calibration - HighRes
typedef float32 RB_DECL_TYPE WP_DATA_tXP0947;

//! Definition of parameter XP0948, GeoCode at Point of Use - HighRes
typedef float32 RB_DECL_TYPE WP_DATA_tXP0948;

//! Definition of parameter XP0949, supported scales
typedef struct {
	float32			totalRange;		//!< Total range in the definition unit
	float32			fineRange2;		//!< Fine range 2 in the definition unit
	float32			fineRange3;		//!< Fine range 3 in the definition unit
	float32			fineRange4;		//!< Fine range 4 in the definition unit
	float32			fineRange5;		//!< Fine range 5 in the definition unit
	uint8_t			definitionUnit;	//!< Definition unit
	uint16_t		resolution;		//!< Resolution
	uint16_t		rangeMode;		//!< Range Mode
	uint8_t			accuracyClass;	//!< Accuracy class
	int8_t			IZmin;			//!< Initial zero lower limit in percent of totalRange
	int8_t			IZmax;			//!< Initial zero upper limit in percent of totalRange
	} RB_DECL_TYPE WP_DATA_tXP0949;

//! Definition of parameter XP0950, Scale configuration
typedef struct {
	uint8_t			set;			//!< Set selector
	uint32_t		approvalType;	//!< Approval type
	} RB_DECL_TYPE WP_DATA_tXP0950;

//! Definition of parameter XP0951, Scale configuration (2nd range detector)
typedef struct {
	uint8_t			set;			//!< Set selector
	uint32_t		approvalType;	//!< Approval type
	} RB_DECL_TYPE WP_DATA_tXP0951;

//! Definition of parameter XP0955, Behaviour of initial zero
typedef uint16_t RB_DECL_TYPE WP_DATA_tXP0955;

//! Definition of parameter XP0956, Nominal Load of Load Cell
typedef float32 RB_DECL_TYPE WP_DATA_tXP0956;

//! Definition of parameter XP0959, Selectable readability
typedef struct {
	uint32_t		mask;			//!< Mask for selection
	uint16_t		factory;		//!< Factory setting
	} RB_DECL_TYPE WP_DATA_tXP0959;

//! Definition of parameter XP0965, HB44 sealing category
typedef uint8_t RB_DECL_TYPE WP_DATA_tXP0965;	//!< HB44 Sealing category

//! Definition of parameter XM1004, MinWeigh methods designation and test parameters
typedef struct {
	char		methodDesignation[20];	//!< Designation of the method
	char		testParameter[20];		//!< Test parameters
	} RB_DECL_TYPE WP_DATA_tXM1004;

//! Definition of parameter XM1005, MinWeigh limits
typedef struct {
	float32		tare;			//!< Tare value in definition unit
	float32		minimalWeight;	//!< Minimum initial weight in definition unit
	} RB_DECL_TYPE WP_DATA_tXM1005;

//! Definition of parameter XM1006, option of the selected weighing attitude
typedef uint16_t RB_DECL_TYPE WP_DATA_tXM1006;

//! Definition of parameter XM1007, MinWeigh next test date
typedef struct {
	uint8_t		day;		//!< Day (date of the month)
	uint8_t		month;		//!< Month (number of the month in the year)
	uint16_t	year;		//!< Year
	} RB_DECL_TYPE WP_DATA_tXM1007;

//! Definition of parameter XM1032, Default SI Behaviour
typedef uint8_t RB_DECL_TYPE WP_DATA_tXM1032;

//! Definition of parameter XM1033, Scaled Ramp Weight Value Configuration
typedef struct {
	float32			offset;			//!< Offset
	float32			amp;			//!< Amplification factor
	} RB_DECL_TYPE WP_DATA_tXM1033;

//! Definition of parameter XP0962, Display Resolution Factors
typedef struct {
	uint16_t		decMask;		//!< Mask for decreased resolutions
	uint16_t		incMask;		//!< Mask for increased resolutions
	} RB_DECL_TYPE WP_DATA_tXP0962;

//! Definition of parameter XM2107, MinWeigh repeatability limit
typedef struct {
	float32		limit;		//!< Repeatability limit in [unit]
	char		unit[6];	//!< Unit of value
	} RB_DECL_TYPE WP_DATA_tXM2107;

//! Definition of parameter C0, Inquiry/Setting of Adjustment WeightType
typedef uint16_t RB_DECL_TYPE WP_DATA_tC0;	//!< 0 internal, 1 external weight)

//! Definition of parameter M01, weighing mode
typedef uint8_t RB_DECL_TYPE WP_DATA_tM01;

//! Definition of parameter M02, environment mode
typedef uint8_t RB_DECL_TYPE WP_DATA_tM02;

//! Definition of parameter M03, auto-zero mode
typedef uint8_t RB_DECL_TYPE WP_DATA_tM03;

//! Definition of parameter M17, FACT time criteria
typedef struct {
	uint8_t		hours;			//!< Hours (0..23)
	uint8_t		minutes;		//!< Minutes (0..59)
	uint8_t		enable;			//!< Enable (0..1)
	uint8_t		weekdaysMask;	//!< Weekday mask (Monday=2^0, Tuesday=2^1,..)
	} RB_DECL_TYPE WP_DATA_tM17;

//! Definition of parameter M18, FACT temperature criteria
typedef uint8_t RB_DECL_TYPE WP_DATA_tM18;

//! Definition of parameter M19, Adjustment weight
typedef WP_tFloat RB_DECL_TYPE WP_DATA_tM19;

//! Definition of parameter M20, Test weight
typedef WP_tFloat RB_DECL_TYPE WP_DATA_tM20;

//! Definition of parameter M21, Unit selection
typedef uint8_t RB_DECL_TYPE WP_DATA_tM21;

//! Definition of parameter M22, Custom unit
typedef struct {
	uint16_t	formula;		//!< Formula selection
	uint16_t	rounding;		//!< Rounding selection
	WP_tFloat	factor;			//!< Conversion factor for custom unit
	WP_tFloat	offset;			//!< Offset value (in custom units)
	WP_tFloat	resolution;		//!< Rounding resolution of custom unit value
	char		unitText[8];	//!< Unit text
} RB_DECL_TYPE WP_DATA_tM22;

//! Custom unit formula selectors (according to MT-SICS manual, command M94)
#define WP_DATA_CU_FORMULA_NONE		0u		//!< Custom unit selector not set
#define WP_DATA_CU_FORMULA_MUL		1u		//!< Result = Offset + (Factor * Weight)
#define WP_DATA_CU_FORMULA_DIV		2u		//!< Result = Offset + (Factor / Weight)

//! Custom unit rounding selectors (according to MT-SICS manual, command M94)
#define WP_DATA_CU_ROUND_NONE		0u		//!< Based on raw weight
#define WP_DATA_CU_ROUND_RANGE		1u		//!< Based on rounded weight (Range resolution)
#define WP_DATA_CU_ROUND_BASE		2u		//!< Based on rounded weight (Base resolution)

//! Definition of parameter M23, Readability
typedef uint8_t RB_DECL_TYPE WP_DATA_tM23;

//! Definition of parameter M29, Value release
typedef uint8_t RB_DECL_TYPE WP_DATA_tM29;

//! Definition of MinWeigh method (see also M34 command)
typedef uint16_t RB_DECL_TYPE WP_DATA_tM34;

//! Definition of zero mode after startup (see also M35 command)
typedef uint32_t RB_DECL_TYPE WP_DATA_tM35;
#define WP_DATA_ZEROMODE_STD        0uL     //!< Use standard zero mode after startup
#define WP_DATA_ZEROMODE_PERMANENT  1uL     //!< Use permanent zero after startup, save now
#define WP_DATA_ZEROMODE_CONTINUOUS 2uL		//!< Use permanent zero after startup, save each zero

//! Definition of tare mode after startup (see also M49 command)
typedef uint32_t RB_DECL_TYPE WP_DATA_tM49;
#define WP_DATA_TAREMODE_STD        0uL	    //!< Use standard tare mode after startup
#define WP_DATA_TAREMODE_PERMANENT  1uL	    //!< Use permanent zero after startup, save now
#define WP_DATA_TAREMODE_CONTINUOUS 2uL	    //!< Use permanent zero after startup, save each zero


//! Definition of parameter M60, enable/disable AutoTare
typedef bool16 RB_DECL_TYPE WP_DATA_tM60;

//! Definition of parameter M61, AutoTare configuration
typedef struct {
	float32 	AutoTareThreshold;					//!< AutoTareThreshold in grams
	float32		AutoRetriggerTareThreshold;			//!< AutoRetriggerTareThreshold in grams
	bool16		AutoTareStabilityCheck;				//!< AutoTare stability check (true=check stability, false=ignore stability)
	bool16		AutoRetriggerTareStabilityCheck;	//!< AutoTare stability check (true=check stability, false=ignore stability)
	} RB_DECL_TYPE WP_DATA_tM61;

//! Definition of parameter M62, enable/disable AutoClearTare
typedef bool16 RB_DECL_TYPE WP_DATA_tM62;

//! Definition of parameter M63, AutoClearTare configuration
typedef struct {
	float32 	AutoClearTareThreshold;			//!< AutoClearTareThreshold in gram
	bool16		AutoClearTareStabilityCheck;	//!< AutoClearTare stability check (true=check stability, false=ignore stability)
	} RB_DECL_TYPE WP_DATA_tM63;

//! Definition of parameter M65, SI Behaviour
typedef uint8_t RB_DECL_TYPE WP_DATA_tM65;		//!< 0 = return immediately(last weight), 1 = return a new computed weight

//! Definition of parameter M67, Weight sample timeout
typedef uint16_t RB_DECL_TYPE WP_DATA_tM67;		//!< Timeout in seconds (0 ... n)

//! Definition of parameter M74, Stability Beep Volume
typedef uint8_t RB_DECL_TYPE WP_DATA_tM74;		//!< 0 = Off, 1 = Low, 2 = Medium, 3 = High

//! Definition of parameter M75, Switch ON/OFF FACT Protocol
typedef bool16 RB_DECL_TYPE WP_DATA_tM75;		//!< 0 = Off, 1 = On

//! Definition of parameter M78, Switch on/off Weight Recall Function
typedef bool16 RB_DECL_TYPE WP_DATA_tM78;		//!< 0 = Off, 1 = On

//! Definition of parameter M79, Inquiry/Setting of Balance Startup Mode
typedef uint16_t RB_DECL_TYPE WP_DATA_tM79;		//!< 0 .. 1(product dependent)

//! Definition of parameter M82, Inquiry/Setting of Actual Zero/Tare Key Zero Range Setting
typedef float32 RB_DECL_TYPE WP_DATA_tM82;		//!< Upper limit in the definition unit

//! Definition of parameter M93, FACT enable
typedef bool16 RB_DECL_TYPE WP_DATA_tM93;		//!< FACT enable

//! Definition of parameter M110, Display resolution
typedef int8_t RB_DECL_TYPE WP_DATA_tM110;

//! Definition of parameter C5, step control mode enable/disable
typedef bool16 RB_DECL_TYPE WP_DATA_tC5;		//!< step control enable

//! Definition of parameter Permanent Zero
typedef struct {
	WP_tFloat 	Zero;							//!< Value of zero
	WP_tFloat	InitZero;						//!< Value of initial zero
	} RB_DECL_TYPE WP_DATA_tPermZero;


//! Definition of parameter Permanent Tare
typedef WP_tFloat RB_DECL_TYPE WP_DATA_tPermTare;

//! Definition of user stability parameter USTB
typedef struct {
	float32			stabWidth;		//!< Stability signal width in digits
	float32			stabTime;		//!< Stability observation time in seconds
	} RB_DECL_TYPE WP_DATA_tUSTB;
#define WP_DATA_USTB_WEIGHING	0	//!< USTB parameter index for weighing and testing
#define WP_DATA_USTB_TARE		1	//!< USTB parameter index for tareing
#define WP_DATA_USTB_ZERO		2	//!< USTB parameter index for zeroing

//! Definition of parameter cutoff frequency
typedef float32 RB_DECL_TYPE WP_DATA_tFCUT;

//! Definition of parameter Dummy, only needed if no other parameters are used to prevent empty struct
typedef uint8_t RB_DECL_TYPE WP_DATA_tDummy;

//! Speaking names for some data types
typedef WP_DATA_tXP0397 RB_DECL_TYPE WP_DATA_tZeroMode;				//!< Zero behavior
typedef WP_DATA_tXP0398 RB_DECL_TYPE WP_DATA_tTareMode;				//!< Tare behavior
typedef WP_DATA_tXP0916 RB_DECL_TYPE WP_DATA_tTareUnidirectional;	//!< Tare direction
typedef WP_DATA_tXP0929 RB_DECL_TYPE WP_DATA_tUnderloadMode;		//!< Underload behavior
typedef WP_DATA_tXP0947 RB_DECL_TYPE WP_DATA_tGeoCodeCal;			//!< Geo Code at Cal
typedef WP_DATA_tXP0948 RB_DECL_TYPE WP_DATA_tGeoCodeUse;			//!< Geo Code at Use
typedef WP_DATA_tXP0955 RB_DECL_TYPE WP_DATA_tInitZeroMode;			//!< Initial zero behavior

typedef WP_DATA_tM01   RB_DECL_TYPE WP_DATA_tWeighingMode;			//!< weighing mode
typedef WP_DATA_tM02   RB_DECL_TYPE WP_DATA_tEnvironmentMode;		//!< environment mode
typedef WP_DATA_tM03   RB_DECL_TYPE WP_DATA_tAutoZeroMode;			//!< auto-zero mode
typedef WP_DATA_tM21   RB_DECL_TYPE WP_DATA_tUnitSelection;			//!< Unit selection
typedef WP_DATA_tM22   RB_DECL_TYPE WP_DATA_tCustomUnit;			//!< Custom unit
typedef WP_DATA_tM23   RB_DECL_TYPE WP_DATA_tReadability;			//!< Readability
typedef WP_DATA_tM34   RB_DECL_TYPE WP_DATA_tMinWeighMethod;		//!< MinWeigh method
typedef WP_DATA_tM35   RB_DECL_TYPE WP_DATA_tPermanentZeroMode;		//!< Permanent zero mode
typedef WP_DATA_tM49   RB_DECL_TYPE WP_DATA_tPermanentTareMode;		//!< Permanent tare mode
typedef WP_DATA_tM60   RB_DECL_TYPE WP_DATA_tAutoTareEnable;		//!< AutoTareEnable
typedef WP_DATA_tM61   RB_DECL_TYPE WP_DATA_tAutoTareConfig;		//!< AutoTareConfig
typedef WP_DATA_tM62   RB_DECL_TYPE WP_DATA_tAutoClearTareEnable;	//!< AutoClearTareEnable
typedef WP_DATA_tM63   RB_DECL_TYPE WP_DATA_tAutoClearTareConfig;	//!< AutoClearTareConfig
typedef WP_DATA_tM93   RB_DECL_TYPE WP_DATA_tFACTEnable;			//!< FACT enable
typedef WP_DATA_tM110  RB_DECL_TYPE WP_DATA_tDisplayResolution;		//!< Display resolution

typedef WP_DATA_tC0    RB_DECL_TYPE WP_DATA_tAdjustmentWeightType;	//!< Adjustment WeightType
typedef WP_DATA_tUSTB  RB_DECL_TYPE WP_DATA_tUserStability;			//!< User stability
typedef WP_DATA_tFCUT  RB_DECL_TYPE WP_DATA_tCutoffFrequency;		//!< Cutoff frequency

//--------------------------------------------------------------------------------------------------
// WP parameter set definition
//--------------------------------------------------------------------------------------------------
// Simple and array data types
#define WP_T_S		0u	//!< Simple parameter type
#define WP_T_A0		1u	//!< Array parameter, index starts with 0
#define WP_T_A1		2u	//!< Array parameter, index starts with 1
#define WP_T_A00	3u	//!< 2D Array parameter, first index starts with 0, second with 0
#define WP_T_A01	4u	//!< 2D Array parameter, first index starts with 0, second with 1
#define WP_T_A10	5u	//!< 2D Array parameter, first index starts with 1, second with 0
#define WP_T_A11	6u	//!< 2D Array parameter, first index starts with 1, second with 1

//! Short cut for the s20 format
#define WP_F_S20	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

//! Parameter set configuration. The macros below are defined in WP_Typedefs.h:
//! - WP_FX_CNFPAR_STD_ is used with standard type definition (XP0317, XP0338, XP0339, ...).
//! - WP_FX_CNFPAR_RED_ is used with alternate scale configuration (XP0949, XP0950, ...).
//! - WP_FX_CNFPAR_COM_ is used for common configuration parameters.
//! - WP_FX_RTPAR_COM_ is used for common run time parameters (typically M commands).
//! - WP_FX_BASIC_ depends only on RB_CONFIG_USE_WP.
//! - WP_FX_CMD_xy_ depends on RB_CONFIG_USE_WP, related configuration switches and RB_PARAM definitions.
//! - WP_FX_xyz_ depends on RB_CONFIG_USE_WP and the related configuration switches.
//! - WP_FX_ must be defined locally to expand the given parameters and undefined afterwards.
//! All WP_FX_xyz_ expands either to an empty line or to its argument.
//! The following list defines all parameter of weighing package:
//! - name			Name of parameter
//! - parameter		ID of parameter
//! - count0		Number of first array elements (1 or more)
//! - count1		Number of second array elements (1 or more)
//! - type			Type of parameter (WP_T_S, WP_T_A0, WP_T_A00 and more)
//! - format		Storage format of one array element
//! - checkfunction	Function to check parameter before writing to NV storage
//! - default		Default value for one array element in the form name = {default}
//! Each line of table must have the same syntax terminated with a backslash. Some parts of this
//! table are expanded in other files, function names or other symbolic names must be
//! defined in the file where it is expanded, e.g. check functions must be defined in WP_Param.
//!

#define WP_DATA_PARAMETER_LIST \
	/* WP_FX_CNFPAR_COM_(  WP_FX_BASIC_( WP_FX_( name      parameter             count0               count1              type        format                          checkfunction   {default} ))) */	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0312  , RB_PARAM_ID_XP( 312), 1					, 1					, WP_T_S   	, "g"                   		, 0				, 0.0F )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0315  , RB_PARAM_ID_XP( 315), 1					, 1					, WP_T_S   	, "S{g;g}"              		, 0				, {-1.0E29F,1.0E29F} )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0317  , RB_PARAM_ID_XP( 317), 1					, 1					, WP_T_S   	, "k20"                 		, 0				, 0 )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0320  , RB_PARAM_ID_XP( 320), 1					, 1					, WP_T_S   	, "S{m;i20 -1}"         		, 0				, {0xFFFFFFFF,0} )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0321  , RB_PARAM_ID_XP( 321), 1					, 1					, WP_T_S   	, "S{m;i20 -1}"         		, 0				, {0xFFFFFFFF,0} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0323  , RB_PARAM_ID_XP( 323), 1					, 1					, WP_T_S   	, "n0"                  		, 0				, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0324  , RB_PARAM_ID_XP( 324), 1					, 1					, WP_T_S   	, "S{g;g;g}"            		, 0				, {0.0F,0.0F,0.0F} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0325  , RB_PARAM_ID_XP( 325), 1					, 1					, WP_T_S   	, "S{n;n}"              		, 0				, {30u,3u} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_FACT_(      WP_FX_( XP0326  , RB_PARAM_ID_XP( 326), 1					, 1					, WP_T_S   	, "S{n;n;g;n}"          		, 0				, {0,0,0.0F,0} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0327  , RB_PARAM_ID_XP( 327), 1					, WP_DATA_N_XP0327	, WP_T_A0 	, "S{g;g}"              		, 0				, {0.5F,1.0F} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_HYSTCORR_(  WP_FX_( XP0329  , RB_PARAM_ID_XP( 329), 1					, WP_DATA_N_XP0329	, WP_T_A0 	, "g"                   		, 0				, 0.0F )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0330  , RB_PARAM_ID_XP( 330), 1					, 1					, WP_T_S   	, "S{g;g;n3}"           		, 0				, {0.0F,0.0F,0} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_S_CURVE_(   WP_FX_( XP0331  , RB_PARAM_ID_XP( 331), 1					, WP_DATA_N_XP0331	, WP_T_A0 	, "S{g;g}"              		, 0				, {0.0F,0.0F} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0333  , RB_PARAM_ID_XP( 333), 1					, 1					, WP_T_S   	, "n511"                   		, 0				, 1u )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0334  , RB_PARAM_ID_XP( 334), 1					, 1					, WP_T_S   	, "S{m;n12}"            		, 0				, {0x00000000,0} )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0335  , RB_PARAM_ID_XP( 335), 1					, 1					, WP_T_S   	, "n6"                  		, check_XP0335	, 0 )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0337  , RB_PARAM_ID_XP( 337), 1					, 1					, WP_T_S   	, "S{g;g}"              		, 0				, {-1.0E29F,1.0E29F} )))	\
	WP_FX_CNFPAR_STD_( WP_FX_RANGES_(    WP_FX_( XP0338  , RB_PARAM_ID_XP( 338), 1					, WP_DATA_N_XP0338	, WP_T_A1	, "S{g;n;n3}"           		, 0				, {0.0F,0,0} )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0339  , RB_PARAM_ID_XP( 339), 1					, 1					, WP_T_S   	, "S{l;g;g;n1000 1;n3}"			, 0				, {2,-1.0E29F,1.0E29F,1u,1u} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0340  , RB_PARAM_ID_XP( 340), 1					, WP_DATA_N_XP0340	, WP_T_A0 	, "n65535 1"            		, 0				, 30u )))	\
	WP_FX_CNFPAR_COM_( WP_FX_CALWGT_(    WP_FX_( XP0344  , RB_PARAM_ID_XP( 344), 1					, 1					, WP_T_S   	, "n2"                  		, 0				, WP_CALWGT_NR)))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0348  , RB_PARAM_ID_XP( 348), 1					, 1					, WP_T_S   	, "S{k;k7}"         			, 0				, WP_SP_DEF_XP0348 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_HYSTCORR_(  WP_FX_( XP0354  , RB_PARAM_ID_XP( 354), 1					, WP_DATA_N_XP0354	, WP_T_A0 	, "g"                   		, 0				, 0.0F )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0355  , RB_PARAM_ID_XP( 355), 1					, 1					, WP_T_S   	, "S{g0.499 0.0;g0.499 0.0}"	, 0				, {0.3F,0.2F} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_CALWGT_(    WP_FX_( XP0368  , RB_PARAM_ID_XP( 368), 1					, 1					, WP_T_S   	, "g"                   		, 0				, 0.0F )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0369  , RB_PARAM_ID_XP( 369), 1					, WP_DATA_N_XP0369	, WP_T_A0 	, "S{g;g}"              		, 0				, {0.5F,1.0F} )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0370  , RB_PARAM_ID_XP( 370), 1					, 1					, WP_T_S   	, "S{g;g}"              		, 0				, {-1.0E29F,1.0E29F} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0371  , RB_PARAM_ID_XP( 371), 1					, 1					, WP_T_S   	, "S{n63 1;n5}"         		, 0				, WP_SP_DEF_XP0371 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0372  , RB_PARAM_ID_XP( 372), 1					, 1					, WP_T_S   	, "S{n31;n4}"           		, 0				, {4u,2u} )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0373  , RB_PARAM_ID_XP( 373), 1					, 1					, WP_T_S   	, "n2"                  		, 0				, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_SP_BLK70_(  WP_FX_( XP0377  , RB_PARAM_ID_XP( 377), 1					, 1					, WP_T_S   	, "j31 -1"              		, check_XP0377	, -1 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0389  , RB_PARAM_ID_XP( 389), 1					, 1					, WP_T_S   	, "S{n4;n;n;n;n2}"        		, check_XP0389	, {0,0,0,0,0} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_SP_BLK70_(  WP_FX_( XP0390  , RB_PARAM_ID_XP( 390), 1					, 1					, WP_T_S   	, "j31 -1"              		, check_XP0390	, -1 )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0397  , RB_PARAM_ID_XP( 397), 1					, 1					, WP_T_S   	, "k1"                  		, 0				, 0 )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0398  , RB_PARAM_ID_XP( 398), 1					, 1					, WP_T_S   	, "k3"                  		, 0				, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_PWRCOMP_(   WP_FX_( XP0399  , RB_PARAM_ID_XP( 399), 1					, 1					, WP_T_S   	, "S{g;g;g;g}"          		, 0				, {0.0F,0.0F,0.0F,0.0F} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_DSP_(       WP_FX_( XP0502  , RB_PARAM_ID_XP( 502), 1					, 1					, WP_T_S   	, "S{i;i}"              		, 0				, {WP_SP_DEFFILTTAB,WP_SP_DEFFILTSET} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0610  , RB_PARAM_ID_XP( 610), 1					, 1					, WP_T_S   	, "k1"                  		, check_XP0610	, 1u )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0620  , RB_PARAM_ID_XP( 620), 1					, 1					, WP_T_S   	, "n1"                  		, 0				, false )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0630  , RB_PARAM_ID_XP( 630), 1					, WP_DATA_N_XP063X	, WP_T_A0 	, "k"                   		, 0				, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP9630  , RB_PARAM_ID_XP(9630), 1					, WP_DATA_N_XP063X	, WP_T_A0 	, "k"                   		, 0				, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0631  , RB_PARAM_ID_XP( 631), 1					, WP_DATA_N_XP063X	, WP_T_A0 	, "m"                   		, 0				, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP9631  , RB_PARAM_ID_XP(9631), 1					, WP_DATA_N_XP063X	, WP_T_A0 	, "m"                   		, 0				, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0901  , RB_PARAM_ID_XP( 901), 1					, 1					, WP_T_S   	, "S{g;g}"              		, 0				, {0.0F,1.0E29F} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0902  , RB_PARAM_ID_XP( 902), 1					, 1					, WP_T_S   	, "S{g;g}"              		, 0				, {0.0F,1.0E29F} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0903  , RB_PARAM_ID_XP( 903), 1					, 1					, WP_T_S   	, "m15"                  		, check_XP0903	, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0904  , RB_PARAM_ID_XP( 904), 1					, 1					, WP_T_S   	, "S{m;g3.4E38 0.0}"            , 0				, {0x00000000,1.0F} )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0907  , RB_PARAM_ID_XP( 907), 1					, 1					, WP_T_S   	, "k2"                  		, 0				, 0 )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0909  , RB_PARAM_ID_XP( 909), 1					, WP_DATA_N_XP0909	, WP_T_A0 	, "g"                   		, 0				, 0.0F )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0910  , RB_PARAM_ID_XP( 910), 1					, 1					, WP_T_S   	, "g"                   		, 0				, 0.0F )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0911  , RB_PARAM_ID_XP( 911), 1					, WP_DATA_N_XP0911	, WP_T_A0 	, "k5"                  		, 0				, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_AUTOTARE_(  WP_FX_( XP0912  , RB_PARAM_ID_XP( 912), 1					, 1					, WP_T_S   	, "n1"                  		, 0				, false )))	\
	WP_FX_CNFPAR_COM_( WP_FX_AUTOTARE_(  WP_FX_( XP0913  , RB_PARAM_ID_XP( 913), 1					, 1					, WP_T_S   	, "S{g;g;n1;n1}"        		, 0				, {0.0F, 0.0F, false, false} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_AUTOTARE_(  WP_FX_( XP0914  , RB_PARAM_ID_XP( 914), 1					, 1					, WP_T_S   	, "n1"                  		, 0				, false )))	\
	WP_FX_CNFPAR_COM_( WP_FX_AUTOTARE_(  WP_FX_( XP0915  , RB_PARAM_ID_XP( 915), 1					, 1					, WP_T_S   	, "S{g;n1}"             		, 0				, {0.0F, false} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0916  , RB_PARAM_ID_XP( 916), 1					, 1					, WP_T_S   	, "k3"                  		, 0				, 0 )))	\
	WP_FX_CNFPAR_STD_( WP_FX_2NDRNGDET_( WP_FX_( XP0918  , RB_PARAM_ID_XP( 918), 1					, 1					, WP_T_S   	, "k20"                 		, 0				, 0 )))	\
	WP_FX_CNFPAR_STD_( WP_FX_2NDRNGDET_( WP_FX_( XP0919  , RB_PARAM_ID_XP( 919), 1					, WP_DATA_N_XP0919	, WP_T_A0 	, "S{g;g}"              		, 0				, {-1.0E29F,1.0E29F} )))	\
	WP_FX_CNFPAR_STD_( WP_FX_2NDRNGDET_( WP_FX_( XP0920  , RB_PARAM_ID_XP( 920), 1					, 1					, WP_T_S   	, "m"                   		, 0				, 0 )))	\
	WP_FX_CNFPAR_STD_( WP_FX_2NDRNGDET_( WP_FX_( XP0921  , RB_PARAM_ID_XP( 921), 1					, 1					, WP_T_S   	, "g"                   		, 0				, 1.0E29F )))	\
	WP_FX_CNFPAR_STD_( WP_FX_2NDRNGDET_( WP_FX_( XP0922  , RB_PARAM_ID_XP( 922), 1					, WP_DATA_N_XP0922	, WP_T_A0 	, "g"                   		, 0				, 0.0F )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0928  , RB_PARAM_ID_XP( 928), 1					, 1					, WP_T_S   	, "k1"                  		, 0				, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0929  , RB_PARAM_ID_XP( 929), 1					, 1					, WP_T_S   	, "k1"                  		, 0				, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_FACT_(      WP_FX_( XP0931  , RB_PARAM_ID_XP( 931), 1					, 1					, WP_T_S   	, "S{k;k}"              		, 0				, {7u,1u} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0932  , RB_PARAM_ID_XP( 932), 1					, 1					, WP_T_S   	, "S{k;k}"              		, 0				, {0,0} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0933  , RB_PARAM_ID_XP( 933), 1					, 1					, WP_T_S   	, "k"                   		, 0				, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0935  , RB_PARAM_ID_XP( 935), 1					, 1					, WP_T_S   	, "g"                   		, 0				, 30.0F )))	\
	WP_FX_CNFPAR_COM_( WP_FX_FACT_(      WP_FX_( XP0937  , RB_PARAM_ID_XP( 937), 1					, 1					, WP_T_S   	, "n1"                  		, 0				, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0938  , RB_PARAM_ID_XP( 938), 1					, 1					, WP_T_S   	, "g"                   		, 0				, 0.0F )))	\
	WP_FX_CNFPAR_COM_( WP_FX_PROFACT_(   WP_FX_( XP0940  , RB_PARAM_ID_XP( 940), 1					, WP_DATA_N_XP0940	, WP_T_A1 	, "S{k23;k59;k59;k1}"   		, 0				, {0,0,0,0} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_PROFACT_(   WP_FX_( XP0941  , RB_PARAM_ID_XP( 941), 1					, 1					, WP_T_S   	, "k127"                		, 0				, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0943  , RB_PARAM_ID_XP( 943), 1					, 1					, WP_T_S   	, "g3.4E38 0.0"         		, 0				, 1.0E29F )))	\
	WP_FX_CNFPAR_COM_( WP_FX_SP_BLK70_(  WP_FX_( XP0947  , RB_PARAM_ID_XP( 947), 1					, 1					, WP_T_S   	, "g31.0 -1.0"          		, check_XP0947	, -1.0F )))	\
	WP_FX_CNFPAR_COM_( WP_FX_SP_BLK70_(  WP_FX_( XP0948  , RB_PARAM_ID_XP( 948), 1					, 1					, WP_T_S   	, "g31.0 -1.0"          		, check_XP0948	, -1.0F )))	\
	WP_FX_CNFPAR_RED_( WP_FX_BASIC_(     WP_FX_( XP0949  , RB_PARAM_ID_XP( 949), 1					, WP_DATA_N_XP0949	, WP_T_A0 	, "S{g3.4E38 0.0;g3.4E38 0.0;g3.4E38 0.0;g3.4E38 0.0;g3.4E38 0.0;k20;n10000;n2;k5;j100 -100;j100 -100}", 0	, {0.0F,0.0F,0.0F,0.0F,0.0F,0,0,0,0,0,0} ))) \
	WP_FX_CNFPAR_RED_( WP_FX_BASIC_(     WP_FX_( XP0950  , RB_PARAM_ID_XP( 950), 1					, 1					, WP_T_S   	, "S{k;m}"						, check_XP0950	, {0,0} )))	\
	WP_FX_CNFPAR_RED_( WP_FX_2NDRNGDET_( WP_FX_( XP0951  , RB_PARAM_ID_XP( 951), 1					, 1					, WP_T_S   	, "S{k;m}"						, check_XP0951	, {0,0} )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0955  , RB_PARAM_ID_XP( 955), 1					, 1					, WP_T_S   	, "n1"							, 0				, 0 )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0956  , RB_PARAM_ID_XP( 956), 1					, 1					, WP_T_S   	, "g3.4E38 0.0"					, 0				, 0.0F )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0959  , RB_PARAM_ID_XP( 959), 1					, 1					, WP_T_S 	, "S{m;n5}"                  	, 0				, {63u,0} )))	\
	WP_FX_CNFPAR_STD_( WP_FX_BASIC_(     WP_FX_( XP0962  , RB_PARAM_ID_XP( 962), 1					, 1					, WP_T_S   	, "S{n;n}"              		, 0				, {0u,0u} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XP0965  , RB_PARAM_ID_XP( 965), 1					, 1					, WP_T_S   	, "k3 1"						, check_XP0965	, 1 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_MIN_WEIGH_( WP_FX_( XM1004  , RB_PARAM_ID_XM(1004), 1					, WP_DATA_N_XM1004	, WP_T_A1 	, "S{s20;s20}"          		, 0				, {WP_F_S20,WP_F_S20} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_MIN_WEIGH_( WP_FX_( XM1005  , RB_PARAM_ID_XM(1005), WP_DATA_M_XM1005	, WP_DATA_N_XM1005	, WP_T_A10	, "S{g3.4E38 0.0;g3.4E38 0.0}"	, 0				, {0.0F,0.0F} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_MIN_WEIGH_( WP_FX_( XM1006  , RB_PARAM_ID_XM(1006), WP_DATA_M_XM1006	, WP_DATA_N_XM1006	, WP_T_A10	, "n"           				, check_XM1006	, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_MIN_WEIGH_( WP_FX_( XM1007  , RB_PARAM_ID_XM(1007), 1					, 1					, WP_T_S   	, "S{k31 1;k12 1;n2099 1970}"	, 0				, {1,1,1997} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XM1032  , RB_PARAM_ID_XM(1032), 1					, 1					, WP_T_S   	, "k1"                  		, 0				, 0 )))	\
	WP_FX_CNFPAR_COM_( WP_FX_BASIC_(     WP_FX_( XM1033  , RB_PARAM_ID_XM(1033), 1					, 1					, WP_T_S   	, "S{g;g}"              		, 0				, {0.0F,1.0F} )))	\
	WP_FX_CNFPAR_COM_( WP_FX_MIN_WEIGH_( WP_FX_( XM2107  , RB_PARAM_ID_XM(2107), 1					, 1					, WP_T_S   	, "S{g3.4E38 0.0;s6}"   		, 0				, {0.0F, {'g',0,0,0,0,0}} )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M01_(   WP_FX_( M01     , RB_PARAM_M01        , 1					, 1					, WP_T_S   	, "k7"                   		, check_M01		, WP_SP_DEF_M01 )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M02_(   WP_FX_( M02     , RB_PARAM_M02        , 1					, 1					, WP_T_S   	, "k4"                  		, check_M02		, WP_SP_DEF_M02 )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M03_(   WP_FX_( M03     , RB_PARAM_M03        , 1					, 1					, WP_T_S   	, "k1"                  		, check_M03		, 0 )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M17_(   WP_FX_( M17     , RB_PARAM_M17        , 1					, WP_DATA_N_M17		, WP_T_A0 	, "S{k23;k59;k59;k127}" 		, 0				, {0,0,0,0} )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M18_(   WP_FX_( M18     , RB_PARAM_M18        , 1					, 1					, WP_T_S   	, "k4"                  		, 0				, 0 )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M19_(   WP_FX_( M19     , RB_PARAM_M19        , 1					, 1					, WP_T_S   	, WGT_FORMAT            		, check_M19_M20	, WP_FLT_ZERO )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M20_(   WP_FX_( M20     , RB_PARAM_M20        , 1					, 1					, WP_T_S   	, WGT_FORMAT            		, check_M19_M20	, WP_FLT_ZERO )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M21_(   WP_FX_( M21     , RB_PARAM_M21        , 1					, WP_DATA_N_M21		, WP_T_A0 	, "k29"                 		, check_M21		, 0 )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M22_(   WP_FX_( M22     , RB_PARAM_M22        , 1					, WP_DATA_N_M22		, WP_T_A0 	, M22_FORMAT            		, 0				, {0, 0, WP_FLT_ZERO, WP_FLT_ZERO, WP_FLT_ZERO, {0,0,0,0,0,0,0,0}} )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M23_(   WP_FX_( M23     , RB_PARAM_M23        , 1					, 1					, WP_T_S   	, "k5"                  		, check_M23		, 0 )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M29_(   WP_FX_( M29     , RB_PARAM_M29        , 1					, 1					, WP_T_S   	, "k4"                  		, check_M29		, 2u )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M34_(   WP_FX_( M34     , RB_PARAM_M34        , 1					, 1					, WP_T_S   	, &M34Format[0]         		, check_M34		, 0 )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M35_(   WP_FX_( M35     , RB_PARAM_M35        , 1					, 1					, WP_T_S   	, "m2"                  		, check_M35		, 0 )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M49_(   WP_FX_( M49     , RB_PARAM_M49        , 1					, 1					, WP_T_S   	, "m2"                  		, check_M49		, 0 )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M60_(   WP_FX_( M60     , RB_PARAM_M60        , 1					, 1					, WP_T_S   	, "n1"                  		, 0				, false )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M61_(   WP_FX_( M61     , RB_PARAM_M61        , 1					, 1					, WP_T_S   	, "S{g;g;n1;n1}"        		, 0				, {9.0F, 0.0F, true, true} )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M62_(   WP_FX_( M62     , RB_PARAM_M62        , 1					, 1					, WP_T_S   	, "n1"                  		, 0				, false )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M63_(   WP_FX_( M63     , RB_PARAM_M63        , 1					, 1					, WP_T_S   	, "S{g;n1}"             		, 0				, {0.5F , true} )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M65_(   WP_FX_( M65     , RB_PARAM_M65        , 1					, 1					, WP_T_S   	, "k1"                  		, 0				, 0 )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M67_(   WP_FX_( M67     , RB_PARAM_M67        , 1					, 1					, WP_T_S   	, "n"                   		, 0				, 40u )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M82_(   WP_FX_( M82     , RB_PARAM_M82        , 1					, 1					, WP_T_S   	, "g"                   		, check_M82		, 0.0F )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M93_(   WP_FX_( M93     , RB_PARAM_M93        , 1					, 1					, WP_T_S   	, "n1"                  		, check_M93		, 1 )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_M110_(  WP_FX_( M110    , RB_PARAM_M110       , 1					, 1					, WP_T_S   	, "j6 -6"						, check_M110	, 0 )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_C0_(    WP_FX_( C0      , RB_PARAM_C0         , 1					, 1					, WP_T_S   	, "n1"                  		, check_C0		, false )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_C5_(    WP_FX_( C5      , RB_PARAM_C5         , 1					, 1					, WP_T_S   	, "n1"                  		, 0				, false )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_USTB_(  WP_FX_( USTB    , RB_PARAM_USTB       , 1					, WP_DATA_N_USTB	, WP_T_A0 	, "S{g1000.0 0.0;g4.0 0.0}" 	, check_USTB	, {0.0F,0.0F} )))	\
	WP_FX_RTPAR_COM_(  WP_FX_CMD_FCUT_(  WP_FX_( FCUT    , RB_PARAM_FCUT       , 1					, 1					, WP_T_S	, "g" 							, check_FCUT	, 0.0F )))	\
	WP_FX_RTPAR_COM_(  WP_FX_PERM_ZERO_( WP_FX_( PermZero, RB_PARAM_PERM_ZERO  , 1					, 1					, WP_T_S  	, WGT2_FORMAT       			, 0				, {WP_FLT_ZERO, WP_FLT_ZERO} )))	\
	WP_FX_RTPAR_COM_(  WP_FX_PERM_TARE_( WP_FX_( PermTare, RB_PARAM_PERM_TARE  , 1					, 1					, WP_T_S  	, WGT_FORMAT        			, 0				, WP_FLT_ZERO )))	\
	WP_FX_PARAM_ANY_(  WP_FX_BASIC_(     WP_FX_( Dummy   , 0                   , 1					, 1					, WP_T_S  	, "k"							, 0				, 0 )))	\


//! Type of global status variable WP_DATA_tValid used for WP_DATA_Valid
//! Example: WP_FX_(M21) expands to uint8_t M21;
#define WP_FX_(n,p,c1,c2,t,f,cf,...)	uint8_t n;
typedef struct {
				WP_DATA_PARAMETER_LIST
				} RB_DECL_TYPE WP_DATA_tValid;
#undef  WP_FX_

//! Helper macros for data definitions. These macros are needed to expand the name to name, name[cnt2]
//! or name[cnt1*cnt2].
//! \attention 2-dimensional array are implemented as 1-dimensional array with size cnt1 * cnt2.
#define WP_A_WP_T_S(n,c1,c2)	n
#define WP_A_WP_T_A0(n,c1,c2)	n[c2]
#define WP_A_WP_T_A1(n,c1,c2)	n[c2]
#define WP_A_WP_T_A00(n,c1,c2)	n[c1*c2]
#define WP_A_WP_T_A01(n,c1,c2)	n[c1*c2]
#define WP_A_WP_T_A10(n,c1,c2)	n[c1*c2]
#define WP_A_WP_T_A11(n,c1,c2)	n[c1*c2]

//! Type of global data variable WP_DATA_tDate used for WP_DATA
//! Example: WP_FX_(M21) expands to WP_DATA_tM21 M21[WP_DATA_N_M21];
#define WP_FX_(n,p,c1,c2,t,f,cf,...)	WP_DATA_t##n WP_A_##t(n,c1,c2);
typedef struct {
				WP_DATA_PARAMETER_LIST
				} RB_DECL_TYPE WP_DATA_tData;
#undef  WP_FX_


//! Declaration of callback function for storing permanent data
typedef RB_DECL_TYPE WP_tStatus (*WP_DATA_tStoreData)(uint16_t ident, const void *pData, int index0, int index1);

//! Declaration of info callback function (used only from WP_Balance)
typedef RB_DECL_TYPE WP_tFloat (*WP_DATA_tInfoCallback)(WP_DATA_RVR_tSelect select);

//==================================================================================================
//  G L O B A L   D A T A
//==================================================================================================

//! Global valid struct
extern WP_DATA_tValid WP_DATA_Valid;

//! Global parameter struct
extern WP_DATA_tData WP_DATA;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// WP_DATA_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialization of the weighing package data module
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_DATA_Initialize(void);


//--------------------------------------------------------------------------------------------------
// WP_DATA_StoreParameter
//--------------------------------------------------------------------------------------------------
//! \brief	Store parameter in permanent memory
//!
//! \param	ident		identifier of parameter
//! \param	pData		pointer to data
//! \param	index0		1st index into 2D array (0 for normal arrays or simple values)
//! \param	index1		2nd index into 2D array (0 for simple values)
//! \return				Status (WP_OK, WP_RANGE_FAIL or WP_STORE_FAIL)
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tStatus WP_DATA_StoreParameter(uint16_t ident, const void *pData, int index0, int index1);


//--------------------------------------------------------------------------------------------------
// WP_DATA_SetStoreDataCallback
//--------------------------------------------------------------------------------------------------
//! \brief	Set callback for storing data
//!
//! \param	pCallback		Pointer to callback function
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_DATA_SetStoreDataCallback(WP_DATA_tStoreData pCallback);


#if defined(WP_FX_SP_BLK70)
//--------------------------------------------------------------------------------------------------
// WP_DATA_GetGeoCodeCal
//--------------------------------------------------------------------------------------------------
//! \brief	Get Geo code data at calibration
//!
//! This function return either the high resolution code if enabled and valid or the low resolution
//! value. If neither of them are valid, the default value is returned.
//!
//! \param	pGeoCode	 out	Pointer to Geo code value to copy the data to (no copy if NULL)
//! \return				TRUE if data is valid, otherwise FALSE
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool WP_DATA_GetGeoCodeCal(WP_DATA_tXP0947 *pGeoCode);
#endif


#if defined(WP_FX_SP_BLK70)
//--------------------------------------------------------------------------------------------------
// WP_DATA_SetGeoCodeCal
//--------------------------------------------------------------------------------------------------
//! \brief	Set Geo code data at calibration
//!
//! \param	geoCode		Geo code
//! \return				Status (WP_OK, WP_RANGE_FAIL or WP_STORE_FAIL)
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tStatus WP_DATA_SetGeoCodeCal(WP_DATA_tXP0947 geoCode);
#endif


#if defined(WP_FX_SP_BLK70)
//--------------------------------------------------------------------------------------------------
// WP_DATA_GetGeoCodeUse
//--------------------------------------------------------------------------------------------------
//! \brief	Get Geo code data at use
//!
//! This function return either the high resolution code if enabled and valid or the low resolution
//! value. If neither of them are valid, the default value is returned.
//!
//! \param	pGeoCode	 out	Pointer to Geo code value to copy the data to (no copy if NULL)
//! \return				TRUE if data is valid, otherwise FALSE
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool WP_DATA_GetGeoCodeUse(WP_DATA_tXP0948 *pGeoCode);
#endif


#if defined(WP_FX_SP_BLK70)
//--------------------------------------------------------------------------------------------------
// WP_DATA_SetGeoCodeUse
//--------------------------------------------------------------------------------------------------
//! \brief	Set Geo code data at use
//!
//! \param	geoCode		Geo code
//! \return				Status (WP_OK, WP_RANGE_FAIL or WP_STORE_FAIL)
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tStatus WP_DATA_SetGeoCodeUse(WP_DATA_tXP0948 geoCode);
#endif


//--------------------------------------------------------------------------------------------------
// WP_DATA_GetCellNominal
//--------------------------------------------------------------------------------------------------
//! \brief	Get cell nominal
//!
//! Returns the cell nominal which corresponds to XP0308 or 1.0
//!
//! \return		cell nominal in Gram
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tFloat WP_DATA_GetCellNominal(void);


//--------------------------------------------------------------------------------------------------
// WP_DATA_GetSumOfBuiltinWeights
//--------------------------------------------------------------------------------------------------
//! \brief	Get sum of internal weights
//!
//! Return the sum of internal weights from Signalprocessing or XP0368 or 0.6 * cell nominal
//!
//! \return		sum of internal weights in Gram
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tFloat WP_DATA_GetSumOfBuiltinWeights(void);


//--------------------------------------------------------------------------------------------------
// WP_DATA_SetRuntimeValueRequestCallback
//--------------------------------------------------------------------------------------------------
//! \brief	Set callback function for info acquisition
//!
//! \attention This function is reserved for WP_Balance only
//!
//! \param  callback       Callback function
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_DATA_SetRuntimeValueRequestCallback(WP_DATA_tInfoCallback callback);


//--------------------------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_WP
#endif // _WP_Data__h
