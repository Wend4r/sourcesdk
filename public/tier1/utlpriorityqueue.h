//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef UTLPRIORITYQUEUE_H
#define UTLPRIORITYQUEUE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier1/utlvector.h"

template < typename T >
class CDefUtlPriorityQueueLessFunc
{
public:
	bool operator()( const T &lhs, const T &rhs, bool (*lessFuncPtr)( T const&, T const& ) )	
	{ 
		return lessFuncPtr( lhs, rhs );	
	}
};

template < typename T, typename I = int >
class CDefUtlPriorityQueueSetIndexFunc
{
public:
	inline static void SetIndex( T &heapElement, I nNewIndex ) { }
};

// T is the type stored in the queue, it must include the priority
// The head of the list contains the element with GREATEST priority
// configure the LessFunc_t to get the desired queue order
template< class T, typename I = int, class LessFunc = CDefUtlPriorityQueueLessFunc< T >, class A = CUtlLeanVector<T>, class SetIndexFunc = CDefUtlPriorityQueueSetIndexFunc< T > > 
class CUtlPriorityQueue
{
public:
	// Less func typedef
	// Returns true if the first parameter is "less priority" than the second
	// Items that are "less priority" sort toward the tail of the queue
	typedef bool (*LessFunc_t)( T const&, T const& );

	typedef T ElemType_t;

	// constructor: lessfunc is required, but may be set after the constructor with
	// SetLessFunc
	CUtlPriorityQueue( I growSize = 0, I initSize = 0, LessFunc_t lessfunc = 0 );
	CUtlPriorityQueue( T *pMemory, I numElements, LessFunc_t lessfunc = 0 );

	// gets particular elements
	inline T const&	ElementAtHead() const { return m_heap.Element(0); }

	inline bool IsValidIndex(I index) { return m_heap.IsValidIndex(index); }

	// O(lgn) to rebalance the heap
	void		RemoveAtHead();
	void		RemoveAt( I index );

	// Update the position of the specified element in the tree for it current value O(lgn)
	void		RevaluateElement( const I index ); 
	
	// O(lgn) to rebalance heap
	void		Insert( T const &element );
	// Sets the less func
	void		SetLessFunc( LessFunc_t func );

	// Returns the count of elements in the queue
	inline I	Count() const { return m_heap.Count(); }
	
	// doesn't deallocate memory
	void		RemoveAll() { m_heap.RemoveAll(); }

	// Memory deallocation
	void		Purge() { m_heap.Purge(); }

	inline const T &	Element( I index ) const { return m_heap.Element(index); }
	inline T &	Element( I index ) { return m_heap.Element(index); }

	bool		IsHeapified();
protected:
	CUtlVector<T, I, A>	m_heap;

	void		Swap( I index1, I index2 );
	I			PercolateDown( I nIndex );
	I			PercolateUp( I nIndex );

	// Used for sorting.
	LessFunc_t m_LessFunc;
};

template< class T, class LessFunc, typename I, class A, class SetIndexFunc >
inline CUtlPriorityQueue<T, typename I, LessFunc, A, SetIndexFunc >::CUtlPriorityQueue( I growSize, I initSize, LessFunc_t lessfunc ) :
	m_heap(growSize, initSize), m_LessFunc(lessfunc)
{
}

template< class T, class LessFunc, typename I, class A, class SetIndexFunc >
inline CUtlPriorityQueue<T, typename I, LessFunc, A, SetIndexFunc >::CUtlPriorityQueue( T *pMemory, I allocationCount, LessFunc_t lessfunc )	: 
	m_heap(pMemory, allocationCount), m_LessFunc(lessfunc)
{
}

template< class T, class LessFunc, typename I, class A, class SetIndexFunc >
inline void CUtlPriorityQueue<T, typename I, LessFunc, A, SetIndexFunc >::RemoveAtHead()
{
	m_heap.FastRemove( 0 );

	if ( Count() > 0 )
	{
		SetIndexFunc::SetIndex( m_heap[ 0 ], 0 );
	}
	
	PercolateDown( 0 );
}

template< class T, class LessFunc, typename I, class A, class SetIndexFunc >
inline void CUtlPriorityQueue<T, typename I, LessFunc, A, SetIndexFunc >::RemoveAt( I index )
{
	Assert(m_heap.IsValidIndex(index));
	m_heap.FastRemove( index );		
	
	if ( index < Count() )
	{
		SetIndexFunc::SetIndex( m_heap[ index ], index );
	}

	RevaluateElement( index );
}

