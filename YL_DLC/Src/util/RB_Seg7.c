//==================================================================================================
//											  Rainbow
//==================================================================================================
//
//! \file		RB_Seg7.c
//! \ingroup	util
//! \brief		Convert ASCII characters to 7-segment code.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger
//
// $Date: 2017/08/23 14:40:55MESZ $
// $Revision: 1.45 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Seg7"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Seg7.h"

#if defined(RB_CONFIG_USE_DISPLAY) && (RB_CONFIG_USE_DISPLAY == RB_CONFIG_YES) && \
    defined(RB_CONFIG_USE_SEG7) && (RB_CONFIG_USE_SEG7 == RB_CONFIG_YES)

#include "RB_Seg7.h"

#if defined(RB_CONFIG_DISPLAY_7_SEG_ENCODING_OHAUS) && (RB_CONFIG_DISPLAY_7_SEG_ENCODING_OHAUS == RB_CONFIG_YES)
	#define SUPPORT_ENCODING_OHAUS
#endif

#if defined(RB_CONFIG_DISPLAY_7_SEG_ENCODING_OVER_128_MT_SICS) && (RB_CONFIG_DISPLAY_7_SEG_ENCODING_OVER_128_MT_SICS == RB_CONFIG_YES)
	#define SUPPORT_128_TO_255_MT_SICS
#endif


//==================================================================================================
//	L O	C A	L	D E	F I	N I	T I	O N	S
//==================================================================================================

//!
//!		 ---a---
//!		|       |
//!		f       b
//!		|       |
//!		 ---g---
//!		|       |
//!		e       c
//!		|       |
//!		 ---d---  dp
//!
//! Segment	value definition
#define	a		RB_CONFIG_SEG7_a
#define	b		RB_CONFIG_SEG7_b
#define	c		RB_CONFIG_SEG7_c
#define	d		RB_CONFIG_SEG7_d
#define	e		RB_CONFIG_SEG7_e
#define	f		RB_CONFIG_SEG7_f
#define	g		RB_CONFIG_SEG7_g
#define	dp		RB_CONFIG_SEG7_dp

//! Segment code definition      a   b   c   d   e   f   g   dp
#define RB_SEG7_overload_L		(a + 0 + 0 + 0 + 0 + f + 0 + 0)
#define RB_SEG7_overload_M		(a + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define RB_SEG7_overload_R		(a + b + 0 + 0 + 0 + 0 + 0 + 0)
#define RB_SEG7_underload_L		(0 + 0 + 0 + d + e + 0 + 0 + 0)
#define RB_SEG7_underload_M		(0 + 0 + 0 + d + 0 + 0 + 0 + 0)
#define RB_SEG7_underload_R		(0 + 0 + c + d + 0 + 0 + 0 + 0)

#define RB_SEG7_space			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define RB_SEG7_exclam			(0 + b + 0 + 0 + 0 + 0 + 0 + dp)
#define RB_SEG7_quotedbl		(0 + b + 0 + 0 + 0 + f + 0 + 0)
#define RB_SEG7_numbersign		(a + 0 + 0 + d + 0 + 0 + g + 0)
#define RB_SEG7_dollar			(a + 0 + 0 + 0 + 0 + 0 + g + 0)
#define RB_SEG7_percent			(0 + 0 + c + 0 + 0 + f + 0 + 0)
#define RB_SEG7_ampersand		(0 + b + 0 + 0 + e + 0 + 0 + 0)
#define RB_SEG7_quoteright		(0 + b + 0 + 0 + 0 + 0 + 0 + 0)
#define RB_SEG7_parenleft		(a + 0 + 0 + d + e + f + 0 + 0)
#define RB_SEG7_parenright		(a + b + c + d + 0 + 0 + 0 + 0)
#define RB_SEG7_asterisk		(a + b + 0 + 0 + 0 + f + g + 0)
#define RB_SEG7_plus			(0 + b + c + 0 + 0 + 0 + g + 0)
#define RB_SEG7_comma			(0 + 0 + 0 + 0 + 0 + 0 + 0 + dp)
#define RB_SEG7_hyphen			(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define RB_SEG7_period			(0 + 0 + 0 + 0 + 0 + 0 + 0 + dp)
#define RB_SEG7_slash			(0 + b + 0 + 0 + e + 0 + g + 0)

