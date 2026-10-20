#ifndef TIER0_DATAMODELSYMBOLTABLE_H
#define TIER0_DATAMODELSYMBOLTABLE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlsymbollarge.h"

//-----------------------------------------------------------------------------
// A process-wide reference counted string table of symbols
//-----------------------------------------------------------------------------
abstract_class IGlobalSymbolMap
{
public:
	virtual void AddUser() = 0;
	virtual void ReleaseUser() = 0;

	virtual UtlSymLargeId_t GetSymbol( const char *pString ) = 0;
	virtual const char *GetString( UtlSymLargeId_t symbol ) = 0;
	virtual uint GetSymbolCount() = 0;
	virtual UtlSymLargeId_t Find( const char *pString ) = 0;
	virtual int GetElements( int nFirstElement, int nCount, UtlSymLargeId_t *pElements ) = 0;
	virtual size_t GetMemoryUsage() = 0;
	virtual void Commit() = 0;
};

// The symbols of the datamodel element type names
PLATFORM_INTERFACE IGlobalSymbolMap *GetDmElementTypeSymbolMap();

#endif // TIER0_DATAMODELSYMBOLTABLE_H
