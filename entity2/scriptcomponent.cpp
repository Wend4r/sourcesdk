#include "scriptcomponent.h"
#include "entityclass.h"
#include "entityinstance.h"
#include "entityio.h"
#include "scriptkeyvalues.h"
#include "scriptprecachecontext.h"
#include "tier0/dbg.h"
#include "tier0/logging.h"
#include "tier0/memalloc.h"
#include "tier0/strtools.h"
#include "vscript/ivscript.h"

void CScriptComponent::KV3TransferLoad( CKV3TransferLoadContext *pContext )
{
	// Game: when the context has no value at its offset 320, initializes a function-local static of the module once.
	// The component itself loads nothing.
}

void CScriptComponent::ReleaseScope()
{
	if ( m_hScope )
	{
		IScriptVM *pVM = g_pScriptVM;
		FuncHandles_t *pFuncHandles = m_pFuncHandles;

		if ( pVM )
		{
			if ( pFuncHandles )
			{
				for ( int i = 0; i < pFuncHandles->m_Size; ++i )
				{
					pVM->ReleaseFunction( *pFuncHandles->m_Elements[ i ] );
					*pFuncHandles->m_Elements[ i ] = nullptr;
				}

				MemAlloc_Free( pFuncHandles );
				m_pFuncHandles = nullptr;
			}

			if ( !( m_nScopeFlags & SCOPE_EXTERNAL ) )
				pVM->ReleaseScope( m_hScope );
		}
		else if ( pFuncHandles )
		{
			MemAlloc_Free( pFuncHandles );
			m_pFuncHandles = nullptr;
		}

		m_hScope = nullptr;
	}

	m_nScopeFlags = 0;
	m_scriptClassName = CUtlSymbolLarge();
}

CEntityIOOutput *CScriptComponent::GetScriptOutputs( int *pCount )
{
	*pCount = 0;

	if ( !m_pScriptOutputs )
		return nullptr;

	// The outputs are abstract CEntityIOOutput objects,
	// so the block is addressed by its layout: an int count followed by the aligned outputs.
	byte *pBlock = static_cast< byte * >( m_pScriptOutputs );

	*pCount = *reinterpret_cast< int * >( pBlock );

	return reinterpret_cast< CEntityIOOutput * >( pBlock + AlignValue( sizeof( int ), alignof( CEntityIOOutput ) ) );
}

void CScriptComponent::FindScriptOutputs( const char *pszName, CUtlVector< CEntityIOOutput * > &outputs )
{
	int nCount;
	CEntityIOOutput *pOutputs = GetScriptOutputs( &nCount );

	for ( int i = 0; i < nCount; ++i )
	{
		const EntityIOOutputDesc_t *pDesc = pOutputs[ i ].GetDescription();

		// The game compares the pooled names, and the pool ignores case.
		if ( !V_stricmp( pDesc->m_pName ? pDesc->m_pName : "", pszName ? pszName : "" ) )
			outputs.AddToTail( &pOutputs[ i ] );
	}
}

void CScriptComponent::PurgeScriptOutputs()
{
	int nCount;
	CEntityIOOutput *pOutputs = GetScriptOutputs( &nCount );

	if ( !pOutputs )
		return;

	for ( int i = nCount; i-- > 0; )
		pOutputs[ i ].CEntityIOOutput::~CEntityIOOutput();

	MemAlloc_Free( m_pScriptOutputs );
	m_pScriptOutputs = nullptr;
}

bool CScriptComponent::CallEntityFrameworkFunction( const char *pszFunction, CVariant *pArgs, int nArgs, CVariant *pReturn )
{
	IScriptVM *pVM = g_pScriptVM;

	if ( !pVM )
		return false;

	// The game keeps its own reference to the table and caches the function handles.
	ScriptVariant_t framework;

	if ( !pVM->GetValue( nullptr, "EntityFramework", &framework ) )
		return false;

	bool bCalled = false;

	if ( !framework.IsNull() && framework.m_hScript )
	{
		HSCRIPT hFunction = pVM->LookupFunction( pszFunction, framework.m_hScript );

		if ( hFunction )
		{
			bCalled = pVM->ExecuteFunction( hFunction, pArgs, nArgs, pReturn, framework.m_hScript, true ) != SCRIPT_ERROR;
			pVM->ReleaseFunction( hFunction );
		}
	}

	pVM->ReleaseValue( framework );

	return bCalled;
}

