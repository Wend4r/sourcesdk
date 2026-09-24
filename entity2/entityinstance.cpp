#include "entityinstance.h"
#include "entityclass.h"
#include "entityio.h"
#include "entitykeyvalues.h"
#include "entitysystem.h"
#include "scriptcomponent.h"
#include "scriptkeyvalues.h"
#include "scriptprecachecontext.h"
#include "tier0/logging.h"
#include "tier0/strtools.h"
#include "vscript/ivscript.h"
#include "vscript_shared.h"

BEGIN_SCRIPTDESC_ROOT_NAMED( CEntityInstance, "CEntityInstance", "CEntityInstance: Root class for all entities" )
	DEFINE_SCRIPTFUNC_NAMED( ScriptFireOutput, "FireOutput", "Fire an entity output" )
	DEFINE_SCRIPTFUNC_NAMED( RemoveSelf, "Destroy", "Delete this entity" )
	DEFINE_SCRIPTFUNC_NAMED( GetIntAttr, "GetIntAttr", "Get Integer Attribute" )
	DEFINE_SCRIPTFUNC_NAMED( SetIntAttr, "SetIntAttr", "Set Integer Attribute" )
	DEFINE_SCRIPTFUNC_NAMED( GetEntityNameAsCStr, "GetName", "Get the entity name" )
	DEFINE_SCRIPTFUNC_NAMED( GetDebugName, "GetDebugName", "Get the entity name w/help if not defined (i.e. classname/etc)" )
	DEFINE_SCRIPTFUNC_NAMED( ConnectOutputToScriptSelf, "ConnectOutput", "Adds an I/O connection that will call the named function on this entity when the specified output fires." )
	DEFINE_SCRIPTFUNC_NAMED( ConnectOutputToScript, "RedirectOutput", "Adds an I/O connection that will call the named function on the passed entity when the specified output fires." )
	DEFINE_SCRIPTFUNC_NAMED( DisconnectOutputFromScriptSelf, "DisconnectOutput", "Removes a connected script function from an I/O event on this entity." )
	DEFINE_SCRIPTFUNC_NAMED( DisconnectOutputFromScript, "DisconnectRedirectedOutput", "Removes a connected script function from an I/O event on the passed entity." )
	DEFINE_SCRIPTFUNC_NAMED( ScriptGetEntityIndex, "entindex", nullptr )
	DEFINE_SCRIPTFUNC_NAMED( ScriptGetEntityIndex, "GetEntityIndex", nullptr )
	DEFINE_SCRIPTFUNC_NAMED( GetClassNameAsCStr, "GetClassname", nullptr )
	DEFINE_SCRIPTFUNC_NAMED( ScriptGetEHandle, "GetEntityHandle", "Get the entity as an EHANDLE" )
	DEFINE_SCRIPTFUNC_NAMED( RemoveSelf, "RemoveSelf", nullptr )
	DEFINE_SCRIPTFUNC_NAMED( ScriptGetPublicScriptScope, "GetPublicScriptScope", "Retrieve the public script-side data associated with an entity" )
	DEFINE_SCRIPTFUNC_NAMED( ScriptGetOrCreatePublicScriptScope, "GetOrCreatePublicScriptScope", "Retrieve, creating if necessary, the public script-side data associated with an entity" )
	DEFINE_SCRIPTFUNC_NAMED( ScriptGetPrivateScriptScope, "GetPrivateScriptScope", "Retrieve the private per-instance script-side data associated with an entity" )
	DEFINE_SCRIPTFUNC_NAMED( ScriptGetOrCreatePrivateScriptScope, "GetOrCreatePrivateScriptScope", "Retrieve, creating if necessary, the private per-instance script-side data associated with an entity" )
END_SCRIPTDESC()

// Follows the game's CEntityInstance; steps without an SDK interface are marked with "Game:"

void CEntityInstance::ReleasePublicScriptScope()
{
	HSCRIPT hPublicScope = reinterpret_cast< HSCRIPT >( m_pEntity->m_hPublicScope );

	if ( !hPublicScope )
		return;

	if ( CScriptComponent *pComponent = m_CScriptComponent )
	{
		pComponent->ReleaseScope();
		pComponent->PurgeScriptOutputs();

		m_CScriptComponent = nullptr;

		// The component is already emptied, so only its memory is freed
		Release( pComponent );
	}

	Assert( g_pScriptVM );

	g_pScriptVM->RemoveInstance( hPublicScope );
}

