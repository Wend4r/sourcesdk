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
#ifdef PLATFORM_OSX
	// The macOS tier0 has a second Find here; the name is ours
	virtual const char *FindSymbol( const char *pString ) = 0;
#endif
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

//-----------------------------------------------------------------------------
// A user of a symbol map for the lifetime of the object
//-----------------------------------------------------------------------------
class CGlobalSymbolMapUser
{
public:
	explicit CGlobalSymbolMapUser( IGlobalSymbolMap *pMap ) : m_pMap( pMap ) { m_pMap->AddUser(); }
	~CGlobalSymbolMapUser() { m_pMap->ReleaseUser(); }

	CGlobalSymbolMapUser( const CGlobalSymbolMapUser & ) = delete;
	CGlobalSymbolMapUser &operator=( const CGlobalSymbolMapUser & ) = delete;

	IGlobalSymbolMap *Get() const { return m_pMap; }
	IGlobalSymbolMap *operator->() const { return m_pMap; }

private:
	IGlobalSymbolMap *m_pMap;
};

#endif // TIER0_DATAMODELSYMBOLTABLE_H
