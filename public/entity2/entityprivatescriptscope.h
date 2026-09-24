#ifndef ENTITYPRIVATESCRIPTSCOPE_H
#define ENTITYPRIVATESCRIPTSCOPE_H

#if _WIN32
#pragma once
#endif

#include "variant.h"

// Per-instance script scope of an entity, where its private scripts run.
class CEntityPrivateScriptScope
{
public:
	// Calls pszFunction from the scope with the given arguments, if the scope defines it.
	void CallFunction( const char *pszFunction, CVariant *pArgs, int nArgs, CVariant *pReturn ) const;

public:
	HSCRIPT m_hScope;
};

#endif // ENTITYPRIVATESCRIPTSCOPE_H
