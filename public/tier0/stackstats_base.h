#ifndef TIER0_STACKSTATS_BASE_H
#define TIER0_STACKSTATS_BASE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/threadtools.h"

class PLATFORM_CLASS CallStackStatStructDescFuncs
{
public:
	//description file format
	//1 byte version per entry. Assuming that the field will get more descriptive over time, reserving this now.
	virtual size_t DescribeField( uint8 *pDescribeWriteBuffer, size_t iDescribeMaxLength ) = 0;

	enum MergeScript_Language
	{
		SSMSL_Squirrel, //Only support squirrel for now, theoretically expandable to any vscript supported language
	};

#if 0 //embedded script handling not ready yet
	//this is expected to write a piece of script code into the body of a function that will merge two values of this type
	//The function will have two parameters, "mergeTo", and "mergeFrom". Both are tables with your field defined by name
	//So, an example to merge an integer count defined as "foo" in the stack struct would look like this "mergeTo.foo += mergeFrom.foo\n"
	virtual size_t DescribeMergeOperation( MergeScript_Language scriptLanguage, uint8 *pDescribeWriteBuffer, size_t iDescribeMaxLength ) = 0;
#endif

	//reserve your description field versions here to avoid stomping others
	enum DescribeFieldVersions_t
	{
		DFV_BasicStatStructFieldTypes_t, //Format: 1 byte BasicStatStructFieldTypes_t type, 4 byte field offset, null terminated string field name
	};

	const char *m_szFieldName;
	size_t m_iFieldOffset;
	CallStackStatStructDescFuncs *m_pNext; //needed for how the description macros are laid out. Couldn't figure out a way to store the static struct instances as a static array
};

enum StatStructDescription_LumpID
{
	SSDLID_UNKNOWN,
	SSDLID_STATICINTERPRETER,
	SSDLID_FIELDDESC,
	SSDLID_EMBEDDEDSCRIPT,


	SSDLID_COUNT,
	SSDLID_FORCE_UINT32 = 0xFFFFFFFF,
};

enum BasicStatStructFieldTypes_t
{
	BSSFT_UNKNOWN,
	BSSFT_BOOL,
	BSSFT_INT8,
	BSSFT_UINT8,
	BSSFT_INT16,
	BSSFT_UINT16,
	BSSFT_INT32,
	BSSFT_UINT32,
	BSSFT_INT64,
	BSSFT_UINT64,
	BSSFT_FLOAT,
	BSSFT_DOUBLE,
	BSSFT_VECTOR2D,
	BSSFT_VECTOR3D,
	BSSFT_VECTOR4D,

	BSSFT_COUNT,
};

enum BasicStatStructFieldCombineMethods_t
{
	BSSFCM_UNKNOWN,
	BSSFCM_CUSTOM, //rely on some outside handler
	BSSFCM_ADD, //add the values
	//BSSFCM_SUBTRACT, //what would subtract even mean? which one from which?
	BSSFCM_MAX, //keep max value
	BSSFCM_MIN, //keep min value
	BSSFCM_AND, // &= , Non-integer behavior undefined
	BSSFCM_OR, // |= , Non-integer behavior undefined
	BSSFCM_XOR, // ^= , Non-integer behavior undefined
	/*BSSFCM_LIST, //keep a list of each value (probably complicated)*/

	BSSFCM_COUNT,
};

class PLATFORM_CLASS BasicStatStructFieldDesc : public CallStackStatStructDescFuncs
{
public:
	BasicStatStructFieldDesc( BasicStatStructFieldTypes_t type, BasicStatStructFieldCombineMethods_t combineMethod ) : m_Type(type), m_Combine(combineMethod) {};
	size_t DescribeField( uint8 *pDescribeWriteBuffer, size_t iDescribeMaxLength );
#if 0 //embedded script handling not ready yet
	size_t DescribeMergeOperation( MergeScript_Language scriptLanguage, uint8 *pDescribeWriteBuffer, size_t iDescribeMaxLength );
#endif

	BasicStatStructFieldTypes_t m_Type;
	BasicStatStructFieldCombineMethods_t m_Combine;	
};

class CCallStackMarkerBase;
class CCallStackStatsGatherer_Base;
struct CallStackStatsGatherer_SharedInstanceData_t;

