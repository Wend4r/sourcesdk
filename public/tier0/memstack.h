//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A fast stack memory allocator that uses virtual memory if available
//
//===========================================================================//

#ifndef MEMSTACK_H
#define MEMSTACK_H

#if defined( _WIN32 )
#pragma once
#endif

#include "platform.h"
#include "utlstring.h"
#include "threadtools.h"
#include "tier1/utlvector.h"

//-----------------------------------------------------------------------------

typedef unsigned MemoryStackMark_t;

class CMemoryStack
{
public:
	DLL_CLASS_IMPORT CMemoryStack();
	DLL_CLASS_IMPORT ~CMemoryStack();

	DLL_CLASS_IMPORT bool Init( const char *name, unsigned int maxSize = 0, unsigned int commitSize = 0, unsigned int initialCommit = 0, unsigned int alignment = 16 );
	DLL_CLASS_IMPORT void Term();

	DLL_CLASS_IMPORT int GetSize();
	int GetMaxSize();
	int GetUsed();
	
	void *Alloc( unsigned bytes, bool bClear = false ) RESTRICT;

	MemoryStackMark_t GetCurrentAllocPoint();
	DLL_CLASS_IMPORT void FreeToAllocPoint( MemoryStackMark_t mark, bool bDecommit = true );
	DLL_CLASS_IMPORT void FreeAll( bool bDecommit = true );
	
	DLL_CLASS_IMPORT void Access( void **ppRegion, unsigned int *pBytes );

	DLL_CLASS_IMPORT void PrintContents();

	void *GetBase();
	const void *GetBase() const {  return const_cast<CMemoryStack *>(this)->GetBase(); }

	bool CommitSize( int );

private:
	DLL_CLASS_IMPORT bool CommitTo( byte * ) RESTRICT;

	byte *m_pNextAlloc;
	byte *m_pCommitLimit;
	byte *m_pAllocLimit;
	byte *m_pHighestAllocLimit;
	byte *m_pBase;

	unsigned int m_maxSize;
	unsigned int m_alignment;
	unsigned int m_commitIncrement;
	unsigned int m_minCommit;

	uint32 m_nAllocStatsId;
	CUtlString m_name;
	CThreadFastMutex m_allocMutex;
};

//-------------------------------------

FORCEINLINE void *CMemoryStack::Alloc( unsigned bytes, bool bClear ) RESTRICT
{
	Assert( m_pBase );

	int alignment = m_alignment;
	if ( bytes )
	{
		bytes = AlignValue( bytes, alignment );
	}
	else
	{
		bytes = alignment;
	}


	void *pResult = m_pNextAlloc;
	byte *pNextAlloc = m_pNextAlloc + bytes;

	if ( pNextAlloc > m_pCommitLimit )
	{
		if ( !CommitTo( pNextAlloc ) )
		{
			return NULL;
		}
	}

	if ( bClear )
	{
		memset( pResult, 0, bytes );
	}

	m_pNextAlloc = pNextAlloc;

	return pResult;
}

//-------------------------------------

inline bool CMemoryStack::CommitSize( int nBytes )
{
	if ( GetSize() != nBytes )
	{
		return CommitTo( m_pBase + nBytes );
	}
	return true;
}

//-------------------------------------

inline int CMemoryStack::GetMaxSize()
{ 
	return m_maxSize;
}

//-------------------------------------

inline int CMemoryStack::GetUsed() 
{ 
	return ( m_pNextAlloc - m_pBase ); 
}

//-------------------------------------

inline void *CMemoryStack::GetBase()
{
	return m_pBase;
}

//-------------------------------------

inline MemoryStackMark_t CMemoryStack::GetCurrentAllocPoint()
{
	return ( m_pNextAlloc - m_pBase );
}


//-----------------------------------------------------------------------------
// The CUtlMemoryStack class:
// A fixed memory class
//-----------------------------------------------------------------------------
template< typename T, typename I, size_t MAX_SIZE, size_t COMMIT_SIZE = 0, size_t INITIAL_COMMIT = 0 >
class CUtlMemoryStack
{
public:
	// constructor, destructor
	CUtlMemoryStack( int nGrowSize = 0, int nInitSize = 0 )	{ m_MemoryStack.Init( MAX_SIZE * sizeof(T), COMMIT_SIZE * sizeof(T), INITIAL_COMMIT * sizeof(T), 4 ); COMPILE_TIME_ASSERT( sizeof(T) % 4 == 0 );	}
	CUtlMemoryStack( T* pMemory, int numElements )			{ Assert( 0 ); 										}

	// Can we use this index?
	bool IsIdxValid( I i ) const							{ long x=i; return (x >= 0) && (x < m_nAllocated); }

	// Specify the invalid ('null') index that we'll only return on failure
	static const I INVALID_INDEX = ( I )-1; // For use with COMPILE_TIME_ASSERT
	static I InvalidIndex() { return INVALID_INDEX; }

	class Iterator_t
	{
		Iterator_t( I i ) : index( i ) {}
		I index;
		friend class CUtlMemoryStack<T,I,MAX_SIZE, COMMIT_SIZE, INITIAL_COMMIT>;
	public:
		bool operator==( const Iterator_t it ) const		{ return index == it.index; }
		bool operator!=( const Iterator_t it ) const		{ return index != it.index; }
	};
	Iterator_t First() const								{ return Iterator_t( m_nAllocated ? 0 : InvalidIndex() ); }
	Iterator_t Next( const Iterator_t &it ) const			{ return Iterator_t( it.index < m_nAllocated ? it.index + 1 : InvalidIndex() ); }
	I GetIndex( const Iterator_t &it ) const				{ return it.index; }
	bool IsIdxAfter( I i, const Iterator_t &it ) const		{ return i > it.index; }
	bool IsValidIterator( const Iterator_t &it ) const		{ long x=it.index; return x >= 0 && x < m_nAllocated; }
	Iterator_t InvalidIterator() const						{ return Iterator_t( InvalidIndex() ); }

	// Gets the base address
	T* Base()												{ return (T*)m_MemoryStack.GetBase(); }
	const T* Base() const									{ return (const T*)m_MemoryStack.GetBase(); }

	// element access
	T& operator[]( I i )									{ Assert( IsIdxValid(i) ); return Base()[i];	}
	const T& operator[]( I i ) const						{ Assert( IsIdxValid(i) ); return Base()[i];	}
	T& Element( I i )										{ Assert( IsIdxValid(i) ); return Base()[i];	}
	const T& Element( I i ) const							{ Assert( IsIdxValid(i) ); return Base()[i];	}

	// Attaches the buffer to external memory....
	void SetExternalBuffer( T* pMemory, int numElements )	{ Assert( 0 ); }

	// Size
	int NumAllocated() const								{ return m_nAllocated; }
	int Count() const										{ return m_nAllocated; }

	// Grows the memory, so that at least allocated + num elements are allocated
	void Grow( int num = 1 )								{ Assert( num > 0 ); m_nAllocated += num; m_MemoryStack.Alloc( num * sizeof(T) ); }

	// Makes sure we've got at least this much memory
	void EnsureCapacity( int num )							{ Assert( num <= MAX_SIZE ); if ( m_nAllocated < num ) Grow( num - m_nAllocated ); }

	// Memory deallocation
	void Purge()											{ m_MemoryStack.FreeAll(); m_nAllocated = 0; }

	// is the memory externally allocated?
	bool IsExternallyAllocated() const						{ return false; }

	// Set the size by which the memory grows
	void SetGrowSize( int size )							{}

private:
	CMemoryStack m_MemoryStack;
	int m_nAllocated;
};

#endif // MEMSTACK_H
