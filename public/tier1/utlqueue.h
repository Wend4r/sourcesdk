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

#include "utlleanvector.h"
#include "utlvectormemory.h"

//#define TEST_UTLQUEUE

enum QueueIter_t { QUEUE_ITERATOR_INVALID = 0xffffffff };

template< class T >
struct CUtlQueueNode_t
{
	T m_Element;
};

// T is the type stored in the queue, M holds its nodes in a ring over the whole allocation
template< class T, class M = CUtlLeanVector< CUtlQueueNode_t< T >, int > >
class CUtlQueue
{
public:

	CUtlQueue( int growSize = 0, int initSize = 0 );
	CUtlQueue( T *pMemory, int numElements );
	CUtlQueue( const CUtlQueue &copyFrom );
	CUtlQueue( CUtlQueue &&moveFrom );
	~CUtlQueue();

	CUtlQueue &operator=( const CUtlQueue &copyFrom );
	CUtlQueue &operator=( CUtlQueue &&moveFrom );

	// return the item from the front of the queue and delete it
	T RemoveAtHead();
	bool RemoveAtHead( T &removedElement );

	// return the item from the end of the queue and delete it
	T RemoveAtTail();
	bool RemoveAtTail( T &removedElement );

	// return item at the front of the queue
	T const& Head() const;
	// return item at the end of the queue
	T const& Tail() const;

	// Add a new item to the end of the queue
	void	Insert( T const &element );

	// checks if an element of this value already exists on the stack, returns true if it does
	bool		Check( T const element ) const;

	// iterators may be invalidated by Insert()
	QueueIter_t First() const;
	QueueIter_t Next( QueueIter_t it ) const;
	QueueIter_t Last() const;
	QueueIter_t Previous( QueueIter_t it ) const;
	bool IsValid( QueueIter_t it ) const;
	T const& Element( QueueIter_t it ) const;

	// Returns the count of elements in the queue
	int			Count() const;

	// Return whether the queue is empty or not, faster than Count().
	bool		IsEmpty() const;

	// doesn't deallocate memory
	void		RemoveAll();

	// Memory deallocation
	void		Purge();

protected:
	QueueIter_t Next_Unchecked( QueueIter_t it ) const;
	QueueIter_t Previous_Unchecked( QueueIter_t it ) const;

	T &Node( QueueIter_t it ) { return m_memory.Base()[ it ].m_Element; }
	T const &Node( QueueIter_t it ) const { return m_memory.Base()[ it ].m_Element; }

	M					m_memory;

	// if m_head == m_tail == QUEUE_ITERATOR_INVALID, then the queue is empty
	QueueIter_t			m_head;
	QueueIter_t			m_tail;

#ifdef TEST_UTLQUEUE
	friend void CUtlQueue_Test();
#endif
};

//-----------------------------------------------------------------------------
// The CUtlQueueFixed class:
// A queue class with a fixed allocation scheme
//-----------------------------------------------------------------------------
template< class T, size_t MAX_SIZE >
class CUtlQueueFixed : public CUtlQueue< T, CUtlVectorMemory_Fixed< CUtlQueueNode_t< T >, MAX_SIZE > >
{
	typedef CUtlQueue< T, CUtlVectorMemory_Fixed< CUtlQueueNode_t< T >, MAX_SIZE > > BaseClass;
public:

	// constructor, destructor
	CUtlQueueFixed( int growSize = 0, int initSize = 0 ) : BaseClass( growSize, initSize ) {}
	CUtlQueueFixed( T* pMemory, int numElements ) : BaseClass( pMemory, numElements ) {}
};

template< class T, class M >
inline CUtlQueue<T, M>::CUtlQueue( int growSize, int initSize ) :
	m_memory( growSize, initSize ), m_head( QUEUE_ITERATOR_INVALID ), m_tail( QUEUE_ITERATOR_INVALID )
{
}

template< class T, class M >
inline CUtlQueue<T, M>::CUtlQueue( T *pMemory, int numElements ) : 
	m_memory( reinterpret_cast< CUtlQueueNode_t< T > * >( pMemory ), numElements ), m_head( QUEUE_ITERATOR_INVALID ), m_tail( QUEUE_ITERATOR_INVALID )
{
}