#define RB_SEG7_0				(a + b + c + d + e + f + 0 + 0)
#define RB_SEG7_1				(0 + b + c + 0 + 0 + 0 + 0 + 0)
#define RB_SEG7_2				(a + b + 0 + d + e + 0 + g + 0)
#define RB_SEG7_3				(a + b + c + d + 0 + 0 + g + 0)
#define RB_SEG7_4				(0 + b + c + 0 + 0 + f + g + 0)
#define RB_SEG7_5				(a + 0 + c + d + 0 + f + g + 0)
#define RB_SEG7_6				(a + 0 + c + d + e + f + g + 0)
#define RB_SEG7_7				(a + b + c + 0 + 0 + 0 + 0 + 0)
#define RB_SEG7_8				(a + b + c + d + e + f + g + 0)
#define RB_SEG7_9				(a + b + c + d + 0 + f + g + 0)
#define RB_SEG7_colon			(a + 0 + 0 + d + 0 + 0 + 0 + 0)
#define RB_SEG7_semicolon		(0 + 0 + 0 + 0 + e + 0 + 0 + 0)
#define RB_SEG7_less			(a + 0 + 0 + 0 + 0 + f + g + 0)
#define RB_SEG7_equal			(0 + 0 + 0 + d + 0 + 0 + g + 0)
#define RB_SEG7_greater			(a + b + 0 + 0 + 0 + 0 + g + 0)
#define RB_SEG7_question		(a + b + 0 + 0 + e + 0 + g + 0)

#define RB_SEG7_at				(0 + 0 + c + d + 0 + 0 + g + 0)
#define RB_SEG7_A				(a + b + c + 0 + e + f + g + 0)
#define RB_SEG7_B				(0 + 0 + c + d + e + f + g + 0)
#define RB_SEG7_C				(a + 0 + 0 + d + e + f + 0 + 0)
#define RB_SEG7_D				(0 + b + c + d + e + 0 + g + 0)
#define RB_SEG7_E				(a + 0 + 0 + d + e + f + g + 0)
#define RB_SEG7_F				(a + 0 + 0 + 0 + e + f + g + 0)
#define RB_SEG7_G				(a + 0 + c + d + e + f + 0 + 0)
#define RB_SEG7_H				(0 + b + c + 0 + e + f + g + 0)
#define RB_SEG7_I				(0 + b + c + 0 + 0 + 0 + 0 + 0)
#define RB_SEG7_J				(0 + b + c + d + e + 0 + 0 + 0)
#define RB_SEG7_K				(0 + 0 + 0 + 0 + e + f + g + 0)
#define RB_SEG7_L				(0 + 0 + 0 + d + e + f + 0 + 0)
#ifdef SUPPORT_ENCODING_OHAUS
#define RB_SEG7_M				(a + b + c + 0 + 0 + f + 0 + 0)
#else
#define RB_SEG7_M				(a + b + c + 0 + e + f + 0 + 0)
#endif
#define RB_SEG7_N				(a + b + c + 0 + e + f + 0 + 0)
#define RB_SEG7_O				(a + b + c + d + e + f + 0 + 0)

#define RB_SEG7_P				(a + b + 0 + 0 + e + f + g + 0)
#define RB_SEG7_Q				(a + b + c + 0 + 0 + f + g + 0)
#define RB_SEG7_R				(a + 0 + 0 + 0 + e + f + 0 + 0)
#define RB_SEG7_S				(a + 0 + c + d + 0 + f + g + 0)
#define RB_SEG7_T				(0 + 0 + 0 + d + e + f + g + 0)
#define RB_SEG7_U				(0 + b + c + d + e + f + 0 + 0)
#define RB_SEG7_V				(0 + b + c + d + e + f + 0 + 0)
#ifdef SUPPORT_ENCODING_OHAUS
#define RB_SEG7_W				(0 + b + c + d + e + 0 + 0 + 0)
#else
#define RB_SEG7_W				(0 + b + c + 0 + e + f + 0 + 0)
#endif
#define RB_SEG7_X				(0 + b + c + d + 0 + 0 + 0 + 0)
#define RB_SEG7_Y				(0 + b + c + d + 0 + f + g + 0)
#define RB_SEG7_Z				(a + b + 0 + d + e + 0 + g + 0)
#define RB_SEG7_bracketlef		(a + 0 + 0 + 0 + 0 + f + 0 + 0)
#define RB_SEG7_backslash		(0 + 0 + c + 0 + 0 + f + g + 0)
#define RB_SEG7_bracketrig		(a + b + 0 + 0 + 0 + 0 + 0 + 0)
#define RB_SEG7_circumflex		(a + b + 0 + 0 + 0 + f + 0 + 0)
#define RB_SEG7_underscore		(0 + 0 + 0 + d + 0 + 0 + 0 + 0)

