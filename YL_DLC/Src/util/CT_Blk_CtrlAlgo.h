//==================================================================================================
//                                         Digital Control
//==================================================================================================
//
//! \file       util/CT_Blk_CtrlAlgo.h
//! \ingroup    util
//! \brief      Implementation of different control algorithms.
//!
//! Implementation of control algorithms such as PID control, BangBang control etc.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author     Thomas Hug
//
// $Date: 2016/11/10 08:58:38MEZ $
// $Revision: 1.19 $
//
//==================================================================================================

#ifndef _CT_Blk_CtrlAlgo__h
#define _CT_Blk_CtrlAlgo__h


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

extern const RB_DECL_CONST CT_tBlockFunction PIDBlockFunction;

//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

// -------------------------------------------------------------------------------------------------

// Definitions for PID controller
#define CT_BLK_CTRLALGO_PID_MODELFLAG_PID  0
#define CT_BLK_CTRLALGO_PID_MODELFLAG_BANGBANG 1

// Types for PID controller
enum {
    CT_BLK_CTRLALGO_PID_PARAM_ID_MODELFLAG = 0,
    CT_BLK_CTRLALGO_PID_PARAM_ID_KP = 1,
    CT_BLK_CTRLALGO_PID_PARAM_ID_TI = 2,
    CT_BLK_CTRLALGO_PID_PARAM_ID_TD = 3,
    CT_BLK_CTRLALGO_PID_PARAM_ID_N = 4,
    CT_BLK_CTRLALGO_PID_PARAM_ID_REF = 5,
    CT_BLK_CTRLALGO_PID_PARAM_ID_POS_CTRL_ERROR_OUTPUT = 6,
    CT_BLK_CTRLALGO_PID_PARAM_ID_NEG_CTRL_ERROR_OUTPUT = 7,
    CT_BLK_CTRLALGO_PID_NUM_PARAM
};

enum {
    CT_BLK_CTRLALGO_PID_IN_SIG_CTRLERR = 0,
    CT_BLK_CTRLALGO_PID_NUM_IN_SIG
};

enum {
    CT_BLK_CTRLALGO_PID_OUT_SIG_CTRLOUT = 0,
    CT_BLK_CTRLALGO_PID_NUM_OUT_SIG
};

//!< State structure for PID controller
typedef struct {
    bool        enabled;                // Flag if block is enabled
    CT_tFloat   ef1;                    // Signal state: Filtered control error at time k-1
    CT_tFloat   uI1;                    // Signal state: Output of I-part at time k-1
    CT_tFloat   u1;                     // Signal state: Controller output at time k-1
    CT_tFloat   kp;                     // PID parameter: kp
    CT_tFloat   kiTs;                   // PID parameter: ki
    CT_tFloat   kdTs;                   // PID parameter: kd
    CT_tFloat   a;                      // PID Filter parameter: a
    CT_tFloat   b;                      // PID Filter parameter: b
    bool        isDiffFilter;           // PID Filter parameter: flag if differential part is
                                        //                       filtered
} RB_DECL_TYPE CT_BLK_CTRLALGO_tPIDState;

//!< Parameter structure for PID controller
typedef struct {
    float32     modelFlag;              // Model flag of the PID control block
    float32     Kp;                     // PID parameter Kp
    float32     Ti;                     // PID parameter Ti
    float32     Td;                     // PID parameter Td
    float32     N;                      // PID parameter N defining the filter time constant Tf=N*Td
    float32     yref;                   // Internal reference used for auto tuning algorithm
    float32     posCtrlErrOut;          // Upper limit of the bang bang controller
    float32     negCtrlErrOut;          // Lower limit of the bang bang controller
} RB_DECL_TYPE CT_BLK_CTRLALGO_tPIDParam;

//!< Block context structure
typedef struct {
    CT_BLK_CTRLALGO_tPIDParam   param;
    CT_BLK_CTRLALGO_tPIDState   state;
    CT_tFloat                   inputSignal[CT_BLK_CTRLALGO_PID_NUM_IN_SIG];
    CT_tFloat                   outputSignal[CT_BLK_CTRLALGO_PID_NUM_OUT_SIG];
    bool                        enabled;
} RB_DECL_TYPE CT_BLK_CTRLALGO_tPIDBlock;

//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

