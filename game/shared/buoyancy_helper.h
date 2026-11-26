//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Buoyancy of physics bodies.
//
//=============================================================================//

#ifndef BUOYANCY_HELPER_H
#define BUOYANCY_HELPER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"
#include "imotionevent.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// The virtual table of CBuoyancyHelper: IMotionEvent and the schema class virtuals. Buoyancy of the bodies in
// water; an IMotionEvent of its own and a schema class. The game class has members this declaration does not
// have.
//-----------------------------------------------------------------------------
abstract_class CBuoyancyHelperVTable : public IMotionEvent
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data ) = 0;
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data ) = 0;
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo ) = 0;
};

#endif // BUOYANCY_HELPER_H
