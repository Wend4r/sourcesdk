#ifndef PLAYER_MOVEMENTSERVICES_H
#define PLAYER_MOVEMENTSERVICES_H

#pragma once

#include "playerpawn_component.h"
#include "mathlib/vector.h"

class CUserCmdBase;
class CMoveData;
class CPhysSurfaceProperties;

//-----------------------------------------------------------------------------
// CPlayer_MovementServices, slots 23-50, on the client 30-53. See
// playerpawn_component.h.
//
// The class runs the user commands of a pawn. RunCommand calls, in this order:
// StartCommand, CanBeFrozen, CheckMovingGround, GetResetMoveData, SetupMove, then for
// every subtick step PreSubtickMove, ProcessMovement and PostSubtickMove, then
// FinishMove, UpdateInputState, PostCommand and FinishCommand.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerPawnComponentVTable >
abstract_class CPlayer_MovementServicesVTable : public TBase
{
public:
	// Takes the buttons of the command into the button state of the services
	virtual void UpdateInputState( CUserCmdBase *pCmd ) = 0;

	// The last step of a command before FinishCommand. Empty in the base class;
	// CCSPlayer_MovementServices reconciles the jump button here.
	virtual void PostCommand() = 0;

	// Runs one command through StartCommand .. FinishCommand
	virtual void RunCommand( CUserCmdBase *pCmd ) = 0;

#ifndef CLIENT_DLL
	// Takes the commands a client sent for a tick. The third argument is the user command message.
	virtual void ProcessUsercmds( CUserCmdBase **ppCmds, int nCmds, void *pMessage ) = 0;

	// The part of ProcessUsercmds that runs when no command has to be dropped
	virtual void ProcessUsercmdsInternal( CUserCmdBase **ppCmds, int nCmds, void *pMessage ) = 0;
#endif

	// Implemented by CCSPlayer_MovementServices and CCSObserver_MovementServices
	virtual void ProcessMovement( CMoveData *pMove ) = 0;

#ifndef CLIENT_DLL
	// The pawn's velocity; CPlayer_MovementServices_Humanoid returns m_vecSmoothedVelocity
	virtual Vector GetSmoothedVelocity() = 0;
#endif

	// Returns true in every class, on the client too; no caller on the server or the client
	virtual bool Stub_30() = 0;

#ifdef CLIENT_DLL
	// Client slot 35. Copies m_vecOldViewAngles into the angles passed when a flag of the pawn
	// is set, or when a check of the pawn's entity index passes and CanBeFrozen returns true;
	// stores the angles passed into m_vecOldViewAngles otherwise. Not overridden.
	virtual void Unk_35( QAngle &angles ) = 0;
#endif

	// The footstep table entry of the pawn: "player" for a humanoid, "t_player" or "ct_player" in CS
	virtual const void *GetFootstepEntry() = 0;

	// CPlayer_MovementServices_Humanoid stores m_groundNormal
	virtual void SetGroundNormal( const Vector &vecNormal ) = 0;

	// The move data of the command, reset to its defaults. Null in the base class.
	virtual CMoveData *GetResetMoveData() = 0;
	virtual void SetupMove( CUserCmdBase *pCmd, CMoveData *pMove ) = 0;

	// Before the movement of a subtick step. Empty in the base class;
	// CCSPlayer_MovementServices clears the per-step outputs of the move data.
	virtual void PreSubtickMove( CMoveData *pMove ) = 0;

	// Run once the subtick moves of the command have been copied into the move data.
	// Empty in the base class; CCSPlayer_MovementServices works on the active weapon of
	// the pawn or of the player it observes.
	virtual void OnSubtickMovesSetup( CUserCmdBase *pCmd, CMoveData *pMove ) = 0;

	// Asked once before the subtick steps of a command. False in the base class,
	// sv_subtick_movement_view_angles in CCSPlayer_MovementServices.
	virtual bool ShouldUseSubtickViewAngles() = 0;
	virtual void FinishMove( CUserCmdBase *pCmd, CMoveData *pMove ) = 0;

	// After the movement of a subtick step. Empty in the base class.
	virtual void PostSubtickMove( CMoveData *pMove ) = 0;

	// Called by FinishMove. Moves the view angles of the move data into the pawn's
	// v_angle, keeping the previous ones.
	virtual void SetViewAngles( CUserCmdBase *pCmd, CMoveData *pMove ) = 0;

	// Run after the button changes of one moment within the tick have been applied.
	// Empty in the base class; CCSPlayer_MovementServices forwards to the weapon services.
	virtual void OnSubtickButtonsApplied( CMoveData *pMove ) = 0;

	// Set and clear the prediction random seed of the command
	virtual void StartCommand( CUserCmdBase *pCmd ) = 0;
	virtual void FinishCommand() = 0;

	// Asked while the world is frozen: true keeps the command from running. True in the
	// base class; CPlayer_MovementServices_Humanoid lets a noclipping pawn move with sv_cheats,
	// CCSObserver_MovementServices always moves.
	virtual bool CanBeFrozen() = 0;

	// Adds the velocity of a conveyor the pawn stands on to its base velocity
	virtual void CheckMovingGround( double flFrameTime ) = 0;

	// Returns false in every class, on the client too; no caller on the server or the client
	virtual bool Stub_46() = 0;

	// Free movement through the world. A value of zero or less takes sv_noclipspeed,
	// sv_noclipaccelerate or sv_noclipfriction.
	virtual void FullNoClipMove( CMoveData *pMove, float flMaxSpeed, float flAcceleration, float flFriction, bool bUnscaled ) = 0;

	// Whether the hull of the pawn fits at the position, or at the origin of the move data
	// when it is null. Returns false in the base class.
	virtual bool CanFitAtPosition( CMoveData *pMove, const Vector *pVecPosition, bool bUnknown ) = 0;

#ifndef CLIENT_DLL
	// Looks for a position around the pawn that it fits into; used when noclip is turned off
	virtual bool FindPassableSpace( Vector &vecOrigin ) = 0;

	// The last step of turning noclip on or off. Empty in the server classes.
	virtual void OnNoclipChanged( bool bUnknown ) = 0;
#endif
};

