//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The structured data of debug snapshots.
//
//=============================================================================//

#ifndef DEBUGSNAPSHOT_H
#define DEBUGSNAPSHOT_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// The virtual table of the schema struct DebugSnapshotBaseStructuredData_t. The base of the structured data of
// a debug snapshot (AI_BaseNPC_DebugSnapshotData_t, DebugDrawBoneTransforms_t, ... override only). The game
// class has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class DebugSnapshotBaseStructuredData_tVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
	virtual ~DebugSnapshotBaseStructuredData_tVTable() = 0;
};

#endif // DEBUGSNAPSHOT_H
