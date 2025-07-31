//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: 
//
// $NoKeywords: $
//
// A growable array class that maintains a free list and keeps elements
// in the same location
//=============================================================================//

#ifndef UTLLEANVECTOR_H
#define UTLLEANVECTOR_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/dbg.h"
#include "tier0/memalloc.h"
#include "tier0/platform.h"

#include "mathlib/mathlib.h"

#define FOR_EACH_LEANVEC( vecName, iteratorName ) \
	for ( auto iteratorName = vecName.First(); vecName.IsValidIterator( iteratorName ); iteratorName = vecName.Next( iteratorName ) )

template< class T, typename I, class A >
class CUtlLeanVectorBase
{
public:
	using ElemType_t = T;
	using IndexLocalType_t = I;
	using MemoryAllocator_t = A;

	static constexpr size_t MAX_ALLOCATED_BITS = sizeof( IndexLocalType_t ) * 8 - 1;
	static constexpr size_t MIN_ALLOCATED = ( MAX_ALLOCATED_BITS + sizeof( ElemType_t ) ) / sizeof( ElemType_t );
	static constexpr size_t MAX_ALLOCATED = ~( 1 << MAX_ALLOCATED_BITS );
	static constexpr I EXTERNAL_CONST_BUFFER_MARKER = -1;

	// constructor, destructor
	CUtlLeanVectorBase() : m_nCount( 0 ), m_nAllocated( 0 ), m_bExternal( false ), m_pElements( nullptr ) {}
	CUtlLeanVectorBase( int nGrowSize, int nInitSize = 0 ) : 
		m_nCount( 0 ), m_nAllocated( nInitSize ), m_bExternal( false ), m_pElements( nullptr )
	{
		Assert( nGrowSize >= nInitSize );
		Grow( nGrowSize );
	}
	CUtlLeanVectorBase( T* pMemory, I nElements ) : 
		m_nCount( 0 ), m_nAllocated( nElements ), m_bExternal( true ), m_pElements( pMemory )
	{
	}
	~CUtlLeanVectorBase()					{ Purge(); }

	// Gets the base address (can change when adding elements!)
	T* Base()								{ return m_nAllocated ? m_pElements : nullptr; }
	const T* Base() const					{ return m_nAllocated ? m_pElements : nullptr; }

	// Is the memory externally allocated?
	bool IsExternallyAllocated() const		{ return m_bExternal; }

	// Is the memory read only?
	bool IsReadOnly() const					{ return m_nAllocated == EXTERNAL_CONST_BUFFER_MARKER; }

	// Makes sure we have enough memory allocated to store a requested # of elements
	void Grow( int num = 1 )				{ EnsureCapacity( m_nCount + num ); m_nCount += num; }
	void EnsureCapacity( int num, bool force = false );

	// Attaches the buffer to external memory.
	void SetExternalBuffer( T* pMemory, I nElements );
	void AssumeMemory( T* pMemory, I nSize );
	T* Detach() { return ( T*  )DetachMemory(); }
	void* DetachMemory();

	// Fast swap
	void Swap( CUtlLeanVectorBase< T, I, A > &mem );

	// Element removal
	void RemoveAll(); // doesn't deallocate memory

	// Memory deallocation
	void Purge();

protected:
	struct
	{
		I m_nCount;

		union
		{
			struct
			{
				I m_nAllocated : MAX_ALLOCATED_BITS;
				I m_bExternal : 1;
			};

			I m_nAllocatedStaff;
		};

		T* m_pElements;
	};
};

