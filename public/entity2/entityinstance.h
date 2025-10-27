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

extern IScriptVM* ScriptVM();

struct NetworkStateChanged_t
{
	NetworkStateChanged_t() : m_Unk00(1), m_Unk48(-1), m_nArrayIndex(-1), m_nPathIndex(ChangeAccessorFieldPathIndex_t()), m_Unk60(0) { }
	explicit NetworkStateChanged_t( bool bFullChanged ) : m_Unk00(static_cast<uint32>(!bFullChanged)), m_Unk48(-1), m_nArrayIndex(-1), m_nPathIndex(ChangeAccessorFieldPathIndex_t()), m_Unk60(0) { }

	// nLocalOffset is the flattened field offset
	//		calculated taking into account embedded structures
	//		if PathIndex is specified, then the offset must start from the last object in the chain
	// nArrayIndex is the index of the array element 
	//		if the field is a CNetworkUtlVectorBase, otherwise pass -1
	// nPathIndex is the value to specify 
	//		if the path to the field goes through one or more pointers, otherwise pass -1
	// 		this value is usually a member of the CNetworkVarChainer and belongs to the last object in the chain
	NetworkStateChanged_t( uint32 nLocalOffset, int32 nArrayIndex = -1, ChangeAccessorFieldPathIndex_t nPathIndex = ChangeAccessorFieldPathIndex_t() ) : m_Unk00(1), m_LocalOffsets{ nLocalOffset }, m_Unk48(-1), m_nArrayIndex(nArrayIndex), m_nPathIndex(nPathIndex), m_Unk60(0) { }
	NetworkStateChanged_t( CUtlVector<uint32> vecLocalOffsets, int32 nArrayIndex = -1, ChangeAccessorFieldPathIndex_t nPathIndex = ChangeAccessorFieldPathIndex_t() ) : m_Unk00(1), m_LocalOffsets(Move(vecLocalOffsets)), m_Unk48(-1), m_nArrayIndex(nArrayIndex), m_nPathIndex(nPathIndex), m_Unk60(1) { }

	uint32 m_Unk00; // Perhaps it is an enum, default 1, when 0 adds FL_FULL_EDICT_CHANGED
	CUtlVector<uint32> m_LocalOffsets;
	// Probably only works in the debug build, as it has always been empty
	CUtlString m_ClassName;
	CUtlString m_FieldName;
	int32 m_Unk48; // default -1
	int32 m_nArrayIndex; // default -1
	ChangeAccessorFieldPathIndex_t m_nPathIndex; // default -1 (can also be -2)
	int16 m_Unk60; // default 0, if m_LocalOffsets has multiple values, it is set to 1
};
COMPILE_TIME_ASSERT(sizeof(NetworkStateChanged_t) == 64);

// Not entirely sure
struct NetworkStateChanged3Data
{
	CUtlVector<uint32> m_Unk0;
	CUtlVector<uint32> m_Unk24;
};


class CEntityInstance
{
public:
	virtual ScriptClassDesc_t* GetScriptDesc() = 0;
	
	virtual ~CEntityInstance() = 0;
	
	virtual void Connect() = 0;
	virtual void Disconnect() = 0;
	virtual void Precache( const CEntityPrecacheContext* pContext ) = 0;
	virtual void AddedToEntityDatabase() = 0;
	virtual void Spawn( const CEntityKeyValues* pKeyValues ) = 0;

	virtual void unk001() = 0;

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
	
	virtual void unk101() = 0;

	virtual int ObjectCaps() = 0;
	virtual CEntityIndex RequiredEdictIndex() = 0;

	// marks a field for transmission over the network
	virtual void NetworkStateChanged( const NetworkStateChanged_t& data ) = 0; // Function replaces old version NetworkStateChanged( uint nOffset, int, ChangeAccessorFieldPathIndex_t PathIndex )
	virtual void NetworkStateChangedBranch( const void* data ) = 0; // Game never call this function during testing
	virtual void NetworkStateChanged_3( const NetworkStateChanged3Data& data ) = 0;
	virtual void NetworkStateUnkSetBool( bool bUnk ) = 0; // Affects behavior of NetworkStateChanged
	virtual void NetworkStateChangedLog( const char* pszFieldName, const char* pszInfo ) = 0;
	virtual bool FullEdictChanged() = 0;

	virtual void unk201() = 0;
	virtual void unk202() = 0;

	virtual ChangeAccessorFieldPathIndex_t AddChangeAccessorPath( const CFieldPath& path ) = 0;
	virtual void AssignChangeAccessorPathIds() = 0;
	virtual ChangeAccessorFieldPathIndexInfo_t* GetChangeAccessorPathInfo_1() = 0;
	virtual ChangeAccessorFieldPathIndexInfo_t* GetChangeAccessorPathInfo_2() = 0;
	
	virtual void unk301() = 0;
	virtual void unk302() = 0;

	virtual void ReloadPrivateScripts() = 0;
	virtual datamap_t* GetDataDescMap() = 0;

	virtual void unk401() = 0;

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
	CUtlSymbolLarge m_iszPrivateVScripts; // 0x8
	CEntityIdentity* m_pEntity; // 0x10
private:
	void* m_hPrivateScope; // 0x18 - CEntityPrivateScriptScope
public:
	CEntityKeyValues* m_pKeyValues; // 0x20
	HSCRIPT m_hScriptInstance; // 0x28
	CScriptComponent* m_CScriptComponent; // 0x30
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
