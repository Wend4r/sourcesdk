#include "schemacompiler.h"

namespace
{

void NonEmptyStrings( const ModelValue_t *pTable, ModelKey_t eKey, CSchemaStringList &strings )
{
	const ModelValue_t *pArray = Model_Find( pTable, eKey );

	for ( int i = 0; i < Model_Count( pArray ); ++i )
	{
		CSchemaPathString sTrimmed;

		if ( *String_Trim( Model_GetString( Model_At( pArray, i ) ), -1, sTrimmed ) )
			strings.AddToTail( CUtlString( sTrimmed.Get() ) );
	}
}

// A project path, absolute against the project file directory
CUtlString ProjectPath( const char *pszPath, const char *pszBase )
{
	CSchemaPathString sPath;

	return CUtlString( Path_Absolute( pszPath, pszBase, sPath ) );
}

} // namespace

const char *SchemaProject_t::ProjectId( CBufferString &sId ) const
{
	return Name_ToIdentifier( m_sProject.Get(), sId );
}

bool Project_Load( const char *pszPath, SchemaProject_t &project )
{
	ModelArena_t *pArena = Model_CreateArena();
	ModelValue_t *pRoot = Model_Root( pArena );

	CBufferStringN< 512 > sError;
	bool bSuccess = Model_LoadFile( pRoot, pszPath, sError );

	if ( !bSuccess )
	{
		g_Diagnostics.Error( SC_PROJECT, {}, "%s", sError.Get() );
		Model_DestroyArena( pArena );
		return false;
	}

	const SchemaDiagLocation_t location{ CUtlString( pszPath ), 0, 0 };
	const int64 nVersion = Model_GetMemberInt( pRoot, MODEL_KEY_VERSION, -1 );

	if ( nVersion != SCHEMACOMPILER_PROJECT_VERSION )
	{
		g_Diagnostics.Error( SC_PROJECT, location, "project version %lld is not supported, expected %d; reconfigure the build", static_cast< long long >( nVersion ), SCHEMACOMPILER_PROJECT_VERSION );
		Model_DestroyArena( pArena );
		return false;
	}

	CSchemaPathString sPath, sBase;

	project.m_sPath = Path_Normalize( pszPath, sPath );
	Path_Directory( sPath.Get(), sBase );

	const char *pszBase = sBase.Get();

	project.m_sProject = Model_GetMemberString( pRoot, MODEL_KEY_PROJECT );
	project.m_sTarget = Model_GetMemberString( pRoot, MODEL_KEY_TARGET, project.m_sProject.Get() );
	project.m_sConfig = Model_GetMemberString( pRoot, MODEL_KEY_CONFIG );
	project.m_sScope = Model_GetMemberString( pRoot, MODEL_KEY_SCOPE, "MODULE" );
	project.m_sPlatform = Model_GetMemberString( pRoot, MODEL_KEY_PLATFORM );
	project.m_sStandard = Model_GetMemberString( pRoot, MODEL_KEY_STANDARD, "17" );
	project.m_eCompiler = !V_strcmp( Model_GetMemberString( pRoot, MODEL_KEY_COMPILER, "gnu" ), "msvc" ) ? SCHEMA_COMPILER_MSVC : SCHEMA_COMPILER_GNU;

	NonEmptyStrings( pRoot, MODEL_KEY_DEFINES, project.m_Defines );
	NonEmptyStrings( pRoot, MODEL_KEY_OPTIONS, project.m_Options );
	NonEmptyStrings( pRoot, MODEL_KEY_IMPORT_MODULES, project.m_ImportModules );
	NonEmptyStrings( pRoot, MODEL_KEY_PRE_INCLUDE, project.m_PreIncludes );
	NonEmptyStrings( pRoot, MODEL_KEY_TAG_HEADERS, project.m_TagHeaders );

	CSchemaStringList includes;

	NonEmptyStrings( pRoot, MODEL_KEY_INCLUDES, includes );

	for ( const CUtlString &sInclude : includes )
		project.m_Includes.AddToTail( ProjectPath( sInclude.Get(), pszBase ) );

	CSchemaPathString sPch, sDefaultCache;

	project.m_sPch = String_Trim( Model_GetMemberString( pRoot, MODEL_KEY_PCH ), -1, sPch );
	project.m_sAtomicConfig = ProjectPath( Model_GetMemberString( pRoot, MODEL_KEY_ATOMIC_CONFIG ), pszBase );
	project.m_sOutputDir = ProjectPath( Model_GetMemberString( pRoot, MODEL_KEY_OUTPUT_DIR, pszBase ), pszBase );
	project.m_sRegistration = ProjectPath( Model_GetMemberString( pRoot, MODEL_KEY_REGISTRATION ), pszBase );

	sDefaultCache.Format( "%s/schema_cache.kv3", project.m_sOutputDir.Get() );

	project.m_sCacheFile = ProjectPath( Model_GetMemberString( pRoot, MODEL_KEY_CACHE_FILE, sDefaultCache.Get() ), pszBase );
	project.m_nUnityBatchSize = static_cast< int >( Model_GetMemberInt( pRoot, MODEL_KEY_UNITY_BATCH_SIZE, 0 ) );
	project.m_bEmitCodegen = Model_GetMemberBool( pRoot, MODEL_KEY_EMIT_CODEGEN, true );

	CBufferStringN< 128 > sId;

	if ( project.m_sProject.IsEmpty() || V_strcmp( Name_ToIdentifier( project.m_sProject.Get(), sId ), project.m_sProject.Get() ) )
	{
		g_Diagnostics.Error( SC_PROJECT, location, "project name \"%s\" must be a C identifier", project.m_sProject.Get() );
		bSuccess = false;
	}

	if ( project.m_sScope != "MODULE" && project.m_sScope != "GLOBAL" )
	{
		g_Diagnostics.Error( SC_PROJECT, location, "scope \"%s\" must be MODULE or GLOBAL", project.m_sScope.Get() );
		bSuccess = false;
	}

	if ( !*Model_GetMemberString( pRoot, MODEL_KEY_REGISTRATION ) )
	{
		g_Diagnostics.Error( SC_PROJECT, location, "no registration output" );
		bSuccess = false;
	}

	const ModelValue_t *pInputs = Model_Find( pRoot, MODEL_KEY_INPUTS );

	for ( int i = 0; i < Model_Count( pInputs ); ++i )
	{
		const ModelValue_t *pInput = Model_At( pInputs, i );

		SchemaInput_t &input = project.m_Inputs[ project.m_Inputs.AddToTail() ];

		input.m_sInput = ProjectPath( Model_GetMemberString( pInput, MODEL_KEY_INPUT ), pszBase );
		input.m_sOutput = ProjectPath( Model_GetMemberString( pInput, MODEL_KEY_OUTPUT ), pszBase );
		input.m_sRelative = Model_GetMemberString( pInput, MODEL_KEY_NAME, input.m_sInput.Get() );
		input.m_bIsCpp = Model_GetMemberBool( pInput, MODEL_KEY_IS_CPP );

		CBufferStringN< 256 > sInputId;

		input.m_sId = Name_ToIdentifier( input.m_sRelative.Get(), sInputId );
	}

	Model_DestroyArena( pArena );

	return bSuccess;
}

