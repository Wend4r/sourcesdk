#ifndef SCRIPTPRECACHECONTEXT_H
#define SCRIPTPRECACHECONTEXT_H

#if _WIN32
#pragma once
#endif

#include "variant.h"
#include "entity2/entityprecachecontext.h"

// Script class "CScriptPrecacheContext": wraps the precache context of an entity for the "DispatchPrecache" function
// of the entity framework script and the "Precache" function of the private script scope.
// The game creates it on the stack for the duration of the call.
class CScriptPrecacheContext
{
public:
	// Registered with the script VM for the lifetime of the object
	CScriptPrecacheContext( const CEntityPrecacheContext *pContext );
	~CScriptPrecacheContext();

	CScriptPrecacheContext( const CScriptPrecacheContext & ) = delete;
	CScriptPrecacheContext &operator=( const CScriptPrecacheContext & ) = delete;

	HSCRIPT GetScriptInstance() const { return m_hPrecacheContext; }

	// Reads a key from the key values of the context
	CVariant GetValue( const char *pszKey );

	// Script function "AddResource": adds the resource to the resource manifest of the context
	void AddResource( const char *pszResource );

public:
	CEntityPrecacheContext m_scratch;
	const CEntityPrecacheContext *m_pContext;
	HSCRIPT m_hPrecacheContext;
};

#endif // SCRIPTPRECACHECONTEXT_H
