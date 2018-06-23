//==================================================================================================
//											  Rainbow
//==================================================================================================
//
//! \file		RB_Seg14.c
//! \ingroup	util
//! \brief		Convert ASCII characters to 14-segment code.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger, Matthias Klaey
//
// $Date: 2017/08/23 14:40:55MESZ $
// $Revision: 1.42 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Seg14"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Seg14.h"

#if defined(RB_CONFIG_USE_DISPLAY) && (RB_CONFIG_USE_DISPLAY == RB_CONFIG_YES) && \
    defined(RB_CONFIG_USE_SEG14) && (RB_CONFIG_USE_SEG14 == RB_CONFIG_YES)


#if defined(RB_CONFIG_DISPLAY_14_SEG_ENCODING_OVER_128_MT_SICS) && (RB_CONFIG_DISPLAY_14_SEG_ENCODING_OVER_128_MT_SICS == RB_CONFIG_YES)
	#define SUPPORT_128_TO_255_MT_SICS
#endif


//==================================================================================================
//	L O	C A	L	D E	F I	N I	T I	O N	S
//==================================================================================================

//!
//!		 -----a-----
//!		|\    |    /|
//!		f  l  m  n  b
//!		|    \|/    |
//!		 --k-- --g--
//!		|    /|\    |
//!		e  j  i  h  c
//!		|/    |    \|
//!		 -----d-----  dp
//!
//! Segment	value definition
#define	a		RB_CONFIG_SEG14_a
#define	b		RB_CONFIG_SEG14_b
#define	c		RB_CONFIG_SEG14_c
#define	d		RB_CONFIG_SEG14_d
#define	e		RB_CONFIG_SEG14_e
#define	f		RB_CONFIG_SEG14_f
#define	g		RB_CONFIG_SEG14_g
#define	h		RB_CONFIG_SEG14_h
#define	i		RB_CONFIG_SEG14_i
#define	j		RB_CONFIG_SEG14_j
#define	k		RB_CONFIG_SEG14_k
#define	l		RB_CONFIG_SEG14_l
#define	m		RB_CONFIG_SEG14_m
#define	n		RB_CONFIG_SEG14_n
#define	dp		RB_CONFIG_SEG14_dp

//! Segment	code definition      a   b   c   d   e   f   g   h   i   j   k   l   m   n   dp
#define	RB_SEG14_space			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_exclam			(0 + b + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + dp)
#define	RB_SEG14_quotedbl		(0 + b + 0 + 0 + 0 + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_numbersign		(a + b + c + d + e + f + g + h + i + j + k + l + m + n + 0)
#define	RB_SEG14_dollar			(a + 0 + c + d + 0 + f + g + 0 + i + 0 + k + 0 + m + 0 + 0)
#define	RB_SEG14_percent		(0 + 0 + c + 0 + 0 + f + 0 + 0 + 0 + j + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_ampersand		(a + 0 + c + d + e + 0 + 0 + h + 0 + 0 + k + l + 0 + n + 0)
#define	RB_SEG14_quote			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + m + 0 + 0)
#define	RB_SEG14_parenleft		(0 + 0 + 0 + 0 + 0 + 0 + 0 + h + 0 + 0 + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_parenright		(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + j + 0 + l + 0 + 0 + 0)
#define	RB_SEG14_asterisk		(0 + 0 + 0 + 0 + 0 + 0 + g + h + i + j + k + l + m + n + 0)
#define	RB_SEG14_plus			(0 + 0 + 0 + 0 + 0 + 0 + g + 0 + i + 0 + k + 0 + m + 0 + 0)
#define	RB_SEG14_comma			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + j + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_hyphen			(0 + 0 + 0 + 0 + 0 + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_period			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + dp)
#define	RB_SEG14_slash			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + j + 0 + 0 + 0 + n + 0)

#define	RB_SEG14_0				(a + b + c + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_1				(0 + b + c + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_2				(a + b + 0 + d + e + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_3				(a + b + c + d + 0 + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_4				(0 + b + c + 0 + 0 + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_5				(a + 0 + c + d + 0 + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_6				(a + 0 + c + d + e + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_7				(a + b + c + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_8				(a + b + c + d + e + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_9				(a + b + c + d + 0 + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_colon			(a + 0 + 0 + d + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_semicolon		(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + j + 0 + 0 + m + 0 + 0)
#define	RB_SEG14_less			(0 + 0 + 0 + 0 + 0 + 0 + 0 + h + 0 + 0 + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_equal			(0 + 0 + 0 + d + 0 + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_greater		(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + j + 0 + l + 0 + 0 + 0)
#define	RB_SEG14_question		(a + b + 0 + 0 + 0 + 0 + g + 0 + i + 0 + 0 + 0 + 0 + 0 + 0)

