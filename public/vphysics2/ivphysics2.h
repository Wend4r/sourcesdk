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
#include "tier0/refcount.h"
#include "appframework/iappsystem.h"
#include "mathlib/vector.h"
#include "tier0/utlvector.h"

class IVPhysics2World;
class IPhysicsShape;
class IPhysicsSoftbody;
class IIkSolver;
class IPhysSurfacePropertyController;
class IPhysIntersectionDictionary;
class IPhysIntersectionController;
class IVConComm;
class IVPhysicsDebugOverlay;
class CCommand;
class CFeModel;
class CGJKProxy;
class CPhysAggregateData;
class CSceneObject;
class CTransform;
class CVPhysSerialStreamSnapshot;
class CVPhysDataDescConvertor;

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

struct matrix3x4_t;
struct Ray_t;
struct RnSphere_t;
struct RnCapsule_t;
struct RnHull_t;
struct RnMesh_t;
struct RnCompound_t;
struct PhysicsBodyDef_t;
struct VPhysXAggregateData_t;
struct DebugSceneObjectParams_t;
struct SceneObjectBuffers_t;

// Result types returned by value. Their layouts are not reconstructed: the shape casts,
// closest point queries and mass property helpers return them through the hidden return
// pointer on both platforms, the linear casts in RAX on Linux (8 bytes: a 3-bit status and
// the fraction)
struct RnShapeCastResult_t;
struct RnLinearCastResult_t;
struct RnClosestPointsResult_t;
struct RnMassProperties_t;
struct RnContactManifold_t;

// Swept transform of a linear cast: start and end position and start and end rotation
// (the rotations are sign-aligned before the cast)
struct RnSweptTransform_t;

// Pointer and element count pair, passed by value
struct RnElementSpan_t
{
	const void *m_pElements;
	size_t m_nCount;
};