template< class T, typename I, class A >
void CUtlLeanVectorBase<T, I, A>::EnsureCapacity( int num, bool force )
{
	I nNewAllocated = m_nAllocated;

	if ( IsExternallyAllocated() )
		return;

	if ( force )
	{
		if ( num == m_nAllocated )
			return;
	}
	else
	{
		if ( num <= m_nAllocated )
			return;
	}

	if ( num > MAX_ALLOCATED )
	{
		Msg( "%s allocation count overflow( %llu + %llu > %llu )\n", __FUNCTION__, ( uint64 )num, ( uint64 )MIN_ALLOCATED, ( uint64 )MAX_ALLOCATED );
		Plat_FatalError( "%s allocation count overflow", __FUNCTION__ );
		DebuggerBreak();
	}

	if ( force )
	{
		nNewAllocated = num;
	}
	else
	{
		while ( nNewAllocated < num )
		{
			if ( nNewAllocated < MAX_ALLOCATED / 2 )
				nNewAllocated = MAX( nNewAllocated * 2, MIN_ALLOCATED );
			else
				nNewAllocated = ( I )MAX_ALLOCATED;
		}
	}

	m_pElements = (T*)MemoryAllocator_t::Realloc( m_pElements, nNewAllocated * sizeof(T) );
	m_nAllocated = nNewAllocated;
}

//-----------------------------------------------------------------------------
// Attaches the buffer to external memory.
//-----------------------------------------------------------------------------

template< class T, typename I, class A >
void CUtlLeanVectorBase<T, I, A>::SetExternalBuffer( T* pMemory, I nElements )
{
	// Blow away any existing allocated memory
	Purge();

	m_nCount = nElements;
	m_pElements = pMemory;

	// Indicate that we don't own the memory
	m_nAllocated = 0;
	m_bExternal = true;
}

template< class T, typename I, class A >
void CUtlLeanVectorBase<T, I, A>::AssumeMemory( T* pMemory, I numElements )
{
	// Blow away any existing allocated memory
	Purge();

	// Simply take the pointer but don't mark us as external
	m_nCount = numElements;
	m_nAllocated = numElements;
	m_pElements = pMemory;
}

template< class T, typename I, class A >
void* CUtlLeanVectorBase<T, I, A>::DetachMemory()
{
	if ( IsReadOnly() )
		return nullptr;

	void* pMemory = m_pElements;

	m_nCount = 0;
	m_nAllocated = 0;
	m_bExternal = false;
	m_pElements = nullptr;

	return pMemory;
}

template< class T, typename I, class A >
void CUtlLeanVectorBase<T, I, A>::Swap( CUtlLeanVectorBase< T, I, A >& mem )
{
	V_swap( m_nCount, mem.m_nCount );
	V_swap( m_nAllocatedStaff, mem.m_nAllocatedStaff );
	V_swap( m_pElements, mem.m_pElements );
}

//-----------------------------------------------------------------------------
// Element removal
//-----------------------------------------------------------------------------
template< class T, typename I, class A >
void CUtlLeanVectorBase<T, I, A>::RemoveAll()
{
	T* pElement = Base();
	const T* pEnd = &pElement[ m_nCount ];
	while ( pElement != pEnd )
		Destruct( pElement++ );

	m_nCount = 0;
}

//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------
template< class T, typename I, class A >
inline void CUtlLeanVectorBase<T, I, A>::Purge()
{
	RemoveAll();
	
	if ( m_nAllocated > 0 && !IsExternallyAllocated() )
	{
		MemoryAllocator_t::Free( (void*)m_pElements );
	}

	m_nAllocated = 0;
	m_pElements = NULL;
}

template< class T, size_t N, typename I >
class CUtlLeanVectorFixedGrowableBase
{
public:
	// constructor, destructor
	CUtlLeanVectorFixedGrowableBase() : m_nCount(0), m_nAllocated(N) {}
	~CUtlLeanVectorFixedGrowableBase() { Purge(); }

	// Gets the base address (can change when adding elements!)
	T* Base();
	const T* Base() const;

	// Makes sure we have enough memory allocated to store a requested # of elements
	void EnsureCapacity( int num, bool force = false );
	
	// Element removal
	void RemoveAll(); // doesn't deallocate memory
	
	// Memory deallocation
	void Purge();

protected:

	union
	{
		struct
		{
			I m_nCount;
			I m_nAllocated;
		};
		
