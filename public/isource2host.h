//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Host interface exposed to the engine, client and server
//
//===========================================================================//

#ifndef ISOURCE2HOST_H
#define ISOURCE2HOST_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier0/uniqueid.h"
#include "tier0/globalsymbol.h"
#include "appframework/iappsystem.h"

class IAnimCPPScript;

abstract_class ISource2Host : public IAppSystem
{
public:
	virtual void OutOfGameFrameBoundary( float flFrameTime ) = 0;

	virtual void GetDemoVersion( CUtlString &sVersionName, UniqueId_t &versionGuid ) = 0;

	// Registered C++ anim scripts, sorted by name symbol
	virtual int GetAnimScriptCount() = 0;

	virtual CGlobalSymbol GetAnimScriptName( int nIndex ) = 0;

	virtual IAnimCPPScript *CreateAnimScript( const CGlobalSymbol &scriptName, void *pBindContext ) = 0;
};

#endif // ISOURCE2HOST_H
