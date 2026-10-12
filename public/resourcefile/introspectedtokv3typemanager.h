#ifndef INTROSPECTEDTOKV3TYPEMANAGER_H
#define INTROSPECTEDTOKV3TYPEMANAGER_H

#ifdef _WIN32
	#pragma once
#endif

#include "resourcetype.h"

#include <tier0/platform.h>
#include "resourcesystem/iresourcesystem.h"

class IResourceSystemUtils;
class IRD_RegisterResourceDataUtils;
class IResourceDeallocatorUtils;
class IAsyncProcessedDataResult;
struct ResourceFileHeader_t;

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
