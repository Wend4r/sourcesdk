//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: A collision shape attached to a physics body
//
//===========================================================================//

#ifndef IPHYSICSSHAPE_H
#define IPHYSICSSHAPE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utlvector.h"
#include "mathlib/aabb.h"
#include "mathlib/mathlib.h"
#include "mathlib/transform.h"
#include "mathlib/vector.h"
#include "physicslib/param_types.h"

class IPhysicsBody;
class CGJKProxy;
struct Ray_t;
struct RnCollisionAttr_t;
struct RnMassProperties_t;

// The 16-byte result of IPhysicsShape::unk026: the support point of the shape in the given direction and one more float,
// four floats returned in two vector registers on Linux. Only its size is known
struct PhysicsShapeUnk026_t
{
	float m_flData[ 4 ];
};

// The 20-byte sums IPhysicsShape::unk033 returns over the contacts of the shape (five 32-bit counters); only its size is known
struct PhysicsShapeUnk033_t
{
	uint32 m_nData[ 5 ];
};

abstract_class IPhysicsShape
{
public:
	// Enabled shapes have a broadphase proxy; disabling one removes the proxy and destroys its contacts
	virtual bool IsEnabled() const = 0;
	virtual void SetEnabled( bool bEnabled ) = 0;

	virtual IPhysicsBody *GetBody() const = 0;
	virtual PhysicsShapeType_t GetType() const = 0;

	// Describes the shape as a generic query shape; the type is stored at the end, as in Ray_t
	virtual bool GetGenericShape( Ray_t *pShape ) const = 0;

	// bUpdateMass recomputes the mass properties of the body when the density changes
	virtual bool SetMaterial( const RnMaterial_t &material, bool bUpdateMass ) = 0;
	virtual const RnMaterial_t &GetMaterial() const = 0;

	// Looks the surface property up by its name token and fills the material from it
	virtual bool SetMaterialIndex( CUtlStringToken surfacePropertyToken ) = 0;
	virtual CUtlStringToken GetMaterialIndex() const = 0;

	// bRefilter refreshes the broadphase pairs and the contacts of the shape
	virtual void SetCollisionAttributes( const RnCollisionAttr_t &attributes, bool bRefilter ) = 0;
	virtual const RnCollisionAttr_t &GetCollisionAttributes() const = 0;
	virtual uint8 GetCollisionGroup() const = 0;

	// Takes the interacts-as layers, the collision group and the detail layers of the given attributes as overrides
	virtual void SetCollisionAttributeOverrides( const RnCollisionAttr_t &attributes ) = 0;

	virtual bool CastRay( CShapeCastResult *pResult, const Vector &vStart, const Vector &vDelta, float flUnknown ) const = 0;

	// A radius below FLT_EPSILON casts a ray
	virtual bool CastSphere( CShapeCastResult *pResult, const Vector &vStart, const Vector &vDelta, float flRadius, float flUnknown ) const = 0;

	// Both convert the transform and forward to shape casts; the rest of the arguments is not recovered
	virtual void unk015( void *p, const matrix3x4_t &transform ) = 0;
	virtual void unk016( void *p, const matrix3x4_t &transform ) = 0;

	// Point within a distance of the shape
	virtual bool unk017( void *p ) = 0;

	// Overlap tests through the same shape routine, with the shape placed by a matrix or by a transform
	virtual bool unk018( const matrix3x4_t &transform, const void *p ) = 0;
	virtual bool unk018( const CTransform &transform, const void *p ) = 0;
	virtual bool unk020( const void *p1, const void *p2 ) = 0;

	// Projects the point onto the surface; points inside are returned unchanged
	virtual Vector GetClosestPoint( const Vector &vPoint ) const = 0;

	virtual RnMassProperties_t ComputeMass() const = 0;

	// Bounds of the broadphase proxy
	virtual AABB_t GetProxyBounds() const = 0;
	virtual AABB_t GetLocalBounds() const = 0;

	// Local bounds moved by the body transform
	virtual AABB_t GetWorldBounds() const = 0;

	// MSVC groups these overloads in reverse order; the second one places the shape at the transform
	virtual PhysicsShapeUnk026_t unk026( const Vector &vDirection ) = 0;
	virtual PhysicsShapeUnk026_t unk026( const CTransform &transform, const Vector &vDirection ) = 0;

	// Support mapping of the shape for the GJK queries
	virtual CGJKProxy GetGJKProxy() const = 0;
	virtual float GetVolume() const = 0;

	// A constant for a sphere, a stored vector for a hull or a mesh
	virtual Vector unk030() = 0;

	// MSVC groups the overloads of these two names in reverse order at the position of their first declaration
	virtual IPhysicsShape *GetOtherShape( IPhysicsShape *const *ppShapePair ) const = 0;
	virtual void GetContacts( void *pContacts ) const = 0;

	// Sums over the contacts of the shape
	virtual PhysicsShapeUnk033_t unk033() = 0;

	virtual IPhysicsShape *GetOtherShape( const void *pContact ) const = 0;
	virtual void GetContacts( CUtlVector< const void * > *pContacts ) const = 0;

	virtual bool unk036( CUtlVector< int > *pList ) = 0;

	virtual void unk037( bool bUnknown ) = 0;
	virtual bool unk038() const = 0;

	// Bytes used by the shape and its geometry
	virtual uintp GetRuntimeSize() const = 0;

	virtual void unk040( void *p1, void *p2 ) = 0;

	virtual void unk041( void *p ) = 0;
	virtual void *unk042() const = 0;

	// Empty in release builds
	virtual void unk043() = 0;
	virtual bool unk044() = 0;
	virtual void *unk045() = 0;

	virtual void unk046( void *p ) = 0;
	virtual void *unk047() const = 0;

	virtual void unk048( void *p ) = 0;

	virtual void unk049( float flUnknown ) = 0;
	virtual float unk050() const = 0;

	virtual ~IPhysicsShape() {}
};

#endif // IPHYSICSSHAPE_H
