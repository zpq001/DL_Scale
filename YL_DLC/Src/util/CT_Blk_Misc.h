//==================================================================================================
//                                     Digital Control
//==================================================================================================
//
//! \file       util/CT_Blk_Misc.h
//! \ingroup    util
//! \brief      Miscellaneous blocks used for digital controller.
//!
//! These blocks are used for different purposes. E.g. there is a block dividing the controller
//! output signal to two actuators considering constant power (constant power block).
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author     Thomas Hug
//
// $Date: 2017/09/20 13:01:51MESZ $
// $Revision: 1.22 $
//
//==================================================================================================

#ifndef _CT_Blk_Misc__h
#define _CT_Blk_Misc__h


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

extern const RB_DECL_CONST CT_tBlockFunction CT_BLK_MISC_ConstPowerBlkFunctions;
extern const RB_DECL_CONST CT_tBlockFunction CT_BLK_MISC_Poly3Seg3BlkFunctions;
extern const RB_DECL_CONST CT_tBlockFunction CT_BLK_MISC_DecimatorBlkFunctions;

//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

// -------------------------------------------------------------------------------------------------
// Types for constant power block

enum {
    CT_BLK_MISC_CONSTPOWER_PARAM_ID_MODEL,                  // Model flag
    CT_BLK_MISC_CONSTPOWER_PARAM_ID_PLOSS,                  // Constant power
    CT_BLK_MISC_CONSTPOWER_PARAM_ID_RS1,                    // Serial resistor before coil 1
    CT_BLK_MISC_CONSTPOWER_PARAM_ID_RS2,                    // Serial resistor before coil 2
    CT_BLK_MISC_CONSTPOWER_PARAM_ID_RS3,                    // Model 1:
                                                            //    Serial resistor in total current; in
                                                            //    a shunt resistor circuit, this is the
                                                            //    shunt resistor
                                                            // Model 2:
                                                            //    Virtual resistor for variable part of
                                                            //    the coil power dissipation
    CT_BLK_MISC_CONSTPOWER_PARAM_ID_RC1,                    // Resistance of coil 1
    CT_BLK_MISC_CONSTPOWER_PARAM_ID_RC2,                    // Resistance of coil 2
    CT_BLK_MISC_CONSTPOWER_PARAM_ID_UOUT1MIN,               // Minimum voltage of booster 1
    CT_BLK_MISC_CONSTPOWER_PARAM_ID_UOUT1MAX,               // Maximum voltage of booster 1
    CT_BLK_MISC_CONSTPOWER_PARAM_ID_UOUT2MIN,               // Minimum voltage of booster 2
    CT_BLK_MISC_CONSTPOWER_PARAM_ID_UOUT2MAX,               // Maximum voltage of booster 2
    CT_BLK_MISC_CONSTPOWER_PARAM_ID_SWITCHFACTOR,           // Switching factor
    CT_BLK_MISC_CONSTPOWER_NUM_PARAM                        // Number of parameter
};

enum {
    CT_BLK_MISC_CONSTPOWER_IN_SIG_1 = 0,                    // Input signal
    CT_BLK_MISC_CONSTPOWER_NUM_IN_SIG
};

enum {
    CT_BLK_MISC_CONSTPOWER_OUT_SIG_1 = 0,                   // Output signal 1
    CT_BLK_MISC_CONSTPOWER_OUT_SIG_2,                       // Output signal 2
    CT_BLK_MISC_CONSTPOWER_OUT_SIG_3,                       // Output signal 3
    CT_BLK_MISC_CONSTPOWER_OUT_SIG_4,                       // Output signal 4
    CT_BLK_MISC_CONSTPOWER_NUM_OUT_SIG
};

//!< State structure
typedef struct {
    CT_tFloat   halfDeltaItotMaxMin;                        // 0.5 * (Itot_max - Itot_min)
    CT_tFloat   iTotMin;                                    // Total current through coil 1 and coil
                                                            // 2 when both D/A converters are set to
                                                            // minimum voltage output
    CT_tFloat   pLossR;                                     // pLoss * (R_C1 + R_C2)
    CT_tFloat   p1Div;                                      // 1/(R_C1 + R_C2)
    CT_tFloat   p2;                                         // R_C1 * R_C2
    CT_tFloat   p3;                                         // R_C1 + R_S1
    CT_tFloat   p4;                                         // R_C2 + R_S2
    CT_tFloat   p5;                                         // R_S3 * (R_C1 + R_C2)
    CT_tFloat   rNorm;                                      // normalization factor for calculation of square root in integer domain
    CT_tFloat   rDeNorm;                                    // de-normalization factor for calculation of square root in integer domain
    CT_tFloat   uOut1DeltaInv;                              // 1/(U_Out1max - U_Out1min)
    CT_tFloat   uOut2DeltaInv;                              // 1/(U_Out2max - U_Out2min)
    int         switchCounter;                              // counter for switching algorithm
} RB_DECL_TYPE CT_BLK_MISC_tConstPowerState;

