//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef ISTEAMAUDIO_H
#define ISTEAMAUDIO_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "tier0/utlstring.h"

abstract_class ISteamAudio : public IAppSystem
{
public:
	virtual ~ISteamAudio() {}
	virtual void unk012() = 0;
	virtual void unk013() = 0;
	// Creates a CSteamAudioScene
	virtual void *Unk_CreateScene( void *p ) = 0;
	virtual void unk015() = 0;
	virtual void unk016() = 0;
	virtual void unk017() = 0;
	virtual void unk018() = 0;
	virtual void unk019() = 0;
	virtual void unk020() = 0;
	virtual void unk021() = 0;
	virtual void unk022() = 0;
	virtual void unk023() = 0;
	virtual void unk024() = 0;
	virtual void unk025() = 0;
	virtual void unk026() = 0;
	virtual void unk027() = 0;
	virtual void unk028() = 0;
	virtual void unk029() = 0;
	virtual void unk030() = 0;
	virtual void unk031() = 0;
	// pPath with its extension replaced by .sareverb
	virtual CUtlString GetReverbDataFilename( const char *pPath ) = 0;
	// pPath with its extension replaced by .sapaths
	virtual CUtlString GetPathingDataFilename( const char *pPath ) = 0;
	virtual CUtlString GetDataFilename( const char *pPath, const char *pExtension ) = 0;
	// maps/<pMapName>.sareverb
	virtual CUtlString GetMapReverbDataFilename( const char *pMapName ) = 0;
	// maps/<pMapName>.sapaths
	virtual CUtlString GetMapPathingDataFilename( const char *pMapName ) = 0;
	virtual CUtlString GetMapDataFilename( const char *pMapName, const char *pExtension ) = 0;
	virtual void unk038() = 0;
	virtual void Unk_GetReverbBakeDefaults( void *p ) = 0;
	virtual void Unk_GetPathingBakeDefaults( void *p ) = 0;
	virtual void Unk_GetCustomDataBakeDefaults( void *p ) = 0;
	virtual void unk042() = 0;
	virtual void unk043() = 0;
	virtual void unk044() = 0;
	virtual void unk045() = 0;
	virtual void unk046() = 0;
	virtual void unk047() = 0;
	virtual void unk048() = 0;
	virtual void unk049() = 0;
	virtual void unk050() = 0;
	virtual void unk051() = 0;
	virtual void Unk_BakeReverb( void *p ) = 0;
	virtual void Unk_BakePathData( void *p ) = 0;
	virtual void Unk_BakeDimensions( void *p ) = 0;
	virtual void unk055() = 0;
	virtual void unk056() = 0;
	virtual void unk057() = 0;
	virtual void unk058() = 0;
	virtual void unk059() = 0;
	virtual void unk060() = 0;
	virtual void unk061() = 0;
	virtual void unk062() = 0;
	virtual void unk063() = 0;
	virtual void unk064() = 0;
	virtual void unk065() = 0;
	virtual void unk066() = 0;
	virtual void unk067() = 0;
	virtual void unk068() = 0;
	virtual void unk069() = 0;
};

#endif // ISTEAMAUDIO_H
