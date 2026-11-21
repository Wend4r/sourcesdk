#ifndef GAMEEVENTMANAGER_H
#define GAMEEVENTMANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include <igameevents.h>
#include <kv3lib/keyvalues3.h>
#include <tier0/bufferstring.h>
#include <tier0/threadtools.h>
#include <tier0/utldict.h>
#include <tier0/utlstring.h>
#include <tier0/utlsymbollarge.h>
#include <tier0/utlvector.h>

class CGameEventCallback
{
public:
	IGameEventListener2	*m_pCallback;		// callback pointer
	int					m_nListenerType;	// client or server side ?
};

// One key of an event, replaces the KeyValues of older games. Named by what it is
class CGameEventKeyDescriptor
{
public:
	CKV3MemberName			name;			// key name and its hash, the string lives in nameBuffer
	CBufferStringN< 32 >	nameBuffer;
	CUtlString				description;	// "desc" of the key
	int						type;			// CGameEventManager::TYPE_*
};

class CGameEventDescriptor
{
public:
	CUtlSymbolLarge	eventFile;		// events file the event was loaded from
	int				eventid;		// network index number, -1 = not networked
	int				elementIndex;	// index of the name in CGameEventManager::m_EventMap, returned by IGameEvent::GetID
	CUtlVector< CGameEventCallback * >	listeners;	// registered listeners
	bool			local;			// local event, never tell clients about that
	bool			reliable;		// send this event as reliable message
	bool			hasToolsDescription;	// set together with toolsDescription

	// Extra data for network monitoring
	int				numSerialized;

	CUtlString		toolsDescription;	// "_tools_event_desc" of the event
	CUtlVector< CGameEventKeyDescriptor >	keys;	// data types of the event

	int				totalSerializedBits;
	int				totalUnserializedBits;
};

// NOTE: Every non-threadsafe externally-callable function must lock m_mutex before modifying the event manager
// member variables. Client & server threads can call into this class simultaneously
class CGameEventManager : public IGameEventManager2
{
public:	// IGameEventManager functions

	enum
	{
		SERVERSIDE = 0,		// this is a server side listener, event logger etc
		CLIENTSIDE,			// this is a client side listener, HUD element etc
		CLIENTSTUB,			// this is a serverside stub for a remote client listener (used by engine only)
	};

	enum
	{
		TYPE_LOCAL = 0,			// not networked
		TYPE_STRING,			// zero terminated ASCII string
		TYPE_FLOAT,				// float 32 bit
		TYPE_LONG,				// signed int 32 bit
		TYPE_SHORT,				// signed int 16 bit
		TYPE_BYTE,				// unsigned int 8 bit
		TYPE_BOOL,				// unsigned int 1 bit
		TYPE_UINT64,			// unsigned int 64 bit
		TYPE_EHANDLE,			// "player_pawn", or "ehandle" in a "type" value
		TYPE_PLAYER_CONTROLLER,	// "player_controller"; "player_controller_and_pawn" adds a "<key>_pawn" key too
	};

	// An events file that LoadEventsFromFile loaded. Named by what it is
	struct EventFile_t
	{
		CUtlSymbolLarge	m_FileName;
		bool			m_bSearchAll;
	};

	virtual ~CGameEventManager();

	virtual int  LoadEventsFromFile( const char *filename, bool bSearchAll ) OVERRIDE;
	virtual void Reset() OVERRIDE;

	virtual int  AddListener( IGameEventListener2 *listener, const char *name, bool bServerSide ) OVERRIDE;
	virtual bool FindListener( IGameEventListener2 *listener, const char *name ) OVERRIDE;
	virtual void RemoveListener( IGameEventListener2 *listener ) OVERRIDE;

	virtual IGameEvent *CreateEvent( const char *name, bool bForce = false, int *pCookie = NULL ) OVERRIDE;
	virtual bool FireEvent( IGameEvent *event, bool bDontBroadcast = false ) OVERRIDE;
	virtual bool FireEventClientSide( IGameEvent *event ) OVERRIDE;
	virtual IGameEvent *DuplicateEvent( IGameEvent *event ) OVERRIDE;
	virtual void FreeEvent( IGameEvent *event ) OVERRIDE;

	virtual bool SerializeEvent( IGameEvent *event, Source1LegacyGameEvent_t *ev ) OVERRIDE;
	virtual IGameEvent *UnserializeEvent( const Source1LegacyGameEvent_t &ev ) OVERRIDE;

	virtual int  LookupEventId( const char *name ) OVERRIDE;
	virtual void PrintEventToString( IGameEvent *event, CUtlString &out ) OVERRIDE;
	virtual bool HasEventDescriptor( const char *name ) OVERRIDE;

public:
	const char *GetEventName( const CGameEventDescriptor &descriptor ) const { return m_EventMap.GetElementName( descriptor.elementIndex ); }

	CUtlVector< CGameEventDescriptor >	m_GameEvents;		// list of all known events, at most 512
	CUtlVector< CGameEventCallback * >	m_Listeners;		// list of all registered listeners
	CUtlSymbolTableLarge				m_EventFiles;		// list of all loaded event files
	CUtlVector< EventFile_t >			m_EventFileNames;
	CUtlDict< int, int >				m_EventMap;			// event name -> index in m_GameEvents
	CThreadFastMutex					m_mutex;			// lock this when modifying the event table

	bool	m_bClientListenersChanged;	// true every time client changed listeners
};

#endif // GAMEEVENTMANAGER_H
