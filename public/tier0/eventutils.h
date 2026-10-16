#ifndef EVENTUTILS_H
#define EVENTUTILS_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/refcount.h"
#include "tier0/threadtools.h"

//-----------------------------------------------------------------------------
// Pooled, reference counted manual-reset event
//-----------------------------------------------------------------------------
class COneTimeEvent : public IRefCounted
{
public:
	DLL_CLASS_IMPORT static COneTimeEvent *Allocate();

	virtual int AddRef() { return ++m_nRefCount; }

	// The last reference returns the event to the pool of tier0, which is only reachable
	// through the vtable of the events Allocate hands out
	virtual int Release();

	void Set() { m_Event.Set(); }
	bool Check() { return m_Event.Check(); }
	bool Wait( uint32 nTimeout = TT_INFINITE ) { return m_Event.Wait( nTimeout ); }

private:
	COneTimeEvent();
	virtual ~COneTimeEvent() {}

	CThreadEvent m_Event;
	CInterlockedInt m_nRefCount;
};

#endif // EVENTUTILS_H
