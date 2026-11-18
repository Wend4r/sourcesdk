#ifndef BASEPLAYERPAWN_H
#define BASEPLAYERPAWN_H

#pragma once

#include "basecombatcharacter.h"
#include "../shared/playerpawn_component.h"

class CAI_Expresser;
class CBasePlayerController;
class CCommand;
class CTakeDamageInfo;
struct CTakeDamageResult;
struct ClientReplayEventParams_t;
struct HltvReplayParams_t;
struct vis_info_t;

//-----------------------------------------------------------------------------
// CBasePlayerPawn adds 62 methods to CBaseCombatCharacter: Linux slots 339-400, Windows 338-399.
// The two CommitSuicide swap places on Windows.
//
// SetOrResetInitialState, PrePhysicsSimulate, PreThink, PostThink and
// ModifyOrAppendPlayerCriteria are named by the profiler strings of the methods.
//-----------------------------------------------------------------------------
template < class TBase = CBaseCombatCharacterVTable<> >
abstract_class CBasePlayerPawnVTable : public TBase
{
public:
	// Creates the nine services through the methods below and links them to the pawn
	virtual void CreatePlayerPawnServices() = 0;
	// Deletes the services CreatePlayerPawnServices made; called by Disconnect. CCSPlayerPawnBase and CCSPlayerPawn delete their own first. Named by what it does
	virtual void DestroyPlayerPawnServices() = 0;
	// For m_pWeaponServices; null in the base class
	virtual CPlayerPawnComponentVTable *CreateWeaponServices() = 0;
	// For m_pItemServices; null in the base class
	virtual CPlayerPawnComponentVTable *CreateItemServices() = 0;
	// For m_pAutoaimServices; null in the base class
	virtual CPlayerPawnComponentVTable *CreateAutoaimServices() = 0;
	// For m_pObserverServices; null in the base class
	virtual CPlayerPawnComponentVTable *CreateObserverServices() = 0;
	// For m_pWaterServices; null in the base class
	virtual CPlayerPawnComponentVTable *CreateWaterServices() = 0;
	// For m_pUseServices; null in the base class
	virtual CPlayerPawnComponentVTable *CreateUseServices() = 0;
	// For m_pFlashlightServices; null in the base class
	virtual CPlayerPawnComponentVTable *CreateFlashlightServices() = 0;
	// For m_pCameraServices; null in the base class
	virtual CPlayerPawnComponentVTable *CreateCameraServices() = 0;
	// For m_pMovementServices; null in the base class
	virtual CPlayerPawnComponentVTable *CreateMovementServices() = 0;
	// Adds the eye position of the pawn or of the observed entity to the visibility of the client; called by CSource2GameClients::ClientSetupVisibility with the observer target. Dota 2 2018: CBasePlayer::SetupVisibility( CBaseEntity *, vis_info_t * )
	virtual void SetupVisibility( CBaseEntity *pViewEntity, vis_info_t *pVisInfo ) = 0;
	// Overridden by CCSPlayerPawn
	virtual bool WantsLagCompensationOnEntity( const CBaseEntity *pEntity, const void *pCmd, const void *pEntityTransmitBits ) = 0;
	// The spawn of a player; overridden by CCSPlayerPawnBase and CCSPlayerPawn
	virtual void SetOrResetInitialState() = 0;
	// Empty in every class; a func_mover that teleports to a path node calls it on the pawn of player slot 0 with itself. Named by its caller
	virtual void OnMoverTeleported( CBaseEntity *pMover ) = 0;
	// Never overridden
	virtual void PrePhysicsSimulate() = 0;
	virtual void PreThink() = 0;
	virtual void PostThink() = 0;
	// Empty; CCSPlayerPawn copies EyeAngles into m_angEyeAngles when they differ. Called by ProcessMovement of the movement services. Named by what CCSPlayerPawn does
	virtual void UpdateEyeAngles() = 0;
	// The sound of a hit; never overridden
	virtual void DamageEffect( const CTakeDamageResult &result ) = 0;
	// True when the damage leaves less than one point of health; never overridden. Named by what it does
	virtual bool IsDamageLethal( const CTakeDamageInfo &info ) = 0;
	// Takes the damage off m_iHealth; never overridden. Named by what it does
	virtual void ApplyDamageToHealth( const CTakeDamageInfo &info ) = 0;
	// Empty in every class; the physics impact damage passes it the mass of the pawn's physics body before it divides the impact energy by it. Named by its caller
	virtual void ModifyPhysicsImpactMass( float *pflMass ) = 0;
	// The height of the eyes above the origin, 62.97 for a standing player; never overridden. Named by what it does
	virtual float GetEyeHeight() = 0;
	// The entity of m_hController; CCSPlayerPawnBase falls back to m_hOriginalController
	virtual CBasePlayerController *GetController() = 0;
	// Empty; CCSPlayerPawnBase overrides it with an empty body too. Called by the controller setter of the pawn (from CBasePlayerController::SetPawn) after m_hController changed and after every component got the same call. Named by its caller. Called when the controller takes the pawn (CBasePlayerController::SetPawn)
	virtual void OnControllerChanged( CBasePlayerController *pOldController, CBasePlayerController *pNewController ) = 0;
	// Empty in every class
	virtual void Stub_364() = 0;
	// Returns the pawn itself, never overridden; a console command asks it about the pawn of the player who sent it
	virtual CBaseCombatCharacter *ActivePlayerCombatCharacter() = 0;
	// The height of the eyes: 0 is the current view offset while alive and the dead view height otherwise, 1 standing, 2 ducked, as the view vectors of the game give them; never overridden. Asked by the observer and movement services and by Event_Killed. Named by what it does
	virtual float GetViewHeight( int nStance ) = 0;
	// The minimum of a hull out of the view vectors of the game; asked by UpdateCollisionBounds of the movement services
	virtual Vector GetPlayerMins( int nHull ) = 0;
	// The maximum of a hull; asked by UpdateCollisionBounds of the movement services
	virtual Vector GetPlayerMaxs( int nHull ) = 0;
	// Returns the constant 0xC3011, never overridden
	virtual uint32 PhysicsSolidMaskForEntity() = 0;
	// Returns false; CCSPlayerPawn returns m_bIsWalking
	virtual bool IsWalking() = 0;
	// Refuses with the message of CCSPlayer::StartHltvReplayEvent of Counter-Strike: Global Offensive: the player is active, replaying the last kill would interfere; overridden by CCSPlayerPawn
	virtual bool StartHltvReplayEvent( const ClientReplayEventParams_t &params ) = 0;
	// Empty in every class; StartHltvReplayEvent calls it when the engine started the replay and the event did not force its settings, where CCSPlayer::StartHltvReplayEvent of Counter-Strike: Global Offensive started the replay for the dead teammates. Named by its caller
	virtual void OnHltvReplayStarted( const HltvReplayParams_t &params ) = 0;
	// Clears m_fInitHUD; never overridden
	virtual void UpdateClientData() = 0;
	// Does nothing but ask IsAlive, never overridden; called with RUMBLE_STOP_ALL (0, 0, 0) from SetOrResetInitialState, Activate and Event_Killed. CBasePlayer::RumbleEffect of Source 1 and Dota 2 2018. Called by SetOrResetInitialState, Activate and Event_Killed
	virtual void RumbleEffect( uint8 nIndex, uint8 nRumbleData, uint8 nRumbleFlags ) = 0;
	// Empty; CCSPlayerPawn overrides it with an empty body. Event_Killed calls it with its argument after the components' Event_Killed and before the active weapon is dropped. Dota 2 2018 has CBasePlayer::DeathSound( const CTakeDamageInfo & ) between RumbleEffect and the impulse commands, the order CS2 keeps; only the CS pawn overrides it, as CCSPlayer::DeathSound of Counter-Strike: Global Offensive, and CS2 plays the Player.Death sounds from a non-virtual helper of CCSPlayerPawn::Event_Killed instead. The client has no counterpart. Called by Event_Killed
	virtual void DeathSound( const CTakeDamageResult &result ) = 0;
	// Runs an impulse command, 200 looks at the weapon and the rest goes to CheatImpulseCommands; called by ImpulseCommands. Named by what it does
	virtual void RunImpulseCommand( int nImpulse ) = 0;
	// Empty; CCSPlayerPawn gives everything on impulse 101 when cheats are on
	virtual void CheatImpulseCommands( int nImpulse ) = 0;
	// The base class asks its observer services; CCSPlayerPawnBase and CCSPlayerPawn handle the commands of the game
	virtual bool ClientCommand( const CCommand &args ) = 0;
	// Calls ClientCommand( args ) and drops its first argument; never overridden, no caller on the server, no counterpart on the client (C_BasePlayerPawn has no command methods). Not an overload of ClientCommand: Windows keeps the Linux order. Named by what it does
	virtual bool DispatchClientCommand( void *pUnused, const CCommand &args ) = 0;
	// Runs RunImpulseCommand with the impulse of the last command and clears it; never overridden
	virtual void ImpulseCommands() = 0;
	// Sets m_fInitHUD, calls UpdateClientData and restarts the ambient sounds, as CBasePlayer::ForceClientDllUpdate of Source 1; never overridden
	virtual void ForceClientDllUpdate() = 0;
	// Takes the pawn out of its team, adds it to the new one and calls CBaseEntity::ChangeTeam; CCSPlayerPawnBase ignores team -1. Called by ChangeTeam of the controller and when the controller takes the pawn. Named by what it does
	virtual void ChangePawnTeam( void *pUnknown, int nTeam ) = 0;
	// Returns null or false in every class
	virtual void *Stub_383() = 0;
	virtual void ModifyOrAppendPlayerCriteria( CResponseCriteriaSet &set ) = 0;
	// Sets the solid type, the bounds and the interaction layers of the pawn; never overridden. Called by SetOrResetInitialState. Named by what it does
	virtual void SetupCollision() = 0;
	virtual void CommitSuicide( bool bExplode, bool bForce ) = 0;
	virtual void CommitSuicide( const Vector &vecForce, bool bExplode, bool bForce ) = 0;
	// The FL_BOT bit of m_fFlags; never overridden
	virtual bool IsBot() = 0;
	// False; CCSPlayerPawnBase returns m_hOriginalController != m_hController, another player has taken the pawn over. Asked many times per tick. Named after CCSPlayer::HasControlledByPlayer of Counter-Strike: Global Offensive. Asked while user commands are processed
	virtual bool HasControlledByPlayer() = 0;
	// What IsObserver of the controller answers; without a controller m_iTeamNum >= 2 here, CCSPlayerPawnBase m_iTeamNum == 0 or not alive. Asked by the player filters. Named by what it does
	virtual bool IsObserver() = 0;
	// Returns null; CCSPlayerPawn allocates the expresser
	virtual CAI_Expresser *CreateExpresser() = 0;
	// Returns m_pExpresser, never overridden
	virtual CAI_Expresser *GetExpresser() = 0;
	// Empty in every class; ProcessUsercmds of the controller calls it every time with the previous and the new m_bGamePaused. Named by its caller. Called by ProcessUsercmds of the controller
	virtual void UpdateGamePaused( bool bWasPaused, bool bPaused ) = 0;
	// Never overridden
	virtual bool StartReplayMode( float flDelay, float flDuration, int nEntity ) = 0;
	// Never overridden
	virtual void StopReplayMode() = 0;
	// Never overridden
	virtual int GetDelayTicks() = 0;
	// Never overridden
	virtual int GetReplayEntity() = 0;
	// The SetHealth input; never overridden
	virtual void InputSetHealth( InputData_t *pInput ) = 0;
	// True when the entity is of the same team; CCSPlayerPawnBase also asks the game rules whether teammates are enemies. Named by what it does
	virtual bool IsTeammate( CBaseEntity *pEntity ) = 0;
};

#endif // BASEPLAYERPAWN_H
