//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Shared object based on a CBaseRecord subclass
//
//=============================================================================

#ifndef PROTOBUFSHAREDOBJECT_H
#define PROTOBUFSHAREDOBJECT_H
#ifdef _WIN32
#pragma once
#endif

#include "google/protobuf/descriptor.h"
#include "tier0/keyvalues.h"


namespace google
{
	namespace protobuf
	{
		class Message;
	}
}

namespace GCSDK
{

//----------------------------------------------------------------------------
// Purpose: Base class for CProtoBufSharedObject. This is where all the actual
//			code lives.
//----------------------------------------------------------------------------
class CProtoBufSharedObjectBase : public CSharedObject
{
public:
	typedef CSharedObject BaseClass;

	virtual bool BParseFromMessage( SOID_t ID, const CUtlBuffer & buffer ) OVERRIDE;
	virtual bool BUpdateFromNetwork( const CSharedObject & objUpdate ) OVERRIDE;

	virtual bool BIsKeyLess( const CSharedObject & soRHS ) const ;
	virtual void Copy( const CSharedObject & soRHS );
	virtual void Dump() const OVERRIDE;
	virtual int FindIndexInVector( const CUtlVector< CSharedObject * > &vecObjects, const CUtlVector< int > &vecIndices ) const OVERRIDE;
	virtual bool BAddToMessage( std::string *pBuffer, bool bUnk ) const OVERRIDE;
	virtual bool BAddDestroyToMessage( std::string *pBuffer ) const OVERRIDE;


	// Static helpers
	static void Dump( const ::google::protobuf::Message & msg );
	static KeyValues *CreateKVFromProtoBuf( const ::google::protobuf::Message & msg );
	static void RecursiveAddProtoBufToKV( KeyValues *pKVDest, const ::google::protobuf::Message & msg );

protected:
	virtual ::google::protobuf::Message *GetPObject() = 0;
	const ::google::protobuf::Message *GetPObject() const { return const_cast<CProtoBufSharedObjectBase *>(this)->GetPObject(); }

private:

};


//----------------------------------------------------------------------------
// Purpose: Template for making a shared object that uses a specific protobuf
//			message class for its wire protocol and in-memory representation.
//----------------------------------------------------------------------------
template< typename Message_t, int nTypeID >
class CProtoBufSharedObject : public CProtoBufSharedObjectBase
{
public:
	~CProtoBufSharedObject()
	{
	}

	virtual int GetTypeID() const { return nTypeID; }

	Message_t & Obj() { return m_msgObject; }
	const Message_t & Obj() const { return m_msgObject; }

	typedef Message_t SchObjectType_t;
	const static int k_nTypeID = nTypeID;

protected:
	::google::protobuf::Message *GetPObject() { return &m_msgObject; }

private:
	Message_t m_msgObject;
};

//----------------------------------------------------------------------------
// Purpose: Template for making a shared object that uses a specific protobuf
//          message class for its wire protocol and in-memory representation.
//
//          The wrapper version of this class wraps a message allocated and
//          owned elsewhere. The user of this class is in charge of
//          guaranteeing that lifetime.
//----------------------------------------------------------------------------
template< typename Message_t, int nTypeID >
class CProtoBufSharedObjectWrapper : public CProtoBufSharedObjectBase
{
public:
	CProtoBufSharedObjectWrapper( Message_t *pMsgToWrap )
		: m_pMsgObject( pMsgToWrap )
	{}

	~CProtoBufSharedObjectWrapper()
	{
	}

	virtual int GetTypeID() const { return nTypeID; }

	Message_t & Obj() { return *m_pMsgObject; }
	const Message_t & Obj() const { return *m_pMsgObject; }

	typedef Message_t SchObjectType_t;
public:
	const static int k_nTypeID = nTypeID;

protected:
	::google::protobuf::Message *GetPObject() { return m_pMsgObject; }

private:
	Message_t *m_pMsgObject;
};

} // GCSDK namespace

#endif //PROTOBUFSHAREDOBJECT_H
