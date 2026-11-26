#ifndef NAV_ENTITIES_H
#define NAV_ENTITIES_H

#pragma once

#include "../shared/basemodelentity.h"
#include "../shared/positionwatcher.h"
#include "nav_obstruction.h"

//-----------------------------------------------------------------------------
// CFuncNavBlocker adds 1 method to CBaseModelEntity, Windows slot 271.
// The Linux table has 2, slots 272-273: 1 override methods of IPositionWatcher, which
// the Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseModelEntityVTable<> >
abstract_class CFuncNavBlockerVTable : public TBase, public IPositionWatcher
{
public:
	// Updates the nav blocker when the watched entity moves. Overrides IPositionWatcher: a slot of this table on Linux only
	virtual void NotifyPositionChanged( CBaseEntity *pEntity ) override = 0;
	// Sets the solid flags of the collision property and updates the nav blocker
	virtual void Unk_271() = 0;
};

//-----------------------------------------------------------------------------
// CFuncNavObstruction adds no method to CBaseModelEntity. The Linux table has 9 more slots, 272-280: overrides of methods of INavObstacle and IPositionWatcher, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseModelEntityVTable<> >
abstract_class CFuncNavObstructionVTable : public TBase, public INavObstacle, public IPositionWatcher
{
public:
	// Overrides the method of CEntityInstance and the one of the second base: no slot of its own
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override = 0;
	// Overrides INavObstacle: a slot of this table on Linux only
	virtual int GetId() const override = 0;
	// The nav obstacle type (INavObstacle::NavObstacleType_t): NAV_OBSTACLE_TYPE_AVOID. Overrides INavObstacle: a slot of this table on Linux only
	virtual INavObstacle::NavObstacleType_t GetNavObstacleType() const override = 0;
	// Whether the obstacle counts now: !m_bDisabled. Overrides INavObstacle: a slot of this table on Linux only
	virtual bool IsNavAvoidanceObstacleActive() const override = 0;
	// 58.0. Overrides INavObstacle: a slot of this table on Linux only
	virtual float GetNavObstructionHeight() const override = 0;
	// Overrides INavObstacle: a slot of this table on Linux only
	virtual void WorldSpaceSurroundingBounds( Vector *pMins, Vector *pMaxs ) const override = 0;
	// Overrides INavObstacle: a slot of this table on Linux only
	virtual void WorldSpaceOrientedBounds( Vector *pMins, Vector *pMaxs, Vector *pOrigin, QAngle *pAngles ) const override = 0;
	// Tests bit 0x400000 of the solid flags of the collision property. Overrides INavObstacle: a slot of this table on Linux only
	virtual bool IsNavBlockingObstacle() const override = 0;
	// Sets or clears bit 0x400000 of the solid flags of the collision property by its bool argument. Overrides INavObstacle: a slot of this table on Linux only
	virtual void SetNavBlockingObstacle( bool bEnable ) override = 0;
	// The same body as the one of CFuncNavBlocker. Overrides IPositionWatcher: a slot of this table on Linux only
	virtual void NotifyPositionChanged( CBaseEntity *pEntity ) override = 0;
};

#endif // NAV_ENTITIES_H
