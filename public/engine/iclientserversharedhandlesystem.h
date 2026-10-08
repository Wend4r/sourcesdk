//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Serial-checked handles to objects shared between client and server code
//
//=============================================================================//

#ifndef ICLIENTSERVERSHAREDHANDLESYSTEM_H
#define ICLIENTSERVERSHAREDHANDLESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>

struct ClientServerSharedHandle_t
{
	int m_nIndex;
	int m_nSerial;

	bool IsValid() const { return m_nIndex != 0 && m_nSerial != 0; }
};
COMPILE_TIME_ASSERT( sizeof( ClientServerSharedHandle_t ) == 0x8 );

abstract_class IClientServerSharedHandleSystem : public IAppSystem
{
public:
	// Returns an invalid handle for a NULL object.
	virtual ClientServerSharedHandle_t CreateHandle( void *pObject ) = 0;

	virtual void DestroyHandle( ClientServerSharedHandle_t hHandle ) = 0;

	// Returns NULL for an invalid or stale handle.
	virtual void *GetHandleObject( ClientServerSharedHandle_t hHandle ) = 0;
};

#endif // ICLIENTSERVERSHAREDHANDLESYSTEM_H
