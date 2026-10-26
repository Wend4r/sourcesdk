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
#include "mathlib/fltx4.h"
#include "mathlib/mathlib.h"
#include "mathlib/transform.h"
#include "mathlib/vector.h"
#include "physicslib/fnphysicsjoint.h"
#include "physicslib/fnphysicsshape.h"
#include "physicslib/param_types.h"
#include "physicslib/physaggregatedata.h"
#include "physicslib/serialize.h"
#include "resourcefile/introspectedtokv3typemanager.h"
#include "resourcefile/kv3typemanager.h"
#include "resourcesystem/iresourcesystem.h"
#include "tier0/refcount.h"
#include "tier0/smartptr.h"
#include "tier0/threadtools.h"
#include "tier0/utlhash.h"
#include "tier0/utlhashtable.h"
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

// A quaternion the joint frame orientation getters of IIkSolver return: a 16-byte class with one vector member, returned in
// one vector register on Linux and through a hidden pointer on Windows
struct IkSolverOrientation_t
{
	fltx4 m_qOrientation;
};

// A target of IIkSolver::Solve
struct IkSolverTarget_t
{
	int m_nJoint;
	uint8 m_pad004[ 12 ];

	// The joint is moved towards it; read without the w component
	VectorAligned m_vPosition;

	// Passed to the orientation solve of the joint, which does not read it
	uint8 m_pad020[ 16 ];
};

struct IkSolverTargets_t
{
	int m_nCount;
	IkSolverTarget_t *m_pTargets;
};

// The joint transforms IIkSolver::Solve reads and replaces, indexed like the joints of the solver
struct IkSolverPose_t
{
	uint8 m_pad000[ 8 ];
	CTransform *m_pTransforms;
};

// Created by IVPhysics2::CreateIkSolver; every method forwards to the solver object the implementation owns.
// The solver keeps one record per joint, copied from the creation description; the transform arguments are applied to its values.
// The roles of the joint vectors and of the two joint transforms are not recovered: nothing calls the solver
abstract_class IIkSolver
{
public:
	virtual ~IIkSolver() {}

	virtual int GetJointCount() const = 0;

	// Index of the joint whose transform the joint is solved against
	virtual int GetJointParent( int nJoint ) const = 0;

	// Summed with the parent value as the inverse mass of the joint pair
	virtual float GetInvMass( int nJoint ) const = 0;

	// Principal inverse inertia, rotated into the solved transform of the joint
	virtual Vector GetInvInertia( int nJoint ) const = 0;

	// Mass center of the body of the joint, in its space and placed at the given body transform
	virtual Vector GetLocalMassCenter( int nJoint ) const = 0;
	virtual Vector GetMassCenter( int nJoint, const CTransform &bodyTransform ) const = 0;

	// Copied from the joint description and read by nothing else
	virtual Vector unk007( int nJoint ) const = 0;

	// The reference frame of the joint in the space of the parent body, and placed at the given parent transform
	virtual CTransform GetLocalReferenceFrame( int nJoint ) const = 0;
	virtual CTransform GetReferenceFrame( int nJoint, const CTransform &parentTransform ) const = 0;
	virtual IkSolverOrientation_t GetLocalReferenceOrientation( int nJoint ) const = 0;
	virtual IkSolverOrientation_t GetReferenceOrientation( int nJoint, const IkSolverOrientation_t &parentOrientation ) const = 0;
	virtual Vector GetLocalReferenceOrigin( int nJoint ) const = 0;
	virtual Vector GetReferenceOrigin( int nJoint, const CTransform &parentTransform ) const = 0;

	// The attached frame of the joint in the space of its own body, and placed at the given body transform
	virtual CTransform GetLocalAttachedFrame( int nJoint ) const = 0;
	virtual CTransform GetAttachedFrame( int nJoint, const CTransform &bodyTransform ) const = 0;
	virtual IkSolverOrientation_t GetLocalAttachedOrientation( int nJoint ) const = 0;
	virtual IkSolverOrientation_t GetAttachedOrientation( int nJoint, const IkSolverOrientation_t &bodyOrientation ) const = 0;
	virtual Vector GetLocalAttachedOrigin( int nJoint ) const = 0;
	virtual Vector GetAttachedOrigin( int nJoint, const CTransform &bodyTransform ) const = 0;

	// Limits in radians: the rotation about the z axis of the joint frame is clamped to the twist range,
	// a positive swing limits the rotation away from that axis
	virtual float GetMinTwist( int nJoint ) const = 0;
	virtual float GetMaxTwist( int nJoint ) const = 0;
	virtual float GetMaxSwing( int nJoint ) const = 0;

	// Runs the solver for a number of iterations: every target is applied, then the joints are solved from the last one down to the second
	virtual void Solve( IkSolverPose_t &pose, const IkSolverTargets_t &targets, int nIterations ) = 0;
};

