//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_ASCII.h
//! \ingroup	util
//! \brief		ASCII control character definitions
//!
//! ASCII-Code (American Standard Code for Information Interchange)	definitions
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger
//
// $Date: 2016/11/10 08:58:38MEZ $
// $Revision: 1.17 $
//
//==================================================================================================

#ifndef _RB_ASCII__h
#define _RB_ASCII__h


#define RB_ASCII_NUL   0x00    // Null or Nil
#define RB_ASCII_SOH   0x01    // Start Of Heading
#define RB_ASCII_STX   0x02    // Start of TeXt
#define RB_ASCII_ETX   0x03    // End of TeXt
#define RB_ASCII_EOT   0x04    // End Of Transmission
#define RB_ASCII_ENQ   0x05    // ENQuiry
#define RB_ASCII_ACK   0x06    // ACKnowledge
#define RB_ASCII_BEL   0x07    // BEL
#define RB_ASCII_BS    0x08    // BackSpace
#define RB_ASCII_HT    0x09    // Horizontal Tabulation
#define RB_ASCII_LF    0x0A    // Line Feed
#define RB_ASCII_VT    0x0B    // Vertical Tabulation
#define RB_ASCII_FF    0x0C    // Form Feed
#define RB_ASCII_CR    0x0D    // Carriage Return
#define RB_ASCII_SO    0x0E    // Shift Out
#define RB_ASCII_SI    0x0F    // Shift In
#define RB_ASCII_DLE   0x10    // Data Link Escape
#define RB_ASCII_DC1   0x11    // Device Control 1
#define RB_ASCII_XON   0x11    // Use for protocol XON
#define RB_ASCII_DC2   0x12    // Device Control 2
#define RB_ASCII_DC3   0x13    // Device Control 3
#define RB_ASCII_XOFF  0x13    // Use for protocol XOFF
#define RB_ASCII_DC4   0x14    // Device Control 4
#define RB_ASCII_NAK   0x15    // Negative AcKnowledge
#define RB_ASCII_SYN   0x16    // SYNchronous Idle
#define RB_ASCII_ETB   0x17    // End of Transmission Block
#define RB_ASCII_CAN   0x18    // CANcel
#define RB_ASCII_EM    0x19    // End of Medium
#define RB_ASCII_SUB   0x1A    // SUBstitute
#define RB_ASCII_ESC   0x1B    // ESCape
#define RB_ASCII_FS    0x1C    // File Separator
#define RB_ASCII_GS    0x1D    // Group Separator
#define RB_ASCII_RS    0x1E    // Record Separator
#define RB_ASCII_US    0x1F    // Unit Separator
#define RB_ASCII_SP    0x20    // SPace
#define RB_ASCII_DEL   0x7F    // DELete


#endif
