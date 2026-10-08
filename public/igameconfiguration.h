//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Game configuration shared by the client and the server
//
//===========================================================================//

#ifndef IGAMECONFIGURATION_H
#define IGAMECONFIGURATION_H

#ifdef _WIN32
#pragma once
#endif

#include "isource2clientconfig.h"
#include "eiface.h"

class KeyValues;
class GameSessionConfiguration_t;

#define GAMECONFIGURATION_CLIENT_INTERFACE_VERSION "GameConfigClientV001"
#define GAMECONFIGURATION_SERVER_INTERFACE_VERSION "GameConfigServerV001"

abstract_class IGameConfiguration : public ISource2ClientConfig, public ISource2ServerConfig
{
public:
	virtual void *unk019() = 0;
	virtual void *unk020( const char *pszName ) = 0;
	virtual bool ParseGameSessionConfiguration( KeyValues *pOptions, GameSessionConfiguration_t *pConfig ) = 0;
	virtual bool SetWorldSession( GameSessionConfiguration_t *pConfig ) = 0;
	virtual void ClearWorldSession() = 0;
	virtual const char *unk024() = 0;
	virtual bool unk025( GameSessionConfiguration_t *pConfig, void *p ) = 0;
	virtual void *unk026() = 0;
	virtual void unk027( const char *pszURLQuery ) = 0;
	virtual void *unk028() = 0;
	virtual void unk029( void *pOutVector ) = 0;
	virtual bool unk030( int nMessageId ) = 0;
	// KeyValues stored by SetWorldSession
	virtual KeyValues *GetSessionKeyValues() = 0;
	virtual bool IsToolControlledSession() = 0;
};

#endif // IGAMECONFIGURATION_H