		struct
		{
			I m_nFixedCount;
			I m_nFixedAllocated;
			T m_FixedAlloc[ N ];
		};
		
		struct
		{
			I m_nAllocCount;
			I m_nAllocAllocated;
			T* m_pElements;
		};
	};
};

//-----------------------------------------------------------------------------
// Gets the base address (can change when adding elements!)
//-----------------------------------------------------------------------------
template< class T, size_t N, typename I >
inline T* CUtlLeanVectorFixedGrowableBase<T, N, I>::Base()
{
	if ( m_nAllocated )
	{
		if ( ( size_t )m_nAllocated > N )
			return m_pElements;
		else
			return &m_FixedAlloc[ 0 ];
	}
	
	return NULL;
}

template< class T, size_t N, typename I >
inline const T* CUtlLeanVectorFixedGrowableBase<T, N, I>::Base() const
{
	if ( m_nAllocated )
	{
		if ( ( size_t )m_nAllocated > N )
			return m_pElements;
		else
			return &m_FixedAlloc[ 0 ];
	}
	
	return NULL;
}

//-----------------------------------------------------------------------------
// Makes sure we have enough memory allocated to store a requested # of elements
//-----------------------------------------------------------------------------
template< class T, size_t N, typename I >
void CUtlLeanVectorFixedGrowableBase<T, N, I>::EnsureCapacity( int num, bool force )
{
	I nMinAllocated = ( 31 + sizeof( T ) ) / sizeof( T );
	I nMaxAllocated = (std::numeric_limits<I>::max)();
	I nNewAllocated = m_nAllocated;
	
	if ( num <= m_nAllocated )
		return;
	
	if ( ( size_t )num > N )
	{
		if ( num > nMaxAllocated )
		{
			Msg( "%s allocation count overflow( %llu > %llu )\n", __FUNCTION__, ( uint64 )num, ( uint64 )nMaxAllocated );
			Plat_FatalError( "%s allocation count overflow", __FUNCTION__ );
			DebuggerBreak();
		}
		
		if ( force )
		{
			nNewAllocated = num;
		}
		else
		{
			while ( nNewAllocated < num )
			{
				if ( nNewAllocated < nMaxAllocated/2 )
					nNewAllocated = MAX( nNewAllocated*2, nMinAllocated );
				else
					nNewAllocated = nMaxAllocated;
			}
		}
	}
	else
	{
		nNewAllocated = num;
	}
	
	if ( ( size_t )m_nAllocated > N )
	{
		m_pElements = (T*)CMemAllocAllocator::Realloc( m_pElements, nNewAllocated * sizeof(T) );
	}
	else if ( ( size_t )nNewAllocated > N )
	{
		T* pNew = (T*)CMemAllocAllocator::Alloc( nNewAllocated * sizeof(T) );
		memcpy( pNew, Base(), m_nCount * sizeof(T) );
		m_pElements = pNew;
	}
	
	m_nAllocated = nNewAllocated;
}

//-----------------------------------------------------------------------------
// Element removal
//-----------------------------------------------------------------------------
template< class T, size_t N, typename I >
void CUtlLeanVectorFixedGrowableBase<T, N, I>::RemoveAll()
{
	T* pElement = Base();
	const T* pEnd = &pElement[ m_nCount ];
	while ( pElement != pEnd )
		Destruct( pElement++ );

	m_nCount = 0;
}

//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------
template< class T, size_t N, typename I >
inline void CUtlLeanVectorFixedGrowableBase<T, N, I>::Purge()
{
	RemoveAll();
	
	if ( ( size_t )m_nAllocated > N )
		CMemAllocAllocator::Free( (void*)m_pElements );
	
	m_nAllocated = N;
}

template< class B, class T, typename I >
class CUtlLeanVectorImpl : public B
{
public:
	using BaseClass = B;
	using ElemType_t = T;
	using IndexLocalType_t = I;

	using iterator = T*;
	using const_iterator = const T*;

