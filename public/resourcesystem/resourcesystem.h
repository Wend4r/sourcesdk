#ifndef RESOURCESYSTEM_RESOURCESYSTEM_H
#define RESOURCESYSTEM_RESOURCESYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "resourcesystem/iresourcesystem.h"
#include "tier0/tslist.h"
#include "tier1/utlvector.h"
#include "tier2/tier2.h"
#include "iresourcehandleutils.h"

class CResourceSystem : public CTier2AppSystem< IResourceSystem >, public IResourceSystemUtils, public IResourceHandleUtils, public IResourceManifestRegistry
{
public:
	uint8 m_pad040[ 0x2DAC ];
	bool m_bShuttingDown;
	uint8 m_pad2DED[ 0x1A3 ];
	CUtlVector< IToolsResourceListener * > m_ToolsResourceListeners;
	uint8 m_pad2FA8[ 0x4C ];

	// Indexed by the SetDebugFlag and GetDebugFlag flag
	bool m_DebugFlags[ 4 ];

	void *m_pTestFS;
	IFileSystem *m_pPreviousFullFileSystem;
	uint8 m_pad3008[ 0x28 ];
	CTSListBase m_Unk3030;
};

#endif // RESOURCESYSTEM_RESOURCESYSTEM_H