//--------------------------------------------------------------------------------------------------
// atomic_types.kv3
//--------------------------------------------------------------------------------------------------
namespace
{

struct SchemaAtomicKindName_t
{
	SchemaAtomicKind_t m_eKind;
	const char *m_pszName;
};

const SchemaAtomicKindName_t s_AtomicKindNames[] =
{
	{ SCHEMA_ATOMIC_KIND_PLAIN, "plain" },
	{ SCHEMA_ATOMIC_KIND_T, "t" },
	{ SCHEMA_ATOMIC_KIND_TT, "tt" },
	{ SCHEMA_ATOMIC_KIND_I, "i" },
	{ SCHEMA_ATOMIC_KIND_COLLECTION, "collection" },
	{ SCHEMA_ATOMIC_KIND_COLLECTION_FIXED, "collection_fixed" },
	{ SCHEMA_ATOMIC_KIND_TRANSPARENT, "transparent" },
};

} // namespace

const char *AtomicKind_Name( SchemaAtomicKind_t eKind )
{
	for ( const SchemaAtomicKindName_t &kind : s_AtomicKindNames )
	{
		if ( kind.m_eKind == eKind )
			return kind.m_pszName;
	}

	return "plain";
}

bool AtomicKind_Parse( const char *pszName, SchemaAtomicKind_t &eKind )
{
	for ( const SchemaAtomicKindName_t &kind : s_AtomicKindNames )
	{
		if ( !V_strcmp( pszName, kind.m_pszName ) )
		{
			eKind = kind.m_eKind;
			return true;
		}
	}

	return false;
}

bool AtomicConfig_Load( const char *pszPath, SchemaAtomicMap_t &atomics )
{
	ModelArena_t *pArena = Model_CreateArena();
	ModelValue_t *pRoot = Model_Root( pArena );

	CBufferStringN< 512 > sError;

	if ( !Model_LoadFile( pRoot, pszPath, sError ) )
	{
		g_Diagnostics.Error( SC_PROJECT, {}, "%s", sError.Get() );
		Model_DestroyArena( pArena );
		return false;
	}

	bool bSuccess = true;
	const SchemaDiagLocation_t location{ CUtlString( pszPath ), 0, 0 };
	const ModelValue_t *pAtomics = Model_Find( pRoot, MODEL_KEY_ATOMICS );

	for ( int i = 0; i < Model_Count( pAtomics ); ++i )
	{
		const ModelValue_t *pAtomic = Model_At( pAtomics, i );

		SchemaAtomicDesc_t atomic;

		atomic.m_sName = Model_GetMemberString( pAtomic, MODEL_KEY_NAME );
		atomic.m_sManipulator = Model_GetMemberString( pAtomic, MODEL_KEY_MANIPULATOR );

		const char *pszKind = Model_GetMemberString( pAtomic, MODEL_KEY_KIND, "plain" );

		if ( atomic.m_sName.IsEmpty() || !AtomicKind_Parse( pszKind, atomic.m_eKind ) )
		{
			g_Diagnostics.Error( SC_PROJECT, location, "atomic %d (\"%s\") has no name or an unknown kind \"%s\"", i, atomic.m_sName.Get(), pszKind );
			bSuccess = false;
			continue;
		}

		if ( ( atomic.m_eKind == SCHEMA_ATOMIC_KIND_COLLECTION || atomic.m_eKind == SCHEMA_ATOMIC_KIND_COLLECTION_FIXED ) && atomic.m_sManipulator.IsEmpty() )
		{
			g_Diagnostics.Error( SC_PROJECT, location, "collection atomic \"%s\" needs a manipulator", atomic.m_sName.Get() );
			bSuccess = false;
			continue;
		}

		atomics.InsertOrReplace( atomic.m_sName, atomic );
	}

	Model_DestroyArena( pArena );

	return bSuccess;
}
