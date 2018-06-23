//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Version.c
//! \ingroup	util
//! \brief		Functions for RB and Package Versions
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
//! \author		Andri Toggenburger
//
// $Date: 2017/03/08 17:06:04MEZ $
// $Revision: 1.36 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Version"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Version.h"
// This module is mandatory and has no RB_CONFIG_USE, no check is needed here.

#include <string.h>


//==================================================================================================
//  G L O B A L   V A R I A B L E S
//==================================================================================================

const char *RB_VERSION_BuildDate = __DATE__;
const char *RB_VERSION_BuildTime = __TIME__;


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

// Head pointer
static RB_VERSION_tPackageInfo *s_packageInfoHead = NULL;


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_VERSION_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module RB_Version
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_VERSION_Initialize(void)
{
}


//--------------------------------------------------------------------------------------------------
// RB_VERSION_GetPackageInfoListHead
//--------------------------------------------------------------------------------------------------
//! \brief	Get the head of the list of initialized package names and version strings
//!
//! \return		Head of the list will be returned. List is sorted ascending by package names
//--------------------------------------------------------------------------------------------------
RB_VERSION_tPackageInfo* RB_VERSION_GetPackageInfoListHead(void)
{
	return s_packageInfoHead;
}


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
void RB_VERSION_SetPackageInfo(RB_VERSION_tPackageInfo *info)
{
	RB_VERSION_tPackageInfo *tmp = RB_VERSION_GetPackageInfoListHead();

	// list was empty, set element as head.
	if (tmp == NULL) {
		s_packageInfoHead = info;
		return;
	}

	// Check if entry is already present in list
	if (RB_VERSION_PackageInfoLookup(info->PackageName) != NULL) {
		return; // an entry for this package is already in list, prevent duplicate.
	}

	if (strcmp(info->PackageName, tmp->PackageName) < 0) {
		// new item becomes head
		info->next = tmp;
		s_packageInfoHead = info;
	} else {
		// add item sorted ascending according to Package Name.
		while (tmp->next != NULL && strcmp(info->PackageName, tmp->next->PackageName) > 0) {
			// loop while next item is smaller than the new item.
			tmp = tmp->next;
		}
		// tmp->next == NULL || tmp->next->PackageName >= info->PackageName

		// add new item to list
		info->next = tmp->next;
		tmp->next = info;
	}
}


//--------------------------------------------------------------------------------------------------
// RB_VERSION_PackageInfoLookup
//--------------------------------------------------------------------------------------------------
//! \brief	Searches for a package info item in the info list.
//!
//! \param	query			Name of the package to be searched for
//! \return					Pointer to package info or NULL if not found
//--------------------------------------------------------------------------------------------------
RB_VERSION_tPackageInfo *RB_VERSION_PackageInfoLookup(const char *query)
{
	RB_VERSION_tPackageInfo *tmp = RB_VERSION_GetPackageInfoListHead();

	while ((tmp != NULL) && (strcmp(query, tmp->PackageName) != 0)) {
		tmp = tmp->next;
	}
	// item == NULL || package name match (strcmp == 0)
	return tmp;
}


//--------------------------------------------------------------------------------------------------
// RB_VERSION_PackageInfoCount
//--------------------------------------------------------------------------------------------------
//! \brief	Returns the number of items in the version info list (excluding the head)
//!
//! \return		Number of items in list.
//--------------------------------------------------------------------------------------------------
size_t RB_VERSION_PackageInfoCount(void)
{
	const RB_VERSION_tPackageInfo *tmp = RB_VERSION_GetPackageInfoListHead();
	size_t count = 0;
	while (tmp != NULL) {
		count++;
		tmp = tmp->next;
	}
	return count;
}
