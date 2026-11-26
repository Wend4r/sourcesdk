//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: What bone setup asks its owner.
//
//=============================================================================//

#ifndef BONESETUP_H
#define BONESETUP_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CModel;

//-----------------------------------------------------------------------------
// What bone setup asks its owner (CBoneSetup): the model, the pose parameters, the time and result slots.
// A pure interface: it has no data members (8 bytes, the table pointer only).
//-----------------------------------------------------------------------------
abstract_class EBoneSetup
{
public:
	virtual const CModel *GetCModel() const = 0;
	virtual float GetPoseParameter( int nIndex ) const = 0;
	virtual const float *GetPoseParameterArray() const = 0;
	virtual float GetRealtime() const = 0;
	virtual int AllocateResult() = 0;
	virtual void FreeResult( int nResult ) = 0;
};

#endif // BONESETUP_H
