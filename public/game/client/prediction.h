#ifndef PREDICTION_H
#define PREDICTION_H

#ifdef _WIN32
	#pragma once
#endif

#include <globalvars_base.h>
#include <iprediction.h>

#include <entityhandle.h>
#include <tier0/platform.h>
#include <tier0/utldict.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>
#include <tier1/utlhashmaplarge.h>
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

public:
	// Set while ReinitPredictables activates prediction on the client entities
	bool m_bInReinitPredictables;
	// Set while the predictables of a split screen slot are shut down
	bool m_bInShutdownPredictables;
	// Prediction state seen by the last CheckPredictConvar, see Source 1 CPrediction
	bool m_bOldCLPredictValue;

private:
	// Read by the client check for whether prediction is enabled; never written outside the constructor
	uint8 m_nUnknown00CB[1];

public:
	// "enable" key of the prediction data received over VConComm
	bool m_bEnable;

private:
	uint8 m_nUnknown00CD[3];

public:
	// TrueView prediction time offset in seconds
	float m_flPredictionTimeOffset;
	bool m_bTrueViewActive;
	bool m_bPredictionTimeOffsetUnknown;

private:
	uint8 m_nUnknown00D6[2];

public:
	// Version of the demo being played back when TrueView can use it, otherwise 0
	int m_nDemoVersion;

	int m_nFinalPredictedTick;
	float m_flFinalPredictedTickFraction;

private:
	// Only cleared together with the final predicted tick, never read
	uint8 m_nUnknown00E4[8];

public:
	// Prediction time offset rounded to ticks
	int m_nPredictionTimeOffsetTicks;

	bool m_bPredictionStateChanged;

	// Field lookup of the pawn's "m_vecOrigin" used by CheckError ( cl_showerror ), see Source 1 CPrediction
	bool m_bPlayerOriginTypedescriptionSearched;

private:
	uint8 m_nUnknown00F2[6];

public:
	// Pawn the origin field was looked up on; a different pawn restarts the lookup
	CEntityInstance *m_pPlayerOriginTypedescriptionEntity;
	// Field description returned by the lookup (its int at +52 is a byte offset into the predicted frame)
	const void *m_pPlayerOriginTypeDescription;
	// Byte offset returned together with the field description
	int m_nPlayerOriginTypeDescriptionOffset;

	int m_nPredictedTickBase;

	// String table sent to the server as "global_strings" of the prediction report
	struct GlobalStrings_t
	{
		CUtlVector< CUtlString > m_Strings;
		// Index of each string in m_Strings
		CUtlDict< int > m_StringIndices;
	};

	GlobalStrings_t *m_pGlobalStrings;

	// "highest_string" key of the prediction data received over VConComm,
	// sent back as "global_start_index" of the prediction report
	int m_nHighestString;

	// Lookup key of m_ReportedEntities while the report is built
	CEntityHandle m_hReportEntity;

	// "report_counter" of the prediction report, incremented after the post network data update
	int m_nReportCounter;
	// "reset" of the prediction report, set when the server asks for strings past the table
	bool m_bReportReset;

private:
	uint8 m_nUnknown0125[3];

public:
	// Per entity report state keyed by entity handle, each value owns a hash table of 80-byte field records ("entities")
	CUtlHashMap< int, void * > m_ReportedEntities;

	// Set when predictables must be reinitialized, consumed by CheckReinitPredictables
	bool m_bReinitPredictables;

private:
	uint8 m_nUnknown0179[7];
};

#endif // PREDICTION_H
