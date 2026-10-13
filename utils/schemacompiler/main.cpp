//--------------------------------------------------------------------------------------------------
// schemacompiler --project <schema_project.kv3> [--stamp <file>] [--depfile <file>] [-j <threads>]
//                [--stats] [--dump-kv3 <file>] [--dump-only] [--no-cache] [--resource-dir <dir>]
//
// Parses the inputs of a project with Clang, writes one <name>_schema.cpp per input and the
// registration unit, and records the headers read in the depfile
//--------------------------------------------------------------------------------------------------
#include "codegen.h"
#include "frontend.h"
#include "schemacompiler.h"

#include <cstdio>
#include <cstdlib>

namespace
{

struct Options_t
{
	const char *m_pszProject = "";
	const char *m_pszConfig = "";
	const char *m_pszStamp = "";
	const char *m_pszDepFile = "";
	const char *m_pszDump = "";
	const char *m_pszResourceDir = "";
	int m_nThreads = 0;
	bool m_bStats = false;
	bool m_bDumpOnly = false;
	bool m_bNoCache = false;
	bool m_bVerbose = false;
};

void PrintUsage()
{
	fputs(
		"usage: schemacompiler --project <schema_project.kv3> [options]\n"
		"  --config <name>        configuration, informational\n"
		"  --stamp <file>         file touched after a successful run\n"
		"  --depfile <file>       Makefile-style depfile of every header read\n"
		"  -j <threads>           parser threads, default: hardware concurrency\n"
		"  --stats                print timing and peak memory\n"
		"  --dump-kv3 <file>      write the parsed model as KV3 text\n"
		"  --dump-only            parse and dump, write no code\n"
		"  --no-cache             ignore and do not write the incremental cache\n"
		"  --resource-dir <dir>   Clang builtin headers\n"
		"  --verbose              list parsed and cached inputs\n"
		"  --version              print the version\n",
		stdout );
}

bool ParseOptions( int argc, char **argv, Options_t &options )
{
	for ( int i = 1; i < argc; ++i )
	{
		const char *pszArg = argv[ i ];

		auto Is = [ & ]( const char *pszName ) { return !V_strcmp( pszArg, pszName ); };

		// Options with a value take the next argument
		const char **ppszValue = nullptr;

		if ( Is( "--project" ) || Is( "-schproj" ) )
			ppszValue = &options.m_pszProject;
		else if ( Is( "--config" ) || Is( "-config" ) )
			ppszValue = &options.m_pszConfig;
		else if ( Is( "--stamp" ) || Is( "-sentinel" ) )
			ppszValue = &options.m_pszStamp;
		else if ( Is( "--depfile" ) )
			ppszValue = &options.m_pszDepFile;
		else if ( Is( "--dump-kv3" ) )
			ppszValue = &options.m_pszDump;
		else if ( Is( "--resource-dir" ) )
			ppszValue = &options.m_pszResourceDir;

		if ( ppszValue )
		{
			if ( i + 1 >= argc )
			{
				g_Diagnostics.Error( SC_PROJECT, {}, "missing value of %s", pszArg );
				return false;
			}

			*ppszValue = argv[ ++i ];
		}
		else if ( Is( "-j" ) )
		{
			if ( i + 1 >= argc )
			{
				g_Diagnostics.Error( SC_PROJECT, {}, "missing value of -j" );
				return false;
			}

			options.m_nThreads = V_atoi( argv[ ++i ] );
		}
		else if ( String_StartsWith( pszArg, "-j" ) )
		{
			options.m_nThreads = V_atoi( pszArg + 2 );
		}
		else if ( Is( "--stats" ) )
		{
			options.m_bStats = true;
		}
		else if ( Is( "--dump-only" ) )
		{
			options.m_bDumpOnly = true;
		}
		else if ( Is( "--no-cache" ) )
		{
			options.m_bNoCache = true;
		}
		else if ( Is( "--verbose" ) )
		{
			options.m_bVerbose = true;
		}
		else if ( Is( "--version" ) )
		{
			puts( "schemacompiler " SCHEMACOMPILER_VERSION );
			exit( 0 );
		}
		else if ( Is( "--help" ) || Is( "-h" ) )
		{
			PrintUsage();
			exit( 0 );
		}
		else
		{
			g_Diagnostics.Error( SC_PROJECT, {}, "unknown option %s", pszArg );
			return false;
		}
	}

	if ( !*options.m_pszProject )
	{
		g_Diagnostics.Error( SC_PROJECT, {}, "no --project" );
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------------------
// Incremental cache: per input the hashes of every file its unit read and its model
//--------------------------------------------------------------------------------------------------
enum
{
	SCHEMA_CACHE_VERSION = 2,
};

// Everything that changes how inputs parse; a change invalidates the whole cache
const char *CacheKey( const SchemaProject_t &project, const CUtlBuffer &atomicConfig, const char *pszResourceDir, CBufferString &sKey )
{
	CUtlBuffer key;

	auto Add = [ &key ]( const char *pszText )
	{
		key.Put( pszText, V_strlen( pszText ) );
		key.Put( "\n", 1 );
	};

	CBufferStringN< 64 > sVersions;

	sVersions.Format( "%s %d %d", SCHEMACOMPILER_VERSION, SCHEMACOMPILER_BINDING_VERSION, project.m_eCompiler );

	Add( sVersions.Get() );
	Add( project.m_sStandard.Get() );
	Add( pszResourceDir );

	for ( const CSchemaStringList *pList : { &project.m_Defines, &project.m_Includes, &project.m_Options, &project.m_PreIncludes, &project.m_TagHeaders } )
	{
		for ( const CUtlString &sItem : *pList )
			Add( sItem.Get() );

		Add( "--" );
	}

	Add( project.m_sPch.Get() );
	key.Put( atomicConfig.Base(), atomicConfig.TellPut() );

	sKey.Format( "%016llx", static_cast< unsigned long long >( Frontend_HashString( static_cast< const char * >( key.Base() ), key.TellPut() ) ) );

	return sKey.Get();
}

struct DepStatus_t
{
	uint64 m_nModTime = 0;
	uint64 m_nSize = 0;
	uint64 m_nHash = 0;
	bool m_bValid = false;
};

class CDepChecker
{
public:
	// Current state of a file, hashed once per run
	const DepStatus_t &Status( const CUtlString &sPath )
	{
		auto i = m_Status.Find( sPath );

		if ( m_Status.IsValidIndex( i ) )
			return m_Status[ i ];

		DepStatus_t status;

		status.m_bValid = Frontend_FileStatus( sPath.Get(), status.m_nModTime, status.m_nSize ) && Frontend_HashFile( sPath.Get(), status.m_nHash );

		return m_Status[ m_Status.Insert( sPath, status ) ];
	}

	// Unchanged when the hash matches; the time and size are only a shortcut
	bool Unchanged( const ModelValue_t *pDep )
	{
		const CUtlString sPath( Model_GetMemberString( pDep, MODEL_KEY_PATH ) );
		const uint64 nModTime = Model_GetMemberUInt( pDep, MODEL_KEY_MTIME ), nSize = Model_GetMemberUInt( pDep, MODEL_KEY_SIZE ), nHash = Model_GetMemberUInt( pDep, MODEL_KEY_HASH );

		auto i = m_Status.Find( sPath );

		if ( !m_Status.IsValidIndex( i ) )
		{
			DepStatus_t status;

			if ( Frontend_FileStatus( sPath.Get(), status.m_nModTime, status.m_nSize ) )
			{
				if ( status.m_nModTime == nModTime && status.m_nSize == nSize )
				{
					status.m_nHash = nHash;
					status.m_bValid = true;
				}
				else
				{
					status.m_bValid = Frontend_HashFile( sPath.Get(), status.m_nHash );
				}
			}

			i = m_Status.Insert( sPath, status );
		}

		return m_Status[ i ].m_bValid && m_Status[ i ].m_nHash == nHash;
	}

	void WriteDeps( ModelValue_t *pArray, const CSchemaStringList &deps )
	{
		for ( const CUtlString &sDep : deps )
		{
			const DepStatus_t &status = Status( sDep );
			ModelValue_t *pDep = Model_Append( pArray );

			Model_SetTable( pDep );
			Model_SetMemberString( pDep, MODEL_KEY_PATH, sDep.Get() );
			Model_SetMemberUInt( pDep, MODEL_KEY_MTIME, status.m_nModTime );
			Model_SetMemberUInt( pDep, MODEL_KEY_SIZE, status.m_nSize );
			Model_SetMemberUInt( pDep, MODEL_KEY_HASH, status.m_nHash );
		}
	}

private:
	CSchemaStringMap< DepStatus_t > m_Status;
};

void DepPaths( const ModelValue_t *pDeps, CSchemaStringList &deps )
{
	for ( int i = 0; i < Model_Count( pDeps ); ++i )
		deps.AddToTail( CUtlString( Model_GetMemberString( Model_At( pDeps, i ), MODEL_KEY_PATH ) ) );
}

bool AllUnchanged( CDepChecker &checker, const ModelValue_t *pDeps )
{
	for ( int i = 0; i < Model_Count( pDeps ); ++i )
	{
		if ( !checker.Unchanged( Model_At( pDeps, i ) ) )
			return false;
	}

	return true;
}

// Depfile: "<stamp>: <dep> <dep> ...", spaces escaped
void AppendDep( CUtlBuffer &output, const char *pszPath )
{
	for ( const char *p = pszPath; *p; ++p )
	{
		if ( *p == ' ' || *p == '#' )
			output.Put( "\\", 1 );
		else if ( *p == '$' )
			output.Put( "$", 1 );

		output.Put( p, 1 );
	}
}

void WriteDump( const char *pszPath, const SchemaProject_t &project, const CUtlLeanVector< int > &sortedInputs, const CUtlLeanVector< const ModelValue_t * > &models )
{
	ModelArena_t *pArena = Model_CreateArena();
	ModelValue_t *pDump = Model_Root( pArena );

	Model_SetTable( pDump );
	Model_SetMemberString( pDump, MODEL_KEY_PROJECT, project.m_sProject.Get() );

	ModelValue_t *pInputs = Model_MemberArray( pDump, MODEL_KEY_INPUTS );

	for ( int nInput : sortedInputs )
	{
		if ( !models[ nInput ] )
			continue;

		ModelValue_t *pInput = Model_Append( pInputs );

		Model_Copy( pInput, models[ nInput ] );

		// Scripts are long and the same for every input
		Model_SetArray( Model_Member( pInput, MODEL_KEY_CODEGEN_TAGS ) );
	}

	CUtlBuffer text;

	Model_SaveText( pDump, text );

	if ( !File_WriteIfChanged( pszPath, text ) )
		g_Diagnostics.Error( SC_IO, { CUtlString( pszPath ), 0, 0 }, "cannot write the dump" );

	Model_DestroyArena( pArena );
}

} // namespace

int main( int argc, char **argv )
{
	const double flStart = Frontend_Seconds();

	Options_t options;

	if ( !ParseOptions( argc, argv, options ) )
	{
		PrintUsage();
		return 2;
	}

	SchemaProject_t project;

	if ( !Project_Load( options.m_pszProject, project ) )
		return 1;

	g_Diagnostics.SetMsvcFormat( project.m_eCompiler == SCHEMA_COMPILER_MSVC );

	SchemaAtomicMap_t atomics;
	CUtlBuffer atomicConfig;

	if ( !File_Read( project.m_sAtomicConfig.Get(), atomicConfig ) || !AtomicConfig_Load( project.m_sAtomicConfig.Get(), atomics ) )
	{
		g_Diagnostics.Error( SC_PROJECT, { project.m_sPath, 0, 0 }, "cannot load the atomic types \"%s\"", project.m_sAtomicConfig.Get() );
		return 1;
	}

	FrontendOptions_t frontendOptions;

	if ( *options.m_pszResourceDir )
	{
		CSchemaPathString sResourceDir;

		frontendOptions.m_sResourceDir = Path_Normalize( options.m_pszResourceDir, sResourceDir );
	}
	else
	{
		frontendOptions.m_sResourceDir = Frontend_DefaultResourceDir( argv[ 0 ] );
	}

	frontendOptions.m_nThreads = options.m_nThreads;
	frontendOptions.m_bVerbose = options.m_bVerbose;

	CBufferStringN< 32 > sCacheKey;

	CacheKey( project, atomicConfig, frontendOptions.m_sResourceDir.Get(), sCacheKey );

	// Inputs in path order
	CUtlLeanVector< int > sortedInputs;

	for ( int i = 0; i < project.m_Inputs.Count(); ++i )
		sortedInputs.AddToTail( i );

	Vector_Sort( sortedInputs, [ &project ]( int a, int b ) { return V_strcmp( project.m_Inputs[ a ].m_sRelative.Get(), project.m_Inputs[ b ].m_sRelative.Get() ) < 0; } );

	// Previous run
	ModelArena_t *pCacheArena = Model_CreateArena();
	ModelValue_t *pCache = Model_Root( pCacheArena );
	bool bCacheValid = false;

	if ( !options.m_bNoCache && File_Exists( project.m_sCacheFile.Get() ) )
	{
		CBufferStringN< 512 > sError;

		bCacheValid = Model_LoadFile( pCache, project.m_sCacheFile.Get(), sError ) && Model_GetMemberInt( pCache, MODEL_KEY_VERSION ) == SCHEMA_CACHE_VERSION && !V_strcmp( sCacheKey.Get(), Model_GetMemberString( pCache, MODEL_KEY_KEY ) );
	}

	CDepChecker depChecker;

	// Prefix
	FrontendPrefix_t prefix;
	const bool bHasPrefix = project.m_PreIncludes.Count() > 0 || project.m_TagHeaders.Count() > 0 || !project.m_sPch.IsEmpty();

	if ( bHasPrefix )
	{
		CSchemaPathString sPchFile;

		sPchFile.Format( "%s/schema_prefix%s%s.pch", project.m_sOutputDir.Get(), project.m_sConfig.IsEmpty() ? "" : ".", project.m_sConfig.Get() );

		const ModelValue_t *pCachedPrefix = bCacheValid ? Model_Find( pCache, MODEL_KEY_PREFIX ) : nullptr;
		const bool bPrefixValid = pCachedPrefix && File_Exists( sPchFile.Get() ) && !V_strcmp( sPchFile.Get(), Model_GetMemberString( pCachedPrefix, MODEL_KEY_PCH ) ) && AllUnchanged( depChecker, Model_Find( pCachedPrefix, MODEL_KEY_DEPS ) );

		if ( bPrefixValid )
		{
			prefix.m_sPchFile = sPchFile.Get();
			prefix.m_pArena = Model_CreateArena();
			prefix.m_pMarkup = Model_Root( prefix.m_pArena );

			DepPaths( Model_Find( pCachedPrefix, MODEL_KEY_DEPS ), prefix.m_Deps );
			Model_Copy( prefix.m_pMarkup, Model_Find( pCachedPrefix, MODEL_KEY_MODEL ) );
		}
		else
		{
			// Inputs parsed against an old prefix are stale too
			bCacheValid = false;

			if ( !Frontend_BuildPrefix( project, frontendOptions, sPchFile.Get(), prefix ) )
				return 1;
		}

		// A PCH hides the markup of its headers; with inputs among them the prefix is included as text
		for ( const SchemaInput_t &input : project.m_Inputs )
		{
			if ( !StringList_Contains( prefix.m_Deps, input.m_sInput.Get() ) )
				continue;

			if ( options.m_bVerbose )
				printf( "schemacompiler: %s is part of the prefix, the prefix is included as text\n", input.m_sRelative.Get() );

			prefix.m_bTextual = true;

			Vector_Copy( prefix.m_TextualIncludes, project.m_PreIncludes );
			Vector_Append( prefix.m_TextualIncludes, project.m_TagHeaders );

			if ( !project.m_sPch.IsEmpty() )
				prefix.m_TextualIncludes.AddToTail( project.m_sPch );

			prefix.m_sPchFile.Clear();
			prefix.m_Deps.RemoveAll();
			Model_SetTable( prefix.m_pMarkup );
			break;
		}
	}

	const double flPrefixDone = Frontend_Seconds();

	// Inputs whose cached model is still valid
	CUtlLeanVector< const ModelValue_t * > models;
	CUtlLeanVector< CSchemaStringList > inputDeps;
	CUtlLeanVector< int > stale;

	models.SetCount( project.m_Inputs.Count() );
	inputDeps.SetCount( project.m_Inputs.Count() );

	for ( int i = 0; i < models.Count(); ++i )
		models[ i ] = nullptr;

	CSchemaStringMap< const ModelValue_t * > cachedInputs;

	if ( bCacheValid )
	{
		const ModelValue_t *pInputs = Model_Find( pCache, MODEL_KEY_INPUTS );

		for ( int i = 0; i < Model_Count( pInputs ); ++i )
			cachedInputs.InsertOrReplace( CUtlString( Model_GetMemberString( Model_At( pInputs, i ), MODEL_KEY_INPUT ) ), Model_At( pInputs, i ) );
	}

	for ( int nInput : sortedInputs )
	{
		auto i = cachedInputs.Find( project.m_Inputs[ nInput ].m_sInput );

		if ( cachedInputs.IsValidIndex( i ) && AllUnchanged( depChecker, Model_Find( cachedInputs[ i ], MODEL_KEY_DEPS ) ) )
		{
			models[ nInput ] = Model_Find( cachedInputs[ i ], MODEL_KEY_MODEL );
			DepPaths( Model_Find( cachedInputs[ i ], MODEL_KEY_DEPS ), inputDeps[ nInput ] );
		}
		else
		{
			stale.AddToTail( nInput );
		}
	}

	// Units: headers in unity batches, a .cpp alone
	CUtlLeanVector< FrontendUnit_t > units;

	for ( int nInput : stale )
	{
		const bool bBatch = project.m_nUnityBatchSize > 0 && !project.m_Inputs[ nInput ].m_bIsCpp;

		if ( bBatch && units.Count() > 0 && units.Tail().m_Inputs.Count() < project.m_nUnityBatchSize && !project.m_Inputs[ units.Tail().m_Inputs[ 0 ] ].m_bIsCpp )
		{
			units.Tail().m_Inputs.AddToTail( nInput );
			continue;
		}

		units[ units.AddToTail() ].m_Inputs.AddToTail( nInput );
	}

	FrontendArenas_t arenas;

	Frontend_ParseUnits( project, atomics, frontendOptions, prefix, units, arenas );

	const double flParseDone = Frontend_Seconds();

	bool bParsed = true;

	for ( FrontendUnit_t &unit : units )
	{
		bParsed &= unit.m_bSuccess;

		for ( int i = 0; i < unit.m_Inputs.Count(); ++i )
		{
			models[ unit.m_Inputs[ i ] ] = unit.m_Models.Count() > 0 ? unit.m_Models[ i ] : nullptr;
			Vector_Copy( inputDeps[ unit.m_Inputs[ i ] ], unit.m_Deps );

			if ( options.m_bVerbose )
				printf( "schemacompiler: parsed %s (%.3f s)\n", project.m_Inputs[ unit.m_Inputs[ i ] ].m_sRelative.Get(), unit.m_flSeconds );
		}
	}

	if ( options.m_bVerbose )
	{
		for ( int nInput : sortedInputs )
		{
			if ( stale.Find( nInput ) < 0 )
				printf( "schemacompiler: cached %s\n", project.m_Inputs[ nInput ].m_sRelative.Get() );
		}
	}

	// The dump comes first, it helps to read a failed run
	if ( *options.m_pszDump )
		WriteDump( options.m_pszDump, project, sortedInputs, models );

	if ( !bParsed || g_Diagnostics.ErrorCount() > 0 )
		return 1;

	int nWritten = 0;

	if ( !options.m_bDumpOnly )
	{
		CUtlLeanVector< CodegenOutput_t * > outputs;

		const bool bGenerated = Codegen_Generate( project, models, outputs ) && g_Diagnostics.ErrorCount() == 0;

		for ( const CodegenOutput_t *pOutput : outputs )
		{
			bool bWritten = false;

			if ( !bGenerated )
				break;

			if ( !File_WriteIfChanged( pOutput->m_sPath.Get(), pOutput->m_Text, &bWritten ) )
			{
				g_Diagnostics.Error( SC_IO, { pOutput->m_sPath, 0, 0 }, "cannot write the output" );
				break;
			}

			nWritten += bWritten ? 1 : 0;
		}

		for ( CodegenOutput_t *pOutput : outputs )
			delete pOutput;

		if ( g_Diagnostics.ErrorCount() > 0 )
			return 1;
	}

	const double flCodegenDone = Frontend_Seconds();

	// Cache of this run
	if ( !options.m_bNoCache )
	{
		ModelArena_t *pNewArena = Model_CreateArena();
		ModelValue_t *pNew = Model_Root( pNewArena );

		Model_SetTable( pNew );
		Model_SetMemberInt( pNew, MODEL_KEY_VERSION, SCHEMA_CACHE_VERSION );
		Model_SetMemberString( pNew, MODEL_KEY_KEY, sCacheKey.Get() );

		if ( bHasPrefix && !prefix.m_bTextual )
		{
			ModelValue_t *pPrefix = Model_MemberTable( pNew, MODEL_KEY_PREFIX );

			Model_SetMemberString( pPrefix, MODEL_KEY_PCH, prefix.m_sPchFile.Get() );
			depChecker.WriteDeps( Model_MemberArray( pPrefix, MODEL_KEY_DEPS ), prefix.m_Deps );
			Model_Copy( Model_Member( pPrefix, MODEL_KEY_MODEL ), prefix.m_pMarkup );
		}

		ModelValue_t *pInputs = Model_MemberArray( pNew, MODEL_KEY_INPUTS );

		for ( int nInput : sortedInputs )
		{
			if ( !models[ nInput ] )
				continue;

			ModelValue_t *pInput = Model_Append( pInputs );

			Model_SetTable( pInput );
			Model_SetMemberString( pInput, MODEL_KEY_INPUT, project.m_Inputs[ nInput ].m_sInput.Get() );
			depChecker.WriteDeps( Model_MemberArray( pInput, MODEL_KEY_DEPS ), inputDeps[ nInput ] );
			Model_Copy( Model_Member( pInput, MODEL_KEY_MODEL ), models[ nInput ] );
		}

		CUtlBuffer cache;

		Model_SaveBinary( pNew, cache );

		if ( !File_WriteIfChanged( project.m_sCacheFile.Get(), cache ) )
			g_Diagnostics.Warning( SC_IO, { project.m_sCacheFile, 0, 0 }, "cannot write the cache" );

		Model_DestroyArena( pNewArena );
	}

	// Depfile and stamp
	if ( *options.m_pszDepFile )
	{
		CSchemaStringList deps;

		deps.AddToTail( project.m_sPath );
		deps.AddToTail( project.m_sAtomicConfig );
		Vector_Append( deps, prefix.m_Deps );

		for ( const CSchemaStringList &inputDepList : inputDeps )
			Vector_Append( deps, inputDepList );

		StringList_SortUnique( deps );

		CUtlBuffer depFile;

		AppendDep( depFile, *options.m_pszStamp ? options.m_pszStamp : project.m_sRegistration.Get() );
		depFile.Put( ":", 1 );

		for ( const CUtlString &sDep : deps )
		{
			depFile.Put( " \\\n  ", 5 );
			AppendDep( depFile, sDep.Get() );
		}

		depFile.Put( "\n", 1 );

		if ( !File_WriteIfChanged( options.m_pszDepFile, depFile ) )
		{
			g_Diagnostics.Error( SC_IO, { CUtlString( options.m_pszDepFile ), 0, 0 }, "cannot write the depfile" );
			return 1;
		}
	}

	if ( *options.m_pszStamp && !File_Touch( options.m_pszStamp ) )
	{
		g_Diagnostics.Error( SC_IO, { CUtlString( options.m_pszStamp ), 0, 0 }, "cannot touch the stamp" );
		return 1;
	}

	if ( options.m_bStats )
	{
		printf( "schemacompiler: %s: %d inputs, %d parsed in %d units, %d cached, %d outputs written\n", project.m_sProject.Get(), project.m_Inputs.Count(), stale.Count(), units.Count(), project.m_Inputs.Count() - stale.Count(), nWritten );
		printf( "schemacompiler: prefix %.3f s, parse %.3f s, codegen %.3f s, total %.3f s, peak memory %.1f MiB\n", flPrefixDone - flStart, flParseDone - flPrefixDone, flCodegenDone - flParseDone, Frontend_Seconds() - flStart, Frontend_PeakMemory() / ( 1024.0 * 1024.0 ) );
	}

	Model_DestroyArena( pCacheArena );

	return 0;
}
