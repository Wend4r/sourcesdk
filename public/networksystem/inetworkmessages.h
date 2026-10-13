#ifndef NETWORKMESSAGES_H
#define NETWORKMESSAGES_H

#ifdef _WIN32
#pragma once
#endif

#include <eiface.h>

#include "inetworkserializer.h"
#include <tier0/bitbuf.h>
#include <tier0/utlstring.h>
#include <tier0/utlsymbol.h>
#include <tier0/utlsymbollarge.h>
#include <tier0/utlhashtable.h>
#include <tier0/utlmap.h>
#include <tier0/utldict.h>
#include <tier0/utlsortvector.h>
#include <tier0/threadtools.h>
#include <tier1/utldelegate.h>
#include <tier0/logging.h>
#include <tier0/convar.h>
#include "color.h"

class CSchemaClassBindingBase;
class INetworkSerializerBindingBuildFilter;
struct NetworkFieldResult_t;
struct NetworkSerializerInfo_t;
struct NetworkUnserializerInfo_t;
struct NetworkableData_t;
struct NetworkFieldInfo_t;
struct FieldMetaInfo_t;
struct NetworkContextData_t;
class CSchemaType;

enum NetworkFieldChangeCallbackPerformType_t
{
	NETWORK_FIELD_CHANGE_CALLBACK_PERFORM_ON_CHANGE,
	NETWORK_FIELD_CHANGE_CALLBACK_PERFORM_ON_CHANGE_OR_CREATE
};

enum NetworkFieldChangedDelegateType_t
{
	NETWORK_FIELD_CHANGED_TYPE_NONE = 0,
	NETWORK_FIELD_CHANGED_NO_VALUE,
	NETWORK_FIELD_CHANGED_WITH_VALUE,
	NETWORK_FIELD_CHANGED_NO_VALUE_NO_CONTEXT,
	NETWORK_FIELD_CHANGED_ARRAY_WITH_VALUE,
	NETWORK_FIELD_CHANGED_ARRAY_NO_VALUE,
	NETWORK_FIELD_CHANGED_REPLAY_COMPAT,
	NETWORK_FIELD_CHANGED_TAG,
	NETWORK_FIELD_CHANGED_COUNT_TYPE
};

typedef NetworkFieldResult_t ( *NetworkFieldSerializeCB )(NetworkSerializerInfo_t const&, int, NetworkableData_t *);
typedef NetworkFieldResult_t ( *NetworkFieldUnserializeCB )(NetworkUnserializerInfo_t const&, int, NetworkableData_t *);

typedef NetworkFieldResult_t ( *NetworkFieldSerializeBufferCB )(NetworkSerializerInfo_t const&, bf_write &, int *, void const **, bool);
typedef NetworkFieldResult_t ( *NetworkFieldUnserializeBufferCB )(NetworkUnserializerInfo_t const&, bf_read &, int *, void **, bool, CSchemaClassBindingBase const*);

typedef uint ( *NetworkFieldInfoCB )(NetworkFieldInfo_t const&);
typedef bool ( *NetworkFieldMetaInfoCB )(FieldMetaInfo_t const&, CSchemaClassBindingBase const*, NetworkFieldInfo_t const&, void *);
typedef bool ( *NetworkableDataCB )(NetworkableData_t const*, CUtlString &);
typedef const char* ( *NetworkFieldSerializerNameCB )(void);

enum NetworkableDataType_t
{
	NET_DATA_TYPE_INVALID = 0,
	NET_DATA_TYPE_INT64,
	NET_DATA_TYPE_UINT64,
	NET_DATA_TYPE_FLOAT32,
	NET_DATA_TYPE_FLOAT64,
	NET_DATA_TYPE_STRING,
	NET_DATA_TYPE_BINARY_BLOCK,
	NET_DATA_TYPE_BOOL,
	NET_DATA_TYPE_COUNT
};

// Registered by RegisterNetworkFieldSerializer, one per serializer name
struct NetworkFieldSerializerGroup_t
{
	// Set per serialization mode; the last two default to the first two when the registration passes NULL
	struct Callbacks_t
	{
		NetworkFieldSerializeCB m_pfnSerialize;
		NetworkFieldUnserializeCB m_pfnUnserialize;
		NetworkFieldSerializeCB m_pfnUnk010;
		NetworkFieldUnserializeCB m_pfnUnk018;
	};

	// Must match for every serialization mode of the serializer
	NetworkableDataType_t m_nDataType;
	int16 m_nComponentCount;

	// Bit per registered NetworkSerializationMode_t
	uint16 m_nRegisteredModes;

