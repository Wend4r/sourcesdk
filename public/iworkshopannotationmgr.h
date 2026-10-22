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

// Implemented by the client's workshop annotation UI component
abstract_class IWorkshopAnnotationMgr
{
public:
	// Marks the annotation list for a refresh on the next lookup
	virtual void unk000() = 0;
	// Appends a pair of strings (name, path) per annotation of the map to a CUtlVector of 16-byte entries,
	// returns the index of the selected one or -1
	virtual int unk001( const char *pszMapName, void *pAnnotations ) = 0;
	// Sets the path stored for the map name
	virtual void unk002( const char *pszMapName, const char *pszPath ) = 0;
};

#endif // IWORKSHOPANNOTATIONMGR_H
