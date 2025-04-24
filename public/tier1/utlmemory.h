//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//
// A growable memory class.
//===========================================================================//

#ifndef UTLMEMORY_H
#define UTLMEMORY_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/dbg.h"
#include <string.h>
#include "tier0/platform.h"
#include "tier0/rawallocator.h"
#include "tier0/utlmemory.h"

#include "tier0/memalloc.h"
#include "mathlib/mathlib.h"

#include "tier0/memdbgon.h"

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

#ifdef _MSC_VER
#pragma warning (disable:4100)
#pragma warning (disable:4514)
#endif

//-----------------------------------------------------------------------------


#ifdef UTLMEMORY_TRACK
#define UTLMEMORY_TRACK_ALLOC()		MemAlloc_RegisterAllocation( "||Sum of all UtlMemory||", 0, m_nAllocationCount * sizeof(T), m_nAllocationCount * sizeof(T), 0 )
#define UTLMEMORY_TRACK_FREE()		if ( !m_pMemory ) ; else MemAlloc_RegisterDeallocation( "||Sum of all UtlMemory||", 0, m_nAllocationCount * sizeof(T), m_nAllocationCount * sizeof(T), 0 )
#else
#define UTLMEMORY_TRACK_ALLOC()		((void)0)
#define UTLMEMORY_TRACK_FREE()		((void)0)
#endif

// Used in debug mode.
#define UTLMEMORY_BAD_ADDRESS 0xFEFEBAAD
#define UTLMEMORY_BAD_LENGTH 0xFEFEBAAD


//-----------------------------------------------------------------------------
// The CUtlMemory class:
// A growable memory class which doubles in size by default.
//-----------------------------------------------------------------------------
template< class T, class I = int >
class CUtlMemory
{
	template< class A, class B, class C> friend class CUtlVector;
	template< class A, int C> friend class CUtlVectorFixedGrowableCompat;

	template< class A, class C> friend class CUtlMemory_RawAllocator;

public:
	// constructor, destructor
	CUtlMemory( I nGrowSize = 0, I nInitSize = 0 );
	CUtlMemory( T* pMemory, I numElements );
	CUtlMemory( const T* pMemory, I numElements );
	~CUtlMemory();

	CUtlMemory( const CUtlMemory &copyFrom );
	CUtlMemory( CUtlMemory &&moveFrom );
	CUtlMemory &operator=( const CUtlMemory &copyFrom );
	CUtlMemory &operator=( CUtlMemory &&moveFrom );

	CUtlMemory<T,I> &CopyFrom( const CUtlMemory &copyFrom );
	CUtlMemory<T,I> &MoveFrom( CUtlMemory &&moveFrom );

	// Set the size by which the memory grows
	void Init( I nGrowSize = 0, I nInitSize = 0 );

	class Iterator_t
	{
	public:
		Iterator_t( I i ) : index( i ) {}
		I index;

		bool operator==( const Iterator_t it ) const	{ return index == it.index; }
		bool operator!=( const Iterator_t it ) const	{ return index != it.index; }
	};
	Iterator_t First() const							{ return Iterator_t( IsIdxValid( 0 ) ? 0 : InvalidIndex() ); }
	Iterator_t Next( const Iterator_t &it ) const		{ return Iterator_t( IsIdxValid( it.index + 1 ) ? it.index + 1 : InvalidIndex() ); }
	I GetIndex( const Iterator_t &it ) const			{ return it.index; }
	bool IsIdxAfter( I i, const Iterator_t &it ) const	{ return i > it.index; }
	bool IsValidIterator( const Iterator_t &it ) const	{ return IsIdxValid( it.index ); }
	Iterator_t InvalidIterator() const					{ return Iterator_t( InvalidIndex() ); }

	// element access
	T& operator[]( I i );
	const T& operator[]( I i ) const;
	T& Element( I i );
	const T& Element( I i ) const;

	// Can we use this index?
	bool IsIdxValid( I i ) const;

	// Specify the invalid ('null') index that we'll only return on failure
#if __cplusplus >= 201703L
	static inline const I INVALID_INDEX = ( I )-1; // For use with COMPILE_TIME_ASSERT
#else
	static const I INVALID_INDEX = ( I )-1; // For use with COMPILE_TIME_ASSERT
#endif
	static I InvalidIndex() { return INVALID_INDEX; }

	// Gets the base address (can change when adding elements!)
	T* Base();
	const T* Base() const;

	// Attaches the buffer to external memory....
	void SetExternalBuffer( T* pMemory, I numElements );
	void SetExternalBuffer( const T* pMemory, I numElements );
	void AssumeMemory( T *pMemory, I nSize );
	T* Detach();
	void *DetachMemory();

	// Fast swap
	void Swap( CUtlMemory< T, I > &mem );

	// Switches the buffer from an external memory buffer to a reallocatable buffer
	// Will copy the current contents of the external buffer to the reallocatable buffer
	void ConvertToGrowableMemory( I nGrowSize );

	// Size
	I NumAllocated() const;
	I Count() const;

	// Grows the memory, so that at least allocated + num elements are allocated
	void Grow( I num = 1 );

	// Makes sure we've got at least this much memory
	void EnsureCapacity( I num );

	// Memory deallocation
	void Purge();

	// Purge all but the given number of elements
	void Purge( I numElements );

	// is the memory externally allocated?
	bool IsExternallyAllocated() const;

	// is the memory read only?
	bool IsReadOnly() const;

	// Set the size by which the memory grows
	void SetGrowSize( I size );

protected:
	void ValidateGrowSize()
	{
#ifdef _X360
		if ( m_nGrowSize && m_nGrowSize != EXTERNAL_BUFFER_MARKER )
		{
			// Max grow size at 128 bytes on XBOX
			const I MAX_GROW = 128;
			if ( m_nGrowSize * sizeof(T) > MAX_GROW )
			{
				m_nGrowSize = max( 1, MAX_GROW / sizeof(T) );
			}
		}
#endif
	}

	T* m_pMemory;

	uint32 m_nAllocationCount;

	union
	{
		struct
		{
			uint32 m_nGrowSize : 30;
			uint32 m_bHasConstMemory : 1;
			uint32 m_bExternal : 1;
		};

		uint32 m_nGrowSizeStaff;
	};
};


