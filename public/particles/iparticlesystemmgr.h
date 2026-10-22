//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Particle system manager interface
//
//===========================================================================//

#ifndef IPARTICLESYSTEMMGR_H
#define IPARTICLESYSTEMMGR_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>
#include <mathlib/vector.h>
#include <materialsystem2/imaterial2.h>
#include <resourcesystem/resourcehandle.h>
#include <resourcesystem/iresourcesystem.h>
#include <scenesystem/sceneobject.h>
#include <schemasystem/schematypes.h>
#include <tier0/generichash.h>
#include <tier0/utlhashtable.h>
#include <tier0/utlleanvector.h>
#include <tier0/utllinkedlist.h>
#include <tier0/utlsoacontainer.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>
#include <tier0/threadtools.h>
#include <tier3/tier3.h>

class CGpuImplicitRenderer;
class CInstancedMeshSceneObject;
class CPoolAllocator;
class CSchemaClassInfo;
class IParticleCollection;
class IParticleFunctionDefinition;
class IParticleSystemDefinition;
class IParticleSystemQuery;
class IRenderContext;
class ISceneObjectDesc;
class ISceneView;
class ISceneWorld;
class KeyValues3;

FORWARD_DECLARE_HANDLE( RenderBufferHandle_t );

class InfoForResourceTypeIParticleSystemDefinition
{
public:
	using RuntimeClass_t = IParticleSystemDefinition;
};

using HParticleSystemDefinition = CWeakHandle< InfoForResourceTypeIParticleSystemDefinition >;

//-----------------------------------------------------------------------------
// Arrays of particle attribute values that fill a snapshot, one element per
// particle for every non-null array; each array maps to the particle attribute
// its name gives
//-----------------------------------------------------------------------------
struct ParticleSnapshotAttributeData_t
{
	const Vector *m_pXYZ;
	const Vector *m_pPrevXYZ;
	const Vector *m_pNormal;
	const Vector *m_pTintRGB;
	const Vector *m_pBoxMins;
	const Vector *m_pBoxMaxs;
	const Vector *m_pBoxAngles;
	const Vector *m_pHitboxRelativeXYZ;
	const Vector *m_pScratchVec;
	const Vector *m_pScratchVec2;
	const float *m_pRadius;
	const float *m_pForceScale;
	const float *m_pAlpha;
	const float *m_pAlpha2;
	const float *m_pScratchFloat;
	const float *m_pScratchFloat1;
	const float *m_pScratchFloat2;
	const float *m_pCreationTime;
	const float *m_pLifeDuration;
	const float *m_pGlowAlpha;
	const float *m_pSequenceNumber;
	const int *m_pBoxFlags;
	const int *m_pHitboxIndex;
	const int *m_pRopeSegmentId;
};

//-----------------------------------------------------------------------------
// A particle snapshot: an attribute container indexed by particle (column),
// row and slice, with a mapping from particle attributes to container fields
//-----------------------------------------------------------------------------
abstract_class IParticleSnapshot
{
public:
	struct AttributeMap
	{
		int m_nContainerAttribute;
		int m_nParticleAttribute;
	};

	using AttributeMapVector = CUtlVector< AttributeMap >;

	virtual bool Init( int nX, int nY, int nZ, const AttributeMapVector &attributeMaps ) = 0;

	// Maps every non-null array of the data to its attribute and fills nCount particles
	virtual bool InitFromAttributeData( int nCount, const ParticleSnapshotAttributeData_t *pData ) = 0;

	// Resets the particle count
	virtual void Clear() = 0;

	// Appends up to nCount particles, pContainerAttributes gives the container field of each array of the data
	virtual int AddAttributeData( int nCount, const ParticleSnapshotAttributeData_t *pData, const int *pContainerAttributes ) = 0;

	// Returns the particle count, same as NumCols()
	virtual int unk004() const = 0;

	virtual int NumCols() const = 0;
	virtual int NumRows() const = 0;
	virtual int NumSlices() const = 0;
	virtual bool HasAttribute( int nParticleAttribute, EAttributeDataType nDataType ) const = 0;
	virtual float *ElementPointerf( int nParticleAttribute, int nX, int nY, int nZ ) const = 0;
	virtual FourVectors *ElementPointer4V( int nParticleAttribute, int nX, int nY, int nZ ) const = 0;
	virtual CUtlVector< CUtlString > &GetBoneNameList() = 0;

	// Incremented by every change of the snapshot
	virtual int GetSerialNumber() const = 0;

	virtual ~IParticleSnapshot() {}
};

