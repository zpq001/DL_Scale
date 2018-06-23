//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/WP_Typedefs.h
//! \ingroup	util
//! \brief		Common type definitions for weighing package.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Martin Heusser
//
// $Date: 2017/02/16 13:50:47MEZ $
// $Revision: 1.49 $
//
//==================================================================================================

#ifndef _WP_Typedefs__h
#define _WP_Typedefs__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"
#include "RB_Typedefs.h"
#include "WP_Version.h"

#if defined(__cplusplus)
extern "C" {
#endif

//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

//! Float values may be float32 or float64, depending on configuration
#if defined(RB_CONFIG_WP_HIGH_RESOLUTION) && (RB_CONFIG_WP_HIGH_RESOLUTION == RB_CONFIG_YES)
	typedef float64 RB_DECL_TYPE WP_tFloat;	//!< Double used for high resolution
	#define WP_FLT_MAX		DBL_MAX			//!< 1.79769313486231571e+308
	#define WP_FLT_BIG		1.0e+32			//!< 1.0e+32, a big number for defaults
	#define WP_FLT_SMALL	1.0e-32			//!< 1.0e-32, a small number for comparison
	#define WP_FLT_MIN		DBL_MIN			//!< 2.22507385850720138e-308
	#define WP_FLT_EPSILON	DBL_EPSILON		//!< 2.2204460492503131e-16
	#define WP_FLT_ZERO		0.0				//!< 0.0
	#define WP_FLT_ONE		1.0				//!< 1.0
#else
	typedef float32 RB_DECL_TYPE WP_tFloat;	//!< Float used for non high resolution
	#define WP_FLT_MAX		FLT_MAX			//!< 3.40282347e+38
	#define WP_FLT_BIG		1.0e+32F		//!< 1.0e+32, a big number for defaults
	#define WP_FLT_SMALL	1.0e-32F		//!< 1.0e-32, a small number for comparison
	#define WP_FLT_MIN		FLT_MIN			//!< 1.17549435e-38
	#define WP_FLT_EPSILON	FLT_EPSILON		//!< 1.19209290e-7
	#define WP_FLT_ZERO		0.0F			//!< 0.0
	#define WP_FLT_ONE		1.0F			//!< 1.0
#endif

//==================================================================================================
//  D E F A U L T S
//==================================================================================================

#ifndef RB_CONFIG_WP_RANGES_MAX
	#define RB_CONFIG_WP_RANGES_MAX					1
#endif

#ifndef RB_CONFIG_WP_PARAM_USE_CONFIG_PARAM
	#define RB_CONFIG_WP_PARAM_USE_CONFIG_PARAM		RB_CONFIG_YES
#endif

#ifndef RB_CONFIG_WP_PARAM_USE_CONFIG_PARAM
	#define RB_CONFIG_WP_PARAM_USE_CONFIG_PARAM		RB_CONFIG_YES
#endif

#ifndef RB_CONFIG_WP_PARAM_USE_CONFIG_SUBSET
	#define RB_CONFIG_WP_PARAM_USE_CONFIG_SUBSET	RB_CONFIG_NO
#endif

#ifndef RB_CONFIG_WP_PARAM_USE_RUNTIME_PARAM
	#define RB_CONFIG_WP_PARAM_USE_RUNTIME_PARAM	RB_CONFIG_YES
#endif

#ifndef RB_CONFIG_WP_MAX_SCALE_CONFIGURATIONS
	#define RB_CONFIG_WP_MAX_SCALE_CONFIGURATIONS		10
#endif

//==================================================================================================
//  G L O B A L   D E F I N I T I O N S   F O R   W P - F E A T U R E S
//==================================================================================================

//! Definition of weighing package features based on configuration switches in RB_Config.h
//! Basic features of WP utilities
#if defined(RB_CONFIG_USE_WP) && (RB_CONFIG_USE_WP == RB_CONFIG_YES)
	//----------------------------------------------------------------------------------------------
	//! Common features
	//----------------------------------------------------------------------------------------------
	#define WP_FX_BASIC
	#if defined(RB_PARAM_M21)
		#define WP_FX_CMD_M21
	#endif
	#if defined(RB_PARAM_M65)
		#define WP_FX_CMD_M65
	#endif

	#if defined(SP_CONFIG_USE_BLOCK_70) && (SP_CONFIG_USE_BLOCK_70 == SP_CONFIG_YES)
		#define WP_FX_SP_BLK70
		#if defined(RB_CONFIG_WP_USE_HIGHRES_GEOCODE) && (RB_CONFIG_WP_USE_HIGHRES_GEOCODE == SP_CONFIG_YES)
			#define WP_FX_SP_BLK70_HIGHRES
		#endif
	#endif

	#if defined(RB_CONFIG_USE_SIGNAL_PROCESSING) && (RB_CONFIG_USE_SIGNAL_PROCESSING == RB_CONFIG_YES)
		#define WP_FX_DSP
		#if defined(SP_CONFIG_USE_SIGNALPROCESSING_INTERFACE_V2)
			#define WP_FX_DSP_V2
		#else
			#define WP_FX_DSP_V1
		#endif
		#if !defined(RB_PARAM_M01)
			#define RB_PARAM_M01	RB_PARAM_ID_RB(1001) // Parameter always needed
		#endif
		#define WP_FX_CMD_M01
		#if !defined(RB_PARAM_M02)
			#define RB_PARAM_M02	RB_PARAM_ID_RB(1002) // Parameter always needed
		#endif
		#define WP_FX_CMD_M02
		#if defined(RB_PARAM_M29)
			#define WP_FX_CMD_M29
		#endif
	#endif

	//! Features of WP_State (no dedicated RB_CONFIG_USE_ switch)
	#if defined(RB_CONFIG_WP_SUPPORT_LFT_SEAL_BREAK) && (RB_CONFIG_WP_SUPPORT_LFT_SEAL_BREAK == RB_CONFIG_YES)
		#define WP_FX_LFT_SEAL_BREAK
	#endif

	//! Features of WP_Unit (no dedicated RB_CONFIG_USE_ switch)
	#if defined(RB_CONFIG_WP_UNIT_MAX_CUSTOM_UNITS) && (RB_CONFIG_WP_UNIT_MAX_CUSTOM_UNITS > 0)
		#define WP_FX_CUSTOM_UNITS
		#if defined(RB_PARAM_M22)
			#define WP_FX_CMD_M22
		#endif
	#endif

	#if defined(RB_CONFIG_WP_SERVICE_RESOLUTION) && (RB_CONFIG_WP_SERVICE_RESOLUTION == RB_CONFIG_YES)
		#define WP_FX_SERVICE_RESOLUTION
	#endif

	#if defined(RB_CONFIG_WP_SHOW_DOT_WITHOUT_DECIMALS) && (RB_CONFIG_WP_SHOW_DOT_WITHOUT_DECIMALS == RB_CONFIG_YES)
		#define WP_FX_DOT_WITHOUT_DECIMALS
	#endif

	#if defined(RB_CONFIG_WP_SHOW_REDUCED_DIGITS) || (RB_CONFIG_WP_SHOW_REDUCED_DIGITS == RB_CONFIG_YES)
		#define WP_FX_SHOW_REDUCED_DIGITS
	#endif

	//----------------------------------------------------------------------------------------------
	//! Features of WP_Sensor
	//----------------------------------------------------------------------------------------------
	#if defined(RB_CONFIG_WP_USE_SENSOR) && (RB_CONFIG_WP_USE_SENSOR == RB_CONFIG_YES)
		#define WP_FX_SENSOR
		#if defined(RB_CONFIG_USE_MELSI) && (RB_CONFIG_USE_MELSI == RB_CONFIG_YES) && \
			defined(RB_CONFIG_MELSI_POWER_COMP) && (RB_CONFIG_MELSI_POWER_COMP == RB_CONFIG_YES)
			#define WP_FX_PWRCOMP
		#endif
	#endif // RB_CONFIG_WP_USE_SENSOR

	//----------------------------------------------------------------------------------------------
	//! Features of WP_PostProc
	//----------------------------------------------------------------------------------------------
	#if defined(RB_CONFIG_WP_USE_POSTPROCESSING) && (RB_CONFIG_WP_USE_POSTPROCESSING == RB_CONFIG_YES)
		#define WP_FX_POSTPROC
		#if defined(RB_CONFIG_WP_2ND_RANGE_DETECTOR) && (RB_CONFIG_WP_2ND_RANGE_DETECTOR == RB_CONFIG_YES)
			#define WP_FX_2ND_RANGE_DETECTOR
		#endif
		#if defined(RB_CONFIG_WP_RANGES_MAX) && (RB_CONFIG_WP_RANGES_MAX > 1)
			#define WP_FX_RANGES
		#endif
		#if defined(RB_CONFIG_WP_S_CURVE) && (RB_CONFIG_WP_S_CURVE == RB_CONFIG_YES)
			#define WP_FX_S_CURVE
		#endif
		#if defined(RB_CONFIG_WP_HYST_CORRECTION) && (RB_CONFIG_WP_HYST_CORRECTION == RB_CONFIG_YES)
			#define WP_FX_HYST_CORRECTION
		#endif
		#if defined(SP_CONFIG_USE_BLOCK_97) && (SP_CONFIG_USE_BLOCK_97 == SP_CONFIG_YES)
			#define WP_FX_SP_BLK97
		#endif
		#if defined(RB_CONFIG_WP_ADDITIONAL_STABILITY_CHECK) && (RB_CONFIG_WP_ADDITIONAL_STABILITY_CHECK == RB_CONFIG_YES)
			#define WP_FX_ADDSTAB
		#endif
		#if defined(RB_CONFIG_WP_MIN_WEIGH) && (RB_CONFIG_WP_MIN_WEIGH == RB_CONFIG_YES)
			#define WP_FX_MIN_WEIGH
			#if defined(RB_CONFIG_WP_MIN_WEIGH_SYMMETRICAL) && (RB_CONFIG_WP_MIN_WEIGH_SYMMETRICAL == RB_CONFIG_YES)
				#define WP_FX_MIN_WEIGH_SYMMETRICAL
			#endif
			#if defined(RB_CONFIG_WP_MIN_WEIGH_USE_LEGACY_IMPLEMENTATION) && (RB_CONFIG_WP_MIN_WEIGH_USE_LEGACY_IMPLEMENTATION == RB_CONFIG_YES)
				#define WP_FX_MIN_WEIGH_USE_LEGACY_IMPLEMENTATION
			#endif
			#if defined(RB_PARAM_M34)
				#define WP_FX_CMD_M34
			#endif
		#endif
		#if defined(RB_CONFIG_WP_MIN_WEIGHT_OIML_CLASS3) && (RB_CONFIG_WP_MIN_WEIGHT_OIML_CLASS3 == RB_CONFIG_YES)
			#define WP_FX_MW_OIML
		#endif

		//! ! Features of WP_PostProc which needs run time parameters
		#if defined(RB_CONFIG_WP_PARAM_USE_RUNTIME_PARAM) && (RB_CONFIG_WP_PARAM_USE_RUNTIME_PARAM == RB_CONFIG_YES)
			#if defined(RB_CONFIG_WP_USER_STAB_SUPPORT) && (RB_CONFIG_WP_USER_STAB_SUPPORT == RB_CONFIG_YES)
				#define WP_FX_USTB
				#if defined(RB_PARAM_USTB)
					#define WP_FX_CMD_USTB
				#endif
			#endif
			#if defined(RB_CONFIG_WP_FCUTOFF_SUPPORT) && (RB_CONFIG_WP_FCUTOFF_SUPPORT == RB_CONFIG_YES)
				#define WP_FX_FCUT
				#if defined(RB_PARAM_FCUT)
					#define WP_FX_CMD_FCUT
				#endif
			#endif
			#if defined(RB_CONFIG_WP_PERMANENT_ZERO) && (RB_CONFIG_WP_PERMANENT_ZERO == RB_CONFIG_YES)
				#define WP_FX_PERM_ZERO
				#if defined(RB_PARAM_M35)
					#define WP_FX_CMD_M35
				#endif
			#endif

			#if defined(RB_CONFIG_WP_PERMANENT_TARE) && (RB_CONFIG_WP_PERMANENT_TARE == RB_CONFIG_YES)
				#define WP_FX_PERM_TARE
				#if defined(RB_PARAM_M49)
					#define WP_FX_CMD_M49
				#endif
			#endif
		#endif // RB_CONFIG_WP_PARAM_USE_RUNTIME_PARAM
		#if defined(RB_PARAM_M03)
			#define WP_FX_CMD_M03
		#endif
		#if defined(RB_PARAM_M23)
			#define WP_FX_CMD_M23
		#endif
		#if defined(RB_PARAM_M82)
			#define WP_FX_CMD_M82
		#endif
		#if defined(RB_PARAM_M110)
			#define WP_FX_CMD_M110
		#endif
		#if defined(RB_PARAM_C0)
			#define WP_FX_CMD_C0
		#endif
		#if defined(RB_PARAM_C5)
			#define WP_FX_CMD_C5
		#endif

	#endif // RB_CONFIG_WP_USE_POSTPROCESSING

	//----------------------------------------------------------------------------------------------
	//! Features of WP_Param
	//----------------------------------------------------------------------------------------------
	#if defined(RB_CONFIG_WP_USE_PARAMETER) && (RB_CONFIG_WP_USE_PARAMETER == RB_CONFIG_YES)
		#define WP_FX_PARAMETER

		//! Configuration parameters
		#if defined(RB_CONFIG_WP_PARAM_USE_CONFIG_PARAM) && (RB_CONFIG_WP_PARAM_USE_CONFIG_PARAM == RB_CONFIG_YES)
			#if defined(RB_CONFIG_WP_PARAM_USE_CONFIG_SUBSET) && (RB_CONFIG_WP_PARAM_USE_CONFIG_SUBSET == RB_CONFIG_YES)
				#define WP_FX_CNFPAR_RED
			#else
				#define WP_FX_CNFPAR_STD
			#endif
			#define WP_FX_CNFPAR_COM
		#endif

		//! Run time parameters
		#if defined(RB_CONFIG_WP_PARAM_USE_RUNTIME_PARAM) && (RB_CONFIG_WP_PARAM_USE_RUNTIME_PARAM == RB_CONFIG_YES)
			#define WP_FX_RTPAR_COM
		#endif

		//! Any parameter used
		#if defined(WP_FX_CNFPAR_COM) || defined(WP_FX_RTPAR_COM)
			#define WP_FX_PARAM_ANY
		#endif

	#endif // RB_CONFIG_WP_USE_PARAMETER

	//----------------------------------------------------------------------------------------------
	//! Features of WP_Cmd
	//----------------------------------------------------------------------------------------------
	#if defined(RB_CONFIG_USE_COMMAND) && (RB_CONFIG_USE_COMMAND == RB_CONFIG_YES) && defined(WP_FX_PARAMETER)
		#define WP_FX_COMMAND
	#endif // RB_CONFIG_USE_COMMAND

	//----------------------------------------------------------------------------------------------
	//! Features of WP_Setup
	//----------------------------------------------------------------------------------------------
	#if defined(RB_CONFIG_WP_USE_SETUP) && (RB_CONFIG_WP_USE_SETUP == RB_CONFIG_YES)
		#define WP_FX_SETUP
	#endif // RB_CONFIG_WP_USE_SETUP

	//----------------------------------------------------------------------------------------------
	//! Features of WP_Display
	//----------------------------------------------------------------------------------------------
	#if defined(RB_CONFIG_WP_USE_DISPLAY) && (RB_CONFIG_WP_USE_DISPLAY == RB_CONFIG_YES)
		#define WP_FX_DISPLAY
	#endif // RB_CONFIG_WP_USE_DISPLAY

	//----------------------------------------------------------------------------------------------
	//! Features of WP_Balance, needs sensor and post processing
	//----------------------------------------------------------------------------------------------
	#if defined(RB_CONFIG_WP_USE_BALANCE) && (RB_CONFIG_WP_USE_BALANCE == RB_CONFIG_YES) && defined(WP_FX_SENSOR) && defined(WP_FX_POSTPROC)
		#define WP_FX_BALANCE
		#if defined(RB_CONFIG_USE_CALWEIGHT) && (RB_CONFIG_USE_CALWEIGHT == RB_CONFIG_YES)
			#define WP_FX_CALWGT
			#if defined(RB_CONFIG_WP_2x3PT_LIN) && (RB_CONFIG_WP_2x3PT_LIN == RB_CONFIG_YES)
				#define WP_FX_2x3PT_LIN
			#endif
		#endif
		#if defined(RB_CONFIG_WP_3PT_LIN) && (RB_CONFIG_WP_3PT_LIN == RB_CONFIG_YES)
			#define WP_FX_3PT_LIN
		#endif
		#if defined(RB_CONFIG_WP_5PT_LIN) && (RB_CONFIG_WP_5PT_LIN == RB_CONFIG_YES)
			#define WP_FX_5PT_LIN
		#endif
		#if defined(RB_CONFIG_WP_LOADHYST_ADJUSTMENT) && (RB_CONFIG_WP_LOADHYST_ADJUSTMENT == RB_CONFIG_YES)
			#define WP_FX_LOADHYST_ADJUSTMENT
		#endif
		#if defined(RB_PARAM_M19)
			#define WP_FX_CMD_M19
		#endif
		#if defined(RB_PARAM_M20)
			#define WP_FX_CMD_M20
		#endif
		#if defined(RB_PARAM_M67)
			#define WP_FX_CMD_M67
		#endif
	#endif // RB_CONFIG_WP_USE_BALANCE

	//----------------------------------------------------------------------------------------------
	//! Features of WP_AutoTare, needs balance
	//----------------------------------------------------------------------------------------------
	#if defined(RB_CONFIG_WP_USE_AUTOTARE) && (RB_CONFIG_WP_USE_AUTOTARE == RB_CONFIG_YES) && defined(WP_FX_BALANCE)
		#define WP_FX_AUTOTARE
		#if defined(RB_PARAM_M60)
			#define WP_FX_CMD_M60
		#endif
		#if defined(RB_PARAM_M61)
			#define WP_FX_CMD_M61
		#endif
		#if defined(RB_PARAM_M62)
			#define WP_FX_CMD_M62
		#endif
		#if defined(RB_PARAM_M63)
			#define WP_FX_CMD_M63
		#endif
	#endif // RB_CONFIG_WP_USE_AUTOTARE

	//----------------------------------------------------------------------------------------------
	//! Features of WP_FACT, needs balance
	//----------------------------------------------------------------------------------------------
	#if defined(RB_CONFIG_WP_USE_FACT) && (RB_CONFIG_WP_USE_FACT == RB_CONFIG_YES) && \
		defined(WP_FX_BALANCE) && defined(WP_FX_CALWGT)
		#define WP_FX_FACT
		#if defined(RB_CONFIG_WP_FACT_NUMBER_OF_TRIGGER_DAYTIMES) && (RB_CONFIG_WP_FACT_NUMBER_OF_TRIGGER_DAYTIMES > 0) && \
			defined(RB_CONFIG_USE_RTC) && (RB_CONFIG_USE_RTC == RB_CONFIG_YES)
			#define WP_FX_PROFACT
			#if defined(RB_PARAM_M17)
				#define WP_FX_CMD_M17
			#endif
		#endif
		#if defined(RB_PARAM_M18)
			#define WP_FX_CMD_M18
		#endif
		#if defined(RB_PARAM_M93)
			#define WP_FX_CMD_M93
		#endif
	#endif // RB_CONFIG_WP_USE_FACT

	//----------------------------------------------------------------------------------------------
#endif // RB_CONFIG_USE_WP

//==================================================================================================
//  G L O B A L   D E F I N I T I O N S   F O R   W P - F E A T U R E - E X P A N S I O N   M A C R O S
//==================================================================================================
//! Definitions of WP_FX_xyz_ macros. These macros expand either to nothing or to their argument
//! depending on the related weighing package feature.
#if defined(RB_CONFIG_USE_WP) && (RB_CONFIG_USE_WP == RB_CONFIG_YES)
	#if defined(WP_FX_BASIC)
		#define WP_FX_BASIC_(a)	a
	#else
		#define WP_FX_BASIC_(a)
	#endif

	#if defined(WP_FX_DSP)
		#define WP_FX_DSP_(a)	a
	#else
		#define WP_FX_DSP_(a)
	#endif

	#if defined(WP_FX_DSP_V1)
		#define WP_FX_DSP_V1_(a)	a
	#else
		#define WP_FX_DSP_V1_(a)
	#endif

	#if defined(WP_FX_DSP_V2)
		#define WP_FX_DSP_V2_(a)	a
	#else
		#define WP_FX_DSP_V2_(a)
	#endif

	#if defined(WP_FX_SP_BLK70)
		#define WP_FX_SP_BLK70_(a)	a
	#else
		#define WP_FX_SP_BLK70_(a)
	#endif

	#if defined(WP_FX_CUSTOM_UNITS)
		#define WP_FX_CUST_UNIT_(a)	a
	#else
		#define WP_FX_CUST_UNIT_(a)
	#endif

	#if defined(WP_FX_SENSOR)
		#define WP_FX_SENSOR_(a)	a
	#else
		#define WP_FX_SENSOR_(a)
	#endif

	#if defined(WP_FX_PWRCOMP)
		#define WP_FX_PWRCOMP_(a)	a
	#else
		#define WP_FX_PWRCOMP_(a)
	#endif

	#if defined(WP_FX_POSTPROC)
		#define WP_FX_POSTPROC_(a)	a
	#else
		#define WP_FX_POSTPROC_(a)
	#endif

	#if defined(WP_FX_2ND_RANGE_DETECTOR)
		#define WP_FX_2NDRNGDET_(a)	a
	#else
		#define WP_FX_2NDRNGDET_(a)
	#endif

	#if defined(WP_FX_RANGES)
		#define WP_FX_RANGES_(a)	a
	#else
		#define WP_FX_RANGES_(a)
	#endif

	#if defined(WP_FX_S_CURVE)
		#define WP_FX_S_CURVE_(a)	a
	#else
		#define WP_FX_S_CURVE_(a)
	#endif

	#if defined(WP_FX_HYST_CORRECTION)
		#define WP_FX_HYSTCORR_(a)	a
	#else
		#define WP_FX_HYSTCORR_(a)
	#endif

	#if defined(WP_FX_SP_BLK97)
		#define WP_FX_SP_BLK97_(a)	a
	#else
		#define WP_FX_SP_BLK97_(a)
	#endif

	#if defined(WP_FX_ADDSTAB)
		#define WP_FX_ADDSTAB_(a)	a
	#else
		#define WP_FX_ADDSTAB_(a)
	#endif

	#if defined(WP_FX_MIN_WEIGH)
		#define WP_FX_MIN_WEIGH_(a)	a
	#else
		#define WP_FX_MIN_WEIGH_(a)
	#endif

	#if defined(WP_FX_MW_OIML)
		#define WP_FX_MW_OIML_(a)	a
	#else
		#define WP_FX_MW_OIML_(a)
	#endif

	#if defined(WP_FX_USTB)
		#define WP_FX_USTB_(a)	a
	#else
		#define WP_FX_USTB_(a)
	#endif

	#if defined(WP_FX_FCUT)
		#define WP_FX_FCUT_(a)	a
	#else
		#define WP_FX_FCUT_(a)
	#endif

	#if defined(WP_FX_PERM_ZERO)
		#define WP_FX_PERM_ZERO_(a)	a
	#else
		#define WP_FX_PERM_ZERO_(a)
	#endif

	#if defined(WP_FX_PERM_TARE)
		#define WP_FX_PERM_TARE_(a)	a
	#else
		#define WP_FX_PERM_TARE_(a)
	#endif

	#if defined(WP_FX_PARAMETER)
		#define WP_FX_PARAMETER_(a)	a
	#else
		#define WP_FX_PARAMETER_(a)
	#endif

	#if defined(WP_FX_CNFPAR_RED)
		#define WP_FX_CNFPAR_RED_(a)	a
	#else
		#define WP_FX_CNFPAR_RED_(a)
	#endif

	#if defined(WP_FX_CNFPAR_STD)
		#define WP_FX_CNFPAR_STD_(a)	a
	#else
		#define WP_FX_CNFPAR_STD_(a)
	#endif

	#if defined(WP_FX_CNFPAR_COM)
		#define WP_FX_CNFPAR_COM_(a)	a
	#else
		#define WP_FX_CNFPAR_COM_(a)
	#endif

	#if defined(WP_FX_RTPAR_COM)
		#define WP_FX_RTPAR_COM_(a)	a
	#else
		#define WP_FX_RTPAR_COM_(a)
	#endif

	#if defined(WP_FX_PARAM_ANY)
		#define WP_FX_PARAM_ANY_(a)	a
	#else
		#define WP_FX_PARAM_ANY_(a)
	#endif

	#if defined(WP_FX_COMMAND)
		#define WP_FX_COMMAND_(a)	a
	#else
		#define WP_FX_COMMAND_(a)
	#endif

	#if defined(WP_FX_SETUP)
		#define WP_FX_SETUP_(a)	a
	#else
		#define WP_FX_SETUP_(a)
	#endif

	#if defined(WP_FX_DISPLAY)
		#define WP_FX_DISPLAY_(a)	a
	#else
		#define WP_FX_DISPLAY_(a)
	#endif

	#if defined(WP_FX_BALANCE)
		#define WP_FX_BALANCE_(a)	a
	#else
		#define WP_FX_BALANCE_(a)
	#endif

	#if defined(WP_FX_CALWGT)
		#define WP_FX_CALWGT_(a)	a
	#else
		#define WP_FX_CALWGT_(a)
	#endif

	#if defined(WP_FX_AUTOTARE)
		#define WP_FX_AUTOTARE_(a)	a
	#else
		#define WP_FX_AUTOTARE_(a)
	#endif

	#if defined(WP_FX_FACT)
		#define WP_FX_FACT_(a)	a
	#else
		#define WP_FX_FACT_(a)
	#endif

	#if defined(WP_FX_PROFACT)
		#define WP_FX_PROFACT_(a)	a
	#else
		#define WP_FX_PROFACT_(a)
	#endif

	//! Additional definitions of WP_FX_cmd_ macros. These macros expand either to nothing or to
	//! their argument depending on the related weighing package feature and configured parameter.
	#if defined(WP_FX_CMD_M01)
		#define WP_FX_CMD_M01_(a)	a
	#else
		#define WP_FX_CMD_M01_(a)
	#endif

	#if defined(WP_FX_CMD_M02)
		#define WP_FX_CMD_M02_(a)	a
	#else
		#define WP_FX_CMD_M02_(a)
	#endif

	#if defined(WP_FX_CMD_M03)
		#define WP_FX_CMD_M03_(a)	a
	#else
		#define WP_FX_CMD_M03_(a)
	#endif

	#if defined(WP_FX_CMD_M17)
		#define WP_FX_CMD_M17_(a)	a
	#else
		#define WP_FX_CMD_M17_(a)
	#endif

	#if defined(WP_FX_CMD_M18)
		#define WP_FX_CMD_M18_(a)	a
	#else
		#define WP_FX_CMD_M18_(a)
	#endif

	#if defined(WP_FX_CMD_M19)
		#define WP_FX_CMD_M19_(a)	a
	#else
		#define WP_FX_CMD_M19_(a)
	#endif

	#if defined(WP_FX_CMD_M20)
		#define WP_FX_CMD_M20_(a)	a
	#else
		#define WP_FX_CMD_M20_(a)
	#endif

	#if defined(WP_FX_CMD_M21)
		#define WP_FX_CMD_M21_(a)	a
	#else
		#define WP_FX_CMD_M21_(a)
	#endif

	#if defined(WP_FX_CMD_M22)
		#define WP_FX_CMD_M22_(a)	a
	#else
		#define WP_FX_CMD_M22_(a)
	#endif

	#if defined(WP_FX_CMD_M23)
		#define WP_FX_CMD_M23_(a)	a
	#else
		#define WP_FX_CMD_M23_(a)
	#endif

	#if defined(WP_FX_CMD_M29)
		#define WP_FX_CMD_M29_(a)	a
	#else
		#define WP_FX_CMD_M29_(a)
	#endif

	#if defined(WP_FX_CMD_M34)
		#define WP_FX_CMD_M34_(a)	a
	#else
		#define WP_FX_CMD_M34_(a)
	#endif

	#if defined(WP_FX_CMD_M35)
		#define WP_FX_CMD_M35_(a)	a
	#else
		#define WP_FX_CMD_M35_(a)
	#endif

	#if defined(WP_FX_CMD_M49)
		#define WP_FX_CMD_M49_(a)	a
	#else
		#define WP_FX_CMD_M49_(a)
	#endif

	#if defined(WP_FX_CMD_M60)
		#define WP_FX_CMD_M60_(a)	a
	#else
		#define WP_FX_CMD_M60_(a)
	#endif

	#if defined(WP_FX_CMD_M61)
		#define WP_FX_CMD_M61_(a)	a
	#else
		#define WP_FX_CMD_M61_(a)
	#endif

	#if defined(WP_FX_CMD_M62)
		#define WP_FX_CMD_M62_(a)	a
	#else
		#define WP_FX_CMD_M62_(a)
	#endif

	#if defined(WP_FX_CMD_M63)
		#define WP_FX_CMD_M63_(a)	a
	#else
		#define WP_FX_CMD_M63_(a)
	#endif

	#if defined(WP_FX_CMD_M65)
		#define WP_FX_CMD_M65_(a)	a
	#else
		#define WP_FX_CMD_M65_(a)
	#endif

	#if defined(WP_FX_CMD_M67)
		#define WP_FX_CMD_M67_(a)	a
	#else
		#define WP_FX_CMD_M67_(a)
	#endif

	#if defined(WP_FX_CMD_M82)
		#define WP_FX_CMD_M82_(a)	a
	#else
		#define WP_FX_CMD_M82_(a)
	#endif

	#if defined(WP_FX_CMD_M93)
		#define WP_FX_CMD_M93_(a)	a
	#else
		#define WP_FX_CMD_M93_(a)
	#endif

	#if defined(WP_FX_CMD_M110)
		#define WP_FX_CMD_M110_(a)	a
	#else
		#define WP_FX_CMD_M110_(a)
	#endif

	#if defined(WP_FX_CMD_C0)
		#define WP_FX_CMD_C0_(a)	a
	#else
		#define WP_FX_CMD_C0_(a)
	#endif

	#if defined(WP_FX_CMD_C5)
		#define WP_FX_CMD_C5_(a)	a
	#else
		#define WP_FX_CMD_C5_(a)
	#endif

	#if defined(WP_FX_CMD_USTB)
		#define WP_FX_CMD_USTB_(a)	a
	#else
		#define WP_FX_CMD_USTB_(a)
	#endif

	#if defined(WP_FX_CMD_FCUT)
		#define WP_FX_CMD_FCUT_(a)	a
	#else
		#define WP_FX_CMD_FCUT_(a)
	#endif

#endif // RB_CONFIG_USE_WP
//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================


//! Status
typedef enum {
	WP_OK         = 0,		//!< Operation succeded
	WP_RANGE_FAIL = 1,		//!< Operation failed, parameter out of range
	WP_STORE_FAIL = 2,		//!< Operation failed, parameter storing failed
	WP_READ_FAIL  = 3		//!< Operation failed, parameter reading failed
} RB_DECL_TYPE WP_tStatus;


//! Range status
typedef uint16_t RB_DECL_TYPE WP_tRangeIndex;

//--------------------------------------------------------------------------------------------------
#if defined(__cplusplus)
}
#endif

#endif // _WP_Typedefs__h