// The 20-byte class IPhysSerialStreamSnapshot::unk005 returns; only its size is known
struct PhysSerialStreamSnapshotUnk005_t
{
	uint8 m_Data[ 20 ];
};

// The 8-byte class IPhysSerialStreamSnapshot::unk007 returns in a register on Linux; only its size is known
struct PhysSerialStreamSnapshotUnk007_t
{
	uint8 m_Data[ 8 ];
};

// Reference counted through CSmartPtr; returned by IVPhysics2::CreateSerialStreamSnapshot
abstract_class IPhysSerialStreamSnapshot
{
public:
	virtual ~IPhysSerialStreamSnapshot() {}

	// Size of the data Serialize writes
	virtual uint32 GetSerializedSize() const = 0;

	// Writes a 0x1D0-byte header followed by the snapshot data and advances pBuffer
	virtual void Serialize( uint8 *&pBuffer ) = 0;

	// The size with the debug data: the header, the keyed records of every type and the string tables of the recorded debug data
	virtual uint32 GetSizeWithDebugData() = 0;

	// Empty in release builds
	virtual void unk004() = 0;

	// Filled with zeros in release builds
	virtual PhysSerialStreamSnapshotUnk005_t unk005() = 0;

	// Returns 0 in release builds
	virtual void *unk006() = 0;

	// Filled with zeros in release builds
	virtual PhysSerialStreamSnapshotUnk007_t unk007() = 0;

	// Copy the pointers of one of the recorded lists: the objects of one of the 24 types, the keys of the serialized records of a
	// type, and the list shared by all types. What the types are is not recovered
	virtual void GetObjects( int nType, CUtlVector< void * > &out ) const = 0;
	virtual void GetRecordKeys( int nType, CUtlVector< void * > &out ) const = 0;
	virtual void GetSharedObjects( CUtlVector< void * > &out ) const = 0;

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

// A joint of physics aggregate data converted from VPhysXJoint_t by IVPhysics2::ConvertAggregateData
struct PhysicsAggregateJointDef_t
{
	PhysicsJointType_t m_nType;

	// -3 when the joint has JOINT_FLAGS_BODY1_FIXED
	int m_nBody1;
	CTransform m_Frame1;
	int m_nBody2;
	CTransform m_Frame2;
	bool m_bEnableCollision;
	CUtlString m_Tag;
};

// The joint friction is VPhysXJoint_t::m_flFriction when it is positive, otherwise it is derived from the maximal force or torque and the mass of m_nBody2
struct PhysicsAggregateSphericalJointDef_t : public PhysicsAggregateJointDef_t
{
	float m_flFriction;
};

struct PhysicsAggregateConicalJointDef_t : public PhysicsAggregateJointDef_t
{
	float m_flFriction;

	// In degrees
	bool m_bEnableSwingLimit;
	float m_flSwingLimit;
	bool m_bEnableTwistLimit;
	float m_flMinTwistAngle;
	float m_flMaxTwistAngle;
};

struct PhysicsAggregateRevoluteJointDef_t : public PhysicsAggregateJointDef_t
{
	float m_flFriction;

	// In degrees
	bool m_bEnableLimit;
	float m_flMinAngle;
	float m_flMaxAngle;

	bool m_bUseBlockSolver;
};

struct PhysicsAggregatePrismaticJointDef_t : public PhysicsAggregateJointDef_t
{
	float m_flFriction;

	bool m_bEnableLimit;
	float m_flMinOffset;
	float m_flMaxOffset;
};

struct PhysicsAggregateWeldJointDef_t : public PhysicsAggregateJointDef_t
{
	float m_flLinearFrequency;
	float m_flLinearDampingRatio;
	float m_flAngularFrequency;
	float m_flAngularDampingRatio;
};

// Filled by IVPhysics2::ConvertAggregateData; the joints are sorted into one list per type
struct PhysicsAggregateDef_t
{
	CUtlVector< PhysicsBodyDef_t > m_Bodies;
	CUtlVector< PhysShapeMarkup_t > m_ShapeMarkups;
	CUtlVector< PhysicsAggregateSphericalJointDef_t > m_SphericalJoints;

	// Both QUAT_ORTHOTWIST_JOINT and CONICAL_JOINT joints, converted to CONICAL_JOINT
	CUtlVector< PhysicsAggregateConicalJointDef_t > m_ConicalJoints;
	CUtlVector< PhysicsAggregateRevoluteJointDef_t > m_RevoluteJoints;
	CUtlVector< PhysicsAggregatePrismaticJointDef_t > m_PrismaticJoints;
	CUtlVector< PhysicsAggregateWeldJointDef_t > m_WeldJoints;
	CUtlVector< PhysicsAggregateJointDef_t > m_NullJoints;
};

enum RnHullFeatureType_t
{
	RN_HULL_FEATURE_VERTEX = 0,
	RN_HULL_FEATURE_EDGE,
	RN_HULL_FEATURE_FACE,
};

// A feature of a hull and its index
struct RnHullFeature_t
{
	RnHullFeatureType_t m_nType;
	int m_nIndex;
};

// The features of two hulls, returned in two integer registers on Linux
struct RnHullFeaturePair_t
{
	RnHullFeature_t m_Feature1;
	RnHullFeature_t m_Feature2;
};

// Flags of IVPhysics2::GetSoftbodies
#define PHYSICS_GET_SOFTBODIES_ACTIVE ( 1 << 0 ) // Only the active softbodies (IPhysicsSoftbody::IsActive)
#define PHYSICS_GET_SOFTBODIES_SKIP_MANUAL_STEP ( 1 << 1 ) // Leaves out the softbodies with IPhysicsSoftbody::SetManualStep
#define PHYSICS_GET_SOFTBODIES_WORLDS_ONLY ( 1 << 2 ) // Leaves out the softbodies created by CreateListedSoftbody

abstract_class IVPhysics2 : public IAppSystem
{
public:
	virtual ~IVPhysics2() {}

	// nFlags takes PHYSICS_WORLD_* flags
	virtual IVPhysics2World *CreateWorld( uint32 nFlags ) = 0;
	virtual void DestroyWorld( IVPhysics2World *pWorld ) = 0;
	virtual int GetWorldCount() = 0;
	virtual IVPhysics2World *GetWorld( int nIndex ) = 0;

	virtual void SimulateWorlds( IVPhysics2World **ppWorlds, int nWorldCount, float flTimeStep, int nSubsteps, bool bAllowThreading, VPhysXWorldStepResults *pResults ) = 0;

	// Steps the softbodies of all given worlds in parallel jobs, nSubsteps times flSubstepTime each
	virtual void SimulateSoftbodies( IVPhysics2World **ppWorlds, int nWorldCount, float flSubstepTime, int nSubsteps ) = 0;

	// Calls IVPhysics2World::FlushDebugRecording( false ) on every world
	virtual void FlushDebugRecordings() = 0;

	// Rubikon threading and SIMD switches; both are process-wide
	virtual bool IsMultithreadingEnabled() = 0;
	virtual void SetMultithreadingEnabled( bool bEnabled ) = 0;
	virtual bool IsSIMDEnabled() = 0;
	virtual void SetSIMDEnabled( bool bEnabled ) = 0;

	virtual IPhysSurfacePropertyController *GetSurfacePropertyController() = 0;

	virtual void InitIntersectionDictionary() = 0;

	virtual void LoadCollisionDetailLayers( bool bCheckForChanges ) = 0;

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
	virtual RnLinearCastResult_t LinearCast( const CTransform &transform1, const CGJKProxy &proxy1, const Vector &vTranslation1, const CTransform &transform2, const CGJKProxy &proxy2, const Vector &vTranslation2, float flUnused, float flRadius ) = 0;
	virtual RnLinearCastResult_t LinearCast( const CTransform &transform1, const CGJKProxy &proxy1, float flRadius1, const Vector &vTranslation1, const CTransform &transform2, const CGJKProxy &proxy2, float flRadius2, const Vector &vTranslation2 ) = 0;

	// Time of impact of two shapes moving between the placements of their swept transforms
	virtual RnConvexCastResult_t ConvexCast( const RnSweptTransform_t &sweep1, IPhysicsShape *pShape1, const RnSweptTransform_t &sweep2, IPhysicsShape *pShape2 ) = 0;
	virtual RnConvexCastResult_t ConvexCast( const RnSweptTransform_t &sweep1, const RnHull_t *pHull1, const RnSweptTransform_t &sweep2, const RnHull_t *pHull2 ) = 0;
	virtual RnConvexCastResult_t ConvexCast( const RnSweptTransform_t &sweep1, const Ray_t &shape1, const RnSweptTransform_t &sweep2, const Ray_t &shape2 ) = 0;
	virtual RnConvexCastResult_t ConvexCast( const RnSweptTransform_t &sweep1, const CGJKProxy &proxy1, float flRadius1, const RnSweptTransform_t &sweep2, const CGJKProxy &proxy2, float flRadius2 ) = 0;

	// Cast vStart + vDelta against a scaled mesh or hull
	virtual bool CastRayAgainstMesh( CShapeCastResult *pResult, const RnMesh_t *pMesh, const Vector &vScale, const Vector &vStart, const Vector &vDelta ) = 0;
	virtual bool CastRayAgainstHull( CShapeCastResult *pResult, const RnHull_t *pHull, float flScale, const Vector &vStart, const Vector &vDelta ) = 0;

	// Cast a shape moved by vDelta from vStart against another shape placed at shapeTransform
	virtual bool CastShapeAgainstShape( PhysicsTrace_t *pTrace, const Ray_t &ray, const Vector &vStart, const Vector &vDelta, const Ray_t &shape, const CTransform &shapeTransform ) = 0;
	virtual bool TraceShape( PhysicsTrace_t *pTrace, const Ray_t &ray, const Vector &vStart, const Vector &vDelta, const Ray_t &shape, const CTransform &shapeTransform ) = 0;

	virtual RnDistanceQueryResult_t ClosestPoints( const CTransform &transform1, IPhysicsShape *pShape1, const CTransform &transform2, IPhysicsShape *pShape2 ) = 0;
	virtual RnDistanceQueryResult_t ClosestPoints( const CTransform &transform1, const RnHull_t *pHull1, const CTransform &transform2, const RnHull_t *pHull2, RnSimplexCache_t *pCache ) = 0;
	virtual RnDistanceQueryResult_t ClosestPoints( const matrix3x4_t &transform1, const CGJKProxy &proxy1, const matrix3x4_t &transform2, const CGJKProxy &proxy2, RnSimplexCache_t *pCache, int nMaxIterations ) = 0;

	// Clamped to zero below a small margin
	virtual float GetDistanceBetweenHulls( const Vector *pVertices1, int nVertexCount1, const Vector *pVertices2, int nVertexCount2 ) = 0;
	virtual float GetDistanceBetweenHulls( const matrix3x4_t &transform1, const Vector *pVertices1, int nVertexCount1, const matrix3x4_t &transform2, const Vector *pVertices2, int nVertexCount2 ) = 0;

	// Distance between the two shapes minus their radii
	virtual float GetShapeDistance( IPhysicsShape *pShape1, const matrix3x4_t &transform1, IPhysicsShape *pShape2, const matrix3x4_t &transform2 ) = 0;

	// Drops the simplex vertices with tiny weights and returns the feature of each hull its remaining vertices span
	virtual RnHullFeaturePair_t GetSimplexFeatures( const RnSimplexCache_t &cache, const RnHull_t *pHull1, const RnHull_t *pHull2 ) = 0;

	// Contact generation: the callees add contact points to pOut and the result tells whether there are any.
	// The five form one overload set, which MSVC lists in reverse order. pOut and the other pointers are internal types of the physics module;
	// each float is paired with the pointer that follows it into a { pointer, float } block for the callee
	virtual bool GenerateContacts( void *pOut, const void *pUnknown1, const matrix3x4_t &transform1, float flUnknown1, const void *pUnknown2, const matrix3x4_t &transform2, float flUnknown2, const void *pUnknown3, void *pUnknown4, int nUnknown ) = 0;
	virtual bool GenerateContacts( void *pOut, const void *pUnknown1, const CTransform &transform1, const void *pUnknown2, int nUnknown1, const CTransform &transform2, const void *pUnknown3, const void *pUnknown4, void *pUnknown5, int nUnknown2 ) = 0;
	virtual bool GenerateContacts( void *pOut, const void *pUnknown1, const matrix3x4_t &transform1, float flUnknown1, const void *pUnknown2, int nUnknown1, const matrix3x4_t &transform2, float flUnknown2, const void *pUnknown3, const void *pUnknown4, void *pUnknown5, int nUnknown2 ) = 0;
	virtual bool GenerateContacts( void *pOut, const void *pUnknown1, const matrix3x4_t &transform1, const void *pUnknown2, const matrix3x4_t &transform2, float flUnknown1, const void *pUnknown3, int nUnknown, float flUnknown2 ) = 0;
	virtual bool GenerateContacts( void *pOut, const void *pUnknown1, const matrix3x4_t &transform1, const void *pUnknown2, const matrix3x4_t &transform2, float flUnknown1, const void *pUnknown3, const void *pUnknown4, void *pUnknown5, float flUnknown2 ) = 0;

	// Convex hull geometry. A NULL pPreallocatedHull allocates a new hull
	virtual RnHull_t *CreateBoxHull( const Vector &vExtent, const Vector &vCenter, RnHull_t *pPreallocatedHull ) = 0;
	virtual RnHull_t *CreateConeHull( const Vector &vStart, const Vector &vEnd, float flRadius, RnHull_t *pPreallocatedHull ) = 0;
	virtual RnHull_t *CreateCylinderHull( const Vector &vStart, const Vector &vEnd, float flRadius, RnHull_t *pPreallocatedHull ) = 0;

	virtual RnHull_t *CreateHull( int nVertexCount, const Vector *pvVertexBase, RnHull_t *pPreallocatedHull, RnHullSimplificationParams_t *pSimplificationParams ) = 0;
	virtual RnHull_t *CreateHull( CMesh *pMesh, RnHull_t *pPreallocatedHull, RnHullSimplificationParams_t *pSimplificationParams ) = 0;

	virtual RnHull_t *CreateRuntimeHull( int nPointCount, const Vector *pPoints ) = 0;
	virtual RnHull_t *CreateRuntimeHullFromPlanes( int nPlaneCount, const RnPlane_t *pPlanes, float flTolerance, const Vector &vInteriorPoint ) = 0;

	virtual RnHull_t *CreateRandomHull( int nPointCount, float flRadius, float flMaxOffset, RnHull_t *pPreallocatedHull ) = 0;

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
	virtual void OnReallocateAggregateData( const CPhysAggregateData *pOldData, CPhysAggregateData *pNewData ) = 0;

	// Reference counting of the aggregate data, used by CPhysAggregateDataHandle
	virtual void AddDataReference( CPhysAggregateData *pData ) = 0;
	virtual void ReleaseDataReference( CPhysAggregateData *pData ) = 0;

	// Converts the bodies, the shape markups and the joints; false without aggregate data
	virtual bool ConvertAggregateData( PhysicsAggregateDef_t &aggregate, const VPhysXAggregateData_t *pAggregateData ) = 0;

	virtual void ConvertBodies( CUtlVector< PhysicsBodyDef_t > &bodies, const VPhysXAggregateData_t *pAggregateData ) = 0;

	virtual void ConvertBody( PhysicsBodyDef_t &body, const VPhysXAggregateData_t *pAggregateData, int nPartIndex ) = 0;

	// Called for every body part of aggregate data without packed compounds and shape markups, before setting m_bCompoundsPacked.
	// PackCompounds groups the shapes of a part without compounds and with at least 20 shapes into compounds and returns true when
	// it did; the part's m_CollisionAttributeIndices are then rebuilt from the collision attribute indices of its shapes
	virtual bool PackCompounds( VPhysics2ShapeDef_t *pShape ) = 0;
	virtual void UpdateCollisionAttributeIndices( VPhysXBodyPart_t *pPart ) = 0;

	virtual CPhysAggregateDataHandle CreateAggregateDataFromDiskData( VPhysXAggregateData_t *pDiskDataRoot ) = 0;

	// Standalone softbodies
	virtual IPhysicsSoftbody *CreateSoftbody( const CFeModel *pFeModel ) = 0;
	virtual void DestroySoftbody( IPhysicsSoftbody *pSoftbody ) = 0;

	// Softbodies kept in the interface's own softbody list
	virtual IPhysicsSoftbody *CreateListedSoftbody( const CFeModel *pFeModel ) = 0;
	virtual void DestroyListedSoftbody( IPhysicsSoftbody *pSoftbody ) = 0;

	// Both include the softbodies of every world; GetSoftbodies takes PHYSICS_GET_SOFTBODIES_* flags
	virtual int GetSoftbodyCount() = 0;
	virtual int GetSoftbodies( int nFlags, IPhysicsSoftbody **ppSoftbodies ) = 0;

	virtual IIkSolver *CreateIkSolver( void *p ) = 0;
	virtual void DestroyIkSolver( IIkSolver *pSolver ) = 0;

	virtual VPhysContactImpactStrength_t ComputeImpactStrength( const VPhysIntersectionNotification_t &notification ) = 0;

	// Pop from and push to a lock-free free list
	virtual void *AllocateContactBlock( uint32 nCount ) = 0;
	virtual void FreeContactBlock( void *p ) = 0;

	// Destroys the hull, mesh or compound by the PHYSICS_TAGGED_GEOMETRY_* tag of the pointer, see IFnPhysicsShape::GetTaggedGeometry
	virtual void DestroyTaggedGeometry( uintp nTaggedGeometry ) = 0;

	virtual RnMassProperties_t ComputeMass( const RnSphere_t *pSphere, const RnMaterial_t &material ) = 0;
	virtual RnMassProperties_t ComputeMass( const RnCapsule_t *pCapsule, const RnMaterial_t &material ) = 0;
	virtual RnMassProperties_t ComputeMass( const RnHull_t *pHull, float flScale, const RnMaterial_t &material ) = 0;

	// Loads the physics debug materials on first use; a non-zero nMaterialVariant selects a replacement for the solid and translucent materials
	virtual CWeakHandle< InfoForResourceTypeIMaterial2 > GetWellKnownMaterial( PhysicsDebugMaterialId_t nMaterial, uint32 nMaterialVariant ) = 0;

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
	virtual void SetVisualDebugCameraHint( int nUnused, const CFrustum &frustum ) = 0;

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

// CVPhysics2ConComm::m_nFlags: the link state and the messages it sends to the physics debugger on the next world frame.
// Starting a stream and the debugger messages also set bit 1 and bit 4, which nothing reads
#define PHYSICS_CONCOMM_SEND_WORLD_INFO_FIELDS ( 1 << 1 ) // The field names of the world info ("Id", "Create Flags", ...)
#define PHYSICS_CONCOMM_SEND_WORLD_INFO ( 1 << 3 ) // The world info (create flags, aggregate and motion controller counts), sent every frame while set
#define PHYSICS_CONCOMM_SEND_DEBUG_DRAW_OPTIONS ( 1 << 4 ) // The debug draw options of the world, once
#define PHYSICS_CONCOMM_WORLDS_CHANGED ( 1 << 6 ) // A world was created or destroyed; the world list is sent again
#define PHYSICS_CONCOMM_STREAMING ( 1 << 12 ) // Streaming to the physics debugger
#define PHYSICS_CONCOMM_SEND_OBJECT_COUNTS ( 1 << 14 ) // The body, softbody, joint and contact counts, sent every frame while set

// The VConsole link of the physics debugger, registered with IVConComm by IVPhysics2::SetVConComm
class CVPhysics2ConComm : public IVConCommDataReceived
{
public:
	bool OnDataReceived( uint32 nMsgType, uint16 nVersion, uint32 nSize, void *pMessage, uint32 nSourceAddress ) override;

	// PHYSICS_CONCOMM_* bits; IsConCommActive tests them for zero
	CInterlockedUInt m_nFlags;

	// PHYSICS_DEBUGGER_FLAG_* flags the debugger messages set; copied to every world while streaming
	uint32 m_nDebuggerFlags;

	// The stream id of the start stream message, 0x8000 by default; the stop message sends it back
	int m_nStreamId;

	// Payload sizes of the world info and the frame messages, the first computed (as 20) on its first send
	int m_nWorldInfoMessageSize;
	int m_nFrameMessageSize;

	// Copy of the PhysicsWorldUpdateConCommParams_t passed to IVPhysics2::UpdateConCommParams, -1 by default; nothing reads it
	float m_flUpdateParams;

	// Without it, starting a stream prints "Please start Physics Debugger"
	bool m_bPhysicsDebuggerStarted;

	// Set while VConsole is connected; losing the connection clears the debug draw filters of the worlds
	bool m_bConnected;

	// 1 (the default, and a start stream message without flag 2) streams every world on each RunVConCommFrame
	int m_nStreamAllWorlds;
};

// The resource type manager of VPhysXAggregateData_t ('vphys').
// Its overrides (the typesafe allocation and release of the aggregate data, and the reallocation hook after them that
// forwards to IVPhysics2::OnReallocateAggregateData) are not declared here
class CPhysDataTypeManager : public CTypesafeIntrospectedToKV3TypeManager< RESOURCE_TYPE_PHYSICS_DATA, VPhysXAggregateData_t >
{
};

// The resource type manager of the surface properties ('vsurf') and the IPhysSurfacePropertyController of the physics interface
class CVPhysics2SurfacePropertyManager : public CDefaultKV3TransferResourceTypeManager< RESOURCE_TYPE_SURFACE_PROPERTIES >, public IPhysSurfacePropertyController
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
	int GetSurfacePropertiesLoadCount() const override;

	struct CSurfacePropertiesPair
	{
		const CPhysSurfaceProperties *m_pDescrition;
	};

public:
	// The loaded surface properties resource; its data is a CUtlVector< CPhysSurfaceProperties * >
	ResourceHandle_t m_hSurfaceProperties;

	// FindSurfaceProperties looks the token up here
	CUtlScalarHash< CSurfacePropertiesPair, CPassthroughHash32 > m_surfacePropertiesDescription;

	// Incremented each time the surface properties resource is allocated, starts at 1
	int m_nLoadCount;

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

	// The three CNameIndex objects (IPhysNameIndex implementations) returned by GetNameIndex for the IntersectionNameIndex_t values.
	// Their layout is not recovered; their size differs between the platforms (0x90 on Linux, 0xA8 on Windows)
#ifdef _WIN32
	uint64 m_NameIndices[ 3 ][ 21 ];
#else
	uint64 m_NameIndices[ 3 ][ 18 ];
#endif

	// Read and written by FindInteractionMask, GetInteractionMask, SetInteractionMask and AddInteractionMask
	CUtlVector< KeyedMask_t > m_KeyedMasks;

	// The entry of m_KeyedMasks whose mask SetInteractionMask also stores in m_nCurrentMask
	int m_nCurrentKeyedMask;

	// Returned by GetDefaultInteractionMask and used by InitDefaultCollisionAttr
	uint64 m_nCurrentMask;

	// AddOverrideMask ORs into it
	uint64 m_nOverrideMask;

	// Set by SetExclusiveOverrideMask
	uint64 m_nExclusiveOverrideMask;

	// A bit for each detail layer whose CollisionDetailLayerInfo_t is query only
	uint16 m_nQueryOnlyDetailLayerMask;

	// Set once the collision detail layers are loaded, cleared by the reset; only copied otherwise
	bool m_bDetailLayersLoaded;

	// Read by HasDetailLayerInfos, FindDetailLayerInfo and GetDetailLayerInfos
	CUtlHashtable< CUtlStringToken, CollisionDetailLayerInfo_t > m_DetailLayerInfos;
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
