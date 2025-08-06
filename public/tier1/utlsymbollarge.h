//===== Copyright ? 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Defines a large symbol table (intp sized handles, can store more than 64k strings)
//
// $Header: $
// $NoKeywords: $
//===========================================================================//

#ifndef UTLSYMBOLLARGE_H
#define UTLSYMBOLLARGE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/basetypes.h"
#include "tier0/threadtools.h"
#include "tier0/memblockallocator.h"
#include "tier0/utlstringtoken.h"
#include "tier1/generichash.h"
#include "tier1/utlcommon.h"
#include "tier1/utlvector.h"
#include "tier1/utlhashtable.h"

//-----------------------------------------------------------------------------
// CUtlSymbolTableLarge:
// description:
//    This class defines a symbol table, which allows us to perform mappings
//    of strings to symbols and back. 
// 
//    This class stores the strings in a series of string pools. The returned CUtlSymbolLarge is just a pointer
//     to the string data, the hash precedes it in memory and is used to speed up searching, etc.
//-----------------------------------------------------------------------------

typedef uint UtlSymLargeId_t;

#define UTL_INVAL_SYMBOL_LARGE ((UtlSymLargeId_t)0)

#define FOR_EACH_SYMBOL_LARGE( table, iter ) \
	for ( UtlSymLargeId_t iter = 0; iter < (table).GetNumStrings(); iter++ )
#define FOR_EACH_SYMBOL_LARGE_BACK( table, iter ) \
	for ( UtlSymLargeId_t iter = (table).GetNumStrings()-1; iter >= 0; iter-- )

class CUtlSymbolLarge
{
public:
	// constructor, destructor
	CUtlSymbolLarge( const char* pString = nullptr ) : u( pString ) {};

	// operator==
	bool operator==( CUtlSymbolLarge const& src ) const { return u.m_addr == src.u.m_addr; }
	bool operator==( const char* pString ) const = delete; // disallow since we don't know if the table this is from was case sensitive or not... maybe we don't care

	// operator!=
	bool operator!=( CUtlSymbolLarge const& src ) const { return u.m_addr != src.u.m_addr; }
	
	// operator<
	bool operator<( CUtlSymbolLarge const& src ) const { return u.m_addr < src.u.m_addr; }

	template< bool CASEINSENSITIVE = true, bool TRACKCREATION = true >
	static uint32 Hash( const char *pString, int nLength = -1 ) { return MakeStringToken2< CASEINSENSITIVE, TRACKCREATION >( pString, nLength ); }

	bool IsValid() const { return u.m_pAsString != nullptr; }
	const char *String() const { return IsValid() ? u.m_pAsString : StringFuncs<char>::EmptyString(); }
	const char *Get() const { return String(); }
	operator const char *() const { return Get(); }

private:
	union Data_t
	{
		Data_t( const char *pString = nullptr ) : m_pAsString( pString ) {}

		uintp m_addr;
		const char *m_pAsString;
	} u;
};

typedef uint32 LargeSymbolTableHashDecoration_t;

// The structure consists of the hash immediately followed by the string data
struct ALIGN8_POST CUtlSymbolTableLargeBaseTreeEntry_t
{
	LargeSymbolTableHashDecoration_t	m_Hash;
	// Variable length string data
	char m_szString[1];

	bool IsEmpty() const
	{
		return ( !m_Hash && !m_szString[0] );
	}

	const char *String() const
	{
		return (const char *)m_szString;
	}

	void Replace( LargeSymbolTableHashDecoration_t nNewHash, const char *pNewString, int nLength )
	{
		m_Hash = nNewHash;
		Q_memcpy( (char *)m_szString, pNewString, nLength );
		m_szString[nLength] = '\0';
	}

	CUtlSymbolLarge ToSymbol() const
	{
		return CUtlSymbolLarge( String() );
	}

	LargeSymbolTableHashDecoration_t HashValue() const
	{
		return m_Hash;
	}
};

// Base Class for threaded and non-threaded types
template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
class CUtlSymbolTableLargeBase
{
public:
	// constructor, destructor
	CUtlSymbolTableLargeBase( int nGrowSize = 0, int nInitSize = 16 )
		:	m_HashTable( 0 ), 
			m_MemBlocks( nGrowSize, nInitSize ), 
			m_Mutex( "CUtlSymbolTableLargeBase" ), 
			m_MemBlockAllocator( ( nInitSize > 0 ) ? 8 : 0, PAGE_SIZE ), 
			m_nElementLimit( INT_MAX - 1 ), 
			m_bThrowError( true ), 
			m_nUsedBytes(0)  { }

