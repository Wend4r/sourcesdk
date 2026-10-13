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
struct SteamAudioUnk041_t
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

// Pathing result of ISteamAudio::unk043
struct SteamAudioUnk043_t
{
	int m_nUnk00;
	Vector *m_pUnk08;
	int m_nUnk10;
	float *m_pUnk18;
	float m_flUnk20;
	float m_flUnk24;
	float m_flUnk28;
	float m_flUnk2C;
};

// Direct simulation result of ISteamAudio::unk045
struct SteamAudioUnk045_t
{
	float m_flUnk00[ 10 ];
};

// Draws one segment of a simulated sound path
typedef void ( *SteamAudioPathVisualizationFn_t )( const Vector &vecFrom, const Vector &vecTo, bool bOccluded );

// Reports the progress of a bake
typedef void ( *SteamAudioProgressFn_t )( float flProgress, void *pUserData );

abstract_class ISteamAudio : public IAppSystem
{
public:
	virtual ~ISteamAudio() {}
	// Creates the CUDA and OptiX devices
	virtual bool unk012() = 0;
	virtual ISteamAudioScene *unk013() = 0;
	// Creates a CSteamAudioScene
	virtual ISteamAudioScene *CreateScene( const void *pUnk1, bool bUnk, void *pUnk2 ) = 0;
	virtual ISteamAudioScene *unk015() = 0;
	virtual void unk016( void *pKey, ISteamAudioScene *pScene ) = 0;
	virtual ISteamAudioScene *unk017( void *pKey ) = 0;
	virtual void unk018( ISteamAudioScene *pScene ) = 0;
	virtual void unk019( void *pKey ) = 0;
	virtual void unk020( const Vector &vecListenerOrigin ) = 0;
	virtual ISourceProcessor *unk021( const Vector &vecUnk ) = 0;
	virtual void unk022( ISourceProcessor *pProcessor ) = 0;
	virtual IPathingProcessor *unk023( const Vector &vecUnk ) = 0;
	virtual void unk024( IPathingProcessor *pProcessor ) = 0;
	virtual IDirectProcessor *unk025( const int32 *pNumChannels ) = 0;
	virtual void unk026( IDirectProcessor *pProcessor ) = 0;
	virtual IHybridReverbProcessor *unk027( int nUnk ) = 0;
	// Returns -1 on failure
	virtual int unk028() = 0;
	virtual void unk029( int nSource ) = 0;
	virtual ISteamAudioSource *unk030() = 0;
	virtual void unk031( ISteamAudioSource *pSource ) = 0;
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
	virtual void GetCustomDataBakeDefaults( const KeyValues3 *pKV, int32 *pGridGenerationType, bool *pbFilterUsingVolumes, bool *pbFilterUsingNavMesh, float *pflGridSpacingAndHeight, SteamAudioUnk041_t *pCustomDataSettings, int32 *pRebakeOption ) = 0;
	// Number of simulation threads from the CPU information
	virtual int unk042() = 0;
	virtual SteamAudioUnk043_t unk043( const Vector &vecUnk1, const Vector &vecUnk2, bool bUnk1, bool bUnk2, bool bUnk3, int nSource, SteamAudioPathVisualizationFn_t pfnVisualize, bool *pbUnk ) = 0;
	virtual float unk044( const Vector &vecUnk1, const Vector &vecUnk2, float flUnk, int nUnk ) = 0;
	virtual SteamAudioUnk045_t unk045( const Vector &vecUnk1, const Vector &vecUnk2, const void *pUnk ) = 0;
	virtual void *unk046() = 0;
	virtual IImpulseResponse *unk047() = 0;
	virtual void unk048( const Vector &vecUnk ) = 0;
	virtual ISteamAudioSimulationData *unk049() = 0;
	virtual void unk050( ISteamAudioSimulationData *pData ) = 0;
	virtual bool unk051() = 0;
	// pBakeData is a CUtlVector of probe batches; the baked data is saved as KV3 into pBuffer
	virtual bool BakeReverb( const void *pBakeData, CUtlBuffer *pBuffer, SteamAudioProgressFn_t pfnProgress, void *pUserData ) = 0;
	virtual bool BakePathing( const void *pBakeData, CUtlBuffer *pBuffer, SteamAudioProgressFn_t pfnProgress, void *pUserData ) = 0;
	virtual bool BakeDimensions( const void *pBakeData, CUtlBuffer *pBuffer, SteamAudioProgressFn_t pfnProgress, void *pUserData ) = 0;
	// Lock and unlock the mutex of unk057
	virtual void unk055() = 0;
	virtual void unk056() = 0;
	virtual void unk057( int nUnk, const CTransform &transform ) = 0;
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
	virtual void unk068( int nUnk1, int nUnk2, float flUnk, void *pUnk ) = 0;
	virtual bool unk069( int nUnk ) = 0;
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

class CSteamAudioPartitioner;
class CSteamAudioReverbSimulator;

struct SteamAudioUnk548_t
{
	ISteamAudioScene *m_pScene;
	void *m_pKey;
};

class CSteamAudio : public CTier2AppSystem< ISteamAudio >
{
public:
	IPLAudioSettings m_Unk028;
	IPLContext m_Unk030;
	CUtlVector< IPLHRTF > m_Unk038;
	int m_nUnk050;
	IPLEmbreeDevice m_Unk058;
	IPLOpenCLDevice m_Unk060;
	IPLRadeonRaysDevice m_Unk068;
	IPLCUDADevice m_Unk070;
	IPLOptiXDevice m_Unk078;

	// Base simulation settings, also passed to the reverb and source objects
	IPLSimulationSettings m_Unk080;
	ISteamAudioScene *m_pUnk0E8;
	IPLSource m_Unk0F0;
	IPLSimulationSettings m_Unk0F8;
	IPLSimulator m_Unk160;
	IPLProbeNeighborhood m_Unk168;
	ISteamAudioScene *m_pUnk170;
	int m_nUnk178;
	int m_nUnk17C;
	int m_nUnk180;

	// Returned by unk046
	bool m_bUnk184;
	float m_flUnk188;
	float m_flUnk18C;
	VMatrix m_Unk190;

	// Source registry: next id, free ids, id to IPLSource hash map (container not identified)
	int m_nUnk1D0;
	CUtlLeanVector< int > m_Unk1D8;
	uint64 m_Unk1E8[ 11 ];
	CThreadFastMutex m_UnkMutex240;
	CThreadFastMutex m_UnkMutex24C;

	// Pathing output of unk043 and the tables the constructor fills with constants (layout not resolved)
	int m_nUnk258;
	int m_nUnk25C;
	uint64 m_Unk260[ 81 ];

	// Index tables of unk059..unk065
	int m_nUnk4E8[ 3 ];
	int m_nUnk4F4[ 3 ];
	int m_nUnk500[ 3 ];
	int m_nUnk50C[ 11 ];
	int m_nUnk538[ 3 ];

	// Scenes registered by unk016
	CUtlVectorFixedGrowable< SteamAudioUnk548_t, 4 > m_Unk548;

	// Listener and HRTF state written by unk020 (layout not resolved)
	uint64 m_Unk5A0[ 132 ];

	CSteamAudioReverbSimulator *m_pUnk9C0;
	CSteamAudioPartitioner *m_pUnk9C8;
	bool m_bUnk9D0;

	// Hash map of unk057 transforms (container not identified)
	uint64 m_Unk9D8[ 10 ];
	CThreadFastMutex m_UnkMutexA28;

	// Set by unk068
	int m_nUnkA34;
	int m_nUnkA38;
	float m_flUnkA3C;
	void *m_pUnkA40;
	int m_nUnkA48;
	Vector m_vecUnkA4C;
	Vector m_vecUnkA58;
	uint8 m_UnkA64[ 0x28 ];
	CUtlVector< void * > m_UnkA90;
	int m_nUnkAA8;
};

#endif // ISTEAMAUDIO_H
