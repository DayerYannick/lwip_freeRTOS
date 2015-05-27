/**
 * This file has a different license to the rest of the GFX system.
 * You can copy, modify and distribute this file as you see fit.
 * You do not need to publish your source modifications to this file.
 * The only thing you are not permitted to do is to relicense it
 * under a different license.
 */

/**
 * Copy this file into your project directory and rename it as gfxconf.h
 * Edit your copy to turn on the GFX features you want to use.
 */

#ifndef _GFXCONF_H
#define _GFXCONF_H

#include "heivs/config.h"

/* The operating system to use. One of these must be defined - preferably in your Makefile */
//#define GFX_USE_OS_CHIBIOS	FALSE
//#define GFX_USE_OS_WIN32		FALSE
//#define GFX_USE_OS_LINUX		FALSE
//#define GFX_USE_OS_OSX		FALSE
//#define GFX_USE_OS_FREERTOS		TRUE

#if USE_FREERTOS
	#define GFX_USE_OS_FREERTOS		TRUE
#else
	#define GFX_USE_OS_NONE			TRUE
#endif

#define GDISP_NEED_STARTUP_LOGO FALSE

/* GFX subsystems to turn on */
#define GFX_USE_GDISP			TRUE
#define GFX_USE_TDISP			FALSE
#define GFX_USE_GWIN			TRUE
#define GFX_USE_GEVENT			TRUE
#define GFX_USE_GTIMER			TRUE
#define GFX_USE_GQUEUE			TRUE
#define GFX_USE_GINPUT			TRUE
#define GFX_USE_GADC			FALSE
#define GFX_USE_GAUDIN			FALSE
#define GFX_USE_GAUDOUT			FALSE
#define GFX_USE_GMISC			FALSE

/* Features for the GDISP subsystem */
#define GDISP_NEED_AUTOFLUSH		FALSE
#define GDISP_NEED_TIMERFLUSH		FALSE
#define GDISP_NEED_VALIDATION		FALSE
#define GDISP_NEED_CLIP				TRUE
#define GDISP_NEED_TEXT				TRUE
#define GDISP_NEED_CIRCLE			TRUE
#define GDISP_NEED_ELLIPSE			FALSE
#define GDISP_NEED_ARC				FALSE
#define GDISP_NEED_CONVEX_POLYGON	FALSE
#define GDISP_NEED_SCROLL			TRUE
#define GDISP_NEED_PIXELREAD		TRUE
#define GDISP_NEED_CONTROL			TRUE
#define GDISP_NEED_QUERY			FALSE
#define GDISP_NEED_IMAGE			TRUE
#define GDISP_NEED_MULTITHREAD		TRUE
#define GDISP_NEED_STREAMING		FALSE

/* GDISP - text features */
#define GDISP_NEED_ANTIALIAS		TRUE
#define GDISP_NEED_UTF8				FALSE
#define GDISP_NEED_TEXT_KERNING		FALSE

/* GDISP - fonts to include */
#define GDISP_INCLUDE_FONT_UI1					FALSE
#define GDISP_INCLUDE_FONT_UI2					FALSE
#define GDISP_INCLUDE_FONT_LARGENUMBERS			FALSE
#define GDISP_INCLUDE_FONT_DEJAVUSANS10			TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS12			TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS16			TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS24			FALSE
#define GDISP_INCLUDE_FONT_DEJAVUSANS32			TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANSBOLD12		FALSE
#define GDISP_INCLUDE_FONT_FIXED_10x20			FALSE
#define GDISP_INCLUDE_FONT_FIXED_7x14			FALSE
#define GDISP_INCLUDE_FONT_FIXED_5x8			FALSE
#define GDISP_INCLUDE_FONT_DEJAVUSANS12_AA		TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS16_AA		TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANS24_AA		FALSE
#define GDISP_INCLUDE_FONT_DEJAVUSANS32_AA		TRUE
#define GDISP_INCLUDE_FONT_DEJAVUSANSBOLD12_AA	FALSE
#define GDISP_INCLUDE_USER_FONTS				FALSE

/* GDISP image decoders */
#define GDISP_NEED_IMAGE_NATIVE		TRUE
#define GDISP_NEED_IMAGE_GIF		TRUE
#define GDISP_NEED_IMAGE_BMP		TRUE
#define GDISP_NEED_IMAGE_JPG		FALSE
#define GDISP_NEED_IMAGE_PNG		FALSE
#define GDISP_NEED_IMAGE_ACCOUNTING	FALSE

/* Optional image support that can be turned off */
/*
	#define GDISP_NEED_IMAGE_BMP_1		TRUE
	#define GDISP_NEED_IMAGE_BMP_4		TRUE
	#define GDISP_NEED_IMAGE_BMP_4_RLE	TRUE
	#define GDISP_NEED_IMAGE_BMP_8		TRUE
	#define GDISP_NEED_IMAGE_BMP_8_RLE	TRUE
	#define GDISP_NEED_IMAGE_BMP_16		TRUE
	#define GDISP_NEED_IMAGE_BMP_24		TRUE
	#define GDISP_NEED_IMAGE_BMP_32		TRUE
*/

