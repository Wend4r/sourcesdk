#ifndef IBONETRANSFORMOVERRIDE_H
#define IBONETRANSFORMOVERRIDE_H

#pragma once

#include "tier0/platform.h"

class CSkeletonInstance;
class CTransform;
template < class T > class CBoneAccessor;

enum BoneTransformOverrideResult_t
{
	BONE_TRANSFORM_NOT_OVERRIDDEN = 0,
	BONE_TRANSFORM_SPECIFIED_IN_PARENT_SPACE = 1,
	BONE_TRANSFORM_SPECIFIED_IN_WORLD_SPACE = 2,
};

//-----------------------------------------------------------------------------
// Lets an entity replace bone transforms while its skeleton is set up. The
// skeleton instance keeps a pointer to it (CCSWeaponBase::Activate hands it
// over); CEconEntity and CRagdollProp derive from it.
//
// Virtual methods only, in the order of the game's table (5 slots on both
// platforms, no destructor). GetBoneTransformOverride, GetScratchMemorySize and
// the result enum are named by Dota 2 2017 (DWARF); the other three have no
// symbol and are named by what their callers do with them.
//-----------------------------------------------------------------------------
abstract_class IBoneTransformOverride
{
public:
	// Called for every bone of the bone setup with scratch memory of GetScratchMemorySize bytes;
	// BONE_TRANSFORM_SPECIFIED_IN_WORLD_SPACE stores *pTransform into the world space bones
	virtual BoneTransformOverrideResult_t GetBoneTransformOverride( const CSkeletonInstance *pInstance, int nBoneIndex, const CBoneAccessor< CTransform > &parentSpaceBones, const CBoneAccessor< CTransform > &worldSpaceBones, bool *pScratchMemoryValid, void *pScratchMemory, CTransform *pTransform ) const = 0;
	// False in every class, on the server and the client. Asked once before the bone loop of
	// the bone setup, which stores the answer in a flag of the skeleton instance (+411 on Linux)
	// that the skeleton code around it never reads. Named by what the caller does with it
	virtual bool GetSkeletonBoneSetupFlag() const = 0;
	// False on the server; C_CSWeaponBase and C_Multimeter return true on the client. When true,
	// the skeleton takes the render bounds of its scene node grown by the distance between the two
	// points it is given as its bounds. Named by what the caller does with it
	virtual bool ShouldExpandBounds() const = 0;
	// The size of the scratch memory the bone setup allocates for GetBoneTransformOverride; the
	// client C_RagdollProp asks 32 bytes per bone
	virtual int GetScratchMemorySize( const CSkeletonInstance *pInstance ) const = 0;
	// Empty in every class on the server and the client, no caller found. Named by its body
	virtual void UnusedHook() = 0;
};

#endif // IBONETRANSFORMOVERRIDE_H
