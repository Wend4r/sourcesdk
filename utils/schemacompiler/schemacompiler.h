#ifndef SCHEMACOMPILER_H
#define SCHEMACOMPILER_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// schemacompiler: generates schema records and the registrar for the schema types of a target.
// The SDK platform macros come from the target, see cmake/sourcesdk/targets/schemacompiler.cmake
//--------------------------------------------------------------------------------------------------
// LLVM_DEFINITIONS give it the value 1, platform.h defines it again without one
#undef __STDC_LIMIT_MACROS

#include "tier0/platform.h"
#include "tier0/bufferstring.h"
#include "tier0/strtools.h"
#include "tier0/threadtools.h"
#include "tier0/utlbuffer.h"
#include "tier0/utlstring.h"
#include "tier1/generichash.h"
#include "tier1/utlleanvector.h"
#include "tier1/utlmap.h"
#include "tier1/utlvector.h"

#include <type_traits>

#include "model.h"

#define SCHEMACOMPILER_VERSION "2.0.0"

// Matches public/schemasystem/schemaversionnumbers.h
#define SCHEMACOMPILER_BINDING_VERSION 1
#define SCHEMACOMPILER_PROJECT_VERSION 1

// Ordered maps and sorts by name
struct CSchemaStringLess
{
	bool operator!() const { return false; }
	bool operator()( const CUtlString &sLeft, const CUtlString &sRight ) const { return V_strcmp( sLeft.Get(), sRight.Get() ) < 0; }
};

template < typename T >
using CSchemaStringMap = CUtlOrderedMap< CUtlString, T, CSchemaStringLess >;

// Strings that live on: map keys, struct members, list elements. Strings that live in a scope are
// CBufferStringN on the stack, e.g. CSchemaPathString
using CSchemaStringList = CUtlLeanVector< CUtlString >;
using CSchemaPathString = CBufferStringN< 512 >;

//--------------------------------------------------------------------------------------------------
// CUtlLeanVector helpers: it has no copy, no sort and no IsEmpty
//--------------------------------------------------------------------------------------------------
template < typename T, typename TOther >
void Vector_Append( CUtlLeanVector< T > &dest, const TOther &source )
{
	for ( const T &item : source )
		dest.AddToTail( item );
}

template < typename T, typename TOther >
void Vector_Copy( CUtlLeanVector< T > &dest, const TOther &source )
{
	dest.RemoveAll();
	Vector_Append( dest, source );
}

// Stable merge sort, equal elements keep their order
template < typename TVector, typename TLess >
void Vector_Sort( TVector &vector, TLess less )
{
	using T = typename std::remove_reference< decltype( vector[ 0 ] ) >::type;

	const int nCount = vector.Count();

	if ( nCount < 2 )
		return;

	CUtlLeanVector< T > scratch;

	scratch.SetCount( nCount );

	for ( int nWidth = 1; nWidth < nCount; nWidth *= 2 )
	{
		for ( int nLeft = 0; nLeft < nCount; nLeft += 2 * nWidth )
		{
			const int nMiddle = Min( nLeft + nWidth, nCount ), nRight = Min( nLeft + 2 * nWidth, nCount );
			int i = nLeft, j = nMiddle, k = nLeft;

			while ( i < nMiddle && j < nRight )
				scratch[ k++ ] = less( vector[ j ], vector[ i ] ) ? vector[ j++ ] : vector[ i++ ];

			while ( i < nMiddle )
				scratch[ k++ ] = vector[ i++ ];

			while ( j < nRight )
				scratch[ k++ ] = vector[ j++ ];
		}

		for ( int i = 0; i < nCount; ++i )
			vector[ i ] = scratch[ i ];
	}
}

