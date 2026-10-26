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

// Reason passed to IPrediction::Update(); the engine spews it as "pred reason %s" with the names below
enum PredictionReason_t : int
{
	PREDICTION_REASON_CLIENT_COMMAND_TICK = 0, // "client_command_tick"
	PREDICTION_REASON_POSTNETUPDATE, // "postnetupdate"
	PREDICTION_REASON_CLIENT_FRAME_SIMULATE, // "client_frame_simulate"
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

	virtual void AdjustTrueViewTiming( int nClientTick, float flClientTickFraction, float *pflFinalPredictedTickFraction ) = 0;
	// TrueView prediction time offset in seconds
	virtual float GetPredictionTimeOffset() = 0;
	virtual void UpdatePredictionTimeOffset( int nClientTick, float flClientTickFraction, float flFinalPredictedTickFraction ) = 0;
	// TrueView prediction time offset rounded to ticks
	virtual int GetPredictionTimeOffsetTicks() = 0;

	// Local origin and rotation of the pawn of nSlot
	virtual void GetViewOrigin( CPlayerSlot nSlot, Vector &vecOrigin ) = 0;
	virtual void GetViewAngles( CPlayerSlot nSlot, QAngle &angRotation ) = 0;

	// v_angle of the pawn of nSlot
	virtual void GetLocalViewAngles( CPlayerSlot nSlot, QAngle &ang ) = 0;
	virtual void SetViewAngles( CPlayerSlot nSlot, const QAngle &ang ) = 0;
	virtual void SetLocalViewAngles( CSplitScreenSlot nSlot, const QAngle &ang ) = 0;

	// True while cl_pred_track tracks an entity field; the engine then spews the prediction reasons
	virtual bool IsPredictionTrackingEnabled() = 0;
	virtual bool IsPlayerSlotPredicted( int nPlayerSlot ) = 0;
	virtual bool IsTrueViewActive() = 0;
	// True when the last TrueView offset could not be determined
	virtual bool IsPredictionTimeOffsetUnknown() = 0;
	// Version of the demo being played back when TrueView can use it, otherwise 0
	virtual int GetTrueViewDemoVersion() = 0;
	// Shuts down every prediction slot
	virtual void ShutdownForDemoSkip() = 0;
};

#endif // IPREDICTION_H
