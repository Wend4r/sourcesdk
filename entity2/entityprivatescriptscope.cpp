#include "entityprivatescriptscope.h"
#include "vscript/ivscript.h"

void CEntityPrivateScriptScope::CallFunction( const char *pszFunction, CVariant *pArgs, int nArgs, CVariant *pReturn ) const
{
	Assert( g_pScriptVM );

	HSCRIPT hFunction = g_pScriptVM->LookupFunction( pszFunction, m_hScope, true );

	if ( !hFunction )
		return;

	g_pScriptVM->ExecuteFunction( hFunction, pArgs, nArgs, pReturn, nullptr, true );
	g_pScriptVM->ReleaseFunction( hFunction );
}
