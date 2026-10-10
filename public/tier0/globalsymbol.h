#ifndef GLOBALSYMBOL_H
#define GLOBALSYMBOL_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "tier1/utlsymbollarge.h"

// tier0 exports these with a leading underscore
// Case-insensitive, hash is the CUtlStringToken hash of the string
PLATFORM_INTERFACE const char *_FindGlobalSymbolByHash( uint32 hash );
// Case-insensitive
PLATFORM_INTERFACE const char *_FindGlobalSymbol( const char *str );
// Case-insensitive
PLATFORM_INTERFACE const char *_MakeGlobalSymbol( const char *str );
// Case-sensitive, in a table of its own
PLATFORM_INTERFACE const char *_MakeGlobalSymbolCaseSensitive( const char *str );

// A string interned in tier0's case-insensitive global symbol table, so equal strings share a pointer.
class CGlobalSymbol
{
public:
	CGlobalSymbol() : m_pString( nullptr ) {}

	// User-provided like the engine's, so a CGlobalSymbol is passed and returned by address
	CGlobalSymbol( const CGlobalSymbol &other ) : m_pString( other.m_pString ) {}
	CGlobalSymbol &operator=( const CGlobalSymbol &other ) = default;

	bool operator==( const CGlobalSymbol &other ) const { return m_pString == other.m_pString; }
	bool operator!=( const CGlobalSymbol &other ) const { return m_pString != other.m_pString; }

	bool IsValid() const { return m_pString != nullptr; }
	const char *String() const { return m_pString ? m_pString : ""; }

	operator CUtlSymbolLarge() const { return CUtlSymbolLarge( m_pString ); }

private:
	explicit CGlobalSymbol( const char *pString ) : m_pString( pString ) {}

	friend CGlobalSymbol FindGlobalSymbolByHash( uint32 hash );
	friend CGlobalSymbol FindGlobalSymbol( const char *str );
	friend CGlobalSymbol MakeGlobalSymbol( const char *str );

	const char *m_pString;
};

inline CGlobalSymbol FindGlobalSymbolByHash( uint32 hash ) { return CGlobalSymbol( _FindGlobalSymbolByHash( hash ) ); }
inline CGlobalSymbol FindGlobalSymbol( const char *str ) { return CGlobalSymbol( _FindGlobalSymbol( str ) ); }
inline CGlobalSymbol MakeGlobalSymbol( const char *str ) { return CGlobalSymbol( _MakeGlobalSymbol( str ) ); }

// A string interned in tier0's case-sensitive global symbol table, not interchangeable with CGlobalSymbol.
class CGlobalSymbolCaseSensitive
{
public:
	CGlobalSymbolCaseSensitive() : m_pString( nullptr ) {}

	// Assumed to match CGlobalSymbol, no engine function passes this type by value
	CGlobalSymbolCaseSensitive( const CGlobalSymbolCaseSensitive &other ) : m_pString( other.m_pString ) {}
	CGlobalSymbolCaseSensitive &operator=( const CGlobalSymbolCaseSensitive &other ) = default;

	bool operator==( const CGlobalSymbolCaseSensitive &other ) const { return m_pString == other.m_pString; }
	bool operator!=( const CGlobalSymbolCaseSensitive &other ) const { return m_pString != other.m_pString; }

	bool IsValid() const { return m_pString != nullptr; }
	const char *String() const { return m_pString ? m_pString : ""; }

	operator CUtlSymbolLarge() const { return CUtlSymbolLarge( m_pString ); }

private:
	explicit CGlobalSymbolCaseSensitive( const char *pString ) : m_pString( pString ) {}

	friend CGlobalSymbolCaseSensitive MakeGlobalSymbolCaseSensitive( const char *str );

	const char *m_pString;
};

inline CGlobalSymbolCaseSensitive MakeGlobalSymbolCaseSensitive( const char *str ) { return CGlobalSymbolCaseSensitive( _MakeGlobalSymbolCaseSensitive( str ) ); }

#endif // GLOBALSYMBOL_H
