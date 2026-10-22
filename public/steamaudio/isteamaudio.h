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
#include "mathlib/vector.h"
#include "mathlib/vmatrix.h"
#include "tier0/threadtools.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlvector.h"
#include "tier0/utlstring.h"
#include "tier2/tier2.h"

class CTransform;
class CUtlBuffer;
class IDirectProcessor;
class IHybridReverbProcessor;
class IImpulseResponse;
class IPathingProcessor;
class ISourceProcessor;
class ISteamAudioScene;
class ISteamAudioSimulationData;
class ISteamAudioSource;
class KeyValues3;

//-----------------------------------------------------------------------------
// Bake settings (schema structs of the steam audio module)
//-----------------------------------------------------------------------------
struct SteamAudioReverbSettings_t
{
	int32 m_nNumRays;
	int32 m_nNumBounces;
	float32 m_flIRDuration;
	int32 m_nAmbisonicsOrder;
	bool m_bExportScene;
};

struct SteamAudioReverbClusteringSettings_t
{
	bool m_bEnableClustering;
	int32 m_nCubeMapResolution;
	float32 m_flDepthThreshold;
};

struct SteamAudioReverbCompressionSettings_t
{
	bool m_bEnableCompression;
	float32 m_flQuality;
};

struct SteamAudioPathSettings_t
{
	int32 m_nNumVisSamples;
	float32 m_flProbeVisRadius;
	float32 m_flProbeVisThreshold;
	float32 m_flProbePathRange;
};

struct SteamAudioCustomDataOcclusionSettings_t
{
	bool m_bEnablePathing;
	bool m_bEnableReflections;
	int32 m_nReflectionRays;
	int32 m_nReflectionBounces;
};

struct SteamAudioCustomDataDimensionsSettings_t
{
	int32 m_nAmbisonicsOrderOutsideField;
	int32 m_nAmbisonicsOrderInsideSizeField;
	float32 m_flOutsideThreshold;
	float32 m_flSizeThreshold;
	float32 m_flInsideThreshold;
};

// Custom data bake defaults ("ResourceCompiler/SteamAudio/CustomDataDefaults/...")
struct SteamAudioCustomDataSettings_t
{
	bool m_bBakeOcclusion;
	bool m_bBakeDimensions;
	bool m_bBakeMaterials;
	SteamAudioCustomDataOcclusionSettings_t m_occlusionSettings;
	SteamAudioCustomDataDimensionsSettings_t m_dimensionsSettings;
};

//-----------------------------------------------------------------------------
// Values returned by ISteamAudio
//-----------------------------------------------------------------------------

// Box of the object passed to ISteamAudio::unk038
struct SteamAudioUnk038_t
{
	float m_flUnk00[ 4 ];
	Vector m_vecCenter;
	Vector m_vecSize;
};

// Pathing result of ISteamAudio::SimulatePathing; the coefficient arrays belong to CSteamAudio
struct SteamAudioPathingResult_t
{
	int m_nEQBandCount;
	float *m_pEQCoeffs;
	int m_nSHCoeffCount;
	float *m_pSHCoeffs;
	float m_flUnk20;
	// Converted back from Steam Audio coordinates
	Vector m_vecDirection;
};

// Direct simulation result of ISteamAudio::SimulateDirect, from the IPLDirectEffectParams of the source
struct SteamAudioDirectParams_t
{
	float m_flDistanceAttenuation;
	float m_flAirAbsorption[ 3 ];
	float m_flUnk10;
	float m_flOcclusion;
	float m_flTransmission[ 3 ];
	float m_flDirectivity;
};

struct IPLPerspectiveCorrection;

// Draws one segment of a simulated sound path
typedef void ( *SteamAudioPathVisualizationFn_t )( const Vector &vecFrom, const Vector &vecTo, bool bOccluded );

// Reports the progress of a bake
typedef void ( *SteamAudioProgressFn_t )( float flProgress, void *pUserData );

