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

// See entitynetwork.h
struct NetworkStateChanged_t;
struct NetworkStateChanged3_t;

extern IScriptVM* ScriptVM();

struct CEntityPrivateScriptScope
{
	HSCRIPT m_hScope;
};

class CEntityInstance
{
public:
#ifdef CS2_BETA
	virtual const CNetworkSerializerClassInfo* GetSerializerClassInfo() = 0;
#endif

	virtual void unk001() = 0;
	virtual void unk002() = 0;

	virtual ScriptClassDesc_t* GetScriptDesc() = 0;
	
	virtual ~CEntityInstance() = 0;
	
	virtual void Connect() = 0;
	virtual void Disconnect() = 0;
	virtual void Precache( const CEntityPrecacheContext* pContext ) = 0;
	virtual void AddedToEntityDatabase() = 0;
	virtual void Spawn( const CEntityKeyValues* pKeyValues ) = 0;

	virtual void unk101() = 0;

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
	
	virtual void unk201() = 0;

	virtual int ObjectCaps() = 0;
	virtual CEntityIndex RequiredEdictIndex() = 0;

	// marks a field for transmission over the network
	virtual void NetworkStateChanged( const NetworkStateChanged_t& data ) = 0; // Function replaces old version NetworkStateChanged( uint nOffset, int, ChangeAccessorFieldPathIndex_t PathIndex )

	virtual void NetworkStateChangedBranch( const void* data ) = 0; // Game never call this function during testing
	virtual void NetworkStateChanged_3( const NetworkStateChanged3_t& data ) = 0;

	// Toggles network update state, if set to false would skip network updates
	virtual void NetworkUpdateState( bool bUnk ) = 0; // Affects behavior of NetworkStateChanged
	virtual void NetworkStateChangedLog( const char* pszFieldName, const char* pszInfo ) = 0;
	virtual bool FullEdictChanged() = 0;

	virtual void unk401() = 0;
	virtual void unk402() = 0;

	virtual ChangeAccessorFieldPathIndex_t AddChangeAccessorPath( const CFieldPath& path ) = 0;
	virtual void AssignChangeAccessorPathIds() = 0;
	virtual ChangeAccessorFieldPathIndexInfo_t* GetChangeAccessorPathInfo_1() = 0;
	virtual ChangeAccessorFieldPathIndexInfo_t* GetChangeAccessorPathInfo_2() = 0;
	
	virtual void unk501() = 0;
	virtual void unk502() = 0;

	virtual void ReloadPrivateScripts() = 0;
	virtual datamap_t* GetDataDescMap() = 0;

	virtual void unk601() = 0;

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
