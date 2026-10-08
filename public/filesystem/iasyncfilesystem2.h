//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Threaded asynchronous file reads and writes
//
//===========================================================================//

#ifndef FILESYSTEM_IASYNCFILESYSTEM2_H
#define FILESYSTEM_IASYNCFILESYSTEM2_H
#ifdef _WIN32
#pragma once
#endif

#include <functional>
#include <memory>

#include "appframework/iappsystem.h"

class IAsyncReadRequest2;
class IAsyncWriteRequest2;
class IIOCompletionQueue;

abstract_class IAsyncFileSystem2 : public IAppSystem
{
public:
	class IReadRequestBuilder;
	class IWriteRequestBuilder;

	// Wakes, joins and destroys the I/O service threads.
	virtual void StopIOThreads() = 0;

	virtual void StartIOThreads() = 0;

	virtual void Barrier() = 0;

	virtual void BarrierForPath( const char *pFullPathPrefix, const std::function< void() > &fnCallback ) = 0;

	virtual void Flush() = 0;

	virtual void CancelAllRequests() = 0;

	// Blocks until the request has completed.
	virtual void WaitForRequest( std::shared_ptr< IAsyncReadRequest2 > pRequest ) = 0;
	virtual void WaitForRequest( std::shared_ptr< IAsyncWriteRequest2 > pRequest ) = 0;

	virtual std::unique_ptr< IReadRequestBuilder > CreateReadRequest( const char *pFileName, const char *pPathID ) = 0;
	virtual std::unique_ptr< IWriteRequestBuilder > CreateWriteRequest( const char *pFileName, const char *pPathID, const void *pData, size_t nDataSize ) = 0;
	virtual std::unique_ptr< IIOCompletionQueue > CreateCompletionQueue() = 0;

	// Request buffer allocation through the global allocator.
	virtual void *AllocBuffer( size_t nSize ) = 0;
	virtual void FreeBuffer( void *pBuffer ) = 0;
};

#endif // FILESYSTEM_IASYNCFILESYSTEM2_H
