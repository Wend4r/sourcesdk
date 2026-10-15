//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Contact between two physics shapes and its function table interface
//
//===========================================================================//

#ifndef IPHYSICSCONTACT_H
#define IPHYSICSCONTACT_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "rubikon/param_types.h"

class IVPhysics2World;
class IPhysicsBody;
class IPhysicsShape;
class CBufferString;
class CRnContact;

// The shapes and the manifolds of a contact. The manifolds are a reference counted block (the reference count, the manifold count
// and the manifolds); IPhysicsShape::GetContacts releases the references of the old elements before it fills the vector again
struct PhysicsContact_t
{
	IPhysicsShape *m_pShape1;
	IPhysicsShape *m_pShape2;

	// Zeroed together with the contact
	uint8 m_pad010[ 8 ];

	void *m_pManifolds;
};

// The manifolds of a contact, returned in two integer registers on Linux
struct PhysicsContactManifolds_t
{
	const Manifold_t *m_pManifolds;
	intp m_nCount;
};

// Function table operating on the contact it is bound to
abstract_class IFnPhysicsContact
{
public:
	virtual void SetContact( CRnContact *pContact ) = 0;

	// The bodies of shape 1 and shape 2; the manifold normals point from shape 1 to shape 2. GetBody and GetShape take
	// index 0 for the first and index 1 for the second
	virtual IPhysicsBody *GetBody1() const = 0;
	virtual IPhysicsBody *GetBody2() const = 0;
	virtual IPhysicsBody *GetBody( int nIndex ) const = 0;
	virtual IPhysicsBody *GetOther( IPhysicsBody *pBody ) const = 0;

	virtual IPhysicsShape *GetShape( int nIndex ) = 0;
	virtual const IPhysicsShape *GetShape( int nIndex ) const = 0;

	// 0 between convex shapes, 1 against a mesh, 2 for a wheel
	virtual int GetContactType() const = 0;

	virtual bool HasManifolds() const = 0;

	// Both the found and the lost notification flags are set
	virtual bool IsFoundAndLost() const = 0;

	// 1 resolve, 4 notify found, 8 notify persist, 16 notify lost
	virtual uint16 GetFlags() const = 0;

	virtual int GetManifoldCount() const = 0;
	virtual PhysicsContactManifolds_t GetManifolds() const = 0;
	virtual int GetContactPointCount() const = 0;

	// The same body as HasManifolds on both platforms; nothing tells the two apart
	virtual bool unk014() const = 0;

	// Fills the shapes and a reference to the manifolds
	virtual void GetContact( PhysicsContact_t &contact ) const = 0;

	// Subtracted from the separation of every point by GetMinSeparation
	virtual float GetSeparationOffset() const = 0;

	// Sum of the impulses of the points
	virtual float GetTotalImpulse() const = 0;

	// Scales the impulses of the manifolds and of their points
	virtual void ScaleImpulses( float flScale ) = 0;

	// Always false
	virtual bool unk019() const = 0;

	// Whether the solver or the report contact list of the world holds the contact at its index
	virtual bool IsInWorld( const IVPhysics2World *pWorld ) const = 0;

	// Index in the solver or the report contact list of the world, -1 outside of the world
	virtual int GetIndexInWorld() const = 0;

	// FLT_MAX without manifolds
	virtual float GetMinSeparation() const = 0;

	// Collides the shapes again at the current transforms of their bodies when the contact is in the world
	virtual void Update() = 0;

	// Appends the names of the set contact flags, the first preceded by pPrefix and the others by pSeparator; returns the prefix of the next name
	virtual const char *GetFlagNames( const char *pPrefix, const char *pSeparator, CBufferString &out ) const = 0;
};

#endif // IPHYSICSCONTACT_H