abstract_class ISteamAudio : public IAppSystem
{
public:
	virtual ~ISteamAudio() {}
	// Creates the CUDA and OptiX devices
	virtual bool CreateCUDADevices() = 0;
	virtual ISteamAudioScene *CreateEmptyScene() = 0;
	// Creates a CSteamAudioScene
	virtual ISteamAudioScene *CreateScene( const void *pUnk1, bool bUnk, void *pUnk2 ) = 0;
	virtual ISteamAudioScene *GetFirstScene() = 0;
	virtual void AddScene( void *pKey, ISteamAudioScene *pScene ) = 0;
	virtual ISteamAudioScene *RemoveScene( void *pKey ) = 0;
	virtual void DestroyScene( ISteamAudioScene *pScene ) = 0;
	virtual void RemoveAndDestroyScene( void *pKey ) = 0;
	virtual void SetListenerOrigin( const Vector &vecListenerOrigin ) = 0;
	virtual ISourceProcessor *CreateSourceProcessor( const Vector &vecUnk ) = 0;
	virtual void DestroySourceProcessor( ISourceProcessor *pProcessor ) = 0;
	virtual IPathingProcessor *CreatePathingProcessor( const Vector &vecUnk ) = 0;
	virtual void DestroyPathingProcessor( IPathingProcessor *pProcessor ) = 0;
	virtual IDirectProcessor *CreateDirectProcessor( const int32 *pNumChannels ) = 0;
	virtual void DestroyDirectProcessor( IDirectProcessor *pProcessor ) = 0;
	virtual IHybridReverbProcessor *CreateHybridReverbProcessor( int nUnk ) = 0;
	// Returns -1 on failure
	virtual int AddSource() = 0;
	virtual void RemoveSource( int nSource ) = 0;
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
	virtual SteamAudioUnk038_t unk038( void *pUnk ) = 0;
	// pflGridSpacingAndHeight receives GridSpacing and HeightAboveFloor
	virtual void GetReverbBakeDefaults( const KeyValues3 *pKV, int32 *pGridGenerationType, bool *pbFilterUsingVolumes, bool *pbFilterUsingNavMesh, float *pflGridSpacingAndHeight, SteamAudioReverbSettings_t *pReverbSettings, SteamAudioReverbClusteringSettings_t *pClusteringSettings, SteamAudioReverbCompressionSettings_t *pCompressionSettings, int32 *pRebakeOption ) = 0;
	virtual void GetPathingBakeDefaults( const KeyValues3 *pKV, int32 *pGridGenerationType, bool *pbFilterUsingVolumes, bool *pbFilterUsingNavMesh, float *pflGridSpacingAndHeight, SteamAudioPathSettings_t *pPathSettings, int32 *pRebakeOption ) = 0;
	virtual void GetCustomDataBakeDefaults( const KeyValues3 *pKV, int32 *pGridGenerationType, bool *pbFilterUsingVolumes, bool *pbFilterUsingNavMesh, float *pflGridSpacingAndHeight, SteamAudioCustomDataSettings_t *pCustomDataSettings, int32 *pRebakeOption ) = 0;
	// Number of simulation threads from the CPU information
	virtual int GetSimulationThreadCount() = 0;
	// Runs iplSimulatorRunPathingPerSource for the source; bUseEQ takes the simulated EQ instead of a flat one, bNormalize
	// scales the SH coefficients by the first one, pbPathFound is set when the first SH coefficient is positive
	virtual SteamAudioPathingResult_t SimulatePathing( const Vector &vecUnk1, const Vector &vecUnk2, bool bUnk1, bool bUseEQ, bool bNormalize, int nSource, SteamAudioPathVisualizationFn_t pfnVisualize, bool *pbPathFound ) = 0;
	// Fraction of nSamples traces between points spread over a disc of flRadius around both ends that are not blocked
	virtual float ComputeOcclusion( const Vector &vecStart, const Vector &vecEnd, float flRadius, int nSamples ) = 0;
	// Runs iplSimulatorRunDirect for one source
	virtual SteamAudioDirectParams_t SimulateDirect( const Vector &vecSource, const Vector &vecListener, const void *pParams ) = 0;
	// Perspective correction SetListenerOrigin builds from the view (snd_steamaudio_perspective_correction_factor)
	virtual const IPLPerspectiveCorrection *GetPerspectiveCorrection() = 0;
	virtual IImpulseResponse *CreateImpulseResponse() = 0;
	virtual void unk048( const Vector &vecUnk ) = 0;
	virtual ISteamAudioSimulationData *CreateSimulationData() = 0;
	virtual void DestroySimulationData( ISteamAudioSimulationData *pData ) = 0;
	// Value of snd_steamaudio_enable_editor_profiling
	virtual bool IsEditorProfilingEnabled() = 0;
	// pBakeData is a CUtlVector of probe batches; the baked data is saved as KV3 into pBuffer
	virtual bool BakeReverb( const void *pBakeData, CUtlBuffer *pBuffer, SteamAudioProgressFn_t pfnProgress, void *pUserData ) = 0;
	virtual bool BakePathing( const void *pBakeData, CUtlBuffer *pBuffer, SteamAudioProgressFn_t pfnProgress, void *pUserData ) = 0;
	virtual bool BakeDimensions( const void *pBakeData, CUtlBuffer *pBuffer, SteamAudioProgressFn_t pfnProgress, void *pUserData ) = 0;
	// Lock and unlock the mutex of the transform map that SetTransform fills
	virtual void LockTransforms() = 0;
	virtual void UnlockTransforms() = 0;
	// Stores the transform as a matrix (position w is the scale) under the index
	virtual void SetTransform( int nIndex, const CTransform &transform ) = 0;
	// Average of the 11 values
	virtual void unk058( const float *pflValues, float *pflAverage ) = 0;
	virtual void unk059( const float *pflIn, float *pflOut ) = 0;
	virtual void unk060( const float *pflIn, float *pflOut ) = 0;
	virtual void unk061( const float *pflIn, float *pflOut ) = 0;
	virtual void unk062( const Vector &vecIn, Vector &vecOut ) = 0;
	virtual void unk063( const Vector &vecIn, Vector &vecOut ) = 0;
	virtual int unk064( int nIndex ) = 0;
	virtual int unk065( int nIndex ) = 0;
	virtual void *unk066() = 0;
	virtual void unk067( void *pUnk ) = 0;
	// snd_steamaudio_display_probes: nProbeType 0 off, 1 reverb, 2 pathing, 3 occlusion, 4 dimensions, 5 materials;
	// nMode 0 default, 1 always, 2 once; pDrawFn draws each probe within flRadius
	virtual void SetProbeDisplay( int nProbeType, int nMode, float flRadius, void *pDrawFn ) = 0;
	// True when the probes of the type are displayed with a non-zero mode
	virtual bool IsDisplayingProbes( int nProbeType ) = 0;
};