//!< Parameter structure
typedef struct {
    char            model;
    float32         pLoss;
    float32         rS1;
    float32         rS2;
    float32         rS3;
    float32         rC1;
    float32         rC2;
    float32         uOut1Min;
    float32         uOut1Max;
    float32         uOut2Min;
    float32         uOut2Max;
    int             switchFactor;
} RB_DECL_TYPE CT_BLK_MISC_tConstPowerParam;

//!< Block context structure
typedef struct {
    CT_BLK_MISC_tConstPowerParam        param;
    CT_BLK_MISC_tConstPowerState        state;
    CT_tFloat                           inputSignal[CT_BLK_MISC_CONSTPOWER_NUM_IN_SIG];
    CT_tFloat                           outputSignal[CT_BLK_MISC_CONSTPOWER_NUM_OUT_SIG];
    bool                                enabled;
} RB_DECL_TYPE CT_BLK_MISC_tConstPowerBlock;

// -------------------------------------------------------------------------------------------------
// Types for  for thre adjacent 3rd order polynomials block

enum {
    CT_BLK_MISC_POLY3SEG3_PARAM_ID_C13 = 0,              // Cubic coefficient of polynomial in first segement
    CT_BLK_MISC_POLY3SEG3_PARAM_ID_C12,                 // Square coefficient of polynomial in first segement
    CT_BLK_MISC_POLY3SEG3_PARAM_ID_C11,                 // Linear coefficient of polynomial in first segement
    CT_BLK_MISC_POLY3SEG3_PARAM_ID_C10,                 // Constant coefficient of polynomial in first segement
    CT_BLK_MISC_POLY3SEG3_PARAM_ID_SEGPNT1,             // First Segmentation point
    CT_BLK_MISC_POLY3SEG3_PARAM_ID_C23,                 // Cubic coefficient of polynomial in second segement
    CT_BLK_MISC_POLY3SEG3_PARAM_ID_C22,                 // Square coefficient of polynomial in second segement
    CT_BLK_MISC_POLY3SEG3_PARAM_ID_C21,                 // Linear coefficient of polynomial in second segement
    CT_BLK_MISC_POLY3SEG3_PARAM_ID_C20,                 // Constant coefficient of polynomial in second segement
    CT_BLK_MISC_POLY3SEG3_PARAM_ID_SEGPNT2,             // Second segmentation point
    CT_BLK_MISC_POLY3SEG3_PARAM_ID_C33,                 // Cubic coefficient of polynomial in third segement
    CT_BLK_MISC_POLY3SEG3_PARAM_ID_C32,                 // Square coefficient of polynomial in third segement
    CT_BLK_MISC_POLY3SEG3_PARAM_ID_C31,                 // Linear coefficient of polynomial in third segement
    CT_BLK_MISC_POLY3SEG3_PARAM_ID_C30,                 // Constant coefficient of polynomial in third segement
    CT_BLK_MISC_POLY3SEG3_NUM_PARAM                     // Number of parameter
};

enum {
    CT_BLK_MISC_POLY3SEG3_IN_SIG_1 = 0,                    // Input signal
    CT_BLK_MISC_POLY3SEG3_NUM_IN_SIG
};

enum {
    CT_BLK_MISC_POLY3SEG3_OUT_SIG_1 = 0,                   // Output signal 1
    CT_BLK_MISC_POLY3SEG3_NUM_OUT_SIG
};

//!< Parameter structure
typedef struct {
    float32     c13;
    float32     c12;
    float32     c11;
    float32     c10;
    float32     segPnt1;
    float32     c23;
    float32     c22;
    float32     c21;
    float32     c20;
    float32     segPnt2;
    float32     c33;
    float32     c32;
    float32     c31;
    float32     c30;
} RB_DECL_TYPE CT_BLK_MISC_tPoly3Seg3Param;

//!< State structure
typedef struct {
    unsigned char dummy;         //!< dummy
} RB_DECL_TYPE CT_BLK_MISC_tPoly3Seg3State;

