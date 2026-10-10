//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//
// A growable array class that keeps its elements in fixed size pages,
// so elements never move when it grows.
//===========================================================================//

#ifndef UTLBLOCKVECTOR_H
#define UTLBLOCKVECTOR_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/dbg.h"
#include "tier0/memblockallocator.h"

#include <iterator>
#include <limits>
#include <type_traits>

template< class T, class I = int, class A = CMemAllocAllocator >
class CUtlBlockVector
{
public:
	// nGrowSize is the number of elements per page, rounded up to a power of two
	CUtlBlockVector( int nGrowSize = 0, int nInitSize = 0 );
	~CUtlBlockVector();

	CUtlBlockVector( const CUtlBlockVector & ) = delete;
	CUtlBlockVector &operator=( const CUtlBlockVector & ) = delete;

	// Index iteration, as CUtlLinkedList and CUtlRBTree use it on their memory
	class Iterator_t
	{
	public:
		Iterator_t( I i ) : index( i ) {}
		I index;

		bool operator==( const Iterator_t it ) const { return index == it.index; }
		bool operator!=( const Iterator_t it ) const { return index != it.index; }
	};
	Iterator_t First() const { return Iterator_t( IsIdxValid( 0 ) ? 0 : InvalidIndex() ); }
	Iterator_t Next( const Iterator_t &it ) const { return Iterator_t( IsIdxValid( it.index + 1 ) ? it.index + 1 : InvalidIndex() ); }
	I GetIndex( const Iterator_t &it ) const { return it.index; }
	bool IsIdxAfter( I i, const Iterator_t &it ) const { return i > it.index; }
	bool IsValidIterator( const Iterator_t &it ) const { return IsIdxValid( it.index ); }
	Iterator_t InvalidIterator() const { return Iterator_t( InvalidIndex() ); }

	// STL compatible random access iterator, for range-based for loops and std algorithms
	template< bool bConst >
	class BlockIterator_t
	{
		typedef std::conditional_t< bConst, const CUtlBlockVector, CUtlBlockVector > Vector_t;

	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef T value_type;
		typedef intp difference_type;
		typedef std::conditional_t< bConst, const T*, T* > pointer;
		typedef std::conditional_t< bConst, const T&, T& > reference;

		BlockIterator_t() : m_pVector( nullptr ), m_nIndex( 0 ) {}
		BlockIterator_t( Vector_t *pVector, difference_type nIndex ) : m_pVector( pVector ), m_nIndex( nIndex ) {}

		template< bool bOtherConst, std::enable_if_t< bConst && !bOtherConst, int > = 0 >
		BlockIterator_t( const BlockIterator_t< bOtherConst > &other ) : m_pVector( other.m_pVector ), m_nIndex( other.m_nIndex ) {}

		reference operator*() const							{ return m_pVector->Element( ( I )m_nIndex ); }
		pointer operator->() const							{ return &m_pVector->Element( ( I )m_nIndex ); }
		reference operator[]( difference_type n ) const		{ return m_pVector->Element( ( I )( m_nIndex + n ) ); }

		BlockIterator_t &operator++()						{ ++m_nIndex; return *this; }
		BlockIterator_t operator++( int )					{ BlockIterator_t it = *this; ++m_nIndex; return it; }
		BlockIterator_t &operator--()						{ --m_nIndex; return *this; }
		BlockIterator_t operator--( int )					{ BlockIterator_t it = *this; --m_nIndex; return it; }
		BlockIterator_t &operator+=( difference_type n )	{ m_nIndex += n; return *this; }
		BlockIterator_t &operator-=( difference_type n )	{ m_nIndex -= n; return *this; }

		friend BlockIterator_t operator+( BlockIterator_t it, difference_type n )	{ it += n; return it; }
		friend BlockIterator_t operator+( difference_type n, BlockIterator_t it )	{ it += n; return it; }
		friend BlockIterator_t operator-( BlockIterator_t it, difference_type n )	{ it -= n; return it; }
		friend difference_type operator-( const BlockIterator_t &a, const BlockIterator_t &b )	{ return a.m_nIndex - b.m_nIndex; }

		friend bool operator==( const BlockIterator_t &a, const BlockIterator_t &b )	{ return a.m_nIndex == b.m_nIndex; }
		friend bool operator!=( const BlockIterator_t &a, const BlockIterator_t &b )	{ return a.m_nIndex != b.m_nIndex; }
		friend bool operator<( const BlockIterator_t &a, const BlockIterator_t &b )	{ return a.m_nIndex < b.m_nIndex; }
		friend bool operator>( const BlockIterator_t &a, const BlockIterator_t &b )	{ return a.m_nIndex > b.m_nIndex; }
		friend bool operator<=( const BlockIterator_t &a, const BlockIterator_t &b )	{ return a.m_nIndex <= b.m_nIndex; }
		friend bool operator>=( const BlockIterator_t &a, const BlockIterator_t &b )	{ return a.m_nIndex >= b.m_nIndex; }

	private:
		template< bool > friend class BlockIterator_t;

		Vector_t *m_pVector;
		difference_type m_nIndex;
	};