#define	RB_SEG14_at				(a + b + 0 + d + e + f + g + 0 + 0 + 0 + 0 + 0 + m + 0 + 0)
#define	RB_SEG14_A				(a + b + c + 0 + e + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_B				(a + b + c + d + 0 + 0 + g + 0 + i + 0 + 0 + 0 + m + 0 + 0)
#define	RB_SEG14_C				(a + 0 + 0 + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_D				(a + b + c + d + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + m + 0 + 0)
#define	RB_SEG14_E				(a + 0 + 0 + d + e + f + 0 + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_F				(a + 0 + 0 + 0 + e + f + 0 + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_G				(a + 0 + c + d + e + f + g + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_H				(0 + b + c + 0 + e + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_I				(a + 0 + 0 + d + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + m + 0 + 0)
#define	RB_SEG14_J				(0 + b + c + d + e + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_K				(0 + 0 + 0 + 0 + e + f + 0 + h + 0 + 0 + k + 0 + 0 + n + 0)
#define	RB_SEG14_L				(0 + 0 + 0 + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_M				(0 + b + c + 0 + e + f + 0 + 0 + 0 + 0 + 0 + l + 0 + n + 0)
#define	RB_SEG14_N				(0 + b + c + 0 + e + f + 0 + h + 0 + 0 + 0 + l + 0 + 0 + 0)
#define	RB_SEG14_O				(a + b + c + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)

#define	RB_SEG14_P				(a + b + 0 + 0 + e + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_Q				(a + b + c + d + e + f + 0 + h + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_R				(a + b + 0 + 0 + e + f + g + h + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_S				(a + 0 + c + d + 0 + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_T				(a + 0 + 0 + 0 + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + m + 0 + 0)
#define	RB_SEG14_U				(0 + b + c + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_V				(0 + 0 + 0 + 0 + e + f + 0 + 0 + 0 + j + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_W				(0 + b + c + 0 + e + f + 0 + h + 0 + j + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_X				(0 + 0 + 0 + 0 + 0 + 0 + 0 + h + 0 + j + 0 + l + 0 + n + 0)
#define	RB_SEG14_Y				(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + i + 0 + 0 + l + 0 + n + 0)
#define	RB_SEG14_Z				(a + 0 + 0 + d + 0 + 0 + 0 + 0 + 0 + j + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_bracketlef		(a + 0 + 0 + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_backslash		(0 + 0 + 0 + 0 + 0 + 0 + 0 + h + 0 + 0 + 0 + l + 0 + 0 + 0)
#define	RB_SEG14_bracketrig		(a + b + c + d + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_circumflex		(0 + 0 + 0 + 0 + 0 + f + 0 + 0 + 0 + 0 + 0 + l + 0 + 0 + 0)
#define	RB_SEG14_underscore		(0 + 0 + 0 + d + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)

#define	RB_SEG14_quoteleft		(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + l + 0 + 0 + 0)
#define	RB_SEG14_a				(0 + 0 + 0 + d + 0 + 0 + 0 + h + 0 + j + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_b				(0 + 0 + c + d + e + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_c				(0 + 0 + 0 + d + e + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_d				(0 + b + c + d + e + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_e				(0 + 0 + 0 + d + e + 0 + 0 + 0 + 0 + j + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_f				(0 + 0 + 0 + 0 + 0 + 0 + g + 0 + i + 0 + k + 0 + 0 + n + 0)
#define	RB_SEG14_g				(a + b + c + d + 0 + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_h				(0 + 0 + c + 0 + e + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_i				(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_j				(0 + b + c + 0 + 0 + 0 + 0 + h + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_k				(0 + 0 + 0 + 0 + e + f + g + h + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_l				(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + m + 0 + 0)
#define	RB_SEG14_m				(0 + 0 + c + 0 + e + 0 + g + 0 + i + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_n				(0 + 0 + c + 0 + e + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_o				(0 + 0 + c + d + e + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)