template< class T, class M >
inline CUtlQueue<T, M>::CUtlQueue( const CUtlQueue &copyFrom ) :
	m_head( QUEUE_ITERATOR_INVALID ), m_tail( QUEUE_ITERATOR_INVALID )
{
	*this = copyFrom;
}

template< class T, class M >
inline CUtlQueue<T, M>::CUtlQueue( CUtlQueue &&moveFrom ) :
	m_memory( Move( moveFrom.m_memory ) ), m_head( moveFrom.m_head ), m_tail( moveFrom.m_tail )
{
	moveFrom.m_head = moveFrom.m_tail = QUEUE_ITERATOR_INVALID;
}

template< class T, class M >
inline CUtlQueue<T, M>::~CUtlQueue()
{
	RemoveAll();
}

template< class T, class M >
inline CUtlQueue<T, M> &CUtlQueue<T, M>::operator=( const CUtlQueue &copyFrom )
{
	if ( this != &copyFrom )
	{
		RemoveAll();

		for ( QueueIter_t it = copyFrom.First(); it != QUEUE_ITERATOR_INVALID; it = copyFrom.Next( it ) )
		{
			Insert( copyFrom.Element( it ) );
		}
	}

	return *this;
}

template< class T, class M >
inline CUtlQueue<T, M> &CUtlQueue<T, M>::operator=( CUtlQueue &&moveFrom )
{
	if ( this != &moveFrom )
	{
		Purge();

		m_memory = Move( moveFrom.m_memory );
		m_head = moveFrom.m_head;
		m_tail = moveFrom.m_tail;
		moveFrom.m_head = moveFrom.m_tail = QUEUE_ITERATOR_INVALID;
	}

	return *this;
}

template <class T, class M>
inline T CUtlQueue<T, M>::RemoveAtHead()
{
	T temp;
	Verify( RemoveAtHead( temp ) );
	return temp;
}

template <class T, class M>
inline bool CUtlQueue<T, M>::RemoveAtHead( T &removedElement )
{
	Assert( m_head != QUEUE_ITERATOR_INVALID );
	if ( m_head == QUEUE_ITERATOR_INVALID )
	{
		Construct( &removedElement );
		return false;
	}

	QueueIter_t it = m_head;
	removedElement = Node( it );
	Destruct( &Node( it ) );
	if ( m_head == m_tail )
	{
		m_head = m_tail = QUEUE_ITERATOR_INVALID;
	}
	else
	{
		m_head = Next_Unchecked( m_head );
	}
	return true;
}

template <class T, class M>
inline T CUtlQueue<T, M>::RemoveAtTail()
{
	T temp;
	RemoveAtTail( temp );
	return temp;
}

template <class T, class M>
inline bool CUtlQueue<T, M>::RemoveAtTail( T &removedElement )
{
	Assert( m_tail != QUEUE_ITERATOR_INVALID );
	if ( m_tail == QUEUE_ITERATOR_INVALID )
	{
		Construct( &removedElement );
		return false;
	}

	removedElement = Node( m_tail );
	Destruct( &Node( m_tail ) );
	if ( m_head == m_tail )
	{
		m_head = m_tail = QUEUE_ITERATOR_INVALID;
	}
	else
	{
		m_tail = Previous_Unchecked( m_tail );
	}
	return true;
}

template <class T, class M>
inline T const& CUtlQueue<T, M>::Head() const
{
	Assert( m_head != QUEUE_ITERATOR_INVALID );
	if ( m_head == QUEUE_ITERATOR_INVALID )
	{
		static T dummy;
		return dummy;
	}

	return Node( m_head );
}

template <class T, class M>
inline T const& CUtlQueue<T, M>::Tail() const
{
	Assert( m_tail != QUEUE_ITERATOR_INVALID );
	if ( m_tail == QUEUE_ITERATOR_INVALID )
	{
		static T dummy;
		return dummy;
	}

	return Node( m_tail );
}