#define RB_SEG7_quoteleft		(0 + 0 + 0 + 0 + 0 + f + 0 + 0)
#define RB_SEG7_a				(a + b + c + d + e + 0 + g + 0)
#define RB_SEG7_b				(0 + 0 + c + d + e + f + g + 0)
#define RB_SEG7_c				(0 + 0 + 0 + d + e + 0 + g + 0)
#define RB_SEG7_d				(0 + b + c + d + e + 0 + g + 0)
#define RB_SEG7_e				(a + b + 0 + d + e + f + g + 0)
#define RB_SEG7_f				(a + 0 + 0 + 0 + e + f + g + 0)
#define RB_SEG7_g				(a + b + c + d + 0 + f + g + 0)
#define RB_SEG7_h				(0 + 0 + c + 0 + e + f + g + 0)
#define RB_SEG7_i				(0 + 0 + c + 0 + 0 + 0 + 0 + 0)
#define RB_SEG7_j				(0 + 0 + c + 0 + 0 + 0 + g + 0)
#ifdef SUPPORT_ENCODING_OHAUS
#define RB_SEG7_k				(0 + 0 + 0 + 0 + e + f + g + 0)
#else
#define RB_SEG7_k				(0 + 0 + 0 + d + e + 0 + 0 + 0)
#endif
#define RB_SEG7_l				(0 + b + c + 0 + 0 + 0 + 0 + 0)
#ifdef SUPPORT_ENCODING_OHAUS
#define RB_SEG7_m				(a + b + 0 + 0 + e + f + 0 + 0)
#else
#define RB_SEG7_m				(0 + 0 + c + 0 + e + 0 + g + 0)
#endif
#define RB_SEG7_n				(0 + 0 + c + 0 + e + 0 + g + 0)
#define RB_SEG7_o				(0 + 0 + c + d + e + 0 + g + 0)

#define RB_SEG7_p				(a + b + 0 + 0 + e + f + g + 0)
#define RB_SEG7_q				(a + b + c + 0 + 0 + f + g + 0)
#define RB_SEG7_r				(0 + 0 + 0 + 0 + e + 0 + g + 0)
#define RB_SEG7_s				(a + 0 + c + d + 0 + f + g + 0)
#define RB_SEG7_t				(0 + 0 + 0 + d + e + f + g + 0)
#define RB_SEG7_u				(0 + 0 + c + d + e + 0 + 0 + 0)
#define RB_SEG7_v				(0 + b + 0 + 0 + 0 + f + g + 0)
#ifdef SUPPORT_ENCODING_OHAUS
#define RB_SEG7_w				(0 + 0 + c + d + e + f + 0 + 0)
#else
#define RB_SEG7_w				(0 + 0 + c + 0 + e + 0 + 0 + 0)
#endif
#define RB_SEG7_x				(0 + 0 + c + d + 0 + 0 + 0 + 0)
#define RB_SEG7_y				(0 + b + c + d + 0 + f + g + 0)
#define RB_SEG7_z				(a + b + 0 + d + e + 0 + g + 0)
#define RB_SEG7_braceleft		(0 + 0 + 0 + 0 + 0 + f + g + 0)
#define RB_SEG7_bar				(0 + 0 + 0 + 0 + e + f + 0 + 0)
#define RB_SEG7_braceright		(0 + b + 0 + 0 + 0 + 0 + g + 0)
#define RB_SEG7_tilde			(a + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define RB_SEG7_delete			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)

#ifdef SUPPORT_128_TO_255_MT_SICS

