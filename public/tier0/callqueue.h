//========== Copyright © 2006, Valve Corporation, All rights reserved. ========
//
// Purpose:
//
//=============================================================================

#ifndef CALLQUEUE_H
#define CALLQUEUE_H

#include <functional>
#include "tier0/tslist.h"
#include "functors.h"
#include "jobthread.h"

#if defined( _WIN32 )
#pragma once
#endif

// Avert thy eyes! Imagine rather:
//
// void QueueCall( <function>, [args1, [arg2,]...]
// void QueueCall( <object>, <function>, [args1, [arg2,]...]
// void QueueRefCall( <object>, <<function>, [args1, [arg2,]...]
//-----------------------------------------------------

#define DEFINE_CALLQUEUE_NONMEMBER_QUEUE_CALL(N) \
	template <typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N> \
	void QueueCall(FUNCTION_RETTYPE (*pfnProxied)( FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ) FUNC_ARG_FORMAL_PARAMS_##N ) \
		{ \
		QueueFunctorInternal( CreateFunctor( pfnProxied FUNC_FUNCTOR_CALL_ARGS_##N ) ); \
		}

//-------------------------------------

#define DEFINE_CALLQUEUE_MEMBER_QUEUE_CALL(N) \
	template <typename OBJECT_TYPE_PTR, typename FUNCTION_CLASS, typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N> \
	void QueueCall(OBJECT_TYPE_PTR pObject, FUNCTION_RETTYPE ( FUNCTION_CLASS::*pfnProxied )( FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ) FUNC_ARG_FORMAL_PARAMS_##N ) \
		{ \
		QueueFunctorInternal( CreateFunctor( pObject, pfnProxied FUNC_FUNCTOR_CALL_ARGS_##N ) ); \
		}

//-------------------------------------

#define DEFINE_CALLQUEUE_CONST_MEMBER_QUEUE_CALL(N) \
	template <typename OBJECT_TYPE_PTR, typename FUNCTION_CLASS, typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N> \
	void QueueCall(OBJECT_TYPE_PTR pObject, FUNCTION_RETTYPE ( FUNCTION_CLASS::*pfnProxied )( FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ) const FUNC_ARG_FORMAL_PARAMS_##N ) \
		{ \
		QueueFunctorInternal( CreateFunctor( pObject, pfnProxied FUNC_FUNCTOR_CALL_ARGS_##N ) ); \
		}

//-------------------------------------

#define DEFINE_CALLQUEUE_REF_COUNTING_MEMBER_QUEUE_CALL(N) \
	template <typename OBJECT_TYPE_PTR, typename FUNCTION_CLASS, typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N> \
	void QueueRefCall(OBJECT_TYPE_PTR pObject, FUNCTION_RETTYPE ( FUNCTION_CLASS::*pfnProxied )( FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ) FUNC_ARG_FORMAL_PARAMS_##N ) \
		{ \
		QueueFunctorInternal( CreateRefCountingFunctor( pObject, pfnProxied FUNC_FUNCTOR_CALL_ARGS_##N ) ); \
		}

//-------------------------------------

#define DEFINE_CALLQUEUE_REF_COUNTING_CONST_MEMBER_QUEUE_CALL(N) \
	template <typename OBJECT_TYPE_PTR, typename FUNCTION_CLASS, typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N> \
	void QueueRefCall(OBJECT_TYPE_PTR pObject, FUNCTION_RETTYPE ( FUNCTION_CLASS::*pfnProxied )( FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ) const FUNC_ARG_FORMAL_PARAMS_##N ) \
		{ \
		QueueFunctorInternal( CreateRefCountingFunctor( pObject, pfnProxied FUNC_FUNCTOR_CALL_ARGS_##N ) ); \
		\
		}

#define FUNC_GENERATE_QUEUE_METHODS() \
	FUNC_GENERATE_ALL( DEFINE_CALLQUEUE_NONMEMBER_QUEUE_CALL ); \
	FUNC_GENERATE_ALL( DEFINE_CALLQUEUE_MEMBER_QUEUE_CALL ); \
	FUNC_GENERATE_ALL( DEFINE_CALLQUEUE_CONST_MEMBER_QUEUE_CALL );\
	FUNC_GENERATE_ALL( DEFINE_CALLQUEUE_REF_COUNTING_MEMBER_QUEUE_CALL ); \
	FUNC_GENERATE_ALL( DEFINE_CALLQUEUE_REF_COUNTING_CONST_MEMBER_QUEUE_CALL )

//-----------------------------------------------------

class CCallQueue
{
public:
	CCallQueue() : m_nRefCount( 0 ), m_bNoQueue( false ), m_bAbortProcessing( false ), m_bProcessing( false )
	{
	}

	~CCallQueue()
	{
		Flush();
	}

	void DisableQueue( bool bDisable )
	{
		if ( m_bNoQueue == bDisable )
		{
			return;
		}

		if ( !m_bNoQueue )
		{
			CallQueued();
		}

		m_bNoQueue = bDisable;
	}

	bool IsDisabled() const
	{
		return m_bNoQueue;
	}

	void AbortProcessing()
	{
		m_bAbortProcessing = true;
	}

	bool IsProcessing() const
	{
		return m_bProcessing;
	}

	int Count()
	{
		return m_queue.Count();
	}

	void CallQueued()
	{
		m_bProcessing = true;

		if ( m_queue.Count() )
		{
			m_queue.PushItem( NULL );

			std::function< void() > *pFunctor;

			while ( m_queue.PopItem( &pFunctor ) && pFunctor != NULL )
			{
				if ( !m_bAbortProcessing )
				{
					( *pFunctor )();
				}

				delete pFunctor;
			}
		}

		m_bAbortProcessing = false;
		m_bProcessing = false;
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
			m_bProcessing = true;

			if ( !m_bAbortProcessing )
			{
				functor();
			}

			m_bAbortProcessing = false;
			m_bProcessing = false;
		}
	}

	void QueueFunctor( CFunctor *pFunctor )
	{
		Assert( pFunctor );
		QueueFunctorInternal( RetAddRef( pFunctor ) );
	}

	void Flush()
	{
		m_queue.PushItem( NULL );

		std::function< void() > *pFunctor;

		while ( m_queue.PopItem( &pFunctor ) && pFunctor != NULL )
		{
			delete pFunctor;
		}

		m_bAbortProcessing = false;
	}

	FUNC_GENERATE_QUEUE_METHODS();

private:
	// Takes over the reference held by the caller
	void QueueFunctorInternal( CFunctor *pFunctor )
	{
		CSmartPtr< CFunctor, CRefCountAccessor > pHeldFunctor( pFunctor );

		pFunctor->Release();
		QueueCall( [pHeldFunctor]() mutable { ( *pHeldFunctor )(); } );
	}

	CTSQueue< std::function< void() > * > m_queue;
	int16 m_nRefCount;
	bool m_bNoQueue;
	bool m_bAbortProcessing;
	bool m_bProcessing;
};

#endif // CALLQUEUE_H
