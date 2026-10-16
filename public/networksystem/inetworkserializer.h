#ifndef NETWORKSERIALIZER_H
#define NETWORKSERIALIZER_H

#ifdef _WIN32
#pragma once
#endif

#include <platform.h>
#include <tier0/utlstring.h>
#include <tier0/utlhash.h>

#include <networksystem/iprotobufbinding.h>
#include <tier0/utlstring.h>
#include <tier0/bitbuf.h>
#include <tier0/utldict.h>
#include "color.h"

#include <memory>

#define CODEGEN_HASH_TOKEN (0x3501A674)

class CEntityInstance;
class CCheckTransmitInfo;
class CNetworkSerializerCodeGenDatabase;
class CNetMessage;

enum NetworkValidationMode_t
{

};

enum NetworkSerializationMode_t : int
{
	NET_SERIALIZATION_MODE_0 = 0x0,
	NET_SERIALIZATION_MODE_1 = 0x1,
	NET_SERIALIZATION_MODE_COUNT = 0x2,
	NET_SERIALIZATION_MODE_DEFAULT = 0x0,
	NET_SERIALIZATION_MODE_SERVER = 0x0,
	NET_SERIALIZATION_MODE_CLIENT = 0x1,
};

typedef int NetworkMessageId;
typedef int8 NetworkGroupId;
typedef int NetworkCategoryId;
typedef int16 NetworkContextDataId;

struct NetworkRecipientsFilter_t
{
	class CFilterOwner {};
	using FilterCb = void ( CFilterOwner::* )( CCheckTransmitInfo *pInfo, CPlayerBitVec &player_mask );

	enum FilterOwnerType : int8
	{
		Filter_Other = 1,
		Filter_Entity = 2,
		Filter_FieldOwner = 3,
	};

	CFilterOwner *m_FilterOwner;
	FilterCb m_FilterFn;
	const char *m_FilterName;
	FilterOwnerType m_FilterOwnerType;
};

struct NetworkChangePointerCallback_t
{
	class CCallbackOwner {};
	// AMNOTE: nAction is 1 when the pointer is set, 2 when it's cleared, 3 when the pointee changes in place
	using ChangeCb = void ( CCallbackOwner::* )( CEntityInstance *pEntity, uint8 nAction );

	// AMNOTE: Selects the object m_CallbackFn is called on
	enum CallbackOwnerType : int8
	{
		// AMNOTE: The pointer's new value, or the stored owner when it's null
		Callback_Pointee = 1,
		// AMNOTE: The object that contains the pointer field
		Callback_FieldOwner = 2,
		// AMNOTE: The stored owner
		Callback_Other = 3
	};

	const char *m_CallbackName;
	const char *m_ClassName;
	CCallbackOwner *m_CallbackOwner;
	ChangeCb m_CallbackFn;
	CallbackOwnerType m_CallbackOwnerType;
};

enum NetworkOverrideType_t
{
	MNetworkOverride_Invalid = -1,
	MNetworkSerializer = 0,
	MNetworkEncoder,
	MNetworkChangeCallback,
	MNetworkChangeTag,
	MNetworkBitCount,
	MNetworkUserGroup,
	MNetworkPriority,
	MNetworkOutOfPVSUpdates,
	MNetworkRemoveAll,
};

enum OOPVSUpdates_t
{
	OOPVSUpdates_OptOut = 0,
	OOPVSUpdates_OptIn,
	OOPVSUpdates_Default,
};

struct NetworkOverride_t
{
	const char *m_ParentClass;
	const char *m_FieldName;
	const char *m_Value;
	NetworkOverrideType_t m_Type;
};

struct VarTypeOverride_t
{
	const char *m_FieldName;
	const char *m_OverrideType;
};

struct SerializedFieldTypeMapping_t
{
	const char *m_FieldName;
	const char *m_FieldType;
};

struct UserGroupProxy_t
{
	const char *m_ClassName;
	const char *m_UserGroup;
	NetworkRecipientsFilter_t::CFilterOwner *m_ProxyOwner;
	NetworkRecipientsFilter_t::FilterCb m_ProxyFn;
	NetworkRecipientsFilter_t::FilterOwnerType m_ProxyOwnerType;
};

struct ReplayCompatField_t
{
	const char *m_FieldPath;
	const char *m_CallbackName;
	void *m_unk001;
	int8 m_unk002;
};

