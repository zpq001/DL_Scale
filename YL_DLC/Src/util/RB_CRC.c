//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_CRC.c
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
//!			1) Declare a constant RB_CRC_tCfg variable containing your algorithm configuration
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
// $Revision: 1.35 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_CRC"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_CRC.h"

#if defined(RB_CONFIG_USE_CRC) && (RB_CONFIG_USE_CRC == RB_CONFIG_YES)


//==================================================================================================
//  L O C A L    C O N S T A N T S
//==================================================================================================

// These are 3 pre-defined CRC algorithm configurations.
// You can declare your own configurations. Please refer to h-file and links in header for more info

// 8 bit CCITT CRC
const RB_CRC_tCfg RB_CRC_8_CCITT_CFG =
	{8,0x8D /*  x8 + x7 + x3 + x2 + 1 */,false,0x00,false,0x00,0,0};

// 16 bit CCITT CRC
const RB_CRC_tCfg RB_CRC_16_CCITT_CFG =
	{16,0x1021 /*  x16 + x12 + x5 + 1  */,false,0xffff,false,0x0000,0,0};

// 32 bit IEEE802_3 CRC
const RB_CRC_tCfg RB_CRC_32_IEEE802_3_CFG =
	{32,0x04C11DB7 /*x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1 */,true,0xffffffff,true,0x00000000,0,0};


//==================================================================================================
//  F O R W A R D   D E C L A R A T I O N S
//==================================================================================================

static RB_CRC_tCRC CRCReflect(RB_CRC_tCRC data, size_t datalen);


//==================================================================================================
// G L O B A L    F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_CRC_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_CRC_Initialize(void)
{
}


//--------------------------------------------------------------------------------------------------
// RB_CRC_InitializeConfig
//--------------------------------------------------------------------------------------------------
//! \brief			Initialize an UNINITIALIZED CRC config data structure based on a predefined CRC configuration.
//! \param	cfg		A pointer to a uninitialised RB_CRC_tCfg structure.
//! \param	cfgdef	A pointer to the predefined (constant) CRC configuration
//!						(e.g. RB_CRC_8_CCITT_CFG or self declared CRC configuartion)
//! \return			true, if initialisation successful, false otherwise (if width larger than int32_t on this HW)
//--------------------------------------------------------------------------------------------------
bool RB_CRC_InitializeConfig(RB_CRC_tCfg* cfg,const RB_CRC_tCfg* cfgdef)
{
	if(cfgdef->width > sizeof(RB_CRC_tCRC) * 8)
		return false; // return false if CRC width is larger than the size of RB_CRC_tCRC


	cfg->width      = cfgdef->width;
	cfg->poly       = cfgdef->poly;
	cfg->reflectIn  = cfgdef->reflectIn;
	cfg->xorIn      = cfgdef->xorIn;
	cfg->reflectOut = cfgdef->reflectOut;
	cfg->xorOut     = cfgdef->xorOut;
	cfg->msbMask    = 0x01uL << (cfg->width - 1);
	cfg->crcMask    = 0xffffffffuL >> (sizeof(RB_CRC_tCRC) * 8 - cfg->width);

	return true;
}


//--------------------------------------------------------------------------------------------------
// RB_CRC_InitialValue
//--------------------------------------------------------------------------------------------------
//! \brief		    Calculate the initial crc value.
//! \param	cfg      A pointer to an initialised RB_CRC_tCfg structure.
//! \return	        The initial crc value.
//--------------------------------------------------------------------------------------------------
RB_CRC_tCRC RB_CRC_InitialValue(const RB_CRC_tCfg* cfg)
{
    return cfg->xorIn & cfg->crcMask;
}


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
RB_CRC_tCRC RB_CRC_UpdateValue(const RB_CRC_tCfg* cfg, RB_CRC_tCRC crc, const uint8_t* data, size_t datalen)
{
    size_t i;
    uint32_t bit;
    uint8_t c;

    while (datalen--) {
        if (cfg->reflectIn) {
            c = (uint8_t)CRCReflect(*data++, 8);
        } else {
            c = *data++;
        }
        for (i = 0x80; i > 0; i >>= 1) {
            bit = (crc & cfg->msbMask);

            if (c & i) {
					if(bit)
						bit = 0;
					else
						bit = 1;
            }
            crc <<= 1;
            if (bit) {
                crc ^= cfg->poly;
            }
        }
        crc &= cfg->crcMask;
    }
    return crc & cfg->crcMask;
}


//--------------------------------------------------------------------------------------------------
// RB_CRC_FinalizeValue
//--------------------------------------------------------------------------------------------------
//! \brief		Calculate the final crc value.
//! \param	cfg  A pointer to a initialised RB_CRC_tCfg structure.
//! \param	crc  The current crc value.
//! \return	    The final crc value. Note that the CRC value occupies exactly the last cfg->width bits.
//--------------------------------------------------------------------------------------------------
RB_CRC_tCRC RB_CRC_FinalizeValue(const RB_CRC_tCfg* cfg, RB_CRC_tCRC crc)
{
    if (cfg->reflectOut) {
        crc = CRCReflect(crc, cfg->width);
    }
    return (crc ^ cfg->xorOut) & cfg->crcMask;
}


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
RB_CRC_tCRC RB_CRC_Calculate(const uint8_t* data, size_t offset, size_t datalen,  const RB_CRC_tCfg* cfg)
{
		RB_CRC_tCfg cfgvar;
		RB_CRC_tCRC crc;
		if(RB_CRC_InitializeConfig(&cfgvar,cfg)){
			crc = RB_CRC_InitialValue(&cfgvar);	//get initial crc value
			crc = RB_CRC_UpdateValue(&cfgvar,crc,data+offset,datalen); //update initial crc value
			crc = RB_CRC_FinalizeValue(&cfgvar,crc); //we have final CRC value now
		}else{
			crc = 0; // if this happens, you have an error in your crc configuration (width too large)
		}

		return crc;
}


//==================================================================================================
// L O C A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// CRCReflect
//--------------------------------------------------------------------------------------------------
//! \brief				Reflect all bits of a \a data word of \a datalen bits.
//! \param	data         The data word to be reflected.
//! \param	datalen     The width of \a data expressed in number of bits.
//! \return				The reflected data.
//--------------------------------------------------------------------------------------------------
static RB_CRC_tCRC CRCReflect(RB_CRC_tCRC data, size_t datalen)
{
	size_t i;
    RB_CRC_tCRC ret;

    ret = 0;
    for (i = 0; i < datalen; i++)
    {
        if (data & 0x01) {
            ret = (ret << 1) | 1;
        } else {
            ret = ret << 1;
        }
        data >>= 1;
    }
    return ret;
}


#endif // RB_CONFIG_USE_CRC