#define	RB_SEG14_p				(0 + 0 + 0 + 0 + e + f + 0 + 0 + 0 + 0 + k + l + 0 + 0 + 0)
#define	RB_SEG14_q				(0 + b + c + 0 + 0 + 0 + g + 0 + 0 + 0 + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_r				(0 + 0 + 0 + 0 + e + 0 + 0 + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_s				(0 + 0 + 0 + d + 0 + 0 + g + h + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_t				(0 + 0 + 0 + d + e + f + 0 + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_u				(0 + 0 + c + d + e + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_v				(0 + 0 + 0 + 0 + e + 0 + 0 + 0 + 0 + j + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_w				(0 + 0 + c + 0 + e + 0 + 0 + h + 0 + j + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_x				(0 + 0 + 0 + 0 + 0 + 0 + 0 + h + 0 + j + 0 + l + 0 + n + 0)
#define	RB_SEG14_y				(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + j + 0 + l + 0 + n + 0)
#define	RB_SEG14_z				(0 + 0 + 0 + d + 0 + 0 + 0 + 0 + 0 + j + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_braceleft		(0 + 0 + 0 + 0 + 0 + 0 + 0 + h + 0 + 0 + k + 0 + 0 + n + 0)
#define	RB_SEG14_bar			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + m + 0 + 0)
#define	RB_SEG14_braceright		(0 + 0 + 0 + 0 + 0 + 0 + g + 0 + 0 + j + 0 + l + 0 + 0 + 0)
#define	RB_SEG14_tilde			(0 + 0 + 0 + 0 + 0 + f + 0 + 0 + 0 + 0 + 0 + l + 0 + n + 0)
#define	RB_SEG14_delete			(a + b + c + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)

#ifdef SUPPORT_128_TO_255_MT_SICS

#define	RB_SEG14_128			(a + 0 + 0 + d + e + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_129			(a + 0 + 0 + 0 + 0 + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_130			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + j + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_131			(0 + 0 + 0 + 0 + 0 + 0 + g + 0 + i + 0 + k + 0 + 0 + n + 0)
#define	RB_SEG14_132			(0 + 0 + c + 0 + e + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_133			(0 + 0 + 0 + d + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_134			(0 + 0 + 0 + 0 + 0 + 0 + g + 0 + i + 0 + k + 0 + m + 0 + 0)
#define	RB_SEG14_135			(a + 0 + 0 + d + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + m + 0 + 0)
#define	RB_SEG14_136			(0 + 0 + 0 + 0 + 0 + 0 + 0 + h + 0 + j + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_137			(0 + 0 + c + 0 + 0 + f + 0 + 0 + i + j + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_138			(a + 0 + c + d + 0 + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_139			(0 + 0 + 0 + 0 + 0 + 0 + 0 + h + 0 + 0 + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_140			(a + 0 + 0 + d + e + f + g + 0 + i + 0 + 0 + 0 + m + 0 + 0)
#define	RB_SEG14_141			(0 + 0 + 0 + d + e + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_142			(0 + 0 + 0 + d + 0 + 0 + 0 + 0 + 0 + j + k + l + 0 + n + 0)
#define	RB_SEG14_143			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + j + 0 + l + 0 + n + 0)

#define	RB_SEG14_144			(a + 0 + 0 + d + e + 0 + 0 + h + 0 + 0 + k + l + 0 + 0 + 0)
#define	RB_SEG14_145			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + l + 0 + 0 + 0)
#define	RB_SEG14_146			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_147			(0 + b + 0 + 0 + 0 + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_148			(0 + b + 0 + 0 + 0 + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_149			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + dp)
#define	RB_SEG14_150			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_151			(0 + 0 + 0 + 0 + 0 + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_152			(0 + 0 + 0 + 0 + 0 + f + 0 + 0 + 0 + 0 + 0 + l + 0 + n + 0)
#define	RB_SEG14_153			(a + b + c + d + e + f + g + h + i + j + k + l + m + n + 0)
#define	RB_SEG14_154			(0 + 0 + 0 + d + 0 + 0 + g + h + 0 + 0 + 0 + l + 0 + n + 0)
#define	RB_SEG14_155			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + j + 0 + l + 0 + 0 + 0)
#define	RB_SEG14_156			(a + b + c + d + e + f + g + h + i + j + k + l + m + n + 0)
#define	RB_SEG14_157			(a + b + c + d + e + f + 0 + 0 + 0 + j + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_158			(0 + 0 + 0 + d + 0 + 0 + 0 + 0 + 0 + j + k + l + 0 + n + 0)
#define	RB_SEG14_159			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + i + 0 + 0 + l + 0 + n + 0)

