#ifndef NETMESSAGE_H
#define NETMESSAGE_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "iserver.h"
#include "serversideclient.h"
#include "networksystem/inetworkmessages.h"
#include "networksystem/inetworkserializer.h"

#include <typeinfo>

#define DEFAULT_NETMESSAGE_COLOR Color(255, 255, 255, 255)

inline constexpr const char *k_pszNetGroupNames[ SG_TOTAL ] = 
{
	"Generic",
	"Local Player",
	"Other PLayer",
	"Entities",
	"Sounds",
	"Events",
	"Voice",
	"String Table",
	"Move",
	"String Command",
	"Signon",
	"System",
	"User Messages",
	"Spawn Groups",
	"Game Engine",
	"Hltv Replay"
};

class CNetMessage
{
public:
	CNetMessage( NetChannelBufType_t bufType = BUF_DEFAULT ) : m_dbRecivedTime( -1.0 ), m_nSignatrue( 0 ), m_bufType( bufType ), m_nPacketSize( -1 ), m_nSendCount( -1 ), m_flMargin( -1.0f ), m_nUnk( -1 ) {}
	CNetMessage( const CNetMessage &copyFrom ) = default;
	CNetMessage( CNetMessage &&moveFrom ) = default;
	CNetMessage &operator=( const CNetMessage &copyFrom ) = default;
	CNetMessage &operator=( CNetMessage &&moveFrom ) = default;
	virtual ~CNetMessage() = default;

	// Returns the underlying proto object
	virtual const google::protobuf::MessageLite *AsMessageLite() const = 0;
	virtual const google::protobuf::Message     *AsMessage() const = 0;

	virtual INetworkSerializerPB *GetSerializerPB() const = 0;
	virtual CNetMessage *Clone() const = 0;

	// Helper function to cast up the abstract message to a concrete T message type.
	// Doesn't do any validity checks itself!
	template< typename T > T *As() { return static_cast< T * >( this ); }
	template< typename T > const T *As() const { return static_cast< const T * >( this ); }

	double GetRecivedTime() const { return m_dbRecivedTime; }
	uint32 GetSignature() const { return m_nSignatrue; }
	NetChannelBufType_t GetBufType() const { return m_bufType; }
	int GetPacketSize() const { return m_nPacketSize; }
	int GetSendCount() const { return m_nSendCount; }
	float GetMargin() const { return m_flMargin; }

	bool Send( CPlayerSlot slot ) const { return Send( CUtlVector< CPlayerSlot >{ slot } ) != 0; }

	int Send( const CPlayerBitVec &playerBits ) const
	{
		if ( !g_pNetworkServerService->IsServerRunning() )
		{
			return 0;
		}

		CNetworkGameServer *pNetServer = g_pNetworkServerService->GetNetworkServer();

		if ( !pNetServer )
		{
			return 0;
		}

		int nSent = 0;

		int index = playerBits.FindNextSetBit( 0 );

		while ( index > -1 )
		{
			CServerSideClientBase *pClient = pNetServer->GetClientBySlot( index );

			index = playerBits.FindNextSetBit( index + 1 );

			if ( !pClient )
			{
				continue;
			}

			if ( pClient->SendNetMessage( this, GetBufType() ) )
			{
				nSent++;
			}
		}

		return nSent;
	}

	int Send( const CUtlVector< CPlayerSlot > &vecSlots ) const
	{
		if ( !g_pNetworkServerService->IsServerRunning() )
		{
			return 0;
		}

		CNetworkGameServer *pNetServer = g_pNetworkServerService->GetNetworkServer();

		if ( !pNetServer )
		{
			return 0;
		}

		int nSent = 0;

		for ( auto slot : vecSlots )
		{
			CServerSideClientBase *pClient = pNetServer->GetClientBySlot( slot );

			if ( !pClient )
			{
				continue;
			}

			if ( pClient->SendNetMessage( this, GetBufType() ) )
			{
				nSent++;
			}
		}

		return nSent;
	}

	int SendToAllClients() const
	{
		if ( !g_pNetworkServerService->IsServerRunning() )
		{
			return 0;
		}

		CNetworkGameServer *pNetServer = g_pNetworkServerService->GetNetworkServer();

		if ( !pNetServer )
		{
			return 0;
		}

		int nSent = 0;

		for ( const auto &pClient : pNetServer->GetClients() )
		{
			if ( pClient->SendNetMessage( this, GetBufType() ) )
			{
				nSent++;
			}
		}

		return nSent;
	}

private:
	double m_dbRecivedTime;
	uint32 m_nSignatrue;
	NetChannelBufType_t m_bufType;
	int m_nPacketSize;
	int m_nSendCount;
	float m_flMargin;
	int64 m_nUnk;
};

// AMNOTE: This is a stub class over real CNetMessagePB!
// This is mainly to access the game constructed objects, and not for direct initialization of them
// since this misses the CNetMessage implementation which requires supplying other proto related info like
// proto binding object, proto msg id/group, etc.
// So to allocate the message yourself use INetworkMessageInternal::AllocateMessage() or INetworkMessages::AllocateNetMessageAbstract()
// functions instead of direct initialization (they both are equivalent)!
// Example usage:
// auto *msg = INetworkMessageInternal::AllocateMessage()->As<CYourMessage_t>();
// msg->field1( 2 );
// msg->field2( 3 );
// IGameEventSystem::PostEventAbstract( ..., msg, ... );
template< NetworkMessageId ID, typename PROTO_TYPE, SignonGroup_t SIGNON = SG_GENERIC, NetChannelBufType_t BUF_TYPE = BUF_RELIABLE, bool IS_FOR_SERVER = false >
class CNetMessagePB : public CNetMessage, public PROTO_TYPE
{
public:
	using MyType_t = CNetMessagePB< ID, PROTO_TYPE, SIGNON, BUF_TYPE, IS_FOR_SERVER >;
	using PBType_t = PROTO_TYPE;

