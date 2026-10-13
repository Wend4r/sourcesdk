#include "schemacompiler.h"

#include "kv3lib/keyvalues3.h"
#include "kv3lib/kv3text.h"

namespace
{

// Indexed by ModelKey_t; the hashes are computed at compile time
const CKV3MemberName s_ModelKeys[] =
{
	"abstract",
	"access",
	"args",
	"atomic",
	"atomic_config",
	"atomic_kind",
	"atomics",
	"bases",
	"bits",
	"builtin",
	"cache_file",
	"category",
	"codegen",
	"codegen_tags",
	"column",
	"compiler",
	"config",
	"count",
	"cpp",
	"data_class",
	"default_error",
	"default_value",
	"defines",
	"deps",
	"dynamic_binding",
	"emit_codegen",
	"end",
	"enumerators",
	"external",
	"fields",
	"file",
	"files",
	"flag_tags",
	"gaps",
	"hash",
	"id",
	"import_modules",
	"includes",
	"inner",
	"input",
	"inputs",
	"int",
	"is_cpp",
	"key",
	"kind",
	"line",
	"locations",
	"manipulator",
	"markers",
	"meta",
	"model",
	"mtime",
	"name",
	"noschema",
	"offset",
	"options",
	"output",
	"output_dir",
	"path",
	"pch",
	"platform",
	"polymorphic",
	"pre_include",
	"prefix",
	"project",
	"registration",
	"return_type",
	"scope",
	"script",
	"size",
	"standard",
	"storage",
	"tag_headers",
	"tags",
	"target",
	"template",
	"type",
	"types",
	"unity_batch_size",
	"value",
	"version",
};

COMPILE_TIME_ASSERT( ARRAYSIZE( s_ModelKeys ) == MODEL_KEY_COUNT_OF );

KeyValues3 *KV( ModelValue_t *pValue ) { return reinterpret_cast< KeyValues3 * >( pValue ); }
const KeyValues3 *KV( const ModelValue_t *pValue ) { return reinterpret_cast< const KeyValues3 * >( pValue ); }
ModelValue_t *Value( KeyValues3 *pKV ) { return reinterpret_cast< ModelValue_t * >( pKV ); }
const ModelValue_t *Value( const KeyValues3 *pKV ) { return reinterpret_cast< const ModelValue_t * >( pKV ); }

CKV3Arena *Arena( ModelArena_t *pArena ) { return reinterpret_cast< CKV3Arena * >( pArena ); }

} // namespace

const char *Model_KeyName( ModelKey_t eKey )
{
	return s_ModelKeys[ eKey ].GetString();
}

ModelArena_t *Model_CreateArena()
{
	return reinterpret_cast< ModelArena_t * >( new CKV3Arena );
}

void Model_DestroyArena( ModelArena_t *pArena )
{
	delete Arena( pArena );
}

void Model_ClearArena( ModelArena_t *pArena )
{
	Arena( pArena )->Clear();
}

ModelValue_t *Model_Root( ModelArena_t *pArena )
{
	return Value( Arena( pArena )->Root() );
}

ModelValue_t *Model_Member( ModelValue_t *pTable, ModelKey_t eKey )
{
	return Value( KV( pTable )->FindOrCreateMember( s_ModelKeys[ eKey ] ) );
}

ModelValue_t *Model_MemberByName( ModelValue_t *pTable, const char *pszName )
{
	return Value( KV( pTable )->FindOrCreateMember( CKV3MemberName( pszName, V_strlen( pszName ) ) ) );
}

ModelValue_t *Model_Append( ModelValue_t *pArray )
{
	return Value( KV( pArray )->ArrayAddElementToTail() );
}

void Model_SetTable( ModelValue_t *pValue )
{
	KV( pValue )->SetToEmptyTable();
}

void Model_SetArray( ModelValue_t *pValue )
{
	KV( pValue )->SetToEmptyKV3Array();
}

void Model_SetNull( ModelValue_t *pValue )
{
	KV( pValue )->SetToNull();
}