//!< Block context structure
typedef struct {
    CT_BLK_MISC_tPoly3Seg3Param        param;
    CT_BLK_MISC_tPoly3Seg3State        state;
    CT_tFloat                          inputSignal[CT_BLK_MISC_POLY3SEG3_NUM_IN_SIG];
    CT_tFloat                          outputSignal[CT_BLK_MISC_POLY3SEG3_NUM_OUT_SIG];
    bool                               enabled;
} RB_DECL_TYPE CT_BLK_MISC_tPoly3Seg3Block;

// -------------------------------------------------------------------------------------------------
// Types for decimator block

enum {
    CT_BLK_MISC_DECIMATOR_IN_SIG_1 = 0,                    // Input signal
    CT_BLK_MISC_DECIMATOR_NUM_IN_SIG
};

enum {
    CT_BLK_MISC_DECIMATOR_OUT_SIG_1 = 0,                   // Output signal 1
    CT_BLK_MISC_DECIMATOR_NUM_OUT_SIG
};

//! Decimator parameters.
//! The decimator block has no parameters, however, a dummy parameter struct needs to be defined
typedef struct
{
	unsigned char dummy;         //!< dummy
} RB_DECL_TYPE CT_BLK_MISC_tDecimatorParam;

//!< State structure
typedef struct {
    int32_t     decFactor;                                  // decimation factor
    int32_t     counter;                                    // decimation counter
    CT_tFloat   sum;                                        // decimation sum
    bool        isInitialized;                              // Flag to indicate if the block is initialized yet
} RB_DECL_TYPE CT_BLK_MISC_tDecimatorState;

//!< Block context structure
typedef struct {
    CT_BLK_MISC_tDecimatorParam         param;
    CT_BLK_MISC_tDecimatorState         state;
    CT_tFloat                           inputSignal[CT_BLK_MISC_DECIMATOR_NUM_IN_SIG];
    CT_tFloat                           outputSignal[CT_BLK_MISC_DECIMATOR_NUM_OUT_SIG];
    bool                                enabled;
} RB_DECL_TYPE CT_BLK_MISC_tDecimatorBlock;

//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

#define CT_BLK_MISC_BLK_ITEMTABLE                                                                                                                                               \
    /*          Block type identifier,              Block function struct,                  Block parameter size,                   Block state size                        */  \
    CT_BLK_ITEM(CT_BLK_MISC_CONSTPOWER_BLKTYPE,     CT_BLK_MISC_ConstPowerBlkFunctions,     sizeof(CT_BLK_MISC_tConstPowerParam),   sizeof(CT_BLK_MISC_tConstPowerState)),      \
    CT_BLK_ITEM(CT_BLK_MISC_POLY3SEG3_BLKTYPE,      CT_BLK_MISC_Poly3Seg3BlkFunctions,      sizeof(CT_BLK_MISC_tPoly3Seg3Param),    sizeof(CT_BLK_MISC_tPoly3Seg3State)),       \
    CT_BLK_ITEM(CT_BLK_MISC_DECIMATOR_BLKTYPE,      CT_BLK_MISC_DecimatorBlkFunctions,      sizeof(CT_BLK_MISC_tDecimatorParam),    sizeof(CT_BLK_MISC_tDecimatorState)),       \

//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

