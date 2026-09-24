//========= Copyright Valve Corporation, All rights reserved. ============//
//
//
//
//=============================================================================
#ifndef GC_CLIENTSYSTEM_H
#define GC_CLIENTSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include <tier0/utlstring.h>
#include <tier1/utlmap.h>

#include <gcsdk_gcmessages.pb.h>

#include "igamesystem.h"
#include "gcsdk/gcclient.h"
#include "gcsdk/gcclient_sharedobjectcache.h"

class CGCClientSystem : public CAutoGameSystem
{
public:
	CGCClientSystem();
	virtual ~CGCClientSystem();

	GCSDK::CGCClient *GetGCClient() { return &m_GCClient; }
	const GCSDK::CGCClient *GetGCClient() const { return &m_GCClient; }

	bool BConnectedtoGC() const { return m_bConnectedToGC; }
	const char *GetTxnCountryCode() const { return m_sTxnCountryCode.Get(); }

protected:
	bool m_bConnectedToGC;

	// the GC's rtime32 minus our own at the moment the welcome arrived
	int32 m_nGCRTimeDelta;
	uint32 m_rtimeGCWelcome;
	double m_timeGCWelcome;

	// the account's store fields, taken from CMsgClientWelcome
	uint32 m_eCurrency;
	CUtlString m_sBalanceUrl;
	CUtlString m_sTxnCountryCode;
	uint32 m_unBalance;
	bool m_bHasBalance;

	CMsgClientWelcome_Location m_msgLocation;

	CUtlMap< char *, CUtlString, int > m_mapKeyedStrings;

	bool m_bInittedGC;

	GCSDK::CGCClient m_GCClient;

	CCallback< CGCClientSystem, SteamServersConnected_t, true > m_CallbackLogonSuccess;
};

class CCSGCServerSystem : public CGCClientSystem, public GCSDK::ISharedObjectListener
{
public:
	CCSGCServerSystem();
	virtual ~CCSGCServerSystem();

protected:
	// .. not reversed vars
};

#endif // GC_CLIENTSYSTEM_H