template <class T, class M>
void CUtlQueue<T, M>::Insert( T const &element )
{
	if ( m_tail == QUEUE_ITERATOR_INVALID )
	{
		// empty
		m_memory.EnsureCapacity( 1 );
		m_head = m_tail = QueueIter_t( 0 );
	}
	else
	{
		// non-empty
		QueueIter_t nextTail = Next_Unchecked( m_tail );
		if ( nextTail == m_head ) // if non-empty, and growing by 1 appears to make the queue of length 1, then we were already full before the Insert
		{
			int nOldAllocCount = m_memory.NumAllocated();
			m_memory.EnsureCapacity( nOldAllocCount + 1 );
			int nNewAllocCount = m_memory.NumAllocated();
			int nGrowAmount = nNewAllocCount - nOldAllocCount;

			nextTail = Next_Unchecked( m_tail ); // if nextTail was 0, then it now should be nOldAllocCount

			if ( m_head != QueueIter_t( 0 ) )
			{
				// if the queue wraps around the end of m_memory, move the part at the end of memory to the new end of memory
				Q_memmove( ( void * )&Node( QueueIter_t( m_head + nGrowAmount ) ), ( void * )&Node( m_head ), ( nOldAllocCount - m_head ) * sizeof( CUtlQueueNode_t< T > ) );
#ifdef _DEBUG
				Q_memset( ( void * )&Node( m_head ), 0xdd, nGrowAmount * sizeof( CUtlQueueNode_t< T > ) );
#endif
				m_head = QueueIter_t( m_head + nGrowAmount );
			}
		}
		m_tail = nextTail;
	}

	CopyConstruct( &Node( m_tail ), element );
}

template <class T, class M>
bool CUtlQueue<T, M>::Check( T const element ) const
{
	for ( QueueIter_t it = First(); it != QUEUE_ITERATOR_INVALID; it = Next( it ) )
	{
		if ( Node( it ) == element )
			return true;
	}
	return false;
}

template <class T, class M>
QueueIter_t CUtlQueue<T, M>::First() const
{
	return m_head;
}

template <class T, class M>
QueueIter_t CUtlQueue<T, M>::Next( QueueIter_t it ) const
{
	if ( it == QUEUE_ITERATOR_INVALID )
		return QUEUE_ITERATOR_INVALID;

	if ( it == m_tail )
		return QUEUE_ITERATOR_INVALID;

	Assert( IsValid( it ) );
	if ( !IsValid( it ) )
		return QUEUE_ITERATOR_INVALID;

	return Next_Unchecked( it );
}

template <class T, class M>
QueueIter_t CUtlQueue<T, M>::Last() const
{
	return m_tail;
}

template <class T, class M>
QueueIter_t CUtlQueue<T, M>::Previous( QueueIter_t it ) const
{
	if ( it == QUEUE_ITERATOR_INVALID )
		return QUEUE_ITERATOR_INVALID;

	if ( it == m_head )
		return QUEUE_ITERATOR_INVALID;

	Assert( IsValid( it ) );
	if ( !IsValid( it ) )
		return QUEUE_ITERATOR_INVALID;

	return Previous_Unchecked( it );
}

template <class T, class M>
QueueIter_t CUtlQueue<T, M>::Next_Unchecked( QueueIter_t it ) const
{
	return ( int )it == m_memory.NumAllocated() - 1 ? QueueIter_t( 0 ) : QueueIter_t( it + 1 );
}

template <class T, class M>
QueueIter_t CUtlQueue<T, M>::Previous_Unchecked( QueueIter_t it ) const
{
	return it == 0 ? QueueIter_t( m_memory.NumAllocated() - 1 ) : QueueIter_t( it - 1 );
}

template <class T, class M>
bool CUtlQueue<T, M>::IsValid( QueueIter_t it ) const
{
	if ( it == QUEUE_ITERATOR_INVALID )
		return false;

	if ( m_head == QUEUE_ITERATOR_INVALID )
		return false;

	if ( m_head <= m_tail )
		return it >= m_head && it <= m_tail;

	return ( it >= m_head && ( int )it < m_memory.NumAllocated() ) || ( it >= 0 && it <= m_tail );
}