//-----------------------------------------------------------------------------
// CPlayer_MovementServices_Humanoid, slots 51-57, on the client 54-61: the step
// sounds of the Source 1 CBasePlayer.
//-----------------------------------------------------------------------------
template < class TBase = CPlayer_MovementServicesVTable<> >
abstract_class CPlayer_MovementServices_HumanoidVTable : public TBase
{
public:
	// Sets the collision bounds of the pawn to its standing or ducked hull. Run from Reset
	// and from the PostThink of CCSPlayer_MovementServices.
	virtual void UpdateCollisionBounds() = 0;

	// The "ladder" surface; CCSPlayer_MovementServices looks m_nLadderSurfacePropIndex up first
	virtual const CPhysSurfaceProperties *GetLadderSurface( const Vector &vecOrigin ) = 0;
	virtual void UpdateStepSound( const CPhysSurfaceProperties *pSurface, const Vector &vecOrigin, const Vector &vecVelocity ) = 0;
	virtual void PlayStepSound( const Vector &vecOrigin, const CPhysSurfaceProperties *pSurface, float flVolume, bool bForce ) = 0;
	virtual void GetStepSoundVelocities( float *pVelWalk, float *pVelRun ) = 0;
	virtual void SetStepSoundTime( int iStepSoundTime, bool bWalking ) = 0;

#ifdef CLIENT_DLL
	// Client slot 60. Looks a sound event up by its name: hashes the lower-cased name and
	// asks the sound system for the hash. CCSPlayer_MovementServices overrides it.
	virtual void *Unk_60( void *pUnknown, const char *pszName ) = 0;
#endif

	// Empty in every class, on the client too; no caller on the server or the client
	virtual void Stub_57() = 0;
};

#endif // PLAYER_MOVEMENTSERVICES_H
