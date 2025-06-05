//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: 
//
// $NoKeywords: $
//
// A growable array class that maintains a free list and keeps elements
// in the same location
//=============================================================================//

#ifndef UTLVECTOR_H
#define UTLVECTOR_H

#ifdef _WIN32
#pragma once
#endif


#include <string.h>
#include "tier0/platform.h"
#include "tier0/dbg.h"
#include "tier0/utlstring.h"
#include "tier1/threadtools.h"
#include "tier1/utlmemory.h"
#include "tier1/utlblockmemory.h"
#include "tier0/strtools.h"

#include <initializer_list>

#define UTL_INVAL_VECTOR_ELEM ((I)~0)

#define FOR_EACH_TYPED_VEC( vecName, iterType, iteratorName ) \
	if ( (vecName).IsUtlVector ) for ( iterType iteratorName = 0; iteratorName < (vecName).Count(); iteratorName++ )
#define FOR_EACH_TYPED_VEC_BACK( vecName, iterType, iteratorName ) \
	if ( (vecName).IsUtlVector ) for ( iterType iteratorName = (vecName).Count()-1; iteratorName >= 0; iteratorName-- )

#define FOR_EACH_VEC( vecName, iteratorName ) FOR_EACH_TYPED_VEC( vecName, typename C11RemoveReference< decltype(vecName) >::Type::IndexType_t, iteratorName )
#define FOR_EACH_VEC_BACK( vecName, iteratorName ) FOR_EACH_TYPED_VEC_BACK( vecName, typename C11RemoveReference< decltype(vecName) >::Type::IndexType_t, iteratorName )

// UtlVector derives from this so we can do the type check above
struct base_vector_t
{
public:
	enum { IsUtlVector = true }; // Used to match this at compiletime 		
};

//-----------------------------------------------------------------------------
// The CUtlVectorBase class:
// A growable array class which doubles in size by default.
// It will always keep all elements consecutive in memory, and may move the
// elements around in memory (via a PvRealloc) when elements are inserted or
// removed. Clients should therefore refer to the elements of the vector
// by index (they should *never* maintain pointers to elements in the vector).
//-----------------------------------------------------------------------------
template< typename T, typename I = int, class A = CUtlMemory<T, I> >
class CUtlVectorBase : public base_vector_t
{
	using CAllocator = A;

public:
	using ElemType_t = T;
	using iterator = T*;
	using const_iterator = const T*;
	using IndexType_t = I;

	// Set the growth policy and initial capacity. Count will always be zero. This is different from std::vector
	// where the constructor sets count as well as capacity.
	// growSize of zero implies the default growth pattern which is exponential.
	explicit CUtlVectorBase( I growSize = 0, I initialCapacity = 0 );

	// Initialize with separately allocated buffer, setting the capacity and count.
	// The container will not be growable.
	CUtlVectorBase( T* pMemory, I initialCapacity, I initialCount = 0 );

	CUtlVectorBase( const CUtlVectorBase<T, I, A> &copyFrom );
	CUtlVectorBase( CUtlVectorBase<T, I, A> &&moveFrom );

	~CUtlVectorBase();
	
	// Copy & move the array.
	CUtlVectorBase<T, I, A>& operator=( const CUtlVectorBase<T, I, A> &copyFrom );
	CUtlVectorBase<T, I, A>& operator=( CUtlVectorBase<T, I, A> &&moveFrom );

	CUtlVectorBase<T, I, A> &CopyFrom( const CUtlVectorBase<T, I, A> &copyFrom );
	CUtlVectorBase<T, I, A> &MoveFrom( CUtlVectorBase<T, I, A> &&moveFrom );

	// element access
	T& operator[]( I i );
	const T& operator[]( I i ) const;
	T& Element( I i );
	const T& Element( I i ) const;
	T&& MoveElement( I i );
	T& Head();
	const T& Head() const;
	T& Tail();
	const T& Tail() const;

	// They are forward compatible with the C++ 11 range-based for loops.
	iterator begin()						{ return Base(); }
	const_iterator begin() const			{ return Base(); }
	iterator end()							{ return Base() + Count(); }
	const_iterator end() const				{ return Base() + Count(); }

	// Gets the base address (can change when adding elements!)
	T* Base()								{ return m_Memory.Base(); }
	const T* Base() const					{ return m_Memory.Base(); }

	// Attaches the buffer to external memory....
	void SetExternalBuffer( T *pMemory, int allocationCount, int numElements = 0 )
	{
		m_Memory.SetExternalBuffer( pMemory, allocationCount );
		m_Size = numElements;
	}
	void SetExternalBuffer( const T *pMemory, int allocationCount, int numElements = 0 )
	{
		m_Memory.SetExternalBuffer( pMemory, allocationCount );
		m_Size = numElements;
	}

	void AssumeMemory( T *pMemory, int allocationCount, int numElements = 0 )
	{
		m_Memory.AssumeMemory( pMemory, allocationCount );
		m_Size = numElements;
	}

	T *Detach()
	{
		m_Size = 0;
		return m_Memory.Detach();
	}
	void *DetachMemory()
	{
		m_Size = 0;
		return m_Memory.DetachMemory();
	}

	// Returns the number of elements in the vector
	I Count() const;
	/// are there no elements? For compatibility with lists.
	inline bool IsEmpty( void ) const
	{
		return ( Count() == 0 );
	}

	// Is element index valid?
	bool IsValidIndex( I i ) const;
	static I InvalidIndex();

	// Adds an element, uses default constructor
	I AddToHead();
	I AddToTail();
	T *AddToTailGetPtr();
	I InsertBefore( I elem );
	I InsertAfter( I elem );

	// Adds an element, uses copy & move constructor
	I AddToHead( const T& copySrc );
	I AddToHead( T&& moveSrc );
	I AddToTail( const T& copySrc );
	I AddToTail( T&& moveSrc );
	I InsertBefore( I elem, const T& copySrc );
	I InsertBefore( I elem, T&& moveSrc );
	I InsertAfter( I elem, const T& copySrc );
	I InsertAfter( I elem, T&& moveSrc );

	// Adds multiple elements, uses default constructor
	I AddMultipleToHead( I num );
	I AddMultipleToTail( I num );	   
	I AddMultipleToTail( I num, const T *pToCopy );	   
	I InsertMultipleBefore( I elem, I num );
	I InsertMultipleBefore( I elem, I num, const T *pToCopy );
	I InsertMultipleAfter( I elem, I num );