	NetworkableDataCB m_pfnNetworkableData;
	NetworkFieldSerializerNameCB m_pfnSerializerName;
	NetworkFieldInfoCB m_pfnFieldInfo[ NET_SERIALIZATION_MODE_COUNT ];
	NetworkFieldMetaInfoCB m_pfnFieldMetaInfo[ NET_SERIALIZATION_MODE_COUNT ];
	Callbacks_t m_Callbacks[ NET_SERIALIZATION_MODE_COUNT ];
};

// Registered by RegisterNetworkArrayFieldSerializer, one per serializer name
struct NetworkArrayFieldSerializerGroup_t
{
	// Set per serialization mode; the last two default to the first two when the registration passes NULL
	struct Callbacks_t
	{
		NetworkFieldSerializeBufferCB m_pfnSerialize;
		NetworkFieldUnserializeBufferCB m_pfnUnserialize;
		NetworkFieldSerializeBufferCB m_pfnUnk010;
		NetworkFieldUnserializeBufferCB m_pfnUnk018;
	};

	// Bit per registered NetworkSerializationMode_t
	uint32 m_nRegisteredModes;

	NetworkFieldInfoCB m_pfnFieldInfo[ NET_SERIALIZATION_MODE_COUNT ];
	NetworkFieldMetaInfoCB m_pfnFieldMetaInfo[ NET_SERIALIZATION_MODE_COUNT ];
	Callbacks_t m_Callbacks[ NET_SERIALIZATION_MODE_COUNT ];
};

#define NETWORKMESSAGES_INTERFACE_VERSION "NetworkMessagesVersion001"

abstract_class INetworkMessages
{
public:
	virtual void RegisterNetworkCategory( NetworkCategoryId nCategoryId, const char *szDebugName ) = 0;

	virtual void AssociateNetMessageWithChannelCategoryAbstract( INetworkSerializerPB *pNetMessage, NetworkCategoryId nCategoryId, bool bCategoryMaskOverride = false ) = 0;

	// Passing nMessasgeId as -1 would auto-assign the id based on the message name hash.
	// AMNOTE: bCannotBeNetworked marks the message as never sent over the network, Serialize rejects it,
	// and its id is auto-assigned too
	virtual INetworkSerializerPB *FindOrCreateNetMessage( NetworkMessageId nMessageId, const IProtobufBinding *pProtoBinding, uint nMessageSize, INetworkSerializerPB *pGloablNetMessageInternal = nullptr, bool bCreateIfNotFound = true, bool bCannotBeNetworked = false ) = 0;

	virtual bool SerializeAbstract( bf_write &pBuf, const CNetMessage *pData ) = 0;

	virtual bool UnserializeMessageInternal( bf_read &pBuf, CNetMessage *pData ) = 0;
	virtual bool SerializeMessageInternal( bf_write &pBuf, const CNetMessage *pData ) = 0;

	// Returns nullptr if failed to unserialize, reason is written to err_reason
	virtual CNetMessage *UnserializeFromStream( bf_read &pBuf, CUtlString &strError ) = 0;
	virtual bool SerializeAbstractInternal( bf_write &pBuf, INetworkSerializerPB *pNetMessage, const CNetMessage *pData ) = 0;

	virtual CNetMessage *AllocateAndCopyConstructNetMessageAbstract( INetworkSerializerPB *pNetMessage, const CNetMessage *pFrom ) = 0;

	virtual void DeallocateNetMessageAbstract( INetworkSerializerPB *pNetMessage, CNetMessage *pData ) = 0;

	virtual void *RegisterNetworkFieldSerializer( const char *, NetworkSerializationMode_t, NetworkableDataType_t, int, NetworkFieldSerializeCB, NetworkFieldUnserializeCB, NetworkFieldInfoCB, 
		NetworkFieldMetaInfoCB, NetworkableDataCB, NetworkFieldSerializerNameCB, NetworkFieldSerializeCB, NetworkFieldUnserializeCB ) = 0;
	virtual void *RegisterNetworkArrayFieldSerializer( const char *, NetworkSerializationMode_t, NetworkFieldSerializeBufferCB, NetworkFieldUnserializeBufferCB, NetworkFieldInfoCB,
		NetworkFieldMetaInfoCB, NetworkFieldSerializeBufferCB, NetworkFieldUnserializeBufferCB ) = 0;

	virtual NetMessageInfo_t *GetNetMessageInfo( INetworkSerializerPB *pNetMessage ) = 0;

	virtual INetworkSerializerPB *FindNetworkMessage( const char *szName ) = 0; // V_stricmp_fast. Example: "CMsgSource1LegacyGameEvent [207]"
	virtual INetworkSerializerPB *FindNetworkMessagePartial( const char *szPartialName ) = 0; // v_stristr_fast

	virtual NetworkGroupId FindNetworkGroup( const char *szGroup, bool bCreateIfNotFound = false ) = 0;
	virtual int GetNetworkGroupCount() = 0;
	virtual const char *GetNetworkGroupName( NetworkGroupId nGroupId ) = 0;
	virtual Color GetNetworkGroupColor( NetworkGroupId nGroupId ) = 0;

	virtual void AssociateNetMessageGroupIdWithChannelCategory( NetworkCategoryId nCategoryId, const char *szGroup ) = 0;

	virtual void SetNetworkSerializationContextData( const char *szContext, NetworkSerializationMode_t eSerializationMode, NetworkContextData_t *pData ) = 0;

	virtual NetworkContextDataId FindNetworkSerializationContextDataId( const char *szContext ) = 0;
	virtual struct NetworkContextData_t *GetNetworkSerializationContextData( NetworkContextDataId nContextId, NetworkSerializationMode_t eSerializationMode = NET_SERIALIZATION_MODE_DEFAULT ) = 0;

	virtual void SetNetworkChangeTagCallback( const CUtlAbstractDelegate &callback ) = 0;

	// Doesn't support duplicated callbacks per field
	virtual void RegisterNetworkFieldChangeCallbackInternal( const char *szFieldName, uint64, NetworkFieldChangedDelegateType_t fieldType, CUtlAbstractDelegate pCallback, NetworkFieldChangeCallbackPerformType_t cbPerformType, int nPriority ) = 0;

	virtual void AllowAdditionalMessageRegistration(bool bAllow) = 0;
	virtual bool IsAdditionalMessageRegistrationAllowed() = 0;

	virtual int GetFieldChangeCallbackOrderCount() = 0;
	virtual void GetFieldChangeCallbackPriorities( int nCount, int *nPriorities ) = 0;

	virtual void RegisterFieldChangeCallbackPriority( int nPriority ) = 0;

	virtual INetworkSerializerPB *FindNetworkMessageById( NetworkMessageId nMessageId ) = 0;

	virtual void SetIsForServer( bool bIsForServer ) = 0;
	virtual bool GetIsForServer() = 0;

	// Example: "CEntityHandle" -> "ehandle".
	virtual void RegisterSchemaTypeOverride( const char *szTypeName, const char *szNetworkTypeName ) = 0;

	virtual int ComputeOrderForPriority( int nPriority ) = 0;

	virtual LoggingChannelID_t GetLoggingChannel() = 0;

	virtual void SetCoordEncodingParams( const void *pParams ) = 0;
	virtual const void *GetCoordEncodingParams() = 0;

	virtual ~INetworkMessages() = 0;
};