class InfoForResourceTypeIParticleSnapshot
{
public:
	using RuntimeClass_t = IParticleSnapshot;
};

using HParticleSnapshot = CWeakHandle< InfoForResourceTypeIParticleSnapshot >;

abstract_class IParticleSystemMgr : public IAppSystem
{
public:
	virtual void SetFallbackParameters( float flBase, float flMultiplier, float flSimFallbackBaseMultiplier, float flSimThresholdMs ) = 0;
	virtual void SetSystemLevel( int nCPULevel, int nGPULevel ) = 0;

	// The client passes the low violence state of its particle host
	virtual void SetLowViolence( bool bLowViolence ) = 0;
	virtual bool IsLowViolence() = 0;

	virtual HParticleSystemDefinition FindParticleSystem( const char *pszName, bool bWarnIfNotPrecached ) = 0;

	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > GetParticleSystemDefinitionClass() = 0;

	// bCheckSystemLevel compares the minimum CPU and GPU levels of the definition with the system level and uses its fallback when they are not met
	virtual IParticleCollection *CreateParticleCollection( const char *pszName, void *pOwner, IParticleSystemQuery *pQuery, bool bCheckSystemLevel, float flDelay, int nRandomSeed ) = 0;
	virtual IParticleCollection *CreateParticleCollection( HParticleSystemDefinition hDefinition, void *pOwner, IParticleSystemQuery *pQuery, bool bCheckSystemLevel, float flDelay, int nRandomSeed ) = 0;
	virtual void DestroyParticleCollection( IParticleCollection *pCollection ) = 0;
	virtual bool unk020( void *p ) = 0;
	virtual bool unk021() = 0;
	virtual void unk022() = 0;

	// Display names such as Position, Life Duration or Radius
	virtual const char *GetParticleAttributeFriendlyName( int nAttribute ) = 0;

	virtual int GetOperatorDefinitionCount( int nFunctionType ) = 0;
	virtual IParticleFunctionDefinition *GetOperatorDefinition( int nFunctionType, int nIndex ) = 0;
	virtual void SetLastSimulationDuration( float flDuration ) = 0;
	virtual float GetLastSimulationDuration() = 0;

	virtual void DumpParticleSystems( const char *pszFilter, bool bUnk ) = 0;

	virtual CSceneObject *CreateParticleSceneObject( const char *pszName, ISceneWorld *pWorld, IParticleSystemQuery *pQuery, float flDelay, int nRandomSeed ) = 0;

	virtual CSceneObject *CreateParticleSceneObject( HParticleSystemDefinition hDefinition, ISceneWorld *pWorld, IParticleSystemQuery *pQuery, float flDelay, int nRandomSeed ) = 0;
	virtual CSceneObject *CreateParticleSceneObject( IParticleCollection *pCollection, ISceneWorld *pWorld, bool bUnk ) = 0;
	virtual void unk032( Vector vecUnk, void *pUnk1, IParticleCollection *pCollection, void *pUnk2, bool bUnk1, void *pUnk3, void *pUnk4, int nUnk1, bool bUnk2, int nUnk2 ) = 0;

	// Draws from the random sequence of a particle collection
	virtual int RandomInt( IParticleCollection *pCollection, int nMin, int nMax ) = 0;
	virtual float RandomFloat( IParticleCollection *pCollection, float flMin, float flMax ) = 0;

	// Returns "unknown" for an attribute above 48
	virtual const char *GetParticleAttributeName( int nAttribute ) = 0;
	virtual EAttributeDataType GetParticleAttributeDataType( int nAttribute ) = 0;

	// Restarts every collection of the definition
	virtual void RestartParticleSystem( HParticleSystemDefinition hDefinition ) = 0;

	// Collections whose renderers all have types in the mask are not rendered
	virtual void SetDisabledRendererTypes( int nRendererTypes ) = 0;

	// Loads the polymorphic MGPUParticleFunction object the KV3 describes
	virtual bool LoadGPUParticleFunction( const KeyValues3 *pKeyValues, void *pOut ) = 0;

	// Releases the buffers of the GPU implicit renderer and the instanced mesh scene objects unused for 100 frames
	virtual void PurgeRenderObjects() = 0;

	virtual HParticleSnapshot CreateProceduralSnapshot( CUtlString sName ) = 0;

	// Forward to the snapshot
	virtual bool InitSnapshotFromAttributeData( HParticleSnapshot hSnapshot, int nCount, const ParticleSnapshotAttributeData_t *pData ) = 0;
	virtual bool InitSnapshot( HParticleSnapshot hSnapshot, int nX, int nY, int nZ, const IParticleSnapshot::AttributeMapVector &attributeMaps ) = 0;

	// Creates a procedural snapshot with the name and copies the snapshot into it
	virtual HParticleSnapshot CopySnapshot( HParticleSnapshot hSnapshot, const CUtlString &sName ) = 0;

	// Element ( nX, nY, nZ ) of a vector container field
	virtual void SetSnapshotVector( HParticleSnapshot hSnapshot, int nX, int nY, int nZ, const int &nContainerAttribute, Vector vecValue ) = 0;
	virtual Vector GetSnapshotVector( HParticleSnapshot hSnapshot, int nParticleAttribute, int nX, int nY, int nZ ) = 0;

	virtual int GetSnapshotParticleCount( HParticleSnapshot hSnapshot ) = 0;
	virtual bool SnapshotHasAttribute( HParticleSnapshot hSnapshot, int nParticleAttribute, EAttributeDataType nDataType ) = 0;

	// Stores the particle indices of a named subset, which operators select by name
	virtual void SetSnapshotSubset( HParticleSnapshot hSnapshot, const char *pszSubsetName, const CUtlVector< int > &indices ) = 0;

	virtual void ClearSnapshot( HParticleSnapshot hSnapshot ) = 0;
	virtual int GetParticleSystemVersion() = 0;
	virtual const char *GetParticleSystemVersionChangeDescription( int nVersion ) = 0;
	// The editor sets the particle system under the cursor; renderers highlight its collections and their children
	virtual void SetHoveredParticleSystem( HParticleSystemDefinition hDefinition ) = 0;

	virtual bool HasFeatureFlags( int nFlags ) = 0;

	// Adds instances to the instanced mesh scene object matching the world, flags and the 32-byte key, creating it when missing; records are 100 bytes
	virtual void AddInstancedMeshes( ISceneWorld *pWorld, uint64 nFlags, uint64 nTypeFlagsMask, uint64 nTypeFlags, const void *pKey, const void *pInstances, uint32 nCount, bool bUnk ) = 0;

	// Physics types of the C_OP_ClientPhysics operators (m_strPhysicsType), each added once
	virtual void AddClientPhysicsType( CUtlString sPhysicsType ) = 0;
	virtual CUtlVector< CUtlString > *GetClientPhysicsTypes() = 0;
	virtual void ClearClientPhysicsTypes() = 0;

	virtual void unk059( ISceneView *pView ) = 0;
	virtual void SimulateGPUParticles( CUtlLeanVector< IParticleCollection * > collections, IRenderContext *pRenderContext ) = 0;
};

