#include "scriptprecachecontext.h"
#include "entitysystem.h"
#include "scriptkeyvalues.h"
#include "vscript/ivscript.h"

BEGIN_SCRIPTDESC_ROOT_NAMED( CScriptPrecacheContext, "CScriptPrecacheContext", "Container to hold context published to precache functions in script" )
	DEFINE_SCRIPTFUNC_NAMED( GetValue, "GetValue", "Reads a spawn key" )
	DEFINE_SCRIPTFUNC_NAMED( AddResource, "AddResource", "Precaches a specific resource" )
END_SCRIPTDESC()

CScriptPrecacheContext::CScriptPrecacheContext( const CEntityPrecacheContext *pContext ) : m_scratch{}, m_pContext( pContext )
{
	Assert( g_pScriptVM );

	m_hPrecacheContext = g_pScriptVM->RegisterInstance( ::GetScriptDesc( this ), this );
}

CScriptPrecacheContext::~CScriptPrecacheContext()
{
	Assert( g_pScriptVM );

	g_pScriptVM->RemoveInstance( m_hPrecacheContext );
}

CVariant CScriptPrecacheContext::GetValue( const char *pszKey )
{
	Assert( m_pContext );
	Assert( m_pContext->m_pKeyValues );

	return CScriptKeyValues( m_pContext->m_pKeyValues ).GetValue( pszKey );
}

void CScriptPrecacheContext::AddResource( const char *pszResource )
{
	Assert( m_pContext );
	Assert( m_pContext->m_pManifest );

	m_pContext->m_pManifest->AddResource( pszResource );
}
