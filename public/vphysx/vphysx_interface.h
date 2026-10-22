//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Surface property access and world step results of the physics interface
//
//===========================================================================//

#ifndef VPHYSX_INTERFACE_H
#define VPHYSX_INTERFACE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utlvector.h"

class CPhysSurfaceProperties;
struct RnMaterial_t;

abstract_class IPhysSurfacePropertyController
{
public:
	virtual ~IPhysSurfacePropertyController() {}

	// Returns the default surface properties when the token is not found
	virtual const CPhysSurfaceProperties *FindSurfaceProperties( CUtlStringToken token, bool *pFound ) const = 0;

	virtual int GetSurfacePropCount() const = 0;
	virtual const CPhysSurfaceProperties *GetSurfaceProperty( int nIndex ) const = 0;

	// pDescriptions is optional; hidden surface properties are skipped unless bIncludeHidden is set
	virtual void GetSurfacePropertiesNames( CUtlVector< CUtlString > *pNames, CUtlVector< CUtlString > *pDescriptions, bool bIncludeHidden ) const = 0;

	virtual const CPhysSurfaceProperties *GetDefaultSurfaceProperties() const = 0;

	// Counts the loads of the surface properties resource; the game compares it with a cached value and rebuilds its surface property data when it changes
	virtual int GetSurfacePropertiesLoadCount() const = 0;

	// A NULL pProperties uses the default surface properties
	virtual void InitializeRnMaterialFromSurfaceProperty( const CPhysSurfaceProperties *pProperties, RnMaterial_t *pMaterial ) const = 0;
};

// Filled by IVPhysics2World::StepEndReadOnlyGetResults after a step
struct VPhysXWorldStepResults
{
	CUtlVector< void * > m_wokeup;
	CUtlVector< void * > m_asleep;
	CUtlVector< void * > m_awake;
};

#endif // VPHYSX_INTERFACE_H