	static constexpr NetworkMessageId kMsgId = ID;
	static constexpr SignonGroup_t kSignonGroup = SIGNON;
	static constexpr NetChannelBufType_t kBufType = BUF_TYPE;
	static constexpr bool kIsForServer = IS_FOR_SERVER;

	inline static class CProtobufBinding : public IProtobufBinding
	{
	public:
		virtual const char *GetName() const
		{
			static std::string s_szResult;

			if ( s_szResult.empty() )
			{
				s_szResult += PBType_t().GetTypeName();
				s_szResult += " [";
				s_szResult += std::to_string( kMsgId );
				s_szResult += "]";
			}

			return s_szResult.c_str();
		}

		virtual int GetSize() const { return sizeof( PBType_t ); }

		virtual const char *ToString( CNetMessage *pData, CUtlString &sResult ) const
		{
			auto *pMsgPB = reinterpret_cast< MyType_t * >(pData);

			CBufferStringN<256> sBuffer;

			sBuffer.Format( "%s\n{\n", GetName() );

			auto sPBDebug = pMsgPB->PBType_t::DebugString();

			sBuffer.Append(sPBDebug.c_str(), sPBDebug.length());
			sBuffer += "}\n";

			sResult = sBuffer;

			return sResult.String();
		}

		virtual const char *GetGroup() const
		{
			if constexpr ( !( 0 <= kSignonGroup && kSignonGroup < ARRAYSIZE(k_pszNetGroupNames) ) )
				return "Unknown";

			return k_pszNetGroupNames[ kSignonGroup ];
		}

		virtual Color GetGroupColor() const
		{
			if ( !g_pNetworkMessages )
				return DEFAULT_NETMESSAGE_COLOR;

			static NetworkGroupId s_nPBGroup = -1;

			if ( s_nPBGroup == -1 )
			{
				s_nPBGroup = g_pNetworkMessages->FindNetworkGroup( GetGroup() );

				if ( s_nPBGroup == -1 )
				{
					return DEFAULT_NETMESSAGE_COLOR;
				}

				return g_pNetworkMessages->GetNetworkGroupColor( s_nPBGroup );
			}

			return DEFAULT_NETMESSAGE_COLOR;
		}

		virtual NetChannelBufType_t GetBufType() const { return kBufType; }
		virtual CNetMessage *AllocateMessage() const
		{
			auto *pNewMsg = Alloc< MyType_t >();

			if ( !pNewMsg )
				return nullptr;

			return static_cast< CNetMessage * >( Construct( pNewMsg ) );
		}

		virtual bool OkToRedispatch() const { return kIsForServer; }
		virtual bool IsParent() const { return false; }
	} sm_binding;

public:
	CNetMessagePB() : CNetMessage( kBufType ), PBType_t() {}
	CNetMessagePB( const MyType_t &copyFrom ) = default;
	CNetMessagePB( MyType_t &&moveFrom ) = default;
	MyType_t &operator=( const MyType_t &copyFrom ) = default;
	MyType_t &operator=( MyType_t &&moveFrom ) = default;
	virtual ~CNetMessagePB() {}

	virtual const google::protobuf::MessageLite *AsMessageLite() const { return static_cast< const PBType_t * >( this ); };
	virtual const google::protobuf::Message     *AsMessage() const { return static_cast< const PBType_t * >( this ); };

	virtual INetworkSerializerPB *GetSerializerPB() const
	{
		static INetworkSerializerPB *s_pSerializerPB = nullptr;

		if ( !s_pSerializerPB && g_pNetworkMessages )
		{
			s_pSerializerPB = g_pNetworkMessages->FindOrCreateNetMessage( kMsgId, &sm_binding, sizeof( MyType_t ) );
		}

		return s_pSerializerPB;
	}

	virtual CNetMessage *Clone() const
	{
		auto *pClone = Alloc< MyType_t >();

		*pClone = *this;

		return static_cast< CNetMessage * >( pClone );
	}

public:
	const char *GetName() const { return PBType_t::GetTypeName().c_str(); }

	struct NetMessageInfo_t *GetProtoInfo() const
	{
		INetworkSerializerPB *pSerializerPB = GetSerializerPB();

		if ( !pSerializerPB )
			return nullptr;

		return pSerializerPB->GetNetMessageInfo();
	}

	IProtobufBinding *GetProtoBindingInfo() const
	{
		NetMessageInfo_t *pPrtobufInfo = GetProtoInfo();

		if ( !pPrtobufInfo )
			return nullptr;

		return pPrtobufInfo->m_pBinding;
	}
};

template< NetworkMessageId ID, typename PROTO_TYPE, NetChannelBufType_t BUF_TYPE = BUF_RELIABLE >
class CUserMessagePB : public CNetMessagePB< ID, PROTO_TYPE, SG_USERMSG, BUF_TYPE > {};

#endif // NETMESSAGE_H

