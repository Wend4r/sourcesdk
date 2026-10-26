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

abstract_class IFnPhysicsShape
{
public:
	// MSVC groups these overloads in reverse order
	virtual void SetShape( IPhysicsShape *pShape ) = 0;
	virtual void SetShape( CRnShape *pShape ) = 0;

	virtual IPhysicsShape *GetShape() const = 0;

	// MSVC groups these overloads in reverse order
	virtual void unk003( int nUnknown1, int nUnknown2, int nUnknown3 ) = 0;
	virtual void unk003( void *p1, const void *p2, void *p3 ) = 0;

	// Switch on the shape type that calls the std::function callbacks given for each type
	virtual void unk005( void *p ) = 0;
	virtual void unk006( void *p ) = 0;
	virtual void unk007( void *p ) = 0;

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

	virtual void unk025( bool bUnknown ) = 0;

	// unk026 and unk029 store color 0 and color 1 of the debug shape
	virtual void unk026( const Vector4D &vColor ) = 0;

	// Four factors (index 0 to 3) whose product hides the debug shape at zero and selects the translucent materials below one
	virtual float GetDebugShapeAlphaFactor( int nIndex ) = 0;
	virtual void SetDebugShapeAlphaFactor( int nIndex, float flValue ) = 0;

	virtual void unk029( const Vector4D &vColor ) = 0;
	virtual void unk030( float flValue ) = 0;

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

	// Wakes the body to update its mass properties when bUpdateMass is set
	virtual void unk037( bool bUpdateMass ) = 0;

	virtual float GetScale() const = 0;

	// Queries against the hull; the arguments are not recovered
	virtual bool unk039( void *p ) = 0;
	virtual void unk040( void *p ) = 0;
	virtual bool unk041( void *p ) = 0;
	virtual bool unk042( void *p ) = 0;
};

abstract_class IFnPhysicsMeshShape : public IFnPhysicsShape
{
public:
	virtual const RnMesh_t *GetMesh() const = 0;

	// bUpdateMass recomputes the mass properties of the body
	virtual void SetMesh( const RnMesh_t *pMesh, bool bUpdateMass ) = 0;

	// MSVC groups these overloads in reverse order; the second takes a 16-byte struct by value
	virtual void unk037( bool bUpdateMass ) = 0;
	virtual void unk037( void *p ) = 0;

	virtual void unk039() = 0;
	virtual Vector unk040( int nIndex ) const = 0;
	virtual void unk041( int nIndex, void *p ) = 0;

	virtual Vector GetScale() const = 0;

	// Scaled vertices of a triangle
	virtual void GetTriangleVertices( int nTriangle, Vector *pVertex1, Vector *pVertex2, Vector *pVertex3 ) const = 0;

	virtual void unk044( void *p ) = 0;
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
