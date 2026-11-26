#ifndef CSTRIKE15_CS_PLAYER_PAWNGRAPHCONTROLLER_H
#define CSTRIKE15_CS_PLAYER_PAWNGRAPHCONTROLLER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/globalsymbol.h"
#include "animlib/graph/animgraph_valuetypes.h"
#include "cs_player_weapongraphcontroller.h"

#include "schemacompiler/schemaon.h"

//-----------------------------------------------------------------------------
// Drives the graph of a player model: the weapon parameters of its base and the movement,
// ladder, air, foot IK, aim and flinch parameters of the pawn. It overrides the virtual
// methods of its bases and adds none.
//
// The game implements the virtual methods; the objects are only reached through pointers.
//-----------------------------------------------------------------------------
schema class CCS2PawnGraphController : public CCS2WeaponGraphController
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	CAnimGraph2ParamOptionalRef< bool > m_bIsDefusing;
	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_moveType;
	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_moveDirectionID;
	CAnimGraph2ParamOptionalRef< float32 > m_flMoveSpeedX;
	CAnimGraph2ParamOptionalRef< float32 > m_flMoveSpeedY;
	CAnimGraph2ParamOptionalRef< float32 > m_flMoveSpeedHorizontal;
	CAnimGraph2ParamOptionalRef< float32 > m_flPreviousMoveSpeedHorizontal;
	CAnimGraph2ParamOptionalRef< float32 > m_flCrouchAmount;
	CAnimGraph2ParamOptionalRef< bool > m_bIsWalking;
	CAnimGraph2ParamOptionalRef< float32 > m_flWeaponDropAmount;
	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_groundAction;
	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_groundActionDirectionID;
	CAnimGraph2ParamOptionalRef< float32 > m_flGroundTurnAngleOrVelocity;
	CAnimGraph2ParamOptionalRef< float32 > m_flLadderCycle;
	CAnimGraph2ParamOptionalRef< float32 > m_flLadderYaw;
	CAnimGraph2ParamOptionalRef< float32 > m_flLadderYawBackwards;
	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_airAction;
	CAnimGraph2ParamOptionalRef< float32 > m_flAirHeightAboveGround;
	CAnimGraph2ParamOptionalRef< CNmTarget > m_leftFootTarget;
	CAnimGraph2ParamOptionalRef< CNmTarget > m_rightFootTarget;
	CAnimGraph2ParamOptionalRef< float32 > m_flFlashedAmount;
	CAnimGraph2ParamOptionalRef< float32 > m_flAimPitchAngle;
	CAnimGraph2ParamOptionalRef< float32 > m_flAimYawAngle;
	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_flinchHead;
	CAnimGraph2ParamOptionalRef< bool > m_flinchHeadRestart;
	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_flinchBody;
	CAnimGraph2ParamOptionalRef< bool > m_flinchBodyRestart;
	CAnimGraph2ParamOptionalRef< bool > m_flinchIsOnFire;
};

#include "schemacompiler/schemaoff.h"

#endif // CSTRIKE15_CS_PLAYER_PAWNGRAPHCONTROLLER_H
