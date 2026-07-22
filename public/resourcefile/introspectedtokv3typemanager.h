#ifndef INTROSPECTEDTOKV3TYPEMANAGER_H
#define INTROSPECTEDTOKV3TYPEMANAGER_H

#ifdef _WIN32
	#pragma once
#endif

#include "resourcetype.h"

#include <tier0/platform.h>

class IResourceSystemUtils;
class IRD_RegisterResourceDataUtils;
class IResourceDeallocatorUtils;
class IAsyncProcessedDataResult;
struct ResourceFileHeader_t;

enum ResourceLoadType_t : int
{
	RESOURCE_LOAD_TYPE_DEFAULT = 0,
};

enum ResourceStreamingType_t : int
{
	RESOURCE_STREAMING_TYPE_NONE = 0,
};

class IResourceTypeManager
{
public:
	virtual bool Init( IResourceSystemUtils *pUtils ) { return false; }
	virtual void Shutdown() {}
	virtual void *GetErrorResource() { return nullptr; }
	virtual bool NeedsFrameUpdate() const { return false; }
	virtual void FrameUpdate( int nFrame ) {}
	virtual bool RequiresFinalizeResourceCall() const { return false; }
	virtual void FinalizeResource( ResourceHandle_t hResource, ResourceLoadType_t nLoadType ) {}
	virtual ResourceStreamingType_t GetStreamingType() const { return RESOURCE_STREAMING_TYPE_NONE; }
	virtual int GetNonStreamingDataLoadSize( ResourceHandle_t hResource, bool ) { return 0; }
	virtual void LoadStreamingData( ResourceHandle_t hResource, IResourceSystemUtils *pUtils ) {}
	virtual void AllocateResource( ResourceHandle_t hResource, ResourceId_t nId, const ResourceFileHeader_t *pHeader, IRD_RegisterResourceDataUtils *pUtils ) {}
	virtual void DeallocateResource( void *pData, IResourceDeallocatorUtils *pUtils ) {}
	virtual void NotifyResourceWithWorkPendingLoadCancelled( ResourceHandle_t hResource, void *pData ) {}
};

class CBaseIntrospectedToKV3TypeManager : public IResourceTypeManager
{
public:
	ResourceType_t GetResourceType() const { return m_nResourceType; }

public:
	ResourceType_t m_nResourceType;
	uint16 m_nIntrospectedResourceVersion;
	uint16 m_nKV3ResourceVersion;
	IResourceSystemUtils *m_pResourceSystemUtils;
};

template < ResourceType_t RESOURCE_TYPE, typename T >
class CDefaultIntrospectedToKV3TypeManager : public CBaseIntrospectedToKV3TypeManager
{
};

class IKV3ResourceAllocatorUtils;
class IKV3ResourceDeallocatorUtils;

template < ResourceType_t RESOURCE_TYPE, typename T >
class CTypesafeIntrospectedToKV3TypeManager : public CBaseIntrospectedToKV3TypeManager
{
public:
	virtual void *AllocateTypesafe( T *pData, IKV3ResourceAllocatorUtils *pUtils ) { return nullptr; }
	virtual void DeallocateTypesafe( void *pObject, IKV3ResourceDeallocatorUtils *pUtils ) {}
};

COMPILE_TIME_ASSERT( sizeof( CBaseIntrospectedToKV3TypeManager ) == 0x20 );

#endif // INTROSPECTEDTOKV3TYPEMANAGER_H