//--------------------------------------------------------------------------------------------------
// Project: schema_project.<config>.kv3 written by sourcesdk_target_schema()
//--------------------------------------------------------------------------------------------------
struct SchemaInput_t
{
	CUtlString m_sInput;		// Absolute, '/' separated
	CUtlString m_sOutput;		// Absolute generated .cpp
	CUtlString m_sRelative;		// Path the outputs are named after, e.g. "src/foo.h"
	CUtlString m_sId;			// Identifier made of m_sRelative, e.g. "src_foo_h"
	bool m_bIsCpp = false;
};

enum SchemaCompilerKind_t
{
	SCHEMA_COMPILER_GNU,		// GCC or Clang command line
	SCHEMA_COMPILER_MSVC,		// cl command line, parsed with --driver-mode=cl
};

struct SchemaProject_t
{
	CUtlString m_sPath;			// The project file itself
	CUtlString m_sProject;		// m_pszProjectName of the records
	CUtlString m_sTarget;
	CUtlString m_sConfig;
	CUtlString m_sScope;		// "MODULE" or "GLOBAL"
	CUtlString m_sPlatform;		// "windows", "linux" or "macos"
	CUtlString m_sStandard;		// "17", "20", ...
	SchemaCompilerKind_t m_eCompiler = SCHEMA_COMPILER_GNU;

	CSchemaStringList m_Defines;
	CSchemaStringList m_Includes;
	CSchemaStringList m_Options;
	CSchemaStringList m_PreIncludes;
	CSchemaStringList m_TagHeaders;	// Parsed ahead of the inputs for the tag declarations
	CSchemaStringList m_ImportModules;
	CUtlString m_sPch;

	CUtlString m_sAtomicConfig;
	CUtlString m_sOutputDir;
	CUtlString m_sRegistration;
	CUtlString m_sCacheFile;
	int m_nUnityBatchSize = 0;
	bool m_bEmitCodegen = true;		// Runs the DECLARE_SCHEMA_CODEGEN_TAG scripts of META_USE_CODEGEN_TAG

	CUtlLeanVector< SchemaInput_t > m_Inputs;

	const char *ProjectId( CBufferString &sId ) const;
};

bool Project_Load( const char *pszPath, SchemaProject_t &project );

//--------------------------------------------------------------------------------------------------
// Atomic templates and classes: cmake/sourcesdk/schema/atomic_types.kv3
//--------------------------------------------------------------------------------------------------
enum SchemaAtomicKind_t
{
	SCHEMA_ATOMIC_KIND_PLAIN,				// Type_Atomic, e.g. CUtlString
	SCHEMA_ATOMIC_KIND_T,					// Type_Atomic_T, e.g. CHandle< T >
	SCHEMA_ATOMIC_KIND_TT,					// Type_Atomic_TT, e.g. CUtlOrderedMap< K, V >
	SCHEMA_ATOMIC_KIND_I,					// Type_Atomic_I, e.g. CBitVec< N >
	SCHEMA_ATOMIC_KIND_COLLECTION,			// Type_Atomic_CollectionOfT, e.g. CUtlVector< T >
	SCHEMA_ATOMIC_KIND_COLLECTION_FIXED,	// Type_Atomic_CollectionOfT with a fixed count, e.g. CUtlVectorFixed< T, N >
	SCHEMA_ATOMIC_KIND_TRANSPARENT,			// Not an atomic: the field is described by its first type argument, e.g. CNetworkVarBase< T, ... >
};

struct SchemaAtomicDesc_t
{
	CUtlString m_sName;
	SchemaAtomicKind_t m_eKind = SCHEMA_ATOMIC_KIND_PLAIN;
	CUtlString m_sManipulator;	// Collection manipulator template, e.g. "SchemaGen_UtlVectorManipulator"
};

using SchemaAtomicMap_t = CSchemaStringMap< SchemaAtomicDesc_t >;

bool AtomicConfig_Load( const char *pszPath, SchemaAtomicMap_t &atomics );
const char *AtomicKind_Name( SchemaAtomicKind_t eKind );
bool AtomicKind_Parse( const char *pszName, SchemaAtomicKind_t &eKind );