	// Calls RemoveAll() then AddMultipleToTail.
	// SetSize is a synonym for SetCount
	void SetSize( I size );
	// SetCount deletes the previous contents of the container and sets the
	// container to have this many elements.
	// Use GetCount to retrieve the current count.
	void SetCount( I count );
	void SetCountNonDestructively( I count ); //sets count by adding or removing elements to tail TODO: This should probably be the default behavior for SetCount
	
	// Calls SetSize and copies each element.
	void CopyArray( const T *pArray, I size );

	// Fast swap
	void Swap( CUtlVectorBase< T, I, A > &vec );
	
	// Add the specified array to the tail.
	I AddVectorToTail( CUtlVectorBase<T, I, A> const &copySrc );
	I AddVectorToTail( CUtlVectorBase<T, I, A> &&moveSrc );

	// Finds an element (element needs operator== defined)
	I Find( const T& compSrc ) const;
	I Find( T&& moveSrc ) const;
	void FillWithValue( const T& copySrc );
	void FillWithValue( T&& moveSrc );

	bool HasElement( const T& copySrc ) const;

	// Makes sure we have enough memory allocated to store a requested # of elements
	// Use NumAllocated() to retrieve the current capacity.
	void EnsureCapacity( I num );

	// Makes sure we have at least this many elements
	// Use GetCount to retrieve the current count.
	void EnsureCount( I num );

	// Element removal
	void FastRemove( I elem );	// doesn't preserve order
	void Remove( I elem );		// preserves order, shifts elements

	// removes first occurrence of src, preserves order, shifts elements
	bool FindAndRemove( const T& compSrc );
	bool FindAndRemove( T&& moveSrc );

	// removes first occurrence of src, doesn't preserve order
	bool FindAndFastRemove( const T& compSrc );
	bool FindAndFastRemove( T&& moveSrc );

	void RemoveMultiple( I elem, I num );	// preserves order, shifts elements
	void RemoveMultipleFromHead(I num); // removes num elements from tail
	void RemoveMultipleFromTail(I num); // removes num elements from tail
	void RemoveAll();				// doesn't deallocate memory

	// Memory deallocation
	void Purge();

	// Purges the list and calls delete on each element in it.
	void PurgeAndDeleteElements();

	// Compacts the vector to the number of elements actually in use 
	void Compact();

	// Set the size by which it grows when it needs to allocate more memory.
	void SetGrowSize( I size )			{ m_Memory.SetGrowSize( size ); }

	I NumAllocated() const;	// Only use this if you really know what you're doing!

	void Sort( I (__cdecl *pfnCompare)(const T *, const T *) );

	// Call this to quickly sort non-contiguously allocated vectors
	void InPlaceQuickSort( I (__cdecl *pfnCompare)(const T *, const T *) );
	// reverse the order of elements
	void Reverse( );

#ifdef DBGFLAG_VALIDATE
	void Validate( CValidator &validator, char *pchName );		// Validate our internal structures
#endif // DBGFLAG_VALIDATE


	I SortedFindLessOrEqual( const T& search, bool( __cdecl *pfnLessFunc )( const T& src1, const T& src2, void *pCtx ), void *pLessContext ) const;
	I SortedInsert( const T& src, bool( __cdecl *pfnLessFunc )( const T& src1, const T& src2, void *pCtx ), void *pLessContext );

	/// sort using std:: and expecting a "<" function to be defined for the type
	void Sort( void );

	/// sort using std:: with a predicate. e.g. [] -> bool ( T &a, T &b ) { return a < b; }
	template <class F> void SortPredicate( F &&predicate );

protected:
	// Grows the vector
	void GrowVector( I num = 1 );


	// Shifts elements....
	void ShiftElementsRight( I elem, I num = 1 );
	void ShiftElementsLeft( I elem, I num = 1 );

	I m_Size;
	CAllocator m_Memory;

#ifndef _X360
	// For easier access to the elements through the debugger
	// it's in release builds so this can be used in libraries correctly
	// T *m_pElements;

	inline void ResetDbgInfo()
	{
		// m_pElements = Base();
	}
#else
	inline void ResetDbgInfo() {}
#endif

private:
	void InPlaceQuickSort_r( I (__cdecl *pfnCompare)(const T *, const T *), I nLeft, I nRight );
};

template < class T, typename I = int, class A = CUtlMemory< T, I > >
class CUtlVector : public CUtlVectorBase< T, I, A >
{
	typedef CUtlVectorBase< T, I, A > BaseClass;

public:
	using BaseClass::BaseClass;

	using ElemType_t = T;
	using iterator = T*;
	using const_iterator = const T*;
	using IndexType_t = I;

	CUtlVector( const std::initializer_list< T > elements );
};

template< class T, typename I = int >
class CUtlVector_RawAllocator : public CUtlVectorBase< T, I, CUtlMemory_RawAllocator<T, I> >
{
	typedef CUtlVectorBase< T, I, CUtlMemory_RawAllocator<T, I> > BaseClass;

public:
	explicit CUtlVector_RawAllocator( I growSize = 0, I initSize = 0 ) : BaseClass( growSize, initSize ) {}
	CUtlVector_RawAllocator( T* pMemory, I allocationCount, I numElements ) : BaseClass( pMemory, allocationCount, numElements ) {}
};

// this is kind of ugly, but until C++ gets templatized typedefs in C++0x, it's our only choice
template < class T, class I = int >
class CUtlBlockVector : public CUtlVectorBase< T, I, CUtlBlockMemory< T, I > >
{
	typedef CUtlVectorBase< T, I, CUtlBlockMemory< T, I > > BaseClass;

public:
	explicit CUtlBlockVector( I growSize = 0, I initSize = 0 ) : BaseClass( growSize, initSize ) {}
};

//-----------------------------------------------------------------------------
// The CUtlVectorMT class:
// An array class with spurious mutex protection. Nothing is actually protected
// unless you call Lock and Unlock. Also, the Mutex_t is actually not a type
// but a member which probably isn't used.
//-----------------------------------------------------------------------------
template< class BASE_UTLVECTOR, class MUTEX_TYPE = CThreadFastMutex >
class CUtlVectorMT : public BASE_UTLVECTOR, public MUTEX_TYPE
{
	typedef BASE_UTLVECTOR BaseClass;
public:
	// MUTEX_TYPE Mutex_t;

	// constructor, destructor
	explicit CUtlVectorMT( int growSize = 0, int initSize = 0 ) : BaseClass( growSize, initSize ) {}
	CUtlVectorMT( typename BaseClass::ElemType_t* pMemory, int numElements ) : BaseClass( pMemory, numElements ) {}
};