//-----------------------------------------------------------------------------
// The CUtlMemory class:
// A growable memory class which doubles in size by default.
//-----------------------------------------------------------------------------
template< class T, size_t SIZE, class I = int >
class CUtlMemoryFixedGrowable : public CUtlMemory< T, I >
{
	typedef CUtlMemory< T, I > BaseClass;

public:
	CUtlMemoryFixedGrowable( I nGrowSize = 0, I nInitSize = SIZE ) : BaseClass( m_pFixedMemory, SIZE ) 
	{
		Assert( nInitSize == 0 || nInitSize == SIZE );
		// m_nMallocGrowSize = nGrowSize;
	}

	void Grow( I nCount = 1 )
	{
		// if ( this->IsExternallyAllocated() )
		// {
		// 	this->ConvertToGrowableMemory( m_nMallocGrowSize );
		// }
		BaseClass::Grow( nCount );
	}

	void EnsureCapacity( I num )
	{
		if ( BaseClass::m_nAllocationCount >= num )
			return;

		// if ( this->IsExternallyAllocated() )
		// {
		// 	// Can't grow a buffer whose memory was externally allocated 
		// 	this->ConvertToGrowableMemory( m_nMallocGrowSize );
		// }

		BaseClass::EnsureCapacity( num );
	}

private:
	// I m_nMallocGrowSize;
	T m_pFixedMemory[ SIZE ];
};

//-----------------------------------------------------------------------------
// The CUtlMemoryFixed class:
// A fixed memory class
//-----------------------------------------------------------------------------
template< typename T, size_t SIZE, size_t nAlignment = 0 >
class CUtlMemoryFixed
{
public:
	// constructor, destructor
	CUtlMemoryFixed( size_t nGrowSize = 0, size_t nInitSize = 0 )	{ Assert( nInitSize == 0 || nInitSize == SIZE ); 	}
	CUtlMemoryFixed( T* pMemory, size_t numElements )			{ Assert( 0 ); 										}

	// Can we use this index?
	bool IsIdxValid( size_t i ) const							{ return (i >= 0) && (i < SIZE) && (!IsDebug() || i != UTLMEMORY_BAD_LENGTH); }

	// Specify the invalid ('null') index that we'll only return on failure
	static const size_t INVALID_INDEX = -1; // For use with COMPILE_TIME_ASSERT
	static size_t InvalidIndex() { return INVALID_INDEX; }

	// Gets the base address
	T* Base()													{ if ( nAlignment == 0 ) return (T*)(&m_Memory[0]); else return (T*)AlignValue( &m_Memory[0], nAlignment ); }
	const T* Base() const										{ if ( nAlignment == 0 ) return (T*)(&m_Memory[0]); else return (T*)AlignValue( &m_Memory[0], nAlignment ); }

	// element access
	T& operator[]( size_t i )									{ Assert( IsIdxValid(i) ); return Base()[i];	}
	const T& operator[]( size_t i ) const						{ Assert( IsIdxValid(i) ); return Base()[i];	}
	T& Element( size_t i )										{ Assert( IsIdxValid(i) ); return Base()[i];	}
	const T& Element( size_t i ) const							{ Assert( IsIdxValid(i) ); return Base()[i];	}

	// Attaches the buffer to external memory....
	void SetExternalBuffer( T* pMemory, size_t numElements )	{ Assert( 0 ); }

	// Size
	size_t NumAllocated() const									{ return SIZE; }
	size_t Count() const										{ return SIZE; }

	// Grows the memory, so that at least allocated + num elements are allocated
	void Grow( size_t num = 1 )									{ Assert( 0 ); }

	// Makes sure we've got at least this much memory
	void EnsureCapacity( size_t num )							{ Assert( num <= SIZE ); }

	// Memory deallocation
	void Purge()												{}

	// Purge all but the given number of elements (NOT IMPLEMENTED IN CUtlMemoryFixed)
	void Purge( size_t numElements )							{ Assert( 0 ); }

	// is the memory externally allocated?
	bool IsExternallyAllocated() const							{ return false; }

	// Set the size by which the memory grows
	void SetGrowSize( size_t size )								{}

	class Iterator_t
	{
	public:
		Iterator_t( size_t i ) : index( i ) {}
		size_t index;
		bool operator==( const Iterator_t it ) const	{ return index == it.index; }
		bool operator!=( const Iterator_t it ) const	{ return index != it.index; }
	};
	Iterator_t First() const							{ return Iterator_t( IsIdxValid( 0 ) ? 0 : InvalidIndex() ); }
	Iterator_t Next( const Iterator_t &it ) const		{ return Iterator_t( IsIdxValid( it.index + 1 ) ? it.index + 1 : InvalidIndex() ); }
	size_t GetIndex( const Iterator_t &it ) const			{ return it.index; }
	bool IsIdxAfter( size_t i, const Iterator_t &it ) const { return i > it.index; }
	bool IsValidIterator( const Iterator_t &it ) const	{ return IsIdxValid( it.index ); }
	Iterator_t InvalidIterator() const					{ return Iterator_t( InvalidIndex() ); }

private:
	char m_Memory[ SIZE*sizeof(T) + nAlignment ];
};

#ifdef _LINUX
#define REMEMBER_ALLOC_SIZE_FOR_VALGRIND 1
#endif

//-----------------------------------------------------------------------------
// The CUtlMemoryConservative class:
// A dynamic memory class that tries to minimize overhead (itself small, no custom grow factor)
//-----------------------------------------------------------------------------
template< typename T, typename I = int >
class CUtlMemoryConservative
{

public:
	// constructor, destructor
	CUtlMemoryConservative( I nGrowSize = 0, I nInitSize = 0 ) : m_pMemory( NULL )
	{
#ifdef REMEMBER_ALLOC_SIZE_FOR_VALGRIND
		m_nCurAllocSize = 0;
#endif

	}
	CUtlMemoryConservative( T* pMemory, I numElements )								{ Assert( 0 ); }
	~CUtlMemoryConservative()								{ if ( m_pMemory ) free( m_pMemory ); }

	// Can we use this index?
	bool IsIdxValid( I i ) const							{ return ( IsDebug() ) ? ( i >= 0 && i < NumAllocated() ) : ( i >= 0 ); }
	static I InvalidIndex()								{ return -1; }

	// Gets the base address
	T* Base()												{ return m_pMemory; }
	const T* Base() const									{ return m_pMemory; }

