//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: Types used by the render device interfaces
//
//===========================================================================//

#ifndef RENDERSYSTEM_RENDERDEVICETYPES_H
#define RENDERSYSTEM_RENDERDEVICETYPES_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "bitmap/imageformat_declarations.h"
#include "rendersystem/rendersystemtypes.h"

enum RenderDeviceAPI_t
{
	RENDER_DEVICE_API_DX11 = 0,
	RENDER_DEVICE_API_VULKAN,
	RENDER_DEVICE_API_EMPTY,
	RENDER_DEVICE_API_MAX
};

enum RenderTextureDimension_t : int8
{
	RENDER_TEXTURE_DIMENSION_INVALID = -1,
	RENDER_TEXTURE_DIMENSION_1D = 0,
	RENDER_TEXTURE_DIMENSION_2D,
	RENDER_TEXTURE_DIMENSION_2D_ARRAY,
	RENDER_TEXTURE_DIMENSION_3D,
	RENDER_TEXTURE_DIMENSION_CUBE,
	RENDER_TEXTURE_DIMENSION_CUBE_ARRAY,
	RENDER_TEXTURE_DIMENSION_BUFFER,
	RENDER_TEXTURE_DIMENSION_COUNT
};

const int RENDER_ADAPTER_NAME_LENGTH = 512;

struct RenderAdapterInfo_t
{
	char m_pDriverName[ RENDER_ADAPTER_NAME_LENGTH ];
	uint32 m_VendorID;
	uint32 m_DeviceID;
	uint32 m_SubSysID;
	uint32 m_Revision;
	uint8 m_unk210;
	uint8 m_pad211[ 3 ];
	int m_nDXSupportLevel;
	int m_nMinDXSupportLevel;
	int m_nMaxDXSupportLevel;
	uint32 m_nDriverVersionHigh;
	uint32 m_nDriverVersionLow;
};

enum RenderColorSpace_t : uint8
{
	RENDER_LINEAR = 0,
	RENDER_SRGB
};

enum IsReadyToCompactResult_t
{
	IS_READY = 0,
	NOT_READY,
	NOT_COMPACTIBLE
};

enum RenderDisplayModeFlags_t
{
	RENDER_DISPLAY_MODE_EXCLUSIVE_FULLSCREEN = ( 1 << 0 ),
	RENDER_DISPLAY_MODE_COOPERATIVE_FULLSCREEN = ( 1 << 1 ),
	RENDER_DISPLAY_MODE_BORDERLESS_WINDOW = ( 1 << 2 ),
	RENDER_DISPLAY_MODE_BORDERED_WINDOW = ( 1 << 3 ),
	RENDER_DISPLAY_MODE_ALL_USAGE = 15,
	RENDER_DISPLAY_MODE_HIGH_DPI = ( 1 << 4 ),
};

struct RenderDisplayMode_t
{
	int m_nVersion;
	int m_nWidth;
	int m_nHeight;
	ImageFormat m_Format;
	int m_nRefreshRateNumerator;
	int m_nRefreshRateDenominator;
	uint32 m_nFlags;
};

struct RenderDeviceInfo_t
{
	int m_nVersion;
	RenderDisplayMode_t m_DisplayMode;
	int m_nBackBufferCount;
	RenderMultisampleType_t m_nMultisampleType;

	// RenderDisplayModeFlags_t
	uint8 m_nModeUsage;

	uint8 m_bUseStencil : 1;
	uint8 m_bWaitForVSync : 1;
	uint8 m_bScaleToOutputResolution : 1;
	uint8 m_bProgressive : 1;
	uint8 m_bUsingMultipleWindows : 1;
	uint8 m_bIsMainWindow : 1;
	uint8 m_unk026_6 : 2;
	uint8 m_pad027[ 1 ];
};

struct BufferDesc_t
{
	uint64 m_nElementCount;
	int m_nElementSizeInBytes;
	const char *m_pDebugName;
	const char *m_pBudgetGroupName;
};

struct RenderShaderStats_t
{
	int32 m_nInstructionCount;
	int32 m_nRegisterCount;
};

#endif // RENDERSYSTEM_RENDERDEVICETYPES_H
