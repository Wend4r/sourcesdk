#ifndef SOUND_OPVAR_SET_POINT_H
#define SOUND_OPVAR_SET_POINT_H

#pragma once

#include "baseentity.h"

class CTransform;

//-----------------------------------------------------------------------------
// CSoundOpvarSetPointBase adds 1 method to CBaseEntity: Linux slot 239, Windows 238.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CSoundOpvarSetPointBaseVTable : public TBase
{
public:
	// Every opvar point entity computes and sets its opvar here
	virtual void SetOpvarThink() = 0;
};

//-----------------------------------------------------------------------------
// CSoundOpvarSetOBBWindEntity adds 3 methods to CSoundOpvarSetPointBase: Linux slots 240-242, Windows 239-241.
//-----------------------------------------------------------------------------
template < class TBase = CSoundOpvarSetPointBaseVTable<> >
abstract_class CSoundOpvarSetOBBWindEntityVTable : public TBase
{
public:
	// Box math on two vectors
	virtual void Unk_239( void *pOut, const Vector &vecA, const Vector &vecB ) = 0;
	// Component-wise min/max. Named by what it does
	virtual void ComputeMinsMaxs( const Vector &vecA, const Vector &vecB, Vector *pMins, Vector *pMaxs ) = 0;
	// The z of a vector read through the entity, like the height of its origin
	virtual float Unk_241() = 0;
};

//-----------------------------------------------------------------------------
// CSoundOpvarSetBoxEntity adds 1 method to CSoundOpvarSetPointEntity (through CSoundOpvarSetPointEntity, which adds no method): Linux slot 240, Windows 239.
//-----------------------------------------------------------------------------
template < class TBase = CSoundOpvarSetPointBaseVTable<> >
abstract_class CSoundOpvarSetBoxEntityVTable : public TBase
{
public:
	// Pure; AABB returns the origin with an identity rotation, OBB the entity transform. Named by what it does
	virtual CTransform GetBoxTransform() = 0;
};

#endif // SOUND_OPVAR_SET_POINT_H