	// element access
	T& operator[]( I i )									{ Assert( IsIdxValid(i) ); return Base()[i];	}
	const T& operator[]( I i ) const						{ Assert( IsIdxValid(i) ); return Base()[i];	}
	T& Element( I i )										{ Assert( IsIdxValid(i) ); return Base()[i];	}
	const T& Element( I i ) const							{ Assert( IsIdxValid(i) ); return Base()[i];	}

	// Attaches the buffer to external memory....
	void SetExternalBuffer( T* pMemory, I numElements )	{ Assert( 0 ); }

	// Size
	FORCEINLINE void RememberAllocSize( size_t sz )
	{
#ifdef REMEMBER_ALLOC_SIZE_FOR_VALGRIND
		m_nCurAllocSize = sz;
#endif
	}

	size_t AllocSize( void ) const
	{
#ifdef REMEMBER_ALLOC_SIZE_FOR_VALGRIND
		return m_nCurAllocSize;
#else
		return ( m_pMemory ) ? g_pMemAlloc->GetSize( m_pMemory ) : 0;
#endif
	}

	I NumAllocated() const
	{
		return AllocSize() / sizeof( T );
	}
	I Count() const
	{
		return NumAllocated();
	}

	FORCEINLINE void ReAlloc( size_t sz )
	{
		m_pMemory = (T*)realloc( m_pMemory, sz );
		RememberAllocSize( sz );
	}
	// Grows the memory, so that at least allocated + num elements are allocated
	void Grow( I num = 1 )
	{
		I nCurN = NumAllocated();
		ReAlloc( ( nCurN + num ) * sizeof( T ) );
	}

	// Makes sure we've got at least this much memory
	void EnsureCapacity( I num )
	{
		size_t nSize = sizeof( T ) * MAX( num, Count() );
		ReAlloc( nSize );
	}

	// Memory deallocation
	void Purge()
	{
		free( m_pMemory ); 
		RememberAllocSize( 0 );
		m_pMemory = NULL; 
	}

	// Purge all but the given number of elements
	void Purge( I numElements )							{ ReAlloc( numElements * sizeof(T) ); }

	// is the memory externally allocated?
	bool IsExternallyAllocated() const						{ return false; }

	// Set the size by which the memory grows
	void SetGrowSize( I size )							{}

	class Iterator_t
	{
	public:
		Iterator_t( I i, I _limit ) : index( i ), limit( _limit ) {}
		I index;
		I limit;
		bool operator==( const Iterator_t it ) const	{ return index == it.index; }
		bool operator!=( const Iterator_t it ) const	{ return index != it.index; }
	};
	Iterator_t First() const							{ I limit = NumAllocated(); return Iterator_t( limit ? 0 : InvalidIndex(), limit ); }
	Iterator_t Next( const Iterator_t &it ) const		{ return Iterator_t( ( it.index + 1 < it.limit ) ? it.index + 1 : InvalidIndex(), it.limit ); }
	I GetIndex( const Iterator_t &it ) const			{ return it.index; }
	bool IsIdxAfter( I i, const Iterator_t &it ) const { return i > it.index; }
	bool IsValidIterator( const Iterator_t &it ) const	{ return IsIdxValid( it.index ) && ( it.index < it.limit ); }
	Iterator_t InvalidIterator() const					{ return Iterator_t( InvalidIndex(), 0 ); }

private:
	T *m_pMemory;
#ifdef REMEMBER_ALLOC_SIZE_FOR_VALGRIND
	size_t m_nCurAllocSize;
#endif

};


//-----------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------

template< class T, class I >
CUtlMemory<T,I>::CUtlMemory( I nGrowSize, I nInitAllocationCount ) : m_pMemory(0), 
	m_nAllocationCount( nInitAllocationCount ), m_nGrowSize( nGrowSize ), m_bHasConstMemory( false ), m_bExternal( false )
{
	ValidateGrowSize();
	Assert( nGrowSize >= 0 );
	if (m_nAllocationCount)
	{
		UTLMEMORY_TRACK_ALLOC();
		MEM_ALLOC_CREDIT_CLASS();
		m_pMemory = (T*)malloc( m_nAllocationCount * sizeof(T) );
	}
}

template< class T, class I >
CUtlMemory<T,I>::CUtlMemory( T* pMemory, I numElements ) : m_pMemory(pMemory),
	m_nAllocationCount( numElements ), m_nGrowSize( 0 ), m_bHasConstMemory( false ), m_bExternal( true )
{
}

template< class T, class I >
CUtlMemory<T,I>::CUtlMemory( const T* pMemory, I numElements ) : m_pMemory( (T*)pMemory ),
	m_nAllocationCount( numElements ), m_nGrowSize( 0 ), m_bHasConstMemory( true ), m_bExternal( false )
{
}

template< class T, class I >
CUtlMemory<T,I>::~CUtlMemory()
{
	Purge();

#ifdef _DEBUG
	m_pMemory = (T*)(size_t)UTLMEMORY_BAD_ADDRESS;
	m_nAllocationCount = (I)UTLMEMORY_BAD_LENGTH;
#endif
}

template< class T, class I >
CUtlMemory<T,I>::CUtlMemory( const CUtlMemory &copyFrom ) : m_pMemory( 0 ), m_nAllocationCount( 0 )
{
	CopyFrom( copyFrom );
}


template< class T, class I >
CUtlMemory<T,I>::CUtlMemory( CUtlMemory &&moveFrom )
{
	MoveFrom( Move( moveFrom ) );
}

template< class T, class I >
CUtlMemory<T,I>& CUtlMemory<T,I>::operator=( const CUtlMemory &copyFrom )
{
	return CopyFrom( copyFrom );
}

template< class T, class I >
CUtlMemory<T,I>& CUtlMemory<T,I>::operator=( CUtlMemory&& moveFrom )
{
	return MoveFrom( Move( moveFrom ) );
}

template< class T, class I >
CUtlMemory<T,I> &CUtlMemory<T,I>::CopyFrom( const CUtlMemory &copyFrom )
{
	Init( copyFrom.m_nGrowSize, copyFrom.m_nAllocationCount );

	if( m_pMemory )
	{
		memcpy( (void*)m_pMemory, (const void*)copyFrom.m_pMemory, copyFrom.m_nAllocationCount * sizeof(T) );
	}

	return *this;
}

