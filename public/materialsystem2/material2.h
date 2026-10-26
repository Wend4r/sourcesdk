#ifndef MATERIAL2_H
#define MATERIAL2_H

#ifdef _WIN32
#pragma once
#endif

#include "materialsystem2/imaterial2.h"
#include <mathlib/vector.h>
#include <mathlib/vector2d.h>
#include <tier0/utlstringtoken.h>
#include <rendersystem/renderattrs.h>

//-----------------------------------------------------------------------------
// Implementation of IMaterial2. Members whose types are not declared in the
// SDK are kept as padding of their size.
//-----------------------------------------------------------------------------
class CMaterial2 : public IMaterial2
{
public:
	struct StaticComboVcsLoad_t;

	// Returned by GetCopySource()
	HMaterialStrong m_hCopySource;

	// Depth of EnterOverrideScope() / LeaveOverrideScope()
	int m_nOverrideScopeDepth;

	// Taken from a global counter when the material is created
	uint32 m_nUniqueId;

	// Copied from m_renderAttributesUsed of the resource data
	CUtlLeanVector< CUtlString > m_renderAttributesUsed;

	// Waited for and released when the material is finalized
	CUtlLeanVector< StaticComboVcsLoad_t *, int > m_staticComboLoadRequests;

	// Set by IMaterialSystem2::CreateMaterialReferencingResource(), returned by GetReferencedResource()
	CStrongHandleBase m_hReferencedResource;

	// Frame count of the material system when GetMode() was last called
	uint32 m_nLastFrameUsed;

	// Returned by GetShaderStateOverriddenByMaterial()
	uint32 m_nShaderStateOverrides;

	// Returned by GetAttributes()
	CRenderAttributes m_attributes;

	// CUtlLeanVectorFixedGrowable< CMaterialMode, 6, int > of 160-byte modes
	uint8 m_modeArray[ 0x3C8 ];

private:
	uint8 m_pad7D8[ 0x8 ];
};

#endif // MATERIAL2_H
