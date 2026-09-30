#ifndef CSTRIKE15_CS_MOVEDATA_H
#define CSTRIKE15_CS_MOVEDATA_H

#pragma once

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "tier1/utlvector.h"
#include "entityhandle.h"
#include "gametrace.h"

//-----------------------------------------------------------------------------
// One subtick step of a command, built from its CSubtickMoveStep entries plus a
// closing step at fraction 1. A step with a button carries the edge; a step
// without one carries analog move deltas. The view angles are absolute: the
// command's angles plus the pitch and yaw deltas accumulated up to this step.
//-----------------------------------------------------------------------------
struct SubtickMove_t
{
	// Fraction of the tick, 0..1.
	float m_flWhen;
	uint64 m_nButton;

	union
	{
		bool m_bPressed;

		struct
		{
			float m_flForwardDelta;
			float m_flLeftDelta;
		} m_AnalogMove;
	};

	float m_flPitch;
	float m_flYaw;
};

//-----------------------------------------------------------------------------
// A subtick edge of IN_ATTACK or IN_ATTACK2, split out of the move steps so that
// weapons can fire at the exact fraction.
//-----------------------------------------------------------------------------
struct SubtickAttackMove_t
{
	float m_flWhen;
	uint64 m_nButton;
	bool m_bPressed;
};

//-----------------------------------------------------------------------------
// A collision recorded while the player moved, for the touch callbacks that run
// after movement.
//-----------------------------------------------------------------------------
struct touchlist_t
{
	Vector deltavelocity;
	trace_t trace;
};

//-----------------------------------------------------------------------------
// Movement input and state for one run of a command through the movement
// services: filled by SetupMove, consumed by ProcessMovement, written back by
// FinishMove.
//-----------------------------------------------------------------------------
class CMoveDataBase
{
public:
	// Set when the command runs without frame time: movement neither advances
	// the tick nor integrates.
	bool m_bHasZeroFrametime : 1;

	// Set for a late command, one that runs on behalf of CCSGOUserCmd::m_pParentCmd.
	bool m_bIsLateCommand : 1;

	CEntityHandle m_nPlayerHandle;

	QAngle m_vecAbsViewAngles;
	QAngle m_vecViewAngles;

	// Forward, left and up input the movement services read from the command.
	Vector m_vecLastMovementImpulses;

	// Same values as m_vecLastMovementImpulses, zeroed while the player is frozen.
	float m_flForwardMove;
	float m_flSideMove;
	float m_flUpMove;

	Vector m_vecVelocity;

	// SetupMove copies m_vecViewAngles here.
	QAngle m_vecAngles;

private:
	// Zeroed by the constructor.
	uint8 m_unk50[ 0x10 ];

public:
	CUtlVector< SubtickMove_t > m_SubtickMoves;
	CUtlVector< SubtickAttackMove_t > m_AttackSubtickMoves;

	// Set to 1 for every command.
	float m_flUnk90;

private:
	uint8 m_unk94[ 0x4 ];

public:
	// Cleared for every command.
	CUtlVector< touchlist_t > m_TouchList;

	// Zeroed with the tick fields when the move data is reset.
	Vector m_collisionNormal;
	Vector m_groundNormal;

	Vector m_vecAbsOrigin;

	// The tick the command starts at and the tick it runs to: the current tick
	// minus one and the current tick, or the current tick twice without frame time.
	int m_nTickCount;
	int m_nTargetTick;
	float m_flSubtickStartFraction;
	float m_flSubtickEndFraction;
};

class CMoveData : public CMoveDataBase
{
public:
	Vector m_outWishVel;

	// The pawn's v_angle before this command.
	QAngle m_vecOldAngles;

	// The per-step outputs below are cleared before every subtick step.
	Vector2D m_vecWalkWishVel;

	// In units per second squared.
	Vector m_vecContinousAcceleration;

	// Immediate velocity change in units per second, applied outside the acceleration.
	Vector m_vecFrameVelocityDelta;

	// The lowest of sv_maxspeed, the client's max speed and the active weapon's
	// max speed; 1 while the player may not move.
	float m_flMaxSpeed;

	// The movement services' m_flMaxspeed.
	float m_flClientMaxSpeed;

	float m_flFrictionDecel;

	// The vertical position, velocity and acceleration before the air move, from which
	// the landing time within the tick is solved.
	float m_flPreAirMovePosZ;
	float m_flPreAirMoveVelZ;
	float m_flPreAirMoveAccelZ;

	// Set before every subtick step while the pawn is not on the ground.
	bool m_bInAir;

	// Game code moved the player after the previous command ran.
	bool m_bGameCodeMovedPlayer;

	// FinishMove copies it into CCSPlayer_MovementServices::m_bUseFrictionStashedSpeed.
	bool m_bUseFrictionStashedSpeed;
};

#endif // CSTRIKE15_CS_MOVEDATA_H
