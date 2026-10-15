#ifndef RESOURCETYPE_H
#define RESOURCETYPE_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include <tier0/platform.h>
#include <tier0/threadtools.h>
#include <tier0/bufferstring.h>
#include <tier0/utlsymbollarge.h>
#include <tier0/generichash.h>
#include <schemasystem/schemametatag.h>

#include <initializer_list>
#include "resourcesystem/resourcesystemtypes.h"
#include "resourcesystem/resourcemanifesthelpers.h"

#include "schemacompiler/schemaon.h"

typedef uint64 ResourceType_t;

// Builds a resource type from the characters of its extension (up to 8), the first character in the low byte, e.g. RESOURCE_TYPE_ID( 'v', 'm', 'a', 'p' )
#define RESOURCE_TYPE_ID_EXPAND( x ) x
#define RESOURCE_TYPE_ID_IMPL( c0, c1, c2, c3, c4, c5, c6, c7, ... ) \
	( ( ResourceType_t )( uint8 )( c0 ) | ( ( ResourceType_t )( uint8 )( c1 ) << 8 ) | ( ( ResourceType_t )( uint8 )( c2 ) << 16 ) | ( ( ResourceType_t )( uint8 )( c3 ) << 24 ) | \
	  ( ( ResourceType_t )( uint8 )( c4 ) << 32 ) | ( ( ResourceType_t )( uint8 )( c5 ) << 40 ) | ( ( ResourceType_t )( uint8 )( c6 ) << 48 ) | ( ( ResourceType_t )( uint8 )( c7 ) << 56 ) )
#define RESOURCE_TYPE_ID( ... ) RESOURCE_TYPE_ID_EXPAND( RESOURCE_TYPE_ID_IMPL( __VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0 ) )

// Surface properties, "surfaceproperties/surfaceproperties.vsurf"
#define RESOURCE_TYPE_SURFACE_PROPERTIES RESOURCE_TYPE_ID( 'v', 's', 'u', 'r', 'f' )

// Physics aggregate data of models, "*.vphys"
#define RESOURCE_TYPE_PHYSICS_DATA RESOURCE_TYPE_ID( 'v', 'p', 'h', 'y', 's' )

// World nodes, "*.vwnod"
#define RESOURCE_TYPE_WORLD_NODE RESOURCE_TYPE_ID( 'v', 'w', 'n', 'o', 'd' )

// Worlds, "*.vwrld"
#define RESOURCE_TYPE_WORLD RESOURCE_TYPE_ID( 'v', 'w', 'r', 'l', 'd' )

// Entity lumps of a world, "*.vents"
#define RESOURCE_TYPE_ENTITY_LUMP RESOURCE_TYPE_ID( 'v', 'e', 'n', 't', 's' )

// Resource type of the info class, the packed file extension, e.g. 'vmap'
DECLARE_SCHEMA_META_TAG( MResourceTypeForInfoType, META_TAG_ON_CLASS, META_VALUE( ResourceType_t ) );

enum ResourceManifestLoadBehavior_t : int8
{
	RESOURCE_MANIFEST_LOAD_DEFAULT = -1,
	RESOURCE_MANIFEST_LOAD_STREAMING_DATA = 0,
	RESOURCE_MANIFEST_INITIALLY_USE_FALLBACKS,
};

enum ResourceManifestLoadPriority_t : int8
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

schema struct ResourceId_t
{
	TYPEMETA( MIsBoxedIntegerType );
	DECLARE_SCHEMA_DATA_CLASS( ResourceId_t );

	ResourceId_t() : m_Value( 0 ) {}
	ResourceId_t( uint64 nValue ) : m_Value( nValue ) {}

	uint64 m_Value;

	operator uint64() const { return Get(); }
	uint64 Get() const { return m_Value; }
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

schema template < typename T >
class CResourcePointer : public CResourcePointerBase
{
};

class CResourceStringOffset : public CResourcePointer< char >
{
};

schema class CResourceString : public CBufferStringN< 200 >
{
public:
	using BaseClass = CBufferStringN< 200 >;

	CResourceString() : BaseClass( true ) {}
	CResourceString( const char *pszPath, bool bAllowHeapAllocation = true ) :
	    BaseClass( pszPath, -1, bAllowHeapAllocation ) {}
	template< size_t N > CResourceString( const char ( &str )[N], bool bAllowHeapAllocation = true ) :
	    BaseClass( str, N - 1, bAllowHeapAllocation ) {}

	ResourceId_t   GetResourceId() const   { return m_nResourceId; }
	ResourceType_t GetResourceType() const { return m_nResourceType; }

	void SetResourceId( ResourceId_t id )    { m_nResourceId = id; }
	void SetResourceType( ResourceType_t t ) { m_nResourceType = t; }

	void Populate( const char *pszPath )
	{
		Set( pszPath );
		NormalizePathInline();
		m_nResourceId.m_Value = MurmurHash64( String(), Length(), 0xEDABCDEFu );
		m_nResourceType = ComputeTypeFromExtension( String() );
	}

private:
	uint64 ComputeTypeFromExtension( const char *pszPath )
	{
		const char *pExt = nullptr;
		for ( const char *p = pszPath; *p; ++p )
			if ( *p == '.' ) pExt = p + 1;
		if ( !pExt ) return 0;
		uint64 t = 0;
		for ( int i = 0; i < 8 && pExt[i]; ++i )
		{
			char c = pExt[i];
			if ( c >= 'A' && c <= 'Z' ) c += 32;
			if ( c == '_' ) break;
			t |= (uint64)(uint8)c << ( i * 8 );
		}
		return t;
	}

	void NormalizePathInline()
	{
		char *pBuf = Base();
		for ( int i = 0, n = Length(); i < n; ++i )
		{
			char c = pBuf[i];
			if ( c == '\\' ) pBuf[i] = '/';
			else if ( c >= 'A' && c <= 'Z' ) pBuf[i] = c + 32;
		}
	}

	ResourceId_t   m_nResourceId;
	ResourceType_t m_nResourceType;
};

class CStrongHandleVoid;

typedef const ResourceNameInfo_t *ResourceNameHandle_t;
typedef uint16 LoadingResourceIndex_t;
typedef uint16 StreamingResourceDataIndex_t;
typedef char ResourceTypeIndex_t;
typedef uint32 ExtRefIndex_t;

struct ResourceBindingBase_t
{
	void* m_pData;
	ResourceNameHandle_t m_Name;
	ResourceId_t m_nResourceId;
	uint16 m_nFlags;
	StreamingResourceDataIndex_t m_nStreamingResource;
	ResourceTypeIndex_t m_nResourceType;
	uint8 m_nReloadCounter;
	LoadingResourceIndex_t m_nLoadingResource;
	CInterlockedInt m_nRefCount;
	ExtRefIndex_t m_nExtRefHandle;
};

typedef const ResourceBindingBase_t* ResourceHandle_t;
typedef void *HGameResourceManifest;

#include "schemacompiler/schemaoff.h"

#endif // RESOURCETYPE_H
