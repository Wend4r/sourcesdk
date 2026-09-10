#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/gameappsystem.h"
#include "eiface.h"
#include "tier0/threadtools.h"
#include "tier1/utldelegate.h"
#include "tier1/utlmap.h"
#include "tier1/utlvector.h"

class CBasePlayerController;

class CSource2GameClients : public CGameAppSystem< ISource2GameClients >
{
public:
	struct SvcUserMessageHandlerInfo_t;

	typedef void ( *SvcUserMessageHandlerFn_t )( CSource2GameClients *pThis, uint32 nSize, const void *pBuffer, SvcUserMessageHandlerInfo_t *pInfo, CBasePlayerController *pPlayer );


	struct SvcUserMessageHandlerInfo_t
	{
		SvcUserMessageHandlerFn_t m_pfnHandler;
		CUtlVector< CUtlAbstractDelegate > m_Delegates;
	};

	CUtlMap< int, SvcUserMessageHandlerInfo_t > m_UserMessageHandlers;
	CThreadRWLock_FastRead m_UserMessageLock;
};

#endif // GAMEINTERFACE_H