//-----------------------------------------------------------------------------
// Steam Audio (Phonon) types used by CSteamAudio
//-----------------------------------------------------------------------------
typedef struct _IPLContext_t *IPLContext;
typedef struct _IPLHRTF_t *IPLHRTF;
typedef struct _IPLEmbreeDevice_t *IPLEmbreeDevice;
typedef struct _IPLOpenCLDevice_t *IPLOpenCLDevice;
typedef struct _IPLRadeonRaysDevice_t *IPLRadeonRaysDevice;
typedef struct _IPLCUDADevice_t *IPLCUDADevice;
typedef struct _IPLOptiXDevice_t *IPLOptiXDevice;
typedef struct _IPLTrueAudioNextDevice_t *IPLTrueAudioNextDevice;
typedef struct _IPLSimulator_t *IPLSimulator;
typedef struct _IPLSource_t *IPLSource;
typedef struct _IPLProbeNeighborhood_t *IPLProbeNeighborhood;

struct IPLAudioSettings
{
	int32 samplingRate;
	int32 frameSize;
};

struct IPLSimulationSettings
{
	int32 flags;
	int32 sceneType;
	int32 reflectionType;
	int32 maxNumOcclusionSamples;
	int32 maxNumRays;
	int32 numDiffuseSamples;
	float32 maxDuration;
	int32 maxOrder;
	int32 maxNumSources;
	int32 numThreads;
	int32 rayBatchSize;
	int32 numVisSamples;
	int32 samplingRate;
	int32 frameSize;
	IPLOpenCLDevice openCLDevice;
	IPLRadeonRaysDevice radeonRaysDevice;
	IPLTrueAudioNextDevice tanDevice;
	float32 m_flUnk50;
	int32 m_nUnk54;
	uint64 m_nUnk58;
	uint64 m_nUnk60;
};

