//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Rubikon physics interface: worlds, shapes, collision geometry and debugging
//
//===========================================================================//

#ifndef IVPHYSICS2_H
#define IVPHYSICS2_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/iappsystem.h"
#include "mathlib/mathlib.h"
#include "mathlib/transform.h"
#include "mathlib/vector.h"
#include "physicslib/fnphysicsjoint.h"
#include "physicslib/fnphysicsshape.h"
#include "physicslib/param_types.h"
#include "physicslib/physaggregatedata.h"
#include "physicslib/serialize.h"
#include "resourcefile/introspectedtokv3typemanager.h"
#include "resourcesystem/iresourcesystem.h"
#include "tier0/refcount.h"
#include "tier0/smartptr.h"
#include "tier0/threadtools.h"
#include "tier0/utlhash.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlvector.h"
#include "tier2/tier2.h"
#include "vconcomm/ivconcomm.h"
#include "vphysics2/handle_decl.h"
#include "vphysics2/interface.h"
#include "vphysics2/intersection_controller_interface.h"
#include "vphysics2/iphysicsbody.h"
#include "vphysics2/iphysicscontact.h"
#include "vphysics2/iphysicsjoint.h"
#include "vphysics2/iphysicsshape.h"
#include "vphysics2/iphysicssoftbody.h"
#include "vphysx/vphysx_collisionmesh_resource.h"
#include "vphysx/vphysx_interface.h"
#include "vphysx/vphysx_interface_params.h"

class IVConComm;
class CCommand;
class ISceneViewDebugOverlays;
class CFrustum;
class InfoForResourceTypeIMaterial2;

template < class T >
class CWeakHandle;
class CSceneAnimatableObject;
class CMesh;
class CFeModel;
class CGJKProxy;

struct VPhysIntersectionNotification_t;
struct SceneObjectBuffers_t;
struct RnHullSimplificationParams_t;
struct RnSimplexCache_t;
struct PhysicsWorldUpdateConCommParams_t;
struct Ray_t;

enum RnState_t
{
	STATE_UNKNOWN = 0,
	STATE_FAILED,
	STATE_OVERLAPPED,
	STATE_TOUCHING,
	STATE_SEPARATED
};

// Start and end placement of a shape moving over a time step
struct RnSweptTransform_t
{
	Vector m_vPosition1;
	Vector m_vPosition2;
	Quaternion m_qOrientation1;
	Quaternion m_qOrientation2;
};

struct RnConvexCastResult_t
{
	RnState_t m_nState;
	float m_flTime;
};

struct RnLinearCastResult_t
{
	RnState_t m_nState;
	float m_flTime;
	Vector m_vClosest1;
	Vector m_vClosest2;
	Vector m_vNormal;
};

struct RnDistanceQueryResult_t
{
	float m_flDistance;
	Vector m_vPoint1;
	Vector m_vPoint2;
};

// Holds a reference to aggregate data through IVPhysics2::AddDataReference and ReleaseDataReference
class CPhysAggregateDataHandle
{
public:
	CPhysAggregateDataHandle() : m_pData( nullptr ) {}
	CPhysAggregateDataHandle( CPhysAggregateData *pData ) : m_pData( nullptr ) { Set( pData ); }
	CPhysAggregateDataHandle( const CPhysAggregateDataHandle &other ) : m_pData( nullptr ) { Set( other.m_pData ); }
	CPhysAggregateDataHandle( CPhysAggregateDataHandle &&other ) : m_pData( other.m_pData ) { other.m_pData = nullptr; }
	~CPhysAggregateDataHandle() { Clear(); }

	CPhysAggregateDataHandle &operator=( const CPhysAggregateDataHandle &other ) { Set( other.m_pData ); return *this; }

	inline void Set( CPhysAggregateData *pData );
	inline void Clear();

	CPhysAggregateData *Get() const { return m_pData; }
	CPhysAggregateData *operator->() const { return m_pData; }
	bool IsValid() const { return m_pData != nullptr; }
	explicit operator bool() const { return IsValid(); }

private:
	CPhysAggregateData *m_pData;
};

struct RnMassProperties_t
{
	matrix3x4_t m_Inertia;
	Vector m_vCenter;
	float m_flMass;
};

