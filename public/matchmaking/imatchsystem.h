//========= Copyright © 1996-2009, Valve Corporation, All rights reserved. ============//

#ifndef _IMATCHSYSTEM_H_
#define _IMATCHSYSTEM_H_

class IPlayerManager;
class IGameManager;
class IServerManager;
class ISearchManager;
class IMatchVoice;
class IDatacenter;

class IMatchSystem
{
public:
	virtual IPlayerManager * GetPlayerManager() = 0;

	// Returns NULL; the name follows the slot order of the older interface,
	// which declared `GetMatchVoice` next.
	virtual void * Unk_GetMatchVoice( void *p ) = 0;
};

#endif