template< class T, class I >
CUtlMemory<T,I> &CUtlMemory<T,I>::MoveFrom( CUtlMemory &&moveFrom )
{
	// Copy member variables to locals before purge to handle self-assignment
	T*&&pMemory = Move( moveFrom.m_pMemory );
	I &&nAllocationCount = Move( moveFrom.m_nAllocationCount );
	I &&nGrowSizeStaff = Move( moveFrom.m_nGrowSizeStaff );

	moveFrom.m_pMemory = nullptr;
	moveFrom.m_nAllocationCount = 0;
	moveFrom.m_nGrowSizeStaff = 0;

	// // If this is a self-assignment, Purge() is a no-op here
	// Purge();

	m_pMemory = pMemory;
	m_nAllocationCount = nAllocationCount;
	m_nGrowSizeStaff = nGrowSizeStaff;
	return *this;
}

template< class T, class I >
void CUtlMemory<T,I>::Init( I nGrowSize /*= 0*/, I nInitSize /*= 0*/ )
{
	Purge();

	m_nGrowSize = nGrowSize;
	m_bHasConstMemory = false;
	m_bExternal = false;

	m_nAllocationCount = nInitSize;

	ValidateGrowSize();
	Assert( nGrowSize >= 0 );

	if (m_nAllocationCount)
	{
		UTLMEMORY_TRACK_ALLOC();
		MEM_ALLOC_CREDIT_CLASS();
		m_pMemory = (T*)malloc( m_nAllocationCount * sizeof(T) );
	}
}

//-----------------------------------------------------------------------------
// Fast swap
//-----------------------------------------------------------------------------
template< class T, class I >
void CUtlMemory<T,I>::Swap( CUtlMemory<T,I> &mem )
{
	V_swap( m_pMemory, mem.m_pMemory );
	V_swap( m_nAllocationCount, mem.m_nAllocationCount );
	V_swap( m_nGrowSizeStaff, mem.m_nGrowSizeStaff );
}


//-----------------------------------------------------------------------------
// Switches the buffer from an external memory buffer to a reallocatable buffer
//-----------------------------------------------------------------------------
template< class T, class I >
void CUtlMemory<T,I>::ConvertToGrowableMemory( I nGrowSize )
{
	if ( !IsExternallyAllocated() )
		return;

	m_nGrowSize = nGrowSize;
	m_bHasConstMemory = false;
	m_bExternal = false;

	if (m_nAllocationCount)
	{
		UTLMEMORY_TRACK_ALLOC();
		MEM_ALLOC_CREDIT_CLASS();

		I nNumBytes = m_nAllocationCount * sizeof(T);
		T *pMemory = (T*)malloc( nNumBytes );
		memcpy( pMemory, m_pMemory, nNumBytes ); 
		m_pMemory = pMemory;
	}
	else
	{
		m_pMemory = NULL;
	}
}


//-----------------------------------------------------------------------------
// Attaches the buffer to external memory....
//-----------------------------------------------------------------------------
template< class T, class I >
void CUtlMemory<T,I>::SetExternalBuffer( T* pMemory, I numElements )
{
	// Blow away any existing allocated memory
	Purge();

	m_pMemory = pMemory;
	m_nAllocationCount = numElements;

	// Indicate that we don't own the memory
	m_nGrowSize = 0;
	m_bHasConstMemory = false;
	m_bExternal = true;
}

template< class T, class I >
void CUtlMemory<T,I>::SetExternalBuffer( const T* pMemory, I numElements )
{
	// Blow away any existing allocated memory
	Purge();

	m_pMemory = const_cast<T*>( pMemory );
	m_nAllocationCount = numElements;

	// Indicate that we don't own the memory
	m_nGrowSize = 0;
	m_bHasConstMemory = true;

	m_bExternal = false;
}

template< class T, class I >
void CUtlMemory<T,I>::AssumeMemory( T* pMemory, I numElements )
{
	// Blow away any existing allocated memory
	Purge();

	// Simply take the pointer but don't mark us as external
	m_pMemory = pMemory;
	m_nAllocationCount = numElements;

	m_bHasConstMemory = false;
	m_bExternal = false;
}

template< class T, class I >
void *CUtlMemory<T,I>::DetachMemory()
{
	if ( IsExternallyAllocated() )
		return NULL;

	void *pMemory = m_pMemory;
	m_pMemory = 0;
	m_nAllocationCount = 0;
	return pMemory;
}

template< class T, class I >
inline T* CUtlMemory<T,I>::Detach()
{
	return (T*)DetachMemory();
}


//-----------------------------------------------------------------------------
// element access
//-----------------------------------------------------------------------------
template< class T, class I >
inline T& CUtlMemory<T,I>::operator[]( I i )
{
	Assert( !IsReadOnly() );
	Assert( IsIdxValid(i) );
	return m_pMemory[i];
}

template< class T, class I >
inline const T& CUtlMemory<T,I>::operator[]( I i ) const
{
	Assert( IsIdxValid(i) );
	return m_pMemory[i];
}

template< class T, class I >
inline T& CUtlMemory<T,I>::Element( I i )
{
	Assert( !IsReadOnly() );
	Assert( IsIdxValid(i) );
	return m_pMemory[i];
}

template< class T, class I >
inline const T& CUtlMemory<T,I>::Element( I i ) const
{
	Assert( IsIdxValid(i) );
	return m_pMemory[i];
}


//-----------------------------------------------------------------------------
// is the memory externally allocated?
//-----------------------------------------------------------------------------
template< class T, class I >
bool CUtlMemory<T,I>::IsExternallyAllocated() const
{
	return ( m_bHasConstMemory | m_bExternal ) != 0;
}


//-----------------------------------------------------------------------------
// is the memory read only?
//-----------------------------------------------------------------------------
template< class T, class I >
bool CUtlMemory<T,I>::IsReadOnly() const
{
	return ( m_bHasConstMemory ) != 0;
}


template< class T, class I >
void CUtlMemory<T,I>::SetGrowSize( I nSize )
{
	Assert( !IsExternallyAllocated() );
	Assert( nSize >= 0 );
	m_nGrowSize = nSize;
	ValidateGrowSize();
}


//-----------------------------------------------------------------------------
// Gets the base address (can change when adding elements!)
//-----------------------------------------------------------------------------
template< class T, class I >
inline T* CUtlMemory<T,I>::Base()
{
	Assert( !IsReadOnly() );
	return m_pMemory;
}

template< class T, class I >
inline const T *CUtlMemory<T,I>::Base() const
{
	return m_pMemory;
}


