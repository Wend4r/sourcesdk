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

	// Returns NULL
	virtual IMatchVoice * GetMatchVoice() = 0;
};

#endif