#define	RB_SEG7_128				(0 + 0 + 0 + d + e + f + 0 + 0)
#define	RB_SEG7_129				(a + 0 + c + d + e + 0 + 0 + 0)
#define	RB_SEG7_130				(a + 0 + 0 + d + e + 0 + g + 0)
#define	RB_SEG7_131				(a + 0 + c + d + e + 0 + g + 0)
#define	RB_SEG7_132				(a + 0 + c + d + e + 0 + g + 0)
#define	RB_SEG7_133				(a + 0 + c + d + e + 0 + g + 0)
#define	RB_SEG7_134				(a + 0 + c + d + e + 0 + g + 0)
#define	RB_SEG7_135				(0 + 0 + 0 + d + e + 0 + g + 0)
#define	RB_SEG7_136				(a + 0 + 0 + d + e + 0 + g + 0)
#define	RB_SEG7_137				(a + 0 + 0 + d + e + 0 + g + 0)
#define	RB_SEG7_138				(a + 0 + 0 + d + e + 0 + g + 0)
#define	RB_SEG7_139				(a + b + c + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG7_140				(a + b + c + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG7_141				(a + b + c + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG7_142				(a + b + c + 0 + e + f + g + 0)
#define	RB_SEG7_143				(a + b + c + 0 + e + f + g + 0)

#define	RB_SEG7_144				(a + 0 + 0 + d + e + f + g + 0)
#define	RB_SEG7_145				(0 + b + 0 + 0 + e + 0 + g + 0)
#define	RB_SEG7_146				(a + 0 + 0 + d + e + f + g + 0)
#define	RB_SEG7_147				(a + 0 + c + d + e + 0 + g + 0)
#define	RB_SEG7_148				(a + 0 + c + d + e + 0 + g + 0)
#define	RB_SEG7_149				(a + 0 + c + d + e + 0 + g + 0)
#define	RB_SEG7_150				(a + 0 + c + d + e + 0 + 0 + 0)
#define	RB_SEG7_151				(a + 0 + c + d + e + 0 + 0 + 0)
#define	RB_SEG7_152				(a + 0 + c + d + e + 0 + 0 + 0)
#define	RB_SEG7_153				(a + 0 + c + d + e + 0 + g + 0)
#define	RB_SEG7_154				(a + 0 + c + d + e + 0 + 0 + 0)
#define	RB_SEG7_155				(0 + 0 + c + d + e + 0 + g + 0)
#define	RB_SEG7_156				(a + 0 + 0 + d + e + f + g + 0)
#define	RB_SEG7_157				(0 + 0 + 0 + d + 0 + 0 + g + 0)
#define	RB_SEG7_158				(a + 0 + 0 + 0 + e + f + 0 + 0)
#define	RB_SEG7_159				(a + 0 + 0 + 0 + e + f + g + 0)

#define	RB_SEG7_160				(a + 0 + c + d + e + 0 + g + 0)
#define	RB_SEG7_161				(a + b + c + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG7_162				(a + 0 + c + d + e + 0 + g + 0)
#define	RB_SEG7_163				(a + 0 + c + d + e + 0 + 0 + 0)
#define	RB_SEG7_164				(a + 0 + c + 0 + e + 0 + g + 0)
#define	RB_SEG7_165				(a + 0 + c + 0 + e + 0 + g + 0)
#define	RB_SEG7_166				(a + b + 0 + d + 0 + f + g + 0)
#define	RB_SEG7_167				(a + b + 0 + d + 0 + f + g + 0)
#define	RB_SEG7_168				(a + 0 + 0 + d + e + 0 + g + 0)
#define	RB_SEG7_169				(0 + 0 + 0 + 0 + e + 0 + g + 0)
#define	RB_SEG7_170				(0 + 0 + c + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_171				(0 + b + c + 0 + e + f + g + 0)
#define	RB_SEG7_172				(0 + b + c + 0 + e + f + g + 0)
#define	RB_SEG7_173				(0 + 0 + c + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG7_174				(a + 0 + 0 + d + e + f + 0 + 0)
#define	RB_SEG7_175				(a + b + c + d + 0 + 0 + 0 + 0)