//-----------------------------------------------------------------------------
// Key of the instanced mesh scene objects of CParticleSystemMgr
//-----------------------------------------------------------------------------
#define PARTICLE_INSTANCED_MESH_KEY_HASH_SEED 0x3501A674

struct ParticleInstancedMeshKey_t
{
	// The first 32 bytes are a copy of the key passed to AddInstancedMeshes(), which the instanced mesh scene object reads too

	// Taken as a strong reference by the scene object
	ResourceHandle_t m_hModel;

	// Looked up in the material group tokens of the model to pick the material remapping
	CUtlStringToken m_MaterialGroup;

	// m_fSizeCullScale of the model renderer, copied into the draw record of every instance next to its bounding radius
	float m_flSizeCullScale;

	// Copied into the draw record of every instance; the particle renderers leave them zero
	float m_flUnk10[ 3 ];

	// Tail padding of the 32-byte key: the renderers never write it and nothing compares or hashes it
	uint8 m_pad1C[ 4 ];

	// m_nReloadCounter of the model binding, so that a reloaded model gets its own scene object
	int m_nModelReloadCounter;

	// Always zero
	uint64 m_nReserved[ 2 ];

	bool operator==( const ParticleInstancedMeshKey_t &other ) const
	{
		if ( m_hModel != other.m_hModel || m_MaterialGroup != other.m_MaterialGroup )
		{
			return false;
		}

		if ( m_flSizeCullScale != other.m_flSizeCullScale )
		{
			return false;
		}

		for ( int i = 0; i < ARRAYSIZE( m_flUnk10 ); i++ )
		{
			if ( m_flUnk10[ i ] != other.m_flUnk10[ i ] )
			{
				return false;
			}
		}

		return m_nModelReloadCounter == other.m_nModelReloadCounter && m_nReserved[ 0 ] == other.m_nReserved[ 0 ] && m_nReserved[ 1 ] == other.m_nReserved[ 1 ];
	}

