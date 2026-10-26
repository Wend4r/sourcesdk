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
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"

// Matchmaking extension name, not exported through CreateInterface
#define WORKSHOPANNOTATIONMGR_INTERFACE_VERSION "WorkshopAnnotationMgr001"

struct WorkshopAnnotation_t
{
	CUtlString m_sName;
	// Directory the client appends the annotation file name to
	CUtlString m_sPath;
};

// Implemented by the client's workshop annotation UI component
abstract_class IWorkshopAnnotationMgr
{
public:
	// Marks the annotation list for a refresh on the next lookup; the client calls it after saving an annotation file
	virtual void InvalidateAnnotations() = 0;
	// Appends the annotations of the map, returns the index of the one whose path SetSelectedAnnotation stored or -1
	virtual int GetMapAnnotations( const char *pszMapName, CUtlVector< WorkshopAnnotation_t > *pAnnotations ) = 0;
	// The client's annotation drop-down stores the chosen path per map
	virtual void SetSelectedAnnotation( const char *pszMapName, const char *pszPath ) = 0;
};

#endif // IWORKSHOPANNOTATIONMGR_H
