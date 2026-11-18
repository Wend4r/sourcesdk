#ifndef PREDICTION_H
#define PREDICTION_H

#ifdef _WIN32
	#pragma once
#endif

#include <globalvars_base.h>

#include <tier0/platform.h>
#include <tier0/utlleanvector.h>
#include <tier0/utlvector.h>
#include <tier4/tier4.h>
#include <vconcomm/ivconcomm.h>
#include <iprediction.h>

class CEntityInstance;
// CPrediction lives in the client module, so this is the client command layout.
// Named directly rather than through the CUserCmd alias, which cannot be
// forward declared.
class C_CSGOUserCmd;

//-----------------------------------------------------------------------------
// Per-split-screen-slot prediction state, 0xB8 bytes per element of CPrediction::m_Splits.
//
// Only the represented field offsets are verified; the byte arrays are the parts of the
// element that are not reconstructed yet.
//-----------------------------------------------------------------------------
struct PredictionSplit_t
{
private:
	uint8 m_nUnknown0000[16];

public:
	CEntityInstance *m_pController;

private:
	uint8 m_nUnknown0018[40];

public:
	CUtlLeanVector< CEntityInstance * > m_PredictedEntities;

private:
	uint8 m_nUnknown0050[40];

public:
	int m_nLastExecutedCommand;
	int m_nBasePredictedCommand;
	int m_nLastPredictedCommand;

private:
	uint8 m_nUnknown0084[52];
};

//-----------------------------------------------------------------------------
// Client side prediction manager, the client module's Source2ClientPrediction001
// ( g_pClientSidePrediction ). Its only bases are the app system (IPrediction, through
// CTier4AppSystem, which brings the tier 2 fields) and the VConsole data receiver at 0x28,
// which takes the "PRED" messages.
//
// The byte arrays are the members that are not reconstructed yet; the represented offsets
// are those of the 2026-11-12 client.dll, the same on Linux.
//-----------------------------------------------------------------------------
class CPrediction : public CTier4AppSystem< IPrediction >, public IVConCommDataReceived
{
public:
	PredictionReason_t m_nPredictionReason;
	bool m_bInPrediction;
	bool m_bIsEnginePaused;

	C_CSGOUserCmd *m_pPredictionCommand;

	bool m_bPrintDebug;

	// The latest snapshot tick, and the one of the last net update ("Last net update snapshot
	// tick %d != latest snapshot tick %d")
	int m_nSnapshotTick;
	int m_nLastNetUpdateSnapshotTick;

	CUtlVector< PredictionSplit_t > m_Splits;

	// gpGlobals saved around the prediction pass
	CGlobalVarsBase m_SavedVars;

private:
	uint8 m_nUnknown00C8[8];

public:
	// Seconds the predicted time is ahead of the simulation time (GetPredictionTimeOffset)
	float m_flPredictionTimeOffset;

	// GetTrueViewState
	PredictionTrueViewState_t m_TrueView;

	// Set by Update when the desired final predicted tick changed during the pass
	bool m_bPredictionStateChanged;

private:
	uint8 m_nUnknown00F5[23];

public:
	// Final predicted tick of ComputePredictionTimeOffset
	int m_nPredictedTickBase;

	// GetTrueViewDeactivationCount
	int m_nTrueViewDeactivationCount;

private:
	uint8 m_nUnknown0114[116];
};

#endif // PREDICTION_H
