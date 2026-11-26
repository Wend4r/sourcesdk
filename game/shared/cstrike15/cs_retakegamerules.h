//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The retake mode state of the game rules.
//
//=============================================================================//

#ifndef CSTRIKE15_CS_RETAKEGAMERULES_H
#define CSTRIKE15_CS_RETAKEGAMERULES_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"
#include "vscript/ccscript.h"
#include "networksystem/networkvar_vtable.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// The virtual table of CRetakeGameRules (the retake mode state of the game rules, a networked schema class and
// a game event listener). The game class has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class CRetakeGameRulesVTable : public CGameEventListener
{
public:
	virtual ~CRetakeGameRulesVTable() = 0;
	DECLARE_EMBEDDED_NETWORKVAR_NOTFLATTENED_VTABLE()
};

#endif // CSTRIKE15_CS_RETAKEGAMERULES_H
