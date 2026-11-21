//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A list several threads append to at once, flushed by one thread.
//
//=============================================================================//

#ifndef UTLCONCURRENTAPPENDVECTOR_H
#define UTLCONCURRENTAPPENDVECTOR_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/threadtools.h"
#include "tier0/utlleanvector.h"

//-----------------------------------------------------------------------------
// No source names this template; it is named by what the code does. CS2 uses it in
// CRenderGameSystem (changed scene object updaters), CPhysicsGameSystem (kinematic updates,
// hierarchy joint updates, dirty scales) and client globals. 256 bytes, aligned to 128.
//
// An appender takes an index with an interlocked increment of m_nNextElement and writes into
// m_Elements, which the owner grows before the concurrent phase; past its count the element goes
// to m_Overflow under m_OverflowMutex. The flush appends m_Overflow to m_Elements, handles
// m_nNextElement elements, resizes m_Elements for twice that many and resets m_nNextElement.
//-----------------------------------------------------------------------------
template < class T >
class alignas( 128 ) CUtlConcurrentAppendVector
{
public:
	CUtlLeanVector< T > m_Elements;
	CUtlLeanVector< T > m_Overflow;
	CThreadFastMutex m_OverflowMutex;
	// On a cache line of its own
	alignas( 128 ) CInterlockedInt m_nNextElement;
	// m_nNextElement when the concurrent phase began, -1 outside it
	int m_nNextElementAtBegin;
};

#endif // UTLCONCURRENTAPPENDVECTOR_H
