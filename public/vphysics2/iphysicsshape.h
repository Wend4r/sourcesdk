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
class CRnContact;
struct Ray_t;
struct RnCollisionAttr_t;
struct RnMassProperties_t;
struct PhysicsContact_t;

// The farthest point of a shape along a direction
struct PhysicsShapeSupport_t
{
	Vector m_vPoint;

	// The distance of the point along the direction
	float m_flDistance;
};

// Counters over the contacts of a shape; a point touches when its separation is not positive
struct PhysicsShapeContactCounts_t
{
	int m_nContactCount;

	// Contacts with a touching point against a static body
	int m_nStaticTouchingContactCount;

	int m_nPointCount;
	int m_nTouchingPointCount;
	int m_nManifoldCount;
};

// The part of a shape below the plane z = 0 of a frame
struct PhysicsShapeSubmergedVolume_t
{
	// The centroid of the submerged part multiplied by its volume
	Vector m_vWeightedCentroid;

	float m_flSubmergedVolume;

	// The volume of the whole shape
	float m_flVolume;
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
	virtual bool GetGenericShape( Ray_t &shape ) const = 0;

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

	virtual bool CastRay( CShapeCastResult &result, const Vector &vStart, const Vector &vDelta, float flUnknown ) const = 0;

	// A radius below FLT_EPSILON casts a ray
	virtual bool CastSphere( CShapeCastResult &result, const Vector &vStart, const Vector &vDelta, float flRadius, float flUnknown ) const = 0;

	// Both convert the transform and forward to shape casts; the rest of the arguments is not recovered
	virtual void unk015( void *p, const matrix3x4_t &transform ) = 0;
	virtual void unk016( void *p, const matrix3x4_t &transform ) = 0;

	// Overlap tests of the shape, MSVC lays the group out in reverse order: against a sphere, against bounds with the shape
	// placed by a matrix or by a transform, and against a query whose type is not recovered with the shape placed by a transform
	virtual bool Overlaps( const Vector &vCenter, float flRadius ) = 0;
	virtual bool Overlaps( const matrix3x4_t &transform, const AABB_t &bounds ) = 0;
	virtual bool Overlaps( const CTransform &transform, const AABB_t &bounds ) = 0;
	virtual bool Overlaps( const CTransform &transform, const void *p ) = 0;

	// Projects the point onto the surface; points inside are returned unchanged
	virtual Vector GetClosestPoint( const Vector &vPoint ) const = 0;

	virtual RnMassProperties_t ComputeMass() const = 0;

	// Bounds of the broadphase proxy
	virtual AABB_t GetProxyBounds() const = 0;
	virtual AABB_t GetLocalBounds() const = 0;

	// Local bounds moved by the body transform
	virtual AABB_t GetWorldBounds() const = 0;

	// MSVC groups these overloads in reverse order; the second one places the shape at the transform
	virtual PhysicsShapeSupport_t GetSupport( const Vector &vDirection ) = 0;
	virtual PhysicsShapeSupport_t GetSupport( const CTransform &transform, const Vector &vDirection ) = 0;

	// Support mapping of the shape for the GJK queries
	virtual CGJKProxy GetGJKProxy() const = 0;
	virtual float GetVolume() const = 0;

	// pi / 4 on every axis for a sphere, the stored areas of a hull or a mesh
	virtual Vector GetOrthographicAreas() = 0;

	// MSVC groups the overloads of these two names in reverse order at the position of their first declaration
	virtual IPhysicsShape *GetOtherShape( IPhysicsShape *const *ppShapePair ) const = 0;
	virtual void GetContacts( CUtlVector< PhysicsContact_t > &contacts ) const = 0;

	virtual PhysicsShapeContactCounts_t GetContactCounts() = 0;

	virtual IPhysicsShape *GetOtherShape( const CRnContact *pContact ) const = 0;
	virtual void GetContacts( CUtlVector< CRnContact * > &contacts ) const = 0;

	// Appends the entity ids of the other bodies of the contacts, then sorts the list and removes the duplicates
	virtual void GetContactEntityIds( CUtlVector< uint32 > &entityIds ) = 0;

	virtual void unk037( bool bUnknown ) = 0;
	virtual bool unk038() const = 0;

	// Bytes used by the shape and its geometry
	virtual uintp GetRuntimeSize() const = 0;

	virtual void ComputeSubmergedVolume( const matrix3x4a_t &surface, PhysicsShapeSubmergedVolume_t &result ) = 0;

	virtual void unk041( void *p ) = 0;
	virtual void *unk042() const = 0;

	// Empty in release builds
	virtual void unk043() = 0;
	virtual bool unk044() = 0;

	// The debug name of the body
	virtual const char *GetDebugName() = 0;

	virtual void unk046( void *p ) = 0;
	virtual void *unk047() const = 0;

	virtual void unk048( void *p ) = 0;

	virtual void unk049( float flUnknown ) = 0;
	virtual float unk050() const = 0;

	virtual ~IPhysicsShape() {}
};

#endif // IPHYSICSSHAPE_H