CEntityInstance::~CEntityInstance()
{
	if ( m_pKeyValues )
	{
		m_pKeyValues->Release();
		m_pKeyValues = nullptr;
	}

	if ( m_pEntity && m_pEntity->m_hPublicScope )
	{
		ReleasePublicScriptScope();
		m_pEntity->m_hPublicScope = 0;
	}

	if ( m_hPrivateScope.m_hScope && g_pScriptVM )
		g_pScriptVM->ReleaseScope( m_hPrivateScope.m_hScope );
}

void CEntityInstance::Precache( const CEntityPrecacheContext *pContext )
{
	if ( m_CScriptComponent )
		m_CScriptComponent->DispatchPrecache( this, pContext );

	if ( !m_pEntity )
		return;

	ReloadPrivateScripts();

	// Without a private scope the game resolves the function in the root table
	CScriptPrecacheContext scriptContext( pContext );
	ScriptVariant_t arg( scriptContext.GetScriptInstance() );

	m_hPrivateScope.CallFunction( "Precache", &arg, 1, nullptr );
}

void CEntityInstance::Spawn( const CEntityKeyValues *pKeyValues )
{
	if ( m_CScriptComponent )
		m_CScriptComponent->DispatchSpawn( this, pKeyValues );

	if ( !m_hPrivateScope.m_hScope )
		return;

	CScriptKeyValues scriptKeyValues( pKeyValues );
	ScriptVariant_t arg( scriptKeyValues.RegisterScriptInstance() );

	m_hPrivateScope.CallFunction( "Spawn", &arg, 1, nullptr );

	Assert( g_pScriptVM );

	g_pScriptVM->RemoveInstance( arg.m_hScript );
}

void CEntityInstance::Activate( ActivateType_t activateType )
{
	if ( m_CScriptComponent )
		m_CScriptComponent->DispatchActivate( this );

	if ( !m_hPrivateScope.m_hScope )
		return;

	ScriptVariant_t arg( static_cast< int32 >( activateType ) );
	ScriptVariant_t result;

	m_hPrivateScope.CallFunction( "Activate", &arg, 1, &result );
}

void CEntityInstance::UpdateOnRemove()
{
	if ( m_CScriptComponent )
		m_CScriptComponent->DispatchUpdateOnRemove( this );

	if ( m_hPrivateScope.m_hScope )
		m_hPrivateScope.CallFunction( "UpdateOnRemove", nullptr, 0, nullptr );

	if ( m_pEntity && m_pEntity->m_hPublicScope )
	{
		ReleasePublicScriptScope();
		m_pEntity->m_hPublicScope = 0;
	}

	if ( m_hPrivateScope.m_hScope )
	{
		if ( g_pScriptVM )
			g_pScriptVM->ReleaseScope( m_hPrivateScope.m_hScope );

		m_hPrivateScope.m_hScope = nullptr;
	}

	if ( !( m_pEntity->m_flags & EF_IS_ANONYMOUS_ALLOCATION ) )
	{
		GameEntitySystem()->RemoveEntityName( m_pEntity );
		m_pEntity->m_pClass->UnlinkFromClassList( m_pEntity );
	}

	if ( m_pKeyValues )
	{
		m_pKeyValues->Release();
		m_pKeyValues = nullptr;
	}
}

void CEntityInstance::OnSetDormant( EntityDormancyType_t prevDormancyType, EntityDormancyType_t newDormancyType )
{
	if ( ( prevDormancyType == ENTITY_DORMANT ) == ( newDormancyType == ENTITY_DORMANT ) )
		return;

	// A dormant entity is not found by name or class
	if ( newDormancyType != ENTITY_DORMANT )
	{
		GameEntitySystem()->AddEntityName( m_pEntity );
		m_pEntity->m_pClass->LinkToClassList( m_pEntity );
	}
	else
	{
		GameEntitySystem()->RemoveEntityName( m_pEntity );
		m_pEntity->m_pClass->UnlinkFromClassList( m_pEntity );
	}
}