class CNetworkSerializerFieldInfo
{
public:
	CUtlStringToken m_FieldNameHash;
	const char *m_pszFieldName;
	CUtlString m_pszTypeName;
	CUtlString m_pszRawType;
	CUtlString m_pszEncodedType;
	CUtlStringToken m_ClassNameHash;
	const char *m_pszClassName;
	int32 m_nFieldSize;
	int32 m_nFieldOffset;
	CUtlStringToken m_NetworkAliasHash;
	const char *m_NetworkAlias;
	CUtlStringToken m_NetworkTypeAliasHash;
	const char *m_NetworkTypeAlias;

	void *m_unk001;

	const char *m_NetworkSerializer;
	const char *m_NetworkEncoder;

	std::shared_ptr<NetworkRecipientsFilter_t> m_NetworkSendProxyRecipientsFilter;
	std::shared_ptr<NetworkChangePointerCallback_t> m_NetworkChangePointerCallback;

	int8 m_NetworkPriority;

	SchemaCollectionManipulatorFn_t m_CollectionManipulatorFn;
	CUtlVector<CUtlString> m_NetworkIncludeByUserGroup;
	CUtlVector<CUtlString> m_NetworkChangeCb;
	CUtlVector< CUtlString > m_NetworkChangeTags;

	int m_NetworkBitCount;
	int m_NetworkEncodeFlags;
	int m_NetworkVarEmbeddedFieldOffsetDelta;
	float m_NetworkMin;
	float m_NetworkMax;

	OOPVSUpdates_t m_NetworkOutOfPVSUpdates;
	bool m_NetworkBitCountSet;
	bool m_NetworkVarEmbeddedNotFlattened;

	bool m_NetworkPolymorphic;
	CUtlString m_pszCodeGenType;
	CNetworkSerializerClassInfo *m_TypeClassInfo;

	int m_CodeGenValueTypeSize;

	CUtlString m_TypeOverride;
	CUtlString m_BuiltinUnderlyingType;

	char m_ResourceTypeForInfoType[ 8 ];
	int m_FixedArraySize;
	char m_IsAtomic;
	char m_IsBuiltIn;
	char m_IsEnum;
	char m_IsCHandle;
	char m_IsPointer;
	char m_IsUtlVector;
	char m_IsFixedArray;
	char m_IsTypeSafeInt;
	char m_IsTypeSafeFloat;
	char m_IsStrongHandle;
	char m_IsWeakHandle;
	char m_IsModifierHandle;
	char m_IsSigned;
	char m_IsNetArray;
};

struct SerializerFieldLookup_t
{
	SerializerFieldLookup_t( const char *fieldname ) : m_FieldName( fieldname ), m_FieldIndex( 0 ) {}

	CUtlString m_FieldName;
	int m_FieldIndex;
};

template<> struct DefaultEqualFunctor<SerializerFieldLookup_t> { bool operator()( SerializerFieldLookup_t a, SerializerFieldLookup_t b ) const { return a.m_FieldName == b.m_FieldName; } };
template<> struct DefaultHashFunctor<SerializerFieldLookup_t> { unsigned int operator()( SerializerFieldLookup_t a ) const { return MurmurHash2LowerCase( a.m_FieldName.String(), CODEGEN_HASH_TOKEN ); } };

class CNetworkSerializerClassInfo
{
public:
	CNetworkSerializerFieldInfo *FindField( const char *field_name ) const
	{
		auto handle = m_FieldLookupTable.Find( field_name );

		if(handle != m_FieldLookupTable.InvalidHandle())
		{
			auto elem = m_FieldLookupTable.Element( handle );
			Assert( elem.m_FieldIndex >= 0 && elem.m_FieldIndex < m_nTotalFieldEntries );
			return m_Fields[elem.m_FieldIndex];
		}

		return nullptr;
	}

public:
	struct ExcludeIncludeFilter_t
	{
		CUtlVector<CUtlString> m_ExcludeList;
		CUtlVector<CUtlString> m_IncludeList;
	};

	// nAction 0 allocates and constructs a new object, 1 destructs and frees pObject,
	// 2 returns the CNetworkSerializerClassInfo of pObject's most derived class
	typedef void *( *ManipulatorFn_t )( int nAction, void *pObject );

