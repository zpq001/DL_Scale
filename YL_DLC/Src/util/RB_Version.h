//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Version.h
//! \ingroup	util
//! \brief		This file (and only this file) defines the version of rainbow.
//!
//! This version is used to
//! - output the version on the display of products
//! - output version on MT-SICS I3 requests
//! - name distribution files (zip files) correctly
//! - assemble XM0021 command output
//!
//! Build-date and build-time is used from RB_MemTest to validate firmware checksum and force
//! rebuilding of checksum after a firmware update. Therefore RB_Version must be recompiled each
//! time a new firmware is built. See also doc\Rainbow-IAR-ARM.pdf chapter "Build Actions".
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		George Fankhauser, Michael von Ow, Andri Toggenburger
//
// $Date: 2017/12/01 13:59:34MEZ $
// $Revision: 1.310.1.1 $
//
//==================================================================================================

#ifndef _RB_Version__h
#define _RB_Version__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

// This module is mandatory and has no RB_CONFIG_USE, no check is needed here.

#include "RB_Typedefs.h"


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

//! Rainbow platform version:
#define RB_VERSION "2.10.0.20171201"

//! Rainbow software certificate number:
#define RB_SW_CERTIFICATE "42700189LA"

// Variables are declared in RB_Version.c
extern const RB_DECL_CONST char *RB_VERSION_BuildDate;
extern const RB_DECL_CONST char *RB_VERSION_BuildTime;


//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

typedef struct RB_VERSION_tPackageInfo_ {
	// Package Name must be according to XM0021 Specifications (e.g. "Rainbow Graphics")
	// Attention: when a new name is needed, the name must be added to the XM0021 specification in the MT-SICS-P Manual
	// Send an e-mail to the manual owner
	const char *PackageName;
	// Package Version must be according to the Rainbow Versioning Scheme (Rainbow CM Plan).
	const char *PackageVersion;
	// Do not modify "next"
	struct RB_VERSION_tPackageInfo_ *next;
} RB_DECL_TYPE RB_VERSION_tPackageInfo;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_VERSION_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module RB_Version
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_VERSION_Initialize(void);


//--------------------------------------------------------------------------------------------------
// RB_VERSION_GetPackageInfoListHead
//--------------------------------------------------------------------------------------------------
//! \brief	Get the head of the list of initialized package names and version strings
//!
//! \return		Head of the list will be returned. List is sorted ascending by package names
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_VERSION_tPackageInfo* RB_VERSION_GetPackageInfoListHead(void);


//--------------------------------------------------------------------------------------------------
// RB_VERSION_SetPackageInfo
//--------------------------------------------------------------------------------------------------
//! \brief	This function is used by the initializer of a package to install its info and version.
//!
//! The info will inserted into the list sorted ascending by the package name
//!
//! \param	info			The package info to be installed
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_VERSION_SetPackageInfo(RB_VERSION_tPackageInfo *info);


//--------------------------------------------------------------------------------------------------
// RB_VERSION_PackageInfoLookup
//--------------------------------------------------------------------------------------------------
//! \brief	Searches for a package info item in the info list.
//!
//! \param	query			Name of the package to be searched for
//! \return					Pointer to package info or NULL if not found
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_VERSION_tPackageInfo *RB_VERSION_PackageInfoLookup(const char *query);


//--------------------------------------------------------------------------------------------------
// RB_VERSION_PackageInfoCount
//--------------------------------------------------------------------------------------------------
//! \brief	Returns the number of items in the version info list (excluding the head)
//!
//! \return		Number of items in list.
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC size_t RB_VERSION_PackageInfoCount(void);


#ifdef __cplusplus
}
#endif

#endif // _RB_Version__h