void CEntityInstance::ReloadPrivateScripts()
{
	const char *pszScripts = m_iszPrivateVScripts.String();

	if ( !pszScripts )
		return;

	CUtlVector< CUtlString > scripts;

	V_SplitString( pszScripts, " ", scripts );

	for ( const CUtlString &script : scripts )
	{
		Log_Detailed( LOG_VSCRIPT, "%s executing script: %s\n", GetDebugName(), script.String() );

		if ( !m_hPrivateScope.m_hScope && g_pScriptVM )
		{
			m_hPrivateScope.m_hScope = g_pScriptVM->CreateScope( nullptr, nullptr );

			if ( m_hPrivateScope.m_hScope )
				InitPrivateScriptScope();
		}

		VScriptRunScript( script.String(), m_hPrivateScope.m_hScope, true );
	}
}

void CEntityInstance::InitPrivateScriptScope()
{
	Assert( g_pScriptVM );

	// Stored even when the public scope could not be created
	ScriptVariant_t thisEntity( ScriptGetOrCreatePublicScriptScope() );

	g_pScriptVM->SetValue( m_hPrivateScope.m_hScope, "thisEntity", thisEntity );
}

ScriptClassDesc_t *CEntityInstance::GetScriptDesc()
{
	return ::GetScriptDesc( this );
}

void CEntityInstance::ScriptFireOutput( const char *pszOutputName, HSCRIPT hActivator, HSCRIPT hCaller, CVariant value, float32 flDelay )
{
	Assert( g_pScriptVM );

	ScriptClassDesc_t *pDesc = ::GetScriptDesc( static_cast< CEntityInstance * >( nullptr ) );

	// An unresolved activator or caller falls back to this entity
	CEntityInstance *pActivator = hActivator ? static_cast< CEntityInstance * >( g_pScriptVM->GetInstanceValue( hActivator, pDesc ) ) : nullptr;
	CEntityInstance *pCaller = hCaller ? static_cast< CEntityInstance * >( g_pScriptVM->GetInstanceValue( hCaller, pDesc ) ) : nullptr;

	if ( !pActivator )
		pActivator = this;

	if ( !pCaller )
		pCaller = this;

	FireOutput( pszOutputName, pActivator, pCaller, value, flDelay );
}

void CEntityInstance::FireOutputInternal( const char *pszOutputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const CPulseArgumentPack *pArgs, const CPulseInputParamMap *pParamMap, const CVariant *pValue, float flDelay )
{
	CUtlVector< CEntityIOOutput * > outputs;

	FindOutputs( pszOutputName, outputs );

	for ( CEntityIOOutput *pOutput : outputs )
		pOutput->FireOutputInternal( pActivator, pCaller, pArgs, pParamMap, pValue, flDelay );
}

void CEntityInstance::FireOutput( const char *pszOutputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const CVariant &value, float flDelay )
{
	// The outputs only read the pack, so one serves them all
	CPulseArgumentPack args;

	args.SetParamFromVariant( value );

	FireOutputInternal( pszOutputName, pActivator, pCaller, &args, nullptr, &value, flDelay );
}

void CEntityInstance::FireOutput( const char *pszOutputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const CVariant &value, const KeyValues3 &params, float flDelay )
{
	CPulseArgumentPack args;
	CPulseInputParamMap paramMap;

	args.SetParamFromVariant( value );
	paramMap.m_KV3 = params;

	FireOutputInternal( pszOutputName, pActivator, pCaller, &args, &paramMap, &value, flDelay );
}

