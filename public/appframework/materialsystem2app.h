//=========== (C) Copyright 1999 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// $Header: $
// $NoKeywords: $
//
// Used for material system apps
//=============================================================================

#ifndef MATERIALSYSTEM2APP_H
#define MATERIALSYSTEM2APP_H

#ifdef _WIN32
#pragma once
#endif


#include "appframework/tier2app.h"
#include "tier0/platwindow.h"
#include "rendersystem/irenderdevice.h"


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
enum RenderMultisampleType_t : uint8;
FORWARD_DECLARE_HANDLE( SwapChainHandle_t );


class CMaterialSystem2AppSystemDict : public CTier2AppSystemDict, public IRenderDeviceSetup
{
public:
	SwapChainHandle_t m_hSwapChain;
	int m_nThreadCount;
	CreateInterfaceFn m_RenderFactory;
	CUtlString m_WindowTitle;
	CUtlString m_WindowIcon;
	CUtlString m_DefaultRenderSystemOption;
	// Named by what it does: cleared by the constructor and when Source2PreInit picks the default
	// render system option, nothing reads it
	bool m_bUnusedRenderSystemFlag;
	CUtlString m_WindowInitialImage;
	RenderMultisampleType_t m_MultisampleType;
	bool m_bInitWithoutMaterialSystem;
	bool m_bEnableExtendedMaterialInfo;
	bool m_bDefaultToRendersystemEmpty;
	bool m_bDiscardMouseFocusClick;
	bool m_bRenderSystemOptionOnCommandLine;
};

#endif // MATERIALSYSTEM2APP_H
