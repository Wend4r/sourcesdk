//====== Copyright c 1996-2007, Valve Corporation, All rights reserved. =======//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef SYSTEMINFORMATION_H
#define SYSTEMINFORMATION_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

#ifndef PLATFORM_INTERFACE
	#define PLATFORM_INTERFACE
#endif

//
//	Plat_GetMemPageSize
//		Returns the size of a memory page in kilobytes.
//
PLATFORM_INTERFACE uint32 Plat_GetMemPageSize();



#endif // #ifndef SYSTEMINFORMATION_H