bool CEntityInstance::AcceptInputInternal( const CUtlSymbolLarge &sInputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const variant_t &value, const CPulseArgumentPack *pArgs, const CPulseInputParamMap *pParamMap )
{
	Assert( m_pEntity );
	Assert( m_pEntity->m_pClass );

	if ( m_pEntity->m_pClass->AcceptInput( this, sInputName, pActivator, pCaller, value, pArgs, pParamMap ) )
		return true;

	const AcceptInputResult_t result = ScriptAcceptInput( sInputName, pActivator, pCaller, value, pArgs, pParamMap );

	if ( result != ACCEPT_INPUT_NONE )
		return result == ACCEPT_INPUT_HANDLED;

	CScriptComponent *pComponent = m_CScriptComponent;
	HSCRIPT hPrivateScope = m_hPrivateScope.m_hScope;

	if ( !pComponent && !hPrivateScope )
		return false;

	IScriptVM *pScriptVM = g_pScriptVM;

	Assert( pScriptVM );

	ScriptVariant_t args;

	pScriptVM->CreateTable( args );

	HSCRIPT hArgs = args.m_hScript;

	pScriptVM->SetValue( hArgs, "activator", ScriptVariant_t( pActivator ? pActivator->GetScriptInstance() : nullptr ) );
	pScriptVM->SetValue( hArgs, "caller", ScriptVariant_t( pCaller ? pCaller->GetScriptInstance() : nullptr ) );
	pScriptVM->SetValue( hArgs, "value", value );
	pScriptVM->SetValue( hArgs, "outputid", static_cast< const char * >( nullptr ) );

	const char *pszInputName = sInputName.String();
	bool bHandled = false;

	if ( pComponent )
		bHandled = pComponent->DispatchInput( pszInputName, hArgs );

	if ( hPrivateScope )
	{
		char szFunction[ 255 ];

		V_strncpy( szFunction, "Input", sizeof( szFunction ) );
		V_strncat( szFunction, pszInputName ? pszInputName : "", sizeof( szFunction ) );

		if ( HSCRIPT hFunction = pScriptVM->LookupFunction( szFunction, hPrivateScope, true ) )
		{
			ScriptVariant_t arg( hArgs );
			ScriptVariant_t ret;

			const ScriptStatus_t status = pScriptVM->ExecuteFunction( hFunction, &arg, 1, &ret, nullptr, true );

			pScriptVM->ReleaseFunction( hFunction );

			bHandled |= status != SCRIPT_ERROR;
		}
	}

	pScriptVM->ReleaseScope( hArgs );

	return bHandled;
}

bool CEntityInstance::AcceptInput( const char *pszInputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const variant_t &value )
{
	const CUtlSymbolLarge sInputName = GameEntitySystem()->AllocPooledString( pszInputName );

	CPulseInputParamMap paramMap;
	CPulseArgumentPack args;

	args.SetParamFromVariant( value );

	return AcceptInputInternal( sInputName, pActivator, pCaller, value, &args, &paramMap );
}

bool CEntityInstance::AcceptInput( const char *pszInputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const variant_t &value, const KeyValues3 &params )
{
	const CUtlSymbolLarge sInputName = GameEntitySystem()->AllocPooledString( pszInputName );

	CPulseInputParamMap paramMap;
	CPulseArgumentPack args;

	paramMap.m_KV3 = params;
	args.SetParamFromVariant( value );

	return AcceptInputInternal( sInputName, pActivator, pCaller, value, &args, &paramMap );
}

void CEntityInstance::RemoveSelf()
{
	CEntitySystem *pEntitySystem = GameEntitySystem();

	Assert( pEntitySystem );

	// The server controls the lifespan of a networked entity
	if ( pEntitySystem->m_eNetworkSerializationMode != NET_SERIALIZATION_MODE_SERVER && m_pEntity->IsNetworked() )
	{
		Log_Warning( LOG_VSCRIPT, "Tried to destroy (from script) a networked entity (%d/%s) on the client, the server controls lifespan, ignoring\n", ScriptGetEntityIndex(), GetClassNameAsCStr() );

		return;
	}

	pEntitySystem->QueueDestroyEntity( this );
}

int32 CEntityInstance::GetIntAttr( const char *pszName ) const
{
	const ObjectAttributeValue_t *pValue = EnsureAttributes()->FindValue( MakeAttributeKey( pszName ) );

	if ( !pValue )
		return 0;

	if ( pValue->IsInt() )
		return pValue->GetInt();

	if ( pValue->IsFloat() )
		return static_cast< int32 >( pValue->GetFloat() );

	return 0;
}