//-----------------------------------------------------------------------------
// The CUtlVectorFixed class:
// A array class with a fixed allocation scheme
//-----------------------------------------------------------------------------
template< class T, int MAX_SIZE >
class CUtlVectorFixed : public CUtlVectorBase< T, int, CUtlMemoryFixed<T, MAX_SIZE > >
{
	typedef CUtlVectorBase< T, int, CUtlMemoryFixed<T, MAX_SIZE > > BaseClass;

public:
	// constructor, destructor
	explicit CUtlVectorFixed( int growSize = 0, int initSize = 0 ) : BaseClass( growSize, initSize ) {}
	CUtlVectorFixed( T* pMemory, int numElements ) : BaseClass( pMemory, numElements ) {}
};


//-----------------------------------------------------------------------------
// The CUtlVectorFixedGrowable class:
// A array class with a fixed allocation scheme backed by a dynamic one
//-----------------------------------------------------------------------------
template< class T, int MAX_SIZE >
class CUtlVectorFixedGrowable : public CUtlVectorBase< T, int, CUtlMemoryFixedGrowable<T, MAX_SIZE, int > >
{
	typedef CUtlVectorBase< T, int, CUtlMemoryFixedGrowable< T, MAX_SIZE > > BaseClass;

public:
	// constructor, destructor
	explicit CUtlVectorFixedGrowable( int growSize = 0 ) : BaseClass( growSize, MAX_SIZE ) {}
};

// A fixed growable vector that's castable to CUtlVector
template< class T, int FIXED_SIZE >
class CUtlVectorFixedGrowableCompat : public CUtlVectorBase< T, int >
{
	typedef CUtlVectorBase< T, int > BaseClass;

public:
	// constructor, destructor
	CUtlVectorFixedGrowableCompat( int growSize = 0 ) : BaseClass( nullptr, FIXED_SIZE, growSize )
	{
		this->m_Memory.m_pMemory = m_FixedMemory.Base();
	}

	AlignedByteArray_t< FIXED_SIZE, T > m_FixedMemory;
};

//-----------------------------------------------------------------------------
// The CUtlVectorConservative class:
// A array class with a conservative allocation scheme
//-----------------------------------------------------------------------------
template< class T, typename I = int >
class CUtlVectorConservative : public CUtlVectorBase< T, I, CUtlMemoryConservative<T, I> >
{
	typedef CUtlVectorBase< T, I, CUtlMemoryConservative<T> > BaseClass;

public:
	// constructor, destructor
	explicit CUtlVectorConservative( I growSize = 0, I initSize = 0 ) : BaseClass( growSize, initSize ) {}
	CUtlVectorConservative( T* pMemory, I numElements ) : BaseClass( pMemory, numElements ) {}
};


//-----------------------------------------------------------------------------
// The CUtlVectorUltra Conservative class:
// A array class with a very conservative allocation scheme, with customizable allocator
// Especialy useful if you have a lot of vectors that are sparse, or if you're
// carefully packing holders of vectors
//-----------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4200) // warning C4200: nonstandard extension used : zero-sized array in struct/union
#pragma warning(disable : 4815 ) // warning C4815: 'staticData' : zero-sized array in stack object will have no elements
#endif

class CUtlVectorUltraConservativeAllocator
{
public:
	static void *Alloc( size_t nSize )
	{
		return MemAlloc_Alloc( nSize );
	}

	static void *Realloc( void *pMem, size_t nSize )
	{
		return g_pMemAlloc->Realloc( pMem, nSize );
	}

	static void Free( void *pMem )
	{
		g_pMemAlloc->Free( pMem );
	}

	static size_t GetSize( void *pMem )
	{
		return mallocsize( pMem );
	}

};

template <typename T, typename I = int, typename A = CUtlVectorUltraConservativeAllocator >
class CUtlVectorUltraConservative : private A
{
public:
	// Don't inherit from base_vector_t because multiple-inheritance increases
	// class size!
	enum { IsUtlVector = true }; // Used to match this at compiletime 		

	CUtlVectorUltraConservative()
	{
		m_pData = StaticData();
	}

	~CUtlVectorUltraConservative()
	{
		RemoveAll();
	}

	I Count() const
	{
		return m_pData->m_Size;
	}

	static I InvalidIndex()
	{
		return UTL_INVAL_VECTOR_ELEM;
	}

	inline bool IsValidIndex( I i ) const
	{
		return (i != UTL_INVAL_VECTOR_ELEM) && (i < Count());
	}

	T& operator[]( I i )
	{
		Assert( IsValidIndex( i ) );
		return m_pData->m_Elements[i];
	}

	const T& operator[]( I i ) const
	{
		Assert( IsValidIndex( i ) );
		return m_pData->m_Elements[i];
	}

	T& Element( I i )
	{
		Assert( IsValidIndex( i ) );
		return m_pData->m_Elements[i];
	}

	const T& Element( I i ) const
	{
		Assert( IsValidIndex( i ) );
		return m_pData->m_Elements[i];
	}

	void EnsureCapacity( I num )
	{
		I nCurCount = Count();
		if ( num <= nCurCount )
		{
			return;
		}
		if ( m_pData == StaticData() )
		{
			m_pData = (Data_t *)A::Alloc( sizeof(Data_t) + ( num * sizeof(T) ) );
			m_pData->m_Size = 0;
		}
		else
		{
			I nNeeded = sizeof(Data_t) + ( num * sizeof(T) );
			I nHave = A::GetSize( m_pData );
			if ( nNeeded > nHave )
			{
				m_pData = (Data_t *)A::Realloc( m_pData, nNeeded );
			}
		}
	}

	I AddToTail( const T& copySrc )
	{
		I iNew = Count();
		EnsureCapacity( Count() + 1 );
		m_pData->m_Elements[iNew] = copySrc;
		m_pData->m_Size++;
		return iNew;
	}

	T * AddToTailGetPtr()
	{
		return &Element( AddToTail() );
	}

	void RemoveAll()
	{
		if ( Count() )
		{
			for (I i = m_pData->m_Size; i-- > 0; )
			{
				// Global scope to resolve conflict with Scaleform 4.0
				::Destruct(&m_pData->m_Elements[i]);
			}
		}
		if ( m_pData != StaticData() )
		{
			A::Free( m_pData );
			m_pData = StaticData();

		}
	}

