#ifndef ENTITYINSTANCE_H
#define ENTITYINSTANCE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/utlsymbollarge.h"
#include "entity2/entitycomponent.h"
#include "entity2/entityidentity.h"
#include "entitytypes.h"
#include "entity2/entityprivatescriptscope.h"
#include "kv3lib/kv3transfer_constants.h"
#include "schemasystem/schematypes.h"
#include "entity2/variant.h"
#include "vscript_shared.h"

#include "schemacompiler/schemaon.h"

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

// The debug overlays an entity draws (ent_text, ent_bbox and the like), as bits.
schema enum DebugOverlayBits_t : uint64
{
	OVERLAY_TEXT_BIT = ( 1 << 0 ),
	OVERLAY_NAME_BIT = ( 1 << 1 ),
	OVERLAY_BBOX_BIT = ( 1 << 2 ),
	OVERLAY_PIVOT_BIT = ( 1 << 3 ),
	OVERLAY_MESSAGE_BIT = ( 1 << 4 ),
	OVERLAY_ABSBOX_BIT = ( 1 << 5 ),
	OVERLAY_RBOX_BIT = ( 1 << 6 ),
	OVERLAY_SHOW_BLOCKSLOS = ( 1 << 7 ),
	OVERLAY_ATTACHMENTS_BIT = ( 1 << 8 ),
	OVERLAY_INTERPOLATED_ATTACHMENTS_BIT = ( 1 << 9 ),
	OVERLAY_INTERPOLATED_PIVOT_BIT = ( 1 << 10 ),
	OVERLAY_SKELETON_BIT = ( 1 << 11 ),
	OVERLAY_INTERPOLATED_SKELETON_BIT = ( 1 << 12 ),
	OVERLAY_TRIGGER_BOUNDS_BIT = ( 1 << 13 ),
	OVERLAY_HITBOX_BIT = ( 1 << 14 ),
	OVERLAY_INTERPOLATED_HITBOX_BIT = ( 1 << 15 ),
	OVERLAY_AUTOAIM_BIT = ( 1 << 16 ),
	OVERLAY_NPC_SELECTED_BIT = ( 1 << 17 ),
	OVERLAY_JOINT_INFO_BIT = ( 1 << 18 ),
	OVERLAY_NPC_ROUTE_BIT = ( 1 << 19 ),
	OVERLAY_VISIBILITY_TRACES_BIT = ( 1 << 20 ),
	OVERLAY_NPC_ENEMIES_BIT = ( 1 << 22 ),
	OVERLAY_NPC_CONDITIONS_BIT = ( 1 << 23 ),
	OVERLAY_NPC_COMBAT_BIT = ( 1 << 24 ),
	OVERLAY_NPC_TASK_BIT = ( 1 << 25 ),
	OVERLAY_NPC_BODYLOCATIONS = ( 1 << 26 ),
	OVERLAY_NPC_VIEWCONE_BIT = ( 1 << 27 ),
	OVERLAY_NPC_KILL_BIT = ( 1 << 28 ),
	OVERLAY_BUDDHA_MODE = ( 1 << 30 ),
	OVERLAY_NPC_STEERING_REGULATIONS = ( 1ull << 31 ),
	OVERLAY_NPC_TASK_TEXT_BIT = ( 1ull << 32 ),
	OVERLAY_PROP_DEBUG = ( 1ull << 33 ),
	OVERLAY_NPC_RELATION_BIT = ( 1ull << 34 ),
	OVERLAY_VIEWOFFSET = ( 1ull << 35 ),
	OVERLAY_VCOLLIDE_WIREFRAME_BIT = ( 1ull << 36 ),
	OVERLAY_NPC_SCRIPTED_COMMANDS_BIT = ( 1ull << 37 ),
	OVERLAY_ACTORNAME_BIT = ( 1ull << 38 ),
	OVERLAY_NPC_CONDITIONS_TEXT_BIT = ( 1ull << 39 ),
	OVERLAY_NPC_ABILITY_RANGE_DEBUG_BIT = ( 1ull << 40 ),
	OVERLAY_MINIMAL_TEXT = ( 1ull << 41 ),
	OVERLAY_NPC_GOD_MODE = ( 1ull << 42 ),
	OVERLAY_NPC_ANIM_AI_HANDSHAKES_BIT = ( 1ull << 43 ),
	OVERLAY_NPC_PATH_QUERIES_BIT = ( 1ull << 44 ),
};