//-----------------------------------------------------------------------------
// Size
//-----------------------------------------------------------------------------
template< class T, class I >
inline I CUtlMemory<T,I>::NumAllocated() const
{
	return m_nAllocationCount;
}

template< class T, class I >
inline I CUtlMemory<T,I>::Count() const
{
	return m_nAllocationCount;
}


//-----------------------------------------------------------------------------
// Is element index valid?
//-----------------------------------------------------------------------------
template< class T, class I >
inline bool CUtlMemory<T,I>::IsIdxValid( I i ) const
{
	I x = i;
	return ( x >= 0 ) && ( x < m_nAllocationCount ) && ( !IsDebug() || x != ( I )UTLMEMORY_BAD_LENGTH );
}

template< class T, class I >
void CUtlMemory<T,I>::Grow( I num )
{
	Assert( num > 0 );

	if ( IsReadOnly() )
	{
		// Can't grow a buffer whose memory was externally allocated 
		Assert(0);
		return;
	}

	if ( ( ( int64 )m_nAllocationCount + num ) > INT_MAX )
		UtlMemory_FailedAllocation( m_nAllocationCount, num );

	// Make sure we have at least numallocated + num allocations.
	// Use the grow rules specified for this memory (in m_nGrowSize)
	int nAllocationRequested = m_nAllocationCount + num;

	UTLMEMORY_TRACK_FREE();

	int nNewAllocationCount = ( I )UtlMemory_CalcNewAllocationCount( ( int )m_nAllocationCount, ( int )( m_nGrowSize ), ( int )nAllocationRequested, ( int )sizeof(T) );

	// if m_nAllocationRequested wraps index type I, recalculate
	if ( nNewAllocationCount < nAllocationRequested )
	{
		if ( nNewAllocationCount == 0 && ( int )( I )( nNewAllocationCount - 1 ) >= nAllocationRequested )
		{
			--nNewAllocationCount; // deal w/ the common case of m_nAllocationCount == MAX_USHORT + 1
		}
		else
		{
			if ( ( I )nAllocationRequested != nAllocationRequested )
			{
				// we've been asked to grow memory to a size s.t. the index type can't address the requested amount of memory
				Assert( 0 );
				return;
			}
			while ( nNewAllocationCount < nAllocationRequested )
			{
				nNewAllocationCount = ( nNewAllocationCount + nAllocationRequested ) / 2;
			}
		}
	}

	MEM_ALLOC_CREDIT_CLASS();
	m_pMemory = (T*)UtlMemory_Alloc( m_pMemory, !IsExternallyAllocated(), ( int )( nNewAllocationCount * sizeof(T) ), ( int )( m_nAllocationCount * sizeof(T) ));
	Assert( m_pMemory );

	if ( IsExternallyAllocated() )
	{
		m_bHasConstMemory = false;
		m_bExternal = false;
	}

	m_nAllocationCount = nNewAllocationCount;

	UTLMEMORY_TRACK_ALLOC();
}


//-----------------------------------------------------------------------------
// Makes sure we've got at least this much memory
//-----------------------------------------------------------------------------
template< class T, class I >
inline void CUtlMemory<T,I>::EnsureCapacity( I num )
{
	if (m_nAllocationCount >= num)
		return;

	if ( IsReadOnly() )
	{
		// Can't grow a buffer whose memory was externally allocated 
		Assert(0);
		return;
	}

	UTLMEMORY_TRACK_FREE();

	MEM_ALLOC_CREDIT_CLASS();
	m_pMemory = (T*)UtlMemory_Alloc( m_pMemory, !IsExternallyAllocated(), ( int )( num * sizeof(T) ), ( int )( m_nAllocationCount * sizeof(T) ) );
	Assert( m_pMemory );

	if ( IsExternallyAllocated() )
	{
		m_bHasConstMemory = false;
		m_bExternal = false;
	}

	m_nAllocationCount = num;

	UTLMEMORY_TRACK_ALLOC();
}

//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------
template< class T, class I >
void CUtlMemory<T,I>::Purge()
{
	if ( !IsExternallyAllocated() )
	{
		if (m_pMemory)
		{
			UTLMEMORY_TRACK_FREE();
			Assert( m_pMemory != (T *)(size_t)UTLMEMORY_BAD_ADDRESS );
			free( (void*)m_pMemory );
			m_pMemory = 0;
		}
		m_nAllocationCount = 0;
	}
}

template< class T, class I >
void CUtlMemory<T,I>::Purge( I numElements )
{
	Assert( numElements >= 0 );

	if( numElements > m_nAllocationCount )
	{
		// Ensure this isn't a grow request in disguise.
		Assert( numElements <= m_nAllocationCount );
		return;
	}

	// If we have zero elements, simply do a purge:
	if( numElements == 0 )
	{
		Purge();
		return;
	}

	if ( IsReadOnly() )
	{
		// Can't shrink a buffer whose memory was externally allocated, fail silently like purge 
		return;
	}

	// If the number of elements is the same as the allocation count, we are done.
	if( numElements == m_nAllocationCount )
	{
		return;
	}


	if( !m_pMemory )
	{
		// Allocation count is non zero, but memory is null.
		Assert( m_pMemory );
		return;
	}

	UTLMEMORY_TRACK_FREE();

	// Allocation count > 0, shrink it down.
	MEM_ALLOC_CREDIT_CLASS();
	m_pMemory = (T*)UtlMemory_Alloc( m_pMemory, !IsExternallyAllocated(), ( int )( numElements * sizeof(T) ), ( int )( m_nAllocationCount * sizeof(T) ) );
	Assert( m_pMemory );

	if ( IsExternallyAllocated() )
	{
		m_bHasConstMemory = false;
		m_bExternal = false;
	}

	m_nAllocationCount = numElements;

	UTLMEMORY_TRACK_ALLOC();
}

//-----------------------------------------------------------------------------
// The CUtlMemory class:
// A growable memory class which doubles in size by default.
//-----------------------------------------------------------------------------
template< class T, typename I = int, I nAlignment = 0 >
class CUtlMemoryAligned	: public CUtlMemory<T, I>
{
public:
	using BaseClass = CUtlMemory<T, I>;

	// constructor, destructor
	CUtlMemoryAligned( I nGrowSize = 0, I nInitSize = 0 );
	CUtlMemoryAligned( T* pMemory, I numElements );
	CUtlMemoryAligned( const T* pMemory, I numElements );
	~CUtlMemoryAligned();

