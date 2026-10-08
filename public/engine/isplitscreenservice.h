//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service that tracks the local split screen players
//
//=============================================================================//

#ifndef ISPLITSCREENSERVICE_H
#define ISPLITSCREENSERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <IEngineService.h>
#include <splitscreenslot.h>
#include <playerslot.h>

class INetChannel;
class CUtlBuffer;

abstract_class ISplitScreenService : public IEngineService
{
public:
	virtual ~ISplitScreenService() {}

	virtual bool AddSplitScreenUser( CSplitScreenSlot nSlot, void *pSplitPlayer ) = 0;
	virtual bool AddBaseUser( CSplitScreenSlot nSlot, void *pSplitPlayer ) = 0;
	virtual bool RemoveSplitScreenUser( CSplitScreenSlot nSlot ) = 0;

	virtual CSplitScreenSlot GetActiveSplitScreenPlayerSlot() = 0;
	virtual CSplitScreenSlot SetActiveSplitScreenPlayerSlot( CSplitScreenSlot nSlot ) = 0;

	virtual bool IsValidSplitScreenSlot( CSplitScreenSlot nSlot ) = 0;
	virtual CSplitScreenSlot FirstValidSplitScreenSlot() = 0;
	// Returns -1 when there is no further valid slot
	virtual CSplitScreenSlot NextValidSplitScreenSlot( CSplitScreenSlot nPreviousSlot ) = 0;

	virtual int GetNumSplitScreenPlayers() = 0;
	virtual CPlayerSlot GetSplitScreenPlayer( CSplitScreenSlot nSlot ) = 0;
	virtual INetChannel *GetSplitScreenPlayerNetChan( CSplitScreenSlot nSlot ) = 0;

	virtual bool IsDisconnecting( CSplitScreenSlot nSlot ) = 0;
	virtual void SetDisconnecting( CSplitScreenSlot nSlot, bool bState ) = 0;

	// Thread-local flag; returns the previous value
	virtual bool SetLocalPlayerIsResolvable( const char *pszContext, int nLine, bool bResolvable ) = 0;
	virtual bool IsLocalPlayerResolvable() = 0;

	// Number of split screen players the game supports (1 to 4)
	virtual int GetMaxSplitScreenPlayers() = 0;

	// Searches every active slot, including slot 0
	virtual bool IsSplitScreenPlayer( CPlayerSlot nPlayerSlot ) = 0;
	virtual CSplitScreenSlot GetSplitScreenSlotForPlayer( CPlayerSlot nPlayerSlot ) = 0;

	virtual bool ReadUserConfig( const char *pszName, const CSplitScreenSlot &nSlot, CUtlBuffer &buf ) = 0;
	virtual bool WriteUserConfig( const char *pszName, const CSplitScreenSlot &nSlot, CUtlBuffer &buf ) = 0;
};

#endif // ISPLITSCREENSERVICE_H
