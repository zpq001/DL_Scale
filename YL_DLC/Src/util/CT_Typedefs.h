//==================================================================================================
//                                         Digital Control
//==================================================================================================
//
//! \file       util/CT_Typedefs.h
//! \ingroup    util
//! \brief      General type definitions for digital control package.
//!
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author     Thomas Hug
//
// $Date: 2016/11/10 08:58:38MEZ $
// $Revision: 1.15 $
//
//==================================================================================================

#ifndef _CT_Typedefs__h
#define _CT_Typedefs__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Typedefs.h"

//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

#define CT_CONFIG_YES   1
#define CT_CONFIG_NO    0

//! Define the precision of float in control package
#define CT_FLOAT_PRECISION_HIGH   CT_CONFIG_NO

#if defined(CT_FLOAT_PRECISION_HIGH) && (CT_FLOAT_PRECISION_HIGH == CT_CONFIG_YES)
    //! Define epsilon used in digital control package
    #define CT_FLT_EPS  FLT64_EPSILON
#else
    //! Define epsilon used in digital control package
    #define CT_FLT_EPS  FLT32_EPSILON
#endif

//! Define maximum number of block parameters
#define CT_MAX_NUM_PARAM    30

//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//! Define datatype CT_tFloat used in digital control package
#if defined(CT_FLOAT_PRECISION_HIGH) && (CT_FLOAT_PRECISION_HIGH == CT_CONFIG_YES)
    typedef float64 RB_DECL_TYPE CT_tFloat;	//!< Double used for high resolution
#else
    typedef float32 RB_DECL_TYPE CT_tFloat;	//!< Float used for lower resolution
#endif

//!< Execution reason
typedef enum {
    CT_EXEC_DEFAULT = 0,                            //!< Default execution reason (e.g. startup)
    CT_EXEC_PARAMETER                               //!< Executed due to new parameters
} RB_DECL_TYPE CT_tBlkExecReason;

//!< Structure for general block parameters
typedef struct {
    unsigned short      numOfParam;                 //!< Number of parameters incl. compInfo
    float32             value[CT_MAX_NUM_PARAM];    //!< Parameter array
} RB_DECL_TYPE CT_tBlockParameter;

//!< Block function prototypes
typedef RB_DECL_TYPE int32_t (*CT_tInitBlock)(void * param, void * state, bool* enabled, float32 tSamp, CT_tBlkExecReason execReason);
typedef RB_DECL_TYPE int32_t (*CT_tGetBlockParameter)(const void* blkParam, CT_tBlockParameter* paramVector);
typedef RB_DECL_TYPE int32_t (*CT_tGetNumParameter)(void);
typedef RB_DECL_TYPE int32_t (*CT_tGetNumStates)(void);
typedef RB_DECL_TYPE int32_t (*CT_tSetBlockParameter)(const CT_tBlockParameter* paramVector, void* blkParam);
typedef RB_DECL_TYPE int32_t (*CT_tGetNumInputSignals)(void);
typedef RB_DECL_TYPE int32_t (*CT_tGetNumOutputSignals)(void);

//!< Structure for block functions
typedef struct {
    CT_tInitBlock             initialize;
    CT_tGetBlockParameter     getParam;
    CT_tGetNumParameter       getNumParam;
    CT_tSetBlockParameter     setParam;
    CT_tGetNumStates          getNumStates;
    CT_tGetNumInputSignals    getNumInputSignals;
    CT_tGetNumOutputSignals   getNumOutputSignals;
} RB_DECL_TYPE CT_tBlockFunction;

#ifdef __cplusplus
}
#endif

#endif // _CT_Typedefs__h
