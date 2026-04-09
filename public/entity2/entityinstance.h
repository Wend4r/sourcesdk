#ifndef ENTITYINSTANCE_H
#define ENTITYINSTANCE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier1/utlsymbollarge.h"
#include "entity2/entitycomponent.h"
#include "entity2/entityidentity.h"
#include "variant.h"
#include "schemasystem/schematypes.h"

class CEntityKeyValues;
class CFieldPath;
class ISave;
class IRestore;
struct CEntityPrecacheContext;
struct ChangeAccessorFieldPathIndexInfo_t;
struct datamap_t;
class IScriptVM;
class CNetworkSerializerClassInfo;
struct NetworkSharedChangeInfoOverflow_t;

// See entitynetwork.h
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

extern IScriptVM* ScriptVM();

class CEntityPrivateScriptScope
{
public:
	HSCRIPT m_hScope;
};

class CEntityInstance
{
public:
#ifdef CS2_BETA
	virtual const CNetworkSerializerClassInfo* GetSerializerClassInfo() = 0;
#endif

	virtual void unk001() = 0; // CDebugHistory override serializes iVersion/Categories/m_DebugLines
	virtual void unk002() = 0; // CDebugHistory override deserializes iVersion/Categories/m_DebugLines

	virtual ScriptClassDesc_t* GetScriptDesc() = 0;
	
	virtual ~CEntityInstance() = 0;
	
	virtual void Connect() = 0;
	virtual void Disconnect() = 0;
	virtual void Precache( const CEntityPrecacheContext* pContext ) = 0;
	virtual void AddedToEntityDatabase() = 0;
	virtual void Spawn( const CEntityKeyValues* pKeyValues ) = 0;

	virtual void unk101() = 0; // No child overrides found

	virtual void PostDataUpdate( /*DataUpdateType_t*/int updateType ) = 0;
	virtual void OnDataUnchangedInPVS() = 0;
	virtual void Activate( /*ActivateType_t*/int activateType ) = 0;
	virtual void UpdateOnRemove() = 0;
	virtual void OnSetDormant( /*EntityDormancyType_t*/int prevDormancyType, /*EntityDormancyType_t*/int newDormancyType ) = 0;

	virtual void* ScriptEntityIO() = 0;
	virtual int ScriptAcceptInput( const CUtlSymbolLarge &sInputName, CEntityInstance* pActivator, CEntityInstance* pCaller, const variant_t &value, int nOutputID, void* pUnk1, void* pUnk2 ) = 0;
	
	virtual void PreDataUpdate( /*DataUpdateType_t*/int updateType ) = 0;
	
	virtual void DrawEntityDebugOverlays( uint64 debug_bits ) = 0;
	virtual void DrawDebugTextOverlays( void* unk, uint64 debug_bits, int flags ) = 0;
	
	virtual int Save( ISave &save ) = 0;
	virtual int Restore( IRestore &restore ) = 0;
	virtual void OnSave() = 0;
	virtual void OnRestore() = 0;
	
	virtual void unk201() = 0; // No child overrides found

	virtual int ObjectCaps() = 0;
	virtual CEntityIndex RequiredEdictIndex() = 0;

	// Include "entity2/entitynetwork.h" to call methods.
	// marks a field for transmission over the network
	virtual void NetworkStateChanged( const NetworkStateChanged_t& data ) = 0; // Function replaces old version NetworkStateChanged( uint nOffset, int, ChangeAccessorFieldPathIndex_t PathIndex )
	virtual void NetworkStateChangedBranch( const CFieldPath& path ) = 0;
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t& data ) = 0;

	// Toggles the early-out in CNetworkTransmitComponent::StateChanged; true disables network updates.
	virtual void NetworkUpdateState( bool bNetworkUpdatesDisabled ) = 0;
	virtual void NetworkStateChangedLog( const char* pszFieldName, const char* pszInfo ) = 0;
	virtual bool FullEdictChanged() = 0;

	virtual void InvalidatePolymorphicMetadataHelper() = 0;
	virtual void unk402() = 0; // nullsub

	virtual ChangeAccessorFieldPathIndex_t AddChangeAccessorPath( const CFieldPath& path ) = 0;
	virtual void AssignChangeAccessorPathIds() = 0;
	virtual NetworkSharedChangeInfoOverflow_t* GetChangeAccessorPathInfo_1() = 0;
	virtual NetworkSharedChangeInfoOverflow_t* GetChangeAccessorPathInfo_2() = 0;
	
	virtual void unk501() = 0; // No child overrides found
	virtual bool unk502() = 0; // No child overrides found; base returns false

	virtual void ReloadPrivateScripts() = 0;
	virtual datamap_t* GetDataDescMap() = 0;

	virtual int unk601() = 0; // Default returns 0; CTestPulseIO overrides

	virtual SchemaMetaInfoHandle_t<CSchemaClassInfo> Schema_DynamicBinding() = 0;

public:
	inline CEntityHandle GetRefEHandle() const
	{
		return m_pEntity->GetRefEHandle();
	}
	
	inline const char *GetName() const
	{
		return m_pEntity->GetName();
	}

	inline const char *GetClassname() const
	{
		return m_pEntity->GetClassname();
	}

	inline CEntityIndex GetEntityIndex() const
	{
		return m_pEntity->GetEntityIndex();
	}

	HSCRIPT GetScriptInstance();

	// Refers to an instance's field.
	template< typename T >
	inline T &Field( uint nOffset )
	{
		return *reinterpret_cast<T *>( reinterpret_cast<uintp>( this ) + nOffset );
	}

public:
	CUtlSymbolLarge m_iszPrivateVScripts;
	CEntityIdentity* m_pEntity;
	CEntityPrivateScriptScope m_hPrivateScope; 
	CEntityKeyValues* m_pKeyValues;
#ifndef CS2_BETA
	HSCRIPT m_hScriptInstance;
#endif
	CScriptComponent* m_CScriptComponent;
};

// -------------------------------------------------------------------------------------------------- //
// CEntityInstance dependant functions
// -------------------------------------------------------------------------------------------------- //

inline bool CEntityHandle::operator <( const CEntityInstance *pEntity ) const
{
	uint32 otherIndex = (pEntity) ? pEntity->GetRefEHandle().m_Index : INVALID_EHANDLE_INDEX;
	return m_Index < otherIndex;
}

inline const CEntityHandle &CEntityHandle::Set( const CEntityInstance *pEntity )
{
	if(pEntity)
	{
		*this = pEntity->GetRefEHandle();
	}
	else
	{
		m_Index = INVALID_EHANDLE_INDEX;
	}

	return *this;
}

#endif // ENTITYINSTANCE_H
