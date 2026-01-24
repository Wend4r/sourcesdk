#ifndef GLOBALSYMBOL_H
#define GLOBALSYMBOL_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "tier1/utlsymbollarge.h"

using CGlobalSymbol = CUtlSymbolLarge;
using CGlobalSymbolCaseSensitive = CGlobalSymbol;

PLATFORM_INTERFACE CGlobalSymbol FindGlobalSymbolByHash( uint32 hash );
PLATFORM_INTERFACE CGlobalSymbol FindGlobalSymbol( const char *str );
PLATFORM_INTERFACE CGlobalSymbol MakeGlobalSymbol( const char *str );
PLATFORM_INTERFACE CGlobalSymbolCaseSensitive MakeGlobalSymbolCaseSensitive( const char *str );

#endif // GLOBALSYMBOL_H
