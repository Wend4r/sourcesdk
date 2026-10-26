//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: Function interfaces over a shape. IVPhysics2::ConstructFnPhysics*Shape
//          builds one in a caller-provided buffer and SetShape binds it to a shape;
//          the typed interfaces only bind shapes of their own type
//
//===========================================================================//

#ifndef FNPHYSICSSHAPE_H
#define FNPHYSICSSHAPE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "mathlib/vector4d.h"
#include "physicslib/param_types.h"
#include "physicslib/serialize.h"

#include <functional>

class CBufferString;
class CRnShape;
class IRnDebugShape;
class IPhysicsShape;

// The zeroed 40-byte struct IFnPhysicsShape::unk019 returns; only its size and 8-byte alignment are known (fields at +16, +24 and +32)
struct FnPhysicsShapeUnk019_t
{
	ALIGN8 uint8 m_Data[ 40 ] ALIGN8_POST;
};

// The 4-byte class IFnPhysicsShape::unk033 returns in an integer register on Linux; only its size is known
struct FnPhysicsShapeUnk033_t
{
	uint8 m_Data[ 4 ];
};

// A vertex of the triangulated surface of a shape
struct PhysicsShapeVertex_t
{
	Vector m_vPosition;
	Vector m_vNormal;
};

abstract_class IFnPhysicsShape
{
public:
	// MSVC groups these overloads in reverse order
	virtual void SetShape( IPhysicsShape *pShape ) = 0;
	virtual void SetShape( CRnShape *pShape ) = 0;

	virtual IPhysicsShape *GetShape() const = 0;

	// Triangulates the surface of the shape (spheres and capsules with a fixed tessellation) and appends the vertices, three indices per
	// triangle and, when pSubShapes is given, the sub-shape of each triangle. MSVC groups these overloads in reverse order
	virtual void Triangulate( CUtlVector< PhysicsShapeVertex_t > &vertices, CUtlVector< int > &indices, CUtlVector< uint64 > *pSubShapes ) = 0;
	virtual void Triangulate( CUtlVector< Vector > &vertices, CUtlVector< int > &indices, CUtlVector< uint64 > *pSubShapes ) = 0;

	// Calls the callback with every sub-shape: -1 for a sphere, a capsule or a hull, each triangle of a mesh and each child of a compound
	virtual void ForEachSubShape( const std::function< void( uint64 nSubShape ) > &callback ) const = 0;

	// Calls the callback of the type of the shape, or of each child of a compound, with the geometry, the scale and the sub-shape
	virtual void ForEachGeometry( const std::function< void( const RnSphere_t &sphere, int nSubShape ) > &sphereCallback, const std::function< void( const RnCapsule_t &capsule, int nSubShape ) > &capsuleCallback,
		const std::function< void( const RnHull_t *pHull, float flScale, int nSubShape ) > &hullCallback, const std::function< void( const RnMesh_t *pMesh, const Vector &vScale, int nSubShape ) > &meshCallback ) const = 0;

	// Calls the callback of the type of the geometry of a sub-shape (the scaled child of a compound); false for the sub-shape -1 of a compound
	virtual bool VisitSubShapeGeometry( int nSubShape, const std::function< void( const RnSphere_t &sphere ) > &sphereCallback, const std::function< void( const RnCapsule_t &capsule ) > &capsuleCallback,
		const std::function< void( const RnHull_t *pHull, float flScale ) > &hullCallback, const std::function< void( const RnMesh_t *pMesh, const Vector &vScale ) > &meshCallback ) const = 0;

	// One for a sphere, two for a capsule, the vertices of a hull or a mesh
	virtual int GetVertexCount() const = 0;

	// Debug text of the collision group and interacts-as overrides
	virtual void DescribeCollisionOverrides( CBufferString &buffer ) const = 0;
	virtual uint8 GetCollisionGroupOverride( bool *pbHasOverride ) const = 0;

	// Interacts-as layers with the override applied
	virtual uint64 GetInteractsAs() const = 0;

	// Refreshes the broadphase pairs and the contacts of the shape
	virtual void RefilterContacts() = 0;

	// The low half selects a compound child and the high half a mesh triangle; -1 means none
	virtual const RnMaterial_t *GetSubShapeMaterial( uint64 nSubShape ) const = 0;
	virtual const RnMaterial_t *GetMaterial() const = 0;

	// True when the material of the shape is not the one of its geometry
	virtual bool HasMaterialOverride() const = 0;

	// Same type, collision group, interacts-as and interacts-with layers
	virtual bool HasSameCollisionFilter( IPhysicsShape *pOther ) const = 0;

	// The geometry pointer tagged in its low bits like IVPhysics2::DestroyTaggedGeometry takes it: a hull, a mesh | 1,
	// a compound | 2, or 3 for a sphere or a capsule
	virtual uintp GetTaggedGeometry() const = 0;

	// Empty
	virtual void unk018() = 0;

	// Returns zeros
	virtual FnPhysicsShapeUnk019_t unk019() = 0;

	// Debug shape of the shape, created on demand with vColor as its color 0
	virtual IRnDebugShape *CreateDebugShape( const Vector4D &vColor ) = 0;
	virtual void DestroyDebugShape() = 0;

	// The debug shape scene object is not disabled
	virtual bool IsDebugShapeVisible() = 0;

	// Keeps the debug shape enabled when its alpha factors fade it out; true creates the debug shape
	virtual void SetDebugShapeAlwaysVisible( bool bAlwaysVisible ) = 0;

	// Selects the depth-tested debug materials instead of the no-depth-test ones; true creates the debug shape
	virtual void SetDebugShapeDepthTest( bool bDepthTest ) = 0;

	// Sets the scene object flag of moving objects on the debug shape; adding the shape to a body passes whether the body is dynamic
	virtual void SetDebugShapeDynamic( bool bDynamic ) = 0;

	// Color 0 of the debug shape, which CreateDebugShape also takes; its tint and the alpha factors multiply it
	virtual void SetDebugShapeColor( const Vector4D &vColor ) = 0;

	// Four factors (index 0 to 3) whose product hides the debug shape at zero and selects the translucent materials below one
	virtual float GetDebugShapeAlphaFactor( int nIndex ) = 0;
	virtual void SetDebugShapeAlphaFactor( int nIndex, float flValue ) = 0;

	// Color 1 of the debug shape, multiplied with its color
	virtual void SetDebugShapeTint( const Vector4D &vTint ) = 0;

	// Passed to the debug materials; at 0.125 or less the debug shape is hidden unless it is always visible
	virtual void SetDebugShapeOpacity( float flOpacity ) = 0;

	virtual CRnShape *GetRnShape() const = 0;

	// A unique number taken from a global counter when the shape is created or its geometry changes
	virtual int GetShapeId() const = 0;

	// Always zero
	virtual FnPhysicsShapeUnk033_t unk033() = 0;

	// Name of the tool material with this token from the tools material list, an empty string for 0
	virtual const char *GetToolMaterialName( uint32 nToken ) const = 0;
};

