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

class IPhysicsBody;
class IPhysicsShape;
class CBufferString;

struct PhysicsContact_t;

// Function table operating on the contact it is bound to
abstract_class IFnPhysicsContact
{
public:
	virtual void unk000( void *pContact ) = 0;

	virtual IPhysicsBody *GetBody1() const = 0;
	virtual IPhysicsBody *GetBody2() const = 0;
	virtual IPhysicsBody *unk003( int nIndex ) const = 0;
	virtual IPhysicsBody *GetOther( IPhysicsBody *pBody ) const = 0;

	virtual IPhysicsShape *unk005( int nIndex ) = 0;
	virtual const IPhysicsShape *unk005( int nIndex ) const = 0;

	virtual bool unk007() const = 0;

	// Whether the contact has manifolds
	virtual bool unk008() const = 0;

	virtual bool unk009() const = 0;
	virtual uint16 unk010() const = 0;

	virtual int GetManifoldCount() const = 0;

	// Returns the manifold array and its count by value; the type is not recovered
	virtual void unk012( void *pResult ) const = 0;

	virtual int GetContactPointCount() const = 0;

	// Whether the contact has manifolds
	virtual bool unk014() const = 0;

	// Fills the shapes and a reference to the manifolds
	virtual void unk015( PhysicsContact_t *pContact ) const = 0;

	virtual float unk016() const = 0;
	virtual float unk017() const = 0;
	virtual void unk018( float flValue ) = 0;

	// Always false
	virtual bool unk019() const = 0;

	virtual bool unk020( const void *pUnknown ) const = 0;
	virtual int unk021() const = 0;

	// FLT_MAX without manifolds
	virtual float GetMinSeparation() const = 0;

	virtual void unk023() = 0;

	// Appends the names of the set contact flags, the first preceded by pPrefix and the others by pSeparator
	virtual const char *unk024( const char *pPrefix, const char *pSeparator, CBufferString *pOut ) const = 0;
};

#endif // IPHYSICSCONTACT_H
