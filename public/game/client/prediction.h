#ifndef PREDICTION_H
#define PREDICTION_H

#ifdef _WIN32
	#pragma once
#endif

#include <globalvars_base.h>
#include <iprediction.h>

#include <tier0/platform.h>
#include <tier0/utlvector.h>
#include <tier4/tier4.h>
#include <vconcomm/ivconcomm.h>

class CEntityInstance;
// CPrediction lives in the client module, so this is the client command layout.
// Named directly rather than through the CUserCmd alias, which cannot be
// forward declared.
class C_CSGOUserCmd;

//-----------------------------------------------------------------------------
// Per-split-screen-slot prediction state.
//
// Only the represented field offsets are verified.
//-----------------------------------------------------------------------------
struct PredictionSplit_t
{
private:
	uint8 m_nUnknown0000[4];

public:
	CPlayerSlot m_nPlayerSlot;

private:
	uint8 m_nUnknown0008[4];

public:
	// Set for the slot that is predicted for TrueView spectating
	bool m_bTrueView;

private:
	uint8 m_nUnknown000D[11];

public:
	CEntityInstance *m_pController;

private:
	uint8 m_nUnknown0020[40];

public:
	CUtlVector< CEntityInstance * > m_PredictedEntities;

private:
	uint8 m_nUnknown0060[32];

public:
	int m_nLastExecutedCommand;
	int m_nBasePredictedCommand;
	int m_nLastPredictedCommand;

private:
	uint8 m_nUnknown008C[44];
};

//-----------------------------------------------------------------------------
// Client side prediction manager, exposed by the client as
// SOURCE2CLIENTPREDICTION_INTERFACE_VERSION ( see g_pClientSidePrediction ).
//
// Only the represented field offsets are verified.
//-----------------------------------------------------------------------------
class CPrediction : public CTier4AppSystem< IPrediction >, public IVConCommDataReceived
{
public:
	PredictionReason_t m_nPredictionReason;
	bool m_bInPrediction;
	bool m_bIsEnginePaused;

private:
	uint8 m_nUnknown0036[2];

public:
	C_CSGOUserCmd *m_pPredictionCommand;

	bool m_bPrintDebug;

private:
	uint8 m_nUnknown0041[3];

public:
	int m_nSnapshotTick;

private:
	uint8 m_nUnknown0048[8];

public:
	CUtlVector< PredictionSplit_t > m_Splits;
	CGlobalVarsBase m_SavedVars;

private:
	uint8 m_nUnknown00C8[8];

public:
	// TrueView prediction time offset in seconds, see IPrediction::unk018
	float m_flUnk00D0;
	// Returned by IPrediction::unk028 and IPrediction::unk029
	bool m_bUnk00D4;
	bool m_bUnk00D5;

private:
	uint8 m_nUnknown00D6[2];

public:
	// Returned by IPrediction::unk030
	int m_nUnk00D8;

private:
	uint8 m_nUnknown00DC[16];

public:
	// Returned by IPrediction::unk020
	int m_nUnk00EC;

	bool m_bPredictionStateChanged;

private:
	uint8 m_nUnknown00F1[27];

public:
	int m_nPredictedTickBase;

private:
	uint8 m_nUnknown0110[112];
};

#endif // PREDICTION_H