	void PurgeAndDeleteElements()
	{
		if ( m_pData != StaticData() )
		{
			for( I i=0; i < m_pData->m_Size; i++ )
			{
				delete Element(i);
			}
			RemoveAll();
		}
	}

	void FastRemove( I elem )
	{
		Assert( IsValidIndex(elem) );

		// Global scope to resolve conflict with Scaleform 4.0
		::Destruct( &Element(elem) );
		if (Count() > 0)
		{
			if ( elem != m_pData->m_Size - 1 )
				memcpy( &Element(elem), &Element(m_pData->m_Size - 1), sizeof(T) );
			--m_pData->m_Size;
		}
		if ( !m_pData->m_Size )
		{
			A::Free( m_pData );
			m_pData = StaticData();
		}
	}

	void Remove( I elem )
	{
		// Global scope to resolve conflict with Scaleform 4.0
		::Destruct( &Element(elem) );
		ShiftElementsLeft(elem);

		--m_pData->m_Size;
		if ( !m_pData->m_Size )
		{
			A::Free( m_pData );
			m_pData = StaticData();
		}
	}

	I Find( const T& copySrc ) const
	{
		I nCount = Count();

		for ( I i = 0; i < nCount; ++i )
			if (Element(i) == copySrc)
				return i;

		return UTL_INVAL_VECTOR_ELEM;
	}

	bool FindAndRemove( const T& copySrc )
	{
		I elem = Find( copySrc );

		if ( elem != UTL_INVAL_VECTOR_ELEM )
		{
			Remove( elem );
			return true;
		}

		return false;
	}


	bool FindAndFastRemove( const T& copySrc )
	{
		I elem = Find( copySrc );
		if ( elem != UTL_INVAL_VECTOR_ELEM )
		{
			FastRemove( elem );
			return true;
		}
		return false;
	}

	bool DebugCompileError_ANonVectorIsUsedInThe_FOR_EACH_VEC_Macro( void ) const { return true; }

	struct Data_t
	{
		I m_Size;
		T m_Elements[];
	};

	Data_t *m_pData;
private:
	void ShiftElementsLeft( I elem, I num = 1 )
	{
		I Size = Count();
		Assert( IsValidIndex(elem) || ( Size == 0 ) || ( num == 0 ));
		I numToMove = Size - elem - num;
		if ((numToMove > 0) && (num > 0))
		{
			Q_memmove( &Element(elem), &Element(elem+num), numToMove * sizeof(T) );

#ifdef _DEBUG
			Q_memset( &Element(Size-num), 0xDD, num * sizeof(T) );
#endif
		}
	}



