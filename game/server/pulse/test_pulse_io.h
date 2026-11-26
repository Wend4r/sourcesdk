//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A test class of the Pulse entity IO
//
//=============================================================================//

#ifndef TEST_PULSE_IO_H
#define TEST_PULSE_IO_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

class CGlobalSymbol;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;

//-----------------------------------------------------------------------------
// The virtual table of CTestPulseIOComponent, a schema class of 48 bytes (m_ComponentData, a
// CUtlString at 8, and the output m_OnComponentTestFunc at 16). The class carries a VTable
// suffix and declares virtual methods only. Five slots on both platforms, no virtual
// destructor. The game derives CTestPulseIOComponent_Derived from it, which only adds its own
// class name to CastToType.
//-----------------------------------------------------------------------------
abstract_class CTestPulseIOComponentVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;

	// Named by what they do, as the KV3 transfer methods of other schema classes
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	// Named by what it does: returns "CTestPulseIOComponent"
	virtual const char *GetClassName() const = 0;
	// Named by what it does: this when typeName is the name of the class, null otherwise
	virtual void *CastToType( const CGlobalSymbol &typeName ) = 0;
};

#endif // TEST_PULSE_IO_H