	// constructor, destructor
	CUtlLeanVectorImpl() {};
	CUtlLeanVectorImpl( int nGrowSize, int nInitSize = 0 ) : BaseClass( nGrowSize, nInitSize ) {}
	CUtlLeanVectorImpl( T* pMemory, int nElements ) : BaseClass( pMemory, nElements ) {}
	~CUtlLeanVectorImpl() {};

	// Copy the array.
	CUtlLeanVectorImpl<B, T, I>& operator=( const CUtlLeanVectorImpl<B, T, I> &other );

	class Iterator_t
	{
	public:
		Iterator_t( I i ) : index( i ) {}
		I index;

		bool operator==( const Iterator_t it ) const	{ return index == it.index; }
		bool operator!=( const Iterator_t it ) const	{ return index != it.index; }
	};
	Iterator_t First() const							{ return Iterator_t( IsIdxValid( 0 ) ? 0 : InvalidIndex() ); }
	Iterator_t Next( const Iterator_t it ) const		{ return Iterator_t( IsIdxValid( it.index + 1 ) ? it.index + 1 : InvalidIndex() ); }
	I GetIndex( const Iterator_t it ) const				{ return it.index; }
	bool IsIdxAfter( I i, const Iterator_t it ) const	{ return i > it.index; }
	bool IsValidIterator( const Iterator_t it ) const	{ return IsIdxValid( it.index ); }
	Iterator_t InvalidIterator() const					{ return Iterator_t( InvalidIndex() ); }

	T& operator[]( const Iterator_t it )				{ return Element( it.index ); }
	const T& operator[]( const Iterator_t it ) const	{ return Element( it.index ); }

	iterator begin()						{ return this->Base(); }
	const_iterator begin() const			{ return this->Base(); }
	iterator end()							{ return this->Base() + Count(); }
	const_iterator end() const				{ return this->Base() + Count(); }

	// element access
	T& operator[]( int i );
	const T& operator[]( int i ) const;
	T& Element( int i );
	const T& Element( int i ) const;
	bool IsIdxValid( int i ) const;

	T& Head();
	const T& Head() const;
	T& Tail();
	const T& Tail() const;
	
	// Returns the number of elements in the vector
	int Count() const;
	int NumAllocated() const;

	// Is element index valid?
	bool IsValidIndex( int i ) const;
	static int InvalidIndex();

	// Adds an element, uses default constructor
	T* AddToTailGetPtr();

	// Adds an element, uses copy constructor
	int AddToTail( const T& src );

	// Adds multiple elements, uses default constructor
	int AddMultipleToTail( int nSize );	

	// Adds multiple elements, uses default constructor
	T* InsertBeforeGetPtr( int nBeforeIndex, int nSize = 1 );

	void SetSize( int size );
	void SetCount( int count );

	// Finds an element (element needs operator== defined)
	int Find( const T& src ) const;

	// Element removal
	void FastRemove( int elem ); // doesn't preserve order
	void Remove( int elem ); // preserves order, shifts elements
	bool FindAndRemove( const T& src );	// removes first occurrence of src, preserves order, shifts elements
	bool FindAndFastRemove( const T& src );	// removes first occurrence of src, doesn't preserve order
	void RemoveMultiple( int elem, int num ); // preserves order, shifts elements

protected:
	// Can't copy this unless we explicitly do it!
	CUtlLeanVectorImpl( CUtlLeanVectorImpl const& vec ) { Assert(0); }

	// Shifts elements....
	void ShiftElements( T* pDest, const T* pSrc, const T* pSrcEnd );

	// construct, destruct elements
	void ConstructElements( T* pElement, const T* pEnd );
	void DestructElements( T* pElement, const T* pEnd );
};

