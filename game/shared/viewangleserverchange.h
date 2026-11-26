//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: View angle changes the server imposes.
//
//=============================================================================//

#ifndef VIEWANGLESERVERCHANGE_H
#define VIEWANGLESERVERCHANGE_H
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
// The virtual table of the schema struct ViewAngleServerChange_t. A view angle change the server imposes,
// networked. The game class has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class ViewAngleServerChange_tVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_NOTFLATTENED_VTABLE()
};

#endif // VIEWANGLESERVERCHANGE_H