	static Data_t *StaticData()
	{
		static Data_t staticData;
		Assert( staticData.m_Size == 0 );
		return &staticData;
	}
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

// Make sure nobody adds multiple inheritance and makes this class bigger.
COMPILE_TIME_ASSERT( sizeof(CUtlVectorUltraConservative<size_t>) == sizeof(void*) );


//-----------------------------------------------------------------------------
// The CCopyableUtlVector class:
// A array class that allows copy construction (so you can nest a CUtlVector inside of another one of our containers)
//  WARNING - this class lets you copy construct which can be an expensive operation if you don't carefully control when it happens
// Only use this when nesting a CUtlVector() inside of another one of our container classes (i.e a CUtlMap)
//-----------------------------------------------------------------------------
template< class T, typename I = int >
class CCopyableUtlVector : public CUtlVectorBase< T, I, CUtlMemory<T, I> >
{
	typedef CUtlVectorBase< T, I, CUtlMemory<T> > BaseClass;
public:
	explicit CCopyableUtlVector( I growSize = 0, I initSize = 0 ) : BaseClass( growSize, initSize ) {}
	CCopyableUtlVector( T* pMemory, I numElements ) : BaseClass( pMemory, numElements ) {}
	virtual ~CCopyableUtlVector() {}
	CCopyableUtlVector( CCopyableUtlVector const& vec ) { this->CopyArray( vec.Base(), vec.Count() ); }
	CCopyableUtlVector( CUtlVectorBase<T> const& vec ) { this->CopyArray( vec.Base(), vec.Count() ); }
};

//-----------------------------------------------------------------------------
// The CCopyableUtlVector class:
// A array class that allows copy construction (so you can nest a CUtlVector inside of another one of our containers)
//  WARNING - this class lets you copy construct which can be an expensive operation if you don't carefully control when it happens
// Only use this when nesting a CUtlVector() inside of another one of our container classes (i.e a CUtlMap)
//-----------------------------------------------------------------------------
template< class T, size_t MAX_SIZE >
class CCopyableUtlVectorFixed : public CUtlVectorFixed< T, MAX_SIZE >
{
	typedef CUtlVectorFixed< T, MAX_SIZE > BaseClass;
public:
	explicit CCopyableUtlVectorFixed( size_t growSize = 0, size_t initSize = 0 ) : BaseClass( growSize, initSize ) {}
	CCopyableUtlVectorFixed( T* pMemory, size_t numElements ) : BaseClass( pMemory, numElements ) {}
	virtual ~CCopyableUtlVectorFixed() {}
	CCopyableUtlVectorFixed( CCopyableUtlVectorFixed const& vec ) { this->CopyArray( vec.Base(), vec.Count() ); }
};

//-----------------------------------------------------------------------------
// The CNetworkUtlVectorBase class:
//-----------------------------------------------------------------------------
template< class T, typename I = int, typename A = CUtlMemory< T, I > >
class CNetworkUtlVectorBase : public CUtlVectorBase< T, I, A >
{
	typedef CUtlVectorBase< T, I, A > BaseClass;
public:
	using BaseClass::BaseClass;
};

// TODO (Ilya): It seems like all the functions in CUtlVector are simple enough that they should be inlined.

//-----------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
inline CUtlVectorBase<T, I, A>::CUtlVectorBase( I growSize, I initSize )	: 
	m_Size(0), m_Memory(growSize, initSize)
{
	ResetDbgInfo();
}

template< typename T, typename I, class A >
inline CUtlVectorBase<T, I, A>::CUtlVectorBase( T* pMemory, I allocationCount, I numElements )	: 
	m_Size(numElements), m_Memory(pMemory, allocationCount)
{
	ResetDbgInfo();
}

template< typename T, typename I, class A >
inline CUtlVectorBase<T, I, A>::CUtlVectorBase( const CUtlVectorBase<T, I, A> &copyFrom ) : 
	m_Size(copyFrom.m_Size), m_Memory(copyFrom.m_Memory)
{
	ResetDbgInfo();
	CopyFrom( copyFrom );
}

template< typename T, typename I, class A >
inline CUtlVectorBase<T, I, A>::CUtlVectorBase( CUtlVectorBase<T, I, A> &&moveFrom ) : 
	m_Size(0)
{
	ResetDbgInfo();
	MoveFrom( Move( moveFrom ) );
}

#if defined(COMPILER_MSVC)
#pragma warning(push)
#pragma warning(disable : 4722)
#endif
template< typename T, typename I, class A >
inline CUtlVectorBase<T, I, A>::~CUtlVectorBase()
{
	Purge();
}
#if defined(COMPILER_MSVC)
#pragma warning(pop)
#endif

template< typename T, typename I, class A >
inline CUtlVectorBase<T, I, A>& CUtlVectorBase<T, I, A>::operator=( const CUtlVectorBase<T, I, A> &copyFrom )
{
	return CopyFrom( copyFrom );
}

template< typename T, typename I, class A >
inline CUtlVectorBase<T, I, A>& CUtlVectorBase<T, I, A>::operator=( CUtlVectorBase<T, I, A> &&moveFrom )
{
	return MoveFrom( Move( moveFrom ) );
}

template< typename T, typename I, class A >
inline CUtlVectorBase<T, I, A>& CUtlVectorBase<T, I, A>::CopyFrom( const CUtlVectorBase<T, I, A> &copyFrom )
{
	I nCount = copyFrom.Count();

	SetSize( nCount );

	for ( I i = 0; i < nCount; i++ )
	{
		CopyConstruct( &Element( i ), copyFrom[ i ] );
	}

	return *this;
}

template< typename T, typename I, class A >
inline CUtlVectorBase<T, I, A>& CUtlVectorBase<T, I, A>::MoveFrom( CUtlVectorBase<T, I, A> &&moveFrom )
{
	I nCount = moveFrom.Count();

	SetSize( nCount );

	for ( I i = 0; i < nCount; i++ )
	{
		MoveConstruct( &Element( i ), Move( moveFrom.MoveElement(i) ) );
	}

	return *this;
}


//-----------------------------------------------------------------------------
// element access
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
inline T& CUtlVectorBase<T, I, A>::operator[]( I i )
{
	Assert( i < m_Size );
	return m_Memory[ i ];
}

template< typename T, typename I, class A >
inline const T& CUtlVectorBase<T, I, A>::operator[]( I i ) const
{
	Assert( i < m_Size );
	return m_Memory[ i ];
}

template< typename T, typename I, class A >
inline T& CUtlVectorBase<T, I, A>::Element( I i )
{
	Assert( i < m_Size );
	return m_Memory[ i ];
}

template< typename T, typename I, class A >
inline const T& CUtlVectorBase<T, I, A>::Element( I i ) const
{
	Assert( i < m_Size );
	return m_Memory[ i ];
}

template< typename T, typename I, class A >
inline T&& CUtlVectorBase<T, I, A>::MoveElement( I i )
{
	Assert( i < m_Size );
	return Move( m_Memory[ i ] );
}

template< typename T, typename I, class A >
inline T& CUtlVectorBase<T, I, A>::Head()
{
	Assert( m_Size > 0 );
	return m_Memory[ 0 ];
}

template< typename T, typename I, class A >
inline const T& CUtlVectorBase<T, I, A>::Head() const
{
	Assert( m_Size > 0 );
	return m_Memory[ 0 ];
}

template< typename T, typename I, class A >
inline T& CUtlVectorBase<T, I, A>::Tail()
{
	Assert( m_Size > 0 );
	return m_Memory[ m_Size - 1 ];
}

template< typename T, typename I, class A >
inline const T& CUtlVectorBase<T, I, A>::Tail() const
{
	Assert( m_Size > 0 );
	return m_Memory[ m_Size - 1 ];
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Reverse - reverse the order of elements, akin to std::vector<>::reverse()
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::Reverse( )
{
	for ( I i = 0; i < m_Size / 2; i++ )
	{
		V_swap( m_Memory[ i ], m_Memory[ m_Size - 1 - i ] );
#if defined( UTLVECTOR_TRACK_STACKS )
		if ( bTrackingEnabled )
		{
			V_swap( m_pElementStackStatsIndices[ i ], m_pElementStackStatsIndices[ m_Size - 1 - i ] );
		}
#endif
	}
}

// Count
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::Count() const
{
	return m_Size;
}


//-----------------------------------------------------------------------------
// Is element index valid?
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
inline bool CUtlVectorBase<T, I, A>::IsValidIndex( I i ) const
{
	return (i >= 0) && (i < m_Size);
}
 

//-----------------------------------------------------------------------------
// Returns in invalid index
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::InvalidIndex()
{
	return UTL_INVAL_VECTOR_ELEM;
}


//-----------------------------------------------------------------------------
// Grows the vector
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::GrowVector( I num )
{
	if (m_Size + num > m_Memory.NumAllocated())
	{
		MEM_ALLOC_CREDIT_CLASS();
		m_Memory.Grow( m_Size + num - m_Memory.NumAllocated() );
	}

	m_Size += num;
	ResetDbgInfo();
}


//-----------------------------------------------------------------------------
// finds a particular element
// You must sort the list before using or your results will be wrong
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
I CUtlVectorBase<T, I, A>::SortedFindLessOrEqual( const T& search, bool (__cdecl *pfnLessFunc)( const T& src1, const T& src2, void *pCtx ), void *pLessContext ) const
{
	I start = 0, end = Count() - 1;
	while (start <= end)
	{
		I mid = (start + end) >> 1;
		if ( pfnLessFunc( Element(mid), search, pLessContext ) )
		{
			start = mid + 1;
		}
		else if ( pfnLessFunc( search, Element(mid), pLessContext ) )
		{
			end = mid - 1;
		}
		else
		{
			return mid;
		}
	}
	return end;
}


template< typename T, typename I, class A >
I CUtlVectorBase<T, I, A>::SortedInsert( const T& src, bool (__cdecl *pfnLessFunc)( const T& src1, const T& src2, void *pCtx ), void *pLessContext )
{
	I pos = SortedFindLessOrEqual( src, pfnLessFunc, pLessContext ) + 1;
	GrowVector();
	ShiftElementsRight(pos);
	CopyConstruct<T>( &Element(pos), src );
	//UTLVECTOR_STACK_STATS_ALLOCATED_SINGLE( pos );
	return pos;
}





//-----------------------------------------------------------------------------
// Sorts the vector
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::Sort( I (__cdecl *pfnCompare)(const T *, const T *) )
{
	typedef I (__cdecl *QSortCompareFunc_t)(const void *, const void *);
	if ( Count() <= 1 )
		return;

	if ( Base() )
	{
		qsort( Base(), Count(), sizeof(T), (QSortCompareFunc_t)(pfnCompare) );
	}
	else
	{
		Assert( 0 );
		// this path is untested
		// if you want to sort vectors that use a non-sequential memory allocator,
		// you'll probably want to patch in a quicksort algorithm here
		// I just threw in this bubble sort to have something just in case...

		for ( I i = m_Size - 1; i-- > 0; )
		{
			for ( I j = 1; j <= i; ++j )
			{
				if ( pfnCompare( &Element( j - 1 ), &Element( j ) ) < 0 )
				{
					V_swap( Element( j - 1 ), Element( j ) );
				}
			}
		}
	}
}


//----------------------------------------------------------------------------------------------
// Private function that does the in-place quicksort for non-contiguously allocated vectors.
//----------------------------------------------------------------------------------------------
template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::InPlaceQuickSort_r( I (__cdecl *pfnCompare)(const T *, const T *), I nLeft, I nRight )
{
	I nPivot;
	I nLeftIdx = nLeft;
	I nRightIdx = nRight;

	if ( nRight - nLeft > 0 )
	{
		nPivot = ( nLeft + nRight ) / 2;

		while ( ( nLeftIdx <= nPivot ) && ( nRightIdx >= nPivot ) )
		{
			while ( ( pfnCompare( &Element( nLeftIdx ), &Element( nPivot ) ) < 0 ) && ( nLeftIdx <= nPivot ) )
			{
				nLeftIdx++;
			}

			while ( ( pfnCompare( &Element( nRightIdx ), &Element( nPivot ) ) > 0 ) && ( nRightIdx >= nPivot ) )
			{
				nRightIdx--;
			}

			V_swap( Element( nLeftIdx ), Element( nRightIdx ) );

			nLeftIdx++;
			nRightIdx--;

			if ( ( nLeftIdx - 1 ) == nPivot )
			{
				nPivot = nRightIdx = nRightIdx + 1;
			}
			else if ( nRightIdx + 1 == nPivot )
			{
				nPivot = nLeftIdx = nLeftIdx - 1;
			}
		}

		InPlaceQuickSort_r( pfnCompare, nLeft, nPivot - 1 );
		InPlaceQuickSort_r( pfnCompare, nPivot + 1, nRight );
	}
}


//----------------------------------------------------------------------------------------------
// Call this to quickly sort non-contiguously allocated vectors. Sort uses a slower bubble sort.
//----------------------------------------------------------------------------------------------
template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::InPlaceQuickSort( I (__cdecl *pfnCompare)(const T *, const T *) )
{
	InPlaceQuickSort_r( pfnCompare, 0, Count() - 1 );
}

// Default Sort using operator<
template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::Sort( void )
{
	SortPredicate( [](const T& a, const T& b) { return a < b; } );
}

// Sort with custom predicate
template< typename T, typename I, class A >
template <class F>
void CUtlVectorBase<T, I, A>::SortPredicate( F&& predicate )
{
	I n = Count();
	if (n < 2)
		return;

	struct StackEntry
	{
		I lo, hi;
	} stack[32]; // Sufficient stack depth for typical use (log2(2^32) = 32).

	int stackPos = 0;

	stack[stackPos++] = StackEntry{ 0, n - 1 };

	while (stackPos > 0)
	{
		StackEntry entry = stack[--stackPos];
		I lo = entry.lo;
		I hi = entry.hi;

		while (lo < hi)
		{
			I i = lo, j = hi;
			T pivot = Base()[(lo + hi) / 2];

			while (i <= j)
			{
				while (predicate(Base()[i], pivot)) ++i;
				while (predicate(pivot, Base()[j])) --j;

				if (i <= j)
				{
					if (i != j)
					{
						T temp = Base()[i];
						Base()[i] = Base()[j];
						Base()[j] = temp;
					}
					++i;
					--j;
				}
			}

			// Tail-call elimination: always recurse into the smaller partition first 
			// and loop on the larger to keep stack size small.
			if (j - lo < hi - i)
			{
				if (i < hi)
					stack[stackPos++] = StackEntry{ i, hi };
				hi = j;
			}
			else
			{
				if (lo < j)
					stack[stackPos++] = StackEntry{ lo, j };
				lo = i;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Makes sure we have enough memory allocated to store a requested # of elements
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::EnsureCapacity( I num )
{
	MEM_ALLOC_CREDIT_CLASS();
	m_Memory.EnsureCapacity(num);
	ResetDbgInfo();
}


//-----------------------------------------------------------------------------
// Makes sure we have at least this many elements
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::EnsureCount( I num )
{
	if (Count() < num)
	{
		AddMultipleToTail( num - Count() );
	}
}


//-----------------------------------------------------------------------------
// Shifts elements
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::ShiftElementsRight( I elem, I num )
{
	Assert( IsValidIndex(elem) || ( m_Size == 0 ) || ( num == 0 ));
	I numToMove = m_Size - elem - num;
	if ((numToMove > 0) && (num > 0))
		Q_memmove( &Element(elem+num), &Element(elem), numToMove * sizeof(T) );
}

template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::ShiftElementsLeft( I elem, I num )
{
	Assert( IsValidIndex(elem) || ( m_Size == 0 ) || ( num == 0 ));
	I numToMove = m_Size - elem - num;
	if ((numToMove > 0) && (num > 0))
	{
		Q_memmove( &Element(elem), &Element(elem+num), numToMove * sizeof(T) );

#ifdef _DEBUG
		Q_memset( &Element(m_Size-num), 0xDD, num * sizeof(T) );
#endif
	}
}


//-----------------------------------------------------------------------------
// Adds an element, uses default constructor
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::AddToHead()
{
	return InsertBefore(0);
}

template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::AddToTail()
{
	return InsertBefore( m_Size );
}

template< typename T, typename I, class A >
inline T *CUtlVectorBase<T, I, A>::AddToTailGetPtr()
{
	return &Element( AddToTail() );
}

template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::InsertAfter( I elem )
{
	return InsertBefore( elem + 1 );
}

template< typename T, typename I, class A >
I CUtlVectorBase<T, I, A>::InsertBefore( I elem )
{
	// Can insert at the end
	Assert( (elem == Count()) || IsValidIndex(elem) );

	GrowVector();
	ShiftElementsRight(elem);
	Construct( &Element(elem) );
	return elem;
}


//-----------------------------------------------------------------------------
// Adds an element, uses copy constructor
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::AddToHead( const T& copySrc )
{
	// Can't insert something that's in the list... reallocation may hose us
	Assert( (Base() == NULL) || (&copySrc < Base()) || (&copySrc >= (Base() + Count()) ) );
	return InsertBefore( 0, copySrc );
}

template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::AddToHead( T&& moveSrc )
{
	return InsertBefore( 0, Move(moveSrc) );
}

template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::AddToTail( const T& copySrc )
{
	// Can't insert something that's in the list... reallocation may hose us
	Assert( (Base() == NULL) || (&copySrc < Base()) || (&copySrc >= (Base() + Count()) ) );
	return InsertBefore( m_Size, copySrc );
}

template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::AddToTail( T&& moveSrc )
{
	return InsertBefore( m_Size, Move(moveSrc) );
}

template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::InsertAfter( I elem, const T& copySrc )
{
	// Can't insert something that's in the list... reallocation may hose us
	Assert( (Base() == NULL) || (&copySrc < Base()) || (&copySrc >= (Base() + Count()) ) );
	return InsertBefore( elem + 1, copySrc );
}


template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::InsertAfter( I elem, T&& moveSrc )
{
	return InsertBefore( elem + 1, Move(moveSrc) );
}

template< typename T, typename I, class A >
I CUtlVectorBase<T, I, A>::InsertBefore( I elem, const T& copySrc )
{
	// Can't insert something that's in the list... reallocation may hose us
	Assert( (Base() == NULL) || (&copySrc < Base()) || (&copySrc >= (Base() + Count()) ) ); 

	// Can insert at the end
	Assert( (elem == Count()) || IsValidIndex(elem) );

	GrowVector();
	ShiftElementsRight(elem);
	CopyConstruct( &Element(elem), copySrc );
	return elem;
}

template< typename T, typename I, class A >
I CUtlVectorBase<T, I, A>::InsertBefore( I elem, T&& moveSrc )
{
	// Can insert at the end
	Assert( (elem == Count()) || IsValidIndex(elem) );

	GrowVector();
	ShiftElementsRight(elem);
	MoveConstruct( &Element(elem), Move(moveSrc) );
	return elem;
}


//-----------------------------------------------------------------------------
// Adds multiple elements, uses default constructor
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::AddMultipleToHead( I num )
{
	return InsertMultipleBefore( 0, num );
}

template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::AddMultipleToTail( I num )
{
	return InsertMultipleBefore( m_Size, num );
}

template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::AddMultipleToTail( I num, const T *pToCopy )
{
	// Can't insert something that's in the list... reallocation may hose us
	Assert( (Base() == NULL) || !pToCopy || (pToCopy + num <= Base()) || (pToCopy >= (Base() + Count()) ) ); 

	return InsertMultipleBefore( m_Size, num, pToCopy );
}

template< typename T, typename I, class A >
I CUtlVectorBase<T, I, A>::InsertMultipleAfter( I elem, I num )
{
	return InsertMultipleBefore( elem + 1, num );
}


template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::SetCount( I count )
{
	RemoveAll();
	AddMultipleToTail( count );
}

template< typename T, typename I, class A >
inline void CUtlVectorBase<T, I, A>::SetSize( I size )
{
	SetCount( size );
}

template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::SetCountNonDestructively( I count )
{
	I delta = count - m_Size;
	if(delta > 0) AddMultipleToTail( delta );
	else if(delta < 0) RemoveMultipleFromTail( -delta );
}

template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::CopyArray( const T *pArray, I size )
{
	// Can't insert something that's in the list... reallocation may hose us
	Assert( (Base() == NULL) || !pArray || (Base() >= (pArray + size)) || (pArray >= (Base() + Count()) ) ); 

	SetSize( size );
	for( I i=0; i < size; i++ )
	{
		Element(i) = pArray[i];
	}
}

template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::Swap( CUtlVectorBase< T, I, A > &vec )
{
	m_Memory.Swap( vec.m_Memory );
	V_swap( m_Size, vec.m_Size );
#ifndef _X360
	// V_swap( m_pElements, vec.m_pElements );
#endif
}

template< typename T, typename I, class A >
I CUtlVectorBase<T, I, A>::AddVectorToTail( CUtlVectorBase< T, I, A > const &copySrc )
{
	Assert( &copySrc != this );

	I base = Count();
	
	// Make space.
	I nSrcCount = copySrc.Count();
	EnsureCapacity( base + nSrcCount );

	// Copy the elements.	
	m_Size += nSrcCount;
	for ( I i=0; i < nSrcCount; i++ )
	{
		CopyConstruct( &Element(base+i), copySrc[i] );
	}
	return base;
}

template< typename T, typename I, class A >
I CUtlVectorBase<T, I, A>::AddVectorToTail( CUtlVectorBase< T, I, A > &&moveSrc )
{
	I base = Count();
	
	// Make space.
	I nSrcCount = moveSrc.Count();
	EnsureCapacity( base + nSrcCount );

	// Move the elements.
	m_Size += nSrcCount;
	for ( I i=0; i < nSrcCount; i++ )
	{
		MoveConstruct( &Element(base+i), Move( moveSrc.MoveElement(i) ) );
	}
	return base;
}

template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::InsertMultipleBefore( I elem, I num )
{
	if( num == 0 )
		return elem;

	// Can insert at the end
	Assert( (elem == Count()) || IsValidIndex(elem) );

	GrowVector(num);
	ShiftElementsRight( elem, num );

	// Invoke default constructors
	for (I i = 0; i < num; ++i )
	{
		Construct( &Element( elem+i ) );
	}

	return elem;
}

template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::InsertMultipleBefore( I elem, I num, const T *pToInsert )
{
	if( num == 0 )
		return elem;
	
	// Can insert at the end
	Assert( (elem == Count()) || IsValidIndex(elem) );

	GrowVector(num);
	ShiftElementsRight( elem, num );

	// Invoke default constructors
	if ( !pToInsert )
	{
		for (I i = 0; i < num; ++i )
		{
			Construct( &Element( elem+i ) );
		}
	}
	else
	{
		for ( I i=0; i < num; i++ )
		{
			CopyConstruct( &Element( elem+i ), pToInsert[i] );
		}
	}

	return elem;
}


//-----------------------------------------------------------------------------
// Finds an element (element needs operator== defined)
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
I CUtlVectorBase<T, I, A>::Find( const T& compSrc ) const
{
	for ( I i = 0; i < Count(); ++i )
		if (Element(i) == compSrc)
			return i;

	return UTL_INVAL_VECTOR_ELEM;
}
template< typename T, typename I, class A >
I CUtlVectorBase<T, I, A>::Find( T&& moveSrc ) const
{
	const T comp( Move(moveSrc) );

	for ( I i = 0; i < Count(); ++i )
		if (Element(i) == comp)
			return i;

	return UTL_INVAL_VECTOR_ELEM;
}

template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::FillWithValue( const T& copySrc )
{
	for ( I i = 0; i < Count(); i++ )
	{
		Element(i) = copySrc;
	}
}

template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::FillWithValue( T&& moveSrc )
{
	const T src( Move(moveSrc) );

	for ( I i = 0; i < Count(); i++ )
	{
		Element(i) = src;
	}
}

template< typename T, typename I, class A >
bool CUtlVectorBase<T, I, A>::HasElement( const T& copySrc ) const
{
	return ( Find(copySrc) != UTL_INVAL_VECTOR_ELEM );
}


//-----------------------------------------------------------------------------
// Element removal
//-----------------------------------------------------------------------------
template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::FastRemove( I elem )
{
	Assert( IsValidIndex(elem) );

	// Global scope to resolve conflict with Scaleform 4.0
	::Destruct( &Element(elem) );
	if (m_Size > 0)
	{
		if ( elem != m_Size - 1 )
			memcpy( (void*)&Element(elem), (const void*)&Element(m_Size - 1), sizeof(T) );
		--m_Size;
	}
}

template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::Remove( I elem )
{
	// Global scope to resolve conflict with Scaleform 4.0
	::Destruct( &Element(elem) );
	ShiftElementsLeft(elem);
	--m_Size;
}

template< typename T, typename I, class A >
bool CUtlVectorBase<T, I, A>::FindAndRemove( const T& compSrc )
{
	I elem = Find( compSrc );
	if ( elem != UTL_INVAL_VECTOR_ELEM )
	{
		Remove( elem );
		return true;
	}
	return false;
}

template< typename T, typename I, class A >
bool CUtlVectorBase<T, I, A>::FindAndRemove( T&& moveSrc )
{
	I elem = Find( moveSrc );
	if ( elem != UTL_INVAL_VECTOR_ELEM )
	{
		Remove( elem );
		return true;
	}
	return false;
}

template< typename T, typename I, class A >
bool CUtlVectorBase<T, I, A>::FindAndFastRemove( const T& compSrc )
{
	I elem = Find( compSrc );
	if ( elem != UTL_INVAL_VECTOR_ELEM )
	{
		FastRemove( elem );
		return true;
	}
	return false;
}

template< typename T, typename I, class A >
bool CUtlVectorBase<T, I, A>::FindAndFastRemove( T&& moveSrc )
{
	I elem = Find( moveSrc );
	if ( elem != UTL_INVAL_VECTOR_ELEM )
	{
		FastRemove( elem );
		return true;
	}
	return false;
}

template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::RemoveMultiple( I elem, I num )
{
	Assert( elem != UTL_INVAL_VECTOR_ELEM );
	Assert( elem + num <= Count() );

	// Global scope to resolve conflict with Scaleform 4.0
	for ( I i = elem + num; i-- > elem; )
		::Destruct(&Element(i));

	ShiftElementsLeft(elem, num);
	m_Size -= num;
}

template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::RemoveMultipleFromHead( I num )
{
	Assert( num <= Count() );

	// Global scope to resolve conflict with Scaleform 4.0
	for ( I i = num; i-- > 0; )
		::Destruct(&Element(i));

	ShiftElementsLeft(0, num);
	m_Size -= num;
}

template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::RemoveMultipleFromTail( I num )
{
	Assert( num <= Count() );

	// Global scope to resolve conflict with Scaleform 4.0
	for (I i = m_Size-num; i < m_Size; i++)
		::Destruct(&Element(i));

	m_Size -= num;
}

template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::RemoveAll()
{
	for (I i = m_Size; i-- > 0; )
	{
		// Global scope to resolve conflict with Scaleform 4.0
		::Destruct(&Element(i));
	}

	m_Size = 0;
}


//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------

template< typename T, typename I, class A >
inline void CUtlVectorBase<T, I, A>::Purge()
{
	RemoveAll();
	m_Memory.Purge();
	ResetDbgInfo();
}


template< typename T, typename I, class A >
inline void CUtlVectorBase<T, I, A>::PurgeAndDeleteElements()
{
	for( I i=0; i < m_Size; i++ )
	{
		delete Element(i);
	}
	Purge();
}

template< typename T, typename I, class A >
inline void CUtlVectorBase<T, I, A>::Compact()
{
	m_Memory.Purge(m_Size);
}

template< typename T, typename I, class A >
inline I CUtlVectorBase<T, I, A>::NumAllocated() const
{
	return m_Memory.NumAllocated();
}


//-----------------------------------------------------------------------------
// Data and memory validation
//-----------------------------------------------------------------------------
#ifdef DBGFLAG_VALIDATE
template< typename T, typename I, class A >
void CUtlVectorBase<T, I, A>::Validate( CValidator &validator, char *pchName )
{
	validator.Push( typeid(*this).name(), this, pchName );

	m_Memory.Validate( validator, "m_Memory" );

	validator.Pop();
}
#endif // DBGFLAG_VALIDATE

template < class T, typename I, class A >
CUtlVector< T, I, A >::CUtlVector( const std::initializer_list< T > elements )	: 
	BaseClass( 0, elements.size() )
{
	for ( const auto& elem : elements )
	{
		this->AddToTail( elem );
	}
}

// A vector class for storing pointers, so that the elements pointed to by the pointers are deleted
// on exit.
template< class T, typename I = int, class A = CUtlMemory< T, I > >
class CUtlVectorAutoPurge : public CUtlVector< T, I, A >
{
	typedef CUtlVector< T, I, A > BaseClass;

public:
	using BaseClass::BaseClass;

	~CUtlVectorAutoPurge( void )
	{
		this->PurgeAndDeleteElements();
	}
};

// Source1 legacy: CSplitString was declared here
#include "tier0/splitstring.h"

#endif // CCVECTOR_H
