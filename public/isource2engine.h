//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Engine interface shared by the server and client DLL interfaces
//
//===========================================================================//

#ifndef ISOURCE2ENGINE_H
#define ISOURCE2ENGINE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/iappsystem.h"

class CResourceManifestPrerequisite;
class CEntityLump;
class IScreenshotCallback;

//-----------------------------------------------------------------------------
// Purpose: Interface the engine exposes to the game DLL and client DLL
//-----------------------------------------------------------------------------
abstract_class ISource2Engine : public IAppSystem
{
public:
	// Is the game paused?
	virtual bool		IsPaused() = 0;

	// What is the game timescale multiplied with the host_timescale?
	virtual float		GetTimescale( void ) const = 0;

	virtual void		*FindOrCreateWorldSession( const char *pszWorldName, CResourceManifestPrerequisite * ) = 0;

	// Creates the world pszWorldName in the world session pszWorldSessionName (see FindOrCreateWorldSession) and returns its entity lump pszLumpName, or NULL when the world or the lump is not available.
	// bLoadLump requests loading of the lump first.
	// bAllowEmptyLumpName accepts an empty or NULL pszLumpName.
	virtual CEntityLump	*GetEntityLumpForTemplate( const char *pszWorldSessionName, bool bLoadLump, const char *pszWorldName, const char *pszLumpName, bool bAllowEmptyLumpName ) = 0;

	virtual uint32		GetStatsAppID() const = 0;

	// Queues a JPEG screenshot to the screenshot service; -1 for nWidth / nHeight uses the screen size
	virtual void		WriteJpegScreenshot( const char *pszFilename, int nQuality, int nWidth, int nHeight, bool bAsyncWrite ) = 0;

	// Queues a screenshot sized by screenshot_width / screenshot_height; the uncompressed image is handed to
	// pCallback->OnRawScreenshot() together with pContext
	virtual void		RequestScreenshot( IScreenshotCallback *pCallback, void *pContext ) = 0;
};

#endif // ISOURCE2ENGINE_H
