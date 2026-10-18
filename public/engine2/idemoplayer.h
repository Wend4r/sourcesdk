//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Demo playback interface of the engine
//
//===========================================================================//

#ifndef IDEMOPLAYER_H
#define IDEMOPLAYER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CDemoFile;
class KeyValues;

// Flags of IDemoPlayer::SkipToTick, the "relative" and "pause" arguments of demo_goto
enum DemoSkipFlags_t : int
{
	DEMO_SKIP_RELATIVE = 1 << 0,
	DEMO_SKIP_PAUSE = 1 << 1,
	// Set by the "F" argument of demo_goto
	DEMO_SKIP_UNK_F = 1 << 2,
};

//-----------------------------------------------------------------------------
// Purpose: The demo player of the engine, IVEngineClient2::GetDemoPlayer
//-----------------------------------------------------------------------------
abstract_class IDemoPlayer
{
public:
	virtual ~IDemoPlayer() {}

	virtual CDemoFile *GetDemoFile() = 0;
	virtual int GetPlaybackStartTick() = 0;
	virtual int GetPlaybackTick() = 0;
	// Same implementation as GetPlaybackTick
	virtual int unk004() = 0;
	virtual int GetTotalTicks() = 0;
	virtual float TicksToTime( int nTicks ) = 0;

	// pOptions is copied, GetPlaybackOptions returns the copy
	virtual void PlayDemo( const char *pszFilename, KeyValues *pOptions ) = 0;
	virtual bool StartPlayback( const char *pszFilename, bool bAsTimeDemo, KeyValues *pOptions ) = 0;

	// Set once the first packet after signon is read, cleared when a demo starts or stops
	virtual bool unk009() = 0;
	virtual KeyValues *GetPlaybackOptions() = 0;

	virtual bool IsPlayingBack() = 0;
	virtual bool IsPlaybackPaused() = 0;
	virtual bool IsPlayingTimeDemo() = 0;
	virtual bool IsSkipping() = 0;
	virtual bool unk015() = 0;

	virtual void SetPlaybackTimeScale( float flTimeScale ) = 0;
	virtual float GetPlaybackTimeScale() = 0;
	virtual void PausePlayback( float flSeconds ) = 0;
	// nFlags is a combination of DemoSkipFlags_t
	virtual void SkipToTick( int nTick, int nFlags ) = 0;
	virtual void ResumePlayback() = 0;
	virtual void StopPlayback() = 0;

	// Null when there is no packet to process this frame
	virtual void *ReadPacket() = 0;
	virtual bool unk023() = 0;
	virtual void *unk024( uint64 nUnk ) = 0;
	virtual bool unk025() = 0;
	// Set while an .evidence file is played back
	virtual void *unk026() = 0;
	virtual bool unk027( int nTick, void *pUnk ) = 0;
	virtual bool unk028() = 0;
	virtual bool unk029( int nTick, uint32 nUnk, void *pUnk1, void *pUnk2 ) = 0;
	// Calls StopPlayback
	virtual void unk030() = 0;

	virtual void SetIgnorePacketsForResourceLoading( bool bIgnore ) = 0;
	// Set once the first packet after signon is read
	virtual bool unk032() = 0;
	virtual void LoadSpawnGroups() = 0;

	virtual void unk034( bool bUnk ) = 0;
	virtual void unk035( bool bUnk ) = 0;
	// Value of unk035, the demo console commands do nothing while it is set
	virtual bool unk036() = 0;
	virtual void unk037( int nUnk ) = 0;
	virtual int unk038( int nTick ) = 0;
	// Inverse of unk038
	virtual int unk039( int nTick ) = 0;
	virtual void unk040( int nUnk ) = 0;
	virtual void unk041() = 0;
};

#endif // IDEMOPLAYER_H
