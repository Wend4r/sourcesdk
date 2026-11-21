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
// The application object for apps that use tier2
//=============================================================================

#ifndef TIER2APP_H
#define TIER2APP_H

#ifdef _WIN32
#pragma once
#endif


#include "appframework/appframework.h"
#include "tier0/convar.h"
#include "appframework/tier1app.h"
#include "tier2/tier2_logging.h"

class IUGCAddonPathResolver;
class KeyValues3;


class CTier2AppSystemDict : public CTier1AppSystemDict
{
public:
	CUtlStringMap<int> m_addonRefcounts;
	CUtlStringMap<KeyValues3 *> m_addonInfo;
	KeyValues *m_pKeyValues;
	CUtlLeanVector< void * > m_StartupManifests;
	int m_nStartupManifestSuppressionCount;
	bool m_bStaleStartupResources;
	bool m_bUseModPathBinDir;
	bool m_bLibrariesConnected;
	bool m_bEnsureToolStartupManifests;
	bool m_bEnsureVRStartupManifests;
	bool m_bEnsureGameStartupManifests;
	bool m_bEnsureConsoleStartupManifests;
	bool m_bAddonsOnRestricted;
	CInterlockedInt m_nAllowAddonChanges;
	CUtlString m_addonToRestrictTo;
	IUGCAddonPathResolver *m_pUGCAddonPathResolver;
	CUtlString m_LogFilename;
	CUtlString m_LogFileSuffix;
	bool m_bLogToFile;
	// Passed to the log file listener when the log file is opened
	int m_nLogFileFlags;
	CFileLoggingListener m_LogToFileListener;
	// Called once the search paths are set up, before the startup memory warnings are read
	void ( *m_pfnAddSearchPaths )();
};

#endif // TIER2APP_H
