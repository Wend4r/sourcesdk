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
#include "mathlib/vector.h"
#include "tier0/threadtools.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlvector.h"
#include "tier2/tier2.h"

class IVPhysics2World;
class CVPhysics2SurfacePropertyManager;
class IPhysicsShape;
class IPhysicsSoftbody;
class IIkSolver;
class IPhysSurfacePropertyController;
class IPhysIntersectionDictionary;
class IPhysIntersectionController;
class IVConComm;
class CCommand;
class ISceneViewDebugOverlays;
class CMesh;

class IFnPhysicsContact;
class IFnPhysicsAnyJoint;
class IFnPhysicsWheelJoint;
class IFnPhysicsWeldJoint;
class IFnPhysicsPrismaticJoint;
class IFnPhysicsRevoluteJoint;
class IFnPhysicsConicalJoint;
class IFnPhysicsSpringJoint;
class IFnPhysicsSphericalJoint;
class IFnPhysicsPulleyJoint;
class IFnPhysicsGenericJoint;
class IFnPhysicsSplineJoint;
class IFnPhysicsAnyShape;
class IFnPhysicsSphereShape;
class IFnPhysicsCapsuleShape;
class IFnPhysicsHullShape;
class IFnPhysicsMeshShape;
class IFnPhysicsCompoundShape;
class IFnPhysicsBody;
class IFnPhysicsSoftbody;
class IFnPhysicsWorld;

struct RnSphere_t;
struct RnCapsule_t;
struct RnHull_t;
struct RnPlane_t;
struct RnMesh_t;
struct RnCompound_t;
struct PhysicsBodyDef_t;
struct VPhysXAggregateData_t;
struct RnHullSimplificationParams_t;

