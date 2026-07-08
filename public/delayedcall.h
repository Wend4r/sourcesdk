#ifndef DELAYEDCALL_H
#define DELAYEDCALL_H

#pragma once

#include <tier0/platform.h>
#include <tier1/utlcommon.h>

// Declares a CDelayedCall2 alias for the enclosing class, taking the dispatch
// base (TBaseObject) and concrete object (TObject) from its BaseClass / ThisClass typedefs.
#define DECLARE_DELAYED_CALL( aliasName, callType, lockType ) \
	using aliasName = CDelayedCall2< BaseClass, ThisClass, callType, lockType >

template < typename TObject >
class CDelayedCallBase
{
public:
	virtual ~CDelayedCallBase() = default;
	virtual void Execute( TObject *pObject ) = 0;
};

template < typename TBaseObject, typename TObject, typename TCall, typename TLock = empty_t >
class CDelayedCall2 : public CDelayedCallBase< TBaseObject >
{
public:
	CDelayedCall2() = default;
	explicit CDelayedCall2( const TCall &call )
		: m_Call( call )
	{
	}
	explicit CDelayedCall2( TCall &&call )
		: m_Call( Move( call ) )
	{
	}

	virtual ~CDelayedCall2() override = default;

	virtual void Execute( TBaseObject *pObject ) override
	{
		static_cast< TObject * >( pObject )->ExecuteDelayedCall( m_Call );
	}

public:
	TCall m_Call;
	TLock m_Lock;
};

template < typename TObject, typename TCall >
class CDelayedCall : public CDelayedCall2< TObject, TObject, TCall, empty_t >
{
public:
	using BaseClass = CDelayedCall2< TObject, TObject, TCall, empty_t >;
	using BaseClass::BaseClass;

	virtual ~CDelayedCall() override = default;
};

#endif // DELAYEDCALL_H
