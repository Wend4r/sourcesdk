//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The hitbox component of a model entity.
//
//=============================================================================//

#ifndef HITBOXCOMPONENT_H
#define HITBOXCOMPONENT_H

#ifdef _WIN32
#pragma once
#endif

#include "entity2/entitycomponent.h"

#include "schemacompiler/schemaon.h"

class CBaseEntity;
class CGameTrace;
class Vector;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;
struct Ray_t;

//-----------------------------------------------------------------------------
// Embedded in CBaseModelEntity (m_CHitboxComponent). 24 bytes on both platforms.
//-----------------------------------------------------------------------------
schema class CHitboxComponent : public CEntityComponent
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	SchemaMetaInfoHandle_t< CSchemaClassInfo > GetSchemaBinding() override;
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( CHitboxComponent );
	CEntityComponentHelper *GetComponentHelper() override;

	virtual const char *GetComponentName();

	// Forward a change of a networked member to the owner entity. NetworkStateChanged first
	// rebases the changed offsets from the component to the owner when the change has no path
	// index.
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data );
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data );

	// Releases m_nHitboxHandle
	virtual ~CHitboxComponent();

public:
	noschema CEntityOwnerPtr m_pOwner;

	// The slot of the component in the game's list of hitbox components; -1 until it is added.
	// The destructor frees the slot.
	noschema uint32 m_nHitboxHandle;

	float32 m_flBoundsExpandRadius;
};

//-----------------------------------------------------------------------------
// The hitbox queries of the game and the bounds of every hitbox component; CCSHitboxSystem
// overrides both methods and adds no member (one static object, 88 bytes on both platforms). Two
// slots on both platforms (on Windows only CCSHitboxSystem has a table), no virtual destructor.
//
// Every hitbox component takes a slot (CHitboxComponent::m_nHitboxHandle). The bounds of the slots
// are stored eight at a time: slot n has its mins in element 2 * ( n / 8 ) of m_Bounds and its
// maxs in the element after it, at lane n % 8 of each axis.
//-----------------------------------------------------------------------------
abstract_class CHitboxSystem
{
public:
	// One coordinate set of eight slots, a lane per slot
	struct ALIGN32 HitboxBounds8_t
	{
		float m_flX[ 8 ];
		float m_flY[ 8 ];
		float m_flZ[ 8 ];
	} ALIGN32_POST;

	// Adds the intervals where the line from vecStart along vecDelta passes through the hitboxes
	// of the entity to pIntervals (a CUtlVector of 56-byte records, an entry and an exit record per
	// hitbox); true when the line hits a hitbox. The default asks the model of the entity; the
	// CCSHitboxSystem override traces against the hitboxes and back from the end. Named by its
	// caller, the TraceLineIntervals query of the physics, which calls it for every entity it hits
	virtual bool TraceLineIntervals( void *pIntervals, CBaseEntity *pEntity, const Vector &vecStart, const Vector &vecDelta, uint32 nUnknown, float flUnknown ) = 0;
	// Traces the ray from vecStart along vecDelta against the hitboxes of the entity and fills
	// pTrace with the nearest hit (fraction, end position, hitbox, bone); nUnknown is passed on to
	// the query of the hitbox list and transforms
	virtual bool TraceShapeAgainstHitboxes( CGameTrace *pTrace, CBaseEntity *pEntity, const Ray_t &ray, const Vector &vecStart, const Vector &vecDelta, uint32 nUnknown ) = 0;

public:
	// The mins and maxs of the slots, two elements per eight slots, allocated 32-byte aligned; a
	// slot that is freed or newly taken has its lanes zeroed
	CUtlVector< HitboxBounds8_t, int, CUtlVectorMemory_Aligned< HitboxBounds8_t, int, 32 > > m_Bounds;
	// Named by what it holds: the number of slots ever taken, the next new slot
	uint32 m_nSlotCount;
	// The entity handle of the owner of each slot
	CUtlVector< uint32 > m_nEntityIds;
	// The freed slots, taken again last in first out
	CUtlVector< uint32 > m_nFreeList;
};

#include "schemacompiler/schemaoff.h"

#endif // HITBOXCOMPONENT_H