template <class T, class M>
T const& CUtlQueue<T, M>::Element( QueueIter_t it ) const
{
	Assert( it != QUEUE_ITERATOR_INVALID );
	if ( it == QUEUE_ITERATOR_INVALID )
	{
		static T dummy;
		return dummy;
	}

	Assert( IsValid( it ) );
	return Node( it );
}

template <class T, class M>
int CUtlQueue<T, M>::Count() const
{
	if ( m_head == QUEUE_ITERATOR_INVALID )
	{
		Assert( m_tail == QUEUE_ITERATOR_INVALID );
		return 0;
	}
	Assert( m_tail != QUEUE_ITERATOR_INVALID );

	if ( m_head <= m_tail )
		return m_tail + 1 - m_head;

	return m_tail + 1 - m_head + m_memory.NumAllocated();
}

template <class T, class M>
bool CUtlQueue<T, M>::IsEmpty() const
{
	Assert( ( m_head == QUEUE_ITERATOR_INVALID ) == ( m_tail == QUEUE_ITERATOR_INVALID ) );
	return ( m_head == QUEUE_ITERATOR_INVALID );
}

template <class T, class M>
void CUtlQueue<T, M>::RemoveAll()
{
	if ( m_head != QUEUE_ITERATOR_INVALID )
	{
		for ( QueueIter_t it = m_head; ; it = Next_Unchecked( it ) )
		{
			Destruct( &Node( it ) );
			if ( it == m_tail )
				break;
		}
	}

	m_head = m_tail = QUEUE_ITERATOR_INVALID;
}

template <class T, class M>
void CUtlQueue<T, M>::Purge()
{
	RemoveAll();
	m_memory.Purge();
}


#ifdef TEST_UTLQUEUE

#include <stdlib.h>

struct Data_t
{
	Data_t( int i = 0xffffffff ) : m_id( i ) {}
	Data_t( const Data_t &that ) : m_id( that.m_id ) {}
	~Data_t() { m_id = 0xdddddddd; }
	Data_t &operator=( const Data_t &that ) { m_id = that.m_id; return *this; }

	int m_id;
};