//-----------------------------------------------------------------------------
// VPhysics2_Interface_001 (CVPhysics2Interface in vphysics2).
//
// There is no symbol source for this interface in a build close to CS2; names
// come from the Dota 2 (2017 DWARF, 2018 Mach-O) IVPhysics2 / CVPhysics2Interface
// where the CS2 body does the same thing, from the exported Rn* functions the
// thunks call and from strings, and are otherwise named by what the CS2 body does.
// The overload sets are declared in their Linux order; MSVC places each set at
// its first declaration in reverse order, as the Windows binary has it.
// The parameter types of the query overloads (ShapeCast, LinearCast,
// ClosestPoints, Collide, ComputeMassProperties) are inferred from how the
// bodies read them; their slots are verified on both platforms.
//-----------------------------------------------------------------------------
abstract_class IVPhysics2 : public IAppSystem
{
public:
	virtual ~IVPhysics2() {}

	virtual IVPhysics2World *CreateWorld( uint32 nFlags ) = 0;
	virtual void DestroyWorld( IVPhysics2World *pWorld ) = 0;
	virtual int GetWorldCount() = 0;
	virtual IVPhysics2World *GetWorld( int nIndex ) = 0;

	virtual void SimulateWorlds( IVPhysics2World **ppWorlds, int nWorldCount, float flTimeStep, int nSubsteps, bool bAllowThreading, void *pContext ) = 0;

	// Steps the softbodies of all given worlds in parallel jobs; nJobParam goes to the step job
	// together with the time step
	virtual void SimulateSoftbodies( IVPhysics2World **ppWorlds, int nWorldCount, float flTimeStep, int nJobParam ) = 0;

	// Named by what it does: resets the per-step data of every world
	virtual void ResetWorldsStepData() = 0;

	// Rubikon threading and SIMD switches; both are process-wide
	virtual bool IsMultithreadingEnabled() = 0;
	virtual void SetMultithreadingEnabled( bool bEnabled ) = 0;
	virtual bool IsSIMDEnabled() = 0;
	virtual void SetSIMDEnabled( bool bEnabled ) = 0;

	virtual IPhysSurfacePropertyController *GetSurfacePropertyController() = 0;

	virtual void InitIntersectionDictionary() = 0;

	// bReload re-reads the layer lists into the existing dictionary
	virtual void LoadCollisionDetailLayers( bool bReload ) = 0;

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

	// Named by what they do: sweep of the first shape against the second (32 iterations); the
	// second overload takes one more float
	virtual RnShapeCastResult_t ShapeCast( const Vector *pPoints, int nPointCount, int nFlags, const CTransform &xform, const IPhysicsShape *pShape, const Vector &vDelta ) = 0;
	virtual RnShapeCastResult_t ShapeCast( const Vector *pPoints, int nPointCount, int nFlags, const CTransform &xform, const IPhysicsShape *pShape, const Vector &vDelta, float flRadius ) = 0;

	// Dota 2 names (LinearCast): time of impact of two shapes moving along swept transforms
	virtual RnLinearCastResult_t LinearCast( const RnSweptTransform_t &sweepA, IPhysicsShape *pShapeA, const RnSweptTransform_t &sweepB, IPhysicsShape *pShapeB ) = 0;
	virtual RnLinearCastResult_t LinearCast( const RnSweptTransform_t &sweepA, const RnHull_t *pHullA, const RnSweptTransform_t &sweepB, const RnHull_t *pHullB ) = 0;
	virtual RnLinearCastResult_t LinearCast( const RnSweptTransform_t &sweepA, const Ray_t &shapeA, const RnSweptTransform_t &sweepB, const Ray_t &shapeB ) = 0;
	virtual RnLinearCastResult_t LinearCast( const RnSweptTransform_t &sweepA, const CGJKProxy &proxyA, const RnSweptTransform_t &sweepB, const CGJKProxy &proxyB, float flMarginA, float flMarginB ) = 0;

	// Named by what they do: ray casts against a mesh (AABB tree walk, scaled) and against a
	// hull swept by a sphere of flRadius
	virtual bool CastRayAgainstMesh( void *pResult, const RnMesh_t *pMesh, const Vector &vScale, const Vector &vStart, const Vector &vDelta ) = 0;
	virtual bool CastSphereAgainstHull( float flRadius, void *pResult, const RnHull_t *pHull, const Vector &vStart, const Vector &vDelta ) = 0;

	// Traces the ray shape, placed at vStart, against one shape; dispatches on the ray type.
	// Used for hitbox traces
	virtual void TraceRayAgainstShape( void *pResult, const Ray_t &ray, const Vector &vStart, const void *pShape, const CTransform *pShapeTransform, const void *pQueryAttributes ) = 0;

	virtual void TraceShape( void *pResult, const Ray_t &ray, const CTransform *pShapeTransform, const void *pShape, const void *pQueryAttributes, void *pContext ) = 0;

	// Named by what they do: GJK closest points (16 iterations) of a query shape and a shape
	virtual RnClosestPointsResult_t ClosestPoints( const CTransform &xformA, const Ray_t &shapeA, const CTransform &xformB, IPhysicsShape *pShapeB ) = 0;
	virtual RnClosestPointsResult_t ClosestPoints( const CTransform &xformA, const Ray_t &shapeA, const CTransform &xformB, const RnHull_t *pHullB, const Vector &vScaleB ) = 0;
	virtual RnClosestPointsResult_t ClosestPoints( const matrix3x4_t &xformA, int nPointCountA, const matrix3x4_t &xformB, const Vector *pPointsB, int nPointCountB, int nMaxIterations ) = 0;

	// Dota 2 names: distance between two point clouds, in place and transformed (0 when closer
	// than 1/8)
	virtual float GetDistanceBetweenHulls( const Vector *pPointsA, int nPointCountA, const Vector *pPointsB, int nPointCountB ) = 0;
	virtual float GetDistanceBetweenHulls( const matrix3x4_t &xformA, const Vector *pPointsA, int nPointCountA, const matrix3x4_t &xformB, const Vector *pPointsB, int nPointCountB ) = 0;

	// Named by what it does: GJK distance of two shapes minus their convex radii
	virtual float GetDistanceBetweenShapes( IPhysicsShape *pShapeA, const CTransform &xformA, IPhysicsShape *pShapeB, const CTransform &xformB ) = 0;

	// Named by what it does: witness points of a GJK simplex (barycentric weights below 2^-13
	// drop the vertex)
	virtual void ComputeClosestPointsFromSimplex( const void *pSimplex, Vector *pPointA, Vector *pPointB, const void *pProxyA, const void *pProxyB ) = 0;

	// Named by what they do: contact manifolds of capsule-capsule, hull-hull, capsule-hull,
	// sphere-hull and sphere-mesh pairs (the floats are the radii)
	virtual bool Collide( float flRadiusA, float flRadiusB, RnContactManifold_t *pManifold, const RnCapsule_t &capsuleA, const CTransform &xformA, const RnCapsule_t &capsuleB, const CTransform &xformB, float flMargin ) = 0;
	virtual bool Collide( RnContactManifold_t *pManifold, const RnHull_t *pHullA, const CTransform &xformA, const Vector &vScaleA, const RnHull_t *pHullB, const CTransform &xformB, const Vector &vScaleB, float flMargin ) = 0;
	virtual bool Collide( float flRadiusA, float flMargin, RnContactManifold_t *pManifold, const RnCapsule_t &capsuleA, const CTransform &xformA, const RnHull_t *pHullB, const CTransform &xformB, const Vector &vScaleB ) = 0;
	virtual bool Collide( float flRadiusA, RnContactManifold_t *pManifold, const RnSphere_t &sphereA, const CTransform &xformA, const RnHull_t *pHullB, const CTransform &xformB, const Vector &vScaleB ) = 0;
	virtual bool Collide( RnContactManifold_t *pManifold, const RnSphere_t &sphereA, const CTransform &xformA, const RnMesh_t *pMeshB, const CTransform &xformB, const Vector &vScaleB, float flRadiusA ) = 0;

	// Convex hull geometry (RnHullCreate*). A NULL pDestHull allocates a new hull
	virtual RnHull_t *CreateBoxHull( const Vector &vCenter, const Vector &vHalfExtents, RnHull_t *pDestHull ) = 0;
	virtual RnHull_t *CreateConeHull( const Vector &vStart, const Vector &vEnd, float flRadius, RnHull_t *pDestHull ) = 0;
	virtual RnHull_t *CreateCylinderHull( const Vector &vStart, const Vector &vEnd, float flRadius, RnHull_t *pDestHull ) = 0;

	// Both wrap RnHullCreate. The second overload first gathers the points from a strided vertex stream
	virtual RnHull_t *CreateHull( int nPointCount, const Vector *pPoints, RnHull_t *pDestHull, const void *pSimplificationParams ) = 0;
	virtual RnHull_t *CreateHull( const void *pVertexStream, RnHull_t *pDestHull, const void *pSimplificationParams ) = 0;

	virtual RnHull_t *CreateRuntimeHull( int nPointCount, const Vector *pPoints ) = 0;

	// RnHullCreateRuntimeFromPlanes; the point and the float go to the plane clipper
	virtual RnHull_t *CreateRuntimeHullFromPlanes( int nPlaneCount, const void *pPlanes, float flEpsilon, const Vector &vReferencePoint ) = 0;

	// At least 4 points; flMaxVolume caps flRadius^3 / 15.59
	virtual RnHull_t *CreateRandomHull( int nPointCount, float flRadius, float flMaxVolume ) = 0;

	virtual void CloneHull( RnHull_t *pDestHull, const RnHull_t *pSourceHull ) = 0;
	virtual void DestroyHull( RnHull_t *pHull ) = 0;

	virtual RnMesh_t *CreateGridMesh( int nHalfExtent, float flSpacing, float flHeight ) = 0;

	// Both wrap RnMeshCreate. The second overload takes the triangles and vertices from a mesh stream.
	// A NULL pOptions uses the default mesh build options
	virtual RnMesh_t *CreateMesh( int nTriangleCount, const void *pTriangles, const uint8 *pMaterials, int nVertexCount, const Vector *pVertices, RnMesh_t *pDestMesh, const void *pOptions ) = 0;
	virtual RnMesh_t *CreateMesh( const void *pMeshStream, RnMesh_t *pDestMesh, const void *pOptions ) = 0;

	virtual void CloneMesh( RnMesh_t *pDestMesh, const RnMesh_t *pSourceMesh ) = 0;
	virtual void DestroyMesh( RnMesh_t *pMesh ) = 0;
	virtual void UpdateMeshChangedVertices( RnMesh_t *pMesh ) = 0;

	virtual void UpdateMeshTriangleAABBs( RnMesh_t *pMesh, RnElementSpan_t triangles ) = 0;
	// The span holds 32-bit triangle indices
	virtual void HideMeshTriangles( RnMesh_t *pMesh, RnElementSpan_t triangleIndices ) = 0;

	// Wraps the compound builder; the spans are named after the Rubikon child shape kinds and
	// their order is inferred
	virtual RnCompound_t *CreateCompound( RnElementSpan_t spheres, RnElementSpan_t capsules, RnElementSpan_t hulls, RnElementSpan_t meshes, RnElementSpan_t children, void *pResourceStream ) = 0;
	virtual void CloneCompound( RnCompound_t *pDestCompound, const RnCompound_t *pSourceCompound ) = 0;
	virtual void DestroyCompound( RnCompound_t *pCompound ) = 0;

	// Named by what they do: reference counted aggregate data (0xE0 bytes, created from a
	// resource or from VPhysXAggregateData_t), its swap in every world and its reference count
	virtual CRefPtr< CPhysAggregateData > CreateAggregateDataFromResource( const void *pResource ) = 0;
	virtual void ReplaceAggregateData( CPhysAggregateData *pOldData, CPhysAggregateData *pNewData ) = 0;
	virtual void AddRefAggregateData( CPhysAggregateData *pData ) = 0;
	virtual void ReleaseAggregateData( CPhysAggregateData *pData ) = 0;

	// Converts the bodies (see ConvertBodies) and copies the joints, collision attributes and
	// hitbox data of the aggregate ("HITGROUP_INVALID")
	virtual bool ConvertAggregateData( CUtlVector< PhysicsBodyDef_t > &bodies, const VPhysXAggregateData_t *pAggregateData ) = 0;

	virtual void ConvertBodies( CUtlVector< PhysicsBodyDef_t > &bodies, const VPhysXAggregateData_t *pAggregateData ) = 0;

	virtual void ConvertBody( PhysicsBodyDef_t &body, const VPhysXAggregateData_t *pAggregateData, int nPartIndex ) = 0;

	// Named by what they do: both work on the five shape lists of a body part; the first
	// rebuilds them when they hold more than 19 shapes, the second builds an index list over
	// all of them
	virtual bool RebuildLargeShapeLists( void *pShapeLists ) = 0;
	virtual void BuildShapeListIndices( void *pShapeLists ) = 0;

	virtual CRefPtr< CPhysAggregateData > CreateAggregateData( const VPhysXAggregateData_t *pAggregateData ) = 0;

	// Standalone softbodies
	virtual IPhysicsSoftbody *CreateSoftbody( const CFeModel *pFeModel ) = 0;
	virtual void DestroySoftbody( IPhysicsSoftbody *pSoftbody ) = 0;

	// Softbodies kept in the interface's own softbody list
	virtual IPhysicsSoftbody *CreateListedSoftbody( const CFeModel *pFeModel ) = 0;
	virtual void DestroyListedSoftbody( IPhysicsSoftbody *pSoftbody ) = 0;

	virtual int GetSoftbodyCount() = 0;

	// Named by what it does: writes the listed softbodies (bit 0: all, bit 1: skip disabled ones,
	// bit 2: do not lock) to ppSoftbodies and returns their count
	virtual int GetSoftbodies( uint8 nFlags, IPhysicsSoftbody **ppSoftbodies ) = 0;

	virtual IIkSolver *CreateIkSolver( const void *pDesc ) = 0;
	virtual void DestroyIkSolver( IIkSolver *pSolver ) = 0;

	// Named by what it does: a float computed from the two bodies of a joint and its constraint
	// data, 0 when either body is missing
	virtual float ComputeJointError( const void *pJoint ) = 0;

	// Named by what they do: reference counted arrays of nCount 224-byte entries, taken from
	// and returned to per-count lock-free free lists (up to 15 entries)
	virtual void *AllocPooledArray( uint32 nCount ) = 0;
	virtual void FreePooledArray( void *pArray ) = 0;

	virtual void DestroyTaggedGeometry( uintp nTaggedGeometry ) = 0;

	// Named by what they do: mass properties of a sphere, a capsule and a scaled hull (identity
	// inertia for a zero mass)
	virtual RnMassProperties_t ComputeMassProperties( const RnSphere_t &sphere, const float &flMass ) = 0;
	virtual RnMassProperties_t ComputeMassProperties( const RnCapsule_t &capsule, const float &flMass ) = 0;
	virtual RnMassProperties_t ComputeMassProperties( float flScale, const RnHull_t *pHull, const float &flMass ) = 0;

	// Named by what it does: the debug draw material of a mode (1 or 3) and color, created on
	// first use
	virtual void *FindOrCreatePhysicsDebugMaterial( int nMode, uint32 nColor ) = 0;

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

	// Physics debugger link over the VConsole connection. Named by what it does: stores the
	// channel id and drops the link when VConsole is not connected
	virtual bool SetPhysicsDebuggerChannel( const uint32 *pChannelId ) = 0;
	virtual void SetVConComm( IVConComm *pVConComm ) = 0;
	virtual void RunVConCommFrame() = 0;

	// True while the VConsole link has any state flag set (Dota 2: IsConCommActive)
	virtual bool IsConCommActive() = 0;
	virtual bool StreamToPhysicsDebugger( bool bStart ) = 0;
	virtual bool IsStreamingToPhysicsDebugger() = 0;
	virtual CRefPtr< CVPhysSerialStreamSnapshot > CreateSerialStreamSnapshot( const void *pSource ) = 0;

	// Returns NULL in CS2
	virtual CVPhysSerialStreamSnapshot *GetSerialStreamSnapshot() = 0;
	virtual CRefPtr< CVPhysDataDescConvertor > CreateDataDescConvertor( const void *pDataDesc ) = 0;

	// Named by what they do: empty in the release build
	virtual void DebugNotifyInt( int nValue ) = 0;
	virtual void DebugNotifyPointer( void *pValue ) = 0;

	virtual void PrintInfo( const CCommand &args ) = 0;

	// Debug draws every world (physics_debug_draw)
	virtual void DebugDrawWorlds( IVPhysicsDebugOverlay *pOverlay ) = 0;

	// Dota 2 names (CreateDebugSceneObject); the update rebuilds the buffers of an existing one
	virtual CSceneObject *CreateDebugSceneObject( const DebugSceneObjectParams_t &params, SceneObjectBuffers_t *pBuffers ) = 0;
	virtual void DestroyDebugSceneObject( CSceneObject *pSceneObject ) = 0;
	virtual void UpdateDebugSceneObject( const DebugSceneObjectParams_t &params, CSceneObject *pSceneObject, SceneObjectBuffers_t *pBuffers ) = 0;

	// Named by its neighbours: empty in CS2
	virtual void FlushDebugDraw() = 0;

	// Named by what it does: copies the 0x41C-byte block at pParams into every world (the
	// first argument is not read)
	virtual void SetAllWorldsParams( uint32 nParamsId, const void *pParams ) = 0;

	virtual const char *GetMassDescription( float flMass ) = 0;

	// Dota 2 name; returns the dictionary member at +136
	virtual IPhysIntersectionDictionary *GetIntersectionDictionaryInternal() = 0;
};

#endif // IVPHYSICS2_H