#define	RB_SEG7_176				(a + b + 0 + 0 + e + f + 0 + 0)
#define	RB_SEG7_177				(a + b + c + 0 + 0 + f + 0 + 0)
#define	RB_SEG7_178				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_179				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_180				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_181				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_182				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_183				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_184				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_185				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_186				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_187				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_188				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_189				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_190				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_191				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)

#define	RB_SEG7_192				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_193				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_194				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_195				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_196				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_197				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_198				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_199				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_200				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_201				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_202				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_203				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_204				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_205				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_206				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_207				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)

#define	RB_SEG7_208				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_209				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_210				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_211				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_212				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_213				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_214				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_215				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_216				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_217				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_218				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_219				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_220				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_221				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_222				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_223				(0 + 0 + 0 + 0 + 0 + 0 + g + 0)

#define	RB_SEG7_224				(0 + 0 + c + d + e + 0 + g + 0)
#define	RB_SEG7_225				(a + b + c + d + e + f + g + 0)
#define	RB_SEG7_226				(a + 0 + 0 + 0 + e + f + 0 + 0)
#define	RB_SEG7_227				(0 + 0 + c + 0 + e + 0 + g + 0)
#define	RB_SEG7_228				(a + 0 + 0 + d + e + f + g + 0)
#define	RB_SEG7_229				(0 + 0 + c + d + e + 0 + g + 0)
#define	RB_SEG7_230				(0 + 0 + c + d + e + 0 + 0 + 0)
#define	RB_SEG7_231				(0 + 0 + 0 + 0 + e + 0 + g + 0)
#define	RB_SEG7_232				(a + 0 + 0 + d + 0 + 0 + g + 0)
#define	RB_SEG7_233				(a + b + c + d + e + f + g + 0)
#define	RB_SEG7_234				(a + b + c + 0 + e + f + 0 + 0)
#define	RB_SEG7_235				(a + 0 + c + d + e + f + g + 0)
#define	RB_SEG7_236				(0 + 0 + c + d + e + 0 + g + 0)
#define	RB_SEG7_237				(0 + 0 + 0 + d + 0 + 0 + g + 0)
#define	RB_SEG7_238				(a + 0 + 0 + d + e + f + g + 0)
#define	RB_SEG7_239				(a + b + c + 0 + e + f + 0 + 0)

#define	RB_SEG7_240				(a + 0 + 0 + d + 0 + 0 + g + 0)
#define	RB_SEG7_241				(a + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_242				(a + b + 0 + d + 0 + 0 + g + 0)
#define	RB_SEG7_243				(a + 0 + 0 + d + 0 + f + g + 0)
#define	RB_SEG7_244				(a + 0 + 0 + 0 + 0 + f + 0 + 0)
#define	RB_SEG7_245				(0 + 0 + c + d + 0 + 0 + 0 + 0)
#define	RB_SEG7_246				(a + 0 + 0 + d + 0 + 0 + g + 0)
#define	RB_SEG7_247				(a + 0 + 0 + 0 + 0 + 0 + g + 0)
#define	RB_SEG7_248				(a + b + 0 + 0 + 0 + f + g + 0)
#define	RB_SEG7_249				(a + b + 0 + 0 + 0 + f + g + 0)
#define	RB_SEG7_250				(a + b + 0 + 0 + 0 + f + g + 0)
#define	RB_SEG7_251				(0 + b + c + d + 0 + 0 + 0 + 0)
#define	RB_SEG7_252				(0 + 0 + c + 0 + e + 0 + g + 0)
#define	RB_SEG7_253				(0 + b + 0 + 0 + 0 + f + 0 + 0)
#define	RB_SEG7_254				(0 + b + 0 + 0 + 0 + f + 0 + 0)
#define	RB_SEG7_255				(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)

#endif // SUPPORT_128_TO_255_MT_SICS


//==================================================================================================
//	L O	C A	L	V A	R I	A B	L E	S
//==================================================================================================

//lint -e941 //Note: Result 0 due to operand(s) equaling 0 in operation '+'