	typedef BlockIterator_t< false > iterator;
	typedef BlockIterator_t< true > const_iterator;
	iterator begin()						{ return iterator( this, 0 ); }
	const_iterator begin() const			{ return const_iterator( this, 0 ); }
	iterator end()							{ return iterator( this, Count() ); }
	const_iterator end() const				{ return const_iterator( this, Count() ); }

	// element access
	T& operator[]( I i )					{ return Element( i ); }
	const T& operator[]( I i ) const		{ return Element( i ); }
	T& operator[]( const Iterator_t &it )	{ return Element( it.index ); }
	const T& operator[]( const Iterator_t &it ) const	{ return Element( it.index ); }
	T& Element( I i );
	const T& Element( I i ) const;
	T& Head()								{ return Element( 0 ); }
	const T& Head() const					{ return Element( 0 ); }
	T& Tail()								{ return Element( ( I )( m_nCount - 1 ) ); }
	const T& Tail() const					{ return Element( ( I )( m_nCount - 1 ) ); }

	int Count() const						{ return m_nCount; }
	bool IsEmpty() const					{ return m_nCount == 0; }
	int NumAllocated() const				{ return m_nAllocated; }

	bool IsValidIndex( I i ) const			{ return IsIdxValid( i ); }
	bool IsIdxValid( I i ) const			{ return ( std::make_unsigned_t< I > )i < ( std::make_unsigned_t< I > )m_nCount; }

	static const I INVALID_INDEX = ( I )-1;
	static I InvalidIndex()					{ return INVALID_INDEX; }

	// Sets the number of elements per page, rounded up to a power of two; purges the vector
	void SetGrowSize( int nGrowSize );

	// Makes sure we have enough memory allocated to store a requested # of elements
	void EnsureCapacity( int num, bool force = false );

	// Adds an element, uses default constructor
	T* AddToTailGetPtr();
	I AddToTail();

	// Adds an element, uses copy constructor
	I AddToTail( const T& src );

	// Adds multiple elements, uses default constructor
	I AddMultipleToTail( int num );

	void SetCount( int count );
	void EnsureCount( int num );

	// Finds an element (element needs operator== defined)
	I Find( const T& src ) const;
	bool HasElement( const T& src ) const	{ return Find( src ) != InvalidIndex(); }

	// Element removal
	void FastRemove( I elem ); // doesn't preserve order
	void Remove( I elem ); // preserves order, shifts elements
	bool FindAndRemove( const T& src ); // removes first occurrence of src, preserves order, shifts elements
	bool FindAndFastRemove( const T& src ); // removes first occurrence of src, doesn't preserve order
	void RemoveMultipleFromTail( int num );
	void RemoveAll(); // doesn't deallocate memory

	// Memory deallocation
	void Purge();

	void Swap( CUtlBlockVector<T, I, A> &other );

protected:
	static int MaxCapacity()				{ return ( std::numeric_limits< std::make_signed_t< I > >::max )(); }

	// Adds num elements without constructing them
	void GrowCount( int num );

	I m_nAllocated;
	I m_nCount;
	CUtlMemoryBlockAllocator< T, A > m_Memory;
};

template< class T, class I, class A >
inline CUtlBlockVector<T, I, A>::CUtlBlockVector( int nGrowSize, int nInitSize ) :
	m_nAllocated( 0 ), m_nCount( 0 )
{
	SetGrowSize( nGrowSize );
	EnsureCapacity( nInitSize, true );
}

template< class T, class I, class A >
inline CUtlBlockVector<T, I, A>::~CUtlBlockVector()
{
	Purge();
}

template< class T, class I, class A >
inline T& CUtlBlockVector<T, I, A>::Element( I i )
{
	Assert( IsIdxValid( i ) );
	return *( T * )m_Memory.GetBlock( ( MemBlockHandle_t )i );
}

template< class T, class I, class A >
inline const T& CUtlBlockVector<T, I, A>::Element( I i ) const
{
	Assert( IsIdxValid( i ) );
	return *( const T * )m_Memory.GetBlock( ( MemBlockHandle_t )i );
}

template< class T, class I, class A >
void CUtlBlockVector<T, I, A>::SetGrowSize( int nGrowSize )
{
	Purge();

	if ( nGrowSize <= 0 )
		nGrowSize = 256;

	uint32 nPageSize = SmallestPowerOfTwoGreaterOrEqual( MAX( nGrowSize, 4 ) );
	m_Memory.SetPageSize( nPageSize, nPageSize, true );
	m_Memory.m_bAllocFromLastPageOnly = true;
}

