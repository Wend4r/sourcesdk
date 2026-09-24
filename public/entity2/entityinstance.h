#ifndef ENTITYINSTANCE_H
#define ENTITYINSTANCE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier1/utlsymbollarge.h"
#include "entity2/entitycomponent.h"
#include "entity2/entityidentity.h"
#include "entitytypes.h"
#include "entity2/entityprivatescriptscope.h"
#include "schemasystem/schematypes.h"
#include "variant.h"
#include "vscript_shared.h"

class CEntityClass;
class CEntityKeyValues;
class CFieldPath;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class ISave;
class IRestore;
class CPulseArgumentPack;
class CPulseInputParamMap;
class KeyValues3;
class CDynamicIOInstance;
class CEntityIOOutput;
class CScriptComponent;
struct CEntityPrecacheContext;
struct ChangeAccessorFieldPathIndexInfo_t;
struct datamap_t;
class IScriptVM;
class CNetworkSerializerClassInfo;
struct NetworkSharedChangeInfoOverflow_t;

// See entitynetwork.h
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

struct DebugTextState_t;

extern IScriptVM *ScriptVM();

abstract_class IEntityVisitor
{
public:
	// The target function signature depends on the field
	virtual void Visit( CEntityInstance *pEntity, void *pField ) = 0;
};

enum AcceptInputResult_t
{
	ACCEPT_INPUT_NONE = 0,
	ACCEPT_INPUT_UNHANDLED,
	ACCEPT_INPUT_HANDLED,
};

class CEntityInstance
{
public:
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() = 0;

	// Custom KV3 save/restore for members the datamap cannot describe
	// TODO(@Wend4r): Implement kv3lib stuff
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const {}
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) {}

	DECLARE_ENT_SCRIPTDESC();

	// Releases the key values and both script scopes
	virtual ~CEntityInstance();

	// The base versions only mark that they were reached
	virtual void Connect() {}
	virtual void Disconnect() {}

	virtual void Precache( const CEntityPrecacheContext *pContext );
	virtual void AddedToEntityDatabase() {}

	virtual void Spawn( const CEntityKeyValues *pKeyValues );

	virtual void DispatchPostDataUpdate( DataUpdateType_t eUpdateType ) { PostDataUpdate( eUpdateType ); }
	virtual void PostDataUpdate( DataUpdateType_t eUpdateType ) {}
	virtual void OnDataUnchangedInPVS() {}

	virtual void Activate( ActivateType_t activateType );
	virtual void UpdateOnRemove();

	// Moves the entity between the active and dormant lists
	virtual void OnSetDormant( EntityDormancyType_t prevDormancyType, EntityDormancyType_t newDormancyType );

	// Dynamically added output connections of the pulse graph, if any
	virtual CDynamicIOInstance *ScriptEntityIO() { return nullptr; }

	// Handles an input the entity class did not handle. The entity override forwards it to the pulse graph
	virtual AcceptInputResult_t ScriptAcceptInput( const CUtlSymbolLarge &sInputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const variant_t &value, const CPulseArgumentPack *pPulseArguments, const CPulseInputParamMap *pParamMap ) { return ACCEPT_INPUT_NONE; }

	virtual void PreDataUpdate( DataUpdateType_t updateType ) {}

	virtual void DrawEntityDebugOverlays( uint64 nDebugBits ) {}
	virtual void DrawDebugTextOverlays( DebugTextState_t &state, uint64 nDebugBits, int nFlags ) {}

	// save/restore stuff
	virtual int Save( ISave &save ) { return 1; }
	virtual int Restore( IRestore &restore ) { return 1; }
	virtual void OnSave() {}
	virtual void OnRestore() {}

	// Reconstructed name. Visits every stored member function pointer of the entity
	virtual void EnumerateVisitor( IEntityVisitor *pVisitor ) {}

	// capabilities for save/restore
	virtual int ObjectCaps() { return 0; }

	virtual CEntityIndex RequiredEdictIndex() { return CEntityIndex(); }

	// Include "entity2/entitynetwork.h" to call methods
	// Marks a field for transmission over the network
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data ) {} // Function replaces old version NetworkStateChanged( uint nOffset, int, ChangeAccessorFieldPathIndex_t PathIndex )
	virtual void NetworkStateChangedBranch( const CFieldPath &path ) {}
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data ) {}

	virtual void NetworkUpdateState( bool bNetworkUpdatesDisabled ) {}
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo ) {}
	virtual bool FullEdictChanged() { return false; }

	virtual void InvalidatePolymorphicMetadataHelper() {}
	virtual void Unk_36() {} // Empty and never overridden

	virtual ChangeAccessorFieldPathIndex_t AddChangeAccessorPath( const CFieldPath &path ) { return ChangeAccessorFieldPathIndex_t(); }
	virtual void AssignChangeAccessorPathIds() {}

	virtual NetworkSharedChangeInfoOverflow_t *GetChangeAccessorPathInfo() { return nullptr; }
	virtual const NetworkSharedChangeInfoOverflow_t *GetChangeAccessorPathInfo() const { return nullptr; }

	virtual bool GetFieldPathChildIndices( const CFieldPath &path, CUtlVector< int > *pOutChildIndices ) { return false; }
	virtual bool Unk_42() { return false; } // Returns false and never overridden

	virtual void OnAttributeChanged() {}

	virtual void ReloadPrivateScripts();
	virtual datamap_t *GetDataDescMap() { return nullptr; }

	virtual CEntityComponent *FindComponent( const CUtlSymbolLarge &sComponentClassName ) { return nullptr; }

	virtual void UpdateNestedWorldGroupIds() {}

	// TODO(@Wend4r): Implement schemacompiler2 stuff
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;