//!	7 Segment Table
static const uint8_t S_CodeTable_26to127[] = {

	RB_SEG7_overload_L,
	RB_SEG7_overload_M,
	RB_SEG7_overload_R,
	RB_SEG7_underload_L,
	RB_SEG7_underload_M,
	RB_SEG7_underload_R,

	RB_SEG7_space,
	RB_SEG7_exclam,
	RB_SEG7_quotedbl,
	RB_SEG7_numbersign,
	RB_SEG7_dollar,
	RB_SEG7_percent,
	RB_SEG7_ampersand,
	RB_SEG7_quoteright,
	RB_SEG7_parenleft,
	RB_SEG7_parenright,
	RB_SEG7_asterisk,
	RB_SEG7_plus,
	RB_SEG7_comma,
	RB_SEG7_hyphen,
	RB_SEG7_period,
	RB_SEG7_slash,

	RB_SEG7_0,
	RB_SEG7_1,
	RB_SEG7_2,
	RB_SEG7_3,
	RB_SEG7_4,
	RB_SEG7_5,
	RB_SEG7_6,
	RB_SEG7_7,
	RB_SEG7_8,
	RB_SEG7_9,
	RB_SEG7_colon,
	RB_SEG7_semicolon,
	RB_SEG7_less,
	RB_SEG7_equal,
	RB_SEG7_greater,
	RB_SEG7_question,

	RB_SEG7_at,
	RB_SEG7_A,
	RB_SEG7_B,
	RB_SEG7_C,
	RB_SEG7_D,
	RB_SEG7_E,
	RB_SEG7_F,
	RB_SEG7_G,
	RB_SEG7_H,
	RB_SEG7_I,
	RB_SEG7_J,
	RB_SEG7_K,
	RB_SEG7_L,
	RB_SEG7_M,
	RB_SEG7_N,
	RB_SEG7_O,

	RB_SEG7_P,
	RB_SEG7_Q,
	RB_SEG7_R,
	RB_SEG7_S,
	RB_SEG7_T,
	RB_SEG7_U,
	RB_SEG7_V,
	RB_SEG7_W,
	RB_SEG7_X,
	RB_SEG7_Y,
	RB_SEG7_Z,
	RB_SEG7_bracketlef,
	RB_SEG7_backslash,
	RB_SEG7_bracketrig,
	RB_SEG7_circumflex,
	RB_SEG7_underscore,

	RB_SEG7_quoteleft,
	RB_SEG7_a,
	RB_SEG7_b,
	RB_SEG7_c,
	RB_SEG7_d,
	RB_SEG7_e,
	RB_SEG7_f,
	RB_SEG7_g,
	RB_SEG7_h,
	RB_SEG7_i,
	RB_SEG7_j,
	RB_SEG7_k,
	RB_SEG7_l,
	RB_SEG7_m,
	RB_SEG7_n,
	RB_SEG7_o,

	RB_SEG7_p,
	RB_SEG7_q,
	RB_SEG7_r,
	RB_SEG7_s,
	RB_SEG7_t,
	RB_SEG7_u,
	RB_SEG7_v,
	RB_SEG7_w,
	RB_SEG7_x,
	RB_SEG7_y,
	RB_SEG7_z,
	RB_SEG7_braceleft,
	RB_SEG7_bar,
	RB_SEG7_braceright,
	RB_SEG7_tilde,
	RB_SEG7_delete
};

#ifdef SUPPORT_128_TO_255_MT_SICS

