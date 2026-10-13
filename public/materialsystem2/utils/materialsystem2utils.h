#ifndef MATERIALSYSTEM2UTILS_H
#define MATERIALSYSTEM2UTILS_H

#ifdef _WIN32
#pragma once
#endif

#include "materialsystem2/imaterialsystem2utils.h"
#include "tier2/tier2.h"
#include <mathlib/vector.h>
#include <resourcesystem/resourcehandle.h>
#include <tier0/utlmap.h>
#include <tier0/utlstringtoken.h>
#include <tier0/utlvector.h>

//-----------------------------------------------------------------------------
// Implementation of IMaterialSystem2Utils
//-----------------------------------------------------------------------------
class CMaterialSystem2Utils : public CTier2AppSystem< IMaterialSystem2Utils >
{
public:
	// Returned by GetDefaultDebugFont()
	HFont m_DebugFont;
	HFont m_DebugFontOutline;

	RenderInputLayout_t m_hInputLayout;

	// "dev/vgui/materials/renderui.vmat" and "dev/materialsystem2utils/pixel.vtex"
	HMaterialStrong m_hUIMaterial;
	CStrongHandleBase m_hUIPixel;

	// Renderables queued by the Queue* methods, and their index by the queue token
	CUtlVector< CUtlVector< IQueuedRenderable * > > m_LayoutLists;
	CUtlMap< CUtlStringToken, int, int > m_LayoutListMap;

	// Renderables drawn by RenderFrame(), and their index by the queue token
	CUtlVector< CUtlVector< IQueuedRenderable * > > m_viewLayoutLists;
	CUtlMap< CUtlStringToken, int, int > m_viewLayoutListMap;

	// Cleared when the UI material is unloaded
	const IMaterialMode *m_pTexturedMaterialMode;
};

#endif // MATERIALSYSTEM2UTILS_H