enum PhysicsDebugMaterialId_t
{
	PHYSICS_DEBUG_MATERIAL_SOLID_WITH_EDGES = 0,
	PHYSICS_DEBUG_MATERIAL_SOLID,
	PHYSICS_DEBUG_MATERIAL_WIRE,
	PHYSICS_DEBUG_MATERIAL_TRANSLUCENT,
	PHYSICS_DEBUG_MATERIAL_NODEPTHTEST,
	PHYSICS_DEBUG_MATERIAL_TRANSLUCENT_NODEPTHTEST,

	PHYSICS_DEBUG_MATERIAL_COUNT
};

struct VPhysContactImpactStrength_t
{
	float m_flTangentImpact;
	float m_flNormalImpact;
};

// Created by IVPhysics2::CreateIkSolver; every method forwards to the solver object the implementation owns.
// The vectors and transforms are read from per-index arrays of that solver, the transform arguments are applied to them
abstract_class IIkSolver
{
public:
	virtual ~IIkSolver() {}

	virtual int unk001() const = 0;
	virtual int unk002( int nIndex ) const = 0;
	virtual float unk003( int nIndex ) const = 0;

	virtual Vector unk004( int nIndex ) const = 0;
	virtual Vector unk005( int nIndex ) const = 0;
	virtual Vector unk006( int nIndex, const CTransform &transform ) const = 0;
	virtual Vector unk007( int nIndex ) const = 0;

	virtual CTransform unk008( int nIndex ) const = 0;
	virtual CTransform unk009( int nIndex, const CTransform &transform ) const = 0;

	// Return the orientation of unk008 (unk011 combined with an argument) as a 16-byte class in one vector register on Linux
	virtual void unk010( void *p ) = 0;
	virtual void unk011( void *p ) = 0;

	// Position of unk008
	virtual Vector unk012( int nIndex ) const = 0;
	virtual Vector unk013( int nIndex, const CTransform &transform ) const = 0;

	virtual CTransform unk014( int nIndex ) const = 0;
	virtual CTransform unk015( int nIndex, const CTransform &transform ) const = 0;

	// Same as unk010 and unk011 for unk014
	virtual void unk016( void *p ) = 0;
	virtual void unk017( void *p ) = 0;

	// Position of unk014
	virtual Vector unk018( int nIndex ) const = 0;
	virtual Vector unk019( int nIndex, const CTransform &transform ) const = 0;

	virtual float unk020( int nIndex ) const = 0;
	virtual float unk021( int nIndex ) const = 0;
	virtual float unk022( int nIndex ) const = 0;

	// Runs the solver for a number of iterations over a list of targets
	virtual void unk023( void *p ) = 0;
};

// Reference counted through CSmartPtr; returned by IVPhysics2::CreateSerialStreamSnapshot
abstract_class IPhysSerialStreamSnapshot
{
public:
	virtual ~IPhysSerialStreamSnapshot() {}

	// Size of the data unk002 writes
	virtual uint32 unk001() const = 0;

	// Writes a 0x1D0-byte header followed by the snapshot data and advances *ppBuffer
	virtual void unk002( uint8 **ppBuffer ) = 0;

	virtual void unk003() = 0;

	// Empty in release builds
	virtual void unk004() = 0;

	// Returns a 20-byte class filled with zeros in release builds
	virtual void unk005( void *p ) = 0;

	// Returns 0 in release builds
	virtual void *unk006() = 0;

	// Returns an 8-byte class filled with zeros in release builds
	virtual void unk007( void *p ) = 0;

	// Copy the pointers of one of the recorded lists
	virtual void unk008( int nType, CUtlVector< void * > *pOut ) const = 0;
	virtual void unk009( int nType, CUtlVector< void * > *pOut ) const = 0;
	virtual void unk010( CUtlVector< void * > *pOut ) const = 0;

	// Return 0
	virtual void *unk011() = 0;
	virtual void *unk012( void *p ) = 0;

	virtual void unk013( void *p ) = 0;

	// Returns 0 in release builds
	virtual void *unk014() = 0;

	int AddRef() { return ++m_nRefCount; }
	int Release()
	{
		int nRefCount = --m_nRefCount;
		if ( nRefCount == 0 )
			delete this;

		return nRefCount;
	}

private:
	CInterlockedInt m_nRefCount;
};

// Reference counted through CSmartPtr; returned by IVPhysics2::CreateDataDescConvertor
abstract_class IDataDescConvertor : public CRefCounted<>
{
};

