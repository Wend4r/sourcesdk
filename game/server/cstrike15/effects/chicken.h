#ifndef CSTRIKE15_CHICKEN_H
#define CSTRIKE15_CHICKEN_H

#pragma once

#include "../../props.h"
#include "igameevents.h"
#include "entity2/entityclass.h"
#include "../bot/improv_locomotor.h"
#include "../../../shared/econ/ihasattributes.h"

class CAttributeContainer;
class CAttributeList;
class CAttributeManager;

//-----------------------------------------------------------------------------
// CChicken adds 2 methods to CDynamicProp, Windows slots 315-316.
// The Linux table has 28, slots 318-345: 26 override methods of IGameEventListener2 and CImprovLocomotor and IHasAttributes, which
// the Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CDynamicPropVTable<> >
abstract_class CChickenVTable : public TBase, public IGameEventListener2, public CImprovLocomotor, public IHasAttributes
{
public:
	// Overrides the method of CEntityInstance and the one of the second base: no slot of its own
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override = 0;
	// Overrides the method of CEntityInstance and the one of IHasAttributes: no slot of its own
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() override = 0;
	virtual void ChickenTouch( CBaseEntity *pOther ) = 0;
	// Its secondary-table thunk maps to this slot. Overrides IGameEventListener2: a slot of this table on Linux only
	virtual void FireGameEvent( IGameEvent *pEvent ) override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual const Vector &GetCentroid() const override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual const Vector &GetFeet() const override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual const Vector &GetEyes() const override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual float GetMoveAngle() const override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual CNavArea *GetLastKnownArea() const override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual bool GetSimpleGroundHeightWithFloor( const Vector &pos, float *height, Vector *normal ) override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual void Crouch() override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual void StandUp() override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual bool IsCrouching() const override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual void Jump() override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual bool IsJumping() const override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual void Run() override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual void Walk() override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual bool IsRunning() const override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual void StartLadder( const CNavLadder *ladder, NavTraverseType how, const Vector &approachPos, const Vector &departPos ) override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual bool TraverseLadder( const CNavLadder *ladder, NavTraverseType how, const Vector &approachPos, const Vector &departPos, float deltaT ) override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual bool IsUsingLadder() const override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual void TrackPath( const Vector &pathGoal, float deltaT ) override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual void OnMoveToSuccess( const Vector &goal ) override = 0;
	// Overrides CImprovLocomotor: a slot of this table on Linux only
	virtual void OnMoveToFailure( const Vector &goal, MoveToFailureType reason ) override = 0;
	// IHasAttributes entry 2. Overrides IHasAttributes: a slot of this table on Linux only
	virtual CAttributeManager *GetAttributeManager() override = 0;
	// IHasAttributes entry 3. Overrides IHasAttributes: a slot of this table on Linux only
	virtual CAttributeContainer *GetAttributeContainer() override = 0;
	// IHasAttributes entry 4. Overrides IHasAttributes: a slot of this table on Linux only
	virtual CBaseEntity *GetAttributeOwner() override = 0;
	// IHasAttributes entry 5. Overrides IHasAttributes: a slot of this table on Linux only
	virtual CAttributeList *GetAttributeList() override = 0;
	// IHasAttributes entry 6. Overrides IHasAttributes: a slot of this table on Linux only
	virtual void ReapplyProvision() override = 0;
	virtual void ChickenUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) = 0;
};

#endif // CSTRIKE15_CHICKEN_H
