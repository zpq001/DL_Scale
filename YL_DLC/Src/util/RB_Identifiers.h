//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Identifiers.h
//! \ingroup	util
//! \brief		Global identifiers in Rainbow.
//!
//! Here, global identifiers such as message sources are defined.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Andri Toggenburger
//
// $Date: 2017/04/28 09:58:13MESZ $
// $Revision: 1.77 $
//
//==================================================================================================

#ifndef _RB_Identifiers__h
#define _RB_Identifiers__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

//#include "RB_Config.h"

#include "RB_Typedefs.h"


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//! Source of a message
typedef enum {
	RB_IDENTIFIERS_SRC_UNDEFINED						= 0,		// Must be zero!

	// Logical channels
	RB_IDENTIFIERS_SRC_HOST								= 1,
	RB_IDENTIFIERS_SRC_PRINTER							= 2,
	RB_IDENTIFIERS_SRC_FEEDER							= 3,
	RB_IDENTIFIERS_SRC_SECONDDISPLAY					= 4,
	RB_IDENTIFIERS_SRC_BARCODE							= 5,
	RB_IDENTIFIERS_SRC_LABELPRINTER						= 6,
	RB_IDENTIFIERS_SRC_REF_BALANCE						= 7,
	RB_IDENTIFIERS_SRC_FAST_HOST						= 8,
	RB_IDENTIFIERS_SRC_GENERAL_IO						= 9,
	RB_IDENTIFIERS_SRC_FOOTSWITCH						= 10,
	RB_IDENTIFIERS_SRC_RELAISBOX						= 11,
	RB_IDENTIFIERS_SRC_EXT_KEYBOARD						= 12,
	RB_IDENTIFIERS_SRC_SERVICE							= 13,		// Encrypted
	// Available									up to 19
	// Next identifier block for "logical channels" see nr 1000

	// Devices
	RB_IDENTIFIERS_SRC_NVMEM							= 20,
	RB_IDENTIFIERS_SRC_DISPLAY							= 21,
	RB_IDENTIFIERS_SRC_CLOCK							= 22,
	// Former RB_IDENTIFIERS_SRC_LOADCELL = 23, replaced by RB_IDENTIFIERS_SRC_WP_SENSOR below
	RB_IDENTIFIERS_SRC_BATTERY							= 24,
	RB_IDENTIFIERS_SRC_SOUND							= 25,
	RB_IDENTIFIERS_SRC_KEYPAD							= 26,
	RB_IDENTIFIERS_SRC_DEVICE							= 27,
	RB_IDENTIFIERS_SRC_CALWEIGHT						= 28,
	RB_IDENTIFIERS_SRC_GPIO								= 29,
	RB_IDENTIFIERS_SRC_POINTINGDEV						= 30,
	RB_IDENTIFIERS_SRC_VOLTMON							= 31,
	RB_IDENTIFIERS_SRC_TRAC								= 32,
	RB_IDENTIFIERS_SRC_KEYBOARD							= 33,
	RB_IDENTIFIERS_SRC_EXTADC							= 34,
	RB_IDENTIFIERS_SRC_RECLOG							= 35,
	// Available									up to 39
	// Next identifier block for "module devices" see nr 1100

	// Module Util
	RB_IDENTIFIERS_SRC_TIMER							= 40,
	RB_IDENTIFIERS_SRC_OS								= 41,
	RB_IDENTIFIERS_SRC_WP_UNIT							= 42,
	RB_IDENTIFIERS_SRC_WP_WEIGHT						= 43,
	RB_IDENTIFIERS_SRC_PARAM							= 44,
	RB_IDENTIFIERS_SRC_TYPE								= 45,
	RB_IDENTIFIERS_SRC_WP_STATE							= 46,
	RB_IDENTIFIERS_SRC_ACCESS							= 47,
	RB_IDENTIFIERS_SRC_FIFO								= 48,
	RB_IDENTIFIERS_SRC_TEXT								= 49,
	// Next identifier block for "module util" see nr 80

	// Module Services
	RB_IDENTIFIERS_SRC_COMMAND							= 50,
	RB_IDENTIFIERS_SRC_WP_BALANCE						= 51,
	RB_IDENTIFIERS_SRC_MEM_TEST							= 52,
	RB_IDENTIFIERS_SRC_EXCEPTION						= 53,
	RB_IDENTIFIERS_SRC_MAIN								= 54,
	RB_IDENTIFIERS_SRC_POWER							= 55,
	RB_IDENTIFIERS_SRC_WP_FACT							= 56,
	RB_IDENTIFIERS_SRC_USAGE							= 57,
	RB_IDENTIFIERS_SRC_BOARDINFO						= 58,
	RB_IDENTIFIERS_SRC_BOOTCONFIG						= 59,
	// Next identifier block for "module services" see nr 1300

	// Module Drivers
	RB_IDENTIFIERS_SRC_PORT								= 60,
	RB_IDENTIFIERS_SRC_I2CM								= 61,
	RB_IDENTIFIERS_SRC_MELSI							= 62,
	RB_IDENTIFIERS_SRC_EEPROM							= 63,
	RB_IDENTIFIERS_SRC_RTC								= 64,
	RB_IDENTIFIERS_SRC_UART								= 65,
	RB_IDENTIFIERS_SRC_CALMOTOR							= 66,
	RB_IDENTIFIERS_SRC_BOOTMONITOR						= 67,
	RB_IDENTIFIERS_SRC_BROWNOUT							= 68,
	RB_IDENTIFIERS_SRC_ADC								= 69,
	RB_IDENTIFIERS_SRC_DAC								= 70,
	RB_IDENTIFIERS_SRC_PWM								= 71,
	RB_IDENTIFIERS_SRC_EXTINT							= 72,
	RB_IDENTIFIERS_SRC_WATCHDOG							= 73,
	RB_IDENTIFIERS_SRC_CAN								= 74,
	RB_IDENTIFIERS_SRC_ACCELERATION						= 75,
	RB_IDENTIFIERS_SRC_GAS								= 76,
	RB_IDENTIFIERS_SRC_TEMPERATURE						= 77,
	RB_IDENTIFIERS_SRC_BLOCKDEV							= 78,		// Was moved to dev folder
	RB_IDENTIFIERS_SRC_RESET							= 79,		// Needed for Win32
	// Next identifier block for "module drivers" see nr 1400

	// Modules Util
	RB_IDENTIFIERS_SRC_SEG7								= 80,
	RB_IDENTIFIERS_SRC_SEG14							= 81,
	RB_IDENTIFIERS_SRC_TIME								= 82,
	RB_IDENTIFIERS_SRC_DATE								= 83,
	RB_IDENTIFIERS_SRC_STRING							= 84,
	RB_IDENTIFIERS_SRC_CRC								= 85,
	RB_IDENTIFIERS_SRC_BASE64							= 86,
	RB_IDENTIFIERS_SRC_MATH								= 87,
	RB_IDENTIFIERS_SRC_QUEUE							= 88,
	RB_IDENTIFIERS_SRC_FORMAT							= 89,
	// Next identifier block for "module util" see nr 1200

	// Rainbow internal packages (SP, WP, CT, ...)
	RB_IDENTIFIERS_SRC_SP								= 90,
	RB_IDENTIFIERS_SRC_WP_POSTPROC						= 91,
	RB_IDENTIFIERS_SRC_WP_SENSOR						= 92,
	RB_IDENTIFIERS_SRC_WP_SETUP							= 93,
	RB_IDENTIFIERS_SRC_WP_PARAM							= 94,
	RB_IDENTIFIERS_SRC_WP_DISPLAY						= 95,
	RB_IDENTIFIERS_SRC_WP_CMD							= 96,
	RB_IDENTIFIERS_SRC_WP_AUTOTARE						= 97,
	// Available									up to 98
	// Next identifier block for "Rainbow internal packages (SP, WP, CT, ...)" see nr 1500

	// TestSuite = 99
	// StressTest = 100
	// RestartShutdown = 101
	// more functional/performance tests up to 109

	// Rainbow Plus Packages
	RB_IDENTIFIERS_SRC_PKG_NETWORK						= 110,
	RB_IDENTIFIERS_SRC_PKG_FILESYSTEM					= 111,
	RB_IDENTIFIERS_SRC_PKG_CRYPTO						= 112,
	RB_IDENTIFIERS_SRC_PKG_USB  						= 113,
	RB_IDENTIFIERS_SRC_PKG_RTOS 						= 114,
	RB_IDENTIFIERS_SRC_PKG_GRAPHICS						= 115,
	RB_IDENTIFIERS_SRC_PKG_FIELDBUS_CAN					= 116,	// CAN subpackage
	RB_IDENTIFIERS_SRC_PKG_FIELDBUS_DATA				= 117,	// Data service layer subpackage of FB package
	RB_IDENTIFIERS_SRC_PKG_FIELDBUS_NETX				= 118,	// NETX subpackage of FB package
	RB_IDENTIFIERS_SRC_PKG_WIRELESS_DRV					= 119,	// Driver layer of WL package
	RB_IDENTIFIERS_SRC_PKG_WIRELESS_WIFI				= 120,	// WIFI subpackage of WL package
	// Available									up to 126
	// Next identifier block for "Rainbow Plus Packages" see nr 1600

	// User defined sources, imported from RB_Config
	RB_IDENTIFIERS_SRC_UserOffsetDummy					= 127,	// Start user sources at 128, do not use this definition!
#ifdef RB_CONFIG_IDENTIFIERS_SRC_USER
	RB_CONFIG_IDENTIFIERS_SRC_USER
#endif
	RB_IDENTIFIERS_SRC_UserListEndDummy					= 256,	// End user sources at 255, do not use this definition!

	// --- Further identifier blocks ---

	// Logical channels
	RB_IDENTIFIERS_LOGICAL_CHANNELS						= 1000,
	//RB_IDENTIFIERS_SRC_<new value>					= 1001,

	// Module Devices
	RB_IDENTIFIERS_DEVICES								= 1100,
	//RB_IDENTIFIERS_SRC_<new value>					= 1101,

	// Module Util
	RB_IDENTIFIERS_MODULE_UTIL							= 1200,
	RB_IDENTIFIERS_SRC_PARSE							= 1201,
	RB_IDENTIFIERS_SRC_PACK								= 1202,
	RB_IDENTIFIERS_SRC_UNICODE							= 1203,
	//RB_IDENTIFIERS_SRC_<new value>					= 1204,

	// Module Services
	RB_IDENTIFIERS_MODULE_SERVICES						= 1300,
	RB_IDENTIFIERS_SRC_POWERSERVICE						= 1301,
	RB_IDENTIFIERS_SRC_SENSACT							= 1302,
	//RB_IDENTIFIERS_SRC_<new value>					= 1303,

	// Module Drivers
	RB_IDENTIFIERS_MODULE_DRIVERS						= 1400,
	RB_IDENTIFIERS_SRC_BACKLIGHT						= 1401,
	RB_IDENTIFIERS_SRC_SPI								= 1402,
	RB_IDENTIFIERS_SRC_EXTADC_ADS1230					= 1403,
	RB_IDENTIFIERS_SRC_SPINOR_AT45XXX					= 1404,
	RB_IDENTIFIERS_SRC_SPINOR_M25PXXX					= 1405,
	RB_IDENTIFIERS_SRC_I2S								= 1407,
	RB_IDENTIFIERS_SRC_GRAPHICCONTROLLER				= 1408,
	RB_IDENTIFIERS_SRC_DMA								= 1410,
	RB_IDENTIFIERS_SRC_PORTEXPANDER						= 1411,
	RB_IDENTIFIERS_SRC_KEYBOARD_USBHID					= 1412,
	RB_IDENTIFIERS_SRC_ONEWIRE							= 1413,
	RB_IDENTIFIERS_SRC_QEI								= 1414,
	RB_IDENTIFIERS_SRC_UID								= 1415,
	RB_IDENTIFIERS_SRC_NVRAM							= 1416,
	RB_IDENTIFIERS_SRC_SPIFI							= 1417,
	RB_IDENTIFIERS_SRC_TOUCHCONTROLLER					= 1418,
	RB_IDENTIFIERS_SRC_SPI_MASTER						= 1419,
	RB_IDENTIFIERS_SRC_PRESSURE							= 1420,
	RB_IDENTIFIERS_SRC_HUMIDITY							= 1421,
	RB_IDENTIFIERS_SRC_INTEEPROM						= 1422,
	//RB_IDENTIFIERS_SRC_<new value>					= 1423,

	// Rainbow internal packages (SP, WP, CT, ...)
	RB_IDENTIFIERS_INTERNAL_PACKAGES					= 1500,
	RB_IDENTIFIERS_SRC_CT_CORE							= 1501,
	//RB_IDENTIFIERS_SRC_<new value>					= 1502,

	// Rainbow Plus Packages, continued
	//RB_IDENTIFIERS_SRC_PKG_<new value>				= 1601,

	// Module System
	RB_IDENTIFIERS_MODULE_SYSTEM						= 1900,
	RB_IDENTIFIERS_SRC_SYSTEM							= 1901,

	// Local source, do not use for inter-module events.
	// This identifier is only used for local purposes, e.g. sending an event from an ISR to its main task.
	RB_IDENTIFIERS_LOCAL								= 2000,

	RB_IDENTIFIERS_SRC_ListEndDummy	// Allow list to have a comma for each entry, do not use this definition!
} RB_DECL_TYPE RB_IDENTIFIERS_tSource;

#define RB_IDENTIFIERS_SRC_LOADCELL						RB_IDENTIFIERS_SRC_WP_SENSOR

#ifdef __cplusplus
}
#endif

#endif // _RB_Identifiers__h