template< class B, class T, typename I >
inline CUtlLeanVectorImpl<B, T, I>& CUtlLeanVectorImpl<B, T, I>::operator=( const CUtlLeanVectorImpl<B, T, I> &other )
{
	int nCount = other.Count();
	SetSize( nCount );

	T* pDest = this->Base();
	const T* pSrc = other.Base();
	const T* pEnd = &pSrc[ nCount ];

	while ( pSrc != pEnd )
		*(pDest++) = *(pSrc++);

	return *this;
}

//-----------------------------------------------------------------------------
// element access
//-----------------------------------------------------------------------------
template< class B, class T, typename I >
inline T& CUtlLeanVectorImpl<B, T, I>::operator[]( int i )
{
	Assert( IsIdxValid( i ) );
	return this->Base()[ i ];
}

template< class B, class T, typename I >
inline const T& CUtlLeanVectorImpl<B, T, I>::operator[]( int i ) const
{
	Assert( IsIdxValid( i ) );
	return this->Base()[ i ];
}

template< class B, class T, typename I >
inline T& CUtlLeanVectorImpl<B, T, I>::Element( int i )
{
	Assert( IsIdxValid( i ) );
	return this->Base()[ i ];
}

template< class B, class T, typename I >
inline const T& CUtlLeanVectorImpl<B, T, I>::Element( int i ) const
{
	Assert( IsIdxValid( i ) );
	return this->Base()[ i ];
}

template< class B, class T, typename I >
inline bool CUtlLeanVectorImpl<B, T, I>::IsIdxValid( int i ) const
{
	return 0 <= i && i < Count();
}

template< class B, class T, typename I >
inline T& CUtlLeanVectorImpl<B, T, I>::Head()
{
	Assert( this->m_nCount > 0 );
	return this->Base()[ 0 ];
}

template< class B, class T, typename I >
inline const T& CUtlLeanVectorImpl<B, T, I>::Head() const
{
	Assert( this->m_nCount > 0 );
	return this->Base()[ 0 ];
}

template< class B, class T, typename I >
inline T& CUtlLeanVectorImpl<B, T, I>::Tail()
{
	Assert( this->m_nCount > 0 );
	return this->Base()[ this->m_nCount - 1 ];
}

template< class B, class T, typename I >
inline const T& CUtlLeanVectorImpl<B, T, I>::Tail() const
{
	Assert( this->m_nCount > 0 );
	return this->Base()[ this->m_nCount - 1 ];
}

//-----------------------------------------------------------------------------
// Count
//-----------------------------------------------------------------------------
template< class B, class T, typename I >
inline int CUtlLeanVectorImpl<B, T, I>::Count() const
{
	return this->m_nCount;
}
//-----------------------------------------------------------------------------
// Number of allocated elements
//-----------------------------------------------------------------------------
template< class B, class T, typename I >
inline int CUtlLeanVectorImpl<B, T, I>::NumAllocated() const
{
	return this->m_nAllocated;
}

//-----------------------------------------------------------------------------
// Is element index valid?
//-----------------------------------------------------------------------------
template< class B, class T, typename I >
inline bool CUtlLeanVectorImpl<B, T, I>::IsValidIndex( int i ) const
{
	return (i >= 0) && (i < this->m_nCount);
}

//-----------------------------------------------------------------------------
// Returns in invalid index
//-----------------------------------------------------------------------------
template< class B, class T, typename I >
inline int CUtlLeanVectorImpl<B, T, I>::InvalidIndex()
{
	return -1;
}

//-----------------------------------------------------------------------------
// Adds an element, uses default constructor
//-----------------------------------------------------------------------------
template< class B, class T, typename I >
T* CUtlLeanVectorImpl<B, T, I>::AddToTailGetPtr()
{
	this->EnsureCapacity( this->m_nCount + 1 );
	T* pBase = this->Base();
	Construct( &pBase[ this->m_nCount ] );
	return &pBase[ this->m_nCount++ ];
}