// The nFlags of CEntityInstance::DrawDebugTextOverlays: the ent_text_filter keywords of the same
// names, from ent_text_flags_active.
#define ENT_TEXT_FILTER_NAME ( 1 << 0 )
#define ENT_TEXT_FILTER_HEALTH ( 1 << 1 )
#define ENT_TEXT_FILTER_GLOBNAME ( 1 << 2 )
#define ENT_TEXT_FILTER_POS ( 1 << 3 )
#define ENT_TEXT_FILTER_FULLPOS ( 1 << 4 )
#define ENT_TEXT_FILTER_LOS ( 1 << 5 )
#define ENT_TEXT_FILTER_MODEL ( 1 << 6 )
#define ENT_TEXT_FILTER_CONTEXT ( 1 << 7 )
#define ENT_TEXT_FILTER_FLAGS ( 1 << 8 )
#define ENT_TEXT_FILTER_EFFECTS ( 1 << 9 )
#define ENT_TEXT_FILTER_SCRIPT ( 1 << 10 )
#define ENT_TEXT_FILTER_ANIM ( 1 << 11 )
#define ENT_TEXT_FILTER_PHYS ( 1 << 12 )
#define ENT_TEXT_FILTER_PLAYER ( 1 << 13 )
#define ENT_TEXT_FILTER_AI ( 1 << 14 )
#define ENT_TEXT_FILTER_OTHER ( 1 << 15 )
#define ENT_TEXT_FILTER_CLOTH ( 1 << 16 )
#define ENT_TEXT_FILTER_GRAPH_STATES ( 1 << 17 )
#define ENT_TEXT_FILTER_GRAPH_PARAM ( 1 << 18 )
#define ENT_TEXT_FILTER_MODIFIERS ( 1 << 19 )
// Not a keyword: set by DrawEntityDebugOverlays, which clears the two graph filters; the name
// line is drawn only with it.
#define ENT_TEXT_FILTER_ENTITY_OVERLAYS ( 1 << 20 )

// CEntityInstance::ObjectCaps bits; the Source 1 FCAP_ bits, renumbered.
// +use: fires Use when the use key is pressed
#define FCAP_IMPULSE_USE ( 1 << 0 )
// +use: fires Use every frame while the use key is held
#define FCAP_CONTINUOUS_USE ( 1 << 1 )
// +use: fires Use with 1 when the use key is pressed and 0 when it is released
#define FCAP_ONOFF_USE ( 1 << 2 )
// Usable from within a radius; set by doors with +use and by weapons
#define FCAP_USE_IN_RADIUS ( 1 << 5 )
// Carried across a level transition; the base class's default, cleared by point entities
#define FCAP_ACROSS_TRANSITION ( 1 << 7 )
// Carried across a level transition even outside the transition volume
#define FCAP_FORCE_TRANSITION ( 1 << 8 )
// Receives InsideTransition or OutsideTransition when a level transition happens
#define FCAP_NOTIFY_ON_TRANSITION ( 1 << 9 )
// Can be the master of other entities
#define FCAP_MASTER ( 1 << 28 )
// Set by items
#define FCAP_WCEDIT_POSITION ( 1 << 30 )
// Not saved
#define FCAP_DONT_SAVE ( 1u << 31 )

enum AcceptInputResult_t
{
	ACCEPT_INPUT_NONE = 0,
	ACCEPT_INPUT_UNHANDLED,
	ACCEPT_INPUT_HANDLED,
};

schema class CEntityInstance
{
public:
	TYPEMETA( MConstructibleClassBase );

	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() = 0;

	// Custom KV3 save/restore for members the datamap cannot describe; the base versions transfer nothing
	CLASS_USES_KV3TRANSFER_VIRTUAL( CEntityInstance );

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

	virtual void DrawEntityDebugOverlays( DebugOverlayBits_t nDebugBits ) {}
	// nFlags holds ENT_TEXT_FILTER_* bits.
	virtual void DrawDebugTextOverlays( DebugTextState_t &state, DebugOverlayBits_t nDebugBits, int nFlags ) {}

	// save/restore stuff
	virtual int Save( ISave &save ) { return 1; }
	virtual int Restore( IRestore &restore ) { return 1; }
	virtual void OnSave() {}
	virtual void OnRestore() {}

	// Reconstructed name. Visits every stored member function pointer of the entity
	virtual void EnumerateVisitor( IEntityVisitor *pVisitor ) {}

	// capabilities for save/restore
	// FCAP_* bits
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
	virtual void unk036() {} // Empty and never overridden

	virtual ChangeAccessorFieldPathIndex_t AddChangeAccessorPath( const CFieldPath &path ) { return ChangeAccessorFieldPathIndex_t(); }
	virtual void AssignChangeAccessorPathIds() {}

	virtual NetworkSharedChangeInfoOverflow_t *GetChangeAccessorPathInfo() { return nullptr; }
	virtual const NetworkSharedChangeInfoOverflow_t *GetChangeAccessorPathInfo() const { return nullptr; }

	virtual bool GetFieldPathChildIndices( const CFieldPath &path, CUtlVector< int > *pOutChildIndices ) { return false; }
	virtual bool unk042() { return false; } // Returns false and never overridden

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
	noschema CEntityPrivateScriptScope m_hPrivateScope;
	noschema CEntityKeyValues *m_pKeyValues;
	CScriptComponent *m_CScriptComponent;
};

#include "schemacompiler/schemaoff.h"

#endif // ENTITYINSTANCE_H