	// Combines the hashes of every compared member except m_nReserved
	struct HashFunctor_t
	{
		unsigned int operator()( const ParticleInstancedMeshKey_t &key ) const
		{
			uint32 nHash = Combine( PARTICLE_INSTANCED_MESH_KEY_HASH_SEED, MurmurHash2( &key.m_hModel, sizeof( key.m_hModel ), PARTICLE_INSTANCED_MESH_KEY_HASH_SEED ) );
			nHash = Combine( nHash, key.m_MaterialGroup.GetHashCode() );

			nHash = Combine( nHash, MurmurHash2( &key.m_flSizeCullScale, sizeof( float ), PARTICLE_INSTANCED_MESH_KEY_HASH_SEED ) );

			for ( int i = 0; i < ARRAYSIZE( key.m_flUnk10 ); i++ )
			{
				nHash = Combine( nHash, MurmurHash2( &key.m_flUnk10[ i ], sizeof( float ), PARTICLE_INSTANCED_MESH_KEY_HASH_SEED ) );
			}

			return Combine( nHash, static_cast< uint32 >( key.m_nModelReloadCounter ) );
		}

	private:
		static uint32 Combine( uint32 nHash, uint32 nValue )
		{
			return nHash ^ ( nValue + 0x9E3779B9 + ( nHash << 6 ) + ( nHash >> 2 ) );
		}
	};
};

//-----------------------------------------------------------------------------
// GPU readback batch of CParticleSystemMgr: the readback buffer and the
// collections whose simulation results it holds
//-----------------------------------------------------------------------------
struct ParticleGPUReadback_t
{
	struct Collection_t
	{
		IParticleCollection *m_pCollection;

		// Unique id hash of the collection, the results are applied only while it is in the live set of the manager
		uint64 m_nCollectionId;
	};

	RenderBufferHandle_t m_hReadbackBuffer;
	CUtlVector< Collection_t > m_Collections;
};