inline void CUtlQueue_Test()
{
	CUtlQueue< Data_t > queue;

	for ( int n = 1; n < 100; ++n )
	{
		Assert( queue.Count() == 0 );
		Assert( queue.m_head == QUEUE_ITERATOR_INVALID );
		Assert( queue.m_tail == QUEUE_ITERATOR_INVALID );

		int w = rand() % n;
		for ( int i = 0; i < w; ++i )
		{
			queue.Insert( Data_t( i ) );
		}

		if ( w > 0 )
		{
			Assert( queue.Head().m_id == queue.First() );
			Assert( queue.Tail().m_id == queue.Last() );
			Assert( queue.Head().m_id == 0 );
			Assert( queue.Tail().m_id == w - 1 );
		}
		Assert( queue.Count() == w );

		for ( int j = 0; j < n; ++j )
		{
			queue.Insert( Data_t( w + j ) );

			if ( j == 0 )
			{
				Assert( queue.Count() == w + j + 1 );

				for ( int i = 0; i < w; ++i )
				{
					queue.RemoveAtHead();
				}
			}

			Assert( queue.Count() == j + 1 );

			Assert( queue.m_head != QUEUE_ITERATOR_INVALID );
			Assert( queue.m_tail != QUEUE_ITERATOR_INVALID );

			int id = queue.Head().m_id % queue.m_memory.NumAllocated();
			for ( QueueIter_t it = queue.First(); it != QUEUE_ITERATOR_INVALID; it = queue.Next( it ) )
			{
				Assert( queue.Element( it ).m_id % queue.m_memory.NumAllocated() == id );
				id = ( id + 1 ) % queue.m_memory.NumAllocated();
			}

			id = queue.Tail().m_id % queue.m_memory.NumAllocated();
			for ( QueueIter_t it = queue.Last(); it != QUEUE_ITERATOR_INVALID; it = queue.Previous( it ) )
			{
				Assert( queue.Element( it ).m_id % queue.m_memory.NumAllocated() == id );
				id = ( id + queue.m_memory.NumAllocated() - 1 ) % queue.m_memory.NumAllocated();
			}

			for ( int i = 0; i < j; ++i )
			{
				int id = queue.m_memory.Base()[ i ].m_Element.m_id;
				if ( queue.IsValid( QueueIter_t( i ) ) )
				{
					Assert( ( id & 0xff000000 ) == 0 );
				}
				else
				{
					Assert( id == 0xdddddddd );
				}
			}
		}

		Assert( queue.Count() == n );
#if 0
		for ( int j = 0; j < n; ++j )
		{
			Assert( queue.m_head != QUEUE_ITERATOR_INVALID );
			Assert( queue.m_tail != QUEUE_ITERATOR_INVALID );

			Assert( queue.Count() == n - j );

			Data_t data = queue.RemoveAtHead();

			Assert( queue.Count() == n - j - 1 );

			if ( queue.Count() > 0 )
			{
				int id = queue.Head().m_id % queue.m_memory.NumAllocated();
				for ( QueueIter_t it = queue.First(); it != QUEUE_ITERATOR_INVALID; it = queue.Next( it ) )
				{
					Assert( queue.Element( it ).m_id % queue.m_memory.NumAllocated() == id );
					id = ( id + 1 ) % queue.m_memory.NumAllocated();
				}

				id = queue.Tail().m_id % queue.m_memory.NumAllocated();
				for ( QueueIter_t it = queue.Last(); it != QUEUE_ITERATOR_INVALID; it = queue.Previous( it ) )
				{
					Assert( queue.Element( it ).m_id % queue.m_memory.NumAllocated() == id );
					id = ( id + queue.m_memory.NumAllocated() - 1 ) % queue.m_memory.NumAllocated();
				}
			}

			for ( int i = 0; i < j; ++i )
			{
				int id = queue.m_memory.Base()[ i ].m_Element.m_id;
				if ( queue.IsValid( QueueIter_t( i ) ) )
				{
					Assert( ( id & 0xff000000 ) == 0 );
				}
				else
				{
					Assert( id == 0xdddddddd );
				}
			}
		}
#else
		for ( int j = n - 1; j >= 0; --j )
		{
			Assert( queue.m_head != QUEUE_ITERATOR_INVALID );
			Assert( queue.m_tail != QUEUE_ITERATOR_INVALID );

			Assert( queue.Count() == j + 1 );

			Data_t data = queue.RemoveAtTail();

			Assert( queue.Count() == j );

			if ( queue.Count() > 0 )
			{
				int id = queue.Head().m_id % queue.m_memory.NumAllocated();
				for ( QueueIter_t it = queue.First(); it != QUEUE_ITERATOR_INVALID; it = queue.Next( it ) )
				{
					Assert( queue.Element( it ).m_id % queue.m_memory.NumAllocated() == id );
					id = ( id + 1 ) % queue.m_memory.NumAllocated();
				}

				id = queue.Tail().m_id % queue.m_memory.NumAllocated();
				for ( QueueIter_t it = queue.Last(); it != QUEUE_ITERATOR_INVALID; it = queue.Previous( it ) )
				{
					Assert( queue.Element( it ).m_id % queue.m_memory.NumAllocated() == id );
					id = ( id + queue.m_memory.NumAllocated() - 1 ) % queue.m_memory.NumAllocated();
				}
			}

			for ( int i = 0; i < j; ++i )
			{
				int id = queue.m_memory.Base()[ i ].m_Element.m_id;
				if ( queue.IsValid( QueueIter_t( i ) ) )
				{
					Assert( ( id & 0xff000000 ) == 0 );
				}
				else
				{
					Assert( id == 0xdddddddd );
				}
			}
		}
#endif

		Assert( queue.Count() == 0 );
		Assert( queue.m_head == QUEUE_ITERATOR_INVALID );
		Assert( queue.m_tail == QUEUE_ITERATOR_INVALID );
	}
}

#endif // TEST_UTLQUEUE

#endif // UTLQUEUE_H
