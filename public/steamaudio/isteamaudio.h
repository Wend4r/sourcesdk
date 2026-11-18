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

class ISteamAudioScene;
class ISteamAudioSource;
class ISteamAudioSimulationData;
class CSteamAudioMaterialList;

// Steam Audio wrapper (steamaudio.dll, CSteamAudio). Named by the Phonon calls and log strings of each
// slot; see re-workspace/vtables/data/proposals/sound_sdk.md.
abstract_class ISteamAudio : public IAppSystem
{
public:
	virtual ~ISteamAudio() {}
	// iplCUDADeviceCreate / iplOptiXDeviceCreate
	virtual bool CreateGPUDevice() = 0;
	// Creates an empty CSteamAudioScene
	virtual ISteamAudioScene *CreateEmptyScene() = 0;
	// iplSceneCreate / iplStaticMeshCreate ("Unable to create scene: %d.")
	virtual ISteamAudioScene *CreateScene( const void *pSceneData, bool bUseGPU ) = 0;
	// First scene of the active scene list, NULL when it is empty
	virtual ISteamAudioScene *GetActiveScene() = 0;
	virtual void PushActiveScene( void *pOwner, ISteamAudioScene *pScene ) = 0;
	virtual void RemoveActiveScene( ISteamAudioScene *pScene ) = 0;
	virtual void DestroyScene( ISteamAudioScene *pScene ) = 0;
	virtual void CommitActiveScenes( void *pContext ) = 0;
	// Probe neighbourhood and reverb simulation for the listener ("reverb_min_absorptive_gain")
	virtual void UpdateSimulation( const void *pListener ) = 0;
	// iplBinauralEffectCreate
	virtual void *CreateBinauralProcessor( void *pParams ) = 0;
	virtual void DestroyBinauralProcessor( void *pProcessor ) = 0;
	// iplPathEffectCreate
	virtual void *CreatePathingProcessor( void *pParams ) = 0;
	virtual void DestroyPathingProcessor( void *pProcessor ) = 0;
	// iplDirectEffectCreate
	virtual void *CreateDirectProcessor( void *pParams ) = 0;
	virtual void DestroyDirectProcessor( void *pProcessor ) = 0;
	// iplHybridReverbEffectCreate / iplAmbisonicsDecodeEffectCreate
	virtual void *CreateHybridReverbProcessor( int nChannels ) = 0;
	// iplSourceCreate / iplSourceAdd; returns the source index
	virtual int CreateSimulationSource() = 0;
	virtual void DestroySimulationSource( uint32 nSource ) = 0;
	// CSteamAudioSource::CSteamAudioSource (energy field, impulse response)
	virtual ISteamAudioSource *CreateSource() = 0;
	virtual void DestroySource( ISteamAudioSource *pSource ) = 0;
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
	virtual void ComputeSceneBounds( void *pBounds, void *pWorld ) = 0;
	// ResourceCompiler/SteamAudio/ReverbDefaults/...
	virtual void GetReverbBakeDefaults( void *pWorld, uint32 *pGridType, bool *pFilterUsingVolumes, bool *pFilterUsingNavMesh, int *pGridSpacing, void *pHeightAboveFloor, void *pRayCount, bool *pBakeConvolution, uint32 *pAmbisonicsOrder ) = 0;
	// ResourceCompiler/SteamAudio/PathingDefaults/...
	virtual void GetPathingBakeDefaults( void *pWorld, uint32 *pGridType, bool *pFilterUsingVolumes, bool *pFilterUsingNavMesh, int *pGridSpacing, int *pHeightAboveFloor, uint32 *pVisibilitySamples ) = 0;
	// ResourceCompiler/SteamAudio/CustomDataDefaults/...
	virtual void GetCustomDataBakeDefaults( void *pWorld, uint32 *pGridType, bool *pFilterUsingVolumes, bool *pFilterUsingNavMesh, int *pGridSpacing, void *pHeightAboveFloor, uint32 *pDataType ) = 0;
	// From the CPU information and the command line
	virtual int GetSimulationThreadCount() = 0;
	// iplSimulatorRunPathingPerSourceForNeighborhood
	virtual void RunPathingSimulation( void *pSource, void *pListener, float *pPathingGains, float *pPathingEQ, uint32 nFlags ) = 0;
	// Occlusion against the world and the hitboxes of entities ("invalid_hitbox", "invalid_bone")
	virtual float ComputeOcclusion( const float *pSource, const float *pListener, const float *pRadius, float flRadius, int nRays ) = 0;
	// iplSimulatorRunDirect / iplSourceGetOutputs
	virtual void RunDirectSimulation( void *pOutputs, void *pSource, void *pListener ) = 0;
	virtual void *GetSimulationSettings() = 0;
	// "Unable to create impulse response [%d]"
	virtual void *CreateImpulseResponse() = 0;
	virtual void SetListenerPosition( const void *pPosition ) = 0;
	virtual ISteamAudioSimulationData *CreateSimulationData() = 0;
	virtual void DestroySimulationData( ISteamAudioSimulationData *pData ) = 0;
	virtual bool IsSteamAudioEnabled() = 0;
	// CSteamAudio::BakeReverb
	virtual void BakeReverb( void *pParams ) = 0;
	// CSteamAudio::BakePathing
	virtual void BakePathing( void *pParams ) = 0;
	// CSteamAudio::BakeDimensions
	virtual void BakeDimensions( void *pParams ) = 0;
	virtual void LockSimulation() = 0;
	virtual void UnlockSimulation() = 0;
	virtual void SetSourceData( int nSource, const void *pData ) = 0;
	// Mean of the 11 bands of pBands
	virtual void GetAverageBandValue( const float *pBands, float *pValue ) = 0;
	// Copy 3 (or 4) values out of an N-band array through the band index tables of the system
	virtual void MapBandsToEQ3( const float *pBands, float *pOut ) = 0;
	virtual void MapBandsToEQ4( const float *pBands, float *pOut ) = 0;
	virtual void MapBandsToAbsorption( const float *pBands, float *pOut ) = 0;
	virtual void MapBandsToTransmission( const float *pBands, float *pOut ) = 0;
	virtual void MapBandsToScattering( const float *pBands, float *pOut ) = 0;
	virtual int GetBandRemapIndex( int nIndex ) = 0;
	virtual int GetBandSourceIndex( int nIndex ) = 0;
	virtual CSteamAudioMaterialList *CreateMaterialList() = 0;
	virtual void ReleaseMaterialList( CSteamAudioMaterialList *pList ) = 0;
	virtual void SetBakeProgress( int nStep, int nSteps, float flProgress, void *pContext ) = 0;
	virtual bool IsBakeStepActive( int nStep ) = 0;
};

#endif // ISTEAMAUDIO_H
