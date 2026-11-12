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
	// Creates an empty CSteamAudioScene
	virtual void *unk013() = 0;
	// Creates a CSteamAudioScene
	virtual void *Unk_CreateScene( void *p ) = 0;
	// First element of an internal list, NULL when it is empty
	virtual void *unk015() = 0;
	virtual void unk016() = 0;
	virtual void unk017() = 0;
	virtual void unk018() = 0;
	virtual void unk019() = 0;
	virtual void unk020() = 0;
	virtual void *Unk_CreateSource3DProcessor( void *pParams ) = 0;
	virtual void Unk_DestroySource3DProcessor( void *pProcessor ) = 0;
	virtual void *Unk_CreatePathingProcessor( void *pParams ) = 0;
	virtual void Unk_DestroyPathingProcessor( void *pProcessor ) = 0;
	virtual void *Unk_CreateDirectProcessor( void *pParams ) = 0;
	virtual void Unk_DestroyDirectProcessor( void *pProcessor ) = 0;
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
	virtual void *unk046() = 0;
	virtual void unk047() = 0;
	virtual void unk048() = 0;
	virtual void unk049() = 0;
	virtual void unk050() = 0;
	virtual bool unk051() = 0;
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
	virtual int unk064( int nIndex ) = 0;
	virtual int unk065( int nIndex ) = 0;
	virtual void unk066() = 0;
	virtual void unk067() = 0;
	virtual void unk068( int nUnknown1, int nUnknown2, float flUnknown3, void *pUnknown4 ) = 0;
	virtual bool unk069( int nUnknown ) = 0;
};

#endif // ISTEAMAUDIO_H
