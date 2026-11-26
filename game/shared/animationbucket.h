//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: CAnimationBucket: records pose commands for the object that owns it
//
//===========================================================================//

#ifndef ANIMATIONBUCKET_H
#define ANIMATIONBUCKET_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CBoneSetup;
class CModel;
class Vector;
class Quaternion;
struct HSequence;

//-----------------------------------------------------------------------------
// A result slot of a bone setup: records pose commands into it. 24 bytes on both
// platforms. The pose operations each append one 64-byte command to the command
// list of the bone setup (the type of the command in its first dword, at most 512
// commands); the four getters forward to the bone setup (EBoneSetup). Every bone
// setup embeds one bucket for its base result.
//-----------------------------------------------------------------------------
abstract_class CAnimationBucket
{
public:
	// Frees m_nResult in the bone setup (EBoneSetup::FreeResult)
	virtual ~CAnimationBucket() {}

	virtual void Slerp( const CAnimationBucket &source, int nIndex, float flWeight ) = 0;
	virtual void Add( const CAnimationBucket &source, int nIndex, float flWeight ) = 0;
	virtual void Subtract( const CAnimationBucket &source, int nIndex, float flWeight ) = 0;
	virtual void Scale( float flScale ) = 0;
	virtual void Copy( const CAnimationBucket &source ) = 0;
	virtual void Blend( const CAnimationBucket &source, int nIndex, float flWeight ) = 0;
	virtual void Worldspace( const CAnimationBucket &source, int nIndex, float flWeight ) = 0;
	virtual void CallSequence( HSequence hSequence, float flCycle, float flWeight ) = 0;
	virtual void FetchCycle( int nIndex, float flCycle ) = 0;
	virtual void FetchFrame( int nIndex, float flFrame ) = 0;
	virtual void IKLockInPlace( int nIndex, float flValue0, float flValue1, bool bFlag ) = 0;
	virtual void IKRestoreAll() = 0;
	virtual void Transform( const CAnimationBucket &source, const Vector &vecOrigin, const Quaternion &qRotation ) = 0;

	// The EBoneSetup methods of the same names
	virtual const CModel *GetCModel() const = 0;
	virtual float GetPoseParameter( int nIndex ) const = 0;
	virtual const float *GetPoseParameterArray() const = 0;
	virtual float GetRealtime() const = 0;

public:
	// The bone setup the commands go to
	CBoneSetup *m_pBoneSetup;
	// The result slot of the bone setup this bucket fills (EBoneSetup::AllocateResult)
	int m_nResult;
};

#endif // ANIMATIONBUCKET_H
