//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Engine interface into the client side prediction system
//
//===========================================================================//

#ifndef IPREDICTION_H
#define IPREDICTION_H

#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "mathlib/vector.h"
#include "playerslot.h"
#include "splitscreenslot.h"

struct PostDataUpdateCall_t;

// Reason passed to the engine-side client prediction entry point.
enum PredictionReason_t : int
{
	PREDICTION_REASON_CLIENT_COMMAND_TICK = 0,
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
abstract_class IPrediction : public IAppSystem
{
public:
	// Runs a full prediction pass
	virtual void Update( PredictionReason_t nReason ) = 0;

	virtual void NetUpdatePreStart() = 0;
	virtual void NetUpdateStart() = 0;
	virtual void PostEntityPacketReceived() = 0;
	virtual void PostNetworkDataReceived( const PostDataUpdateCall_t *pCalls, int nCount ) = 0;
	// Resets the prediction state after a full update
	virtual void OnReceivedUncompressedPacket() = 0;

	virtual void unk017( int nClientTick, float flClientTickFraction, float *pflFinalPredictedTickFraction ) = 0;
	// TrueView prediction time offset in seconds
	virtual float unk018() = 0;
	virtual void unk019( int nClientTick, float flClientTickFraction, float flFinalPredictedTickFraction ) = 0;
	// TrueView prediction time offset rounded to ticks
	virtual int unk020() = 0;

	// Local origin and rotation of the pawn of nSlot
	virtual void GetViewOrigin( CPlayerSlot nSlot, Vector &vecOrigin ) = 0;
	virtual void GetViewAngles( CPlayerSlot nSlot, QAngle &angRotation ) = 0;

	// v_angle of the pawn of nSlot
	virtual void GetLocalViewAngles( CPlayerSlot nSlot, QAngle &ang ) = 0;
	virtual void SetViewAngles( CPlayerSlot nSlot, const QAngle &ang ) = 0;
	virtual void SetLocalViewAngles( CSplitScreenSlot nSlot, const QAngle &ang ) = 0;

	// Convar-backed gate for the engine's prediction reason spew
	virtual bool unk026() = 0;
	// True when a prediction slot with this id is tracked
	virtual bool unk027( int nSlot ) = 0;
	// True while TrueView is active
	virtual bool unk028() = 0;
	// True when the last TrueView offset could not be determined
	virtual bool unk029() = 0;
	// Version of the demo being played back when TrueView can use it, otherwise 0
	virtual int unk030() = 0;
	// Shuts down every prediction slot
	virtual void unk031() = 0;
};

#endif // IPREDICTION_H