struct IPLPerspectiveCorrection
{
	bool enabled;
	float32 xfactor;
	float32 yfactor;
	VMatrix transform;
};

class CSteamAudioPartitioner;
class CSteamAudioReverbSimulator;

struct SteamAudioSceneEntry_t
{
	ISteamAudioScene *m_pScene;
	void *m_pKey;
};

class CSteamAudio : public CTier2AppSystem< ISteamAudio >
{
public:
	IPLAudioSettings m_AudioSettings;
	IPLContext m_Context;
	CUtlVector< IPLHRTF > m_HRTFs;
	int m_nUnk050;
	IPLEmbreeDevice m_EmbreeDevice;
	IPLOpenCLDevice m_OpenCLDevice;
	IPLRadeonRaysDevice m_RadeonRaysDevice;
	IPLCUDADevice m_CUDADevice;
	IPLOptiXDevice m_OptiXDevice;

	// Base simulation settings, also passed to the reverb and source objects
	IPLSimulationSettings m_SimulationSettings;
	ISteamAudioScene *m_pUnk0E8;
	IPLSource m_Unk0F0;
	IPLSimulationSettings m_Unk0F8;
	IPLSimulator m_Simulator;
	IPLProbeNeighborhood m_ProbeNeighborhood;
	ISteamAudioScene *m_pUnk170;
	int m_nUnk178;
	int m_nUnk17C;
	int m_nUnk180;

	IPLPerspectiveCorrection m_PerspectiveCorrection;

	// Source registry: next id, free ids, id to IPLSource hash map (container not identified)
	int m_nNextSourceId;
	CUtlLeanVector< int > m_FreeSourceIds;
	uint64 m_Unk1E8[ 11 ];
	CThreadFastMutex m_UnkMutex240;
	CThreadFastMutex m_UnkMutex24C;

	// Pathing output of SimulatePathing and the tables the constructor fills with constants (layout not resolved)
	int m_nUnk258;
	int m_nUnk25C;
	uint64 m_Unk260[ 81 ];

	// Index tables of unk059..unk065
	int m_nUnk4E8[ 3 ];
	int m_nUnk4F4[ 3 ];
	int m_nUnk500[ 3 ];
	int m_nUnk50C[ 11 ];
	int m_nUnk538[ 3 ];

	// Scenes registered by AddScene
	CUtlVectorFixedGrowable< SteamAudioSceneEntry_t, 4 > m_Scenes;

	// Listener and HRTF state written by SetListenerOrigin (layout not resolved)
	uint64 m_Unk5A0[ 132 ];

	CSteamAudioReverbSimulator *m_pReverbSimulator;
	CSteamAudioPartitioner *m_pPartitioner;
	bool m_bUnk9D0;

	// Hash map of SetTransform transforms (container not identified) and the mutex LockTransforms takes
	uint64 m_TransformMap[ 10 ];
	CThreadFastMutex m_TransformMapMutex;

	// Probe display of SetProbeDisplay; the count of probe updates lets the "once" mode draw a single update
	int m_nProbeDisplayType;
	int m_nProbeDisplayMode;
	float m_flProbeDisplayRadius;
	void *m_pProbeDrawFn;
	int m_nProbeDisplayUpdateCount;
	Vector m_vecUnkA4C;
	Vector m_vecUnkA58;
	uint8 m_UnkA64[ 0x28 ];
	CUtlVector< void * > m_UnkA90;
	int m_nUnkAA8;
};

#endif // ISTEAMAUDIO_H
