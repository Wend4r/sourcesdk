#ifndef CSTRIKE15_CS_SIMPLE_HOSTAGE_H
#define CSTRIKE15_CS_SIMPLE_HOSTAGE_H

#pragma once

#include "../../ai_speech.h"
#include "../../basecombatcharacter.h"
#include "entity2/entityclass.h"
#include "../bot/improv_locomotor.h"

class CAI_Expresser;

//-----------------------------------------------------------------------------
// CHostage adds 2 methods to CAI_ExpresserHost< CHostageExpresserShim >, Windows slots 343-344.
// The Linux table has 22, slots 344-365: 344-363 override the methods of CImprovLocomotor, the
// second base, which the Itanium ABI also enters into the primary table and MSVC does not.
// CHostageExpresserShim adds no method to CBaseCombatCharacter; CAI_ExpresserHost adds its 5
// methods at Linux slots 339-343, Windows 338-342 (the overload pairs swap places on Windows).
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CAI_ExpresserHost< CBaseCombatCharacterVTable<> > >
abstract_class CHostageVTable : public TBase, public CImprovLocomotor
{
public:
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
	virtual CAI_Expresser *CreateExpresser() = 0;
	virtual void HostageUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) = 0;
};

#endif // CSTRIKE15_CS_SIMPLE_HOSTAGE_H
