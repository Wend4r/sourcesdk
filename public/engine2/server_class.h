//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef SERVER_CLASS_H
#define SERVER_CLASS_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/dbg.h"
#include "dt_send.h"
#include "networkstringtabledefs.h"
#include "networksystem/iflattenedserializers.h"

class CEntityClass;

class ServerClass
{
public:
	const char*	GetName()		{ return m_pNetworkName; }

public:
	const char *m_pDLLClassName; // On the server this matches the m_pNetworkName, on the client it is prefixed with C_*
	const char *m_pNetworkName;
	
	CEntityClass *m_pEntityClass;

	FlattenedSerializerDesc_t m_flattenedSerializer;

	int m_ClassID;

	// This is an index into the network string table (sv.GetInstanceBaselineTable()).
	int m_InstanceBaselineIndex; // INVALID_STRING_INDEX if not initialized yet.

	// AMNOTE: The server builds every ServerClass in the entity networking setup (server.dll sub_180BFCCD0): it
	// allocates 56 bytes, zeroes the dword at +48 and the byte at +52, sets m_ClassID to the index in the list and
	// m_InstanceBaselineIndex to -1, then fills the flags below. No code of the game was found that writes
	// m_bReservedFlag48 / m_bReservedFlag51 afterwards, so they have no name source.
	bool m_bReservedFlag48;
	// Named by what it does: a class of the entity class hierarchy has bit 8 (0x100) of CEntityClass::m_flags
	bool m_bBaseClassHasFlag8;
	// Named by what it does: a class of the entity class hierarchy has bit 9 (0x200) of CEntityClass::m_flags
	bool m_bBaseClassHasFlag9;
	bool m_bReservedFlag51;
	// Named by what it does: IFlattenedSerializers::GetFieldPathCaches returned both send proxy lists of the
	// serializer and both are non-empty
	bool m_bHasFieldPathCaches;
};


class CBaseNetworkable;

// If you do a DECLARE_SERVERCLASS, you need to do this inside the class definition.
#define DECLARE_SERVERCLASS()									\
	public:														\
		virtual ServerClass* GetServerClass();					\
		static SendTable *m_pClassSendTable;					\
		template <typename T> friend int ServerClassInit(T *);	\
		virtual int YouForgotToImplementOrDeclareServerClass();	\

#define DECLARE_SERVERCLASS_NOBASE()							\
	public:														\
		template <typename T> friend int ServerClassInit(T *);	\

// Use this macro to expose your class's data across the network.
#define IMPLEMENT_SERVERCLASS( DLLClassName, sendTable ) \
	IMPLEMENT_SERVERCLASS_INTERNAL( DLLClassName, sendTable )

// You can use this instead of BEGIN_SEND_TABLE and it will do a DECLARE_SERVERCLASS automatically.
#define IMPLEMENT_SERVERCLASS_ST(DLLClassName, sendTable) \
	IMPLEMENT_SERVERCLASS_INTERNAL( DLLClassName, sendTable )\
	BEGIN_SEND_TABLE(DLLClassName, sendTable)

#define IMPLEMENT_SERVERCLASS_ST_NOBASE(DLLClassName, sendTable) \
	IMPLEMENT_SERVERCLASS_INTERNAL( DLLClassName, sendTable )\
	BEGIN_SEND_TABLE_NOBASE( DLLClassName, sendTable )


#ifdef VALIDATE_DECLARE_CLASS
	#define CHECK_DECLARE_CLASS( DLLClassName, sendTable ) \
		template <typename T> int CheckDeclareClass_Access(T *); \
		template <> int CheckDeclareClass_Access<sendTable::ignored>(sendTable::ignored *, const char *pIgnored) \
		{ \
			return DLLClassName::CheckDeclareClass( #DLLClassName ); \
		} \
		namespace sendTable \
		{ \
			int verifyDeclareClass = CheckDeclareClass_Access( (sendTable::ignored*)0 ); \
		}
#else
	#define CHECK_DECLARE_CLASS( DLLClassName, sendTable )
#endif


#define IMPLEMENT_SERVERCLASS_INTERNAL( DLLClassName, sendTable ) \
	namespace sendTable \
	{ \
		struct ignored; \
		extern SendTable g_SendTable; \
	} \
	CHECK_DECLARE_CLASS( DLLClassName, sendTable ) \
	static ServerClass g_##DLLClassName##_ClassReg(\
		#DLLClassName, \
		&sendTable::g_SendTable\
	); \
	\
	ServerClass* DLLClassName::GetServerClass() {return &g_##DLLClassName##_ClassReg;} \
	SendTable *DLLClassName::m_pClassSendTable = &sendTable::g_SendTable;\
	int DLLClassName::YouForgotToImplementOrDeclareServerClass() {return 0;}


#endif
