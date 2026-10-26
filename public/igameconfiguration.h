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

#include "engine2/isource2clientconfig.h"
#include "eiface.h"

class KeyValues;
class GameSessionConfiguration_t;
class ILoopModePrerequisiteRegistry;
class CEntityInstance;
struct StringTableDef_t;

#define GAMECONFIGURATION_CLIENT_INTERFACE_VERSION "GameConfigClientV001"
#define GAMECONFIGURATION_SERVER_INTERFACE_VERSION "GameConfigServerV001"

abstract_class IGameConfiguration : public ISource2ClientConfig, public ISource2ServerConfig
{
public:
	virtual const unsigned char *GetEncryptionKey() = 0;
	virtual CEntityInstance *FindProceduralEntity( const char *pszName ) = 0;
	virtual bool ParseGameSessionConfiguration( KeyValues *pOptions, GameSessionConfiguration_t *pConfig ) = 0;
	virtual bool SetWorldSession( GameSessionConfiguration_t *pConfig ) = 0;
	virtual void ClearWorldSession() = 0;
	virtual const char *GetModName() = 0;
	virtual bool LoopPostInitAllGameSystems( const GameSessionConfiguration_t &config, ILoopModePrerequisiteRegistry *pRegistry ) = 0;
	// Empty in the game's configurations
	virtual void RegisterGameActivities() = 0;
	virtual void ExecuteURL( const char *pszURLQuery ) = 0;
	virtual void GetBugReportLaunchURLParameters( KeyValues *pParameters ) = 0;
	virtual void GetStringTableDefs( CUtlVector< StringTableDef_t > &defs ) = 0;
	virtual bool IsUserMessage( int nMessageId ) = 0;
	// KeyValues stored by SetWorldSession
	virtual KeyValues *GetSessionKeyValues() = 0;
	virtual bool IsToolControlledSession() = 0;
};

#endif // IGAMECONFIGURATION_H
