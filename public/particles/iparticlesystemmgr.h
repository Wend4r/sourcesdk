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
#include <resourcesystem/resourcehandle.h>

class IParticleSystemDefinition;
class IParticleSnapshot;
class IParticleCollection;
class IParticleSystemQuery;
class IRenderContext;
class ISceneWorld;
class CSceneObject;
class CSchemaClassInfo;
class KeyValues3;
class Vector;
struct ParticleRenderData_t;
template < class T > struct SchemaMetaInfoHandle_t;

class InfoForResourceTypeIParticleSystemDefinition
{
public:
	using RuntimeClass_t = IParticleSystemDefinition;
};

class InfoForResourceTypeIParticleSnapshot
{
public:
	using RuntimeClass_t = IParticleSnapshot;
};

using HParticleSystemDefinition = CWeakHandle< InfoForResourceTypeIParticleSystemDefinition >;
using HParticleSnapshot = CWeakHandle< InfoForResourceTypeIParticleSnapshot >;

// Bits of IsFeatureEnabled, read from the "features" key of the Particles block of gameinfo
enum ParticleSystemFeatureFlags_t
{
	PARTICLE_FEATURE_NO_GPU_BLOBS = ( 1 << 0 ),							// "no_gpu_blobs"
	PARTICLE_FEATURE_NON_HOMOGENOUS_FORWARD_LAYER_ONLY = ( 1 << 1 ),	// "non_homogenous_forward_layer_only"
};

