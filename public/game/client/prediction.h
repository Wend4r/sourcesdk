#ifndef PREDICTION_H
#define PREDICTION_H

#ifdef _WIN32
	#pragma once
#endif

#include <globalvars_base.h>

#include <tier0/platform.h>
#include <tier0/utlleanvector.h>
#include <tier0/utlvector.h>

class CEntityInstance;
// CPrediction lives in the client module, so this is the client command layout.
// Named directly rather than through the CUserCmd alias, which cannot be
// forward declared.
class C_CSGOUserCmd;
struct PostDataUpdateCall_t;

// Reason passed to the engine-side client prediction entry point.
enum PredictionReason_t : int
{
	PREDICTION_REASON_CLIENT_COMMAND_TICK = 0,
};

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
// Client side prediction manager, exposed by the client module as
// SOURCE2CLIENTPREDICTION_INTERFACE_VERSION ( see g_pClientSidePrediction ).
//
// Reconstructed from client.dll. The class has two vtables - the engine only
// ever reaches it through the primary one, which is the 32-slot table declared
// below; the secondary vtable pointer sits at 0x0028 and is left as an opaque
// member because its slots have not been reconstructed.
//
// Every slot is kept in its verified index. Slots whose purpose is unknown are
// placeholders named after their index - do not reorder, remove or collapse
// them. Where a placeholder's signature was recovered from the decompiler it is
// declared with that signature; the Unk_ prefix only means the NAME is
// unverified.
//
// Only the represented field offsets are verified, and the total size of the
// class is NOT verified.
//-----------------------------------------------------------------------------
class CPrediction
{
public:
	virtual bool Unk_Slot0( void *p ) = 0;
	virtual void Unk_Slot1( void *p ) = 0;
	virtual void Unk_Slot2() {}
	virtual void Unk_Slot3( void *p ) = 0;
	virtual void Unk_Slot4( void *p ) = 0;
	virtual void Unk_Slot5() {}
	virtual void Unk_Slot6() {}
	virtual int Unk_Slot7() = 0; // returns the constant 4
	virtual void Unk_Slot8( void *p ) = 0;
	virtual bool Unk_Slot9() = 0; // returns the constant 1
	virtual int Unk_Slot10() = 0; // returns the constant 2

	// Runs a full prediction pass. engine2!CNetworkGameClient::ClientSidePredict
	// calls this through vtable+0x58 while m_bInSimulation is forced true.
	virtual void Update( PredictionReason_t nReason ) = 0;

	virtual void NetUpdatePreStart() = 0;
	virtual void NetUpdateStart() = 0;
	virtual void PostEntityPacketReceived() = 0;
	virtual void PostNetworkDataReceived( const PostDataUpdateCall_t *pCalls, uint64 nCount ) = 0;

	virtual void Unk_Slot16( void *p ) = 0;
	virtual void Unk_Slot17( void *p ) = 0;
	virtual void Unk_Slot18( void *p ) = 0;
	virtual void Unk_Slot19( void *p ) = 0;
	virtual int Unk_Slot20() = 0;
	virtual void Unk_Slot21( void *p ) = 0;
	virtual void Unk_Slot22( void *p ) = 0;
	virtual void Unk_Slot23( void *p ) = 0;
	virtual void Unk_Slot24( void *p ) = 0;
	virtual void Unk_Slot25( void *p ) = 0;

	// Convar-backed gate. engine2 calls this through vtable+0xD0 to decide
	// whether to Msg() the "pred reason %s -- start/finish" lines around Update().
	virtual bool Unk_Slot26() = 0;

	virtual void Unk_Slot27( void *p ) = 0;
	virtual void Unk_Slot28( void *p ) = 0;
	virtual bool Unk_Slot29() = 0;
	virtual void Unk_Slot30( void *p ) = 0;
	virtual void Unk_Slot31( void *p ) = 0;

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
