#ifndef TSFASTPUSHQUEUE_H
#define TSFASTPUSHQUEUE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// Lock-free intrusive queue, the nodes are linked through k_LinkedListMember.
// An empty queue pushes to the loopback address, which is the pop slot itself
//-----------------------------------------------------------------------------
template< class T, class TNodeBase, class TNodeLink, TNodeLink *TNodeBase::*k_LinkedListMember >
class CTSFastPushQueue_Base
{
public:
	CTSFastPushQueue_Base() : m_pPush( GetLoopbackAddress() ), m_pPop( nullptr ) {}

protected:
	T *GetLoopbackAddress() { return reinterpret_cast< T * >( const_cast< T ** >( &m_pPop ) ); }

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
