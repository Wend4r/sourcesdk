#ifndef RENDERSYSTEM_RENDERSYSTEMTYPES_H
#define RENDERSYSTEM_RENDERSYSTEMTYPES_H

#ifdef _WIN32
#pragma once
#endif

#include "network_connection.pb.h"
#include "tier0/platform.h"

enum RenderMultisampleType_t : uint8
{
	RENDER_MULTISAMPLE_INVALID = 0xFF,
	RENDER_MULTISAMPLE_NONE = 0,
	RENDER_MULTISAMPLE_2X,
	RENDER_MULTISAMPLE_4X,
	RENDER_MULTISAMPLE_6X,
	RENDER_MULTISAMPLE_8X,
	RENDER_MULTISAMPLE_16X,
	RENDER_MULTISAMPLE_TYPE_COUNT
};

struct RenderViewport_t
{
	int m_nVersion;
	int m_nTopLeftX;
	int m_nTopLeftY;
	int m_nWidth;
	int m_nHeight;
	float m_flMinZ;
	float m_flMaxZ;
};

struct RenderDisplayMode_t
{
	int m_nVersion;
	int m_nWidth;
	int m_nHeight;
	int m_Format;
	int m_nRefreshRateNumerator;
	int m_nRefreshRateDenominator;
	uint32 m_nFlags;
};

// The mode of a swap chain
struct RenderDeviceInfo_t
{
	int m_nVersion;
	RenderDisplayMode_t m_DisplayMode;
	int m_nBackBufferCount;
	RenderMultisampleType_t m_nMultisampleType;

	// 1 exclusive fullscreen, 2 desktop-friendly (cooperative) fullscreen, 4 borderless window,
	// otherwise a bordered window
	uint8 m_nModeUsage;

	bool m_bUseStencil : 1;
	bool m_bWaitForVSync : 1;
	bool m_bScaleToOutputResolution : 1;
	bool m_bProgressive : 1;
	bool m_bUsingMultipleWindows : 1;
	bool m_bIsMainWindow : 1;
};

#endif // RENDERSYSTEM_RENDERSYSTEMTYPES_H
