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
class Color;
class ISceneViewDebugOverlays;
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

	// The casts report a hit only below flMaxFraction and then store the material of the shape in the result
	virtual bool CastRay( CShapeCastResult &result, const Vector &vStart, const Vector &vDelta, float flMaxFraction ) const = 0;

	// A radius below FLT_EPSILON casts a ray
	virtual bool CastSphere( CShapeCastResult &result, const Vector &vStart, const Vector &vDelta, float flRadius, float flMaxFraction ) const = 0;

	// Cast against the shape placed at the transform: a capsule given by its center, half axis and radius, and a generic
	// query shape (a point, a sphere, a capsule or a hull by the type stored in it)
	virtual bool CastCapsule( CShapeCastResult &result, const matrix3x4_t &transform, const Vector &vCenter, const Vector &vDelta, const Vector &vHalfAxis, float flRadius ) = 0;
	virtual bool CastShape( CShapeCastResult &result, const matrix3x4_t &transform, const Vector &vStart, const Vector &vDelta, const Ray_t &shape, float flMaxFraction ) = 0;

	// Overlap tests of the shape, MSVC lays the group out in reverse order: against a sphere, against bounds with the shape
	// placed by a matrix or by a transform, and against a generic query shape (a cast without motion) with the shape placed by a transform
	virtual bool Overlaps( const Vector &vCenter, float flRadius ) = 0;
	virtual bool Overlaps( const matrix3x4_t &transform, const AABB_t &bounds ) = 0;
	virtual bool Overlaps( const CTransform &transform, const AABB_t &bounds ) = 0;
	virtual bool Overlaps( const CTransform &transform, const Ray_t &shape ) = 0;

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

	// The resolved contacts of a shape with it set are passed to the contact callback of the world
	virtual void SetContactCallbackEnabled( bool bEnabled ) = 0;
	virtual bool IsContactCallbackEnabled() const = 0;

	// Bytes used by the shape and its geometry
	virtual uintp GetRuntimeSize() const = 0;

	virtual void ComputeSubmergedVolume( const matrix3x4a_t &surface, PhysicsShapeSubmergedVolume_t &result ) = 0;

	virtual void SetUserData( void *pUserData ) = 0;
	virtual void *GetUserData() const = 0;

	// Empty in release builds, where the name is null
	virtual void SetDebugName( const char *pszName ) = 0;
	virtual const char *GetDebugName() = 0;

	virtual const char *GetBodyDebugName() = 0;

	// The hit group of the shape markup of the aggregate data, an empty string by default
	virtual void SetHitGroup( const char *pszHitGroup ) = 0;
	virtual const char *GetHitGroup() const = 0;

	// Debug draw of the shape placed at the transform; flDuration is passed on to the overlays
	virtual void Draw( ISceneViewDebugOverlays *pDebugOverlays, const CTransform &transform, const Color &color, uint64 nDebugDrawOptions, float flDuration ) = 0;

	// Opacity factor of the debug shape, 1 by default
	virtual void SetDebugAlpha( float flAlpha ) = 0;
	virtual float GetDebugAlpha() const = 0;

	virtual ~IPhysicsShape() {}
};

#endif // IPHYSICSSHAPE_H