	~CUtlSymbolTableLargeBase() { }

	// Finds the symbol for pString
	UtlSymLargeId_t Find( const char* pString, int nLength ) const;
	UtlSymLargeId_t Find( const char* pString ) const { return Find( pString, V_strlen( pString ) ); }

	CUtlSymbolLarge FindString( const char* pString, int nLength ) const { return String( Find( pString, nLength ) ); }
	CUtlSymbolLarge FindString( const char* pString ) const { return FindString( pString, V_strlen( pString ) ); }

	// Finds and/or creates a symbol based on the string
	UtlSymLargeId_t Add( const char* pString, int nLength );
	UtlSymLargeId_t Add( const char* pString ) { return Add( pString, V_strlen( pString ) ); }

	CUtlSymbolLarge AddString( const char* pString, int nLength ) { return String( Add( pString, nLength ) ); }
	CUtlSymbolLarge AddString( const char* pString ) { return String( Add( pString, V_strlen( pString ) ) ); }

	const char*		String( UtlSymLargeId_t id ) const;
	uint32			Hash( UtlSymLargeId_t id ) const;

	int				GetNumStrings() const { return m_MemBlocks.Count(); };

	// Remove all symbols in the table.
	void RemoveAll();
	void Purge();
	
private:
	UtlSymLargeId_t Find( uint32 hash, const char* pString, int nLength ) const;
	UtlSymLargeId_t Add( uint32 hash, const char* pString, int nLength );

	struct UtlSymTableLargeAltKey
	{ 
		const CUtlSymbolTableLargeBase*	m_pTable;
		const char*						m_pString;
		int								m_nLength;
	};

	struct UtlSymTableLargeHashFunctor
	{
		uint32 operator()( UtlSymTableLargeAltKey k ) const
		{
			return CUtlSymbolLarge::Hash< CASEINSENSITIVE >( k.m_pString, k.m_nLength );
		}

		uint32 operator()( UtlSymLargeId_t k ) const
		{
			static const ptrdiff_t tableoffset = (uintp)(&((Hashtable_t*)1024)->GetHashRef()) - 1024;
			static const ptrdiff_t owneroffset = offsetof(CUtlSymbolTableLargeBase, m_HashTable) + tableoffset;
			const CUtlSymbolTableLargeBase* pTable = (const CUtlSymbolTableLargeBase*)((uintp)this - owneroffset);

			return pTable->Hash( k );
		}
	};

	struct UtlSymTableLargeEqualFunctor
	{
		bool operator()( UtlSymLargeId_t a, UtlSymLargeId_t b ) const 
		{ 
			static const ptrdiff_t tableoffset = (uintp)(&((Hashtable_t*)1024)->GetEqualRef()) - 1024;
			static const ptrdiff_t owneroffset = offsetof(CUtlSymbolTableLargeBase, m_HashTable) + tableoffset;
			const CUtlSymbolTableLargeBase* pTable = (const CUtlSymbolTableLargeBase*)((uintp)this - owneroffset);

			if ( CASEINSENSITIVE ) 
				return V_stricmp( pTable->String( a ), pTable->String( b ) ) == 0; 
			else
				return V_strcmp( pTable->String( a ), pTable->String( b ) ) == 0; 
		}

		bool operator()( UtlSymTableLargeAltKey a, UtlSymLargeId_t b ) const 
		{ 
			const char* pString = a.m_pTable->String( b );
			int nLength = ( int )strlen( pString );

			if ( a.m_nLength != nLength )
				return false;

			if ( CASEINSENSITIVE ) 
				return V_strnicmp( a.m_pString, pString, a.m_nLength ) == 0; 
			else
				return V_strncmp( a.m_pString, pString, a.m_nLength ) == 0; 
		}

		bool operator()( UtlSymLargeId_t a, UtlSymTableLargeAltKey b ) const 
		{ 
			return operator()( b, a );
		}
	};

	typedef CUtlHashtable< UtlSymLargeId_t, empty_t, UtlSymTableLargeHashFunctor, UtlSymTableLargeEqualFunctor, UtlSymTableLargeAltKey > Hashtable_t;
	typedef CUtlLeanVector< MemBlockHandle_t > MemBlocksVec_t;

