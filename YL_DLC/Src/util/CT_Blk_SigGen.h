//==================================================================================================
//                                     Digital Control
//==================================================================================================
//
//! \file       util/CT_Blk_SigGen.h
//! \ingroup    util
//! \brief      Signal generator blocks.
//!
//! These blocks generate a signal (e.g. random signal or pulse signal) which can be used to be
//! modulated e.g. on the controller output or on the reference signal.
//! The main purpose to modulate a signal e.g. onto the controller output, is for system
//! identification.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author     Thomas Hug
//
// $Date: 2016/11/10 08:58:38MEZ $
// $Revision: 1.16 $
//
//==================================================================================================

#ifndef _CT_Blk_SigGen__h
#define _CT_Blk_SigGen__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "CT_Typedefs.h"

//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif

//==================================================================================================
//  G L O B A L   C O N S T A N T S
//==================================================================================================

extern const RB_DECL_CONST CT_tBlockFunction CT_BLK_SIGGEN_GeneralSigBlkFunctions;
extern const RB_DECL_CONST CT_tBlockFunction CT_BLK_SIGGEN_ConstSigBlkFunctions;

//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

// -------------------------------------------------------------------------------------------------
// Types for general signal generator block

typedef enum {
    CT_BLK_SIGGEN_GENERALSIG_MODEL_RANDOM = 0,
    CT_BLK_SIGGEN_GENERALSIG_MODEL_PULSE,
    CT_BLK_SIGGEN_GENERALSIG_MODEL_TRIANGLE,
    CT_BLK_SIGGEN_GENERALSIG_MODEL_SINE
} RB_DECL_TYPE CT_BLK_SIGGEN_GENERALSIG_tModel;

enum {
    CT_BLK_SIGGEN_GENERALSIG_PARAM_ID_MODELFLAG = 0,        // Model flag of signal to be generated
    CT_BLK_SIGGEN_GENERALSIG_PARAM_ID_AMPL = 1,             // Amplitude of signal
    CT_BLK_SIGGEN_GENERALSIG_PARAM_ID_FREQ = 2,             // Frequency of signal
    CT_BLK_SIGGEN_GENERALSIG_PARAM_ID_PARAM1 = 3,           // General parameter 1
    CT_BLK_SIGGEN_GENERALSIG_PARAM_ID_PARAM2 = 4,           // General parameter 2
    CT_BLK_SIGGEN_GENERALSIG_NUM_PARAM                      // Number of parameter
};

enum {
    CT_BLK_SIGGEN_GENERALSIG_IN_SIG_DUMMY = 0,              // To prevent empty array
    CT_BLK_SIGGEN_GENERALSIG_NUM_IN_SIG
};

enum {
    CT_BLK_SIGGEN_GENERALSIG_OUT_SIG_1 = 0,                 // Random output signal
    CT_BLK_SIGGEN_GENERALSIG_NUM_OUT_SIG
};

//!< State structure
typedef struct {
    CT_tFloat   y1;                                     // Previous pulse signal value
    int         counter;                                // Counter for holding the value
    int         holdSamples;                            // Hold time in samples
} RB_DECL_TYPE CT_BLK_SIGGEN_tGeneralSigState;

//!< Parameter structure
typedef struct {
    int         modelFlag;                              // Model flag of signal to be generated
    float32     ampl;                                   // Amplitude of signal
    float32     freq;                                   // Frequency of signal
    float32     param1;                                 // General parameter 1
    float32     param2;                                 // General parameter 2
} RB_DECL_TYPE CT_BLK_SIGGEN_tGeneralSigParam;

//!< Block context structure
typedef struct {
    CT_BLK_SIGGEN_tGeneralSigParam      param;
    CT_BLK_SIGGEN_tGeneralSigState      state;
    CT_tFloat                       inputSignal[CT_BLK_SIGGEN_GENERALSIG_NUM_IN_SIG];
    CT_tFloat                       outputSignal[CT_BLK_SIGGEN_GENERALSIG_NUM_OUT_SIG];
    bool                            enabled;
} RB_DECL_TYPE CT_BLK_SIGGEN_tGeneralSigGenBlock;

