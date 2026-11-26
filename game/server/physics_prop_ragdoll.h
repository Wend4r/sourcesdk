#ifndef PHYSICS_PROP_RAGDOLL_H
#define PHYSICS_PROP_RAGDOLL_H

#pragma once

#include "../shared/baseanimgraph.h"
#include "../shared/ibonetransformoverride.h"

//-----------------------------------------------------------------------------
// CRagdollProp adds 2 methods to CBaseAnimGraph, Windows slots 306-307.
// The Linux table has 4, slots 307-310: 2 override methods of IBoneTransformOverride, which
// the Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseAnimGraphVTable<> >
abstract_class CRagdollPropVTable : public TBase, public IBoneTransformOverride
{
public:
	virtual void SetDamageEntity( CBaseEntity *pEntity ) = 0;
	// IBoneTransformOverride entry 0 (its secondary table entry is a thunk with the same body). Overrides IBoneTransformOverride: a slot of this table on Linux only
	virtual BoneTransformOverrideResult_t GetBoneTransformOverride( const CSkeletonInstance *pInstance, int nBoneIndex, const CBoneAccessor< CTransform > &parentSpaceBones, const CBoneAccessor< CTransform > &worldSpaceBones, bool *pScratchMemoryValid, void *pScratchMemory, CTransform *pTransform ) const override = 0;
	// IBoneTransformOverride entry 3. Overrides IBoneTransformOverride: a slot of this table on Linux only
	virtual int GetScratchMemorySize( const CSkeletonInstance *pInstance ) const override = 0;
	virtual bool HasActivationRecord() = 0;
};

#endif // PHYSICS_PROP_RAGDOLL_H
