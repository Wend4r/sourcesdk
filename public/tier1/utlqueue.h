//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef UTLQUEUE_H
#define UTLQUEUE_H
#ifdef _WIN32
#pragma once
#endif

#include "utlvector.h"

// T is the type stored in the queue
template< class T, typename I = int, class M = CUtlMemory< T, I > > 
class CUtlQueue
{
public:

	// constructor: lessfunc is required, but may be set after the constructor with
	// SetLessFunc
	CUtlQueue( I growSize = 0, I initSize = 0 );
	CUtlQueue( T *pMemory, I numElements );

	// element access
	T& operator[]( I i );
	T const& operator[]( I i ) const;
	T& Element( I i );
	T const& Element( I i ) const;

	// return the item from the front of the queue and delete it
	T const& RemoveAtHead();
	// return the item from the end of the queue and delete it
	T const& RemoveAtTail();

	// return item at the front of the queue
	T const& Head();
	// return item at the end of the queue
	T const& Tail();

	// put a new item on the queue to the tail.
	void	Insert( T const &element );

	// checks if an element of this value already exists on the queue, returns true if it does
	bool		Check( T const element );

	// Returns the count of elements in the queue
	I			Count() const { return m_heap.Count(); }
	
	// Is element index valid?
	bool IsIdxValid( I i ) const;

	// doesn't deallocate memory
	void		RemoveAll() { m_heap.RemoveAll(); }

	// Memory deallocation
	void		Purge() { m_heap.Purge(); }

protected:
	CUtlVector<T, I, M>	m_heap;
	T					m_current;
};

//-----------------------------------------------------------------------------
// The CUtlQueueFixed class:
// A queue class with a fixed allocation scheme
//-----------------------------------------------------------------------------
template< class T, typename I, size_t MAX_SIZE >
class CUtlQueueFixed : public CUtlQueue< T, size_t, CUtlMemoryFixed<T, MAX_SIZE> >
{
	typedef CUtlQueue< T, size_t, CUtlMemoryFixed<T, MAX_SIZE> > BaseClass;
public:

	// constructor, destructor
	CUtlQueueFixed( size_t growSize = 0, size_t initSize = 0 ) : BaseClass( growSize, initSize ) {}
	CUtlQueueFixed( T* pMemory, size_t numElements ) : BaseClass( pMemory, numElements ) {}
};

template< class T, typename I, class M >
inline CUtlQueue<T, I, M>::CUtlQueue( I growSize, I initSize ) :
	m_heap(growSize, initSize)
{
}

template< class T, typename I, class M >
inline CUtlQueue<T, I, M>::CUtlQueue( T *pMemory, I numElements )	: 
	m_heap(pMemory, numElements)
{
}

//-----------------------------------------------------------------------------
// element access
//-----------------------------------------------------------------------------

template< class T, typename I, class M >
inline T& CUtlQueue<T,I,M>::operator[]( I i )
{
	return m_heap[i];
}

template< class T, typename I, class M >
inline T const& CUtlQueue<T,I,M>::operator[]( I i ) const
{
	return m_heap[i];
}

template< class T, typename I, class M >
inline T& CUtlQueue<T,I,M>::Element( I i )
{
	return m_heap[i];
}

template< class T, typename I, class M >
inline T const& CUtlQueue<T,I,M>::Element( I i ) const
{
	return m_heap[i];
}

//-----------------------------------------------------------------------------
// Is element index valid?
//-----------------------------------------------------------------------------

template< class T, typename I, class M >
inline bool CUtlQueue<T,I,M>::IsIdxValid( I i ) const
{
	return (i >= 0) && (i < m_heap.Count());
}

template< class T, typename I, class M >
inline T const& CUtlQueue<T, I, M>::RemoveAtHead()
{
	m_current = m_heap[0];
	m_heap.Remove((I)0);
	return m_current; 
}

template< class T, typename I, class M >
inline T const& CUtlQueue<T, I, M>::RemoveAtTail()
{
	m_current = m_heap[ m_heap.Count() - 1 ];
	m_heap.Remove((I)(m_heap.Count() - 1));
	return m_current; 
}

template< class T, typename I, class M >
inline T const& CUtlQueue<T, I, M>::Head()
{
	m_current = m_heap[0];
	return m_current; 
}

template< class T, typename I, class M >
inline T const& CUtlQueue<T, I, M>::Tail()
{
	m_current = m_heap[ m_heap.Count() - 1 ];
	return m_current; 
}

template< class T, typename I, class M >
void CUtlQueue<T, I, M>::Insert( T const &element )
{
	I index = m_heap.AddToTail();
	m_heap[index] = element;
}

template< class T, typename I, class M >
bool CUtlQueue<T, I, M>::Check( T const element )
{
	I index = m_heap.Find(element);
	return ( index != -1 );
}


#endif // UTLQUEUE_H
