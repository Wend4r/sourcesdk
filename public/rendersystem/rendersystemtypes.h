#ifndef RENDERSYSTEM_RENDERSYSTEMTYPES_H
#define RENDERSYSTEM_RENDERSYSTEMTYPES_H

#ifdef _WIN32
#pragma once
#endif

#include "network_connection.pb.h"

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

#endif // RENDERSYSTEM_RENDERSYSTEMTYPES_H
