//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_CRC.h
//! \ingroup	util
//! \brief		Functions to calculate CRC codes of arbitrary data. These functions are optimized
//!					for low memory consumption (no lookup tables are used).
//!
//!	For more information about CRC algorithms refer to:
//!	"A PAINLESS GUIDE TO CRC ERROR DETECTION ALGORITHMS", available on the internet.
//!
//! Refer to http://www.zorc.breitbandkatze.de/crc.html for online calculation of CRC codes
//!
//! Usage:
//!
//!			CALCULATE CRC of a message (using predefined poly):
//!			1) Initialize a RB_CRC_tCfg variable by a call to RB_CRC_InitializeConfig.
//!				E.g. RB_CRC_InitializeConfig(&cfg,&RB_CRC_8_CCITT_CFG). Check for return value.
//!			2) Call RB_CRC_InitialValue with the initialised RB_CRC_tCfg
//!			3) Subsequent calls to RB_CRC_UpdateValue until all data has been CRCed
//!			4) Call to RB_CRC_FinalizeValue yields the final CRC value for all data
//!
//!			You can also use the convenience function RB_CRC_Calculate, which does all the steps for you
//!
//! \code
//!				static const char *str[4] = {"1","234","5678","90123"};
//!				RB_CRC_tCfg cfg;
//!				RB_CRC_tCRC crc;
//!				int i = 0;
//!				if(RB_CRC_InitializeConfig(&cfg,&RB_CRC_32_IEEE802_3_CFG)){
//!					crc = RB_CRC_InitialValue(&cfg);
//!					for(i = 0;i<4;i++){
//!						crc = RB_CRC_UpdateValue(&cfg,crc,(uint8_t *)str[i],strlen(str[i]));
//!					}
//!					crc = RB_CRC_FinalizeValue(&cfg,crc); //we have final CRC value now
//!				}
//! \endcode
//!
//!			CALCULATE CRC of a message (using custom poly):
//!			1) Declare a constant RB_CRC_tCfg variable containing your algorithm configuration.
//!			2) Initialize another RB_CRC_tCfg variable  by a call to RB_CRC_InitializeConfig.
//!				E.g. RB_CRC_InitializeConfig(&cfg,&myConfig). Check for return value.
//!			3) Call RB_CRC_InitialValue with the initialised RB_CRC_tCfg
//!			4) Subsequent calls to RB_CRC_UpdateValue until all data has been CRCed
//!			5) Call to RB_CRC_FinalizeValue yields the final CRC value for all data
//!
//!			You can also use the convenience function RB_CRC_Calculate, which does all the steps for you (except 1)
//!
//!
//! \code
//!				static const char *str[4] = {"1","234","5678","90123"};
//!				const RB_CRC_tCfg myConfig = {8,0x8D /*  x8 + x7 + x3 + x2 + 1 */,false,0x00,false,0x00,0,0};
//!				RB_CRC_tCfg cfg;
//!				RB_CRC_tCRC crc;
//!				int i = 0;
//!				if(RB_CRC_InitializeConfig(&cfg,&myConfig)){
//!					crc = RB_CRC_InitialValue(&cfg);
//!					for(i = 0;i<4;i++){
//!						crc = RB_CRC_UpdateValue(&cfg,crc,(uint8_t *)str[i],strlen(str[i]));
//!					}
//!					crc = RB_CRC_FinalizeValue(&cfg,crc); //we have final CRC value now
//!				}
//! \endcode
//!
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Andri Toggenburger
//
// $Date: 2016/11/10 08:58:38MEZ $
// $Revision: 1.30 $
//
//==================================================================================================


#ifndef _RB_CRC__h
#define _RB_CRC__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_CRC) && (RB_CONFIG_USE_CRC == RB_CONFIG_YES)

#include "RB_Typedefs.h"


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L    D E F I N I T I O N S
//==================================================================================================

//! The type of the CRC values. The size of this type limits the max. CRC width
typedef uint32_t RB_DECL_TYPE RB_CRC_tCRC;


