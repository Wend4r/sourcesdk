//========== Copyright © 2006, Valve Corporation, All rights reserved. ========
//
// Purpose:
//
//=============================================================================

#ifndef CALLQUEUE_H
#define CALLQUEUE_H

#include <functional>
#include "tier0/tslist.h"
#include "jobthread.h"

#if defined( _WIN32 )
#pragma once
#endif

//-----------------------------------------------------

class CCallQueue
{
public:
	CCallQueue() : m_bUnk0040( false ), m_bUnk0041( false ), m_bNoQueue( false ), m_bSkipCalls( false ), m_bRunningCalls( false )
	{
	}

	~CCallQueue()
	{
		Flush();
	}

	int Count()
	{
		return m_queue.Count();
	}

	void CallQueued()
	{
		m_bRunningCalls = true;

		if ( m_queue.Count() )
		{
			m_queue.PushItem( NULL );

			std::function< void() > *pFunctor;

			while ( m_queue.PopItem( &pFunctor ) && pFunctor != NULL )
			{
				if ( !m_bSkipCalls )
				{
					( *pFunctor )();
				}

				delete pFunctor;
			}
		}

		m_bSkipCalls = false;
		m_bRunningCalls = false;
	}

	DLL_CLASS_IMPORT void ParallelCallQueued( const char *pszName, IThreadPool *pPool = NULL );

	template < typename FUNCTOR >
	void QueueCall( FUNCTOR &&functor )
	{
		if ( !m_bNoQueue )
		{
			m_queue.PushItem( new std::function< void() >( std::forward< FUNCTOR >( functor ) ) );
		}
		else
		{
			m_bRunningCalls = true;

			if ( !m_bSkipCalls )
			{
				functor();
			}

			m_bSkipCalls = false;
			m_bRunningCalls = false;
		}
	}

	void Flush()
	{
		m_queue.PushItem( NULL );

		std::function< void() > *pFunctor;

		while ( m_queue.PopItem( &pFunctor ) && pFunctor != NULL )
		{
			delete pFunctor;
		}

		m_bSkipCalls = false;
	}

private:
	CTSQueue< std::function< void() > * > m_queue;
	bool m_bUnk0040;
	bool m_bUnk0041;
	bool m_bNoQueue;
	bool m_bSkipCalls;
	bool m_bRunningCalls;
};

#endif // CALLQUEUE_H
