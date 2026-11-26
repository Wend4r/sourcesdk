//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Polymorphic schema metadata.
//
//=============================================================================//

#ifndef IPOLYMORPHICMETADATAHELPER_H
#define IPOLYMORPHICMETADATAHELPER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// Polymorphic schema metadata of a class family (CEntityInstancePolymorphicMetadataHelper). No virtual
// destructor.
//-----------------------------------------------------------------------------
abstract_class IPolymorphicMetadataHelper
{
public:
	// The only method: CEntityInstancePolymorphicMetadataHelper returns the record (12 bytes) whose int key
	// equals nKey, null when there is none.
	virtual const void *Stub_0( int nKey ) = 0;
};

#endif // IPOLYMORPHICMETADATAHELPER_H
