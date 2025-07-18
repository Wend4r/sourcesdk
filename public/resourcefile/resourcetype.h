#ifndef RESOURCETYPE_H
#define RESOURCETYPE_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include <tier0/platform.h>
#include <tier0/threadtools.h>
#include <tier1/utlsymbollarge.h>

#include <initializer_list>

typedef uint64 ResourceType_t;

enum ResourceStatus_t
{
	RESOURCE_STATUS_UNKNOWN = 0,
	RESOURCE_STATUS_KNOWN_BUT_NOT_RESIDENT,
	RESOURCE_STATUS_PARTIALLY_RESIDENT,
	RESOURCE_STATUS_RESIDENT,
};

enum ResourceManifestLoadBehavior_t
{
	RESOURCE_MANIFEST_LOAD_DEFAULT = -1,
	RESOURCE_MANIFEST_LOAD_STREAMING_DATA = 0,
	RESOURCE_MANIFEST_INITIALLY_USE_FALLBACKS,
};

enum ResourceManifestLoadPriority_t
{
	RESOURCE_MANIFEST_LOAD_PRIORITY_DEFAULT = -1,
	RESOURCE_MANIFEST_LOAD_PRIORITY_LOW = 0,
	RESOURCE_MANIFEST_LOAD_PRIORITY_MEDIUM,
	RESOURCE_MANIFEST_LOAD_PRIORITY_HIGH,
	RESOURCE_MANIFEST_LOAD_PRIORITY_IMMEDIATE,

	RESOURCE_MANIFEST_LOAD_PRIORITY_COUNT,
};

enum ResourceBindingFlags_t
{
	RESOURCE_BINDING_LOADED = 0x1,
	RESOURCE_BINDING_ERROR = 0x2,
	RESOURCE_BINDING_UNLOADABLE = 0x4,
	RESOURCE_BINDING_PROCEDURAL = 0x8,
	RESOURCE_BINDING_TRACKLEAKS = 0x20,
	RESOURCE_BINDING_IS_ERROR_BINDING_FOR_TYPE = 0x40,
	RESOURCE_BINDING_HAS_EVER_BEEN_LOADED = 0x80,
	RESOURCE_BINDING_ANONYMOUS = 0x100,

	RESOURCE_BINDING_FIRST_UNUSED_FLAG = 0x200
};

enum ResourceSystemUpdateMode_t : int
{
};

enum ResourceSystemGetNamedResourcesFlags_t : int
{
};

struct ResourceId_t
{
	uint64 m_Data;

	operator uint64() const { return Get(); }
	uint64 Get() const { return m_Data; }
};

struct ResourceNameInfo_t
{
	CUtlSymbolLarge m_ResourceNameSymbol;
};

class CResourcePointerBase
{
public:
	int32 GetOffset() const { return m_nOffset; }

private:
	int32 m_nOffset;
};

template < typename T >
class CResourcePointer : public CResourcePointerBase
{
};

class CResourceString : public CResourcePointer< char >
{
};

class CStrongHandleVoid;

struct ResourceManifestEntry_t
{
	const char *m_pResourceName;
	bool m_bResourceNameIsManifestName;
	CStrongHandleVoid *m_pHandle;
	ResourceManifestEntry_t *m_pNextInstanceOfThisEntry;
	int m_nHandleReferences;
};

typedef std::initializer_list< std::initializer_list< ResourceManifestEntry_t > > ManifestEntryGroupList_t;

struct ResourceManifestDesc_t
{
	CUtlString m_sManifestName;
	CUtlString m_sManifestGroup;
	ManifestEntryGroupList_t *m_pEntryGroups;
	CUtlString m_sFileName;
	int m_nLine;
	bool m_bRegistered;
	bool m_bDisallowRegistration;
};

typedef const ResourceNameInfo_t *ResourceNameHandle_t;
typedef uint16 LoadingResourceIndex_t;
typedef char ResourceTypeIndex_t;
typedef uint32 ExtRefIndex_t;

struct ResourceBindingBase_t
{
	void* m_pData;
	ResourceNameHandle_t m_Name;
	uint16 m_nFlags;
	uint16 m_nReloadCounter;
	ResourceTypeIndex_t m_nTypeIndex;
	LoadingResourceIndex_t m_nLoadingResource;
	CInterlockedInt m_nRefCount;
	ExtRefIndex_t m_nExtRefHandle;
};

typedef const ResourceBindingBase_t *ResourceHandle_t;
typedef void *HGameResourceManifest;

#endif // RESOURCETYPE_H