//-----------------------------------------------------------------------------
// Adds an element, uses copy constructor
//-----------------------------------------------------------------------------
template< class B, class T, typename I >
int CUtlLeanVectorImpl<B, T, I>::AddToTail( const T& src )
{
	this->EnsureCapacity( this->m_nCount + 1 );
	T* pBase = this->Base();
	CopyConstruct( &pBase[ this->m_nCount ], src );
	return this->m_nCount++;
}

//-----------------------------------------------------------------------------
// Adds multiple elements, uses default constructor
//-----------------------------------------------------------------------------
template< class B, class T, typename I >
int CUtlLeanVectorImpl<B, T, I>::AddMultipleToTail( int nSize )
{
	int nOldSize = this->m_nCount;

	if ( nSize > 0 )
	{
		if ( ( ( int )BaseClass::MAX_ALLOCATED - nOldSize ) < nSize )
		{
			Msg( "%s allocation count overflow( add %llu + current %llu > max %llu )\n", __FUNCTION__, ( uint64 )nSize, ( uint64 )nOldSize, ( uint64 )BaseClass::MAX_ALLOCATED );
			Plat_FatalError( "%s allocation count overflow", __FUNCTION__ );
			DebuggerBreak();
		}

		int nNewSize = nOldSize + nSize;

		this->EnsureCapacity( nNewSize );

		T* pBase = this->Base();

		ConstructElements( &pBase[ nOldSize ], &pBase[ nNewSize ] );

		this->m_nCount = nNewSize;
	}

	return nOldSize;
}

//-----------------------------------------------------------------------------
// Adds multiple elements, uses default constructor
//-----------------------------------------------------------------------------
template< class B, class T, typename I >
T* CUtlLeanVectorImpl<B, T, I>::InsertBeforeGetPtr( int nBeforeIndex, int nSize )
{
	int nOldSize = this->m_nCount;

	if ( nBeforeIndex < 0 || nBeforeIndex > nOldSize )
	{
		Plat_FatalError( "%s: invalid nBeforeIndex %d\n", __FUNCTION__, nBeforeIndex );
		DebuggerBreak();
	}

	if ( nSize <= 0 )
	{
		Plat_FatalError( "%s: invalid nSize %d\n", __FUNCTION__, nSize );
		DebuggerBreak();
	}

	int nMaxSize = (std::numeric_limits<I>::max)();

	if ( ( nMaxSize - nOldSize ) < nSize )
	{
		Msg( "%s allocation count overflow( add %llu + current %llu > max %llu )\n", __FUNCTION__, ( uint64 )nSize, ( uint64 )nOldSize, ( uint64 )nMaxSize );
		Plat_FatalError( "%s allocation count overflow", __FUNCTION__ );
		DebuggerBreak();
	}

	int nNewSize = nOldSize + nSize;

	this->EnsureCapacity( nNewSize );

	T* pBase = this->Base();

	ShiftElements( &pBase[ nBeforeIndex + nSize ], &pBase[ nBeforeIndex ], &pBase[ nOldSize ] );
	ConstructElements( &pBase[ nBeforeIndex ], &pBase[ nBeforeIndex + nSize ] );

	this->m_nCount = nNewSize;

	return &pBase[ nBeforeIndex ];
}

template< class B, class T, typename I >
void CUtlLeanVectorImpl<B, T, I>::SetCount( int count )
{
	this->EnsureCapacity( count );

	T* pBase = this->Base();

	if ( this->m_nCount < count )
		ConstructElements( &pBase[ this->m_nCount ], &pBase[ count ] );
	else if ( this->m_nCount > count )
		DestructElements( &pBase[ count ], &pBase[ this->m_nCount ] );

	this->m_nCount = count;
}

template< class B, class T, typename I >
inline void CUtlLeanVectorImpl<B, T, I>::SetSize( int size )
{
	SetCount( size );
}

//-----------------------------------------------------------------------------
// Finds an element (element needs operator== defined)
//-----------------------------------------------------------------------------
template< class B, class T, typename I >
int CUtlLeanVectorImpl<B, T, I>::Find( const T& src ) const
{
	const T* pBase = this->Base();
	for ( int i = 0; i < Count(); ++i )
	{
		if ( pBase[i] == src )
			return i;
	}
	return -1;
}