class CNetworkMessages : public INetworkMessages
{
public:
	CConCommandMemberAccessor< CNetworkMessages > m_NetValidateMessagesCommand; // "net_validatemessages"
	CConCommandMemberAccessor< CNetworkMessages > m_NetListAllMessagesCommand; // "net_listallmessages"
	CConCommandMemberAccessor< CNetworkMessages > m_NetMessageInfoCommand; // "net_messageinfo"

	struct NetCategoryInfo_t
	{
		CUtlString m_DebugName; // Set by RegisterNetworkCategory
		int m_nAssociationCount; // Incremented by AssociateNetMessageWithChannelCategoryAbstract and AssociateNetMessageGroupIdWithChannelCategory
	};

	// Registered by RegisterNetworkFieldChangeCallbackInternal
	struct ChangeCallbackInfo_t
	{
		CUtlAbstractDelegate m_Callback;
		uint64 m_nUnk1; // The unnamed uint64 argument
		uint8 m_FieldType; // NetworkFieldChangedDelegateType_t
		NetworkFieldChangeCallbackPerformType_t m_PerformType;
		int m_nPriority;
		bool m_bUnk1;
	};

	// Set by SetNetworkSerializationContextData, one pointer per serialization mode
	struct NetworkContextData_t
	{
		::NetworkContextData_t *m_pData[NET_SERIALIZATION_MODE_COUNT];
	};

	class CPriorityLess
	{
	public:
		bool Less( const int &lhs, const int &rhs, void *pCtx );
	};

	CUtlVector< NetCategoryInfo_t > m_NetCategories; // Indexed by NetworkCategoryId, 32 entries

