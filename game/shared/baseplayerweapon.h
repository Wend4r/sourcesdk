#ifndef BASEPLAYERWEAPON_H
#define BASEPLAYERWEAPON_H

#pragma once

#include "econ/econ_entity.h"

//-----------------------------------------------------------------------------
// CBasePlayerWeapon adds 44 methods to CEconEntity: Linux slots 323-366, Windows 315-358.
//
// The mode of a method is 0 for the primary and 1 for the secondary ammo and clip; a weapon
// whose data has m_bTreatAsSingleClip uses the primary ones for both.
//-----------------------------------------------------------------------------
template < class TBase = CEconEntityVTable<> >
abstract_class CBasePlayerWeaponVTable : public TBase
{
public:
	// Returns false; CCSWeaponBase returns true, the same on the client
	virtual bool IsPredicted() const = 0;
	// Stops the weapon, makes it follow the owner and sets the owner entity
	virtual void Equip( CBaseEntity *pOwner ) = 0;
	// Overridden by CCSWeaponBase, CCSWeaponBaseGun and the bomb
	virtual void Drop( const Vector &vecVelocity ) = 0;
	// Empty in every class. On the client too. Called first by Drop and by the helper that removes a weapon (clears the touch function and thinks SUB_Remove), which CPlayer_ItemServices::RemoveAllItems uses for every weapon; OnRemovedFromOwner would be a guess
	virtual void Stub_318() = 0;
	// Returns true in every class. Returns true, and Equip then calls VPhysicsDestroyObject. Asked by Equip
	virtual bool Stub_319() = 0;
	// Returns true; CCSWeaponBase asks VisibleInWeaponSelection when the weapon has an owner
	virtual bool CanBeSelected() = 0;
	// Returns true; CCSWeaponBaseGun refuses in burst mode unless the weapon data has m_bAllowBurstHolster
	virtual bool CanHolster() = 0;
	// Returns true; CCSWeaponBase requires an owner
	virtual bool CanDeploy() = 0;
	// Returns false; overridden by CCSWeaponBase, CCSWeaponBaseGun and the grenades
	virtual bool Deploy() = 0;
	// Empty; CCSWeaponBase passes it to a method of its own
	virtual void Holster( CBasePlayerWeapon *pSwitchingTo ) = 0;
	// By EF_NODRAW
	virtual void SetWeaponVisible( bool bVisible ) = 0;
	virtual bool IsWeaponVisible() = 0;
	// m_bAutoSwitchTo of the weapon data; never overridden
	virtual bool AllowsAutoSwitchTo() = 0;
	// m_bAutoSwitchFrom of the weapon data; overridden by the grenades
	virtual bool AllowsAutoSwitchFrom() = 0;
	// Returns true; the grenades look at their ammo. Asked by CanBeSelected of CCSWeaponBase
	virtual bool VisibleInWeaponSelection() = 0;
	// True when a mode uses ammo and the flag 1 of the weapon data is not set; never overridden. Named by what it does
	virtual bool RequiresAmmoToBeSelected() = 0;
	// CCSWeaponBase returns m_szName of the weapon data
	virtual const char *GetName() = 0;
	// m_szWorldModel of the weapon data
	virtual const char *GetWorldModel() = 0;
	// m_iFlags of the weapon data; never overridden
	virtual int GetWeaponFlags() = 0;
	// Asks the weapon data
	virtual int GetSlot() = 0;
	// Asks the weapon data
	virtual int GetPosition() = 0;
	// Called by HandleAnimEvent; empty, CCSWeaponBase handles the events.
	virtual void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseEntity *pOperator ) = 0;
	// True when the secondary mode has an ammo type
	virtual bool UsesSecondaryAmmo() = 0;
	// m_nPrimaryAmmoType or m_nSecondaryAmmoType of the weapon data, 255 for none; never overridden
	virtual uint8 GetAmmoType( int nMode ) = 0;
	// True when the mode has an ammo type; never overridden
	virtual bool UsesAmmo( int nMode ) = 0;
	// True when the mode has ammo in the clip or in reserve; never overridden
	virtual bool HasAmmo( int nMode ) = 0;
	// True when no mode in use has ammo left; never overridden. Asked by the bots. Named by what it does
	virtual bool IsOutOfAmmo() = 0;
	// m_iMaxClip1 or m_iMaxClip2 of the weapon data; never overridden
	virtual int GetMaxClip( int nMode ) = 0;
	// m_iDefaultClip1 or m_iDefaultClip2 of the weapon data; never overridden
	virtual int GetDefaultClip( int nMode ) = 0;
	// m_iClip1 or m_iClip2; never overridden
	virtual int GetClip( int nMode ) = 0;
	// Does nothing for a mode without a clip; never overridden
	virtual void SetClip( int nMode, int nCount ) = 0;
	// Adds to the clip through SetClip, not above the clip size; a shotgun shell and the recharge of the taser come here. Never overridden. Named by what it does. Called when a shotgun loads a shell
	virtual void AddToClip( int nMode, int nCount ) = 0;
	// Takes from the clip through SetClip; called for every shot. Never overridden. Named by what it does. Called when a shot is fired and by BumpWeapon
	virtual void TakeFromClip( int nMode, int nCount ) = 0;
	// Empty in every class, on the client too; SetClip calls it after it stored a different clip value, with the mode, the old and the new count. Named by its caller. Called by SetClip
	virtual void OnClipChanged( int nMode, int nOldCount, int nNewCount ) = 0;
	// A flag of the ammo type of the mode: the reserve is kept in the weapon, not in the owner; never overridden. Asked by GetReserveAmmoCount and GetReserveAmmoMax. Named by what it does
	virtual bool StoresReserveAmmoInWeapon( int nMode ) = 0;
	// m_pReserveAmmo of the weapon or the count of the owner; never overridden
	virtual int GetReserveAmmoCount( int nMode ) = 0;
	// Changes the reserve by a difference; never overridden. Called by GiveDefaultItems and by WeaponReset of the grenades
	virtual int GiveReserveAmmo( int nMode, int nCount, bool bSuppressSound ) = 0;
	// Returns true; the dual pistols agree when two rounds are missing. Named by what it does. Asked by RefillActiveWeaponClips of the weapon services when sv_infinite_ammo is 1: whether the clip of the mode is refilled
	virtual bool ShouldRefillClip( int nMode ) = 0;
	// Returns true; the grenades and the healthshot agree only when sv_infinite_ammo is 1. Named by what it does. Asked by RefillActiveWeaponClips of the weapon services when sv_infinite_ammo is set (deathmatch has 2): whether the reserve of the mode is refilled
	virtual bool ShouldRefillReserveAmmo( int nMode ) = 0;
	// Empty in every class, on the client too; GiveReserveAmmo calls it when the reserve of the mode changed, with the old and the new count, before the pickup sound. Named by its caller
	virtual void OnReserveAmmoChanged( int nMode, int nOldCount, int nNewCount ) = 0;
	// CCSWeaponBase returns m_nPrimaryReserveAmmoMax or m_nSecondaryReserveAmmoMax of the weapon data
	virtual int GetReserveAmmoMax( int nMode ) = 0;
	// The SetReserveAmmoAmount input calls it; never overridden
	virtual void SetReserveAmmoCount( int nMode, int nCount, bool bUnknown ) = 0;
	// Moves ammo from the reserve into the clip and returns how much; called while a reload finishes. Never overridden. Named by what it does
	virtual int MoveReserveAmmoToClip( int nMode, int nCount ) = 0;
	// m_pMovementServices of the owner pawn; never overridden. Named by what it does. Asked on every command of the owner
	virtual void *GetOwnerMovementServices() = 0;
};

#endif // BASEPLAYERWEAPON_H
