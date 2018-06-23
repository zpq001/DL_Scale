//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_GraphicBuffer.c
//! \ingroup	util
//! \brief		Utility module to allocate framebuffers
//!
//! This module is used to manage the framebuffers needed by the different GUI libraries and
//! graphic controllers.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Silvan Sturzenegger
//
// $Date: 2016/11/10 08:58:39MEZ $
// $Revision: 1.15 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_GraphicBuffer"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_GraphicBuffer.h"

#if defined(RB_CONFIG_USE_GRAPHICCONTROLLER) && (RB_CONFIG_USE_GRAPHICCONTROLLER == RB_CONFIG_YES)

#if defined(RB_CONFIG_GRAPHICS_USE_EASYGUI) && (RB_CONFIG_GRAPHICS_USE_EASYGUI == RB_CONFIG_YES)
#include "GuiDisplay.h"
#elif defined(RB_CONFIG_GRAPHICS_USE_UCGUI) && (RB_CONFIG_GRAPHICS_USE_UCGUI == RB_CONFIG_YES)
#include "LCD.h"
#include "LCD_SIM.h"
#endif


//==================================================================================================
//  L O C A L   D E F I N I T I O N S
//==================================================================================================

#if defined(RB_CONFIG_GRAPHICS_USE_TOUCHGFX) && (RB_CONFIG_GRAPHICS_USE_TOUCHGFX == RB_CONFIG_YES)
	#define SUPPORT_TOUCHGFX
#elif defined(RB_CONFIG_GRAPHICS_USE_EASYGUI) && (RB_CONFIG_GRAPHICS_USE_EASYGUI == RB_CONFIG_YES)
	#define SUPPORT_EASYGUI
#elif defined(RB_CONFIG_GRAPHICS_USE_UCGUI) && (RB_CONFIG_GRAPHICS_USE_UCGUI == RB_CONFIG_YES)
	#define SUPPORT_UCGUI
#elif defined(RB_CONFIG_GRAPHICS_USE_NOGUI) && (RB_CONFIG_GRAPHICS_USE_NOGUI == RB_CONFIG_YES)
	#define SUPPORT_NOGUI
#endif

// Use default values in case these values are not defined. This makes it possible to compile
// RB_GraphicBuffer without using the Graphics package.
#if !defined(RB_CONFIG_GRAPHICS_BUFFER_WIDTH)
	#define RB_CONFIG_GRAPHICS_BUFFER_WIDTH		240
#endif

#if !defined(RB_CONFIG_GRAPHICS_BUFFER_HEIGHT)
	#define RB_CONFIG_GRAPHICS_BUFFER_HEIGHT	320
#endif

#if !defined(RB_CONFIG_GRAPHICS_DISPLAY_HEIGHT)
	#define RB_CONFIG_GRAPHICS_DISPLAY_HEIGHT	320
#endif

#if !defined(RB_CONFIG_GRAPHICS_BITS_PER_PIXEL)
	#define RB_CONFIG_GRAPHICS_BITS_PER_PIXEL	16
#endif

#define NUM_OF_BUFFER_PIXELS	(RB_CONFIG_GRAPHICS_BUFFER_WIDTH * \
								 RB_CONFIG_GRAPHICS_BUFFER_HEIGHT)

#define NUM_OF_PADDING_PIXELS	(RB_CONFIG_GRAPHICS_BUFFER_WIDTH * \
								 (RB_CONFIG_GRAPHICS_DISPLAY_HEIGHT + \
								  RB_CONFIG_GRAPHICS_BUFFER_HEIGHT))

// The number of pixels that fit in a 32 bit block. 24 bit pixels are handled specially.
#define PIXELS_PER_BLOCK		(32 / RB_CONFIG_GRAPHICS_BITS_PER_PIXEL)

// Calculate the required buffer size in uint32_t blocks.
// Make sure the buffer is large enough when the number of pixels is not divisible by 32 bits by
// adding PIXELS_PER_BLOCK - 1 before dividing.
#define NUM_OF_BUFFER_BLOCKS	((NUM_OF_BUFFER_PIXELS + (PIXELS_PER_BLOCK - 1)) / PIXELS_PER_BLOCK)

#define NUM_OF_PADDING_BLOCKS	((NUM_OF_PADDING_PIXELS + (PIXELS_PER_BLOCK - 1)) / PIXELS_PER_BLOCK)

#if defined(RB_CONFIG_GRAPHICS_BITS_PER_PIXEL) && \
	(RB_CONFIG_GRAPHICS_BITS_PER_PIXEL == 1  || \
	 RB_CONFIG_GRAPHICS_BITS_PER_PIXEL == 2  || \
	 RB_CONFIG_GRAPHICS_BITS_PER_PIXEL == 4  || \
	 RB_CONFIG_GRAPHICS_BITS_PER_PIXEL == 8  || \
	 RB_CONFIG_GRAPHICS_BITS_PER_PIXEL == 16)
	#define BUFFER_SIZE		(NUM_OF_BUFFER_BLOCKS)
	#define PADDING_SIZE	(NUM_OF_PADDING_BLOCKS)
