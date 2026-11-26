//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A condition a loop mode waits for.
//
//=============================================================================//

#ifndef IPREREQUISITE_H
#define IPREREQUISITE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CPrerequisiteDescriptionHelper;

//-----------------------------------------------------------------------------
// A condition a loop mode waits for (CResourceManifestPrerequisite: a resource manifest is loaded).
//-----------------------------------------------------------------------------
abstract_class IPrerequisite
{
public:
	virtual ~IPrerequisite() {}
	virtual void AddRef() = 0;
	virtual void Release() = 0;

	// The status of the prerequisite, waiting at most flTimeout seconds.
	virtual int GetPrerequisiteStatus( double flTimeout ) = 0;
	virtual const char *GetName() const = 0;
	virtual void Describe( int nIndent, CPrerequisiteDescriptionHelper &helper ) const = 0;
};

#endif // IPREREQUISITE_H