#define	RB_SEG14_160			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_161			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_162			(0 + 0 + 0 + d + e + 0 + g + 0 + 0 + j + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_163			(0 + 0 + 0 + d + 0 + 0 + g + 0 + 0 + j + k + 0 + 0 + n + 0)
#define	RB_SEG14_164			(a + b + c + d + e + f + 0 + h + 0 + j + 0 + l + 0 + n + 0)
#define	RB_SEG14_165			(0 + 0 + 0 + 0 + 0 + 0 + g + 0 + i + 0 + k + l + 0 + n + 0)
#define	RB_SEG14_166			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + m + n + 0)
#define	RB_SEG14_167			(a + b + c + d + e + f + g + h + i + j + k + l + m + n + 0)
#define	RB_SEG14_168			(0 + 0 + 0 + 0 + 0 + f + 0 + 0 + 0 + 0 + 0 + 0 + m + 0 + 0)
#define	RB_SEG14_169			(a + b + c + d + e + f + g + h + i + j + k + l + m + n + 0)
#define	RB_SEG14_170			(0 + 0 + 0 + d + 0 + 0 + 0 + h + 0 + j + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_171			(0 + 0 + 0 + 0 + 0 + 0 + 0 + h + 0 + 0 + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_172			(a + b + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_173			(a + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_174			(a + b + c + d + e + f + g + h + i + j + k + l + m + n + 0)
#define	RB_SEG14_175			(a + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)

#define	RB_SEG14_176			(a + b + 0 + 0 + 0 + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_177			(0 + 0 + 0 + d + 0 + 0 + g + 0 + i + 0 + k + 0 + m + 0 + 0)
#define	RB_SEG14_178			(a + b + 0 + d + e + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_179			(a + b + c + d + 0 + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_180			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_181			(0 + b + 0 + 0 + e + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_182			(a + b + c + 0 + 0 + f + g + 0 + i + 0 + k + 0 + m + 0 + 0)
#define	RB_SEG14_183			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + dp)
#define	RB_SEG14_184			(0 + 0 + c + d + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_185			(0 + b + c + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_186			(a + b + 0 + 0 + 0 + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_187			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + j + 0 + l + 0 + 0 + 0)
#define	RB_SEG14_188			(a + b + c + d + e + f + g + h + i + j + k + l + m + n + 0)
#define	RB_SEG14_189			(a + b + c + d + e + f + g + h + i + j + k + l + m + n + 0)
#define	RB_SEG14_190			(a + b + c + d + e + f + g + h + i + j + k + l + m + n + 0)
#define	RB_SEG14_191			(0 + 0 + 0 + d + e + 0 + 0 + 0 + 0 + 0 + k + 0 + m + 0 + 0)

#define	RB_SEG14_192			(a + b + c + 0 + e + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_193			(a + b + c + 0 + e + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_194			(a + b + c + 0 + e + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_195			(a + b + c + 0 + e + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_196			(a + b + c + 0 + e + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_197			(a + b + c + 0 + e + f + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_198			(a + 0 + 0 + d + e + f + g + 0 + i + 0 + k + 0 + m + 0 + 0)
#define	RB_SEG14_199			(a + 0 + 0 + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_200			(a + 0 + 0 + d + e + f + 0 + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_201			(a + 0 + 0 + d + e + f + 0 + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_202			(a + 0 + 0 + d + e + f + 0 + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_203			(a + 0 + 0 + d + e + f + 0 + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_204			(a + 0 + 0 + d + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + m + 0 + 0)
#define	RB_SEG14_205			(a + 0 + 0 + d + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + m + 0 + 0)
#define	RB_SEG14_206			(a + 0 + 0 + d + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + m + 0 + 0)
#define	RB_SEG14_207			(a + 0 + 0 + d + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + m + 0 + 0)

