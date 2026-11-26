//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: Shader compiler of a render system API (vfx_dx11)
//
//===========================================================================//

#ifndef IVFX_H
#define IVFX_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/interface.h"

class CVfx;
struct VfxCompiledShaderInfo_t;

enum VfxCompileTarget_t : int;
enum VfxProgram_t : int;

//-----------------------------------------------------------------------------
// Loaded by the material system with Plat_LoadInterface( "vfx_dx11" or "vfx_vulkan",
// VFX_INTERFACE_VERSION ) when dynamic shader compiling is enabled. Not an app system.
//-----------------------------------------------------------------------------
abstract_class IVfx
{
public:
	// Connects the file system from fsFactory, loads the D3D compiler; bPurgeOldShaderCache
	// deletes the *.vshadercache files older than 30 days
	virtual void Init( CreateInterfaceFn fsFactory, bool bPurgeOldShaderCache ) = 0;

	// pInfoOut is reset first. nFlags holds the compile options; with bWriteShaderCache the result is
	// added to the shader cache unless nFlags has bit 3
	virtual bool CompileShaderAndGetVariables( uint64 nStaticComboId, uint64 nDynamicComboId, const CVfx *pVfx, VfxCompileTarget_t nTarget, VfxProgram_t nProgram, VfxCompiledShaderInfo_t *pInfoOut, bool bWriteShaderCache, uint32 nFlags ) = 0;

	// Runs the preprocessor only and collects the variables of the program ( "$Globals" ) into
	// pInfoOut; bOption goes to the variable collector, which the compile path calls with false
	virtual bool GetAllShaderVariables( const CVfx *pVfx, VfxCompileTarget_t nTarget, VfxProgram_t nProgram, VfxCompiledShaderInfo_t *pInfoOut, bool bOption ) = 0;

	// Deletes every *.vshadercache file
	virtual void ClearShaderCache() = 0;

	// Named by what it does: has no arguments and returns false in vfx_dx11; no caller in the
	// material system
	virtual bool AlwaysReturnsFalse() = 0;

	virtual ~IVfx() {}
};

#endif // IVFX_H
