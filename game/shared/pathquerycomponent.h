//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Sampled path queries and the entity component that holds them.
//
//=============================================================================//

#ifndef PATHQUERYCOMPONENT_H
#define PATHQUERYCOMPONENT_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/utlvector.h"
#include "mathlib/vector.h"
#include "mathlib/transform.h"
#include "entity2/entitycomponent.h"

#include "schemacompiler/schemaon.h"

//-----------------------------------------------------------------------------
// A path sampled into positions, curve parameters and distances, answering the closest point,
// position and direction at a parameter, and distance queries. 128 bytes, 16 aligned, on both
// platforms.
//-----------------------------------------------------------------------------
schema class CPathQueryUtil
{
public:
	// Named by what it does: the path the samples are taken from, passed to the curve evaluation
	// (in CPathSimple the path data that follows its path query component)
	noschema const void *m_pPath;

	CTransform m_PathToEntityTransform;
	CUtlVector< Vector > m_vecPathSamplePositions;
	CUtlVector< float32 > m_vecPathSampleParameters;
	CUtlVector< float32 > m_vecPathSampleDistances;

	// True at construction
	bool m_bIsClosedLoop;
};

//-----------------------------------------------------------------------------
// Embedded in CPathSimple (m_CPathQueryComponent). Save and load go to the path query utility.
// 160 bytes on both platforms.
//-----------------------------------------------------------------------------
schema class CPathQueryComponent : public CEntityComponent, public CPathQueryUtil
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	SchemaMetaInfoHandle_t< CSchemaClassInfo > GetSchemaBinding() override;
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( CPathQueryComponent );
	CEntityComponentHelper *GetComponentHelper() override;

	virtual const char *GetComponentName();

public:
	noschema CEntityOwnerPtr m_pOwner;
};

#include "schemacompiler/schemaoff.h"

#endif // PATHQUERYCOMPONENT_H