#define	RB_SEG14_208			(a + b + c + d + 0 + 0 + 0 + 0 + i + 0 + k + 0 + m + 0 + 0)
#define	RB_SEG14_209			(0 + b + c + 0 + e + f + 0 + h + 0 + 0 + 0 + l + 0 + 0 + 0)
#define	RB_SEG14_210			(a + b + c + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_211			(a + b + c + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_212			(a + b + c + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_213			(a + b + c + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_214			(a + b + c + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_215			(0 + 0 + 0 + 0 + 0 + 0 + 0 + h + 0 + j + 0 + l + 0 + n + 0)
#define	RB_SEG14_216			(a + b + c + d + e + f + 0 + 0 + 0 + j + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_217			(0 + b + c + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_218			(0 + b + c + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_219			(0 + b + c + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_220			(0 + b + c + d + e + f + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_221			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + i + 0 + 0 + l + 0 + n + 0)
#define	RB_SEG14_222			(0 + 0 + 0 + 0 + e + f + 0 + 0 + 0 + j + 0 + l + 0 + 0 + 0)
#define	RB_SEG14_223			(a + 0 + 0 + d + e + f + 0 + h + 0 + 0 + 0 + 0 + 0 + n + 0)

#define	RB_SEG14_224			(0 + 0 + 0 + d + 0 + 0 + 0 + h + 0 + j + k + l + 0 + 0 + 0)
#define	RB_SEG14_225			(0 + 0 + 0 + d + 0 + 0 + 0 + h + 0 + j + k + 0 + 0 + n + 0)
#define	RB_SEG14_226			(a + 0 + 0 + d + 0 + 0 + 0 + h + 0 + j + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_227			(a + 0 + 0 + d + 0 + 0 + 0 + h + 0 + j + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_228			(0 + 0 + 0 + d + 0 + 0 + 0 + h + 0 + j + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_229			(0 + 0 + 0 + d + 0 + 0 + 0 + h + 0 + j + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_230			(0 + 0 + 0 + d + e + 0 + 0 + h + 0 + j + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_231			(0 + 0 + 0 + d + e + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_232			(0 + 0 + 0 + d + e + 0 + 0 + 0 + 0 + j + k + l + 0 + 0 + 0)
#define	RB_SEG14_233			(0 + 0 + 0 + d + e + 0 + 0 + 0 + 0 + j + k + 0 + 0 + n + 0)
#define	RB_SEG14_234			(a + 0 + 0 + d + e + 0 + 0 + 0 + 0 + j + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_235			(0 + 0 + 0 + d + e + 0 + 0 + 0 + 0 + j + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_236			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + i + 0 + 0 + l + 0 + 0 + 0)
#define	RB_SEG14_237			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_238			(a + 0 + 0 + 0 + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_239			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + i + 0 + 0 + 0 + 0 + 0 + 0)

#define	RB_SEG14_240			(0 + 0 + 0 + d + e + 0 + 0 + h + 0 + 0 + k + l + 0 + 0 + 0)
#define	RB_SEG14_241			(a + 0 + c + 0 + e + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_242			(0 + 0 + c + d + e + 0 + g + 0 + 0 + 0 + k + l + 0 + 0 + 0)
#define	RB_SEG14_243			(0 + 0 + c + d + e + 0 + g + 0 + 0 + 0 + k + 0 + 0 + n + 0)
#define	RB_SEG14_244			(a + 0 + c + d + e + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_245			(a + 0 + c + d + e + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_246			(0 + 0 + c + d + e + 0 + g + 0 + 0 + 0 + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_247			(a + b + c + d + e + f + g + h + i + j + k + l + m + n + 0)
#define	RB_SEG14_248			(0 + 0 + c + d + e + 0 + g + 0 + 0 + j + k + 0 + 0 + 0 + 0)
#define	RB_SEG14_249			(0 + 0 + c + d + e + 0 + 0 + 0 + 0 + 0 + 0 + l + 0 + 0 + 0)
#define	RB_SEG14_250			(0 + 0 + c + d + e + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + n + 0)
#define	RB_SEG14_251			(a + 0 + c + d + e + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_252			(0 + 0 + c + d + e + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0)
#define	RB_SEG14_253			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + j + 0 + l + 0 + n + 0)
#define	RB_SEG14_254			(0 + 0 + 0 + 0 + e + f + 0 + 0 + 0 + j + 0 + l + 0 + 0 + 0)
#define	RB_SEG14_255			(0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + 0 + j + 0 + l + 0 + n + 0)

#endif // SUPPORT_128_TO_255_MT_SICS


