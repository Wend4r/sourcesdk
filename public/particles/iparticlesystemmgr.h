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
#include <resourcesystem/resourcehandle.h>
#include <resourcesystem/iresourcesystem.h>
#include <schemasystem/schematypes.h>
#include <tier0/utlhashtable.h>
#include <tier0/utlleanvector.h>
#include <tier0/utlsoacontainer.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>
#include <tier0/threadtools.h>
#include <tier3/tier3.h>

class CGpuImplicitRenderer;
class CPoolAllocator;
class CSceneObject;
class CSchemaClassInfo;
class IParticleCollection;
class IParticleFunctionDefinition;
class IParticleSnapshot;
class IParticleSystemDefinition;
class IParticleSystemQuery;
class IRenderContext;
class ISceneObjectDesc;
class ISceneView;
class ISceneWorld;
class KeyValues3;

class InfoForResourceTypeIParticleSystemDefinition
{
public:
	using RuntimeClass_t = IParticleSystemDefinition;
};

using HParticleSystemDefinition = CWeakHandle< InfoForResourceTypeIParticleSystemDefinition >;

class InfoForResourceTypeIParticleSnapshot
{
public:
	using RuntimeClass_t = IParticleSnapshot;
};

abstract_class IParticleSystemMgr : public IAppSystem
{
public:
	virtual void SetFallbackParameters( float flBase, float flMultiplier, float flSimFallbackBaseMultiplier, float flSimThresholdMs ) = 0;
	virtual void SetSystemLevel( int nCPULevel, int nGPULevel ) = 0;

	// Setter and getter of the same flag
	virtual void unk013( bool bValue ) = 0;
	virtual bool unk014() = 0;

	virtual HParticleSystemDefinition FindParticleSystem( const char *pszName, bool bWarnIfNotPrecached ) = 0;

	// Schema class of CParticleSystemDefinition
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > unk016() = 0;

	virtual IParticleCollection *CreateParticleCollection( const char *pszName, void *pOwner, IParticleSystemQuery *pQuery, bool bUnk, float flDelay, int nRandomSeed ) = 0;
	virtual IParticleCollection *CreateParticleCollection( HParticleSystemDefinition hDefinition, void *pOwner, IParticleSystemQuery *pQuery, bool bUnk, float flDelay, int nRandomSeed ) = 0;
	virtual void DestroyParticleCollection( IParticleCollection *pCollection ) = 0;
	virtual bool unk020( void *p ) = 0;
	virtual bool unk021() = 0;
	virtual void unk022() = 0;

	// Display names such as Position, Life Duration or Radius
	virtual const char *unk023( int nAttribute ) = 0;

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

	virtual void unk037( HParticleSystemDefinition hDefinition ) = 0;
	virtual void unk038( int nValue ) = 0;
	virtual bool unk039( const KeyValues3 *pKeyValues, void *pOut ) = 0;
	virtual void unk040() = 0;

	virtual CWeakHandle< InfoForResourceTypeIParticleSnapshot > CreateProceduralSnapshot( CUtlString sName ) = 0;

	virtual bool unk042( CWeakHandle< InfoForResourceTypeIParticleSnapshot > hSnapshot, int nCount, const void *pData ) = 0;
	virtual bool unk043( CWeakHandle< InfoForResourceTypeIParticleSnapshot > hSnapshot, int nX, int nY, int nZ, const void *pAttributeMaps ) = 0;
	virtual CWeakHandle< InfoForResourceTypeIParticleSnapshot > unk044( CWeakHandle< InfoForResourceTypeIParticleSnapshot > hSnapshot, const CUtlString &sName ) = 0;
	virtual void unk045( CWeakHandle< InfoForResourceTypeIParticleSnapshot > hSnapshot, int nX, int nY, int nZ, const int &nAttribute, Vector vecValue ) = 0;
	virtual Vector unk046( CWeakHandle< InfoForResourceTypeIParticleSnapshot > hSnapshot, int nAttribute, int nX, int nY, int nZ ) = 0;
	virtual int unk047( CWeakHandle< InfoForResourceTypeIParticleSnapshot > hSnapshot ) = 0;
	virtual bool unk048( CWeakHandle< InfoForResourceTypeIParticleSnapshot > hSnapshot, int nAttribute, EAttributeDataType nDataType ) = 0;
	virtual void unk049( CWeakHandle< InfoForResourceTypeIParticleSnapshot > hSnapshot, const char *pszUnk, void *pUnk ) = 0;
	virtual void unk050( CWeakHandle< InfoForResourceTypeIParticleSnapshot > hSnapshot ) = 0;
	virtual int GetParticleSystemVersion() = 0;
	virtual const char *GetParticleSystemVersionChangeDescription( int nVersion ) = 0;
	virtual void unk053( HParticleSystemDefinition hDefinition ) = 0;
	virtual bool unk054( int nFlags ) = 0;
	// Adds instances to the instanced mesh scene object matching the world, flags and the 32-byte key, creating it when missing; records are 100 bytes
	virtual void unk055( ISceneWorld *pWorld, uint64 nFlags, uint64 nTypeFlagsMask, uint64 nTypeFlags, const void *pKey, const void *pInstances, uint32 nCount, bool bUnk ) = 0;
	virtual void unk056( CUtlString sName ) = 0;
	virtual CUtlVector< CUtlString > *unk057() = 0;
	virtual void unk058() = 0;
	virtual void unk059( ISceneView *pView ) = 0;
	virtual void SimulateGPUParticles( CUtlLeanVector< IParticleCollection * > collections, IRenderContext *pRenderContext ) = 0;
};

