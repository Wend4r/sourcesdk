//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service for voice chat
//
//=============================================================================//

#ifndef ISOUNDSERVICE_H
#define ISOUNDSERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <engine/IEngineService.h>
#include <entity2/entityindex.h>

abstract_class ISoundService : public IEngineService
{
public:
	// Dota 2 2018 names (CSoundService)
	virtual bool PlayNetworkedVoice( uint64 nSteamID, CEntityIndex nEntityIndex, const void *pDescription, const void *pData, int nSize, bool bLocal, void *pOutLevel ) = 0;
	virtual bool GetNetworkedVoiceLevel( uint64 nSteamID, void *pOutLevel ) = 0;

	virtual void AddVoiceListener( void *pListener ) = 0;
	virtual void RemoveVoiceListener( void *pListener ) = 0;

	virtual bool IsVoiceRecording() = 0;

	// Named by what they do: copy a 40 byte voice recording override (format and flags) into the service and
	// raise its flag, and clear the flag again
	virtual void SetVoiceRecordingOverride( const void *pOverride ) = 0;
	virtual void ClearVoiceRecordingOverride() = 0;

	virtual void SetVoiceThreshold( float flThresholdDB ) = 0;
	virtual void SetVoiceThresholdDelay( float flTime ) = 0;
	virtual void SetVoiceThresholdAttack( float flTime ) = 0;
	virtual void SetVoiceThresholdHold( float flTime ) = 0;
	virtual float GetVoiceCurrentPeak() = 0;
	virtual bool IsVoiceActive() = 0;

	virtual void StopVoiceRecording() = 0;
};

#endif // ISOUNDSERVICE_H
