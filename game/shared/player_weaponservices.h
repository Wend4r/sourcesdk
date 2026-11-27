#ifndef PLAYER_WEAPONSERVICES_H
#define PLAYER_WEAPONSERVICES_H

#pragma once

#include "playerpawn_component.h"
#include "mathlib/vector.h"

class CBasePlayerWeapon;

//-----------------------------------------------------------------------------
// CPlayer_WeaponServices, slots 23-44, on the client 30-47. See
// playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerPawnComponentVTable >
abstract_class CPlayer_WeaponServicesVTable : public TBase
{
public:
	// Run after the pawn has sent the ResetHUD message to its client. Empty in the server classes.
	virtual void OnResetHUD() = 0;

	// Sets m_hActiveWeapon. CCSPlayer_WeaponServices also passes the weapon to the pawn's animation graph.
	virtual void SetActiveWeapon( CBasePlayerWeapon *pWeapon ) = 0;

	// The pawn's eye position and eye angles. CCSPlayer_WeaponServices adds the aim punch to the angles.
	virtual Vector GetShootPosition() = 0;
	virtual QAngle GetShootAngles() = 0;

	// Whether the pawn may pick the weapon up. The base class allows everything.
	virtual bool CanUse( CBasePlayerWeapon *pWeapon ) = 0;

	// Implemented by CCSPlayer_WeaponServices only
	virtual void DropWeapon( CBasePlayerWeapon *pWeapon, Vector *pVecTarget = nullptr, Vector *pVelocity = nullptr ) = 0;

#ifndef CLIENT_DLL
	// The pawn touched a weapon on the ground. CCSPlayer_WeaponServices returns 0, 1 or 2; the base class returns 0.
	virtual int BumpWeapon( CBasePlayerWeapon *pWeapon ) = 0;
#endif

	// Makes the weapon the active one. Implemented by CCSPlayer_WeaponServices only.
	virtual bool SelectItem( CBasePlayerWeapon *pWeapon, int iSubType = 0 ) = 0;

	// m_hLastWeapon, null once the pawn no longer owns it
	virtual CBasePlayerWeapon *GetLastWeapon() = 0;
	virtual void SetLastWeapon( CBasePlayerWeapon *pWeapon ) = 0;

	// Asked by SelectItem: whether the weapon being put away becomes the last weapon. Always true.
	virtual bool ShouldSetLastWeapon( CBasePlayerWeapon *pOldWeapon, CBasePlayerWeapon *pNewWeapon ) = 0;

#ifndef CLIENT_DLL
	// False while m_bPreventWeaponPickup is set; CCSPlayer_WeaponServices also requires the pawn to be alive
	virtual bool CanPickupWeapons() = 0;
#endif

	// Empty on the server and on the client. Called when a weapon handle is appended to
	// m_hMyWeapons (from EquipWeapon). Named by its caller
	virtual void OnWeaponAdded( CBasePlayerWeapon *pWeapon ) = 0;

	// The weapon is being taken away from the pawn: clears the active weapon if this is the one
	virtual void OnDetachWeapon( CBasePlayerWeapon *pWeapon ) = 0;

	// Empty in every class, on the client too; no caller on the server or the client
	virtual void Stub_37() = 0;
	virtual void Stub_38() = 0;

	// Empty on the server; the client tells the HUD about the active weapon of the local player
	virtual void UpdateActiveWeaponHUD() = 0;
	virtual void RemoveAmmo( int iCount, int8 nAmmoIndex ) = 0;

	// Forwards the ammo index to the ammo definition
	virtual bool CanCarryInfiniteAmmo( uint8 nAmmoIndex ) = 0;

#ifndef CLIENT_DLL
	// Returns 0 in every class; no caller found, the client has no such slot
	virtual float Stub_42() = 0;

	// Empty in every class; no caller found, the client has no such slot
	virtual void Stub_43() = 0;
#endif

	// Tops up the clips of the active weapon from its reserve ammo, depending on a console variable
	virtual void RefillActiveWeaponClips() = 0;
};

#endif // PLAYER_WEAPONSERVICES_H