//==================================================================================================
//	L O	C A	L	V A	R I	A B	L E	S
//==================================================================================================

//lint -e941 //Note: Result 0 due to operand(s) equaling 0 in operation '+'

//!	14 Segment Table
static const uint16_t S_CodeTable_32to127[] = {

	RB_SEG14_space,
	RB_SEG14_exclam,
	RB_SEG14_quotedbl,
	RB_SEG14_numbersign,
	RB_SEG14_dollar,
	RB_SEG14_percent,
	RB_SEG14_ampersand,
	RB_SEG14_quote,
	RB_SEG14_parenleft,
	RB_SEG14_parenright,
	RB_SEG14_asterisk,
	RB_SEG14_plus,
	RB_SEG14_comma,
	RB_SEG14_hyphen,
	RB_SEG14_period,
	RB_SEG14_slash,

	RB_SEG14_0,
	RB_SEG14_1,
	RB_SEG14_2,
	RB_SEG14_3,
	RB_SEG14_4,
	RB_SEG14_5,
	RB_SEG14_6,
	RB_SEG14_7,
	RB_SEG14_8,
	RB_SEG14_9,
	RB_SEG14_colon,
	RB_SEG14_semicolon,
	RB_SEG14_less,
	RB_SEG14_equal,
	RB_SEG14_greater,
	RB_SEG14_question,

	RB_SEG14_at,
	RB_SEG14_A,
	RB_SEG14_B,
	RB_SEG14_C,
	RB_SEG14_D,
	RB_SEG14_E,
	RB_SEG14_F,
	RB_SEG14_G,
	RB_SEG14_H,
	RB_SEG14_I,
	RB_SEG14_J,
	RB_SEG14_K,
	RB_SEG14_L,
	RB_SEG14_M,
	RB_SEG14_N,
	RB_SEG14_O,

	RB_SEG14_P,
	RB_SEG14_Q,
	RB_SEG14_R,
	RB_SEG14_S,
	RB_SEG14_T,
	RB_SEG14_U,
	RB_SEG14_V,
	RB_SEG14_W,
	RB_SEG14_X,
	RB_SEG14_Y,
	RB_SEG14_Z,
	RB_SEG14_bracketlef,
	RB_SEG14_backslash,
	RB_SEG14_bracketrig,
	RB_SEG14_circumflex,
	RB_SEG14_underscore,

	RB_SEG14_quoteleft,
	RB_SEG14_a,
	RB_SEG14_b,
	RB_SEG14_c,
	RB_SEG14_d,
	RB_SEG14_e,
	RB_SEG14_f,
	RB_SEG14_g,
	RB_SEG14_h,
	RB_SEG14_i,
	RB_SEG14_j,
	RB_SEG14_k,
	RB_SEG14_l,
	RB_SEG14_m,
	RB_SEG14_n,
	RB_SEG14_o,

	RB_SEG14_p,
	RB_SEG14_q,
	RB_SEG14_r,
	RB_SEG14_s,
	RB_SEG14_t,
	RB_SEG14_u,
	RB_SEG14_v,
	RB_SEG14_w,
	RB_SEG14_x,
	RB_SEG14_y,
	RB_SEG14_z,
	RB_SEG14_braceleft,
	RB_SEG14_bar,
	RB_SEG14_braceright,
	RB_SEG14_tilde,
	RB_SEG14_delete
};

#ifdef SUPPORT_128_TO_255_MT_SICS