//-----------------------------------------------------------------------------
// One entry of the batch passed to IToolsResourceListener
//-----------------------------------------------------------------------------
struct ResourceStatusChange_t
{
	ResourceId_t m_nResourceId;
	ResourceHandle_t m_hResource;
	ResourceType_t m_nResourceType;
	ResourceLoadType_t m_nLoadType;
};

abstract_class IToolsResourceListener
{
public:
	virtual bool unk000() = 0;
	virtual void NotifyResourceStatusChange( const ResourceStatusChange_t *pChanges, int nNumChanges ) = 0;
};

abstract_class ISceneObjectListener
{
public:
	virtual void unk000( CSceneObject *pSceneObject ) = 0;
};

//-----------------------------------------------------------------------------
// Implementation of IParticleSystemMgr. Engine containers whose element types
// are not known stay as padding: 17 hash tables with 24-byte entries after the
// flags, a hash table of 88-byte entries keyed by instanced mesh parameters
// after the lock, and the linked list of GPU readback batches.
//-----------------------------------------------------------------------------
class CParticleSystemMgr : public CTier3AppSystem< IParticleSystemMgr >, public ISceneObjectListener, public IToolsResourceListener
{
public:
	void unk000( CSceneObject *pSceneObject ) override;
	void NotifyResourceStatusChange( const ResourceStatusChange_t *pChanges, int nNumChanges ) override;

	CUtlVector< IParticleFunctionDefinition * > m_ParticleOperators[ 9 ];
	uint8 m_unk110[ 8 ];
	uint8 m_unk118[ 4 ];
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
	uint8 m_unk154[ 4 ];

	// Incremented when the type manager releases a particle system definition
	uint64 m_nUnk158;

	bool m_bUnk160;

	// Game info settings read in Init
	bool m_bPerObjectLightingSetup;
	bool m_bSupportFadingOpaqueModels;
	bool m_bEnableParticleShaderFeatureBranching;
	bool m_bBindlessParticleShader;
	bool m_bFloat16HDRBackBuffer;
	bool m_bGameSupportsLegacyShaders;
	bool m_bParticlesFoggedByDefault;
	bool m_bDisableSorting;

	int m_nUnk16C;
	uint8 m_unk170[ 17 ][ 32 ];
	CUtlVectorFixedGrowable< CUtlString, 16 > m_Unk390;
	uint8 m_unk428[ 0xA0 ];
	CStrongHandle< InfoForResourceTypeIParticleSystemDefinition > m_hUnk4C8;
	uint8 m_unk4D0[ 8 ];
	CGpuImplicitRenderer *m_pGpuImplicitRenderer;

	// 1 = no_gpu_blobs, 2 = non_homogenous_forward_layer_only (Particles/features)
	int m_nFlags;

	CThreadRWLock_FastRead m_Lock;
	uint8 m_unk528[ 0x20 ];
	ISceneObjectDesc *m_pInstancedMeshSceneObjectDesc;
	HGameResourceManifest m_hGPUParticleRequiredResources;
	CUtlVector< CUtlString > m_Unk558;
	CPoolAllocator *m_pUnk570;
	CPoolAllocator *m_pUnk578;

	// Uploaded to the per-frame buffer, sized to the first pool
	CUtlVector< uint8 > m_Unk580;

	void *m_hReadbackBuffer;
	void *m_hPerFrameBuffer;
	CAtomicMutex m_Mutex;
	uint64 m_unk5B8[ 3 ];
	CUtlHashtable< uint64 > m_Unk5D0;
};

#endif // IPARTICLESYSTEMMGR_H
