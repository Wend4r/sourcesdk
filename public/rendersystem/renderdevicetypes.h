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

struct BufferDesc_t
{
	uint64 m_nElementCount;
	int m_nElementSizeInBytes;
	const char *m_pDebugName;
	const char *m_pBudgetGroupName;
};

#endif // RENDERSYSTEM_RENDERDEVICETYPES_H