static const unsigned char S_CodeTable_128to255[] = {

	RB_SEG7_128,
	RB_SEG7_129,
	RB_SEG7_130,
	RB_SEG7_131,
	RB_SEG7_132,
	RB_SEG7_133,
	RB_SEG7_134,
	RB_SEG7_135,
	RB_SEG7_136,
	RB_SEG7_137,
	RB_SEG7_138,
	RB_SEG7_139,
	RB_SEG7_140,
	RB_SEG7_141,
	RB_SEG7_142,
	RB_SEG7_143,

	RB_SEG7_144,
	RB_SEG7_145,
	RB_SEG7_146,
	RB_SEG7_147,
	RB_SEG7_148,
	RB_SEG7_149,
	RB_SEG7_150,
	RB_SEG7_151,
	RB_SEG7_152,
	RB_SEG7_153,
	RB_SEG7_154,
	RB_SEG7_155,
	RB_SEG7_156,
	RB_SEG7_157,
	RB_SEG7_158,
	RB_SEG7_159,

	RB_SEG7_160,
	RB_SEG7_161,
	RB_SEG7_162,
	RB_SEG7_163,
	RB_SEG7_164,
	RB_SEG7_165,
	RB_SEG7_166,
	RB_SEG7_167,
	RB_SEG7_168,
	RB_SEG7_169,
	RB_SEG7_170,
	RB_SEG7_171,
	RB_SEG7_172,
	RB_SEG7_173,
	RB_SEG7_174,
	RB_SEG7_175,

	RB_SEG7_176,
	RB_SEG7_177,
	RB_SEG7_178,
	RB_SEG7_179,
	RB_SEG7_180,
	RB_SEG7_181,
	RB_SEG7_182,
	RB_SEG7_183,
	RB_SEG7_184,
	RB_SEG7_185,
	RB_SEG7_186,
	RB_SEG7_187,
	RB_SEG7_188,
	RB_SEG7_189,
	RB_SEG7_190,
	RB_SEG7_191,

	RB_SEG7_192,
	RB_SEG7_193,
	RB_SEG7_194,
	RB_SEG7_195,
	RB_SEG7_196,
	RB_SEG7_197,
	RB_SEG7_198,
	RB_SEG7_199,
	RB_SEG7_200,
	RB_SEG7_201,
	RB_SEG7_202,
	RB_SEG7_203,
	RB_SEG7_204,
	RB_SEG7_205,
	RB_SEG7_206,
	RB_SEG7_207,

	RB_SEG7_208,
	RB_SEG7_209,
	RB_SEG7_210,
	RB_SEG7_211,
	RB_SEG7_212,
	RB_SEG7_213,
	RB_SEG7_214,
	RB_SEG7_215,
	RB_SEG7_216,
	RB_SEG7_217,
	RB_SEG7_218,
	RB_SEG7_219,
	RB_SEG7_220,
	RB_SEG7_221,
	RB_SEG7_222,
	RB_SEG7_223,

	RB_SEG7_224,
	RB_SEG7_225,
	RB_SEG7_226,
	RB_SEG7_227,
	RB_SEG7_228,
	RB_SEG7_229,
	RB_SEG7_230,
	RB_SEG7_231,
	RB_SEG7_232,
	RB_SEG7_233,
	RB_SEG7_234,
	RB_SEG7_235,
	RB_SEG7_236,
	RB_SEG7_237,
	RB_SEG7_238,
	RB_SEG7_239,

	RB_SEG7_240,
	RB_SEG7_241,
	RB_SEG7_242,
	RB_SEG7_243,
	RB_SEG7_244,
	RB_SEG7_245,
	RB_SEG7_246,
	RB_SEG7_247,
	RB_SEG7_248,
	RB_SEG7_249,
	RB_SEG7_250,
	RB_SEG7_251,
	RB_SEG7_252,
	RB_SEG7_253,
	RB_SEG7_254,
	RB_SEG7_255
};

#endif // SUPPORT_128_TO_255_MT_SICS

//lint +e941


//==================================================================================================
//	G L	O B	A L	  F	U N	C T	I O	N	I M	P L	E M	E N	T A	T I	O N
//==================================================================================================


//--------------------------------------------------------------------------------------------------
// RB_SEG7_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize module
//!
//--------------------------------------------------------------------------------------------------
void RB_SEG7_Initialize(void)
	{
	}


//--------------------------------------------------------------------------------------------------
// RB_SEG7_GetCode
//--------------------------------------------------------------------------------------------------
//! \brief	Return 7-segment code for an ASCII character, range	0x1A..0x7F/0xFF, otherwise return 0.
//!
//! \param	ascii	input	ASCII character
//! \return	7-segment code
//--------------------------------------------------------------------------------------------------
uint8_t RB_SEG7_GetCode(char ascii)
{
	uint8_t index = (uint8_t)ascii;

	if ((index >= 0x1A)	&& (index <= 0x7F)) {           // 0x1A..0x7F |  26..127
		return (S_CodeTable_26to127[index - 0x1A]);
	}
#ifdef SUPPORT_128_TO_255_MT_SICS
	return (S_CodeTable_128to255[index - 0x80]);
#else
	return (0);
#endif
}


#endif // RB_CONFIG_USE_DISPLAY && RB_CONFIG_USE_SEG7
