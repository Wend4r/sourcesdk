#include "entityinstance.h"
#include "vscript/ivscript.h"

HSCRIPT CEntityInstance::GetScriptInstance()
{
	if ( !m_hScriptInstance )
	{
		char *szName = (char *)stackalloc( 1024 );
		g_pScriptVM->GenerateUniqueKey( ( GetName() != nullptr ) ? GetName() : GetClassname(), szName, 1024 );
		m_hScriptInstance = g_pScriptVM->RegisterInstance( GetScriptDesc(), this );
		g_pScriptVM->SetInstanceUniqeId( m_hScriptInstance, szName );
	}
	return m_hScriptInstance;
}