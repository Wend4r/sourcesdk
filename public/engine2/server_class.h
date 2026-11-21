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


#endif