	Hashtable_t						m_HashTable;
	MemBlocksVec_t					m_MemBlocks;
	MUTEX_TYPE						m_Mutex;
	CUtlMemoryBlockAllocator<char>	m_MemBlockAllocator;
	int								m_nElementLimit;
	bool							m_bThrowError;
	int								m_nUsedBytes;
};

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline UtlSymLargeId_t CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::Find( uint32 hash, const char* pString, int nLength ) const
{
	UtlSymTableLargeAltKey key;
	
	key.m_pTable = this;
	key.m_pString = pString;
	key.m_nLength = nLength;

	UtlHashHandle_t h = m_HashTable.Find( key, hash );

	if ( h == m_HashTable.InvalidHandle() )
		return UTL_INVAL_SYMBOL_LARGE;

	return m_HashTable[ h ];
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline UtlSymLargeId_t CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::Find( const char* pString, int nLength ) const
{
	if ( pString && *pString )
	{
		uint32 hash = CUtlSymbolLarge::Hash< CASEINSENSITIVE >( pString, nLength );

		AUTO_LOCK( m_Mutex );

		return Find( hash, pString, nLength );
	}

	return UTL_INVAL_SYMBOL_LARGE;
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline UtlSymLargeId_t CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::Add( uint32 hash, const char* pString, int nLength )
{
	if ( m_MemBlocks.Count() >= m_nElementLimit )
	{
		if ( m_bThrowError )
		{
			Plat_FatalError( "FATAL ERROR: CUtlSymbolTableLarge element limit of %u exceeded\n", m_nElementLimit );
			DebuggerBreak();
		}

		Warning( "ERROR: CUtlSymbolTableLarge element limit of %u exceeded\n", m_nElementLimit );

		return UTL_INVAL_SYMBOL_LARGE;
	}

	MemBlockHandle_t block = m_MemBlockAllocator.Alloc( nLength + sizeof( LargeSymbolTableHashDecoration_t ) + 1 );

	CUtlSymbolTableLargeBaseTreeEntry_t *entry = (CUtlSymbolTableLargeBaseTreeEntry_t *)m_MemBlockAllocator.GetBlock( block );

	entry->Replace( hash, pString, nLength );

	m_nUsedBytes += nLength + 1;

	UtlSymLargeId_t elem = m_MemBlocks.AddToTail( block + sizeof( LargeSymbolTableHashDecoration_t ) );

	m_HashTable.Insert( elem, empty_t(), hash );

	return elem;
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline UtlSymLargeId_t CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::Add( const char* pString, int nLength )
{
	if ( pString && *pString )
	{
		uint32 hash = CUtlSymbolLarge::Hash< CASEINSENSITIVE >( pString, nLength );

		AUTO_LOCK( m_Mutex );

		UtlSymLargeId_t symid = Find( hash, pString, nLength );

		if ( symid != UTL_INVAL_SYMBOL_LARGE )
			return symid;

		return Add( hash, pString, nLength );
	}

	return UTL_INVAL_SYMBOL_LARGE;
}


template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline const char* CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::String( UtlSymLargeId_t elem ) const
{
	return ( const char* )m_MemBlockAllocator.GetBlock( m_MemBlocks[ elem ] );
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline uint32 CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::Hash( UtlSymLargeId_t elem ) const
{
	CUtlSymbolTableLargeBaseTreeEntry_t *entry = (CUtlSymbolTableLargeBaseTreeEntry_t *)m_MemBlockAllocator.GetBlock( m_MemBlocks[ elem ] - sizeof( LargeSymbolTableHashDecoration_t ) );

	return entry->HashValue();
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline void CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::RemoveAll()
{	
	AUTO_LOCK( m_Mutex );

	m_HashTable.RemoveAll();
	m_MemBlocks.RemoveAll();
	m_MemBlockAllocator.RemoveAll();
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline void CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::Purge()
{	
	AUTO_LOCK( m_Mutex );

	m_HashTable.Purge();
	m_MemBlocks.Purge();
	m_MemBlockAllocator.Purge();
}

// Case-sensitive
typedef CUtlSymbolTableLargeBase< false, 2048, CThreadNullMutex > CUtlSymbolTableLarge;
// Case-insensitive
typedef CUtlSymbolTableLargeBase< true, 2048, CThreadNullMutex > CUtlSymbolTableLarge_CI;
// Multi-threaded case-sensitive
typedef CUtlSymbolTableLargeBase< false, 2048, CThreadMutex > CUtlSymbolTableLargeMT;
// Multi-threaded case-insensitive
typedef CUtlSymbolTableLargeBase< true, 2048, CThreadMutex > CUtlSymbolTableLargeMT_CI;

#endif // UTLSYMBOLLARGE_H