//-----------------------------------------------------------------------------
// Element removal
//-----------------------------------------------------------------------------
template< class B, class T, typename I >
void CUtlLeanVectorImpl<B, T, I>::FastRemove( int elem )
{
	Assert( IsValidIndex(elem) );

	T* pBase = this->Base();
	Destruct( &pBase[ elem ] );
	if ( this->m_nCount > 0 )
	{
		if ( elem != this->m_nCount - 1 )
			memcpy( &pBase[ elem ], &pBase[ this->m_nCount - 1 ], sizeof( T ) );
		--this->m_nCount;
	}
}

template< class B, class T, typename I >
void CUtlLeanVectorImpl<B, T, I>::Remove( int elem )
{
	T* pBase = this->Base();
	Destruct( &pBase[ elem ] );
	ShiftElements( &pBase[ elem ], &pBase[ elem + 1 ], &pBase[ this->m_nCount ] );
	--this->m_nCount;
}

template< class B, class T, typename I >
bool CUtlLeanVectorImpl<B, T, I>::FindAndRemove( const T& src )
{
	int elem = Find( src );
	if ( elem != -1 )
	{
		Remove( elem );
		return true;
	}
	return false;
}

template< class B, class T, typename I >
bool CUtlLeanVectorImpl<B, T, I>::FindAndFastRemove( const T& src )
{
	int elem = Find( src );
	if ( elem != -1 )
	{
		FastRemove( elem );
		return true;
	}
	return false;
}

template< class B, class T, typename I >
void CUtlLeanVectorImpl<B, T, I>::RemoveMultiple( int elem, int num )
{
	Assert( elem >= 0 );
	Assert( elem + num <= Count() );

	T* pBase = this->Base();
	DestructElements( &pBase[ elem ], &pBase[ elem + num ] );
	ShiftElements( &pBase[ elem ], &pBase[ elem + num ], &pBase[ this->m_nCount ] );
	this->m_nCount -= num;
}

//-----------------------------------------------------------------------------
// Shifts elements
//-----------------------------------------------------------------------------
template< class B, class T, typename I >
void CUtlLeanVectorImpl<B, T, I>::ShiftElements( T* pDest, const T* pSrc, const T* pSrcEnd )
{
	ptrdiff_t numToMove = pSrcEnd - pSrc;
	if ( numToMove > 0 )
		memmove( pDest, pSrc, numToMove * sizeof( T ) );
}

//-----------------------------------------------------------------------------
// construct, destruct elements
//-----------------------------------------------------------------------------
template< class B, class T, typename I >
void CUtlLeanVectorImpl<B, T, I>::ConstructElements( T* pElement, const T* pEnd )
{
	while ( pElement < pEnd )
		Construct( pElement++ );
}

template< class B, class T, typename I >
void CUtlLeanVectorImpl<B, T, I>::DestructElements( T* pElement, const T* pEnd )
{
	while ( pElement < pEnd )
		Destruct( pElement++ );
}

template < class T, typename I = int, class A = CMemAllocAllocator >
class CUtlLeanVector : public CUtlLeanVectorImpl< CUtlLeanVectorBase< T, I, A >, T, I >
{
public:
	using BaseClass = CUtlLeanVectorImpl< CUtlLeanVectorBase< T, I, A >, T, I >;
	using BaseClass::BaseClass;
};

template < class T, size_t N = 3, typename I = int >
class CUtlLeanVectorFixedGrowable : public CUtlLeanVectorImpl< CUtlLeanVectorFixedGrowableBase< T, N, I >, T, I >
{
public:
	using BaseClass = CUtlLeanVectorImpl< CUtlLeanVectorFixedGrowableBase< T, N, I >, T, I >;
	using BaseClass::BaseClass;
};

#include "tier0/memdbgoff.h"

#endif // UTLLEANVECTOR_H