static const uint16_t S_CodeTable_128to255[] = {

	RB_SEG14_128,
	RB_SEG14_129,
	RB_SEG14_130,
	RB_SEG14_131,
	RB_SEG14_132,
	RB_SEG14_133,
	RB_SEG14_134,
	RB_SEG14_135,
	RB_SEG14_136,
	RB_SEG14_137,
	RB_SEG14_138,
	RB_SEG14_139,
	RB_SEG14_140,
	RB_SEG14_141,
	RB_SEG14_142,
	RB_SEG14_143,

	RB_SEG14_144,
	RB_SEG14_145,
	RB_SEG14_146,
	RB_SEG14_147,
	RB_SEG14_148,
	RB_SEG14_149,
	RB_SEG14_150,
	RB_SEG14_151,
	RB_SEG14_152,
	RB_SEG14_153,
	RB_SEG14_154,
	RB_SEG14_155,
	RB_SEG14_156,
	RB_SEG14_157,
	RB_SEG14_158,
	RB_SEG14_159,

	RB_SEG14_160,
	RB_SEG14_161,
	RB_SEG14_162,
	RB_SEG14_163,
	RB_SEG14_164,
	RB_SEG14_165,
	RB_SEG14_166,
	RB_SEG14_167,
	RB_SEG14_168,
	RB_SEG14_169,
	RB_SEG14_170,
	RB_SEG14_171,
	RB_SEG14_172,
	RB_SEG14_173,
	RB_SEG14_174,
	RB_SEG14_175,

	RB_SEG14_176,
	RB_SEG14_177,
	RB_SEG14_178,
	RB_SEG14_179,
	RB_SEG14_180,
	RB_SEG14_181,
	RB_SEG14_182,
	RB_SEG14_183,
	RB_SEG14_184,
	RB_SEG14_185,
	RB_SEG14_186,
	RB_SEG14_187,
	RB_SEG14_188,
	RB_SEG14_189,
	RB_SEG14_190,
	RB_SEG14_191,

	RB_SEG14_192,
	RB_SEG14_193,
	RB_SEG14_194,
	RB_SEG14_195,
	RB_SEG14_196,
	RB_SEG14_197,
	RB_SEG14_198,
	RB_SEG14_199,
	RB_SEG14_200,
	RB_SEG14_201,
	RB_SEG14_202,
	RB_SEG14_203,
	RB_SEG14_204,
	RB_SEG14_205,
	RB_SEG14_206,
	RB_SEG14_207,

	RB_SEG14_208,
	RB_SEG14_209,
	RB_SEG14_210,
	RB_SEG14_211,
	RB_SEG14_212,
	RB_SEG14_213,
	RB_SEG14_214,
	RB_SEG14_215,
	RB_SEG14_216,
	RB_SEG14_217,
	RB_SEG14_218,
	RB_SEG14_219,
	RB_SEG14_220,
	RB_SEG14_221,
	RB_SEG14_222,
	RB_SEG14_223,

	RB_SEG14_224,
	RB_SEG14_225,
	RB_SEG14_226,
	RB_SEG14_227,
	RB_SEG14_228,
	RB_SEG14_229,
	RB_SEG14_230,
	RB_SEG14_231,
	RB_SEG14_232,
	RB_SEG14_233,
	RB_SEG14_234,
	RB_SEG14_235,
	RB_SEG14_236,
	RB_SEG14_237,
	RB_SEG14_238,
	RB_SEG14_239,

	RB_SEG14_240,
	RB_SEG14_241,
	RB_SEG14_242,
	RB_SEG14_243,
	RB_SEG14_244,
	RB_SEG14_245,
	RB_SEG14_246,
	RB_SEG14_247,
	RB_SEG14_248,
	RB_SEG14_249,
	RB_SEG14_250,
	RB_SEG14_251,
	RB_SEG14_252,
	RB_SEG14_253,
	RB_SEG14_254,
	RB_SEG14_255
};

#endif // SUPPORT_128_TO_255_MT_SICS

//lint +e941


//==================================================================================================
//	G L	O B	A L	  F	U N	C T	I O	N	I M	P L	E M	E N	T A	T I	O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_SEG14_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize module
//!
//--------------------------------------------------------------------------------------------------
void RB_SEG14_Initialize(void)
	{
	}


//--------------------------------------------------------------------------------------------------
// RB_SEG14_GetCode
//--------------------------------------------------------------------------------------------------
//! \brief	Return 14-segment code for an ASCII	character, range 0x20..0x7F/0xFF, otherwise return 0.
//!
//! \param	ascii	input	ASCII character
//! \return	14-segment code
//--------------------------------------------------------------------------------------------------
uint16_t RB_SEG14_GetCode(char ascii)
{
	uint8_t index = (uint8_t)ascii;

	if ((index >= 0x20) && (index <= 0x7F)) {           // 0x20..0x7F |  32..127
		return (S_CodeTable_32to127[index - 0x20]);
	}
#ifdef SUPPORT_128_TO_255_MT_SICS
	else if (index >= 0x80) {      // 0x80..0xFF | 128..255
		return (S_CodeTable_128to255[index - 0x80]);
	}
#endif
	else {
		return (0);
	}
}


#endif // RB_CONFIG_USE_DISPLAY && RB_CONFIG_USE_SEG14
