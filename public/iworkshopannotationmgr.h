//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Lookup of workshop map annotations
//
//===========================================================================//

#ifndef IWORKSHOPANNOTATIONMGR_H
#define IWORKSHOPANNOTATIONMGR_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

// Matchmaking extension name, not exported through CreateInterface
#define WORKSHOPANNOTATIONMGR_INTERFACE_VERSION "WorkshopAnnotationMgr001"

abstract_class IWorkshopAnnotationMgr
{
public:
	virtual void unk000() = 0;
	virtual int unk001( const char *pszMapName, void *pAnnotations ) = 0;
	virtual void unk002( const char *pszMapName, const char *pszPath ) = 0;
};

#endif // IWORKSHOPANNOTATIONMGR_H
