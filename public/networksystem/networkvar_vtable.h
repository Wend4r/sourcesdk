//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The virtual methods every schema class with embedded network variables starts with,
// for the declarations that describe only the virtual table of a game class (<Class>VTable).
//
//=============================================================================//

#ifndef NETWORKVAR_VTABLE_H
#define NETWORKVAR_VTABLE_H
#ifdef _WIN32
#pragma once
#endif

#include "schemasystem/schematypes.h"

class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// A schema class whose network variables are flattened into the serializer of the class that
// embeds it: the schema binding of the most derived class and the three change notifications,
// which the embedding entity or component receives.
//-----------------------------------------------------------------------------
#define DECLARE_EMBEDDED_NETWORKVAR_VTABLE() \
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0; \
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data ) = 0; \
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data ) = 0; \
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo ) = 0;

//-----------------------------------------------------------------------------
// A schema class that is networked through a serializer of its own instead of being flattened
// into the embedding class: the same methods and the getter of that serializer's class info.
//-----------------------------------------------------------------------------
#define DECLARE_EMBEDDED_NETWORKVAR_NOTFLATTENED_VTABLE() \
	DECLARE_EMBEDDED_NETWORKVAR_VTABLE() \
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const = 0;

#endif // NETWORKVAR_VTABLE_H
