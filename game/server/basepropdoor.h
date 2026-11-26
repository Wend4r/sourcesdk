#ifndef BASEPROPDOOR_H
#define BASEPROPDOOR_H

#pragma once

#include "props.h"
#include "igameevents.h"

class CEntityKeyValues;
class KeyValues3;
class QAngle;
struct Extent;
struct SndOpEventGuid_t;

//-----------------------------------------------------------------------------
// CBasePropDoor adds 28 methods to CDynamicProp, Windows slots 315-342.
// The Linux table has 29, slots 318-346: 1 override methods of IGameEventListener2, which
// the Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CDynamicPropVTable<> >
abstract_class CBasePropDoorVTable : public TBase, public IGameEventListener2
{
public:
	virtual bool IsAbleToCloseAreaPortals() const = 0;
	// Overrides IGameEventListener2: a slot of this table on Linux only
	virtual void FireGameEvent( IGameEvent *pEvent ) override = 0;
	virtual bool IsDoorLocked() = 0;
	// Returns !m_bNoNPCs; OpenIfUnlocked asks it together with IsDoorLocked. Named by what it does
	virtual bool CanNPCsOpen() = 0;
	// TryOpenClose asks it for an ajar door (true = open, false = close); true here, CPropDoorRotating tests m_bAjarDoorShouldntAlwaysOpen and the yaw against m_angRotationClosed. Named by what it does
	virtual bool ShouldAjarDoorOpen( CBaseEntity *pActivator ) = 0;
	virtual bool DoorCanClose( bool bAutoClose ) = 0;
	virtual bool DoorCanOpen() = 0;
	// CPropDoorRotating returns rotation distance / m_flSpeed
	virtual float GetOpenInterval() = 0;
	virtual void ComputeDoorExtent( Extent *extent, unsigned int extentType ) = 0;
	virtual void CalcDoorSounds( KeyValues3 *pModelKV, CEntityKeyValues *pKeyValues ) = 0;
	// Named by what it does
	virtual void ScheduleAutoClose() = 0;
	virtual void OnDoorOpened() = 0;
	virtual void OnDoorClosed() = 0;
	virtual void OnLockedUse( CBaseEntity *pActivator, CBaseEntity *pCaller ) = 0;
	virtual void OnUnlockedUse( CBaseEntity *pActivator, CBaseEntity *pCaller ) = 0;
	virtual void BeginOpening( CBaseEntity *pOpenAwayFrom ) = 0;
	virtual void BeginClosing() = 0;
	virtual void DoorStop() = 0;
	virtual void DoorResume() = 0;
	virtual void DoorTeleportToSpawnPosition() = 0;
	virtual void Lock( bool bSilent ) = 0;
	virtual void Unlock( bool bSilent ) = 0;
	virtual SndOpEventGuid_t EmitDoorSound( const char *pszSound, bool bAtLatch ) = 0;
	// Sets the networked m_bNoNPCs when it changes. Named by what it does
	virtual void SetNoNPCs( bool bNoNPCs ) = 0;
	virtual bool TryOpenClose( CBaseEntity *pActivator, CBaseEntity *pCaller, int nUseType ) = 0;
	virtual void OpenIfUnlocked( CBaseEntity *pActivator, CBaseEntity *pOpenAwayFrom ) = 0;
	virtual void OnStartOpeningFromClosed( CBaseEntity *pOpenAwayFrom, bool bStartMovingSound ) = 0;
	// Starts m_SoundMoving (EmitDoorSound) and sets its angular_momentum parameter; OnStartOpeningFromClosed calls it when its bool is set. Named by what it does
	virtual void StartMovingSound() = 0;
	// Stops the sound guid that StartMovingSound stored. Named by what it does
	virtual void StopMovingSound() = 0;
};

//-----------------------------------------------------------------------------
// CPropDoorRotating adds 6 methods to CBasePropDoor: Linux slots 347-352, Windows 343-348.
//-----------------------------------------------------------------------------
template < class TBase = CBasePropDoorVTable<> >
abstract_class CPropDoorRotatingVTable : public TBase
{
public:
	// Named by what it does
	virtual bool ChooseOpenAngles( CBaseEntity *pOpenAwayFrom, QAngle *pAngOpen ) = 0;
	// Removes the old m_hDoorBlocker and creates a blocker entity over the forward or back bounds (bool selects m_vecBackBounds*). Named by what it does
	virtual void CreateDoorBlocker( CBaseEntity *pOwner, bool bBack ) = 0;
	virtual void AngularMove( const QAngle &vecDestAngle, float flSpeed ) = 0;
	virtual void CalcOpenAngles() = 0;
	// Rotation from m_angRotationClosed by m_flAjarAngle about the door axis. Named by what it does
	virtual QAngle GetAjarAngles() = 0;
	// Returns -1 in every class
	virtual int Stub_348() = 0;
};

#endif // BASEPROPDOOR_H
