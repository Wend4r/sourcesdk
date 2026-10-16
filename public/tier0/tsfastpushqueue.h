#ifndef TSFASTPUSHQUEUE_H
#define TSFASTPUSHQUEUE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/threadtools.h"

//-----------------------------------------------------------------------------
// Lock-free intrusive queue, the nodes are linked through k_LinkedListMember.
// An empty queue pushes to the loopback address, which is the pop slot itself
//-----------------------------------------------------------------------------
template< class T, class TNodeBase, class TNodeLink, TNodeLink *TNodeBase::*k_LinkedListMember >
class CTSFastPushQueue_Base
{
public:
	CTSFastPushQueue_Base() : m_pPush( GetLoopbackAddress() ), m_pPop( nullptr ) {}

	// Takes every pushed node and leaves the queue empty, waiting for the pushes that are
	// in progress; the nodes stay linked in push order, null when the queue is empty
	T *Detach()
	{
		T *pHead = static_cast< T * >( ThreadInterlockedExchangePointer( ( void *volatile * )&m_pPop, nullptr ) );

		for ( int nSpins = 0; !pHead; )
		{
			if ( m_pPush == GetLoopbackAddress() )
				return nullptr;

			if ( ++nSpins == 100 )
			{
				nSpins = 0;
				ThreadSleep( 0 );
			}

			pHead = static_cast< T * >( ThreadInterlockedExchangePointer( ( void *volatile * )&m_pPop, nullptr ) );
		}

		// The last node links to the end marker once the push that made it the last one ends
		T *pTail = static_cast< T * >( ThreadInterlockedExchangePointer( ( void *volatile * )&m_pPush, GetLoopbackAddress() ) );
		*NextSlot( pTail ) = GetEndMarker();

		T *pNode = pHead;

		for ( int nSpins = 0; ; )
		{
			T *pNext = *NextSlot( pNode );

			if ( !pNext )
			{
				if ( ++nSpins == 100 )
				{
					nSpins = 0;
					ThreadSleep( 0 );
				}

				continue;
			}

			if ( pNext == GetEndMarker() )
				break;

			pNode = pNext;
		}

		*NextSlot( pNode ) = nullptr;

		return pHead;
	}

protected:
	T *GetLoopbackAddress() { return reinterpret_cast< T * >( const_cast< T ** >( &m_pPop ) ); }
	static T *GetEndMarker() { return reinterpret_cast< T * >( 1 ); }
	static T *volatile *NextSlot( T *pNode ) { return reinterpret_cast< T *volatile * >( &( pNode->*k_LinkedListMember ) ); }

	T *volatile m_pPush;
	T *volatile m_pPop;

private:
	CTSFastPushQueue_Base( const CTSFastPushQueue_Base & ) = delete;
	CTSFastPushQueue_Base &operator=( const CTSFastPushQueue_Base & ) = delete;
};

template< class T, class TNodeBase, class TNodeLink, TNodeLink *TNodeBase::*k_LinkedListMember >
class CTSFastPushQueue : public CTSFastPushQueue_Base< T, TNodeBase, TNodeLink, k_LinkedListMember >
{
};

#endif // TSFASTPUSHQUEUE_H