//-----------------------------------------------------------------------------
// Implementation of IParticleSystemMgr
//-----------------------------------------------------------------------------
class CParticleSystemMgr : public CTier3AppSystem< IParticleSystemMgr >, public ISceneObjectListener, public IToolsResourceListener
{
public:
	void OnSceneObjectDeleted( CSceneObject *pSceneObject ) override;
	void NotifyResourceStatusChange( const ResourceStatusChange_t *pChanges, int nNumChanges ) override;

	CUtlVector< IParticleFunctionDefinition * > m_ParticleOperators[ 9 ];

private:
	uint8 m_unk110[ 8 ];

public:
	// Zeroed by the constructors only
	int m_nUnk118;

	float m_flLastSimulationDuration;

	// particle_profile / particle_profile_spike state
	int m_nNumFramesMeasured;
	float m_flSimTimeTotal;
	float m_flSimTimeMin;
	float m_flSimTimeMax;
	int m_nProfileFrameCount;
	int m_nProfileSpikeThresholdMs;
	int m_nProfileType;

	float m_flFallbackBase;
	float m_flFallbackMultiplier;
	float m_flSimFallbackBaseMultiplier;
	float m_flSimThresholdMs;
	int m_nCPULevel;
	int m_nGPULevel;

private:
	uint8 m_unk154[ 4 ];

public:
	// Incremented when the type manager releases a particle system definition
	uint64 m_nDefinitionReleaseSerial;

	bool m_bLowViolence;

	// Game info settings read in Init
	bool m_bPerObjectLightingSetup;
	bool m_bSupportFadingOpaqueModels;
	bool m_bEnableParticleShaderFeatureBranching;
	bool m_bBindlessParticleShader;
	bool m_bFloat16HDRBackBuffer;
	bool m_bGameSupportsLegacyShaders;
	bool m_bParticlesFoggedByDefault;
	bool m_bDisableSorting;

	int m_nDisabledRendererTypes;

	// Procedural sprite materials by the hash of their parameters, one table per renderer shader type;
	// override shaders use 1 + their index in m_ShaderOverrideNames
	CUtlHashtable< uint64, HMaterialStrong > m_ProceduralMaterials[ 17 ];

	CUtlVectorFixedGrowable< CUtlString, 16 > m_ShaderOverrideNames;

private:
	uint8 m_unk428[ 0xA0 ];

public:
	CStrongHandle< InfoForResourceTypeIParticleSystemDefinition > m_hHoveredParticleSystem;

	// Zeroed by the constructors only
	uint64 m_nUnk4D0;

	CGpuImplicitRenderer *m_pGpuImplicitRenderer;

	// 1 = no_gpu_blobs, 2 = non_homogenous_forward_layer_only (Particles/features)
	int m_nFeatureFlags;

	CThreadRWLock_FastRead m_Lock;
	CUtlHashtable< ParticleInstancedMeshKey_t, CUtlLinkedList< CInstancedMeshSceneObject *, unsigned short >, ParticleInstancedMeshKey_t::HashFunctor_t > m_InstancedMeshSceneObjects;
	ISceneObjectDesc *m_pInstancedMeshSceneObjectDesc;
	HGameResourceManifest m_hGPUParticleRequiredResources;
	CUtlVector< CUtlString > m_ClientPhysicsTypes;

	// Ranges of GPU particle slots and one slot per GPU collection
	CPoolAllocator *m_pGPUParticlePool;
	CPoolAllocator *m_pGPUCollectionPool;

	// One byte per GPU particle slot, uploaded to the per-frame buffer
	CUtlVector< uint8 > m_GPUParticleData;

	RenderBufferHandle_t m_hReadbackBuffer;
	RenderBufferHandle_t m_hPerFrameBuffer;
	CAtomicMutex m_Mutex;
	CUtlLinkedList< ParticleGPUReadback_t, unsigned short > m_GPUReadbacks;

	// Unique id hashes of the live GPU collections
	CUtlHashtable< uint64 > m_GPUCollectionIds;
};

#endif // IPARTICLESYSTEMMGR_H
