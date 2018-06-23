//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		WP_Version.h
//! \ingroup	util
//! \brief		This file (and only this file) defines the version of weighing package.
//!
//! Process for changing the version of the weighing package
//! - The Rainbow Change Control Board (CCB) will generally decide when to change the major, minor and bugfix version and
//!   the weighing package responsible will upgrade the version.
//! - During the release process, a checklist-item is executed to ensure that the weighing package version was updated correctly
//!
//! This version is used to
//! - assemble XM0021 command output
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Martin Heusser
//
// $Date: 2017/12/01 14:01:06MEZ $
// $Revision: 1.52 $
//
//==================================================================================================
#ifndef _WP_Version__h
#define _WP_Version__h


//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

//! Version of weighing package
//! Format: [major].[minor].[bugfix].[timestamp]
//!   [major]         Major version      Will be changed for complete new design
//!   [minor]         Minor version      Will be changed when approval relevant new features are added
//!   [bugfix]        Bug fix version    Will be changed when approval relevant bugs are fixed
//!   [timestamp]     Timestamp version  Will be changed on each approval and non approval relevant change
//!
//! \note In case of changing mayor, minor or bugfix version, TS_WP_PostProc.tat has to be adapted.

#define WP_VERSION "2.2.5.20171201" // Do not change the [major] and [minor] numbers unless approval relevant new features are added.

//==================================================================================================
#endif // _RB_Version__h
