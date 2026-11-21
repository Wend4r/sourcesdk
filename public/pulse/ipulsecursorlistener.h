//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A listener of the Pulse cursors (IPulseSystem::AddCursorStateListener):
//          keeps per-cursor state in the cursor's save data.
//
//=============================================================================//

#ifndef IPULSECURSORLISTENER_H
#define IPULSECURSORLISTENER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "kv3lib/keyvalues3.h"

class CPulseExecCursor;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;

//-----------------------------------------------------------------------------
// No source names the methods; they are named by what CBasePulseGameSystem / CServerPulseGameSystem
// do in them. No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class IPulseCursorListener
{
public:
	// The KV3 member the listener's state is saved under; CServerPulseGameSystem returns
	// "server_game_system"
	virtual CKV3MemberName GetSaveStateMemberName() = 0;
	// Writes / reads the listener's state of the cursor ("output_callbacks", "queued_outputs")
	virtual void SaveCursorState( CPulseExecCursor *pCursor, CKV3TransferSaveContext *pContext ) = 0;
	virtual void LoadCursorState( CPulseExecCursor *pCursor, CKV3TransferLoadContext *pContext ) = 0;
	// The cursor is gone: drops everything registered for it
	virtual void OnCursorRemoved( CPulseExecCursor *pCursor ) = 0;
};

#endif // IPULSECURSORLISTENER_H
