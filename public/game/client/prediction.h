#ifndef PREDICTION_H
#define PREDICTION_H

#ifdef _WIN32
	#pragma once
#endif

#include <globalvars_base.h>
#include <iprediction.h>

#include <tier0/platform.h>
#include <tier0/utlleanvector.h>
#include <tier0/utlvector.h>

class CEntityInstance;
// CPrediction lives in the client module, so this is the client command layout.
// Named directly rather than through the CUserCmd alias, which cannot be
// forward declared.
class C_CSGOUserCmd;

//-----------------------------------------------------------------------------
// Per-split-screen-slot prediction state.
//
// Reconstructed layout: only the represented field offsets are verified. The total size of
// this structure is NOT verified, so the stride used by CUtlVector< PredictionSplit_t >
// is an assumption; do not rely on it for anything other than the first element.
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
};

//-----------------------------------------------------------------------------
// Client side prediction manager, exposed by the client as
// SOURCE2CLIENTPREDICTION_INTERFACE_VERSION ( see g_pClientSidePrediction ).
//
// The class has a second vtable for another base; its pointer is kept as the
// opaque m_pSecondaryVTable member because those slots have not been
// reconstructed.
//
// Only the represented field offsets are verified, and the total size of the
// class is NOT verified.
//-----------------------------------------------------------------------------
class CPrediction : public IPrediction
{
private:
	uint8 m_nUnknown0008[32];

public:
	void *m_pSecondaryVTable;

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
	uint8 m_nUnknown00C8[40];

public:
	bool m_bPredictionStateChanged;

private:
	uint8 m_nUnknown00F1[27];

public:
	int m_nPredictedTickBase;
};

#endif // PREDICTION_H