// CParticleSystemMgr in particles.dll / libparticles.so. No destructor slots in the interface;
// on Linux the CParticleSystemMgr table has two more slots at the end that are not part of it.
abstract_class IParticleSystemMgr : public IAppSystem
{
public:
	// Stored as they are; CParticleCollection::Init picks the fallback definition with them
	virtual void SetFallbackParameters( float flBase, float flMultiplier, float flSimFallbackBaseMultiplier, float flSimThresholdMs ) = 0;
	// A definition whose minimum CPU or GPU level is above these falls back when created with bCheckSystemLevel
	virtual void SetSystemLevel( int nCPULevel, int nGPULevel ) = 0;

	// Named by what it does: a flag the client sets every frame next to SetFallbackParameters and
	// SetSystemLevel; particles.dll itself never reads it
	virtual void SetExternalFlag( bool bValue ) = 0;
	virtual bool GetExternalFlag() = 0;

	// Performs a blocking load (with a warning when bWarnIfNotPrecached) if the system was not precached
	virtual HParticleSystemDefinition FindParticleSystem( const char *pszName, bool bWarnIfNotPrecached ) = 0;

	// Named by what it does: the schema class of CParticleSystemDefinition
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > GetParticleSystemDefinitionClassInfo() = 0;

	// Unknown names and empty definitions fall back to the error particle system.
	// pOwner is returned by slot 1 of the collection (child collections get NULL); pQuery defaults to
	// the built-in CDefaultParticleSystemQuery and is passed on to the child collections.
	// bCheckSystemLevel falls back when the definition needs a higher CPU or GPU level than SetSystemLevel set.
	// MSVC orders the two overloads in reverse (Windows: handle first)
	virtual IParticleCollection *CreateParticleCollection( const char *pszName, void *pOwner, IParticleSystemQuery *pQuery, bool bCheckSystemLevel, float flDelay, int nRandomSeed ) = 0;
	virtual IParticleCollection *CreateParticleCollection( HParticleSystemDefinition hDefinition, void *pOwner, IParticleSystemQuery *pQuery, bool bCheckSystemLevel, float flDelay, int nRandomSeed ) = 0;
	virtual void DestroyParticleCollection( IParticleCollection *pCollection ) = 0;

	// Named by what the caller does: the client asks it for every queued effect before it runs the
	// deferred callback of that effect. Always true in CS2
	virtual bool IsCollectionReady( IParticleCollection *pCollection ) = 0;
	// Named by what it does: always true in CS2 (the linker folded it with IsCollectionReady); no caller found
	virtual bool IsReady() = 0;
	// Named by what it does: empty in CS2; no caller found
	virtual void EmptyFrameHook() = 0;

	// Display names: "Position", "Life Duration", "Position Previous", "Radius", ...
	virtual const char *GetParticleAttributeDisplayName( int nAttribute ) = 0;

	// The registered operator definitions of a function type (emitter, initializer, operator, renderer, ...)
	virtual int GetOperatorDefinitionCount( int nFunctionType ) = 0;
	virtual void *GetOperatorDefinition( int nFunctionType, int nIndex ) = 0;

	// Also runs the sleep and fallback bookkeeping of the frame
	virtual void SetLastSimulationTime( float flTime ) = 0;
	virtual float GetLastSimulationTime() = 0;

	// bInstanceCounts also lists the instance counts per system
	virtual void DumpParticleList( const char *pszNameSubstring, bool bInstanceCounts ) = 0;

	// Creates a collection and a particle scene object for it in pWorld. MSVC orders the three
	// overloads in reverse (Windows: collection, definition, name)
	virtual CSceneObject *CreateParticleSceneObject( const char *pszName, ISceneWorld *pWorld, IParticleSystemQuery *pQuery, float flDelay, int nRandomSeed ) = 0;
	virtual CSceneObject *CreateParticleSceneObject( HParticleSystemDefinition hDefinition, ISceneWorld *pWorld, IParticleSystemQuery *pQuery, float flDelay, int nRandomSeed ) = 0;
	// The scene object deletes the collection with itself when bOwnsCollection is set
	virtual CSceneObject *CreateParticleSceneObject( IParticleCollection *pCollection, ISceneWorld *pWorld, bool bOwnsCollection ) = 0;

	// Fills pOut with the particles of pParticles for rendering in one of three layouts (nLayout 0..2) and sorts the
	// pointers in ppSorted by their key when bSorted is set. Returns the number of particles.
	// Named by what the code does with them: pDirection is a Vector followed by a bool, read only with
	// bUseDirection; pIndexRemap maps particle indices (negative entries are skipped); nKeyAttribute
	// is the particle attribute the keys are taken from
	virtual int GenerateSortedIndexList( Vector vecCameraPos, const void *pDirection, IParticleCollection *pParticles, const CUtlVector< int > *pIndexRemap, bool bSorted, ParticleRenderData_t *pOut, ParticleRenderData_t **ppSorted, int nLayout, bool bUseDirection, int nKeyAttribute ) = 0;

	// Draws from the random sequence of the collection
	virtual int RandomInt( IParticleCollection *pCollection, int nMin, int nMax ) = 0;
	virtual float RandomFloat( IParticleCollection *pCollection, float flMin, float flMax ) = 0;

	// The internal attribute names ("XYZ", "LIFE_DURATION", ...); "unknown" for an index out of range
	virtual const char *GetParticleAttributeName( int nAttribute ) = 0;
	// EAttributeDataType of the attribute
	virtual int GetParticleAttributeDataType( int nAttribute ) = 0;

	// Named by what it does: restarts every collection that uses the definition
	virtual void RestartAllCollections( HParticleSystemDefinition hDefinition ) = 0;
	// Named by what it does: a collection whose flags are not zero and all inside nMask is treated as
	// empty (its bounds are reset and it is not drawn)
	virtual void SetHiddenCollectionFlags( int nMask ) = 0;

	// Named by what it does: reads the polymorphic MGPUParticleFunction objects of pKV into outFunctions
	virtual bool LoadGPUParticleFunctions( const KeyValues3 *pKV, CUtlVector< void * > &outFunctions ) = 0;

	// Named by what it does: frees the queued GPU buffers and the instanced mesh scene objects
	// that were not used for 100 frames
	virtual void PurgeUnusedSceneObjects() = 0;

	// Particle snapshots made at run time ("_procedural_particle_snapshot.vsnap" when sName is empty)
	virtual HParticleSnapshot CreateProceduralSnapshot( CUtlString sName ) = 0;
	// Named by what it does: IParticleSnapshot slot 1, fills the snapshot from vertex data
	virtual void InitSnapshotFromData( HParticleSnapshot hSnapshot, int nCount, const void *pData ) = 0;
	// Named by what it does: IParticleSnapshot slot 0 (CParticleSnapshot::Init)
	virtual void InitSnapshot( HParticleSnapshot hSnapshot, int nParticleCount, int nRowCount, int nSliceCount, const void *pAttributeMap ) = 0;
	// Named by what it does: a new procedural snapshot named sName with the data of hSource
	virtual HParticleSnapshot CloneSnapshot( HParticleSnapshot hSource, const CUtlString &sName ) = 0;
	virtual void SetSnapshotVector( HParticleSnapshot hSnapshot, int nX, int nY, int nZ, const int &nAttribute, Vector vecValue ) = 0;
	// A zero vector for a particle index out of range
	virtual Vector GetSnapshotVector( HParticleSnapshot hSnapshot, int nAttribute, int nX, int nY, int nZ ) = 0;
	virtual int GetSnapshotParticleCount( HParticleSnapshot hSnapshot ) = 0;
	virtual bool SnapshotHasAttributeOfType( HParticleSnapshot hSnapshot, int nAttribute, int nDataType ) = 0;
	// Named by what it does: sets the index list named pszName (case-insensitive), adding it when missing
	virtual void SetSnapshotIndexList( HParticleSnapshot hSnapshot, const char *pszName, const CUtlVector< int > &indices ) = 0;
	// Named by what it does: sets the particle count to zero
	virtual void ClearSnapshot( HParticleSnapshot hSnapshot ) = 0;

	// The version of the particle system format and the description of each version change
	virtual int GetParticleSystemVersion() = 0;
	virtual const char *GetParticleSystemVersionChangeDescription( int nVersion ) = 0;

	// Named by what it does: keeps a reference to hResource; NULL releases it
	virtual void HoldResourceReference( ResourceHandle_t hResource ) = 0;

	// nFeatureMask: ParticleSystemFeatureFlags_t
	virtual bool IsFeatureEnabled( uint32 nFeatureMask ) = 0;

	// Named by what it does: adds nInstanceCount 100-byte instances to pooled instanced mesh scene
	// objects (CInstancedMeshSceneObjectDesc) of pWorld, in batches. A pooled object is reused when its
	// world, object flags, type flags under nTypeFlagsMask, the 32-byte pBatchKey and bBatchVariant match
	virtual void AddInstancedMeshInstances( ISceneWorld *pWorld, uint64 nObjectFlags, uint64 nTypeFlagsMask, uint64 nTypeFlags, const void *pBatchKey, const void *pInstances, uint32 nInstanceCount, bool bBatchVariant ) = 0;

	// Named by what it does: a list of unique strings: add, get and clear
	virtual void AddUniqueString( CUtlString sValue ) = 0;
	virtual CUtlVector< CUtlString > *GetUniqueStrings() = 0;
	virtual void ClearUniqueStrings() = 0;

	// Named by what it does: empty in CS2; no caller found
	virtual void EmptyGPUHook() = 0;

	// Simulates the queued GPU particle work and empties the queue
	virtual void SimulateGPUParticles( CUtlVector< void * > &queuedWork, IRenderContext *pRenderContext ) = 0;
};

#endif // IPARTICLESYSTEMMGR_H
