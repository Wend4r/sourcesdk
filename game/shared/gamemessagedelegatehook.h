#ifndef GAMEMESSAGEDELEGATEHOOK_H
#define GAMEMESSAGEDELEGATEHOOK_H

#pragma once

#include "tier0/platform.h"
#include "tier0/utlsignalslot.h"
#include "networksystem/inetworkserializer.h"

class CNetMessage;

//-----------------------------------------------------------------------------
// A hook of one network message: what the owner of the hooks (CGameParticleManager keeps a list
// of them) calls. No data: 8 bytes, the virtual table pointer.
//
// Windows slots 0-4; on Linux the destructor takes two slots.
//-----------------------------------------------------------------------------
abstract_class IGameMessageDelegateHook
{
public:
	// Removes the hook from the network message system when it is hooked and logs "%d unhooked %s"
	virtual void Unhook() = 0;
	// Sets m_bMarkedForDestruction, which Dispatch checks before it calls the delegate; no name source
	virtual void Unk_1() = 0;
	// The id of the hooked message
	virtual NetworkMessageId GetMessageId() const = 0;
	// The name the network message system registered for that message
	virtual const char *GetMessageName() const = 0;

	// Logs and unhooks
	virtual ~IGameMessageDelegateHook() = 0;
};

//-----------------------------------------------------------------------------
// Hooks the network message T and hands every one it receives to a delegate (CGameParticleManager
// hooks CUserMsg_ParticleManager_t this way). 80 bytes on Linux, 72 on Windows (the member function
// pointer of the delegate is 16 bytes on Linux, 8 on Windows). The network message system calls
// Dispatch through a delegate bound to the hook and to the CUtlSlot base; every hook also counts
// in a global hash set of the live hooks.
//-----------------------------------------------------------------------------
template < class T >
abstract_class CGameMessageDelegateHook : public IGameMessageDelegateHook, public CUtlSlot
{
public:
	typedef CUtlDelegate< void ( int, const T * ) > Delegate_t;

	// Logs when the message was never hooked or the hook is marked for destruction; otherwise hands
	// the message and the slot of the client it came from to m_Delegate. The argument type is not
	// confirmed
	virtual void Dispatch( const CNetMessage *pMessage ) = 0;

public:
	// What receives the messages: the object and its member function
	Delegate_t m_Delegate;

	// Set to -1 by the constructor; nothing else reads or writes it
	int32 m_nReserved;

	// True while the message is hooked in the network message system; Unhook clears it
	bool m_bHooked;

	// Cleared by the constructor; nothing else reads or writes it
	bool m_bReserved;

	// Set by Unk_1 before the owner unhooks and destroys the hook; Dispatch then drops the messages
	bool m_bMarkedForDestruction;
};

#endif // GAMEMESSAGEDELEGATEHOOK_H
