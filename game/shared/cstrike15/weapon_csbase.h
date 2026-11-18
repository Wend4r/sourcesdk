#ifndef CSTRIKE15_WEAPON_CSBASE_H
#define CSTRIKE15_WEAPON_CSBASE_H

#pragma once

#include "../baseplayerweapon.h"
#include "../ibonetransformmodifier.h"

class CCS2WeaponGraphController;

//-----------------------------------------------------------------------------
// CCSWeaponBase adds 78 methods to CBasePlayerWeapon, Windows slots 359-436. The Linux
// table has 81, slots 367-447: 443 and 444 override methods of IBoneTransformOverride, a second
// base of CEconEntity, and 445 a method of IBoneTransformModifier, the second base this class
// adds (RTTI). The Itanium ABI also enters such an override into the primary table, MSVC does
// not. The second base sits at offset 32 here, not where the game has it (Linux 4552, Windows
// 3824 bytes into the entity): do not convert a pointer to it.
//
// The getters named after a member return that member of CCSWeaponBaseVData; nMode is the
// firing mode, 0 or 1.
//-----------------------------------------------------------------------------
template < class TBase = CBasePlayerWeaponVTable<> >
abstract_class CCSWeaponBaseVTable : public TBase, public IBoneTransformModifier
{
public:
	// Returns 0; the taser returns how far it has recharged, 1 when it can fire. Named by what it does
	virtual float GetChargeFraction() = 0;
	// Returns true; the bomb and the molotov return false, the incendiary true again, the same on the client. Asked only by CanRedeploy. Named by what it does. Asked by SelectItem of the weapon services
	virtual bool AllowsRedeploy() = 0;
	// AllowsRedeploy and CanHolster; SelectItem of the weapon services asks it when the selected weapon is already the active one and, for sub type 4, holsters and deploys it again. Never overridden. Named by what it does. Asked by SelectItem of the weapon services
	virtual bool CanRedeploy() = 0;
	// Empty; overridden by CCSWeaponBaseGun. The arguments are the ones of Source 1, not checked
	virtual void BulletWasFired( const Vector &vecStart, const Vector &vecEnd ) = 0;
	// True for a weapon without an owner; overridden by the bomb
	virtual bool ShouldRemoveOnRoundRestart() = 0;
	// Empty; the CZ75a puts its magazine back
	virtual void OnRoundRestart() = 0;
	// By m_bRemoveable and the time; overridden by the bomb
	virtual bool IsRemoveable() = 0;
	// Removes a weapon nobody owns; the profiler string is WpnCsBase_RemoveUnownedWpn
	virtual void RemoveUnownedWeaponThink() = 0;
	// Empty in every class. Returns its argument
	virtual float Stub_367( float flUnknown ) = 0;
	// Returns its Vector argument in every class, on the client too
	virtual Vector Stub_368( Vector vecValue ) = 0;
	// m_nPrice
	virtual int GetWeaponPrice() = 0;
	// m_bIsFullAuto
	virtual bool IsFullAuto() = 0;
	// IsFullAuto and not IsInBurstMode: true for the rifles and submachine guns that fire automatically now. Asked when a shot is fired. Named by what it does
	virtual bool IsInFullAutoMode() = 0;
	// m_nDamage
	virtual int GetDamage() = 0;
	// m_nKillAward
	virtual int GetKillAward() = 0;
	// m_flCycleTime
	virtual float GetCycleTime( int nMode ) = 0;
	// m_flArmorRatio
	virtual float GetArmorRatio() = 0;
	virtual float GetInaccuracyCrouch( int nMode ) = 0;
	virtual float GetInaccuracyStand( int nMode ) = 0;
	virtual float GetInaccuracyJump( int nMode ) = 0;
	virtual float GetInaccuracyLand( int nMode ) = 0;
	virtual float GetInaccuracyLadder( int nMode ) = 0;
	virtual float GetInaccuracyFire( int nMode ) = 0;
	virtual float GetInaccuracyMove( int nMode ) = 0;
	// m_bCannotShootUnderwater
	virtual bool CannotShootUnderwater() = 0;
	virtual float GetRecoilMagnitude( int nMode ) = 0;
	virtual float GetRecoilMagnitudeVariance( int nMode ) = 0;
	virtual float GetRecoilAngle( int nMode ) = 0;
	virtual float GetRecoilAngleVariance( int nMode ) = 0;
	// m_flMaxSpeed for the current m_weaponMode; overridden by the bomb
	virtual float GetMaxSpeed() = 0;
	// m_nZoomLevels
	virtual int GetZoomLevels() = 0;
	// m_bUnzoomsAfterShot
	virtual bool DoesUnzoomAfterShot() = 0;
	// Out of m_aShootSounds; never overridden
	virtual const char *GetZoomInSound() = 0;
	// Out of m_aShootSounds; never overridden
	virtual const char *GetZoomOutSound() = 0;
	// m_flThrowVelocity
	virtual float GetThrowVelocity() = 0;
	// m_flSpread for the current m_weaponMode; never overridden
	virtual float GetSpread() = 0;
	// m_flAttackMovespeedFactor
	virtual float GetAttackMovespeedFactor() = 0;
	// m_WeaponType, a CSWeaponType; overridden by the taser
	virtual int GetWeaponType() = 0;
	// Returns 1, never overridden
	virtual float GetRunBobScale() = 0;
	// m_nZoomFOV1 or m_nZoomFOV2
	virtual int GetZoomFOV( int nZoomLevel ) = 0;
	// m_flZoomTime1 or m_flZoomTime2
	virtual float GetZoomTime( int nZoomLevel ) = 0;
	// The name of the item definition; never overridden
	virtual const char *GetDefinitionName() = 0;
	// m_nRecoilSeed
	virtual int GetRecoilSeed() = 0;
	// Returns false; CCSWeaponBaseGun tests m_zoomLevel
	virtual bool IsZoomed() = 0;
	// Tests m_iFlags of the weapon data; never overridden
	virtual bool HasFlag( int nFlag ) = 0;
	// Returns false; the CZ75a returns m_bMagazineRemoved. Asked every tick. Named after the member
	virtual bool IsMagazineRemoved() = 0;
	// The weapon info found by the definition index of the item; CCSWeaponBaseGun caches it. Named by the assert of the client (C_CSWeaponBase::GetEconWpnData, "Weapon '%s' script file not found")
	virtual const void *GetEconWpnData() = 0;
	// Returns 0; CCSWeaponBaseGun returns m_zoomLevel
	virtual int GetCSZoomLevel() = 0;
	// Overridden by the grenades and the bomb
	virtual void OnPickedUp( CBaseEntity *pNewOwner ) = 0;
	// The landing kick of the view; never overridden
	virtual void OnLand( float flVelocity ) = 0;
	// Returns false; CCSWeaponBaseGun tests m_nZoomLevels
	virtual bool HasZoom() = 0;
	// Returns null or false in every class. Also false on the client. EquipWeapon of the weapon services switches to the new weapon only when this is false for the active one. Asked by EquipWeapon of the weapon services about the active weapon when it decides whether to switch to the new one
	virtual bool Stub_410() = 0;
	// Returns null or false in every class. Also false on the client. EquipWeapon of the weapon services asks it about the new weapon when it decides whether to switch to it. Asked by EquipWeapon of the weapon services about the new weapon when it decides whether to switch to it
	virtual bool Stub_411() = 0;
	// Returns true; CBaseCSGrenade refuses during the throw animation (m_bThrowAnimating) and keeps the weapon in m_hSwitchToWeaponAfterThrow. SelectItem asks the active weapon with the weapon to switch to; a refusal stops the switch. Named by what it does. Asked by SelectItem of the weapon services about the weapon the player leaves
	virtual bool AllowsSwitchTo( CBasePlayerWeapon *pNewWeapon ) = 0;
	// Returns false; CBaseCSGrenade answers whether m_hSwitchToWeaponAfterThrow is the grenade itself, and SelectItem then deploys the active weapon again. Named by what it does. Asked by SelectItem of the weapon services
	virtual bool IsSwitchingToSelfAfterThrow() = 0;
	// Returns false; overridden by CCSWeaponBaseGun, the taser, the grenades, the bomb, the knife and the items
	virtual bool PrimaryAttack() = 0;
	// Returns false; overridden by CCSWeaponBaseGun, the grenades and the knife
	virtual bool SecondaryAttack() = 0;
	// Overridden by CCSWeaponBaseGun, the taser, the grenades, the bomb and the items
	virtual void ItemPostFrame() = 0;
	// Overridden by CCSWeaponBaseGun
	virtual void ItemBusyFrame() = 0;
	// Empty; overridden by the grenades and the bomb
	virtual void WeaponReset() = 0;
	// Returns false; CCSWeaponBaseGun returns m_bHasBurstMode of the weapon data
	virtual bool WeaponHasBurst() = 0;
	// m_bBurstMode; never overridden
	virtual bool IsInBurstMode() = 0;
	// Returns false; CCSWeaponBaseGun returns m_bIsRevolver of the weapon data
	virtual bool IsRevolver() = 0;
	// Returns 0; CCSWeaponBaseGun returns m_nRevolverCylinderIdx. Named after the member
	virtual int GetRevolverCylinderIdx() = 0;
	// Empty; CCSWeaponBaseGun moves m_nRevolverCylinderIdx back by one (wrapping to 7) when the argument is set, else sets it to the primary clip, the same on the client. Named by what it does
	virtual void UpdateRevolverCylinderIdx( bool bShotFired ) = 0;
	// What Holster does for a Counter-Strike weapon; overridden by the grenades, the bomb and the items. Called by SelectItem, EquipWeapon and OnDetachWeapon of the weapon services. Named by what it does
	virtual void HolsterWeapon( CBasePlayerWeapon *pSwitchingTo ) = 0;
	// Asks DefaultReload first; overridden by CCSWeaponBaseGun, the shotguns, the grenades and the items
	virtual bool Reload() = 0;
	// Sets m_bInReload; never overridden. The arguments are the ones of Source 1, not checked
	virtual bool DefaultReload( int nClipSize1, int nClipSize2, int nActivity ) = 0;
	// False while reloading, while the silencer is switched, for a grenade with the pin pulled and always for the healthshot. Named by what it does. Asked by ItemPostFrame before the inspect animation starts (m_bInspectPending)
	virtual bool CanInspect() = 0;
	// Called by Touch: lets a pawn pick the weapon up when m_bCanBePickedUp, m_nextOwnerTouchTime and m_hPrevOwner allow it, as CWeaponCSBase::DefaultTouch of Counter-Strike: Global Offensive; never overridden. Asked by Touch
	virtual void DefaultTouch( CBaseEntity *pOther ) = 0;
	// Never overridden
	virtual float GetInaccuracy() = 0;
	// Adds the penalties of the stance, the ladder and the movement; never overridden
	virtual void UpdateAccuracyPenalty() = 0;
	// Applies what the animation graph of the weapon reported for this command, read out of the weapon graph controller: the silencer on or off, the silent part of a reload, the looping inspect and others; overridden by CWeaponBaseItem. Called on every command of the owner. Named by what it does
	virtual void ProcessAnimGraphFlags( CCS2WeaponGraphController *pController ) = 0;
	// Moves the ammo into the clip through MoveReserveAmmoToClip, one shell or the whole clip, clears m_flRecoilIndex and moves m_iWeaponGameplayAnimState on, as CBaseCombatWeapon::FinishReload of Source 1; overridden by the CZ75a. Called while a reload finishes
	virtual void FinishReload() = 0;
	// Ends the reload, the inspect and the other states of m_iWeaponGameplayAnimState: clears m_bInReload and m_bIsInStealthReload and goes back to state 50; overridden by the grenades and the items. Named by what it does. Called while a reload finishes
	virtual void FinishGameplayAnimState() = 0;
	// m_bSilencerOn; never overridden
	virtual bool IsSilenced() = 0;
	// Named by Dota 2 2017 (DWARF) and its caller, which sizes the scratch memory of the bone setup by it. Returns 0. Overrides IBoneTransformOverride: a slot of this table on Linux only
	virtual int GetScratchMemorySize( const CSkeletonInstance *pInstance ) const override = 0;
	// Named by Dota 2 2017 (DWARF). Overrides the bones of the dual pistols (elite_holster, weapon_l) and the magazine of the CZ75a (magazine2); returns BONE_TRANSFORM_SPECIFIED_IN_WORLD_SPACE when it wrote a world space transform. Overrides IBoneTransformOverride: a slot of this table on Linux only
	virtual BoneTransformOverrideResult_t GetBoneTransformOverride( const CSkeletonInstance *pInstance, int nBoneIndex, const CBoneAccessor< CTransform > &parentSpaceBones, const CBoneAccessor< CTransform > &worldSpaceBones, bool *pScratchMemoryValid, void *pScratchMemory, CTransform *pTransform ) const override = 0;
	// Moves the silencer and hand bones (weapon_m4a1_silencer, weapon_usp_silencer, weapon_hand_r) after the bone transform override. Named by what it does. Overrides IBoneTransformModifier: a slot of this table on Linux only
	virtual bool ModifyBoneTransform( int nParentBone, int nBone, CTransform *pTransform ) override = 0;
	// Returns true; the grenades and CItem_Healthshot return false, and Drop then removes the weapon when its reserve is empty, as CBaseCombatWeapon::RemoveIfZeroAmmo. Named by the sources of Counter-Strike: Global Offensive. Asked by Drop
	virtual bool IsZeroAmmoStateValid() const = 0;
	// Returns true in every class
	virtual bool Stub_436() = 0;
};

#endif // CSTRIKE15_WEAPON_CSBASE_H
