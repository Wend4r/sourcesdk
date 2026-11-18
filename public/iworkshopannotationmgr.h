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

// Implemented by CUiComponent_WorkshopAnnotation in client.dll ( no Linux build: the Linux dedicated server
// has no client module ); the server reaches it through the matchmaking extension
abstract_class IWorkshopAnnotationMgr
{
public:
	// Named by what it does: the next LoadMapAnnotations rescans annotations/local/<map>/*.txt
	virtual void MarkLocalAnnotationsDirty() = 0;

	// Named by what it does: loads the annotations of the map ( or of its alias ) into pAnnotations and
	// returns how many were loaded, -1 when the map has no annotation file
	virtual int LoadMapAnnotations( const char *pszMapName, void *pAnnotations ) = 0;

	// Named by what it does: remembers the annotation file of the map
	virtual void SetMapAnnotationPath( const char *pszMapName, const char *pszPath ) = 0;
};

#endif // IWORKSHOPANNOTATIONMGR_H