template< class T, class LessFunc, typename I, class A, class SetIndexFunc >
inline void CUtlPriorityQueue<T, typename I, LessFunc, A, SetIndexFunc >::RevaluateElement( const I nStartingIndex )
{	
	I index = PercolateDown( nStartingIndex );

	// If index is still the same as the starting index, then the specified element was larger than 
	// its children, so it could be larger than its parent, so treat this like an insertion and swap
	// the node with its parent until it is no longer larger than its parent.
	if ( index == nStartingIndex )
	{
		PercolateUp( index );
	}
}

template< class T, class LessFunc, typename I, class A, class SetIndexFunc >
inline bool CUtlPriorityQueue<T, typename I, LessFunc, A, SetIndexFunc >::IsHeapified()
{
	LessFunc lessFunc;
	for ( I child = Count(); child-- > 1; ) // no need to check the element [0] , it's the parent of all and has no parent itself
	{
		I parent = ( ( child + 1 ) / 2 ) - 1;
		if ( lessFunc( m_heap[ parent ], m_heap[ child ], m_LessFunc ) )
		{
			return false; // this priority queue is not properly heapified, needs reordering
		}
	}
	return true; // the priority queue is heapified correctly, needs no reordering
}

template< class T, class LessFunc, typename I, class A, class SetIndexFunc >
inline I CUtlPriorityQueue<T, typename I, LessFunc, A, SetIndexFunc >::PercolateDown( I index )
{
	I count = Count();
	
	LessFunc lessFunc;
	I half = count/2;
	I larger = index;
	while ( index < half )
	{
		I child = ((index+1) * 2) - 1;	// if we wasted an element, this math would be more compact (1 based array)
		if ( child < count )
		{
			// Item has been filtered down to its proper place, terminate.
			if ( lessFunc( m_heap[index], m_heap[child], m_LessFunc ) )
			{
				// mark the potential swap and check the other child
				larger = child;
			}
		}
		// go to sibling
		child++;
		if ( child < count )
		{
			// If this child is larger, swap it instead
			if ( lessFunc( m_heap[larger], m_heap[child], m_LessFunc ) )
				larger = child;
		}
		
		if ( larger == index )
			break;

		// swap with the larger child
		Swap( index, larger );
		index = larger;
	}

	return index;
}

template< class T, class LessFunc, typename I, class A, class SetIndexFunc >
inline I CUtlPriorityQueue<T, typename I, LessFunc, A, SetIndexFunc >::PercolateUp( I index )
{
	if ( index >= Count() )
		return index;

	LessFunc lessFunc;
	while ( index != 0 )
	{
		I parent = ((index+1) / 2) - 1;
		if ( lessFunc( m_heap[index], m_heap[parent], m_LessFunc ) )
			break;

		// swap with parent and repeat
		Swap( parent, index );
		index = parent;
	}

	return index;
}

template< class T, class LessFunc, typename I, class A, class SetIndexFunc >
inline void CUtlPriorityQueue<T, typename I, LessFunc, A, SetIndexFunc >::Insert( T const &element )
{
	I index = m_heap.AddToTail();
	m_heap[index] = element;	
	SetIndexFunc::SetIndex( m_heap[ index ], index );

	PercolateUp( index );
}

template< class T, class LessFunc, typename I, class A, class SetIndexFunc >
inline void CUtlPriorityQueue<T, typename I, LessFunc, A, SetIndexFunc >::Swap( I index1, I index2 )
{
	T tmp = m_heap[index1];
	m_heap[index1] = m_heap[index2];
	m_heap[index2] = tmp;

	SetIndexFunc::SetIndex( m_heap[ index1 ], index1 );
	SetIndexFunc::SetIndex( m_heap[ index2 ], index2 );
}

template< class T, class LessFunc, typename I, class A, class SetIndexFunc >
inline void CUtlPriorityQueue<T, typename I, LessFunc, A, SetIndexFunc >::SetLessFunc( LessFunc_t lessfunc )
{
	m_LessFunc = lessfunc;
}

#endif // UTLPRIORITYQUEUE_H
