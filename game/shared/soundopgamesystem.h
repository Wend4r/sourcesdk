//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Queues the server's sound event messages and sends them after the entities think.
//
//=============================================================================//

#ifndef SOUNDOPGAMESYSTEM_H
#define SOUNDOPGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <tier0/threadtools.h>
#include <tier0/utlsignalslot.h>
#include <tier0/utlvector.h>
#include <tier0/convar.h>
#include <soundsystem/isoundopsystem.h>

// The kind of a queued sound event message
enum SosMessageQueueInfoType_t
{
	SOSMSGQ_START = 0,
	SOSMSGQ_STOP,
	SOSMSGQ_STOP_HASH,
	SOSMSGQ_SOUND_FIELDS,
	SOSMSGQ_LIBRARY_FIELDS,
};

//-----------------------------------------------------------------------------
// The base of the sound event messages the system queues (CSosStartSoundEventInfo_t,
// CSosStopSoundEventInfo_t, CSosStopSoundEventHashInfo_t, CSosSetSoundEventFieldsInfo_t,
// CSosSetLibraryStackFieldsInfo_t and their filtered versions, through
// CSosEventInfoWithFieldData_t for the ones that carry field data), 16 bytes. It declares its
// destructor only; the derived classes add no virtual method.
//-----------------------------------------------------------------------------
abstract_class CSosEventInfo_t
{
public:
	virtual ~CSosEventInfo_t() = 0;

public:
	// Set by the constructor of each derived message
	SosMessageQueueInfoType_t m_msgType;
};

// Named by what it does: the objects of m_MessageListeners, whose first virtual method gets every
// message the system sends
class ISoundOpMessageListener;

// A queue of messages keyed by T
template < class T >
class CSosMessageQueue
{
public:
	struct Entry_t
	{
		T key;
		CSosEventInfo_t *pMessage;
	};

	CUtlVector< Entry_t > m_messages;
};

//-----------------------------------------------------------------------------
// Registered as "SoundOpGameSystem" (static factory). 520 bytes on Windows, 528 on Linux (the
// owner thread ID of CThreadFastMutex is 4 bytes on Windows, 8 on Linux).
//-----------------------------------------------------------------------------
class CSoundOpGameSystem : public CAutoGameSystem, public CUtlSlot
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CSoundOpGameSystem();

	// Returns true
	virtual bool Init() override;
	// Empty
	GS_EVENT( GameInit );
	GS_EVENT( GameActivate );
	GS_EVENT( GameDeactivate );
	// Sends the queued messages to the clients and to m_MessageListeners ("FrameUpdatePostEntityThink")
	GS_EVENT( ServerPostEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// Console command handlers
	void snd_sos_sv_soundevent_start( const CCommandContext &context, const CCommand &args );
	void snd_sos_sv_soundevent_stop_last( const CCommandContext &context, const CCommand &args );
	void snd_sos_sv_test_gender( const CCommandContext &context, const CCommand &args );
	void snd_sos_sv_soundevent_pause_last( const CCommandContext &context, const CCommand &args );
	void snd_sos_sv_soundevent_unpause_last( const CCommandContext &context, const CCommand &args );

public:
	// Named by what the code does with them: zeroed by the constructor, read nowhere
	int m_nZeroedAtConstruction[ 3 ];
	CSosMessageQueue< SoundEventGuid_t > m_serverGuidMessageQueue;
	CSosMessageQueue< uint32 > m_serverLibraryStackMessageQueue;
	CSosMessageQueue< uint32 > m_serverSoundEventHashQueue;
	// Named by what it does: guards the three queues
	CThreadFastMutex m_MessageQueueMutex;
	// The event snd_sos_sv_soundevent_start started last: the whole StartSoundEventInfo_t (packed,
	// 20 bytes)
	StartSoundEventInfo_t m_nLastGuidStarted;
	CConCommandMemberAccessor< CSoundOpGameSystem > m_snd_sos_sv_soundevent_start_register;
	CConCommandMemberAccessor< CSoundOpGameSystem > m_snd_sos_sv_soundevent_stop_last_register;
	CConCommandMemberAccessor< CSoundOpGameSystem > m_snd_sos_sv_test_gender_register;
	CConCommandMemberAccessor< CSoundOpGameSystem > m_snd_sos_sv_soundevent_pause_last_register;
	CConCommandMemberAccessor< CSoundOpGameSystem > m_snd_sos_sv_soundevent_unpause_last_register;
	// Named by what it holds
	CUtlVector< ISoundOpMessageListener * > m_MessageListeners;
};

#endif // SOUNDOPGAMESYSTEM_H
