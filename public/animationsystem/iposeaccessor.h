//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Access to the bones of a pose.
//
//=============================================================================//

#ifndef IPOSEACCESSOR_H
#define IPOSEACCESSOR_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CModel;
class CTransform;

//-----------------------------------------------------------------------------
// Reads and writes the bones of a pose in parent, model or world space.
//-----------------------------------------------------------------------------
abstract_class IPoseAccessor
{
public:
	virtual ~IPoseAccessor() {}
	virtual const CModel *GetModel() const = 0;
	virtual const CTransform &GetModelToWorld() const = 0;
	virtual void SetParentSpaceBone( int nBone, const CTransform &transform ) = 0;
	virtual CTransform GetParentSpaceBone( int nBone ) const = 0;
	virtual void SetModelSpaceBone( int nBone, const CTransform &transform ) = 0;
	virtual CTransform GetModelSpaceBone( int nBone ) const = 0;
	virtual void SetWorldSpaceBone( int nBone, const CTransform &transform ) = 0;
	virtual CTransform GetWorldSpaceBone( int nBone ) const = 0;
};

#endif // IPOSEACCESSOR_H
