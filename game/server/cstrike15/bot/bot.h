//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: The base of every bot brain: the movement and button state it folds into its own
//          user command each frame.
//
//=============================================================================//

#ifndef BOT_H
#define BOT_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "cstrike15/usercmd.h"
#include "schemasystem/schematypes.h"

class BotProfile;
class CCSPlayerController;
class CCSPlayerPawn;

//-----------------------------------------------------------------------------
// The bot manager runs every bot once a frame:
//
//     Upkeep();
//     if ( the think interval is up )
//     {
//         clear the movement and the buttons;
//         Update();
//     }
//     RunCommand();
//
// RunCommand folds the movement fields, the buttons and the pawn's eye angles into
// m_UserCmd and runs it on the controller the way a client's command is run.
//-----------------------------------------------------------------------------
abstract_class CBot
{
public:
	// The "CBot" schema class; CCSBot returns "CCSBot".
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual ~CBot() = 0;

	// Stores the profile and clears the movement state.
	virtual bool Initialize( const BotProfile *pProfile ) = 0;

	// Every frame: where the head is turned, what has been seen.
	virtual void Upkeep() = 0;
	// Every think interval: the state machine that decides what to press.
	virtual void Update() = 0;

	virtual void Run() = 0;
	virtual void Walk() = 0;
	virtual bool IsRunning() const = 0;

	virtual void Crouch() = 0;
	virtual void StandUp() = 0;

	virtual void MoveForward() = 0;
	virtual void MoveBackward() = 0;
	virtual void StrafeLeft() = 0;
	virtual void StrafeRight() = 0;

	// Rate limited by m_flJumpTimestamp unless bMustJump.
	virtual bool Jump( bool bMustJump ) = 0;

	virtual void ClearMovement() = 0;

	virtual void UseEnvironment() = 0;
	virtual void PrimaryAttack() = 0;
	virtual void ClearPrimaryAttack() = 0;
	virtual void TogglePrimaryAttack() = 0;
	virtual void SecondaryAttack() = 0;
	virtual void Reload() = 0;

	// Adds IN_DUCK while crouching and IN_SPEED unless running, builds m_UserCmd from the fields
	// below and runs it. Everything is zeroed while the pawn is frozen.
	virtual void RunCommand() = 0;
	virtual void BuildUserCmd( CCSGOUserCmd *pCmd, const QAngle &angViewAngles, float flForward, float flLeft, float flUp, uint64 nButtons, uint8 nImpulse ) = 0;
	// Some game types let players pass through each other; this pushes the bot away from the
	// players around it by adding to the forward and side move of the command. Empty in the
	// base; CCSBot sums a push from every nearby living player.
	virtual void AvoidPlayers( CCSGOUserCmd *pCmd ) = 0;
	// The movement speed for walking and running. The base reads a float at 0x1AC of the
	// pawn's m_pMovementServices; CCSBot returns 250.
	virtual float GetMoveSpeed() = 0;

public:
	const BotProfile *m_pProfile;
	CCSPlayerController *m_pController;
	// The controller's pawn when the bot was made.
	CCSPlayerPawn *m_pPlayer;
	bool m_bHasSpawned;
	// A running number from 1.
	uint32 m_nID;

	CCSGOUserCmd m_UserCmd;

	bool m_bIsRunning;
	bool m_bIsCrouching;
	// -1 to 1, forward positive.
	float m_flForwardSpeed;
	// -1 to 1, left positive.
	float m_flLeftSpeed;
	// 1 when idle.
	float m_flVerticalSpeed;
	// InputBitMask_t.
	uint64 m_nButtonFlags;
	float m_flJumpTimestamp;
	Vector m_vecViewForward;

	// Saved run and crouch states: attacking pushes one on entry and pops it on exit.
	struct PostureContext_t
	{
		bool m_bIsRunning;
		bool m_bIsCrouching;
	};

	enum
	{
		MAX_POSTURE_STACK = 8
	};

	PostureContext_t m_PostureStack[ MAX_POSTURE_STACK ];
	int m_nPostureStackIndex;
};

#endif // BOT_H