	// Attaches the buffer to external memory....
	void SetExternalBuffer( T* pMemory, I numElements );
	void SetExternalBuffer( const T* pMemory, I numElements );

	// Grows the memory, so that at least allocated + num elements are allocated
	void Grow( I num = 1 );

	// Makes sure we've got at least this much memory
	void EnsureCapacity( I num );

	// Memory deallocation
	void Purge();

	// Purge all but the given number of elements (NOT IMPLEMENTED IN CUtlMemoryAligned)
	void Purge( I numElements )	{ Assert( 0 ); }

private:
	void *Align( const void *pAddr );
};


//-----------------------------------------------------------------------------
// Aligns a pointer
//-----------------------------------------------------------------------------
template< class T, typename I, I nAlignment >
void *CUtlMemoryAligned<T, I, nAlignment>::Align( const void *pAddr )
{
	size_t nAlignmentMask = nAlignment - 1;
	return (void*)( ((size_t)pAddr + nAlignmentMask) & (~nAlignmentMask) );
}


//-----------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------
template< class T, typename I, I nAlignment >
CUtlMemoryAligned<T, I, nAlignment>::CUtlMemoryAligned( I nGrowSize, I nInitAllocationCount )
{
	BaseClass::m_pMemory = 0; 
	BaseClass::m_nAllocationCount = nInitAllocationCount;
	BaseClass::m_nGrowSize = nGrowSize;
	this->ValidateGrowSize();

	// Alignment must be a power of two
	COMPILE_TIME_ASSERT( (nAlignment & (nAlignment-1)) == 0 );
	Assert( (nGrowSize >= 0) && (nGrowSize != BaseClass::EXTERNAL_BUFFER_MARKER) );
	if ( BaseClass::m_nAllocationCount )
	{
		UTLMEMORY_TRACK_ALLOC();
		MEM_ALLOC_CREDIT_CLASS();
		BaseClass::m_pMemory = (T*)_aligned_malloc( nInitAllocationCount * sizeof(T), nAlignment );
	}
}

template< class T, typename I, I nAlignment >
CUtlMemoryAligned<T, I, nAlignment>::CUtlMemoryAligned( T* pMemory, I numElements )
{
	// Special marker indicating externally supplied memory
	BaseClass::m_nGrowSize = BaseClass::EXTERNAL_BUFFER_MARKER;

	BaseClass::m_pMemory = (T*)Align( pMemory );
	BaseClass::m_nAllocationCount = ( (I)(pMemory + numElements) - (I)BaseClass::m_pMemory ) / sizeof(T);
}

template< class T, typename I, I nAlignment >
CUtlMemoryAligned<T, I, nAlignment>::CUtlMemoryAligned( const T* pMemory, I numElements )
{
	// Special marker indicating externally supplied memory
	BaseClass::m_nGrowSize = BaseClass::EXTERNAL_CONST_BUFFER_MARKER;

	BaseClass::m_pMemory = (T*)Align( pMemory );
	BaseClass::m_nAllocationCount = ( (I)(pMemory + numElements) - (I)BaseClass::m_pMemory ) / sizeof(T);
}

template< class T, typename I, I nAlignment >
CUtlMemoryAligned<T, I, nAlignment>::~CUtlMemoryAligned()
{
	Purge();
}


//-----------------------------------------------------------------------------
// Attaches the buffer to external memory....
//-----------------------------------------------------------------------------
template< class T, typename I, I nAlignment >
void CUtlMemoryAligned<T, I, nAlignment>::SetExternalBuffer( T* pMemory, I numElements )
{
	// Blow away any existing allocated memory
	Purge();

	BaseClass::m_pMemory = (T*)Align( pMemory );
	BaseClass::m_nAllocationCount = ( (I)(pMemory + numElements) - (I)BaseClass::m_pMemory ) / sizeof(T);

	// Indicate that we don't own the memory
	BaseClass::m_nGrowSize = BaseClass::EXTERNAL_BUFFER_MARKER;
}

template< class T, typename I, I nAlignment >
void CUtlMemoryAligned<T, I, nAlignment>::SetExternalBuffer( const T* pMemory, I numElements )
{
	// Blow away any existing allocated memory
	Purge();

	BaseClass::m_pMemory = (T*)Align( pMemory );
	BaseClass::m_nAllocationCount = ( (I)(pMemory + numElements) - (I)BaseClass::m_pMemory ) / sizeof(T);

	// Indicate that we don't own the memory
	BaseClass::m_nGrowSize = BaseClass::EXTERNAL_CONST_BUFFER_MARKER;
}


//-----------------------------------------------------------------------------
// Grows the memory
//-----------------------------------------------------------------------------
template< class T, typename I, I nAlignment >
void CUtlMemoryAligned<T, I, nAlignment>::Grow( I num )
{
	Assert( num > 0 );

	if ( this->IsExternallyAllocated() )
	{
		// Can't grow a buffer whose memory was externally allocated 
		Assert(0);
		return;
	}

	UTLMEMORY_TRACK_FREE();

	// Make sure we have at least numallocated + num allocations.
	// Use the grow rules specified for this memory (in m_nGrowSize)
	I nAllocationRequested = BaseClass::m_nAllocationCount + num;

	BaseClass::m_nAllocationCount = ( I )UtlMemory_CalcNewAllocationCount( ( int )BaseClass::m_nAllocationCount, ( int )BaseClass::m_nGrowSize, ( int )nAllocationRequested, ( int )sizeof(T) );

	UTLMEMORY_TRACK_ALLOC();

	if ( BaseClass::m_pMemory )
	{
		MEM_ALLOC_CREDIT_CLASS();
		BaseClass::m_pMemory = (T*)MemAlloc_ReallocAligned( BaseClass::m_pMemory, BaseClass::m_nAllocationCount * sizeof(T), nAlignment );
		Assert( BaseClass::m_pMemory );
	}
	else
	{
		MEM_ALLOC_CREDIT_CLASS();
		BaseClass::m_pMemory = (T*)MemAlloc_AllocAligned( BaseClass::m_nAllocationCount * sizeof(T), nAlignment );
		Assert( BaseClass::m_pMemory );
	}
}