/* Features for the TDISP subsystem. */
#define TDISP_NEED_MULTITHREAD	FALSE

/* Features for the GWIN subsystem. */
#define GWIN_NEED_WINDOWMANAGER		TRUE
#define GWIN_NEED_CONSOLE			TRUE
#define GWIN_CONSOLE_ESCSEQ			TRUE
#define GWIN_CONSOLE_USE_HISTORY 	TRUE
#define GWIN_CONSOLE_HISTORY_ATCREATE TRUE
#define GWIN_NEED_GRAPH				TRUE
#define GWIN_NEED_WIDGET			TRUE
#define GWIN_NEED_LABEL				TRUE
#define GWIN_NEED_BUTTON			TRUE
#define GWIN_NEED_SLIDER			TRUE
#define GWIN_NEED_CHECKBOX			TRUE
#define GWIN_NEED_IMAGE				TRUE
#define GWIN_NEED_RADIO				TRUE
#define GWIN_NEED_LIST				FALSE

/* Features for the GEVENT subsystem. */
#define GEVENT_ASSERT_NO_RESOURCE	FALSE

/* Features for the GTIMER subsystem. */
/* NONE */

/* Features for the GQUEUE subsystem. */
#define GQUEUE_NEED_ASYNC		TRUE
#define GQUEUE_NEED_GSYNC		FALSE
#define GQUEUE_NEED_FSYNC		FALSE

/* Features for the GINPUT subsystem. */
#define GINPUT_NEED_MOUSE		TRUE
#define GINPUT_NEED_KEYBOARD	FALSE
#define GINPUT_NEED_TOGGLE		FALSE
#define GINPUT_NEED_DIAL		FALSE

/* Features for the GADC subsystem. */
/* NONE */

/* Features for the GAUDIN subsystem. */
/* NONE */

/* Features for the GAUDOUT subsystem. */
/* NONE */

/* Features for the GMISC subsystem. */
#define GMISC_NEED_ARRAYOPS		FALSE
#define GMISC_NEED_FASTTRIG		FALSE
#define GMISC_NEED_FIXEDTRIG	FALSE
#define GMISC_NEED_INVSQRT		FALSE

/* Optional Multiple Display support */
/*
	#define GDISP_TOTAL_DISPLAYS			1
	#define GDISP_TOTAL_CONTROLLERS			1

	// Extra stuff required when GDISP_TOTAL_CONTROLLERS > 1
	#define GDISP_CONTROLLER_LIST			GDISPVMT_Win32, GDISPVMT_Win32
	#define GDISP_CONTROLLER_DISPLAYS		1, 1
	#define GDISP_PIXELFORMAT				GDISP_PIXELFORMAT_RGB888

	// Optional extra stuff when GDISP_TOTAL_CONTROLLERS > 1
	#define GDISP_HARDWARE_STREAM_WRITE		FALSE
	#define GDISP_HARDWARE_STREAM_READ		FALSE
	#define GDISP_HARDWARE_STREAM_POS		FALSE
	#define GDISP_HARDWARE_DRAWPIXEL		FALSE
	#define GDISP_HARDWARE_CLEARS			FALSE
	#define GDISP_HARDWARE_FILLS			FALSE
	#define GDISP_HARDWARE_BITFILLS			FALSE
	#define GDISP_HARDWARE_SCROLL			FALSE
	#define GDISP_HARDWARE_PIXELREAD		FALSE
	#define GDISP_HARDWARE_CONTROL			FALSE
	#define GDISP_HARDWARE_QUERY			FALSE
	#define GDISP_HARDWARE_CLIP				FALSE
*/

/* Optional Parameters for various subsystems */
/*
	#define GDISP_DEFAULT_ORIENTATION		GDISP_ROTATE_LANDSCAPE
	#define GDISP_LINEBUF_SIZE				128
	#define GEVENT_MAXIMUM_SIZE				32
	#define GEVENT_MAX_SOURCE_LISTENERS		32
	#define GTIMER_THREAD_PRIORITY			HIGH_PRIORITY
	#define GTIMER_THREAD_WORKAREA_SIZE		1024
	#define GADC_MAX_LOWSPEED_DEVICES		4
	#define GWIN_BUTTON_LAZY_RELEASE		FALSE
	#define GWIN_CONSOLE_USE_BASESTREAM		FALSE
	#define GWIN_CONSOLE_USE_FLOAT			FALSE
	#define GMISC_INVSQRT_MIXED_ENDIAN		FALSE
	#define GMISC_INVSQRT_REAL_SLOW			FALSE
*/
//#define GWIN_NEED_IMAGE_ANIMATION		TRUE

/* Optional Low Level Driver Definitions */
/*
	#define GDISP_SCREEN_WIDTH			320
	#define GDISP_SCREEN_HEIGHT			240
	#define TDISP_COLUMNS				16
	#define TDISP_ROWS					2
*/

#endif /* _GFXCONF_H */

