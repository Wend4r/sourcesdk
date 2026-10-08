//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Cache of loaded response rules systems, keyed by file name
//
//===========================================================================//

#ifndef IRESPONSERULESFILECACHE_H
#define IRESPONSERULESFILECACHE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/iappsystem.h"
#include "resourcefile/resourcetype.h"
#include "tier1/random.h"

class IResponseSystem;
class CUtlSymbolTable;
class CUtlBuffer;

abstract_class IResponseRulesFileCache : public IAppSystem
{
public:
	virtual IResponseSystem *LoadResponseSystem( const char *pszFileName ) = 0;

	virtual void RemoveResponseSystem( const char *pszFileName ) = 0;

	virtual IResponseSystem *LoadResponseSystem( ResourceHandle_t hResource ) = 0;
	virtual void RemoveResponseSystem( ResourceHandle_t hResource ) = 0;

	virtual CUtlSymbolTable *GetSymbolTable() = 0;

	// Random stream shared by every cached response system
	virtual CUniformRandomStream *GetRandomStream() = 0;

	virtual bool UpconvertLegacyResponseRules( CUtlBuffer &buf, const char *pszFileName ) = 0;
};

#endif // IRESPONSERULESFILECACHE_H
