#ifndef ASYNCFUNCTION_H
#define ASYNCFUNCTION_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utldelegateimpl.h"

//-----------------------------------------------------------------------------
// Handle of a function that IAsyncScheduledFunctions calls later
//-----------------------------------------------------------------------------
class AsyncFunctionHandle_t
{
public:
	AsyncFunctionHandle_t() {}
	explicit AsyncFunctionHandle_t( int32 nValue ) : m_Value( nValue ) {}

	void SetRaw( int32 nValue ) { m_Value = nValue; }
	int32 GetRaw() const { return m_Value; }
	int32 &GetRawRef() { return m_Value; }
	const int32 &GetRawRef() const { return m_Value; }
	int32 *GetRawPtrForWrite() { return &m_Value; }

	bool operator==( const AsyncFunctionHandle_t &other ) const { return m_Value == other.m_Value; }
	bool operator!=( const AsyncFunctionHandle_t &other ) const { return m_Value != other.m_Value; }
	bool operator<( const AsyncFunctionHandle_t &other ) const { return m_Value < other.m_Value; }

	AsyncFunctionHandle_t &operator=( int32 nValue ) { m_Value = nValue; return *this; }

private:
	int32 m_Value;
};

//-----------------------------------------------------------------------------
// Calls functions at a time on a thread of its own; a function returns the
// interval of its next call
//-----------------------------------------------------------------------------
abstract_class IAsyncScheduledFunctions
{
public:
	virtual ~IAsyncScheduledFunctions() {}

	virtual void Init() = 0;
	virtual void Shutdown() = 0;

	// flTime is when the first call happens
	virtual AsyncFunctionHandle_t RegisterScheduledFunction( const CUtlDelegate< float() > &function, double flTime ) = 0;
	virtual bool RescheduleFunction( AsyncFunctionHandle_t hFunction, double flTime ) = 0;
	virtual void UnregisterScheduledFunction( AsyncFunctionHandle_t hFunction ) = 0;
};

PLATFORM_INTERFACE IAsyncScheduledFunctions *g_pScheduledFunctions;

#endif // ASYNCFUNCTION_H