// -------------------------------------------------------------------------------------------------
// Functions for constant power block

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_InitConstPower
//--------------------------------------------------------------------------------------------------
//! \brief  Initialize constant power block
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
RB_DECL_FUNC int32_t CT_BLK_MISC_InitConstPower(void* vParam, void* vState, bool* enabled, float32 tSamp, CT_tBlkExecReason execReason);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_DoConstPower
//--------------------------------------------------------------------------------------------------
//! \brief  Divide input signal into two output signals under consideration of constant power
//!
//!
//! \param  input   input           Block input signal
//! \param  output2 output          Block output signal 2
//! \param  block   input/output    Block context
//! \return Block output signal 1
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC CT_tFloat CT_BLK_MISC_DoConstPower(CT_tFloat input, CT_tFloat* output2, CT_BLK_MISC_tConstPowerBlock* block);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_SetConstPowerParameter
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
RB_DECL_FUNC int32_t CT_BLK_MISC_SetConstPowerParameter(const CT_tBlockParameter* paramVector, void* blkParam);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_GetConstPowerParameter
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
RB_DECL_FUNC int32_t CT_BLK_MISC_GetConstPowerParameter(const void* blkParam, CT_tBlockParameter* paramVector);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_GetNumConstPowerParameter
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block parameter
//!
//!
//!
//! \return Number of block parameter
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_MISC_GetNumConstPowerParameter(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_GetNumConstPowerStates
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block states
//!
//!
//!
//! \return Number of block states
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_MISC_GetNumConstPowerStates(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_GetNumConstPowerInputSignals
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block input signals
//!
//!
//!
//! \return Number of block input signals
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_MISC_GetNumConstPowerInputSignals(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_GetNumConstPowerOutputSignals
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block output signals
//!
//!
//!
//! \return Number of block output signals
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_MISC_GetNumConstPowerOutputSignals(void);

// -------------------------------------------------------------------------------------------------
// Functions for thre adjacent 3rd order polynomials block

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_InitPoly3Seg3
//--------------------------------------------------------------------------------------------------
//! \brief  Initialize Poly3Seg3
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
RB_DECL_FUNC int32_t CT_BLK_MISC_InitPoly3Seg3(void* vParam, void* vState, bool* enabled, float32 tSamp, CT_tBlkExecReason execReason);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_DoPoly3Seg3
//--------------------------------------------------------------------------------------------------
//! \brief  Do Poly3Seg3
//!
//! \param  input   input           Block input signal
//! \param  block   input/output    Block context
//! \return Block output
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC CT_tFloat CT_BLK_MISC_DoPoly3Seg3(CT_tFloat input, CT_BLK_MISC_tPoly3Seg3Block* block);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_SetPoly3Seg3Parameter
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
RB_DECL_FUNC int32_t CT_BLK_MISC_SetPoly3Seg3Parameter(const CT_tBlockParameter* paramVector, void* blkParam);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_GetPoly3Seg3Parameter
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
RB_DECL_FUNC int32_t CT_BLK_MISC_GetPoly3Seg3Parameter(const void* blkParam, CT_tBlockParameter* paramVector);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_GetNumPoly3Seg3Parameter
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block parameter
//!
//!
//!
//! \return Number of block parameter
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_MISC_GetNumPoly3Seg3Parameter(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_GetNumPoly3Seg3States
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block states
//!
//!
//!
//! \return Number of block states
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_MISC_GetNumPoly3Seg3States(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_GetNumPoly3Seg3InputSignals
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block input signals
//!
//!
//!
//! \return Number of block input signals
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_MISC_GetNumPoly3Seg3InputSignals(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_GetNumPoly3Seg3OutputSignals
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block output signals
//!
//!
//!
//! \return Number of block output signals
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_MISC_GetNumPoly3Seg3OutputSignals(void);

// -------------------------------------------------------------------------------------------------
// Functions for decimator block

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_InitDecimator
//--------------------------------------------------------------------------------------------------
//! \brief  Initialize decimator block
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
RB_DECL_FUNC int32_t CT_BLK_MISC_InitDecimator(void* vParam, void* vState, bool* enabled, float32 tSamp, CT_tBlkExecReason execReason);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_DoDecimator
//--------------------------------------------------------------------------------------------------
//! \brief  Downsampling with running mean filter
//!
//! \param  input   input           Block input signal
//! \param  output output           Block output signal
//! \param  block   input/output    Block context
//! \return true if output is ready, else false
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool CT_BLK_MISC_DoDecimator(CT_tFloat input, CT_tFloat* output, CT_BLK_MISC_tDecimatorBlock* block);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_SetDecimatorParameter
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
RB_DECL_FUNC int32_t CT_BLK_MISC_SetDecimatorParameter(const CT_tBlockParameter* paramVector, void* blkParam);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_GetDecimatorParameter
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
RB_DECL_FUNC int32_t CT_BLK_MISC_GetDecimatorParameter(const void* blkParam, CT_tBlockParameter* paramVector);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_GetNumDecimatorParameter
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block parameter
//!
//!
//!
//! \return Number of block parameter
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_MISC_GetNumDecimatorParameter(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_GetNumDecimatorStates
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block states
//!
//!
//!
//! \return Number of block states
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_MISC_GetNumDecimatorStates(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_GetNumDecimatorInputSignals
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block input signals
//!
//!
//!
//! \return Number of block input signals
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_MISC_GetNumDecimatorInputSignals(void);

//--------------------------------------------------------------------------------------------------
// CT_BLK_MISC_GetNumDecimatorOutputSignals
//--------------------------------------------------------------------------------------------------
//! \brief  Get number of block output signals
//!
//!
//!
//! \return Number of block output signals
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int32_t CT_BLK_MISC_GetNumDecimatorOutputSignals(void);

#ifdef __cplusplus
}
#endif

#endif // _CT_Blk_Misc__h
