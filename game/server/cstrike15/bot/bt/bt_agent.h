#ifndef BT_AGENT_H
#define BT_AGENT_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"

class CBaseEntity;
class CBasePlayerWeapon;
class CCSBot;
class CCSPlayerPawn;
class CCSWeaponBase;
class CNavArea;

// A record of an entity the bot has sensed, see bt_decorator.h.
struct BtSensedEntity_t;

//-----------------------------------------------------------------------------
// What a behaviour tree sees of the bot it drives. The game implements it with CBtAgentBot,
// a thin wrapper that holds the CCSBot after its vtable pointer: nearly every method is one
// call or one field access on the bot, its pawn or the pawn's services.
//
// The methods are in declaration order, which is the order of the Linux table (107 slots).
// The Windows table has 106: the destructor takes one slot there, so a method sits one slot
// lower on Windows, except that the two overloads of IsOtherEnemy and of IsVisible swap places
// on Windows, as the compiler lays overloads out in reverse. The numbers in the Unk_ names are
// Windows slots.
//
// Most names are those of the bot or pawn method the wrapper calls; where the wrapper reads a
// field, the name says what it returns. Unk_ methods are the ones whose purpose is not known;
// the comment says what the game's implementation does.
//-----------------------------------------------------------------------------
abstract_class IBtAgent
{
public:
	virtual ~IBtAgent() = 0;

	virtual CCSBot *GetBot() = 0;
	virtual CCSPlayerPawn *GetPawn() = 0;
	// -1 without an entity.
	virtual int GetEntityIndex() = 0;
	// The world group of the pawn's entity; the game time of that group is read with it.
	virtual uint32 GetWorldGroupId() = 0;
	// The name stored in the pawn's entity identity, or its entity class name when that is empty.
	virtual const char *GetDebugName() = 0;
	virtual int GetTeamNumber() = 0;
	// The Supports methods answer whether the agent can do what the methods after them do; the
	// behaviour tree nodes ask them in Init and warn "'%s': <what> is not supported by this agent."
	// otherwise. CBtAgentBot returns true from all of them.
	virtual bool SupportsPurchasing() = 0;
	// The money of the bot's controller; 0 without a controller.
	virtual int GetAccount() = 0;
	// Runs the client command "buy <pszItem>" on the pawn.
	virtual void Buy( const char *pszItem ) = 0;
	// True when the pawn has no ground entity and a test of its movement services fails
	// (CBtNodeConditionIsAirborne).
	virtual bool IsAirborne() = 0;
	// Until the flashbang blindness ends.
	virtual bool IsBlind() = 0;
	// How much of the blindness is left: 1 when it starts, down to 0 when it ends; 0 when not
	// blind. The bot service writes it to the blackboard as "BlindnessPercentage" (times 100).
	virtual float GetBlindnessFraction() = 0;
	// True when pPawn is controlled by a bot and is the agent's own pawn; the sensors skip the
	// players it answers true for.
	virtual bool IsSelf( CCSPlayerPawn *pPawn ) = 0;
	// Compare the teams, mp_teammates_are_enemies makes everyone else an enemy.
	virtual bool IsOtherEnemy( CBaseEntity *pEntity ) = 0;
	virtual bool IsOtherEnemy( const BtSensedEntity_t &entity ) = 0;
	virtual bool IsInBombZone() = 0;
	// 0 to 7: the override of the bot, or one derived from the skill of its profile.
	virtual int GetSkillLevel() = 0;
	// The skill of the bot's profile, 0 to 1.
	virtual float GetSkill() = 0;
	virtual float GetCombatRange() = 0;
	virtual int GetHealth() = 0;
	virtual const Vector &GetAbsOrigin() = 0;
	virtual Vector WorldSpaceCenter() = 0;
	virtual void EyeVectors( Vector *pForward, Vector *pRight, Vector *pUp ) = 0;
	virtual QAngle EyeAngles() = 0;
	virtual const Vector &GetEyePosition() = 0;
	virtual void CommitSuicide() = 0;
	virtual void ClearMovement() = 0;
	virtual void MoveForward() = 0;
	virtual void MoveBackward() = 0;
	virtual void Teleport( const Vector *pNewPosition, const QAngle *pNewAngles, const Vector *pNewVelocity ) = 0;
	virtual bool SupportsJumping() = 0;
	virtual bool Jump( bool bMustJump ) = 0;
	virtual bool SupportsCrouching() = 0;
	virtual void Crouch() = 0;
	virtual bool IsCrouching() = 0;
	virtual bool SupportsStandingUp() = 0;
	virtual void StandUp() = 0;
	virtual bool SupportsWalking() = 0;
	virtual void Walk() = 0;
	virtual bool SupportsRunning() = 0;
	virtual void Run() = 0;
	virtual bool SupportsPrimaryAttack() = 0;
	virtual void PrimaryAttack() = 0;
	virtual bool SupportsSecondaryAttack() = 0;
	virtual void SecondaryAttack() = 0;
	virtual bool SupportsReloading() = 0;
	virtual void Reload() = 0;
	virtual bool SupportsStrafing() = 0;
	virtual void StrafeAwayFromPosition( const Vector &vecPos ) = 0;
	virtual void StrafeLeft() = 0;
	virtual void StrafeRight() = 0;
	virtual bool SupportsUsingEnvironment() = 0;
	virtual void UseEnvironment() = 0;
	// "Breaking items"
	virtual bool SupportsBreakingItems() = 0;
	virtual void BreakablesCheck() = 0;
	virtual bool IsLookingAtSpot() = 0;
	// "Checking if stuck"
	virtual bool SupportsStuckCheck() = 0;
	virtual void StuckCheck() = 0;
	// "Looking"
	virtual bool SupportsLooking() = 0;
	virtual void ClearLookAt() = 0;
	virtual float GetLookAheadAngle() = 0;
	// "Inventory": asked by every node that uses the weapon methods below
	virtual bool SupportsInventory() = 0;
	// The first weapon of the pawn whose slot is nSlot.
	virtual CBasePlayerWeapon *Weapon_GetSlot( int nSlot ) = 0;
	// The weapon of the pawn whose name (the weapon's GetName) is pszName.
	virtual CBasePlayerWeapon *Unk_64( const char *pszName ) = 0;
	// The weapon of the pawn whose class name is pszWeapon.
	virtual CBasePlayerWeapon *Weapon_OwnsThisType( const char *pszWeapon ) = 0;
	virtual bool HasDefuser() = 0;
	virtual bool SelectItem( const char *pszItem ) = 0;
	virtual bool DoEquip( CBasePlayerWeapon *pWeapon ) = 0;
	virtual void DropWeapon( CBasePlayerWeapon *pWeapon ) = 0;
	virtual CCSWeaponBase *GetActiveCSWeapon() = 0;
	// Looks nKey up in a tree map of the active weapon's data.
	virtual bool Unk_71( const uint64 &nKey ) = 0;
	// The field of view of the camera is narrower than its default.
	virtual bool IsZoomed() = 0;
	virtual CBasePlayerWeapon *GetWeapon( int i ) = 0;
	virtual int WeaponCount() = 0;
	// Asks the active weapon to play its inspect action (CBtActionInspectCurrentWeapon); false
	// without an active weapon.
	virtual bool InspectCurrentWeapon() = 0;
	virtual bool CanSeeSniper() = 0;
	// pOther looks towards the agent: the forward of its eye angles plus its view punch points
	// away from the agent's direction to it.
	virtual bool IsPlayerFacingMe( CCSPlayerPawn *pOther ) = 0;
	// Returns true. CBtActionMoveTo asks it before the avoid methods below and skips them on
	// false, like the Supports methods, but no warning names it.
	virtual bool Unk_78() = 0;
	// When the bot started to make way for a higher priority player.
	virtual float GetAvoidTimestamp() = 0;
	// The handle of the player the bot makes way for.
	virtual uint32 GetAvoid() = 0;
	virtual void ClearAvoid() = 0;
	// The active weapon is reloading.
	virtual bool IsReloading() = 0;
	// The active weapon is the weapon_m249.
	virtual bool IsUsingMachinegun() = 0;
	virtual bool IsUsingSniperRifle() = 0;
	// The pitch of GetAimPunchAngle is below -1.5.
	virtual bool IsActiveWeaponRecoilHigh() = 0;
	virtual bool IsUsingShotgun() = 0;
	// "Aiming"
	virtual bool SupportsAiming() = 0;
	// From the pawn's aim punch services.
	virtual QAngle GetAimPunchAngle() = 0;
	// nPart is a CCSBot::VisiblePartType bit or 0; other values warn and use 0.
	virtual const Vector &GetPartPosition( CCSPlayerPawn *pPlayer, int nPart ) = 0;
	virtual void GetAimGoal( float *pPitch, float *pYaw, float *pRoll ) = 0;
	virtual void SetAimGoal( float flPitch, float flYaw, float flRoll ) = 0;
	virtual void AimAtEnemy() = 0;
	virtual void StopAiming() = 0;
	virtual bool IsEyeAnglesUnderPathFinderControl() = 0;
	// While the navigation mesh is loaded: raises *pflHeight to the height of the navigation area
	// at the position when there is one. False when no mesh is loaded.
	virtual bool GetSimpleGroundHeightWithFloor( const Vector &vecPos, float *pflHeight ) = 0;
	// nRoute 0 to 3; other values warn and use 0.
	virtual bool ComputePath( const Vector &vecGoal, int nRoute ) = 0;
	// Returns 0 progressing, 1 end of path, 2 path failure.
	virtual int UpdatePathMovement( bool bAllowSpeedChange ) = 0;
	virtual void DestroyPath() = 0;
	virtual bool IsUsingLadder() = 0;
	virtual bool HasPath() = 0;
	virtual CNavArea *GetLastKnownArea() = 0;
	virtual bool IsVisible( const Vector &vecPos, bool bTestFOV ) = 0;
	virtual bool IsVisible( CCSPlayerPawn *pPlayer, bool bTestFOV ) = 0;
	virtual bool FInViewCone( const Vector &vecSpot ) = 0;
	// 0 none, 1 low, 2 high.
	virtual int GetZoomLevel() = 0;
};

#endif // BT_AGENT_H