void CEntityInstance::SetIntAttr( const char *pszName, int32 nValue )
{
	EnsureAttributes()->SetInt( MakeAttributeKey( pszName ), nValue, pszName );

	OnAttributeChanged();
}

const char *CEntityInstance::GetStringAttr( const char *pszName ) const
{
	const ObjectAttributeValue_t *pValue = m_pEntity->FindAttribute( MakeAttributeKey( pszName ) );

	return pValue ? pValue->GetString() : "";
}

void CEntityInstance::SetStringAttr( const char *pszName, const char *pszValue )
{
	EnsureAttributes()->SetString( MakeAttributeKey( pszName ), pszValue, pszName );

	OnAttributeChanged();
}

float32 CEntityInstance::GetFloatAttr( const char *pszName ) const
{
	const ObjectAttributeValue_t *pValue = m_pEntity->FindAttribute( MakeAttributeKey( pszName ) );

	return pValue ? pValue->GetFloat() : 0.f;
}

void CEntityInstance::SetFloatAttr( const char *pszName, float32 flValue )
{
	EnsureAttributes()->SetFloat( MakeAttributeKey( pszName ), flValue, pszName );

	OnAttributeChanged();
}

uint64 CEntityInstance::GetUInt64Attr( const char *pszName ) const
{
	const ObjectAttributeValue_t *pValue = m_pEntity->FindAttribute( MakeAttributeKey( pszName ) );

	return pValue ? pValue->GetUInt64() : 0;
}

void CEntityInstance::SetUInt64Attr( const char *pszName, uint64 nValue )
{
	EnsureAttributes()->SetUInt64( MakeAttributeKey( pszName ), nValue, pszName );

	OnAttributeChanged();
}

void *CEntityInstance::GetPointerAttr( const char *pszName ) const
{
	const ObjectAttributeValue_t *pValue = m_pEntity->FindAttribute( MakeAttributeKey( pszName ) );

	return pValue ? pValue->GetPointer() : nullptr;
}

void CEntityInstance::SetPointerAttr( const char *pszName, void *pValue )
{
	EnsureAttributes()->SetPointer( MakeAttributeKey( pszName ), pValue, pszName );

	OnAttributeChanged();
}

void CEntityInstance::FindOutputs( const char *pszOutputName, CUtlVector< CEntityIOOutput * > &outputs )
{
	if ( CEntityIOOutput *pOutput = m_pEntity->m_pClass->FindOutput( pszOutputName, this ) )
		outputs.AddToTail( pOutput );

	if ( CDynamicIOInstance *pDynamicIO = ScriptEntityIO() )
		pDynamicIO->FindOutputs( pszOutputName, outputs );

	if ( m_CScriptComponent )
		m_CScriptComponent->FindScriptOutputs( pszOutputName, outputs );
}

void CEntityInstance::ConnectOutputToScript( const char *pszOutputName, const char *pszFunctionName, HSCRIPT hEntity )
{
	CUtlVector< CEntityIOOutput * > outputs;

	FindOutputs( pszOutputName, outputs );

	if ( !outputs.Count() )
	{
		Log_Detailed( LOG_VSCRIPT, "Script failed to find output \"%s\"\n", pszOutputName );

		return;
	}

	CEntitySystem *pEntitySystem = GameEntitySystem();

	Assert( pEntitySystem );

	const char *pszValueOverride = pEntitySystem->AllocPooledString( pszFunctionName ).String();

	EntityIOConnection_t *pConnection = Create< EntityIOConnection_t >();

	pConnection->m_targetDesc = NULL_STRING;
	pConnection->m_targetInput = MAKE_STRING( pEntitySystem->AllocPooledString( "CallScriptFunction" ).String() );
	pConnection->m_valueOverride = MAKE_STRING( pszValueOverride );
	pConnection->m_nTargetType = ENTITY_IO_TARGET_EHANDLE;
	pConnection->m_nTimesToFire = -1;
	pConnection->m_flDelay = 0.f;
	pConnection->m_paramMap.m_bForwardAllArgs = false;
	pConnection->m_bMarkedForRemoval = false;

	// Without hEntity the target stays invalid
	pConnection->m_hTarget = CEntityHandle();

	if ( hEntity )
	{
		CEntityInstance *pTarget = static_cast< CEntityInstance * >( g_pScriptVM->GetInstanceValue( hEntity, ::GetScriptDesc( static_cast< CEntityInstance * >( nullptr ) ) ) );

		if ( pTarget && pTarget->m_pEntity )
			pConnection->m_hTarget = pTarget->GetRefEHandle();
	}

	// TODO(@Wend4r): Implement kv3lib stuff.
	if ( *pszValueOverride )
		pConnection->m_paramMap.m_KV3.FindOrCreateMember( "--old-connection-literal--" )->FindOrCreateMember( "value" )->SetString( pszValueOverride );

	outputs[ 0 ]->AddConnection( pConnection );
}