//! The configuration type of the CRC algorithm. For more information about different CRC options refer links
//! in file's header
typedef struct {
    uint32_t width;				//!< The width of the polynom (e.g. 8,16,32)
    RB_CRC_tCRC poly;		//!< The CRC polynom. View poly in BIN format to see polynomial behind it
    bool reflectIn;       //!< Whether the input shall be reflected or not
    RB_CRC_tCRC xorIn;      //!< The initial value of the algorithm
    bool reflectOut;      //!< Wether the output shall be reflected or not
    RB_CRC_tCRC xorOut;     //!< The value which shall be XOR-ed to the final CRC value

    // internal parameters, do not change
    RB_CRC_tCRC msbMask;    //!< bitmask with the Most Significant Bit set to 1
    RB_CRC_tCRC crcMask;    //!< bitmask with all bits set to 1 except the MSB

} RB_DECL_TYPE RB_CRC_tCfg;


//==================================================================================================
//  G L O B A L    C O N S T A N T S
//==================================================================================================

//! The definitions of 3 standard configurations for 8 bit CCITT,16 bit CCITT and 32 bit IEEE802.3 CRC.
//! Refer to explanation in header about how to use them.
extern const RB_DECL_CONST RB_CRC_tCfg RB_CRC_8_CCITT_CFG;

extern const RB_DECL_CONST RB_CRC_tCfg RB_CRC_16_CCITT_CFG;

extern const RB_DECL_CONST RB_CRC_tCfg RB_CRC_32_IEEE802_3_CFG;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_CRC_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_CRC_Initialize(void);

//--------------------------------------------------------------------------------------------------
// RB_CRC_InitializeConfig
//--------------------------------------------------------------------------------------------------
//! \brief			Initialize an UNINITIALIZED CRC config data structure based on a predefined CRC configuration.
//! \param	cfg		A pointer to a uninitialised RB_CRC_tCfg structure.
//! \param	cfgdef	A pointer to the predefined (constant) CRC configuration
//!					(e.g. RB_CRC_8_CCITT_CFG or self declared CRC configuartion)
//! \return			true, if initialisation successful, false otherwise (if width larger than int32_t on this HW)
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_CRC_InitializeConfig(RB_CRC_tCfg* cfg, const RB_CRC_tCfg* cfgdef);

//--------------------------------------------------------------------------------------------------
// RB_CRC_InitialValue
//--------------------------------------------------------------------------------------------------
//! \brief		    Calculate the initial crc value.
//! \param	cfg      A pointer to an initialised RB_CRC_tCfg structure.
//! \return	        The initial crc value.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_CRC_tCRC RB_CRC_InitialValue(const RB_CRC_tCfg *cfg);

//--------------------------------------------------------------------------------------------------
// RB_CRC_UpdateValue
//--------------------------------------------------------------------------------------------------
//! \brief		    Update the crc value with new data.
//! \param	crc      The current crc value.
//! \param	cfg      A pointer to a initialised RB_CRC_tCfg structure.
//! \param	data     Pointer to a buffer of \a data_len bytes.
//! \param	datalen  Number of bytes in the \a data buffer.
//! \return	        The updated crc value.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_CRC_tCRC RB_CRC_UpdateValue(const RB_CRC_tCfg* cfg, RB_CRC_tCRC crc, const uint8_t* data, size_t datalen);

//--------------------------------------------------------------------------------------------------
// RB_CRC_FinalizeValue
//--------------------------------------------------------------------------------------------------
//! \brief		Calculate the final crc value.
//! \param	cfg  A pointer to a initialised RB_CRC_tCfg structure.
//! \param	crc  The current crc value.
//! \return	    The final crc value.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_CRC_tCRC RB_CRC_FinalizeValue(const RB_CRC_tCfg* cfg, RB_CRC_tCRC crc);

//--------------------------------------------------------------------------------------------------
// RB_CRC_Calculate
//--------------------------------------------------------------------------------------------------
//! \brief			Calculate crc value of a buffer
//! \param	cfg		A pointer to a predefined (constant) CRC configuration
//! \param	data		Pointer to buffer to be CRCed
//! \param	offset	Offset into the buffer
//! \param	datalen	Length of data to be CRCed (starting at offset)
//! \return			The final crc value.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_CRC_tCRC RB_CRC_Calculate(const uint8_t* data, size_t offset, size_t datalen,  const RB_CRC_tCfg* cfg);


#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_CRC
#endif // _RB_CRC__h