//-----------------------------------------------------------------------------
// Makes sure we've got at least this much memory
//-----------------------------------------------------------------------------
template< class T, typename I, I nAlignment >
inline void CUtlMemoryAligned<T, I, nAlignment>::EnsureCapacity( I num )
{
	if ( BaseClass::m_nAllocationCount >= num )
		return;

	if ( this->IsExternallyAllocated() )
	{
		// Can't grow a buffer whose memory was externally allocated 
		Assert(0);
		return;
	}

	UTLMEMORY_TRACK_FREE();

	BaseClass::m_nAllocationCount = num;

	UTLMEMORY_TRACK_ALLOC();

	if ( BaseClass::m_pMemory )
	{
		MEM_ALLOC_CREDIT_CLASS();
		BaseClass::m_pMemory = (T*)MemAlloc_ReallocAligned( BaseClass::m_pMemory, BaseClass::m_nAllocationCount * sizeof(T), nAlignment );
	}
	else
	{
		MEM_ALLOC_CREDIT_CLASS();
		BaseClass::m_pMemory = (T*)MemAlloc_AllocAligned( BaseClass::m_nAllocationCount * sizeof(T), nAlignment );
	}
}


//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------
template< class T, typename I, I nAlignment >
void CUtlMemoryAligned<T, I, nAlignment>::Purge()
{
	if ( !this->IsExternallyAllocated() )
	{
		if ( BaseClass::m_pMemory )
		{
			UTLMEMORY_TRACK_FREE();
			MemAlloc_FreeAligned( BaseClass::m_pMemory );
			BaseClass::m_pMemory = NULL;
		}
		BaseClass::m_nAllocationCount = 0;
	}
}

template< class T, typename I = int >
class CUtlMemory_RawAllocator
{
public:
	// constructor, destructor
	CUtlMemory_RawAllocator( I nGrowSize = 0, I nInitSize = 0, RawAllocatorType_t eAllocatorType = RawAllocator_Standard );
	CUtlMemory_RawAllocator( T* pMemory, I numElements ) { Assert( 0 ); }
	~CUtlMemory_RawAllocator();

	CUtlMemory_RawAllocator( const CUtlMemory_RawAllocator &init );
	const CUtlMemory_RawAllocator< T, I > &operator=( const CUtlMemory_RawAllocator &from );

	void CopyFrom( const CUtlMemory_RawAllocator &from );

	// Can we use this index?
	bool IsIdxValid( I i ) const						{ return (i >= 0) && (i < NumAllocated()); }
	static I InvalidIndex()							{ return -1; }

	// Gets the base address (can change when adding elements!)
	T* Base()											{ return m_pMemory; }
	const T* Base() const								{ return m_pMemory; }

	// element access
	T& operator[]( I i )								{ Assert( IsIdxValid(i) ); return Base()[i];	}
	const T& operator[]( I i ) const					{ Assert( IsIdxValid(i) ); return Base()[i];	}
	T& Element( I i )									{ Assert( IsIdxValid(i) ); return Base()[i];	}
	const T& Element( I i ) const						{ Assert( IsIdxValid(i) ); return Base()[i];	}

	// Attaches the buffer to external memory....
	void SetExternalBuffer( T* pMemory, I numElements ) { Assert( 0 ); }
	void AssumeMemory( T *pMemory, I nSize, RawAllocatorType_t eAllocatorType = RawAllocator_Standard );
	T* Detach();
	void *DetachMemory();

	// Fast swap
	void Swap( CUtlMemory_RawAllocator< T, I > &mem );

	// Size
	int NumAllocated() const							{ return m_nAllocationCount; }
	int Count() const									{ return m_nAllocationCount; }

	// Grows the memory, so that at least allocated + num elements are allocated
	void Grow( I num = 1 );

	// Makes sure we've got at least this much memory
	void EnsureCapacity( I num );

	// Memory deallocation
	void Purge();

	// Purge all but the given number of elements
	void Purge( I numElements );

	// is the memory externally allocated?
	bool IsExternallyAllocated() const						{ return false; }

	// Set the size by which the memory grows
	void SetGrowSize( I size );

	RawAllocatorType_t GetRawAllocatorType() const;

	class Iterator_t
	{
	public:
		Iterator_t( I i ) : index( i ) {}
		I index;
		bool operator==( const Iterator_t it ) const	{ return index == it.index; }
		bool operator!=( const Iterator_t it ) const	{ return index != it.index; }
	};
	Iterator_t First() const							{ return Iterator_t( IsIdxValid( 0 ) ? 0 : InvalidIndex() ); }
	Iterator_t Next( const Iterator_t &it ) const		{ return Iterator_t( IsIdxValid( it.index + 1 ) ? it.index + 1 : InvalidIndex() ); }
	I GetIndex( const Iterator_t &it ) const			{ return it.index; }
	bool IsIdxAfter( I i, const Iterator_t &it ) const { return i > it.index; }
	bool IsValidIterator( const Iterator_t &it ) const	{ return IsIdxValid( it.index ); }
	Iterator_t InvalidIterator() const					{ return Iterator_t( InvalidIndex() ); }

private:
	void SetRawAllocatorType( RawAllocatorType_t eAllocatorType );

	T* m_pMemory;

	uint32 m_nAllocationCount;

	union
	{
		struct
		{
			uint32 m_nGrowSize : 30;
			uint32 m_bPlatformAllocated : 1;
			uint32 m_bUnused : 1;
		};

		uint32 m_nGrowSizeStaff;
	};
};


//-----------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------

template< class T, typename I >
CUtlMemory_RawAllocator<T, I>::CUtlMemory_RawAllocator( I nGrowSize, I nInitAllocationCount, RawAllocatorType_t eAllocatorType ) : m_pMemory( 0 ), 
	m_nAllocationCount( 0 ), m_nGrowSize( nGrowSize ), m_bPlatformAllocated( false ), m_bUnused( false )
{
	SetRawAllocatorType( eAllocatorType );
	EnsureCapacity( nInitAllocationCount );
}

template< class T, typename I >
CUtlMemory_RawAllocator<T, I>::CUtlMemory_RawAllocator( const CUtlMemory_RawAllocator &init ) : m_pMemory( 0 ), 
	m_nAllocationCount( 0 )
{
	CopyFrom( init );
}

template< class T, typename I >
const CUtlMemory_RawAllocator< T, I > &CUtlMemory_RawAllocator<T, I>::operator=( const CUtlMemory_RawAllocator<T, I> &from )
{
	CopyFrom( from );
	return *this;
}