#define CT_BLK_CTRLALGO_BLK_ITEMTABLE                                                                                                                   \
    /*          Block type identifier,              Block function struct,  Block parameter size,               Block state size                    */  \
    CT_BLK_ITEM(CT_BLK_CTRLALGO_PID_BLKTYPE,        PIDBlockFunction,       sizeof(CT_BLK_CTRLALGO_tPIDParam),  sizeof(CT_BLK_CTRLALGO_tPIDState)),     \


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// CT_BLK_CTRLALGO_InitPIDControl
//--------------------------------------------------------------------------------------------------
//! \brief  Initialize PID control block
//!
//! \param  vParam          input/output    Pointer to parameter struct
//! \param  vState          output          Pointer to state struct
//! \param  enabled         output          Pointer to block enable flag
//! \param  tSamp           input           Sampling time
//! \param  execReason      input           Execution reason
//! \return On success, a zero value is returned.
//!         Otherwise, a non-zero value is returned which specifies the error.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_CTRLALGO_InitPIDControl(void* vParam, void* vState, bool* enabled, float32 tSamp, CT_tBlkExecReason execReason);

//--------------------------------------------------------------------------------------------------
// CT_BLK_CTRLALGO_PIDControl
//--------------------------------------------------------------------------------------------------
//! \brief  Control algorithm for a PID controller
//!
//! Implementation of a parallel PID (proportional, integral, differential) controller.
//!
//! The control algorithm is derived from the analog PID controller:
//! u(t) = Kp*e(t) + Kp/Ti * integral(e*dt) + Kp*Td*def(t)/dt]
//! where def(t) is the derivative of the filtered error signal with
//! EF(s) = 1 / (Tf*s + 1) * E(s) where EF(s) and E(s) are the Laplace-transformed signals ef and e,
//! respectively. The parameter Tf is defined by Tf = Td/N.
//! The controller is discretized with the Euler backward differentiation method:
//! u(tk) = Kp*e(tk) + uI(tk) + KdTs*[ef(tk)-ef(tk-1)]
//! where uI(tk) = uI(tk-1) + KiTs*e(tk),
//! and KiTs = Kp*ts/Ti,
//! and KdTs = Kp*Td/ts
//!
//! \param  in      input           Input signal
//! \param  block   input/output    Block context
//! \return Controller output signal
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC CT_tFloat CT_BLK_CTRLALGO_DoPIDControl(CT_tFloat in, CT_BLK_CTRLALGO_tPIDBlock* block);

//--------------------------------------------------------------------------------------------------
// CT_BLK_CTRLALGO_SetPIDControlParameter
//--------------------------------------------------------------------------------------------------
//! \brief  Set block parameter
//!
//! \param  paramVector input   Pointer to general parameter array
//! \param  blkParam    output  Pointer to stored block parameters
//!
//! \return On success, a zero value is returned.
//!         Otherwise, a non-zero value is returned which specifies the error.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_CTRLALGO_SetPIDControlParameter(const CT_tBlockParameter* paramVector, void* blkParam);

//--------------------------------------------------------------------------------------------------
// CT_BLK_CTRLALGO_GetPIDControlParameter
//--------------------------------------------------------------------------------------------------
//! \brief  Get block parameter
//!
//! \param  blkParam    input   Pointer to stored block parameters
//! \param  paramVector output  Pointer to general parameter array
//!
//! \return On success, a zero value is returned.
//!         Otherwise, a non-zero value is returned which specifies the error.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_CTRLALGO_GetPIDControlParameter(const void* blkParam, CT_tBlockParameter* paramVector);

//--------------------------------------------------------------------------------------------------
// CT_BLK_CTRLALGO_GetNumPIDControlParameter
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block parameter
//!
//! \return Number of block parameter
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_CTRLALGO_GetNumPIDControlParameter(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_CTRLALGO_GetNumPIDControlStates
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block states
//!
//! \return Number of block states
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_CTRLALGO_GetNumPIDControlStates(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_CTRLALGO_GetNumPIDControlInputSignals
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block input signals
//!
//! \return Number of block input signals
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_CTRLALGO_GetNumPIDControlInputSignals(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_CTRLALGO_GetNumPIDControlOutputSignals
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block output signals
//!
//! \return Number of block output signals
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_CTRLALGO_GetNumPIDControlOutputSignals(void);


#ifdef __cplusplus
}
#endif

#endif // _CT_Blk_CtrlAlgo__h