// -------------------------------------------------------------------------------------------------
// Types for constant signal generator block

enum {
    CT_BLK_SIGGEN_CONSTSIG_PARAM_ID_CONSTVALUE = 0,         // Value for block output
    CT_BLK_SIGGEN_CONSTSIG_NUM_PARAM                        // Number of parameter
};

enum {
    CT_BLK_SIGGEN_CONSTSIG_IN_SIG_DUMMY = 0,                // To prevent empty array
    CT_BLK_SIGGEN_CONSTSIG_NUM_IN_SIG
};

enum {
    CT_BLK_SIGGEN_CONSTSIG_OUT_SIG_1 = 0,                   // Constant output signal
    CT_BLK_SIGGEN_CONSTSIG_NUM_OUT_SIG
};

//!< State structure
typedef struct {
    int         dummy;
} RB_DECL_TYPE CT_BLK_SIGGEN_tConstSigState;

//!< Parameter structure
typedef struct {
    float32     constVal;                               // Value for block output
} RB_DECL_TYPE CT_BLK_SIGGEN_tConstSigParam;

//!< Block context structure
typedef struct {
    CT_BLK_SIGGEN_tConstSigParam        param;
    CT_BLK_SIGGEN_tConstSigState        state;
    CT_tFloat                       inputSignal[CT_BLK_SIGGEN_CONSTSIG_NUM_IN_SIG];
    CT_tFloat                       outputSignal[CT_BLK_SIGGEN_CONSTSIG_NUM_OUT_SIG];
    bool                            enabled;
} RB_DECL_TYPE CT_BLK_SIGGEN_tConstSigGenBlock;

//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

#define CT_BLK_SIGGEN_BLK_ITEMTABLE                                                                                                                                             \
    /*          Block type identifier,              Block function struct,                  Block parameter size,                   Block state size                        */  \
    CT_BLK_ITEM(CT_BLK_SIGGEN_GENERALSIG_BLKTYPE,   CT_BLK_SIGGEN_GeneralSigBlkFunctions,   sizeof(CT_BLK_SIGGEN_tGeneralSigParam), sizeof(CT_BLK_SIGGEN_tGeneralSigState)),    \
    CT_BLK_ITEM(CT_BLK_SIGGEN_CONSTSIG_BLKTYPE,     CT_BLK_SIGGEN_ConstSigBlkFunctions,     sizeof(CT_BLK_SIGGEN_tConstSigParam),   sizeof(CT_BLK_SIGGEN_tConstSigState)),      \

//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

