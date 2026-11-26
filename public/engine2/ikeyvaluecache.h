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
#include <tier1/utlvector.h>

class KeyValues;

abstract_class IKeyValueCache : public IAppSystem
{
public:
	virtual KeyValues *LoadCachedKeyValues( const char *pszFileName, bool bUsesEscapeSequences, const char *pszRootName, const char *pszPathID ) = 0;

	// The files are loaded in order and merged into the first one.
	virtual KeyValues *LoadCachedKeyValuesList( CUtlVector< const char * > &fileNames, bool bUsesEscapeSequences, const char *pszRootName, const char *pszPathID ) = 0;

	virtual KeyValues *FlushReloadCachedKeyValues( const char *pszFileName, bool bUsesEscapeSequences, const char *pszRootName, const char *pszPathID ) = 0;

	// Drops one reference; the KeyValues are deleted with the last one.
	virtual bool ReleaseCachedKeyValues( KeyValues *pKeyValues ) = 0;

	virtual bool IsKeyValueListInCache( CUtlVector< const char * > &fileNames ) = 0;

	// Named by what it does: hashes the file list and counts one more reference to that hash in a list of its own
	// (separate from the cached KeyValues).
	virtual void AddKeyValueListReference( CUtlVector< const char * > &fileNames ) = 0;
};

#endif // IKEYVALUECACHE_H
