//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Reference-counted cache of KeyValues loaded from files
//
//=============================================================================//

#ifndef IKEYVALUECACHE_H
#define IKEYVALUECACHE_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>

class KeyValues;

abstract_class IKeyValueCache : public IAppSystem
{
public:
	virtual KeyValues *LoadKeyValues( const char *pszFileName, bool bUsesEscapeSequences, const char *pszRootName, const char *pszPathID ) = 0;

	virtual KeyValues *LoadKeyValuesMerged( const void *pFileNames, bool bUsesEscapeSequences, const char *pszRootName, const char *pszPathID ) = 0;

	virtual KeyValues *ReloadKeyValues( const char *pszFileName, bool bUsesEscapeSequences, const char *pszRootName, const char *pszPathID ) = 0;

	virtual bool ReleaseKeyValues( KeyValues *pKeyValues ) = 0;

	virtual bool IsKeyValuesMergedCached( const void *pFileNames ) = 0;

	virtual void InvalidateKeyValuesMerged( const void *pFileNames ) = 0;
};

#endif // IKEYVALUECACHE_H
