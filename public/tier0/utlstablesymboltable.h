#ifndef UTLSTABLESYMBOLTABLE_H
#define UTLSTABLESYMBOLTABLE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlhashtable.h"
#include "tier0/utlvector.h"
#include "tier0/memblockallocator.h"

//-----------------------------------------------------------------------------
// Case-insensitive symbol table with stable ids, the symbols are stored in a block allocator
//-----------------------------------------------------------------------------
class CStableSymbolTable
{
public:
	DLL_CLASS_IMPORT int GetId( const char *pSymbol, bool *pIsNewSymbol );
	DLL_CLASS_IMPORT void Insert( int nId, const char *pSymbol );

	const char *GetSymbol( int nId ) const { return m_IdToSymbol[ nId ]; }
	int Count() const { return m_IdToSymbol.Count(); }

protected:
	// Case-insensitive MurmurHash2 of the symbol
	struct Hash_t
	{
		uint operator()( const char *pString ) const;
	};

	struct Eq_t
	{
		bool operator()( const char *a, const char *b ) const;
	};

	CUtlHashtable< const char *, int, Hash_t, Eq_t > m_SymbolToId;
	CUtlVector< const char * > m_IdToSymbol;
	CUtlMemoryBlockAllocator< char > m_stringStorage;
};

#endif // UTLSTABLESYMBOLTABLE_H