	CUtlStringToken m_nHash;
	const char *m_pszClassName;
	CUtlVector<CNetworkSerializerFieldInfo *> m_Fields;
	ExcludeIncludeFilter_t m_NetworkFilterByUserGroup;
	ExcludeIncludeFilter_t m_NetworkFilterByName;

	CUtlHash<SerializerFieldLookup_t> m_FieldLookupTable;
	int m_nTotalFieldEntries;

	CUtlVectorFixedGrowable< CNetworkSerializerClassInfo *, 1 > m_ParentClassInfo;
	CUtlVector<int> m_ParentClassOffset;

	// Maps member names like __m_pChainEntity and m_PathIndex to their offsets in the class
	CUtlDict< uint16 > m_MemberOffsets;

	CUtlVector<NetworkOverride_t *> m_NetworkOverrides;
	// Includes this class and parent overreides
	CUtlVector<NetworkOverride_t *> m_NetworkFlattenedOverrides;

	CUtlVector<VarTypeOverride_t *> m_NetworkVarTypeOverrides;
	CUtlVector<SerializedFieldTypeMapping_t *> m_FieldTypeMappings;
	CUtlVector<UserGroupProxy_t *> m_UserGroupProxies;
	CUtlVector<ReplayCompatField_t *> m_NetworkReplayCompatFields;
	CNetworkSerializerCodeGenDatabase *m_pDatabase;

	int32_t m_nClassSize;
	OOPVSUpdates_t m_NetworkOutOfPVSUpdates;
	// Allocation mode, only 1 and 2 allow allocating and freeing through m_pfnManipulator
	int m_nAllocationMode;

	ManipulatorFn_t m_pfnManipulator;

	bool m_Initialized;
	bool m_NetworkVarsAtomic;
	bool m_unk301;
	bool m_NetworkNoBase;
	bool m_NetworkStructNotInNetworkUtlVectorEmbedded;
	bool m_NetworkVarEmbeddedNotFlattened;
	// Only the constructor writes it
	bool m_unk401;
};

class CNetworkSerializerCodeGenDatabase
{
public:
	struct EnumInfo_t
	{
		int32_t m_nValue;
		int8_t m_nFlags;
	};

	CUtlString m_ModuleName;
	CUtlDict<CNetworkSerializerClassInfo *> m_ClassInfos;
	CUtlDict<CNetworkSerializerCodeGenDatabase::EnumInfo_t> m_EnumInfos;
	CUtlDict<CUtlString> m_AtomicTypeMapping;

	bool m_bDebugSpew;
};

struct NetMessageInfo_t
{
	int m_nCategories;
	IProtobufBinding *m_pBinding;
	CUtlString m_szGroup;
	NetworkMessageId m_MessageId;
	NetworkGroupId m_GroupId;

	// AMNOTE: From IProtobufBinding::GetBufType()
	NetChannelBufType_t m_nBufType : 6;
	// AMNOTE: Set by FindOrCreateNetMessage's bCannotBeNetworked
	bool m_bCannotBeNetworked : 1;
	uint8 m_bAssumeFieldsNotNetworked : 1;

	// AMNOTE: AddCategoryMask ORs nMask into it too when bRequireHandler is true. ConfirmAllMessageHandlersInstalled
	// reports channels in these categories that have no handler registered for the message
	int m_nRequiredHandlerCategories;
	bool m_bOkayToRedispatch;
};

abstract_class INetworkMessageInternal
{
public:
	virtual ~INetworkMessageInternal() = 0;

	virtual const char *GetUnscopedName() = 0;
	virtual NetMessageInfo_t *GetNetMessageInfo() = 0;

	virtual void SetMessageId( NetworkMessageId nMessageId ) = 0;

	virtual void AddCategoryMask( uint32 nMask, bool bRequireHandler = false ) = 0;

	virtual void SwitchMode( NetworkValidationMode_t nMode ) = 0;

	virtual CNetMessage *AllocateMessage() = 0;

	// Calls to INetworkMessages::SerializeMessageInternal
	virtual bool Serialize( bf_write &pBuf, const CNetMessage *pData ) = 0;
	// Calls to INetworkMessages::UnserializeMessageInternal
	virtual bool Unserialize( bf_read &pBuf, CNetMessage *pData ) = 0;
};

using INetworkSerializerPB = INetworkMessageInternal;

#endif /* NETWORKSERIALIZER_H */
