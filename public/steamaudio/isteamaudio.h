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
#include "mathlib/camera.h"
#include "mathlib/vector.h"
#include "mathlib/vmatrix.h"
#include "tier0/threadtools.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlpriorityqueue.h"
#include "tier0/utlvector.h"
#include "tier0/utlstring.h"
#include "tier1/utlhashmaplarge.h"
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

// Box (center and size) ISteamAudio::GetBoundsBox builds from the bounds of an object
struct SteamAudioBox_t
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
	// 1.0 for a source closer than a unit, else the path ratio of the source outputs (or the direct distance over
	// snd_steamaudio_invalid_path_length), at most 1.0
	float m_flPathDistanceRatio;
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

// Probes snd_steamaudio_display_probes shows ("reverb", "pathing", "occlusion", "dimensions", "materials", "off")
enum SteamAudioProbeDisplayType_t
{
	STEAMAUDIO_PROBE_DISPLAY_NONE = 0,
	STEAMAUDIO_PROBE_DISPLAY_REVERB,
	STEAMAUDIO_PROBE_DISPLAY_PATHING,
	STEAMAUDIO_PROBE_DISPLAY_OCCLUSION,
	STEAMAUDIO_PROBE_DISPLAY_DIMENSIONS,
	STEAMAUDIO_PROBE_DISPLAY_MATERIALS,
};

// Update mode of the probe display ("always", "once", "off" or none)
enum SteamAudioProbeDisplayMode_t
{
	// The probes are drawn without the listener data of the dimensions and occlusion displays
	STEAMAUDIO_PROBE_DISPLAY_MODE_OFF = 0,
	// The listener data is updated on every probe update
	STEAMAUDIO_PROBE_DISPLAY_MODE_ALWAYS,
	// The listener data is taken on the first probe update only
	STEAMAUDIO_PROBE_DISPLAY_MODE_ONCE,
};

// Reports the progress of a bake
typedef void ( *SteamAudioProgressFn_t )( float flProgress, void *pUserData );

