#ifndef GLOBALSYMBOL_H
#define GLOBALSYMBOL_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "tier1/utlsymbollarge.h"

using CGlobalSymbol = CUtlSymbolLarge;
using CGlobalSymbolCaseSensitive = CGlobalSymbol;

// tier0 exports these with a leading underscore
PLATFORM_INTERFACE const char *_FindGlobalSymbolByHash( uint32 hash );
PLATFORM_INTERFACE const char *_FindGlobalSymbol( const char *str );
PLATFORM_INTERFACE const char *_MakeGlobalSymbol( const char *str );
PLATFORM_INTERFACE const char *_MakeGlobalSymbolCaseSensitive( const char *str );

inline CGlobalSymbol FindGlobalSymbolByHash( uint32 hash ) { return _FindGlobalSymbolByHash( hash ); }
inline CGlobalSymbol FindGlobalSymbol( const char *str ) { return _FindGlobalSymbol( str ); }
inline CGlobalSymbol MakeGlobalSymbol( const char *str ) { return _MakeGlobalSymbol( str ); }
inline CGlobalSymbolCaseSensitive MakeGlobalSymbolCaseSensitive( const char *str ) { return _MakeGlobalSymbolCaseSensitive( str ); }

#endif // GLOBALSYMBOL_H