abstract_class IFnPhysicsAnyShape : public IFnPhysicsShape
{
};

abstract_class IFnPhysicsSphereShape : public IFnPhysicsShape
{
public:
	// The sphere with the scale applied, the sphere it is built from and the scale
	virtual const RnSphere_t &GetSphere() const = 0;
	virtual const RnSphere_t &GetUnscaledSphere() const = 0;
	virtual float GetScale() const = 0;

	// bUpdateMass recomputes the mass properties of the body
	virtual void SetSphere( const RnSphere_t &sphere, bool bUpdateMass ) = 0;
	virtual void SetUnscaledSphere( const RnSphere_t &sphere, bool bUpdateMass ) = 0;
};

abstract_class IFnPhysicsCapsuleShape : public IFnPhysicsShape
{
public:
	// The capsule with the scale applied, the capsule it is built from and the scale
	virtual const RnCapsule_t &GetCapsule() const = 0;
	virtual const RnCapsule_t &GetUnscaledCapsule() const = 0;
	virtual float GetScale() const = 0;

	// bUpdateMass recomputes the mass properties of the body
	virtual void SetCapsule( const RnCapsule_t &capsule, bool bUpdateMass ) = 0;
	virtual void SetUnscaledCapsule( const RnCapsule_t &capsule, bool bUpdateMass ) = 0;
};

