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

class CMaterial2;

// The shader of a mode (CVfx, loaded by CMaterialMode::FindOrLoadVfx), reference counted at +5196
class CVfx;

// Reference counted at +364; the material system flags its static combo cache when the count drops to 0
class CVfxStaticComboData;

FORWARD_DECLARE_HANDLE( ConstantBufferHandle_t );
FORWARD_DECLARE_HANDLE( RenderBufferHandle_t );

//-----------------------------------------------------------------------------
// Implementation of IMaterialMode, 6 inline in every CMaterial2. Spans whose
// meaning is unknown are padding.
//-----------------------------------------------------------------------------
class CMaterialMode : public IMaterialMode
{
public:
	int ComputeRenderablePasses( const CRenderAttributes *pAttributes, const CRenderAttributes *pAttributes2, MaterialRenderablePass_t *pRenderablePass, bool bForce ) override;
	int ComputeRenderablePassesForContext( const CRenderAttributes *pAttributes, IRenderContext *pRenderContext, MaterialRenderablePass_t *pRenderablePass, bool bForce ) override;
	bool unk002( void **ppOut, CUtlVector< uint8 > *pData, uint32 *pOut, const CRenderAttributes *pAttributes, const CRenderAttributes *pAttributes2 ) override;
	bool IsShaderUpToDate() override;
	uint64 GetModeKey() override;

	// 72 bytes, indexed through the shader
	struct ShaderTypeData_t
	{
		CUtlLeanVectorFixedGrowable< uint16, 8, int > m_unk000;
		CUtlLeanVectorFixedGrowable< uint8, 12, int16 > m_unk018;
		CVfxStaticComboData *m_pStaticComboData;

		// IMaterialMode::unk002 returns one of these, falling back to the static combo data's list when empty
		CUtlVector< void * > m_Entries;
	};

	struct PassStateEntry_t
	{
	private:
		uint8 m_pad000[ 8 ];

	public:
		CUtlVector< uint8 > m_unk008;
	};

	// 504 bytes
	struct PassState_t
	{
		CUtlLeanVectorFixedGrowable< uint32, 4, int > m_unk000;

		// Destroyed with the mode
		ConstantBufferHandle_t m_hConstantBuffer;
		RenderBufferHandle_t m_hBuffer;

	private:
		uint8 m_pad028[ 8 ];

	public:
		CUtlLeanVectorFixedGrowable< uint8, 16, int > m_unk030;
		CUtlLeanVectorFixedGrowable< PassStateEntry_t, 12, int > m_unk048;

	private:
		// A small-buffer container (count at +0x1D4, inline or heap data at +0x1D8) whose class is not identified
		uint8 m_pad1D0[ 16 ];

	public:
		// Freed with delete[]
		uint8 *m_pArray;

		uint32 m_unk1E8;
		uint32 m_unk1EC;

		// ORed into MaterialRenderablePass_t::nAllStateSetBits
		uint32 m_nStateSetBits;

		uint8 m_unk1F4[ 4 ];
	};

private:
	// Copied, initialised to -1, 0 and 0
	uint16 m_unk008;
	uint8 m_unk00A;
	uint8 m_unk00B;

public:
	CVfx *m_pShader;

	// The material owning the mode
	CMaterial2 *m_pMaterial;

private:
	uint8 m_pad020[ 8 ];

public:
	// Returned by GetModeKey()
	uint64 m_nModeKey;

private:
	// Copied, initialised to -1 and to 1, 1
	int m_unk030;
	uint8 m_unk034;
	uint8 m_unk035;
	uint8 m_pad036[ 58 ];

public:
	CUtlLeanVector< ShaderTypeData_t, int > m_ShaderTypes;
	CUtlLeanVector< PassState_t, int > m_PassStates;
	CUtlLeanVector< uint8, int > m_unk090;
};

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

	CUtlLeanVectorFixedGrowable< CMaterialMode, 6, int > m_modeArray;

private:
	uint8 m_pad7D8[ 0x8 ];
};

#endif // MATERIAL2_H