abstract_class IVPhysics2 : public IAppSystem
{
public:
	virtual ~IVPhysics2() {}

	virtual IVPhysics2World *CreateWorld( uint32 nFlags ) = 0;
	virtual void DestroyWorld( IVPhysics2World *pWorld ) = 0;
	virtual int GetWorldCount() = 0;
	virtual IVPhysics2World *GetWorld( int nIndex ) = 0;

	virtual void SimulateWorlds( IVPhysics2World **ppWorlds, int nWorldCount, float flTimeStep, int nSubsteps, bool bAllowThreading, VPhysXWorldStepResults *pResults ) = 0;

	// Steps the softbodies of all given worlds in parallel jobs, nSubsteps times flSubstepTime each
	virtual void SimulateSoftbodies( IVPhysics2World **ppWorlds, int nWorldCount, float flSubstepTime, int nSubsteps ) = 0;

	// Locks every world and resets a set of its per-step arrays
	virtual void unk018() = 0;

	// Rubikon threading and SIMD switches; both are process-wide
	virtual bool IsMultithreadingEnabled() = 0;
	virtual void SetMultithreadingEnabled( bool bEnabled ) = 0;
	virtual bool IsSIMDEnabled() = 0;
	virtual void SetSIMDEnabled( bool bEnabled ) = 0;

	virtual IPhysSurfacePropertyController *GetSurfacePropertyController() = 0;

	virtual void InitIntersectionDictionary() = 0;

	virtual void LoadCollisionDetailLayers( bool bUnknown ) = 0;

	virtual IPhysIntersectionDictionary *GetIntersectionDictionary() = 0;
	virtual const IPhysIntersectionDictionary *GetIntersectionDictionary() const = 0;
	virtual IPhysIntersectionController *GetIntersectionController() = 0;
	virtual const IPhysIntersectionController *GetIntersectionController() const = 0;

	virtual IPhysicsShape *CreateSphereShape( const RnSphere_t &sphere, float flScale ) = 0;
	virtual IPhysicsShape *CreateCapsuleShape( const RnCapsule_t &capsule, float flScale ) = 0;
	virtual IPhysicsShape *CreateHullShape( const RnHull_t *pHull, float flScale ) = 0;
	virtual IPhysicsShape *CreateMeshShape( const RnMesh_t *pMesh, const Vector &vScale ) = 0;
	virtual IPhysicsShape *CreateCompoundShape( const RnCompound_t *pCompound, float flScale ) = 0;
	virtual void DestroyShape( IPhysicsShape *pShape ) = 0;

	virtual bool ShapesHaveSameCollisionFilter( IPhysicsShape *pShape1, IPhysicsShape *pShape2 ) = 0;

	// Casts two convex proxies, each moving by its own translation
	virtual RnLinearCastResult_t LinearCast( const CTransform &transform1, const CGJKProxy &proxy1, const Vector &vTranslation1, const CTransform &transform2, const CGJKProxy &proxy2, const Vector &vTranslation2, float flUnknown, float flRadius ) = 0;
	virtual RnLinearCastResult_t LinearCast( const CTransform &transform1, const CGJKProxy &proxy1, float flRadius1, const Vector &vTranslation1, const CTransform &transform2, const CGJKProxy &proxy2, float flRadius2, const Vector &vTranslation2 ) = 0;

	// Time of impact of two shapes moving between the placements of their swept transforms
	virtual RnConvexCastResult_t ConvexCast( const RnSweptTransform_t &sweep1, IPhysicsShape *pShape1, const RnSweptTransform_t &sweep2, IPhysicsShape *pShape2 ) = 0;
	virtual RnConvexCastResult_t ConvexCast( const RnSweptTransform_t &sweep1, const RnHull_t *pHull1, const RnSweptTransform_t &sweep2, const RnHull_t *pHull2 ) = 0;
	virtual RnConvexCastResult_t ConvexCast( const RnSweptTransform_t &sweep1, const Ray_t &shape1, const RnSweptTransform_t &sweep2, const Ray_t &shape2 ) = 0;
	virtual RnConvexCastResult_t ConvexCast( const RnSweptTransform_t &sweep1, const CGJKProxy &proxy1, float flRadius1, const RnSweptTransform_t &sweep2, const CGJKProxy &proxy2, float flRadius2 ) = 0;

	// Cast vStart + vDelta against a scaled mesh or hull
	virtual bool unk043( CShapeCastResult *pResult, const RnMesh_t *pMesh, const Vector &vScale, const Vector &vStart, const Vector &vDelta ) = 0;
	virtual bool unk044( CShapeCastResult *pResult, const RnHull_t *pHull, float flScale, const Vector &vStart, const Vector &vDelta ) = 0;

	// Cast a shape moved by vDelta from vStart against another shape placed at shapeTransform
	virtual bool unk045( PhysicsTrace_t *pTrace, const Ray_t &ray, const Vector &vStart, const Vector &vDelta, const Ray_t &shape, const CTransform &shapeTransform ) = 0;
	virtual bool TraceShape( PhysicsTrace_t *pTrace, const Ray_t &ray, const Vector &vStart, const Vector &vDelta, const Ray_t &shape, const CTransform &shapeTransform ) = 0;

	virtual RnDistanceQueryResult_t ClosestPoints( const CTransform &transform1, IPhysicsShape *pShape1, const CTransform &transform2, IPhysicsShape *pShape2 ) = 0;
	virtual RnDistanceQueryResult_t ClosestPoints( const CTransform &transform1, const RnHull_t *pHull1, const CTransform &transform2, const RnHull_t *pHull2, RnSimplexCache_t *pCache ) = 0;
	virtual RnDistanceQueryResult_t ClosestPoints( const matrix3x4_t &transform1, const CGJKProxy &proxy1, const matrix3x4_t &transform2, const CGJKProxy &proxy2, RnSimplexCache_t *pCache, int nMaxIterations ) = 0;

	// Clamped to zero below a small margin
	virtual float GetDistanceBetweenHulls( const Vector *pVertices1, int nVertexCount1, const Vector *pVertices2, int nVertexCount2 ) = 0;
	virtual float GetDistanceBetweenHulls( const matrix3x4_t &transform1, const Vector *pVertices1, int nVertexCount1, const matrix3x4_t &transform2, const Vector *pVertices2, int nVertexCount2 ) = 0;

	// Distance between the two shapes minus their radii
	virtual float unk052( IPhysicsShape *pShape1, const matrix3x4_t &transform1, IPhysicsShape *pShape2, const matrix3x4_t &transform2 ) = 0;

	// Takes ( const RnSimplexCache_t &, const RnHull_t *, const RnHull_t * ) and returns the hull feature pair of the cached simplex as a 16-byte struct
	virtual void unk053( void *p ) = 0;

	// Contact generation: the callees add contact points to pOut and the result tells whether there are any.
	// The five form one overload set, which MSVC lists in reverse order. pOut and the other pointers are internal types of the physics module;
	// each float is paired with the pointer that follows it into a { pointer, float } block for the callee
	virtual bool unk054( void *pOut, const void *pUnknown1, const matrix3x4_t &transform1, float flUnknown1, const void *pUnknown2, const matrix3x4_t &transform2, float flUnknown2, const void *pUnknown3, void *pUnknown4, int nUnknown ) = 0;
	virtual bool unk054( void *pOut, const void *pUnknown1, const CTransform &transform1, const void *pUnknown2, int nUnknown1, const CTransform &transform2, const void *pUnknown3, const void *pUnknown4, void *pUnknown5, int nUnknown2 ) = 0;
	virtual bool unk054( void *pOut, const void *pUnknown1, const matrix3x4_t &transform1, float flUnknown1, const void *pUnknown2, int nUnknown1, const matrix3x4_t &transform2, float flUnknown2, const void *pUnknown3, const void *pUnknown4, void *pUnknown5, int nUnknown2 ) = 0;
	virtual bool unk054( void *pOut, const void *pUnknown1, const matrix3x4_t &transform1, const void *pUnknown2, const matrix3x4_t &transform2, float flUnknown1, const void *pUnknown3, int nUnknown, float flUnknown2 ) = 0;
	virtual bool unk054( void *pOut, const void *pUnknown1, const matrix3x4_t &transform1, const void *pUnknown2, const matrix3x4_t &transform2, float flUnknown1, const void *pUnknown3, const void *pUnknown4, void *pUnknown5, float flUnknown2 ) = 0;

	// Convex hull geometry. A NULL pPreallocatedHull allocates a new hull
	virtual RnHull_t *CreateBoxHull( const Vector &vExtent, const Vector &vCenter, RnHull_t *pPreallocatedHull ) = 0;
	virtual RnHull_t *CreateConeHull( const Vector &vStart, const Vector &vEnd, float flRadius, RnHull_t *pPreallocatedHull ) = 0;
	virtual RnHull_t *CreateCylinderHull( const Vector &vStart, const Vector &vEnd, float flRadius, RnHull_t *pPreallocatedHull ) = 0;

	virtual RnHull_t *CreateHull( int nVertexCount, const Vector *pvVertexBase, RnHull_t *pPreallocatedHull, RnHullSimplificationParams_t *pSimplificationParams ) = 0;
	virtual RnHull_t *CreateHull( CMesh *pMesh, RnHull_t *pPreallocatedHull, RnHullSimplificationParams_t *pSimplificationParams ) = 0;

	virtual RnHull_t *CreateRuntimeHull( int nPointCount, const Vector *pPoints ) = 0;
	virtual RnHull_t *CreateRuntimeHullFromPlanes( int nPlaneCount, const RnPlane_t *pPlanes, float flUnknown, const Vector &vUnknown ) = 0;

	virtual RnHull_t *CreateRandomHull( int nPointCount, float flRadius, float flUnknown, RnHull_t *pPreallocatedHull ) = 0;

	virtual void CloneHull( RnHull_t *pDestHull, const RnHull_t *pSourceHull ) = 0;
	virtual void DestroyHull( RnHull_t *pHull ) = 0;

	virtual RnMesh_t *CreateGridMesh( int nHalfExtent, float flSpacing, float flHeight ) = 0;

	// pSimplifyParams is an optional mesh simplification block, its layout is not recovered
	virtual RnMesh_t *CreateMesh( int nTriangleCount, const uint32 *pIndexBase, const uint8 *pMaterialBase, int nVertexCount, const Vector *pVertexBase, RnMesh_t *pPreallocatedMesh, const void *pSimplifyParams ) = 0;
	virtual RnMesh_t *CreateMesh( CMesh *pMesh, RnMesh_t *pPreallocatedMesh, const void *pSimplifyParams ) = 0;

	virtual void CloneMesh( RnMesh_t *pDestMesh, const RnMesh_t *pSourceMesh ) = 0;
	virtual void DestroyMesh( RnMesh_t *pMesh ) = 0;
	virtual void UpdateMeshChangedVertices( RnMesh_t *pMesh ) = 0;

	virtual void UpdateMeshTriangleAABBs( void *p ) = 0;
	virtual void HideMeshTriangles( void *p ) = 0;

	virtual void CreateCompound( void *p ) = 0;
	virtual void CloneCompound( RnCompound_t *pDestCompound, const RnCompound_t *pSourceCompound ) = 0;
	virtual void DestroyCompound( RnCompound_t *pCompound ) = 0;

	// Takes over the aggregate data loaded from a resource block
	virtual CPhysAggregateDataHandle CreateAggregateDataFromEmbeddedDiskData( const EmbeddedPhysicsInfo_t &embeddedPhysicsInfo ) = 0;

	// Moves the cloth data of reallocated aggregate data; the data type manager forwards its reallocation here
	virtual void unk081( const CPhysAggregateData *pOldData, CPhysAggregateData *pNewData ) = 0;

	// Reference counting of the aggregate data, used by CPhysAggregateDataHandle
	virtual void AddDataReference( CPhysAggregateData *pData ) = 0;
	virtual void ReleaseDataReference( CPhysAggregateData *pData ) = 0;

	virtual bool unk084( CUtlVector< PhysicsBodyDef_t > &bodies, const VPhysXAggregateData_t *pAggregateData ) = 0;

	virtual void ConvertBodies( CUtlVector< PhysicsBodyDef_t > &bodies, const VPhysXAggregateData_t *pAggregateData ) = 0;

	virtual void ConvertBody( PhysicsBodyDef_t &body, const VPhysXAggregateData_t *pAggregateData, int nPartIndex ) = 0;

	// Both are called for every body part of loaded aggregate data, unk088 only when unk087 returns true
	virtual bool unk087( VPhysics2ShapeDef_t *pShape ) = 0;
	virtual void unk088( VPhysXBodyPart_t *pPart ) = 0;

	virtual CPhysAggregateDataHandle CreateAggregateDataFromDiskData( VPhysXAggregateData_t *pDiskDataRoot ) = 0;

	// Standalone softbodies
	virtual IPhysicsSoftbody *CreateSoftbody( const CFeModel *pFeModel ) = 0;
	virtual void DestroySoftbody( IPhysicsSoftbody *pSoftbody ) = 0;

	// Softbodies kept in the interface's own softbody list
	virtual IPhysicsSoftbody *CreateListedSoftbody( const CFeModel *pFeModel ) = 0;
	virtual void DestroyListedSoftbody( IPhysicsSoftbody *pSoftbody ) = 0;

	// Both include the softbodies of every world
	virtual int GetSoftbodyCount() = 0;
	virtual int GetSoftbodies( int nFlags, IPhysicsSoftbody **ppSoftbodies ) = 0;

	virtual IIkSolver *CreateIkSolver( void *p ) = 0;
	virtual void DestroyIkSolver( IIkSolver *pSolver ) = 0;

	virtual VPhysContactImpactStrength_t ComputeImpactStrength( const VPhysIntersectionNotification_t &notification ) = 0;

	// Pop from and push to a lock-free free list
	virtual void *unk099( uint32 nUnknown ) = 0;
	virtual void unk100( void *p ) = 0;

	virtual void DestroyTaggedGeometry( uintp nTaggedGeometry ) = 0;

	virtual RnMassProperties_t ComputeMass( const RnSphere_t *pSphere, const RnMaterial_t &material ) = 0;
	virtual RnMassProperties_t ComputeMass( const RnCapsule_t *pCapsule, const RnMaterial_t &material ) = 0;
	virtual RnMassProperties_t ComputeMass( const RnHull_t *pHull, float flScale, const RnMaterial_t &material ) = 0;

	// Loads the physics debug materials on first use; a non-zero nUnknown selects a replacement for the solid and translucent materials
	virtual CWeakHandle< InfoForResourceTypeIMaterial2 > GetWellKnownMaterial( PhysicsDebugMaterialId_t nMaterial, uint32 nUnknown ) = 0;

	virtual IFnPhysicsContact *ConstructFnPhysicsContact( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsAnyJoint *ConstructFnPhysicsAnyJoint( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsWheelJoint *ConstructFnPhysicsWheelJoint( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsWeldJoint *ConstructFnPhysicsWeldJoint( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsPrismaticJoint *ConstructFnPhysicsPrismaticJoint( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsRevoluteJoint *ConstructFnPhysicsRevoluteJoint( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsConicalJoint *ConstructFnPhysicsConicalJoint( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsSpringJoint *ConstructFnPhysicsSpringJoint( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsSphericalJoint *ConstructFnPhysicsSphericalJoint( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsPulleyJoint *ConstructFnPhysicsPulleyJoint( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsGenericJoint *ConstructFnPhysicsGenericJoint( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsSplineJoint *ConstructFnPhysicsSplineJoint( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsAnyShape *ConstructFnPhysicsAnyShape( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsSphereShape *ConstructFnPhysicsSphereShape( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsCapsuleShape *ConstructFnPhysicsCapsuleShape( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsHullShape *ConstructFnPhysicsHullShape( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsMeshShape *ConstructFnPhysicsMeshShape( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsCompoundShape *ConstructFnPhysicsCompoundShape( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsBody *ConstructFnPhysicsBody( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsSoftbody *ConstructFnPhysicsSoftbody( void *pMemory, int nMemorySize ) = 0;
	virtual IFnPhysicsWorld *ConstructFnPhysicsWorld( void *pMemory, int nMemorySize ) = 0;

	// Physics debugger link over the VConsole connection
	virtual bool UpdateConCommParams( PhysicsWorldUpdateConCommParams_t &params ) = 0;
	virtual void SetVConComm( IVConComm *pVConComm ) = 0;
	virtual void RunVConCommFrame() = 0;

	// True while the VConsole link has any state flag set
	virtual bool IsConCommActive() = 0;
	virtual bool StreamToPhysicsDebugger( bool bStart ) = 0;
	virtual bool IsStreamingToPhysicsDebugger() = 0;
	// Both are stubs in release builds
	virtual CSmartPtr< IPhysSerialStreamSnapshot > CreateSerialStreamSnapshot( void *p ) = 0;
	virtual void *unk134() = 0;
	virtual CSmartPtr< IDataDescConvertor > CreateDataDescConvertor( const void *pData ) = 0;

	// Empty in release builds
	virtual void unk136( int nUnknown ) = 0;
	virtual void unk137( void *p ) = 0;

	virtual void PrintInfo( const CCommand &args ) = 0;

	// Debug draws every world (physics_debug_draw)
	virtual void DebugDrawWorlds( ISceneViewDebugOverlays *pDebugOverlays ) = 0;

	virtual bool CreateDebugSceneObject( const DebugSceneObjectParams_t &params, SceneObjectBuffers_t *pOut ) = 0;
	virtual void DestroyDebugSceneObject( CSceneAnimatableObject *pSceneObject ) = 0;
	virtual void UpdateDebugSceneObject( const DebugSceneObjectParams_t &params, void *pUnknown1, void *pUnknown2 ) = 0;

	// Empty
	virtual void unk143() = 0;

	// Copies the frustum into every world for the physics debugger
	virtual void SetVisualDebugCameraHint( int nUnknown, const CFrustum &frustum ) = 0;

	virtual const char *GetMassDescription( float flMass ) = 0;

public:
	// Freed by the destructor, not used otherwise
	CUtlLeanVector< CPhysAggregateData * > m_AggDataRegistry;
};

inline void CPhysAggregateDataHandle::Set( CPhysAggregateData *pData )
{
	if ( pData )
	{
		g_pVPhysics2->AddDataReference( pData );
	}

	Clear();
	m_pData = pData;
}

inline void CPhysAggregateDataHandle::Clear()
{
	if ( m_pData )
	{
		g_pVPhysics2->ReleaseDataReference( m_pData );
	}

	m_pData = nullptr;
}

// The VConsole link of the physics debugger, registered with IVConComm by IVPhysics2::SetVConComm
class CVPhysics2ConComm : public IVConCommDataReceived
{
public:
	bool OnDataReceived( uint32 nMsgType, uint16 nVersion, uint32 nSize, void *pMessage, uint32 nSourceAddress ) override;

	// Link state bits, 0x1000 while streaming to the physics debugger; IsConCommActive tests it for zero
	CInterlockedUInt m_nFlags;

	uint32 m_unk00C;
	int m_unk010;

	// Set to 20 when the state bit 8 is handled
	int m_unk014;
	int m_unk018;

	// Copy of the PhysicsWorldUpdateConCommParams_t passed to IVPhysics2::UpdateConCommParams
	float m_unk01C;

	// Without it, starting a stream prints "Please start Physics Debugger"
	bool m_bPhysicsDebuggerStarted;

	bool m_unk021;

	// 1 streams every world on each RunVConCommFrame
	int m_unk024;
};

// The resource type manager of VPhysXAggregateData_t ('vphys').
// The binaries have two more virtuals in CBaseIntrospectedToKV3TypeManager than the SDK declares, so the overrides
// of this class (the KV3 load, the release of the aggregate data, the reallocation hook that forwards to
// IVPhysics2::unk081) are not declared here
class CPhysDataTypeManager : public CTypesafeIntrospectedToKV3TypeManager< 0x7379687076ull, VPhysXAggregateData_t >
{
};

// The bases of CVPhysics2SurfacePropertyManager named by the run-time type information. The SDK does not declare them
// elsewhere; which of the two declares each member and virtual is not known, all of them are declared in the first one
template < ResourceType_t RESOURCE_TYPE >
class CKV3ResourceTypeManager : public IResourceTypeManager
{
public:
	virtual void unk024( void *p );

	// CVPhysics2SurfacePropertyManager frees the CUtlVector< CPhysSurfaceProperties * > of a loaded resource and its elements here
	virtual void unk025( void *p );

	// Calls unk024
	virtual void unk026( void *p );

	virtual bool unk027();

public:
	// Stored by IResourceTypeManager::Init
	IResourceSystemUtils *m_pResourceSystemUtils;

	// The version the loaded resources must have ("Unsupported version %d for resource '%s' (Expected %d)")
	int m_nResourceVersion;

	// "generic" in CVPhysics2SurfacePropertyManager
	const char *m_unk018;

	uint64 m_unk020[ 2 ];
};

template < ResourceType_t RESOURCE_TYPE >
class CDefaultKV3TransferResourceTypeManager : public CKV3ResourceTypeManager< RESOURCE_TYPE >
{
};

// The resource type manager of the surface properties ('vsurf') and the IPhysSurfacePropertyController of the physics interface
class CVPhysics2SurfacePropertyManager : public CDefaultKV3TransferResourceTypeManager< 0x6672757376ull >, public IPhysSurfacePropertyController
{
public:
	virtual ~CVPhysics2SurfacePropertyManager() {}

	// The binaries list the overriders in this order in the Linux primary vtable
	const CPhysSurfaceProperties *FindSurfaceProperties( CUtlStringToken token, bool *pFound ) const override;
	int GetSurfacePropCount() const override;
	const CPhysSurfaceProperties *GetSurfaceProperty( int nIndex ) const override;
	void GetSurfacePropertiesNames( CUtlVector< CUtlString > *pNames, CUtlVector< CUtlString > *pDescriptions, bool bIncludeHidden ) const override;
	const CPhysSurfaceProperties *GetDefaultSurfaceProperties() const override;
	void InitializeRnMaterialFromSurfaceProperty( const CPhysSurfaceProperties *pProperties, RnMaterial_t *pMaterial ) const override;
	int unk007() const override;

	struct CSurfacePropertiesPair
	{
		const CPhysSurfaceProperties *m_pDescrition;
	};

public:
	// The loaded surface properties resource; its data is a CUtlVector< CPhysSurfaceProperties * >
	ResourceHandle_t m_hSurfaceProperties;

	// FindSurfaceProperties looks the token up here
	CUtlScalarHash< CSurfacePropertiesPair, CPassthroughHash32 > m_surfacePropertiesDescription;

	// Returned by unk007, starts at 1
	int m_unk058;

	const CPhysSurfaceProperties *m_pDefaultSurfaceProperties;
};

// IVPhysics2::GetIntersectionDictionary returns a static instance; InitIntersectionDictionary creates a copy of it
class CIntersectionDictionary : public IPhysIntersectionDictionary
{
public:
	struct KeyedMask_t
	{
		int m_nKey;
		uint64 m_nMask;
	};

	// The three CNameIndex objects (IPhysNameIndex implementations) returned by unk014( 0 ) to unk014( 2 ).
	// Their layout is not recovered; their size differs between the platforms (0x90 on Linux, 0xA8 on Windows)
#ifdef _WIN32
	uint64 m_NameIndices[ 3 ][ 21 ];
#else
	uint64 m_NameIndices[ 3 ][ 18 ];
#endif

	// Read and written by unk016 to unk019
	CUtlVector< KeyedMask_t > m_KeyedMasks;

	// The entry of m_KeyedMasks whose mask unk018 also stores in m_nCurrentMask
	int m_nCurrentKeyedMask;

	// Returned by unk011 and used by unk010
	uint64 m_nCurrentMask;

	// unk012 ORs into it
	uint64 m_unk1E0;

	// Set by unk013
	uint64 m_unk1E8;

	uint16 m_unk1F0;
	uint8 m_unk1F2;

	// CUtlHashtable< CUtlStringToken, 64-byte record > looked up by unk004 and unk005; the record type is not recovered
	uint64 m_unk1F8[ 4 ];
};

class CVPhysics2Interface : public CTier2AppSystem< IVPhysics2 >
{
public:
	// Returns m_SurfacePropertyManager; GetSurfacePropertyController returns its controller base
	virtual CVPhysics2SurfacePropertyManager *GetSurfacePropertyManager() = 0;

	CVPhysics2ConComm m_ConComm;

	// Created by InitIntersectionDictionary
	CIntersectionDictionary *m_pIntersectionDictionary;

	CPhysDataTypeManager m_typeManagerPhysData;
	CVPhysics2SurfacePropertyManager m_SurfacePropertyManager;

	CUtlVector< IVPhysics2World * > m_Worlds;

	// Set when Init installs the resource type managers, cleared by Shutdown
	bool m_bPhysDataTypeManagerInstalled;
	bool m_bSurfacePropertyManagerInstalled;

	// Guards m_Softbodies
	CAtomicMutex m_SoftbodyMutex;

	// The internal softbody objects created by CreateListedSoftbody
	CUtlLeanVector< void * > m_Softbodies;
};

#endif // IVPHYSICS2_H
