//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Value types the nodes of an AnimGraph2 graph produce
//
//===========================================================================//

#ifndef ANIMGRAPH_VALUETYPES_H
#define ANIMGRAPH_VALUETYPES_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/globalsymbol.h"
#include "mathlib/transform.h"

#include "schemacompiler/schemaon.h"

class CNmSkeleton;
class CNmClip;
class CNmGraphDefinition;

class InfoForResourceTypeCNmSkeleton
{
public:
	using RuntimeClass_t = CNmSkeleton;
};

class InfoForResourceTypeCNmClip
{
public:
	using RuntimeClass_t = CNmClip;
};

class InfoForResourceTypeCNmGraphDefinition
{
public:
	using RuntimeClass_t = CNmGraphDefinition;
};

// Type of the value a graph node produces
schema enum class NmGraphValueType_t : uint8
{
	Unknown = 0,
	Bool,
	ID,
	Float,
	Vector,
	Target,
	BoneMask,
	Pose,
	Special,
};

// Value of a target parameter: a bone of the skeleton or a transform
schema class CNmTarget
{
public:
	CTransform m_transform;
	CGlobalSymbol m_boneID;
	bool m_bIsBoneTarget;
	bool m_bIsUsingBoneSpaceOffsets;
	bool m_bHasOffsets;
	bool m_bIsSet;
};

#include "schemacompiler/schemaoff.h"

#endif // ANIMGRAPH_VALUETYPES_H
