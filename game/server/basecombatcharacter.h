#ifndef BASECOMBATCHARACTER_H
#define BASECOMBATCHARACTER_H

#pragma once

#include "../shared/baseanimgraph.h"

class CNavArea;
struct CTakeDamageResult;

//-----------------------------------------------------------------------------
// CBaseCombatCharacter adds 32 methods to CBaseAnimGraph: Linux slots 307-338, Windows 306-337.
// The two FInViewCone swap places on Windows.
//-----------------------------------------------------------------------------
template < class TBase = CBaseAnimGraphVTable<> >
abstract_class CBaseCombatCharacterVTable : public TBase
{
public:
	// A static table; overridden by CBasePlayerPawn
	virtual const void *GetPhysicsImpactDamageTable() = 0;
	// Returns the address of the same static object in every class. A second static table, which VPhysicsShadowCollision takes for a network client
	virtual const void *Stub_307() = 0;
	// Returns true in every class
	virtual bool Stub_308() = 0;
	// Calls the other one with the WorldSpaceCenter of the entity
	virtual bool FInViewCone( CBaseEntity *pEntity ) = 0;
	virtual bool FInViewCone( const Vector &vecSpot ) = 0;
	// Empty; CAI_ExpresserHost tells the expresser
	virtual void NoteSpeaking( float flDuration, float flDelay ) = 0;
	// Out of BodyAngles
	virtual Vector BodyDirection2D() = 0;
	virtual Vector BodyDirection3D() = 0;
	// BodyDirection2D
	virtual Vector HeadDirection2D() = 0;
	// BodyDirection3D
	virtual Vector HeadDirection3D() = 0;
	// The dot product FInViewCone compares with: -1 here, 0.766 for a player pawn. Named by what it does. Asked by FInViewCone on every call
	virtual float GetFieldOfView() = 0;
	// Returns true in every class
	virtual bool Stub_317() = 0;
	// Empty in every class
	virtual void Stub_318() = 0;
	// Never overridden
	virtual Vector CalcDeathForceVector( const CTakeDamageResult &result ) = 0;
	// Returns false, never overridden
	virtual bool ShouldGib( const CTakeDamageResult &result ) = 0;
	// Empty; CBasePlayerPawn passes the call to DropActivePlayerWeapon of its item services
	virtual void DropActivePlayerWeapon( const Vector &vecForce, void *pUnknown, bool bUnknown ) = 0;
	// False in every class; Event_Killed of the pawn asks it only when ShouldGib answered true and removes the entity after the base Event_Killed when it answers true.
	virtual bool Event_Gibbed( const CTakeDamageResult &result ) = 0;
	// Overridden by CBasePlayerPawn
	virtual bool BecomeRagdoll( const CTakeDamageResult &result, const Vector &vecForce ) = 0;
	// The first test of VPhysicsShadowCollision: true only for a moving VPHYSICS object that is not the ground entity, not a ragdoll, not thrown by this character, while the character moves at 300 units/s or less; never overridden. Named by what it does
	virtual bool ShouldTakeShadowCollisionDamage( CBaseEntity *pOther, void *pOtherPhysics ) = 0;
	// Empty in every class
	virtual void Stub_325() = 0;
	// Returns the address of the same static object in every class. A zero vector
	virtual const void *Stub_326() = 0;
	// False in every class; CCSPlayerPawn::RoundRespawn calls it with 0 when m_fFlags has FL_IN_VEHICLE, before it clears the parent. The argument is not explained
	virtual bool ExitVehicle( int nUnknown ) = 0;
	// Empty in every class
	virtual void Stub_328() = 0;
	// Empty in every class
	virtual void Stub_329() = 0;
	// Moves the player count of the team from the left area to the entered one; CCSPlayerPawn also tells its bot and sets m_szLastPlaceName. Called by UpdateLastKnownNav, ClearLastKnownArea and SetLastKnownNavArea.
	virtual void OnNavAreaChanged( CNavArea *pEnteredArea, CNavArea *pLeftArea ) = 0;
	// When the current, last or fallback nav area of the character is in the sorted list of removed areas, clears them (OnNavAreaChanged( NULL, current )); never overridden.
	virtual void OnNavAreasRemoved( int nCount, CNavArea **ppAreas ) = 0;
	// True; CCSObserverPawn returns false. PreThink of the pawn asks it before CBaseCombatCharacter::UpdateLastKnownNav. Named by its caller. Asked every tick by PreThink of the pawn and by SetOrResetInitialState
	virtual bool ShouldUpdateLastKnownNav() = 0;
	// Traces a line from EyePosition to the EyePosition of a combat character (WorldSpaceCenter of anything else), ignoring this and its owner; true when nothing or pEntity is hit. Never overridden, no caller found
	virtual bool FVisible( CBaseEntity *pEntity ) = 0;
	// The forward vector of the absolute angles; never overridden. Named by what it does
	virtual Vector GetAbsForward() = 0;
	// Asks the navigation mesh for the area nearest to the character, by m_nNavHullIdx and within 10000 units; a second search with other flags goes to the argument when the first finds nothing. Never overridden. Named by what it does
	virtual CNavArea *FindNearestNavArea( CNavArea **ppFallbackArea ) = 0;
	// Calls OnNavAreaChanged( pArea, current ) and stores the current, last and fallback nav areas, the stores UpdateLastKnownNav makes after FindNearestNavArea; never overridden. Named by what it does
	virtual void SetLastKnownNavArea( CNavArea *pArea, CNavArea *pFallbackArea ) = 0;
	// Empty in every class
	virtual void Stub_337() = 0;
};

#endif // BASECOMBATCHARACTER_H
