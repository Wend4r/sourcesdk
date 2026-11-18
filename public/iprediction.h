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

// Reason passed to the client prediction entry point.
enum PredictionReason_t : int
{
	PREDICTION_REASON_CLIENT_COMMAND_TICK = 0,
};

// TrueView timing state of the client prediction (CPrediction::m_TrueView)
struct PredictionTrueViewState_t
{
	bool m_bActive;
	// Set when the prediction time offset could not be determined
	bool m_bOffsetUndetermined;
	// Desired prediction time offset in ticks
	float m_flDesiredOffsetTicks;
	// Named by what it does: Update stores how many commands it walked past the last predicted
	// command of the split screen slot
	int m_nCommandsPastLastPredicted;
	// Zeroed by the constructor; nothing in client.dll reads or writes it in this build
	int m_nUnused0C;
	// Desired final predicted tick and its fraction
	int m_nDesiredPredictedTick;
	float m_flDesiredPredictedTickFraction;
	// Named by what they do: Update copies the desired final predicted tick here after a pass
	int m_nLastDesiredPredictedTick;
	float m_flLastDesiredPredictedTickFraction;
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
	virtual void PostNetworkDataReceived( const PostDataUpdateCall_t *pCalls, uint64 nCount ) = 0;
	// Resets the prediction state after a full update
	virtual void OnReceivedUncompressedPacket() = 0;

	// With TrueView active, computes the desired prediction time offset for the client tick and writes the fraction of the desired final predicted tick to pflPredictedTickFraction
	virtual void AdjustTrueViewTiming( int nClientTick, float flClientTickFraction, float *pflPredictedTickFraction ) = 0;
	// Seconds the predicted time is ahead of the simulation time
	virtual float GetPredictionTimeOffset() = 0;
	// Recomputes the prediction time offset from the client tick and the final predicted tick
	virtual void ComputePredictionTimeOffset( int nClientTick, float flClientTickFraction, float flPredictedTickFraction ) = 0;
	virtual const PredictionTrueViewState_t *GetTrueViewState() = 0;

	// Local origin and local rotation of the scene node of the player pawn in nSlot (m_vecWrappedLocalOrigin, m_angWrappedLocalRotation); zero without a pawn
	virtual void GetViewOrigin( CPlayerSlot nSlot, Vector &vecOrigin ) = 0;
	virtual void GetViewAngles( CPlayerSlot nSlot, QAngle &angRotation ) = 0;

	// v_angle of the player pawn; the setters also move the old value to v_anglePrevious
	virtual void GetLocalViewAngles( CPlayerSlot nSlot, QAngle &ang ) = 0;
	virtual void SetViewAngles( CPlayerSlot nSlot, const QAngle &ang ) = 0;
	virtual void SetLocalViewAngles( CSplitScreenSlot nSlot, const QAngle &ang ) = 0;

	// Convar-backed gate for the engine's prediction reason spew
	virtual bool IsPredictionReasonSpewEnabled() = 0;
	// True when a prediction slot with this id is tracked
	virtual bool IsSlotTracked( int nSlot ) = 0;
	// Shuts every prediction slot down because of a demo skip and deactivates TrueView
	virtual void ShutdownSlotsForDemoSkip() = 0;
	// Number of times TrueView went inactive
	virtual int GetTrueViewDeactivationCount() = 0;
};

#endif // IPREDICTION_H
