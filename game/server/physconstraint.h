//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The list of constraint anchors (info_constraint_anchor) the
//          physics constraints look up by name.
//
//=============================================================================//

#ifndef PHYSCONSTRAINT_H
#define PHYSCONSTRAINT_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "ehandle.h"
#include "iattachmentoverride.h"

#include <mathlib/vector.h>
#include <tier0/utlsymbollarge.h>
#include <tier0/utlvector.h>

class CBaseEntity;

//-----------------------------------------------------------------------------
// One anchor (40 bytes).
//-----------------------------------------------------------------------------
struct constraint_anchor_t
{
	// The anchor's origin in the space of its parent
	Vector localOrigin;
	// The anchor's parent entity, invalid when it has none
	CHandle< CBaseEntity > hEntity;
	// A byte: the attachment the anchor is parented to
	AttachmentHandle_t parentAttachment;
	// The anchor entity's name
	CUtlSymbolLarge name;
	// CInfoConstraintAnchor::m_massScale
	float massScale;
};

//-----------------------------------------------------------------------------
// Registered as "CAnchorList" (static factory). It adds no virtual methods to IGameSystem.
//-----------------------------------------------------------------------------
class CAnchorList : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	CAnchorList();
	virtual ~CAnchorList();

	// Purges m_list
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// CInfoConstraintAnchor appends itself on spawn; the constraints search the
	// list from the end by name
	CUtlVector< constraint_anchor_t > m_list;
};

// BEGIN ENTITY VTABLES: generated, do not edit

#include "baseentity.h"
#include "../shared/positionwatcher.h"

class IPhysicsJoint;
struct hl_constraint_info_t;

//-----------------------------------------------------------------------------
// CPhysConstraint adds 3 methods to CLogicalEntity (through CLogicalEntity and CServerOnlyEntity, which add no method): Linux slots 239-241, Windows 238-240.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CPhysConstraintVTable : public TBase
{
public:
	virtual void Deactivate() = 0;
	virtual void OnConstraintSetup( hl_constraint_info_t &info ) = 0;
	virtual IPhysicsJoint *CreateConstraint( const hl_constraint_info_t &info ) = 0;
};

//-----------------------------------------------------------------------------
// CGenericConstraint adds no method to CPhysConstraint. The Linux table has 1 more slot, 242: the override of a method of IVPhysicsWatcher, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CPhysConstraintVTable<> >
abstract_class CGenericConstraintVTable : public TBase, public IVPhysicsWatcher
{
public:
	// Overrides IVPhysicsWatcher: a slot of this table on Linux only
	virtual void NotifyVPhysicsStateChanged( IPhysicsObject *pPhysics, CBaseEntity *pEntity, bool bAwake ) override = 0;
};

//-----------------------------------------------------------------------------
// CPhysHinge adds no method to CPhysConstraint. The Linux table has 2 more slots, 242-243: overrides of methods of IVPhysicsWatcher, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CPhysConstraintVTable<> >
abstract_class CPhysHingeVTable : public TBase, public IVPhysicsWatcher
{
public:
	// Applies m_hingeFriction to the body when it is dynamic and the flag is set. Overrides IVPhysicsWatcher: a slot of this table on Linux only
	virtual void Unk_2( IPhysicsBody *pBody, bool bAwake ) override = 0;
	// Overrides IVPhysicsWatcher: a slot of this table on Linux only
	virtual void NotifyVPhysicsStateChanged( IPhysicsObject *pPhysics, CBaseEntity *pEntity, bool bAwake ) override = 0;
};

//-----------------------------------------------------------------------------
// CPhysSlideConstraint adds no method to CPhysConstraint. The Linux table has 1 more slot, 242: the override of a method of IVPhysicsWatcher, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CPhysConstraintVTable<> >
abstract_class CPhysSlideConstraintVTable : public TBase, public IVPhysicsWatcher
{
public:
	// Overrides IVPhysicsWatcher: a slot of this table on Linux only
	virtual void NotifyVPhysicsStateChanged( IPhysicsObject *pPhysics, CBaseEntity *pEntity, bool bAwake ) override = 0;
};

//-----------------------------------------------------------------------------
// CSplineConstraint adds no method to CPhysConstraint. The Linux table has 1 more slot, 242: the override of a method of IPositionWatcher, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CPhysConstraintVTable<> >
abstract_class CSplineConstraintVTable : public TBase, public IPositionWatcher
{
public:
	// Overrides IPositionWatcher: a slot of this table on Linux only
	virtual void NotifyPositionChanged( CBaseEntity *pEntity ) override = 0;
};

// END ENTITY VTABLES

#endif // PHYSCONSTRAINT_H
