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
	// Set by the "F" argument of demo_goto; SkipToTick of this build does not read it
	DEMO_SKIP_F_ARGUMENT = 1 << 2,
};

//-----------------------------------------------------------------------------
// Purpose: The demo player of the engine, IVEngineClient2::GetDemoPlayer
//
// AMNOTE: Names come from the CDemoPlayer vtable of Dota 2 2018 (libengine2.dylib symbols) where the CS2 slot does
// the same thing; the others are named by what the code does.
//-----------------------------------------------------------------------------
abstract_class IDemoPlayer
{
public:
	virtual ~IDemoPlayer() {}

	virtual CDemoFile *GetDemoFile() = 0;
	virtual int GetPlaybackStartTick() = 0;
	virtual int GetPlaybackTick() = 0;
	// Same implementation as GetPlaybackTick
	virtual int GetPlaybackDeltaTick() = 0;
	virtual int GetTotalTicks() = 0;
	virtual float TicksToTime( int nTicks ) = 0;

	// pOptions is copied, GetPlaybackOptions returns the copy
	virtual void PlayDemo( const char *pszFilename, KeyValues *pOptions ) = 0;
	virtual bool StartPlayback( const char *pszFilename, bool bAsTimeDemo, KeyValues *pOptions ) = 0;

	// Named by what it does: set together with GetAllowSpawnGroupParsing once the signon packets are read, cleared
	// when a demo starts or stops
	virtual bool HasFinishedSignon() = 0;
	virtual KeyValues *GetPlaybackOptions() = 0;

	virtual bool IsPlayingBack() = 0;
	virtual bool IsPlaybackPaused() = 0;
	virtual bool IsPlayingTimeDemo() = 0;
	virtual bool IsSkipping() = 0;
	// AMNOTE: Returns false on both platforms; no source names it and nothing in the engine calls it
	virtual bool ConstantFalseQuery() = 0;

	virtual void SetPlaybackTimeScale( float flTimeScale ) = 0;
	virtual float GetPlaybackTimeScale() = 0;
	virtual void PausePlayback( float flSeconds ) = 0;
	// nFlags is a combination of DemoSkipFlags_t
	virtual void SkipToTick( int nTick, int nFlags ) = 0;
	virtual void ResumePlayback() = 0;
	virtual void StopPlayback() = 0;

	// Null when there is no packet to process this frame
	virtual void *ReadPacket() = 0;
	// Named by what it does: true in this build; the network game client reads demo packets in a loop while it is set
	virtual bool ShouldReadPacketsThisFrame() = 0;
	// Named by what it does: the next buffered packet whose time is past nTime, reading more of the file when the
	// buffer runs out (the broadcast playback loop of the network game client)
	virtual void *ReadPacketForTime( uint64 nTime ) = 0;
	// AMNOTE: Returns false on both platforms; no source names it and nothing in the engine calls it
	virtual bool ConstantFalseQuery2() = 0;
	// Set while an .evidence file is played back
	virtual void *GetEvidenceData() = 0;
	virtual bool GetClosestSave( int nTick, void *pSaveGame ) = 0;
	// Named by what they do: a map of per-key tick ranges the demo player builds while it reads the file
	virtual bool HasTickRangeData() = 0;
	virtual bool FindTickRangeData( int nTick, uint32 nKey, void *pOut, int *pIndex ) = 0;
	// Calls StopPlayback while playing back
	virtual void Disconnect() = 0;

	virtual void SetIgnorePacketsForResourceLoading( bool bIgnore ) = 0;
	// Set once the first packet after signon is read
	virtual bool GetAllowSpawnGroupParsing() = 0;
	virtual void LoadSpawnGroups() = 0;

	// Named by what it does: when set, the end of the demo pauses playback instead of requesting the idle host state
	virtual void SetPauseAtEnd( bool bPause ) = 0;
	// Named by what it does: the demo console commands do nothing while it is set
	virtual void SetDemoCommandsLocked( bool bLocked ) = 0;
	virtual bool AreDemoCommandsLocked() = 0;
	// Named by what it does: the pause mode PausePlayback and ResumePlayback switch on; 1 hands the pause to
	// ISource2Client::SetDemoPlaybackPaused
	virtual void SetPauseMode( int nMode ) = 0;
	// Named by what they do: subtract and add the tick offset of the demo
	virtual int RemoveTickOffset( int nTick ) = 0;
	virtual int AddTickOffset( int nTick ) = 0;
	// Named by what they do: write out and reset the demo frame stats ("Closed demo frame stats file '%s'"); the
	// first one only for nMode 2
	virtual void FlushFrameStats( int nMode ) = 0;
	virtual double ResetFrameStats() = 0;
};

#endif // IDEMOPLAYER_H