abstract_class IFnPhysicsHullShape : public IFnPhysicsShape
{
public:
	virtual const RnHull_t *GetHull() const = 0;

	// bUpdateMass recomputes the mass properties of the body
	virtual void SetHull( const RnHull_t *pHull, bool bUpdateMass ) = 0;

	// Updates the shape after its vertices changed in place, waking the body to update its mass properties when bUpdateMass is set
	virtual void UpdateVertices( bool bUpdateMass ) = 0;

	virtual float GetScale() const = 0;

	// Casts a sphere of flRadius from vStart along vDelta against the hull, up to flMaxFraction; true on a hit. CastSphere uses the
	// face clipping cast only when the hull has its face tree and the GJK cast otherwise, CastSphereClipped always tries the face
	// clipping cast first, CastSphereBounded rejects casts that miss the bounding sphere of the hull
	virtual bool CastSphere( CShapeCastResult *pResult, const Vector &vStart, const Vector &vDelta, float flRadius, float flMaxFraction ) = 0;
	virtual void CastSphereBounded( CShapeCastResult *pResult, const Vector &vStart, const Vector &vDelta, float flRadius ) = 0;
	virtual bool CastSphereClipped( CShapeCastResult *pResult, const Vector &vStart, const Vector &vDelta, float flRadius, float flMaxFraction ) = 0;

	// The same face clipping cast against another hull, ignoring the bound shape
	virtual bool CastSphereAgainstHull( CShapeCastResult *pResult, const RnHull_t *pHull, const Vector &vStart, const Vector &vDelta, float flScale, float flRadius, float flMaxFraction ) = 0;
};

// The vertex velocities IFnPhysicsMeshShape::UpdateVertices takes by value
struct PhysicsMeshVertexVelocities_t
{
	const Vector *m_pVelocities;
	intp m_nCount;
};

abstract_class IFnPhysicsMeshShape : public IFnPhysicsShape
{
public:
	virtual const RnMesh_t *GetMesh() const = 0;

	// bUpdateMass recomputes the mass properties of the body
	virtual void SetMesh( const RnMesh_t *pMesh, bool bUpdateMass ) = 0;

	// Updates the shape after its vertices changed in place, waking the body to update its mass properties when bUpdateMass is set.
	// The second overload also stores a velocity for each vertex, which contacts on the triangles use, when the count matches the
	// vertex count and a velocity is not zero; nMode 1 needs a mesh flagged for it. MSVC groups these overloads in reverse order
	virtual void UpdateVertices( bool bUpdateMass ) = 0;
	virtual void UpdateVertices( PhysicsMeshVertexVelocities_t velocities, int nMode ) = 0;

	virtual void ClearVertexVelocities() = 0;

	// Zero without vertex velocities
	virtual Vector GetVertexVelocity( int nVertex ) const = 0;

	// The vertex velocities of a triangle interpolated at a point on it
	virtual Vector GetVertexVelocityAtPoint( int nTriangle, const Vector &vPoint ) const = 0;

	virtual Vector GetScale() const = 0;

	// Scaled vertices of a triangle
	virtual void GetTriangleVertices( int nTriangle, Vector *pVertex1, Vector *pVertex2, Vector *pVertex3 ) const = 0;

	// Appends the triangles whose bounds overlap the bounds, or that overlap the box itself unless bBoundsOnly is set
	virtual void GetTrianglesInBox( CUtlVector< int > &triangles, const AABB_t &bounds, bool bBoundsOnly ) const = 0;
};

abstract_class IFnPhysicsCompoundShape : public IFnPhysicsShape
{
public:
	virtual const RnCompound_t *GetCompound() const = 0;

	// bUpdateMass recomputes the mass properties of the body
	virtual void SetCompound( const RnCompound_t *pCompound, bool bUpdateMass ) = 0;

	virtual float GetScale() const = 0;
};

#endif // FNPHYSICSSHAPE_H