void CEntityInstance::DisconnectOutputFromScript( const char *pszOutputName, const char *pszFunctionName, HSCRIPT hEntity )
{
	CUtlVector< CEntityIOOutput * > outputs;

	FindOutputs( pszOutputName, outputs );

	if ( !outputs.Count() )
	{
		DevMsg( 2, "Script failed to find output \"%s\"\n", pszOutputName );

		return;
	}

	CEntitySystem *pEntitySystem = GameEntitySystem();

	Assert( pEntitySystem );

	const string_t targetInput = MAKE_STRING( pEntitySystem->AllocPooledString( "CallScriptFunction" ).String() );

	// Without hEntity the connection targets this entity
	CEntityHandle hTarget;

	if ( hEntity )
	{
		CEntityInstance *pTarget = static_cast< CEntityInstance * >( g_pScriptVM->GetInstanceValue( hEntity, ::GetScriptDesc( static_cast< CEntityInstance * >( nullptr ) ) ) );

		if ( pTarget && pTarget->m_pEntity )
			hTarget = pTarget->GetRefEHandle();
	}
	else if ( m_pEntity )
	{
		hTarget = GetRefEHandle();
	}

	// Only the first connection to the target in each output is checked, and only the first match is removed
	for ( CEntityIOOutput *pOutput : outputs )
	{
		EntityIOConnection_t *pConnection = pOutput->FindConnection( hTarget );

		if ( pConnection && IDENT_STRINGS( pConnection->m_targetInput, targetInput ) && !V_strcmp( STRING( pConnection->m_valueOverride ), pszFunctionName ) )
		{
			pOutput->MarkConnectionForRemoval( pConnection );

			return;
		}
	}
}

int32 CEntityInstance::ScriptGetEntityIndex()
{
	if ( !m_pEntity )
		return -1;

	return GameEntitySystem()->GetEntityIndex( m_pEntity ).Get();
}

HSCRIPT CEntityInstance::ScriptGetOrCreatePublicScriptScope()
{
	if ( !m_pEntity->m_hPublicScope )
		m_pEntity->m_hPublicScope = reinterpret_cast< uint64 >( CreatePublicScriptScope() );

	return reinterpret_cast< HSCRIPT >( m_pEntity->m_hPublicScope );
}

HSCRIPT CEntityInstance::CreatePublicScriptScope()
{
	if ( !g_pScriptVM )
		return nullptr;

	ScriptVariant_t framework;
	bool bHasFramework = false;

	g_pScriptVM->GetValue( nullptr, "EntityFramework", &framework );

	if ( framework.m_type != FIELD_VOID )
	{
		bHasFramework = framework.m_hScript != nullptr;

		g_pScriptVM->ReleaseValue( framework );
	}

	if ( !bHasFramework )
		return nullptr;

	return g_pScriptVM->RegisterInstance( m_pEntity->m_pClass->m_pScriptDesc, this );
}

HSCRIPT CEntityInstance::ScriptGetOrCreatePrivateScriptScope()
{
	if ( m_hPrivateScope.m_hScope || !g_pScriptVM )
		return m_hPrivateScope.m_hScope;

	m_hPrivateScope.m_hScope = g_pScriptVM->CreateScope( nullptr, nullptr );

	if ( m_hPrivateScope.m_hScope )
		InitPrivateScriptScope();

	return m_hPrivateScope.m_hScope;
}
