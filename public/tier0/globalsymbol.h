#ifndef GLOBALSYMBOL_H
#define GLOBALSYMBOL_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

class CGlobalSymbol
{
public:
	operator uint64() { return Get(); }
	uint64 Get() { return m_handle; }

private:
	uint64 m_handle;
};

using CGlobalSymbolCaseSensitive = CGlobalSymbol;

PLATFORM_INTERFACE CGlobalSymbol FindGlobalSymbolByHash( uint32 hash );
PLATFORM_INTERFACE CGlobalSymbol FindGlobalSymbol( const char *str );
PLATFORM_INTERFACE CGlobalSymbol MakeGlobalSymbol( const char *str );
PLATFORM_INTERFACE CGlobalSymbolCaseSensitive MakeGlobalSymbolCaseSensitive( const char *str );

#endif // GLOBALSYMBOL_H