namespace detail
{
	template< class GATHERER >
	struct CallStackGatherer_FunctionTable_Template;
}

typedef detail::CallStackGatherer_FunctionTable_Template< CCallStackStatsGatherer_Base > CallStackGatherer_FunctionTable;

//-----------------------------------------------------------------------------
// Describes the fields of a stat struct, compiled out in CS2 (writes nothing)
//-----------------------------------------------------------------------------
class CStackStatsStructInfo
{
public:
	DLL_CLASS_IMPORT size_t WriteStructDescription( uint8 *pWriteBuffer, size_t nWriteBufferSize );
};

//-----------------------------------------------------------------------------
// Type-erased base of the call stack stats gatherers. The gatherers are compiled out
// in CS2: the STUB_ members do nothing and its data members are never accessed
//-----------------------------------------------------------------------------
class CCallStackStatsGatherer_Base
{
public:
	DLL_CLASS_IMPORT static const CallStackGatherer_FunctionTable *GetSTUBFunctionTable();

protected:
	DLL_CLASS_IMPORT uint32 STUB_GetCapturedCallStackDepth() const;
	DLL_CLASS_IMPORT uint32 STUB_GetStatStructSize() const;
	DLL_CLASS_IMPORT const char *STUB_GetStatStructName() const;
	DLL_CLASS_IMPORT void STUB_GetVectorData_StatEntries( const void *&pEntries, uint32 &nCount ) const;
	DLL_CLASS_IMPORT void STUB_GetVectorData_StoredSubTrees( const CCallStackStatsGatherer_Base *const *&pSubTrees, uint32 &nCount ) const;
	DLL_CLASS_IMPORT void STUB_EvaluateGathererLinkage( CCallStackStatsGatherer_Base &gatherer );
	DLL_CLASS_IMPORT void STUB_OnGathererRemoved( const CCallStackStatsGatherer_Base &gatherer );
	DLL_CLASS_IMPORT void STUB_OnLinkedGathererPushed( const CCallStackStatsGatherer_Base &gatherer, const CCallStackMarkerBase &marker );
	DLL_CLASS_IMPORT void STUB_OnLinkedGathererPopped( const CCallStackStatsGatherer_Base &gatherer );
	DLL_CLASS_IMPORT void *STUB_LockEntry( uint32 nEntry );
	DLL_CLASS_IMPORT void STUB_UnlockEntry( uint32 nEntry );
	DLL_CLASS_IMPORT void STUB_PopFromLinkedGatherers() const;
	DLL_CLASS_IMPORT void STUB_RemoveSharedInstanceData( const CallStackStatsGatherer_SharedInstanceData_t &data );
};

// The gatherers of the call stack stats, compiled out in CS2
typedef bool ( *FN_StackStats_EnumInstances_Callback )( CCallStackStatsGatherer_Base *pGatherer, const char *szUniqueID, void *pContext );
PLATFORM_INTERFACE void StackStats_EnumerateRegisteredInstances( FN_StackStats_EnumInstances_Callback pFN_Callback, void *pContext );
PLATFORM_INTERFACE bool StackStats_DumpNamedInstance( const char *szUniqueID, const char *szFileName, bool bDumpTranslationTable, bool bAllowMemoryAllocations );
PLATFORM_INTERFACE void StackStats_GathererRegistration( CCallStackStatsGatherer_Base &StatsGatherer, bool bRegister );
PLATFORM_INTERFACE bool StackStats_GetSymbolTranslationDefault();

// The instances of the gatherers shared between modules by their name
PLATFORM_INTERFACE CCallStackStatsGatherer_Base *_CCallStackStatsGatherer_GetSharedGlobalInstance( const char *szUniqueID, CCallStackStatsGatherer_Base *( *pCreateNewFunc )( void *pContext ), void *pContext );
PLATFORM_INTERFACE void _CCallStackStatsGatherer_ReleaseSharedGlobalInstance( const char *szUniqueID, CCallStackStatsGatherer_Base *pGatherer, const CallStackStatsGatherer_SharedInstanceData_t *pRemoveSharedInstanceData, void ( *pDeleteThisTypeFunc )( CCallStackStatsGatherer_Base *pGatherer ) );

#endif // TIER0_STACKSTATS_BASE_H