//--------------------------------------------------------------------------------------------------
// Diagnostics: file(line,col): error SC####: ... for MSVC, file:line:col: error: ... otherwise
//--------------------------------------------------------------------------------------------------
enum SchemaDiagCode_t
{
	SC_NONE = 0,
	SC_PROJECT = 1000,			// Project, config or command line
	SC_UNKNOWN_TAG = 1001,		// Tag without DECLARE_SCHEMA_META_TAG
	SC_TAG_LOCATION = 1002,		// Tag not allowed at this location
	SC_TAG_VALUE = 1003,		// Value on a tag declared with META_TAG_ONLY()
	SC_UNKNOWN_ATOMIC = 1004,	// Template not listed in atomic_types.kv3
	SC_FIELD_TYPE = 1005,		// Field type the schema system cannot describe
	SC_MARKER = 1006,			// Markup not attached to a declaration
	SC_PARSE = 1007,			// Clang errors
	SC_SCRIPT = 1008,			// Codegen tag script
	SC_DUPLICATE = 1009,		// Two definitions of one name
	SC_IO = 1010,				// Files
	SC_LAYOUT = 1011,			// Layout diagnostics
};

struct SchemaDiagLocation_t
{
	CUtlString m_sFile;
	int m_nLine = 0;
	int m_nColumn = 0;
};

class CSchemaDiagnostics
{
public:
	CSchemaDiagnostics();

	void SetMsvcFormat( bool bMsvc ) { m_bMsvcFormat = bMsvc; }

	void Error( SchemaDiagCode_t eCode, const SchemaDiagLocation_t &location, const char *pszFormat, ... ) FMTFUNCTION( 4, 5 );
	void Warning( SchemaDiagCode_t eCode, const SchemaDiagLocation_t &location, const char *pszFormat, ... ) FMTFUNCTION( 4, 5 );
	void Note( const SchemaDiagLocation_t &location, const char *pszFormat, ... ) FMTFUNCTION( 3, 4 );

	int ErrorCount() const { return m_nErrors; }

private:
	void Print( const char *pszLevel, SchemaDiagCode_t eCode, const SchemaDiagLocation_t &location, const char *pszFormat, va_list args );

	bool m_bMsvcFormat;
	CInterlockedInt m_nErrors;
	CThreadFastMutex m_Mutex;
};

extern CSchemaDiagnostics g_Diagnostics;

//--------------------------------------------------------------------------------------------------
// Files and names
//--------------------------------------------------------------------------------------------------
// Results go into a caller buffer, usually a CSchemaPathString on the stack, and are returned
const char *Path_Normalize( const char *pszPath, CBufferString &sOut );						// '/' separators, no "." and ".." parts
const char *Path_Absolute( const char *pszPath, const char *pszBase, CBufferString &sOut );
bool Path_Relative( const char *pszPath, const char *pszBase, CBufferString &sOut );			// False without a relative path
const char *Path_Directory( const char *pszPath, CBufferString &sOut );
bool Path_IsUnder( const char *pszPath, const char *pszDirectory );

bool File_Read( const char *pszPath, CUtlBuffer &data );
bool File_Exists( const char *pszPath );

// Writes only if the content differs; returns false on an I/O error
bool File_WriteIfChanged( const char *pszPath, const void *pData, int nSize, bool *pWritten = nullptr );
bool File_WriteIfChanged( const char *pszPath, const CUtlBuffer &data, bool *pWritten = nullptr );

// Touches the stamp file the build system tracks
bool File_Touch( const char *pszPath );

bool Directory_Create( const char *pszPath );

const char *Name_ToIdentifier( const char *pszName, CBufferString &sOut );			// "Outer::CFoo" -> "Outer__CFoo"
const char *String_Trim( const char *pszString, int nLength, CBufferString &sOut );
bool String_StartsWith( const char *pszString, const char *pszPrefix );

// Sorted, without duplicates
void StringList_SortUnique( CSchemaStringList &list );
bool StringList_Contains( const CSchemaStringList &list, const char *pszString );

#endif // SCHEMACOMPILER_H