public:
	CEntityHandle GetRefEHandle() const { return m_pEntity->GetRefEHandle(); }
	const char *GetName() const { return m_pEntity->GetName(); }
	const char *GetClassname() const { return m_pEntity->GetClassname(); }
	CEntityIndex GetEntityIndex() const { return m_pEntity->GetEntityIndex(); }

	// The script instance is the public script scope
	HSCRIPT GetScriptInstance() { return ScriptGetOrCreatePublicScriptScope(); }
	
	void FireOutputInternal( const char *pszOutputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const CPulseArgumentPack *pArgs, const CPulseInputParamMap *pParamMap, const CVariant *pValue, float flDelay );
	void FireOutput( const char *pszOutputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const CVariant &value, float flDelay );
	void FireOutput( const char *pszOutputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const CVariant &value, const KeyValues3 &params, float flDelay );

	void ScriptFireOutput( const char *pszOutputName, HSCRIPT hActivator, HSCRIPT hCaller, CVariant value, float32 flDelay );

	// Dispatches an input to the entity class, the pulse graph and scripts; true when handled
	bool AcceptInputInternal( const CUtlSymbolLarge &sInputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const variant_t &value, const CPulseArgumentPack *pArgs, const CPulseInputParamMap *pParamMap );
	bool AcceptInput( const char *pszInputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const variant_t &value );
	bool AcceptInput( const char *pszInputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const variant_t &value, const KeyValues3 &params );

	void RemoveSelf();

	// Attribute keys are case-insensitive. GetIntAttr also reads a float attribute
	int32 GetIntAttr( const char *pszName ) const;
	void SetIntAttr( const char *pszName, int32 nValue );

	// Not bound to script. A getter returns an empty value for another type
	const char *GetStringAttr( const char *pszName ) const;
	void SetStringAttr( const char *pszName, const char *pszValue );

	float32 GetFloatAttr( const char *pszName ) const;
	void SetFloatAttr( const char *pszName, float32 flValue );

	uint64 GetUInt64Attr( const char *pszName ) const;
	void SetUInt64Attr( const char *pszName, uint64 nValue );

	void *GetPointerAttr( const char *pszName ) const;
	void SetPointerAttr( const char *pszName, void *pValue );

	const char *GetEntityNameAsCStr() { return m_pEntity->GetName(); }
	const char *GetDebugName() { return m_pEntity->GetDebugName(); }
	const char *GetClassNameAsCStr() { return m_pEntity->GetClassname(); }

	void ConnectOutputToScriptSelf( const char *pszOutputName, const char *pszFunctionName ) { ConnectOutputToScript( pszOutputName, pszFunctionName, nullptr ); }
	void ConnectOutputToScript( const char *pszOutputName, const char *pszFunctionName, HSCRIPT hEntity );
	void DisconnectOutputFromScriptSelf( const char *pszOutputName, const char *pszFunctionName ) { DisconnectOutputFromScript( pszOutputName, pszFunctionName, nullptr ); }
	void DisconnectOutputFromScript( const char *pszOutputName, const char *pszFunctionName, HSCRIPT hEntity );

	// -1 when the entity is not in the entity list
	int32 ScriptGetEntityIndex();
	CEntityHandle ScriptGetEHandle() { return m_pEntity ? m_pEntity->GetRefEHandle() : CEntityHandle(); }

	HSCRIPT ScriptGetPublicScriptScope() { return reinterpret_cast< HSCRIPT >( m_pEntity->m_hPublicScope ); }
	HSCRIPT ScriptGetOrCreatePublicScriptScope();
	HSCRIPT ScriptGetPrivateScriptScope() { return m_hPrivateScope.m_hScope; }
	HSCRIPT ScriptGetOrCreatePrivateScriptScope();

protected:
	static CEntityIdentity::AttributeKey_t MakeAttributeKey( const char *pszName ) { return CEntityIdentity::MakeAttributeKey( pszName ); }
	CEntityIdentity::AttributeTable_t *EnsureAttributes() const { return m_pEntity->EnsureAttributes(); }

	// Exposes the public script scope to the private one
	void InitPrivateScriptScope();

	// Null until the entity framework is ready
	HSCRIPT CreatePublicScriptScope();

	void FindOutputs( const char *pszOutputName, CUtlVector< CEntityIOOutput * > &outputs );

	// Also releases the script component; the caller clears the handle
	void ReleasePublicScriptScope();

public:
	CUtlSymbolLarge m_iszPrivateVScripts;
	CEntityIdentity *m_pEntity;
	CEntityPrivateScriptScope m_hPrivateScope;
	CEntityKeyValues *m_pKeyValues;
	CScriptComponent *m_CScriptComponent;
};

#endif // ENTITYINSTANCE_H