template< class T, class I, class A >
void CUtlBlockVector<T, I, A>::EnsureCapacity( int num, bool force )
{
	if ( num <= NumAllocated() )
		return;

	if ( num > MaxCapacity() )
	{
		Plat_FatalError( "%s: maximum capacity of %llu exceeded by request %llu (currently %llu)\n", __FUNCTION__, ( uint64 )MaxCapacity(), ( uint64 )num, ( uint64 )NumAllocated() );
		DebuggerBreak();
	}

	int nNewAllocated = num;
	if ( !force )
	{
		nNewAllocated = NumAllocated();
		do
		{
			if ( nNewAllocated > MaxCapacity() / 2 )
			{
				nNewAllocated = MaxCapacity();
				break;
			}

			nNewAllocated = ( nNewAllocated < 4 ) ? 4 : nNewAllocated * 2;
		}
		while ( nNewAllocated < num );
	}

	int64 nAllocated = ( int64 )NumAllocated() + m_Memory.Grow( nNewAllocated - NumAllocated(), true );
	m_nAllocated = ( I )MIN( nAllocated, ( int64 )MaxCapacity() );
}

template< class T, class I, class A >
void CUtlBlockVector<T, I, A>::GrowCount( int num )
{
	if ( num > MaxCapacity() - Count() )
	{
		Plat_FatalError( "%s: unable to grow count by %u (currently %u)\n", __FUNCTION__, num, Count() );
		DebuggerBreak();
	}

	m_nCount = ( I )( m_nCount + num );
	EnsureCapacity( m_nCount );
}

template< class T, class I, class A >
T* CUtlBlockVector<T, I, A>::AddToTailGetPtr()
{
	return &Element( AddToTail() );
}

template< class T, class I, class A >
I CUtlBlockVector<T, I, A>::AddToTail()
{
	GrowCount( 1 );
	I elem = ( I )( m_nCount - 1 );
	Construct( &Element( elem ) );
	return elem;
}

template< class T, class I, class A >
I CUtlBlockVector<T, I, A>::AddToTail( const T& src )
{
	GrowCount( 1 );
	I elem = ( I )( m_nCount - 1 );
	CopyConstruct( &Element( elem ), src );
	return elem;
}

template< class T, class I, class A >
I CUtlBlockVector<T, I, A>::AddMultipleToTail( int num )
{
	int nOldCount = Count();

	if ( num > 0 )
	{
		GrowCount( num );

		for ( int i = nOldCount; i < Count(); i++ )
			Construct( &Element( ( I )i ) );
	}

	return ( I )nOldCount;
}

template< class T, class I, class A >
void CUtlBlockVector<T, I, A>::SetCount( int count )
{
	if ( count > Count() )
		AddMultipleToTail( count - Count() );
	else if ( count < Count() )
		RemoveMultipleFromTail( Count() - count );
}

template< class T, class I, class A >
void CUtlBlockVector<T, I, A>::EnsureCount( int num )
{
	if ( Count() < num )
		AddMultipleToTail( num - Count() );
}

template< class T, class I, class A >
I CUtlBlockVector<T, I, A>::Find( const T& src ) const
{
	for ( int i = 0; i < Count(); ++i )
	{
		if ( Element( ( I )i ) == src )
			return ( I )i;
	}

	return InvalidIndex();
}

template< class T, class I, class A >
void CUtlBlockVector<T, I, A>::FastRemove( I elem )
{
	Assert( IsValidIndex( elem ) );

	Destruct( &Element( elem ) );

	I last = ( I )( m_nCount - 1 );
	if ( elem != last )
		V_memmove( &Element( elem ), &Element( last ), sizeof( T ) );

	--m_nCount;
}

template< class T, class I, class A >
void CUtlBlockVector<T, I, A>::Remove( I elem )
{
	Assert( IsValidIndex( elem ) );

	Destruct( &Element( elem ) );

	for ( int i = elem; i < Count() - 1; i++ )
		V_memmove( &Element( ( I )i ), &Element( ( I )( i + 1 ) ), sizeof( T ) );

	--m_nCount;
}

template< class T, class I, class A >
bool CUtlBlockVector<T, I, A>::FindAndRemove( const T& src )
{
	I elem = Find( src );
	if ( elem == InvalidIndex() )
		return false;

	Remove( elem );
	return true;
}

template< class T, class I, class A >
bool CUtlBlockVector<T, I, A>::FindAndFastRemove( const T& src )
{
	I elem = Find( src );
	if ( elem == InvalidIndex() )
		return false;

	FastRemove( elem );
	return true;
}

template< class T, class I, class A >
void CUtlBlockVector<T, I, A>::RemoveMultipleFromTail( int num )
{
	Assert( num >= 0 && num <= Count() );

	for ( int i = Count() - num; i < Count(); i++ )
		Destruct( &Element( ( I )i ) );

	m_nCount = ( I )( m_nCount - num );
}

template< class T, class I, class A >
void CUtlBlockVector<T, I, A>::RemoveAll()
{
	RemoveMultipleFromTail( Count() );
}

template< class T, class I, class A >
void CUtlBlockVector<T, I, A>::Purge()
{
	RemoveAll();
	m_Memory.Purge();
	m_nAllocated = 0;
}

template< class T, class I, class A >
void CUtlBlockVector<T, I, A>::Swap( CUtlBlockVector<T, I, A> &other )
{
	V_swap( m_nAllocated, other.m_nAllocated );
	V_swap( m_nCount, other.m_nCount );
	m_Memory.Swap( other.m_Memory );
}

#endif // UTLBLOCKVECTOR_H
