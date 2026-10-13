#ifndef SCHEMACOMPILER_MODEL_H
#define SCHEMACOMPILER_MODEL_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// The schemacompiler model: KeyValues3 values of one CKV3Arena per thread, behind opaque handles so
// the Clang frontend does not see the kv3lib headers
//--------------------------------------------------------------------------------------------------
#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier1/utlleanvector.h"

class CBufferString;
class CUtlBuffer;

struct ModelArena_t;
struct ModelValue_t;

// Member names of the model, each a CKV3MemberName with a precomputed hash
enum ModelKey_t
{
	MODEL_KEY_ABSTRACT,
	MODEL_KEY_ACCESS,
	MODEL_KEY_ARGS,
	MODEL_KEY_ATOMIC,
	MODEL_KEY_ATOMIC_CONFIG,
	MODEL_KEY_ATOMIC_KIND,
	MODEL_KEY_ATOMICS,
	MODEL_KEY_BASES,
	MODEL_KEY_BITS,
	MODEL_KEY_BUILTIN,
	MODEL_KEY_CACHE_FILE,
	MODEL_KEY_CATEGORY,
	MODEL_KEY_CODEGEN,
	MODEL_KEY_CODEGEN_TAGS,
	MODEL_KEY_COLUMN,
	MODEL_KEY_COMPILER,
	MODEL_KEY_CONFIG,
	MODEL_KEY_COUNT,
	MODEL_KEY_CPP,
	MODEL_KEY_DATA_CLASS,
	MODEL_KEY_DEFAULT_ERROR,
	MODEL_KEY_DEFAULT_VALUE,
	MODEL_KEY_DEFINES,
	MODEL_KEY_DEPS,
	MODEL_KEY_DYNAMIC_BINDING,
	MODEL_KEY_EMIT_CODEGEN,
	MODEL_KEY_END,
	MODEL_KEY_ENUMERATORS,
	MODEL_KEY_EXTERNAL,
	MODEL_KEY_FIELDS,
	MODEL_KEY_FILE,
	MODEL_KEY_FILES,
	MODEL_KEY_FLAG_TAGS,
	MODEL_KEY_GAPS,
	MODEL_KEY_HASH,
	MODEL_KEY_ID,
	MODEL_KEY_IMPORT_MODULES,
	MODEL_KEY_INCLUDES,
	MODEL_KEY_INNER,
	MODEL_KEY_INPUT,
	MODEL_KEY_INPUTS,
	MODEL_KEY_INT,
	MODEL_KEY_IS_CPP,
	MODEL_KEY_KEY,
	MODEL_KEY_KIND,
	MODEL_KEY_LINE,
	MODEL_KEY_LOCATIONS,
	MODEL_KEY_MANIPULATOR,
	MODEL_KEY_MARKERS,
	MODEL_KEY_META,
	MODEL_KEY_MODEL,
	MODEL_KEY_MTIME,
	MODEL_KEY_NAME,
	MODEL_KEY_NOSCHEMA,
	MODEL_KEY_OFFSET,
	MODEL_KEY_OPTIONS,
	MODEL_KEY_OUTPUT,
	MODEL_KEY_OUTPUT_DIR,
	MODEL_KEY_PATH,
	MODEL_KEY_PCH,
	MODEL_KEY_PLATFORM,
	MODEL_KEY_POLYMORPHIC,
	MODEL_KEY_PRE_INCLUDE,
	MODEL_KEY_PREFIX,
	MODEL_KEY_PROJECT,
	MODEL_KEY_REGISTRATION,
	MODEL_KEY_RETURN_TYPE,
	MODEL_KEY_SCOPE,
	MODEL_KEY_SCRIPT,
	MODEL_KEY_SIZE,
	MODEL_KEY_STANDARD,
	MODEL_KEY_STORAGE,
	MODEL_KEY_TAG_HEADERS,
	MODEL_KEY_TAGS,
	MODEL_KEY_TARGET,
	MODEL_KEY_TEMPLATE,
	MODEL_KEY_TYPE,
	MODEL_KEY_TYPES,
	MODEL_KEY_UNITY_BATCH_SIZE,
	MODEL_KEY_VALUE,
	MODEL_KEY_VERSION,

	MODEL_KEY_COUNT_OF,
};

