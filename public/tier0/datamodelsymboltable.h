#ifndef TIER0_DATAMODELSYMBOLTABLE_H
#define TIER0_DATAMODELSYMBOLTABLE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// A process-wide reference counted string table; a symbol is the pooled string itself, so equal
// strings give equal pointers
//-----------------------------------------------------------------------------
abstract_class IGlobalSymbolMap
{
public:
	virtual void AddUser() = 0;
	virtual void ReleaseUser() = 0;

	virtual const char *GetSymbol( const char *pString ) = 0;
	virtual const char *GetString( const char *pSymbol ) = 0;
	virtual uint GetSymbolCount() = 0;

	// Null when the string is not in the table
	virtual const char *Find( const char *pString ) = 0;

	// Copies up to nCount symbols from nFirstElement on, returns how many
	virtual int GetElements( int nFirstElement, int nCount, const char **ppElements ) = 0;

	virtual size_t GetMemoryUsage() = 0;
	virtual void Commit() = 0;
};

// The symbols of the datamodel element type names
PLATFORM_INTERFACE IGlobalSymbolMap *GetDmElementTypeSymbolMap();

#endif // TIER0_DATAMODELSYMBOLTABLE_H