	// Messages that can be networked, registered by FindOrCreateNetMessage
	CUtlMap< NetworkMessageId, INetworkSerializerPB *, unsigned short > m_NetworkableMessages;
	CUtlMap< int, int, unsigned short > m_NetworkableMessageNameIndices; // Keyed by the name symbol
	CUtlSymbolTable m_NetworkableMessageNames;
	CThreadSpinRWLock m_NetworkableMessageNamesLock;
	CAtomicMutex m_NetworkableMessagesMutex;

	// Messages registered with bCannotBeNetworked, ids are auto-assigned
	CUtlMap< NetworkMessageId, INetworkSerializerPB *, unsigned short > m_NonNetworkableMessages;
	CUtlMap< int, int, unsigned short > m_NonNetworkableMessageNameIndices; // Keyed by the name symbol
	CUtlSymbolTable m_NonNetworkableMessageNames;
	CThreadSpinRWLock m_NonNetworkableMessageNamesLock;
	CAtomicMutex m_NonNetworkableMessagesMutex;

	CUtlDict< void *, unsigned int > m_FakeFieldInfos; // Values are 96-byte records numbered by m_nFakeFieldInfoCount, created for fake serializer fields
	CUtlSymbolTable m_NetworkGroupNames; // NetworkGroupId is the symbol, at most 256 groups
	CThreadSpinRWLock m_NetworkGroupNamesLock;

	CUtlDict< CUtlString, unsigned int > m_SchemaTypeOverrides; // RegisterSchemaTypeOverride

	CUtlVector< NetworkFieldSerializerGroup_t > m_NetworkFieldSerializers; // Indexed by the name symbol in m_NetworkFieldSerializerNames
	CUtlSymbolTable m_NetworkFieldSerializerNames;
	CThreadSpinRWLock m_NetworkFieldSerializerNamesLock;

	CUtlVector< NetworkArrayFieldSerializerGroup_t > m_NetworkArrayFieldSerializers; // Indexed by the name symbol in m_NetworkArrayFieldSerializerNames
	CUtlSymbolTable m_NetworkArrayFieldSerializerNames;
	CThreadSpinRWLock m_NetworkArrayFieldSerializerNamesLock;

	CUtlHashtable< CUtlSymbolLarge, ChangeCallbackInfo_t, PointerHashFunctor, PointerEqualFunctor > m_FieldChangeCallbacks; // Keyed by m_FieldChangeCallbackNames symbols
	CUtlSymbolTableLarge m_FieldChangeCallbackNames;

	CUtlVector< NetworkContextData_t > m_NetworkSerializationContexts; // Indexed by NetworkContextDataId
	CUtlSymbolTable m_NetworkSerializationContextNames;
	CThreadSpinRWLock m_NetworkSerializationContextNamesLock;

	CUtlSortVector< int, CPriorityLess > m_FieldChangeCallbackPriorities; // RegisterFieldChangeCallbackPriority
	int m_nFakeFieldInfoCount;
	bool m_bAllowAdditionalMessageRegistration;
	bool m_bCanStillRegisterCallbackPriorities;
	bool m_bIsForServer;
	CAtomicMutex m_MessageRegistrationMutex; // Held by FindOrCreateNetMessage
	CAtomicMutex m_FakeFieldInfosMutex; // Held while inserting into m_FakeFieldInfos
	CAtomicMutex m_FieldChangeCallbackPrioritiesMutex;
	CUtlSymbolTableLargeMT m_UnkSymbolTableMT; // Purged by two network system helpers, no name found
	CUtlAbstractDelegate m_NetworkChangeTagCallback; // SetNetworkChangeTagCallback
	byte m_CoordEncodingParams[96]; // SetCoordEncodingParams copies it, GetCoordEncodingParams returns its address
};

DECLARE_TIER2_INTERFACE(INetworkMessages, g_pNetworkMessages);

// ------------------------------------------------------------------------ //
// Datatable send proxies are used to tell the engine where the datatable's 
// data is and to specify which clients should get the data. 
//
// pRecipients is the object that allows you to specify which clients will
// receive the data.
// ------------------------------------------------------------------------ //
class CSendProxyRecipients
{
public:
	void	SetAllRecipients();					// Note: recipients are all set by default when each proxy is called.
	void	ClearAllRecipients();

	void	SetRecipient( int iClient );		// Note: these are CLIENT indices, not entity indices (so the first player's index is 0).
	void	ClearRecipient( int iClient );

	// Clear all recipients and set only the specified one.
	void	SetOnly( int iClient );
	// Set all recipients, save for the specified on
	void    ExcludeOnly( int iClient );

public:
	// Make sure we have enough room for the max possible player count
	CPlayerBitVec	m_Bits;
};

#endif /* NETWORKMESSAGES_H */