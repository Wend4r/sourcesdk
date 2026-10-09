#ifndef THREADEDJOB_H
#define THREADEDJOB_H

#if COMPILER_MSVC
#pragma once
#endif

#include <tier1/jobthread.h>

template < typename T, typename TCall >
class CAsyncCallJob : public CThreadedJobWithDependencies
{
public:
	T *m_pObject;
	TCall m_Call;
};

#endif // THREADEDJOB_H