const char *Model_KeyName( ModelKey_t eKey );

// Arenas; values are freed only with their arena
ModelArena_t *Model_CreateArena();
void Model_DestroyArena( ModelArena_t *pArena );
void Model_ClearArena( ModelArena_t *pArena );
ModelValue_t *Model_Root( ModelArena_t *pArena );

// Builders; strings are interned in the arena of the value
ModelValue_t *Model_Member( ModelValue_t *pTable, ModelKey_t eKey );
ModelValue_t *Model_MemberByName( ModelValue_t *pTable, const char *pszName );
ModelValue_t *Model_Append( ModelValue_t *pArray );
void Model_SetTable( ModelValue_t *pValue );
void Model_SetArray( ModelValue_t *pValue );
void Model_SetNull( ModelValue_t *pValue );
void Model_SetString( ModelValue_t *pValue, const char *pszString, int nLength = -1 );
void Model_SetInt( ModelValue_t *pValue, int64 nValue );
void Model_SetUInt( ModelValue_t *pValue, uint64 nValue );
void Model_SetBool( ModelValue_t *pValue, bool bValue );
void Model_Copy( ModelValue_t *pDest, const ModelValue_t *pSource );

void Model_SetMemberString( ModelValue_t *pTable, ModelKey_t eKey, const char *pszString );
void Model_SetMemberInt( ModelValue_t *pTable, ModelKey_t eKey, int64 nValue );
void Model_SetMemberUInt( ModelValue_t *pTable, ModelKey_t eKey, uint64 nValue );
void Model_SetMemberBool( ModelValue_t *pTable, ModelKey_t eKey, bool bValue );
ModelValue_t *Model_MemberTable( ModelValue_t *pTable, ModelKey_t eKey );
ModelValue_t *Model_MemberArray( ModelValue_t *pTable, ModelKey_t eKey );

// Readers
bool Model_IsNull( const ModelValue_t *pValue );
bool Model_IsTable( const ModelValue_t *pValue );
bool Model_IsArray( const ModelValue_t *pValue );
bool Model_IsString( const ModelValue_t *pValue );
const ModelValue_t *Model_Find( const ModelValue_t *pTable, ModelKey_t eKey );
const ModelValue_t *Model_FindByName( const ModelValue_t *pTable, const char *pszName );
const char *Model_GetString( const ModelValue_t *pValue, const char *pszDefault = "" );
int64 Model_GetInt( const ModelValue_t *pValue, int64 nDefault = 0 );
uint64 Model_GetUInt( const ModelValue_t *pValue, uint64 nDefault = 0 );
bool Model_GetBool( const ModelValue_t *pValue, bool bDefault = false );
int Model_Count( const ModelValue_t *pArray );
const ModelValue_t *Model_At( const ModelValue_t *pArray, int nIndex );
int Model_MemberCount( const ModelValue_t *pTable );
const char *Model_MemberNameAt( const ModelValue_t *pTable, int nIndex );
const ModelValue_t *Model_MemberAt( const ModelValue_t *pTable, int nIndex );

const char *Model_GetMemberString( const ModelValue_t *pTable, ModelKey_t eKey, const char *pszDefault = "" );
int64 Model_GetMemberInt( const ModelValue_t *pTable, ModelKey_t eKey, int64 nDefault = 0 );
uint64 Model_GetMemberUInt( const ModelValue_t *pTable, ModelKey_t eKey, uint64 nDefault = 0 );
bool Model_GetMemberBool( const ModelValue_t *pTable, ModelKey_t eKey, bool bDefault = false );
void Model_GetMemberStrings( const ModelValue_t *pTable, ModelKey_t eKey, CUtlLeanVector< CUtlString > &strings );

// Text or binary KV3 through the kv3lib codec; on failure sError is "<name>:<line>:<col>: <message>"
bool Model_LoadFile( ModelValue_t *pRoot, const char *pszPath, CBufferString &sError );
bool Model_LoadMemory( ModelValue_t *pRoot, const void *pData, int nSize, const char *pszName, CBufferString &sError );
void Model_SaveText( const ModelValue_t *pRoot, CUtlBuffer &output );
void Model_SaveBinary( const ModelValue_t *pRoot, CUtlBuffer &output );

#endif // SCHEMACOMPILER_MODEL_H