void Model_SetString( ModelValue_t *pValue, const char *pszString, int nLength )
{
	KeyValues3 *pKV = KV( pValue );
	CKV3Arena *pArena = pKV->GetContext();

	// A slice is copied to terminate it
	CBufferStringN< 256 > sSlice;

	if ( nLength >= 0 )
	{
		sSlice.Set( pszString, nLength );
		pszString = sSlice.Get();
	}

	// Repeated names, types and paths share one copy in the arena symbol table; it keeps no empty
	// string and allocates in 2048-byte pages, long text such as scripts goes to the heap
	if ( pArena && *pszString && V_strlen( pszString ) < 1024 )
		pKV->SetStringExternal( pArena->AllocString( pszString ) );
	else
		pKV->SetString( pszString );
}

void Model_SetInt( ModelValue_t *pValue, int64 nValue )
{
	KV( pValue )->SetInt64( nValue );
}

void Model_SetUInt( ModelValue_t *pValue, uint64 nValue )
{
	KV( pValue )->SetUInt64( nValue );
}

void Model_SetBool( ModelValue_t *pValue, bool bValue )
{
	KV( pValue )->SetBool( bValue );
}

// Deep copy, strings are interned in the destination arena
void Model_Copy( ModelValue_t *pDest, const ModelValue_t *pSource )
{
	const KeyValues3 *pKV = KV( pSource );

	switch ( pKV->GetType() )
	{
		case KV3_TYPE_TABLE:
		{
			Model_SetTable( pDest );

			for ( KV3MemberId_t id = 0; id < pKV->GetMemberCount(); ++id )
				Model_Copy( Model_MemberByName( pDest, pKV->GetMemberName( id ) ), Value( pKV->GetMember( id ) ) );

			break;
		}

		case KV3_TYPE_ARRAY:
		{
			Model_SetArray( pDest );

			for ( int i = 0; i < pKV->GetArrayElementCount(); ++i )
				Model_Copy( Model_Append( pDest ), Value( pKV->GetArrayElement( i ) ) );

			break;
		}

		case KV3_TYPE_STRING: Model_SetString( pDest, pKV->GetString() ); break;
		case KV3_TYPE_BOOL: Model_SetBool( pDest, pKV->GetBool() ); break;
		case KV3_TYPE_INT: Model_SetInt( pDest, pKV->GetInt64() ); break;
		case KV3_TYPE_UINT: Model_SetUInt( pDest, pKV->GetUInt64() ); break;
		case KV3_TYPE_DOUBLE: KV( pDest )->SetDouble( pKV->GetDouble() ); break;
		default: Model_SetNull( pDest ); break;
	}
}

void Model_SetMemberString( ModelValue_t *pTable, ModelKey_t eKey, const char *pszString )
{
	Model_SetString( Model_Member( pTable, eKey ), pszString );
}

void Model_SetMemberInt( ModelValue_t *pTable, ModelKey_t eKey, int64 nValue )
{
	Model_SetInt( Model_Member( pTable, eKey ), nValue );
}

void Model_SetMemberUInt( ModelValue_t *pTable, ModelKey_t eKey, uint64 nValue )
{
	Model_SetUInt( Model_Member( pTable, eKey ), nValue );
}

void Model_SetMemberBool( ModelValue_t *pTable, ModelKey_t eKey, bool bValue )
{
	Model_SetBool( Model_Member( pTable, eKey ), bValue );
}

ModelValue_t *Model_MemberTable( ModelValue_t *pTable, ModelKey_t eKey )
{
	ModelValue_t *pMember = Model_Member( pTable, eKey );

	if ( !KV( pMember )->IsTable() )
		Model_SetTable( pMember );

	return pMember;
}

ModelValue_t *Model_MemberArray( ModelValue_t *pTable, ModelKey_t eKey )
{
	ModelValue_t *pMember = Model_Member( pTable, eKey );

	if ( !KV( pMember )->IsKV3Array() )
		Model_SetArray( pMember );

	return pMember;
}

bool Model_IsNull( const ModelValue_t *pValue )
{
	return !pValue || KV( pValue )->IsNull();
}

bool Model_IsTable( const ModelValue_t *pValue )
{
	return pValue && KV( pValue )->IsTable();
}

bool Model_IsArray( const ModelValue_t *pValue )
{
	return pValue && KV( pValue )->IsArray();
}

bool Model_IsString( const ModelValue_t *pValue )
{
	return pValue && KV( pValue )->IsString();
}