bool CScriptComponent::DispatchActivate( const CEntityInstance *pEntity )
{
	if ( !m_hScope )
	{
		Warning( "Failed to activate script entity %s of type %s: script entity instance is null\n", pEntity->GetName(),pEntity->m_pEntity->m_pClass->m_pClassInfo->m_pszClassname );

		return true;
	}

	ScriptVariant_t instance( m_hScope );
	ScriptVariant_t result;

	if ( !CallEntityFrameworkFunction( "DispatchActivate", &instance, 1, &result ) )
		return true;

	bool bResult = false;

	result.AssignTo( &bResult );

	return bResult;
}

void CScriptComponent::DispatchPrecache( const CEntityInstance *pEntity, const CEntityPrecacheContext *pContext )
{
	if ( !m_hScope )
	{
		Warning( "Failed to precache script entity %s of type %s: script entity instance is null\n", pEntity->GetName(),pEntity->m_pEntity->m_pClass->m_pClassInfo->m_pszClassname );

		return;
	}

	CScriptPrecacheContext scriptContext( pContext );

	ScriptVariant_t args[] = { ScriptVariant_t( m_hScope ), ScriptVariant_t( scriptContext.GetScriptInstance() ) };

	CallEntityFrameworkFunction( "DispatchPrecache", args, ARRAYSIZE( args ), nullptr );
}

void CScriptComponent::DispatchSpawn( const CEntityInstance *pEntity, const CEntityKeyValues *pKeyValues )
{
	if ( !m_hScope )
	{
		Warning( "Failed to spawn script entity %s of type %s: script entity instance is null\n", pEntity->GetName(),pEntity->m_pEntity->m_pClass->m_pClassInfo->m_pszClassname );

		return;
	}

	CScriptKeyValues scriptKeyValues( pKeyValues );
	HSCRIPT hKeyValues = scriptKeyValues.RegisterScriptInstance();

	ScriptVariant_t args[] = { ScriptVariant_t( m_hScope ), ScriptVariant_t( hKeyValues ) };

	CallEntityFrameworkFunction( "DispatchSpawn", args, ARRAYSIZE( args ), nullptr );

	g_pScriptVM->RemoveInstance( hKeyValues );
}

void CScriptComponent::DispatchUpdateOnRemove( const CEntityInstance *pEntity )
{
	if ( !m_hScope )
	{
		Warning( "Failed to update on remove script entity %s of type %s: script entity instance is null\n", pEntity->GetName(),pEntity->m_pEntity->m_pClass->m_pClassInfo->m_pszClassname );

		return;
	}

	ScriptVariant_t instance( m_hScope );
	ScriptVariant_t result;

	if ( !CallEntityFrameworkFunction( "DispatchUpdateOnRemove", &instance, 1, &result ) )
		return;

	// The game converts the result to bool and ignores it; the conversion warns on a type it cannot convert.
	bool bResult = false;

	result.AssignTo( &bResult );
}

bool CScriptComponent::DispatchInput( const char *pszInputName, HSCRIPT hArgs )
{
	if ( !g_pScriptVM || !m_hScope )
		return false;

	ScriptVariant_t args[] = { ScriptVariant_t( m_hScope ), ScriptVariant_t( pszInputName ? pszInputName : "", false ), ScriptVariant_t( hArgs ) };
	ScriptVariant_t result;

	if ( !CallEntityFrameworkFunction( "DispatchInput", args, ARRAYSIZE( args ), &result ) )
		return false;

	bool bResult = false;

	result.AssignTo( &bResult );

	return bResult;
}
