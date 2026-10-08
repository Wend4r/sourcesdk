#ifndef IRESOURCEHANDLEUTILS_H
#define IRESOURCEHANDLEUTILS_H

#pragma once

#include "interfaces/interfaces.h"
#include "resourcefile/resourcetype.h"

class IResourceTypeManager;

struct SerialResourceHandle_t
{
	ResourceHandle_t GetPointer() const { return reinterpret_cast< ResourceHandle_t >( m_nData & 0xFFFFFFFFFFF8ull ); }
	uint32 GetSerial() const { return static_cast< uint32 >( ( m_nData & 7 ) | ( ( m_nData >> 45 ) & 0x7FFF8 ) ); }

	uint64 m_nData;
};

COMPILE_TIME_ASSERT( sizeof( SerialResourceHandle_t ) == 0x8 );

class IResourceHandleUtils
{
public:
	virtual ResourceHandle_t FindOrRegisterResourceByName( CResourceString &pResourceName ) = 0;
	virtual ResourceHandle_t FindResourceById( ResourceId_t nResourceId, ResourceType_t nType ) = 0;
	virtual void DeleteResource( ResourceHandle_t hResource ) = 0;
	virtual void unk003() = 0;
	virtual void unk004() = 0;
	virtual void unk005() = 0;
	virtual ResourceStatus_t GetResourceStatus( ResourceHandle_t hResource ) = 0;
	virtual IResourceTypeManager *GetTypeManagerForBinding( ResourceHandle_t hResource ) = 0;
	// Allocator and serial handle self-test.
	virtual void RunSelfTest() = 0;
	// Returns an empty handle if hResource is not a live binding.
	virtual SerialResourceHandle_t HandleUtils_MakeSerialHandle( ResourceHandle_t hResource ) = 0;
	virtual ResourceHandle_t HandleUtils_Deref( SerialResourceHandle_t hSerial ) = 0;
};

DECLARE_TIER2_INTERFACE( IResourceHandleUtils, g_pResourceHandleUtils );

#endif // IRESOURCEHANDLEUTILS_H
