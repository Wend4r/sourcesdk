#ifndef MODULELISTSNAPSHOT_H
#define MODULELISTSNAPSHOT_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// Snapshot of the modules loaded into a process, kept as variable-sized records
// in a caller-provided buffer or in a heap buffer of 4 KB
//-----------------------------------------------------------------------------
class CModuleListSnapshot
{
public:
	DLL_CLASS_IMPORT CModuleListSnapshot( uint8 *pBuffer = nullptr, size_t nBufferSize = 0 );
	// The heap buffer comes from g_pMemAlloc like in Purge
	~CModuleListSnapshot()
	{
		if ( m_pAllocatedBuffer )
			Release( m_pAllocatedBuffer );
	}

	DLL_CLASS_IMPORT bool Capture( uint32 nProcessId );
	DLL_CLASS_IMPORT bool Capture( void *hProcess );
	DLL_CLASS_IMPORT void Purge();

	// Start with a null *ppIterator, returns false after the last module
	DLL_CLASS_IMPORT bool GetNextModule( const void **ppIterator, const char **ppszName, void **ppBase, size_t *pSize ) const;

	int GetModuleCount() const { return m_nModuleCount; }

	// An executable or a shared library; b64BitOnly refuses a 32-bit one
	DLL_CLASS_IMPORT static bool IsElfHeader( const void *pData, size_t nSize, bool b64BitOnly );
	DLL_CLASS_IMPORT static bool IsElfHeader( void *hProcess, uint64 nAddress, uint64 nSize, bool b64BitOnly );
	DLL_CLASS_IMPORT static bool IsMachoHeader( const void *pData, size_t nSize, bool b64BitOnly );
	DLL_CLASS_IMPORT static bool IsExeModule( const void *pData, size_t nSize );
	DLL_CLASS_IMPORT static bool IsNotPlatModule( const char *pszName, const void *pData, size_t nSize );

	DLL_CLASS_IMPORT static bool FindElfBuildId( const void *pData, size_t nSize, uint8 **ppBuildId, uint32 *pBuildIdSize );
	DLL_CLASS_IMPORT static bool FindMachoUuid( const void *pData, size_t nSize, uint8 **ppUuid, uint32 *pUuidSize );

	// The extent of the loadable segments, rounded out to nAlignment (a power of two)
	DLL_CLASS_IMPORT static size_t FindElfVirtualSize( const void *pData, size_t nSize, size_t nAlignment );
	DLL_CLASS_IMPORT static size_t FindElfVirtualSize( void *hProcess, uint64 nAddress, uint64 nSize, size_t nAlignment );
	DLL_CLASS_IMPORT static size_t FindMachoVirtualSize( const void *pData, size_t nSize, size_t nAlignment );
	DLL_CLASS_IMPORT static size_t FindModuleVirtualSize( const void *pData, size_t nSize );
#ifdef PLATFORM_LINUX
	DLL_CLASS_IMPORT static size_t FindModuleVirtualSize( void *hProcess, uint64 nAddress, uint64 nSize );
#endif

private:
	uint8 *m_pUserBuffer;
	size_t m_nUserBufferSize;
	uint8 *m_pBuffer;
	size_t m_nBufferSize;
	uint8 *m_pBufferEnd;
	uint8 *m_pAllocatedBuffer;
	int m_nModuleCount;

	CModuleListSnapshot( const CModuleListSnapshot & ) = delete;
	CModuleListSnapshot &operator=( const CModuleListSnapshot & ) = delete;
};

#endif // MODULELISTSNAPSHOT_H
