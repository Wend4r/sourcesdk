//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Trace filter of the rotating prop doors.
//
//=============================================================================//

#ifndef PROPS_H
#define PROPS_H
#ifdef _WIN32
#pragma once
#endif

#include "../shared/util_shared.h"

//-----------------------------------------------------------------------------
// Checks whether a rotating door can swing: weapons never block it, a dynamic physics object
// blocks it only when it weighs at least 32 kg. No fields of its own.
//-----------------------------------------------------------------------------
class CTraceFilterDoor : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterDoor( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;
};

// BEGIN ENTITY VTABLES: generated, do not edit

#include "../shared/baseanimgraph.h"
#include "player_pickup.h"
#include "nav_obstruction.h"

class IGameEvent;
class IPhysicsBody;
class QAngle;

//-----------------------------------------------------------------------------
// CBaseProp adds 2 methods to CBaseAnimGraph: Linux slots 307-308, Windows 306-307.
//-----------------------------------------------------------------------------
template < class TBase = CBaseAnimGraphVTable<> >
abstract_class CBasePropVTable : public TBase
{
public:
	virtual bool OverridePropdata() = 0;
	// Named by what it does
	virtual bool ShouldAutoConvertBackFromDebris() = 0;
};

//-----------------------------------------------------------------------------
// CBreakableProp adds 6 methods to CBaseProp, Windows slots 308-313.
// The Linux table has 8, slots 309-316: 2 override methods of CDefaultPlayerPickupVPhysics, which
// the Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBasePropVTable<> >
abstract_class CBreakablePropVTable : public TBase, public CDefaultPlayerPickupVPhysics
{
public:
	virtual void PlayPuntSound() = 0;
	// Empty in every class
	virtual void Stub_309() = 0;
	// Break() calls it for the physics body to break
	virtual IPhysicsBody *GetRootPhysicsObjectForBreak() = 0;
	// Overrides CDefaultPlayerPickupVPhysics: a slot of this table on Linux only
	virtual bool HasPreferredCarryAnglesForPlayer( CBasePlayerPawn *pPlayer ) override = 0;
	// Overrides CDefaultPlayerPickupVPhysics: a slot of this table on Linux only
	virtual QAngle PreferredCarryAngles() override = 0;
	virtual int GetMinHealthDamage() = 0;
	// Break() passes the break_prop game event to it before firing; CPhysicsProp adds player_held / player_thrown / player_dropped. Named by what it does
	virtual void AddBreakEventInfo( IGameEvent *pEvent ) = 0;
	virtual void OnBreak( const Vector &vecVelocity, const AngularImpulse &angVel, CBaseEntity *pBreaker ) = 0;
};

//-----------------------------------------------------------------------------
// CDynamicProp adds 1 method to CBreakableProp: Linux slot 317, Windows 314.
//-----------------------------------------------------------------------------
template < class TBase = CBreakablePropVTable<> >
abstract_class CDynamicPropVTable : public TBase
{
public:
	// Activate() calls it when the prop should block nav; allocates a CNavObstacleEntity (CNavObstacleEntityDoor in CBasePropDoor) and registers it. Named by what it does
	virtual void CreateNavObstacle() = 0;
};

//-----------------------------------------------------------------------------
// CPhysicsProp adds 1 method to CBreakableProp, Windows slot 314.
// The Linux table has 10, slots 317-326: 9 override methods of INavObstacle, which
// the Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBreakablePropVTable<> >
abstract_class CPhysicsPropVTable : public TBase, public INavObstacle
{
public:
	// Overrides the method of CEntityInstance and the one of the second base: no slot of its own
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override = 0;
	// Windows only in the INavObstacle table). Overrides INavObstacle: a slot of this table on Linux only
	virtual int GetId() const override = 0;
	// Windows only in the INavObstacle table). Overrides INavObstacle: a slot of this table on Linux only
	virtual void WorldSpaceSurroundingBounds( Vector *pVecMins, Vector *pVecMaxs ) const override = 0;
	// Windows only in the INavObstacle table). Overrides INavObstacle: a slot of this table on Linux only
	virtual void WorldSpaceOrientedBounds( Vector *pVecMins, Vector *pVecMaxs, Vector *pOrigin, QAngle *pAngles ) const override = 0;
	// m_nNavObstacleType, an INavObstacle::NavObstacleType_t. Overrides INavObstacle: a slot of this table on Linux only
	virtual INavObstacle::NavObstacleType_t GetNavObstacleType() const override = 0;
	// m_bUpdateNavWhenMoving. Overrides INavObstacle: a slot of this table on Linux only
	virtual bool ShouldUpdateNavWhenMoving() const override = 0;
	// False while attached to a reference frame, m_bUpdateNavWhenMoving while awake, otherwise true for a solid prop at least 18 units tall. Overrides INavObstacle: a slot of this table on Linux only
	virtual bool IsNavAvoidanceObstacleActive() const override = 0;
	// Windows only in the INavObstacle table). Overrides INavObstacle: a slot of this table on Linux only
	virtual float GetNavObstructionHeight() const override = 0;
	// Classifies the prop for the nav mesh from its size, mass and solidity: an INavObstacle::NavObstacleType_t (0 none, 3, or a type a convar sets). Named by what it does
	virtual INavObstacle::NavObstacleType_t CalcNavObstacleType() = 0;
	// Tests bit 0x400000 of the solid flags of the collision property. Overrides INavObstacle: a slot of this table on Linux only
	virtual bool IsNavBlockingObstacle() const override = 0;
	// Sets or clears bit 0x400000 of the solid flags of the collision property by its bool argument. Overrides INavObstacle: a slot of this table on Linux only
	virtual void SetNavBlockingObstacle( bool bEnable ) override = 0;
};

// END ENTITY VTABLES

#endif // PROPS_H