#elif defined(RB_CONFIG_GRAPHICS_BITS_PER_PIXEL) && (RB_CONFIG_GRAPHICS_BITS_PER_PIXEL == 24)
	// 24 bit pixel data uses 32 bits per pixel in the framebuffer (with 8 unused bits)
	#define BUFFER_SIZE		(NUM_OF_BUFFER_PIXELS)
	#define PADDING_SIZE	(NUM_OF_PADDING_PIXELS)
#else
	#error Unsupported pixel format (RB_CONFIG_GRAPHICS_BITS_PER_PIXEL)
#endif


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

// Define bitmap buffer for maximum size of display
#if defined(RB_ENV_BSP_WIN32)
	#if defined(SUPPORT_TOUCHGFX)
		// TouchGFX uses three buffers, two for double buffering and one for animations
		static RB_DECL_BUF(uint32_t, displayBuffer, 3*BUFFER_SIZE+PADDING_SIZE, LCD_BUFFER);
	#elif defined(SUPPORT_EASYGUI)
		// EasyGUI requires this name and datatype (not static, declared as extern in GuiLib.h)
		RB_DECL_VAR(DisplayBufUnion, GuiLib_DisplayBuf, LCD_BUFFER);
	#elif defined(SUPPORT_UCGUI)
		// uC-GUI allocates its own buffer in Win32
	#else
		// SUPPORT_NOGUI and everything else
		static RB_DECL_BUF(uint32_t, displayBuffer, BUFFER_SIZE, LCD_BUFFER);
	#endif
#elif defined(RB_ENV_BSP_LPC3000)
	// On LPC3000 MCUs the framebuffer has to be placed in a non-cached memory region
	#if defined(SUPPORT_TOUCHGFX)
		#pragma data_alignment=8
		// TouchGFX uses three buffers, two for double buffering and one for animations.
		static RB_DECL_BUF(uint32_t, displayBuffer, 3*BUFFER_SIZE+PADDING_SIZE, NonCachedBufferSpace);
	#elif defined(SUPPORT_EASYGUI)
		#pragma data_alignment=8
		// EasyGUI requires this name and datatype (not static, declared as extern in GuiLib.h)
		RB_DECL_VAR(DisplayBufUnion, GuiLib_DisplayBuf, NonCachedBufferSpace);
	#else // SUPPORT_UCGUI, SUPPORT_NOGUI and everything else
		#pragma data_alignment=8
		static RB_DECL_BUF(uint32_t, displayBuffer, BUFFER_SIZE, NonCachedBufferSpace);
	#endif
#else
	#if defined(SUPPORT_TOUCHGFX)
		#pragma data_alignment=8
		// TouchGFX uses three buffers, two for double buffering and one for animations.
		static RB_DECL_BUF(uint32_t, displayBuffer, 3*BUFFER_SIZE+PADDING_SIZE, LCD_BUFFER);
	#elif defined(SUPPORT_EASYGUI)
		#pragma data_alignment=8
		// EasyGUI requires this name and datatype (not static, declared as extern in GuiLib.h)
		RB_DECL_VAR(DisplayBufUnion, GuiLib_DisplayBuf, LCD_BUFFER);
	#else // SUPPORT_UCGUI, SUPPORT_NOGUI and everything else
		#pragma data_alignment=8
		static RB_DECL_BUF(uint32_t, displayBuffer, BUFFER_SIZE, LCD_BUFFER);
	#endif
#endif


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_GRAPHICBUFFER_GetBuffer
//--------------------------------------------------------------------------------------------------
//! \brief	Get a pointer to the start of the frame buffer.
//!
//! \return	pointer to the frame buffer
//!			(void pointer because the type is dependent on the GUI library)
//--------------------------------------------------------------------------------------------------
void* RB_GRAPHICBUFFER_GetBuffer(void)
{
	#ifdef RB_ENV_BSP_WIN32
		#if defined(SUPPORT_EASYGUI)
		return &GuiLib_DisplayBuf;
		#elif defined(SUPPORT_UCGUI)
		return LCDSIM_GetRGBVideoBuffer();
		#else
		return displayBuffer;
		#endif
	#else
		#if defined(SUPPORT_EASYGUI)
		return &GuiLib_DisplayBuf;
		#else
		return displayBuffer;
		#endif
	#endif
}

#endif // RB_CONFIG_USE_GRAPHICCONTROLLER

