#ifndef MATERIALSYSTEM2_MATERIAL_RESOURCE_DATA_H
#define MATERIALSYSTEM2_MATERIAL_RESOURCE_DATA_H

#ifdef _WIN32
#pragma once
#endif

#include <resourcefile/resourcetype.h>
#include <mathlib/vector4d.h>

struct MaterialParam_t
{
	Vector4D m_vValue;
	ResourceHandle_t m_hTexture;
	uint32 m_nValueHash;
	uint32 m_nFlags;
	void *m_pBlob;
	CUtlSymbolLarge m_symName;
	CUtlSymbolLarge m_symString;
	bool m_bShaderDefault;

private:
	uint8 m_nUnknown0039[7];
};

#endif // MATERIALSYSTEM2_MATERIAL_RESOURCE_DATA_H
