//========= Copyright © 1996-2009, Valve Corporation, All rights reserved. ============//

#ifndef _IMATCHSYSTEM_H_
#define _IMATCHSYSTEM_H_

class IPlayerManager;
class IGameManager;
class IMatchVoice;

class IMatchSystem
{
public:
	virtual IPlayerManager * GetPlayerManager() = 0;

	virtual IMatchVoice * GetMatchVoice() = 0;
};

#endif