template< class T, typename I >
CUtlMemory_RawAllocator<T, I>::~CUtlMemory_RawAllocator()
{
	Purge();
}


template< class T, typename I >
void CUtlMemory_RawAllocator<T, I>::CopyFrom( const CUtlMemory_RawAllocator<T, I> &from )
{
	m_nAllocationCount = 0;
	m_nGrowSize = from.m_nGrowSize;

	SetRawAllocatorType( from.GetRawAllocatorType() );
	EnsureCapacity( m_nGrowSize );
}

//-----------------------------------------------------------------------------
// Fast swap
//-----------------------------------------------------------------------------
template< class T, typename I >
void CUtlMemory_RawAllocator<T, I>::Swap( CUtlMemory_RawAllocator<T, I> &mem )
{
	V_swap( m_pMemory, mem.m_pMemory );
	V_swap( m_nAllocationCount, mem.m_nAllocationCount );
	V_swap( m_nGrowSizeStaff, mem.m_nGrowSizeStaff );
}

template< class T, typename I >
void CUtlMemory_RawAllocator<T, I>::AssumeMemory( T* pMemory, I numElements, RawAllocatorType_t eAllocatorType )
{
	// Blow away any existing allocated memory
	Purge();

	// Simply take the pointer but don't mark us as external
	m_pMemory = pMemory;
	m_nAllocationCount = numElements;

	SetRawAllocatorType( eAllocatorType );
}

template< class T, typename I >
void *CUtlMemory_RawAllocator<T, I>::DetachMemory()
{
	void *pMemory = m_pMemory;
	m_pMemory = 0;
	m_nAllocationCount = 0;
	return pMemory;
}

template< class T, typename I >
inline T* CUtlMemory_RawAllocator<T, I>::Detach()
{
	return (T*)DetachMemory();
}

template< class T, typename I >
void CUtlMemory_RawAllocator<T, I>::SetGrowSize( I nSize )
{
	m_nGrowSize = nSize;
}

//-----------------------------------------------------------------------------
// Grows the memory
//-----------------------------------------------------------------------------
template< class T, typename I >
void CUtlMemory_RawAllocator<T, I>::Grow( I num )
{
	Assert( num > 0 );
	
	if ( ( INT_MAX - m_nAllocationCount ) < num )
	{
		Plat_FatalErrorFunc( "%s: Invalid grow amount %d\n", __FUNCTION__, num );
		DebuggerBreak();
	}
	
	EnsureCapacity( m_nAllocationCount + num );
}

//-----------------------------------------------------------------------------
// Makes sure we've got at least this much memory
//-----------------------------------------------------------------------------
template< class T, typename I >
inline void CUtlMemory_RawAllocator<T, I>::EnsureCapacity( I num )
{
	if (m_nAllocationCount >= num)
		return;
	
	if ( ( size_t )num > ( SIZE_MAX / sizeof(T) ) )
	{
		Plat_FatalErrorFunc( "%s: Invalid capacity %u\n", __FUNCTION__, num );
		DebuggerBreak();
	}
	
	void *pMemory = m_pMemory;
	size_t nSize = m_nAllocationCount * sizeof(T);
	RawAllocatorType_t eAllocatorType = GetRawAllocatorType();

	size_t adjustedSize;
	m_pMemory = (T*)CRawAllocator::Alloc( eAllocatorType, num * sizeof(T), &adjustedSize );
	m_nAllocationCount = ( I )( adjustedSize / sizeof(T) );

	if ( pMemory )
	{
		memcpy( m_pMemory, pMemory, nSize );
		CRawAllocator::Free( eAllocatorType, pMemory, nSize );
	}
}

//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------
template< class T, typename I >
void CUtlMemory_RawAllocator<T, I>::Purge()
{
	if (m_pMemory)
	{
		CRawAllocator::Free( GetRawAllocatorType(), m_pMemory, m_nAllocationCount * sizeof(T) );
		m_pMemory = 0;
		m_nAllocationCount = 0;
	}
}

template< class T, typename I >
void CUtlMemory_RawAllocator<T, I>::Purge( I numElements )
{
	Assert( numElements >= 0 );

	if( numElements > m_nAllocationCount )
	{
		// Ensure this isn't a grow request in disguise.
		Assert( numElements <= m_nAllocationCount );
		return;
	}

	// If we have zero elements, simply do a purge:
	if( numElements == 0 )
	{
		Purge();
		return;
	}

	// If the number of elements is the same as the allocation count, we are done.
	if( numElements == m_nAllocationCount )
	{
		return;
	}


	if( !m_pMemory )
	{
		// Allocation count is non zero, but memory is null.
		Assert( m_pMemory );
		return;
	}

	void *pMemory = m_pMemory;
	size_t nSize = m_nAllocationCount * sizeof(T);
	RawAllocatorType_t eAllocatorType = GetRawAllocatorType();

	size_t adjustedSize;
	m_pMemory = (T*)CRawAllocator::Alloc( eAllocatorType, numElements * sizeof(T), &adjustedSize );

	if ( adjustedSize < nSize )
	{
		m_nAllocationCount = ( int )( adjustedSize / sizeof(T) );
		memcpy( m_pMemory, pMemory, adjustedSize );
		CRawAllocator::Free( eAllocatorType, pMemory, nSize );
	}
	else
	{
		CRawAllocator::Free( eAllocatorType, m_pMemory, adjustedSize );
		m_pMemory = pMemory;
	}
}

template< class T, typename I >
RawAllocatorType_t CUtlMemory_RawAllocator<T, I>::GetRawAllocatorType() const
{
	return ( RawAllocatorType_t )( ( m_bPlatformAllocated ) != 0 );
}

template< class T, typename I >
void CUtlMemory_RawAllocator<T, I>::SetRawAllocatorType( RawAllocatorType_t eAllocatorType )
{
	if ( eAllocatorType == RawAllocator_Platform )
	{
		m_bPlatformAllocated = true;
	}
	else
	{
		if ( eAllocatorType != RawAllocator_Standard )
		{
			Plat_FatalErrorFunc( "%s: Unsupported raw allocator type %u\n", __FUNCTION__, eAllocatorType );
			DebuggerBreak();
		}

		m_bPlatformAllocated = false;
	}
}

#include "tier0/memdbgoff.h"

#endif // UTLMEMORY_H
