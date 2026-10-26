#ifndef PREDICTION_H
#define PREDICTION_H

#ifdef _WIN32
	#pragma once
#endif

#include <globalvars_base.h>
#include <iprediction.h>

#include <entityhandle.h>
#include <splitscreenslot.h>
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
// Members named m_nUnknown are zeroed by the constructor (CPrediction adds a
// slot for each predicted player) and, the ints, again when the slot is reset;
// no reader was found in the prediction code.
//-----------------------------------------------------------------------------
struct PredictionSplit_t
{
	// Prediction errors of the predicted entities ( cl_showerror ), keyed by an int of the entity: a red-black
	// tree with unsigned short links over 48-byte nodes, each holding a CUtlVector of 32-byte field error records
	struct PredictionErrorTree_t
	{
		uint16 m_nNodeCount;
		uint16 m_nNodeAllocationCount;
		void *m_pNodes;
		uint16 m_nRoot;
		uint16 m_nElementCount;
		uint16 m_nFirstFree;

	private:
		uint8 m_pad16[ 2 ];
	};

	// m_nSplitScreenSlot of the controller while it is the local player controller, otherwise -1
	CSplitScreenSlot m_nSplitScreenSlot;
	CPlayerSlot m_nPlayerSlot;
	int m_nUnknown0008;

	// Set for the slot that is predicted for TrueView spectating
	bool m_bTrueView;

private:
	uint8 m_pad0D[ 3 ];

public:
	// Non-zero marks the prediction state as changed when the slot's predictables are shut down
	int m_nUnknown0010;

	int m_nUnknown0014;
	CEntityInstance *m_pController;
	int m_nUnknown0020;
	int m_nUnknown0024;

private:
	// Not initialized by the constructor and not used
	uint8 m_pad28[ 8 ];

public:
	PredictionErrorTree_t m_PredictionErrors;
	CUtlVector< CEntityInstance * > m_PredictedEntities;

	uint64 m_nUnknown0060[ 2 ];
	int m_nUnknown0070;
	int m_nUnknown0074;
	int m_nUnknown0078;
	int m_nUnknown007C;
	int m_nLastExecutedCommand;
	int m_nBasePredictedCommand;
	int m_nLastPredictedCommand;

	int m_nUnknown008C;
	uint64 m_nUnknown0090;
	int m_nUnknown0098;
	int m_nUnknown009C;

	// Element type not recovered: the prediction code only clears and frees it when the slot is reset or shut down
	CUtlVector< void * > m_UnknownVector00A0;
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
	uint8 m_pad36[ 2 ];

public:
	C_CSGOUserCmd *m_pPredictionCommand;

	bool m_bPrintDebug;

private:
	uint8 m_pad41[ 3 ];

public:
	int m_nSnapshotTick;

private:
	// -1 after the constructor, like m_nSnapshotTick; no reader found
	int m_nUnknown0048;
	uint8 m_pad4C[ 4 ];

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
	bool m_bUnknown00CB;

public:
	// "enable" key of the prediction data received over VConComm
	bool m_bEnable;

private:
	uint8 m_padCD[ 3 ];

public:
	// TrueView prediction time offset in seconds
	float m_flPredictionTimeOffset;
	bool m_bTrueViewActive;
	bool m_bPredictionTimeOffsetUnknown;

private:
	uint8 m_padD6[ 2 ];

public:
	// Version of the demo being played back when TrueView can use it, otherwise 0
	int m_nDemoVersion;

	int m_nFinalPredictedTick;
	float m_flFinalPredictedTickFraction;

	// The final predicted tick and fraction before the latest update ( "TrueView target time going in reverse" )
	int m_nPreviousFinalPredictedTick;
	float m_flPreviousFinalPredictedTickFraction;

	// Prediction time offset rounded to ticks
	int m_nPredictionTimeOffsetTicks;

	bool m_bPredictionStateChanged;

	// Field lookup of the pawn's "m_vecOrigin" used by CheckError ( cl_showerror ), see Source 1 CPrediction
	bool m_bPlayerOriginTypedescriptionSearched;

private:
	uint8 m_padF2[ 6 ];

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
	uint8 m_pad125[ 3 ];

public:
	// Per entity report state keyed by entity handle, each value owns a hash table of 80-byte field records ("entities")
	CUtlHashMap< int, void * > m_ReportedEntities;

	// Set when predictables must be reinitialized, consumed by CheckReinitPredictables
	bool m_bReinitPredictables;

private:
	uint8 m_pad179[ 7 ];
};

#endif // PREDICTION_H