abstract_class ISteamAudio : public IAppSystem
{
public:
	virtual ~ISteamAudio() {}
	// Creates the CUDA and OptiX devices
	virtual bool CreateCUDADevices() = 0;
	virtual ISteamAudioScene *CreateEmptyScene() = 0;
	// Creates a CSteamAudioScene from the geometry; bCreateGPUScene also builds the scene for the GPU device, and
	// pReplacementMaterial replaces every material with a value below 1.0
	virtual ISteamAudioScene *CreateScene( const void *pGeometry, bool bCreateGPUScene, void *pReplacementMaterial ) = 0;
	virtual ISteamAudioScene *GetFirstScene() = 0;
	virtual void AddScene( void *pKey, ISteamAudioScene *pScene ) = 0;
	virtual ISteamAudioScene *RemoveScene( void *pKey ) = 0;
	virtual void DestroyScene( ISteamAudioScene *pScene ) = 0;
	virtual void RemoveAndDestroyScene( void *pKey ) = 0;
	virtual void SetListenerOrigin( const Vector &vecListenerOrigin ) = 0;
	// vecDirection is the direction the binaural effect of the processor renders
	virtual ISourceProcessor *CreateSourceProcessor( const Vector &vecDirection ) = 0;
	virtual void DestroySourceProcessor( ISourceProcessor *pProcessor ) = 0;
	virtual IPathingProcessor *CreatePathingProcessor( const Vector &vecUnused ) = 0;
	virtual void DestroyPathingProcessor( IPathingProcessor *pProcessor ) = 0;
	virtual IDirectProcessor *CreateDirectProcessor( const int32 *pNumChannels ) = 0;
	virtual void DestroyDirectProcessor( IDirectProcessor *pProcessor ) = 0;
	// nChannels selects the speaker layout of the effect: 1, 2, 4, 6 or 8
	virtual IHybridReverbProcessor *CreateHybridReverbProcessor( int nChannels ) = 0;
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
	// pObject gives the bounds through its virtual slot 154
	virtual SteamAudioBox_t GetBoundsBox( void *pObject ) = 0;
	// pflGridSpacingAndHeight receives GridSpacing and HeightAboveFloor
	virtual void GetReverbBakeDefaults( const KeyValues3 *pKV, int32 *pGridGenerationType, bool *pbFilterUsingVolumes, bool *pbFilterUsingNavMesh, float *pflGridSpacingAndHeight, SteamAudioReverbSettings_t *pReverbSettings, SteamAudioReverbClusteringSettings_t *pClusteringSettings, SteamAudioReverbCompressionSettings_t *pCompressionSettings, int32 *pRebakeOption ) = 0;
	virtual void GetPathingBakeDefaults( const KeyValues3 *pKV, int32 *pGridGenerationType, bool *pbFilterUsingVolumes, bool *pbFilterUsingNavMesh, float *pflGridSpacingAndHeight, SteamAudioPathSettings_t *pPathSettings, int32 *pRebakeOption ) = 0;
	virtual void GetCustomDataBakeDefaults( const KeyValues3 *pKV, int32 *pGridGenerationType, bool *pbFilterUsingVolumes, bool *pbFilterUsingNavMesh, float *pflGridSpacingAndHeight, SteamAudioCustomDataSettings_t *pCustomDataSettings, int32 *pRebakeOption ) = 0;
	// Number of simulation threads from the CPU information
	virtual int GetSimulationThreadCount() = 0;
	// Runs iplSimulatorRunPathingPerSource for the source; bFindAlternatePaths sets both enableValidation and
	// findAlternatePaths of the source inputs, bUseEQ takes the simulated EQ instead of a flat one, bNormalize
	// scales the SH coefficients by the first one, pbPathFound is set when the first SH coefficient is positive
	virtual SteamAudioPathingResult_t SimulatePathing( const Vector &vecSource, const Vector &vecListener, bool bFindAlternatePaths, bool bUseEQ, bool bNormalize, int nSource, SteamAudioPathVisualizationFn_t pfnVisualize, bool *pbPathFound ) = 0;
	// Fraction of nSamples traces between points spread over a disc of flRadius around both ends that are not blocked
	virtual float ComputeOcclusion( const Vector &vecStart, const Vector &vecEnd, float flRadius, int nSamples ) = 0;
	// Runs iplSimulatorRunDirect for one source
	virtual SteamAudioDirectParams_t SimulateDirect( const Vector &vecSource, const Vector &vecListener, const void *pParams ) = 0;
	// Perspective correction SetListenerOrigin builds from the view (snd_steamaudio_perspective_correction_factor)
	virtual const IPLPerspectiveCorrection *GetPerspectiveCorrection() = 0;
	virtual IImpulseResponse *CreateImpulseResponse() = 0;
	// Origin of the listener coordinate space of the reverb simulator
	virtual void SetReverbListenerOrigin( const Vector &vecOrigin ) = 0;
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
	// Band conversions through the band tables of CSteamAudio, between 11 bands and the 3 Steam Audio bands; the input
	// conversions feed the EQ of the Steam Audio processors, the output conversions map the simulation results back
	// Average of the 11 bands
	virtual void AverageBands( const float *pflBands, float *pflAverage ) = 0;
	// 3 of the 11 bands, one per Steam Audio band
	virtual void SampleInputBands( const float *pflBands, float *pflSteamAudioBands ) = 0;
	// Each of the 11 bands takes the value of its Steam Audio band
	virtual void ExpandInputBands( const float *pflSteamAudioBands, float *pflBands ) = 0;
	virtual void SampleOutputBands( const float *pflBands, float *pflSteamAudioBands ) = 0;
	virtual void ConvertInputEQ( const Vector &vecEQ, Vector &vecSteamAudioEQ ) = 0;
	virtual void ConvertOutputEQ( const Vector &vecSteamAudioEQ, Vector &vecEQ ) = 0;
	virtual int GetInputEQBand( int nBand ) = 0;
	virtual int GetOutputEQBand( int nBand ) = 0;
	virtual void *unk066() = 0;
	virtual void unk067( void *pUnk ) = 0;
	// snd_steamaudio_display_probes; pDrawFn draws each probe within flRadius
	virtual void SetProbeDisplay( SteamAudioProbeDisplayType_t nProbeType, SteamAudioProbeDisplayMode_t nMode, float flRadius, void *pDrawFn ) = 0;
	// True when the probes of the type are displayed with a mode other than STEAMAUDIO_PROBE_DISPLAY_MODE_OFF
	virtual bool IsDisplayingProbes( SteamAudioProbeDisplayType_t nProbeType ) = 0;
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

// Steam Audio API enumerations
enum IPLSimulationFlags
{
	IPL_SIMULATIONFLAGS_DIRECT = ( 1 << 0 ),
	IPL_SIMULATIONFLAGS_REFLECTIONS = ( 1 << 1 ),
	IPL_SIMULATIONFLAGS_PATHING = ( 1 << 2 ),
};

enum IPLSceneType
{
	IPL_SCENETYPE_DEFAULT,
	IPL_SCENETYPE_EMBREE,
	IPL_SCENETYPE_RADEONRAYS,
	IPL_SCENETYPE_CUSTOM,
};

enum IPLReflectionEffectType
{
	IPL_REFLECTIONEFFECTTYPE_CONVOLUTION,
	IPL_REFLECTIONEFFECTTYPE_PARAMETRIC,
	IPL_REFLECTIONEFFECTTYPE_HYBRID,
	IPL_REFLECTIONEFFECTTYPE_TAN,
};

struct IPLSimulationSettings
{
	IPLSimulationFlags flags;
	IPLSceneType sceneType;
	IPLReflectionEffectType reflectionType;
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
	// Members of the libphonon build of the game, past the public Steam Audio settings; libphonon passes them to the
	// reflection simulator together with the devices
	// Seconds per energy field bin (0.01 by default), the bin count is maxDuration / binDuration
	float32 binDuration;
	// Above 1 the energy field also accumulates the energy by its offset within a bin, in this many steps (1 by default)
	int32 numSubBins;
	IPLCUDADevice cudaDevice;
	IPLOptiXDevice optixDevice;
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

// What the dimensions probe display draws, computed for the listener on a display update
struct SteamAudioDimensionsDisplay_t
{
	Vector m_vecListenerOrigin;
	Vector m_vecListenerForward;
	float m_flInOut;
	float m_flSize;
	float m_flOutsideDistance;
	Vector m_vecOutsideDirection;
	float m_flInsideDistance;
	Vector m_vecInsideDirection;
};

// Entry the occlusion probe display draws
struct SteamAudioOcclusionDisplayProbe_t
{
	Vector m_vecPosition;
	// Radius of the probe in inches, not drawn by the display
	float m_flRadius;
	float m_flOcclusion;
	// The display merges entries by it
	int m_nId;
};

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
	// Index into m_HRTFs SetListenerOrigin takes from its convar, 0 when out of range or not loaded
	int m_nHRTFIndex;
	IPLEmbreeDevice m_EmbreeDevice;
	IPLOpenCLDevice m_OpenCLDevice;
	IPLRadeonRaysDevice m_RadeonRaysDevice;
	IPLCUDADevice m_CUDADevice;
	IPLOptiXDevice m_OptiXDevice;

	// Base simulation settings (IPL_SIMULATIONFLAGS_REFLECTIONS, IPL_SCENETYPE_CUSTOM, IPL_REFLECTIONEFFECTTYPE_HYBRID), also
	// passed to the reverb and source objects
	IPLSimulationSettings m_SimulationSettings;
	// Empty scene the reverb simulator works on
	ISteamAudioScene *m_pReverbScene;
	// Source of SimulateDirect, created with the direct flag and added to m_Simulator in Init
	IPLSource m_DirectSource;
	// m_SimulationSettings with IPL_SIMULATIONFLAGS_DIRECT | IPL_SIMULATIONFLAGS_PATHING, m_Simulator is created from them
	IPLSimulationSettings m_SimulatorSettings;
	IPLSimulator m_Simulator;
	IPLProbeNeighborhood m_ProbeNeighborhood;
	// Scene of m_Simulator, also used for the probe occlusion checks of SetListenerOrigin
	ISteamAudioScene *m_pSimulationScene;
	// Id Shutdown removes from m_Sources; only the constructor writes it (-1)
	int m_nSourceId;
	int m_nUnk17C;
	// SimulatePathing and the probe neighborhood update of SetListenerOrigin run only while it is positive; only the
	// constructor writes it (0)
	int m_nPathingEnableCount;

	IPLPerspectiveCorrection m_PerspectiveCorrection;

	// Source registry of AddSource and RemoveSource: the lowest free id is reused before m_nNextSourceId grows
	int m_nNextSourceId;
	CUtlPriorityQueue< int > m_FreeSourceIds;
	CUtlHashMap< int, IPLSource, CDefEquals< int >, MurmurHash3Functor< int > > m_Sources;
	CThreadFastMutex m_FreeSourceIdsMutex;
	CThreadFastMutex m_SourcesMutex;

	int m_nUnk260;
	// Coefficients SimulatePathing returns, the counts are set by the constructor
	int m_nPathingSHCoeffCount;
	float m_flPathingEQCoeffs[ 3 ];
	float m_flPathingSHCoeffs[ 16 ];
	uint8 m_pad2B4[ 0xC ];

	// Sample points of ComputeOcclusion on a unit disc (squared radius, angle as a fraction of a turn), constants of
	// the constructor; nSamples is clamped to the count
	int m_nOcclusionSampleCount;
	float m_flOcclusionSampleRadiusSq[ 64 ];
	float m_flOcclusionSampleAngle[ 64 ];

	// Band tables of the band conversions, constants of the constructor: the band maps give the Steam Audio band of
	// each of the 11 bands, the sample tables the band each Steam Audio band takes, the EQ tables order the Steam Audio bands
	// Same values as m_nInputBandMap, never read
	int m_nUnk4C4[ 11 ];
	int m_nOutputEQBands[ 3 ];
	int m_nOutputBandSamples[ 3 ];
	int m_nInputEQBands[ 3 ];
	int m_nInputBandMap[ 11 ];
	int m_nInputBandSamples[ 3 ];

	// Scenes registered by AddScene
	CUtlVectorFixedGrowable< SteamAudioSceneEntry_t, 4 > m_Scenes;

	// View frustum SetListenerOrigin builds from the client view, its view projection is the perspective correction transform
	CFrustum m_ListenerFrustum;

	CSteamAudioReverbSimulator *m_pReverbSimulator;
	CSteamAudioPartitioner *m_pPartitioner;
	// SetListenerOrigin enables enable_iir_order_8 of the context once
	bool m_bIIROrder8Enabled;

	// Transforms of SetTransform by index and the mutex LockTransforms takes
	CUtlHashMap< int, matrix3x4_t, CDefEquals< int >, IdentityHashFunctor > m_TransformMap;
	CThreadFastMutex m_TransformMapMutex;

	// Probe display of SetProbeDisplay; the count of probe updates lets the "once" mode draw a single update
	SteamAudioProbeDisplayType_t m_nProbeDisplayType;
	SteamAudioProbeDisplayMode_t m_nProbeDisplayMode;
	float m_flProbeDisplayRadius;
	void *m_pProbeDrawFn;
	int m_nProbeDisplayUpdateCount;
	// Data passed to the draw function for the dimensions and occlusion displays
	SteamAudioDimensionsDisplay_t m_DimensionsDisplay;
	CUtlVector< SteamAudioOcclusionDisplayProbe_t > m_OcclusionDisplayProbes;
	int m_nPathingEQBandCount;
};

#endif // ISTEAMAUDIO_H
