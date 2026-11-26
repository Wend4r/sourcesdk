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
	virtual void SuspendAsyncIO() = 0;

	// Starts the I/O service threads again.
	virtual void ResumeAsyncIO() = 0;

	// Waits until every I/O service thread has finished the requests queued before the call.
	virtual void FinishAsyncIO() = 0;

	// Named by what it does: like FinishAsyncIO for the requests under pFullPathPrefix, then calls fnCallback.
	virtual void BarrierForPath( const char *pFullPathPrefix, const std::function< void() > &fnCallback ) = 0;

	// FinishAsyncIO in this implementation.
	virtual void FinishAsyncWrites() = 0;

	virtual void AbortAllAsyncIO() = 0;

	// Blocks until the request has completed.
	virtual void BlockUntilComplete( std::shared_ptr< IAsyncReadRequest2 > pRequest ) = 0;
	virtual void BlockUntilComplete( std::shared_ptr< IAsyncWriteRequest2 > pRequest ) = 0;

	virtual std::unique_ptr< IReadRequestBuilder > NewReadRequest( const char *pFileName, const char *pPathID ) = 0;
	virtual std::unique_ptr< IWriteRequestBuilder > NewWriteRequest( const char *pFileName, const char *pPathID, const void *pData, size_t nDataSize ) = 0;
	virtual std::unique_ptr< IIOCompletionQueue > CreateCompletionQueue() = 0;

	// Request buffer allocation through the global allocator.
	virtual void *AllocateBuffer( size_t nSize ) = 0;
	virtual void ReleaseBuffer( void *pBuffer ) = 0;
};

#endif // FILESYSTEM_IASYNCFILESYSTEM2_H
