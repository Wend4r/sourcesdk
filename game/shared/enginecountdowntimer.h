//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A countdown in engine time.
//
//=============================================================================//

#ifndef ENGINECOUNTDOWNTIMER_H
#define ENGINECOUNTDOWNTIMER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"
#include "networksystem/networkvar_vtable.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// The virtual table of the schema struct EngineCountdownTimer. A countdown in engine time. The game class has
// members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class EngineCountdownTimerVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_VTABLE()
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
};

#endif // ENGINECOUNTDOWNTIMER_H