const ModelValue_t *Model_Find( const ModelValue_t *pTable, ModelKey_t eKey )
{
	return pTable ? Value( KV( pTable )->FindMember( s_ModelKeys[ eKey ] ) ) : nullptr;
}

const ModelValue_t *Model_FindByName( const ModelValue_t *pTable, const char *pszName )
{
	return pTable ? Value( KV( pTable )->FindMember( CKV3MemberName( pszName, V_strlen( pszName ) ) ) ) : nullptr;
}

const char *Model_GetString( const ModelValue_t *pValue, const char *pszDefault )
{
	return pValue ? KV( pValue )->GetString( pszDefault ) : pszDefault;
}

int64 Model_GetInt( const ModelValue_t *pValue, int64 nDefault )
{
	return pValue ? KV( pValue )->GetInt64( nDefault ) : nDefault;
}

uint64 Model_GetUInt( const ModelValue_t *pValue, uint64 nDefault )
{
	return pValue ? KV( pValue )->GetUInt64( nDefault ) : nDefault;
}

bool Model_GetBool( const ModelValue_t *pValue, bool bDefault )
{
	return pValue ? KV( pValue )->GetBool( bDefault ) : bDefault;
}

int Model_Count( const ModelValue_t *pArray )
{
	return pArray && KV( pArray )->IsArray() ? KV( pArray )->GetArrayElementCount() : 0;
}

const ModelValue_t *Model_At( const ModelValue_t *pArray, int nIndex )
{
	return Value( KV( pArray )->GetArrayElement( nIndex ) );
}

int Model_MemberCount( const ModelValue_t *pTable )
{
	return pTable && KV( pTable )->IsTable() ? KV( pTable )->GetMemberCount() : 0;
}

const char *Model_MemberNameAt( const ModelValue_t *pTable, int nIndex )
{
	return KV( pTable )->GetMemberName( nIndex );
}

const ModelValue_t *Model_MemberAt( const ModelValue_t *pTable, int nIndex )
{
	return Value( KV( pTable )->GetMember( nIndex ) );
}

const char *Model_GetMemberString( const ModelValue_t *pTable, ModelKey_t eKey, const char *pszDefault )
{
	return Model_GetString( Model_Find( pTable, eKey ), pszDefault );
}

int64 Model_GetMemberInt( const ModelValue_t *pTable, ModelKey_t eKey, int64 nDefault )
{
	return Model_GetInt( Model_Find( pTable, eKey ), nDefault );
}

uint64 Model_GetMemberUInt( const ModelValue_t *pTable, ModelKey_t eKey, uint64 nDefault )
{
	return Model_GetUInt( Model_Find( pTable, eKey ), nDefault );
}

bool Model_GetMemberBool( const ModelValue_t *pTable, ModelKey_t eKey, bool bDefault )
{
	return Model_GetBool( Model_Find( pTable, eKey ), bDefault );
}

void Model_GetMemberStrings( const ModelValue_t *pTable, ModelKey_t eKey, CUtlLeanVector< CUtlString > &strings )
{
	const ModelValue_t *pArray = Model_Find( pTable, eKey );

	for ( int i = 0; i < Model_Count( pArray ); ++i )
		strings.AddToTail( CUtlString( Model_GetString( Model_At( pArray, i ) ) ) );
}

bool Model_LoadMemory( ModelValue_t *pRoot, const void *pData, int nSize, const char *pszName, CBufferString &sError )
{
	CUtlString sCodecError;

	if ( KV3Codec_Load( KV( pRoot ), &sCodecError, pData, nSize, pszName ) )
		return true;

	sError.Set( sCodecError.Get() );

	return false;
}

bool Model_LoadFile( ModelValue_t *pRoot, const char *pszPath, CBufferString &sError )
{
	CUtlBuffer data;

	if ( !File_Read( pszPath, data ) )
	{
		sError.Format( "%s: cannot open the file", pszPath );
		return false;
	}

	return Model_LoadMemory( pRoot, data.Base(), data.TellPut(), pszPath, sError );
}

void Model_SaveText( const ModelValue_t *pRoot, CUtlBuffer &output )
{
	KV3Codec_SaveText( KV( pRoot ), output );
}

void Model_SaveBinary( const ModelValue_t *pRoot, CUtlBuffer &output )
{
	KV3Codec_SaveBinary( KV( pRoot ), output );
}