// -------------------------------------------------------------------------------------------------
// Functions for general signal generator block

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_InitGeneralSigGen
//--------------------------------------------------------------------------------------------------
//! \brief  Initialize general signal generator block
//!
//!
//! \param  vParam          input/output    Pointer to parameter struct
//! \param  vState          output          Pointer to state struct
//! \param  enabled         output          Pointer to block enable flag
//! \param  tSamp           input           Sampling time
//! \param  execReason      input           Execution reason
//! \return On success, a zero value is returned.
//!         Otherwise, a non-zero value is returned which specifies the error.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_SIGGEN_InitGeneralSigGen(void* vParam, void* vState, bool* enabled, float32 tSamp, CT_tBlkExecReason execReason);

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_DoGeneralSigGen
//--------------------------------------------------------------------------------------------------
//! \brief  Generate a signal (defined by the model flag)
//!
//!
//! \param  block   input/output    Block context
//! \return Block output signal
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC CT_tFloat CT_BLK_SIGGEN_DoGeneralSigGen(CT_BLK_SIGGEN_tGeneralSigGenBlock* block);

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_SetGeneralSigGenParameter
//--------------------------------------------------------------------------------------------------
//! \brief  Set block parameter
//!
//!
//! \param  paramVector input   Pointer to general parameter array
//! \param  blkParam    output  Pointer to stored block parameters
//!
//! \return On success, a zero value is returned.
//!         Otherwise, a non-zero value is returned which specifies the error.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_SIGGEN_SetGeneralSigGenParameter(const CT_tBlockParameter* paramVector, void* blkParam);

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_GetGeneralSigGenParameter
//--------------------------------------------------------------------------------------------------
//! \brief  Get block parameter
//!
//!
//! \param  blkParam    input   Pointer to stored block parameters
//! \param  paramVector output  Pointer to general parameter array
//!
//! \return On success, a zero value is returned.
//!         Otherwise, a non-zero value is returned which specifies the error.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_SIGGEN_GetGeneralSigGenParameter(const void* blkParam, CT_tBlockParameter* paramVector);

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_GetNumGeneralSigGenParameter
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block parameter
//!
//!
//!
//! \return Number of block parameter
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_SIGGEN_GetNumGeneralSigGenParameter(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_GetNumGeneralSigGenStates
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block states
//!
//!
//!
//! \return Number of block states
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_SIGGEN_GetNumGeneralSigGenStates(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_GetNumGeneralSigGenInputSignals
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block input signals
//!
//!
//!
//! \return Number of block input signals
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_SIGGEN_GetNumGeneralSigGenInputSignals(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_GetNumGeneralSigGenOutputSignals
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block output signals
//!
//!
//!
//! \return Number of block output signals
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_SIGGEN_GetNumGeneralSigGenOutputSignals(void);

// -------------------------------------------------------------------------------------------------
// Functions for constant signal generator block

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_InitConstSigGen
//--------------------------------------------------------------------------------------------------
//! \brief  Initialize constant signal generator block
//!
//!
//! \param  vParam          input/output    Pointer to parameter struct
//! \param  vState          output          Pointer to state struct
//! \param  enabled         output          Pointer to block enable flag
//! \param  tSamp           input           Sampling time
//! \param  execReason      input           Execution reason
//! \return On success, a zero value is returned.
//!         Otherwise, a non-zero value is returned which specifies the error.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_SIGGEN_InitConstSigGen(void* vParam, void* vState, bool* enabled, float32 tSamp, CT_tBlkExecReason execReason);

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_DoConstSigGen
//--------------------------------------------------------------------------------------------------
//! \brief  Generate a constant output signal value
//!
//!
//! \param  block   input/output    Block context
//! \return Block output signal
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC CT_tFloat CT_BLK_SIGGEN_DoConstSigGen(CT_BLK_SIGGEN_tConstSigGenBlock* block);

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_SetConstSigGenParameter
//--------------------------------------------------------------------------------------------------
//! \brief  Set block parameter
//!
//!
//! \param  paramVector input   Pointer to general parameter array
//! \param  blkParam    output  Pointer to stored block parameters
//!
//! \return On success, a zero value is returned.
//!         Otherwise, a non-zero value is returned which specifies the error.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_SIGGEN_SetConstSigGenParameter(const CT_tBlockParameter* paramVector, void* blkParam);

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_GetConstSigGenParameter
//--------------------------------------------------------------------------------------------------
//! \brief  Get block parameter
//!
//!
//! \param  blkParam    input   Pointer to stored block parameters
//! \param  paramVector output  Pointer to general parameter array
//!
//! \return On success, a zero value is returned.
//!         Otherwise, a non-zero value is returned which specifies the error.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_SIGGEN_GetConstSigGenParameter(const void* blkParam, CT_tBlockParameter* paramVector);

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_GetNumConstSigGenParameter
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block parameter
//!
//!
//!
//! \return Number of block parameter
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_SIGGEN_GetNumConstSigGenParameter(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_GetNumConstSigGenStates
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block states
//!
//!
//!
//! \return Number of block states
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_SIGGEN_GetNumConstSigGenStates(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_GetNumConstSigGenInputSignals
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block input signals
//!
//!
//!
//! \return Number of block input signals
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_SIGGEN_GetNumConstSigGenInputSignals(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_SIGGEN_GetNumConstSigGenOutputSignals
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block output signals
//!
//!
//!
//! \return Number of block output signals
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_SIGGEN_GetNumConstSigGenOutputSignals(void);

#ifdef __cplusplus
}
#endif

#endif // _CT_Blk_SigGen__h