abstract_class IVPhysics2 : public IAppSystem
{
public:
	virtual ~IVPhysics2() {}

	virtual IVPhysics2World *CreateWorld( uint32 nFlags ) = 0;
	virtual void DestroyWorld( IVPhysics2World *pWorld ) = 0;
	virtual int GetWorldCount() = 0;
	virtual IVPhysics2World *GetWorld( int nIndex ) = 0;

	virtual void SimulateWorlds( IVPhysics2World **ppWorlds, int nWorldCount, float flTimeStep, int nSubsteps, bool bAllowThreading, void *pContext ) = 0;

	// Steps the softbodies of all given worlds in parallel jobs
	virtual void SimulateSoftbodies( IVPhysics2World **ppWorlds, int nWorldCount, float flTimeStep, int nUnknown ) = 0;

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

#ifdef _WIN32
	virtual void unk037( void *p ) = 0;
	virtual void unk038( void *p ) = 0;
#else
	virtual void unk038( void *p ) = 0;
	virtual void unk037( void *p ) = 0;
#endif

#ifdef _WIN32
	virtual void unk039( void *p ) = 0;
	virtual void unk040( void *p ) = 0;
	virtual void unk041( void *p ) = 0;
	virtual void unk042( void *p ) = 0;
#else
	virtual void unk042( void *p ) = 0;
	virtual void unk041( void *p ) = 0;
	virtual void unk040( void *p ) = 0;
	virtual void unk039( void *p ) = 0;
#endif

	virtual void unk043( void *p ) = 0;
	virtual void unk044( void *p ) = 0;

	virtual void unk045( void *p ) = 0;

	virtual void TraceShape( void *p ) = 0;

#ifdef _WIN32
	virtual void unk047( void *p ) = 0;
	virtual void unk048( void *p ) = 0;
	virtual void unk049( void *p ) = 0;
#else
	virtual void unk049( void *p ) = 0;
	virtual void unk048( void *p ) = 0;
	virtual void unk047( void *p ) = 0;
#endif

#ifdef _WIN32
	virtual void unk050( void *p ) = 0;
	virtual void unk051( void *p ) = 0;
#else
	virtual void unk051( void *p ) = 0;
	virtual void unk050( void *p ) = 0;
#endif

	virtual void unk052( void *p ) = 0;
	virtual void unk053( void *p ) = 0;

#ifdef _WIN32
	virtual void unk054( void *p ) = 0;
	virtual void unk055( void *p ) = 0;
	virtual void unk056( void *p ) = 0;
	virtual void unk057( void *p ) = 0;
	virtual void unk058( void *p ) = 0;
#else
	virtual void unk058( void *p ) = 0;
	virtual void unk057( void *p ) = 0;
	virtual void unk056( void *p ) = 0;
	virtual void unk055( void *p ) = 0;
	virtual void unk054( void *p ) = 0;
#endif

	// Convex hull geometry. A NULL pPreallocatedHull allocates a new hull
	virtual RnHull_t *CreateBoxHull( const Vector &vExtent, const Vector &vCenter, RnHull_t *pPreallocatedHull ) = 0;
	virtual RnHull_t *CreateConeHull( const Vector &vStart, const Vector &vEnd, float flRadius, RnHull_t *pPreallocatedHull ) = 0;
	virtual RnHull_t *CreateCylinderHull( const Vector &vStart, const Vector &vEnd, float flRadius, RnHull_t *pPreallocatedHull ) = 0;

	virtual RnHull_t *CreateHull( int nVertexCount, const Vector *pvVertexBase, RnHull_t *pPreallocatedHull, RnHullSimplificationParams_t *pSimplificationParams ) = 0;
	virtual RnHull_t *CreateHull( CMesh *pMesh, RnHull_t *pPreallocatedHull, RnHullSimplificationParams_t *pSimplificationParams ) = 0;

	virtual RnHull_t *CreateRuntimeHull( int nPointCount, const Vector *pPoints ) = 0;
	virtual RnHull_t *CreateRuntimeHullFromPlanes( int nPlaneCount, const RnPlane_t *pPlanes, float flUnknown, const Vector &vUnknown ) = 0;

	virtual RnHull_t *CreateRandomHull( int nPointCount, float flRadius, float flUnknown ) = 0;

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

	virtual void CreateFromResource( void *p ) = 0;

	virtual void unk081( void *p ) = 0;

	virtual void AddRef( void *p ) = 0;
	virtual void Release( void *p ) = 0;

	virtual bool unk084( CUtlVector< PhysicsBodyDef_t > &bodies, const VPhysXAggregateData_t *pAggregateData ) = 0;

	virtual void ConvertBodies( CUtlVector< PhysicsBodyDef_t > &bodies, const VPhysXAggregateData_t *pAggregateData ) = 0;

	virtual void ConvertBody( PhysicsBodyDef_t &body, const VPhysXAggregateData_t *pAggregateData, int nPartIndex ) = 0;

	virtual bool unk087( void *p ) = 0;
	virtual void unk088( void *p ) = 0;

	virtual void Create( void *p ) = 0;

	// Standalone softbodies
	virtual IPhysicsSoftbody *CreateSoftbody( void *p ) = 0;
	virtual void DestroySoftbody( IPhysicsSoftbody *pSoftbody ) = 0;

	// Softbodies kept in the interface's own softbody list
	virtual IPhysicsSoftbody *CreateListedSoftbody( void *p ) = 0;
	virtual void DestroyListedSoftbody( IPhysicsSoftbody *pSoftbody ) = 0;

	// Both include the softbodies of every world
	virtual int GetSoftbodyCount() = 0;
	virtual int GetSoftbodies( int nFlags, IPhysicsSoftbody **ppSoftbodies ) = 0;

	virtual IIkSolver *CreateIkSolver( void *p ) = 0;
	virtual void DestroyIkSolver( IIkSolver *pSolver ) = 0;

	virtual void unk098( void *p ) = 0;

	// Pop from and push to a lock-free free list
	virtual void *unk099( uint32 nUnknown ) = 0;
	virtual void unk100( void *p ) = 0;

	virtual void DestroyTaggedGeometry( uintp nTaggedGeometry ) = 0;

#ifdef _WIN32
	virtual void unk102( void *p ) = 0;
	virtual void unk103( void *p ) = 0;
	virtual void unk104( void *p ) = 0;
#else
	virtual void unk104( void *p ) = 0;
	virtual void unk103( void *p ) = 0;
	virtual void unk102( void *p ) = 0;
#endif

	virtual void LoadPhysicsDebugMaterials( void *p ) = 0;

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
	virtual bool unk127( const uint32 *pUnknown ) = 0;
	virtual void SetVConComm( IVConComm *pVConComm ) = 0;
	virtual void RunVConCommFrame() = 0;

	// True while the VConsole link has any state flag set
	virtual bool unk130() = 0;
	virtual bool StreamToPhysicsDebugger( bool bStart ) = 0;
	virtual bool IsStreamingToPhysicsDebugger() = 0;
	virtual void CreateSerialStreamSnapshot( void *p ) = 0;
	virtual void *unk134() = 0;
	virtual void CreateDataDescConvertor( void *p ) = 0;

	// Empty in release builds
	virtual void unk136( int nUnknown ) = 0;
	virtual void unk137( void *p ) = 0;

	virtual void PrintInfo( const CCommand &args ) = 0;

	// Debug draws every world (physics_debug_draw)
	virtual void DebugDrawWorlds( ISceneViewDebugOverlays *pDebugOverlays ) = 0;

	virtual void CreateDebugSceneObject( void *p ) = 0;
	virtual void DestroyDebugSceneObject( void *pSceneObject ) = 0;
	virtual void UpdateDebugSceneObject( void *p ) = 0;

	// Empty
	virtual void unk143() = 0;

	// Copies a 128-byte block into every world
	virtual void unk144( void *p ) = 0;

	virtual const char *GetMassDescription( float flMass ) = 0;
};

class CVPhysics2Interface : public CTier2AppSystem< IVPhysics2 >
{
public:
	// The surface property manager embedded in this object; GetSurfacePropertyController returns its controller base
	virtual CVPhysics2SurfacePropertyManager *unk146() = 0;

	uint64 m_pad028[ 2 ];

	// CVPhysics2ConComm, the VConsole physics debugger link
	uint64 m_Unk038[ 5 ];

	uint64 m_pad060;

	// CPhysDataTypeManager for VPhysXAggregateData_t
	uint64 m_Unk068[ 4 ];

	// CVPhysics2SurfacePropertyManager, with its IPhysSurfacePropertyController base at 0xB8
	uint64 m_Unk088[ 13 ];

	CUtlVector< IVPhysics2World * > m_Worlds;

	// Set when Init installs the resource type managers, cleared by Shutdown
	bool m_bPhysDataTypeManagerInstalled;
	bool m_bSurfacePropertyManagerInstalled;

	// Guards m_Softbodies
	CAtomicMutex m_SoftbodyMutex;

	// Softbodies created by CreateListedSoftbody
	CUtlLeanVector< void * > m_Softbodies;
};

#endif // IVPHYSICS2_H
