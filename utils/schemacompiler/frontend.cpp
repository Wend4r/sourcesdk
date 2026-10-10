// clang/Basic/LangOptions.h has an enumerator POSIX, the SDK platform macro of the target
#pragma push_macro( "POSIX" )
#undef POSIX

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/QualTypeNames.h>
#include <clang/AST/RecordLayout.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Lex/Lexer.h>
#include <clang/Lex/MacroArgs.h>
#include <clang/Lex/PPCallbacks.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/ThreadPool.h>
#include <llvm/Support/Threading.h>
#include <llvm/Support/VirtualFileSystem.h>
#include <llvm/Support/xxhash.h>

#pragma pop_macro( "POSIX" )

#if defined( _WIN32 )
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#endif

#include "frontend.h"

#ifndef SCHEMACOMPILER_CLANG_RESOURCE_DIR
#define SCHEMACOMPILER_CLANG_RESOURCE_DIR ""
#endif

using namespace clang;

namespace
{

// Names and spellings of one declaration, on the stack
using CSchemaNameString = CBufferStringN< 256 >;

const char *NormalizePath( llvm::StringRef sPath, CBufferString &sOut )
{
	CSchemaPathString sRaw;

	sRaw.Set( sPath.data(), static_cast< int >( sPath.size() ) );

	return Path_Normalize( sRaw.Get(), sOut );
}

// A normalized path that is kept, e.g. as a map key or a dependency
CUtlString StoredPath( llvm::StringRef sPath )
{
	CSchemaPathString sNormalized;

	return CUtlString( NormalizePath( sPath, sNormalized ) );
}

const char *SetText( llvm::StringRef sText, CBufferString &sOut )
{
	sOut.Set( sText.data(), static_cast< int >( sText.size() ) );

	return sOut.Get();
}

//--------------------------------------------------------------------------------------------------
// Markup recorded by the preprocessor callbacks
//--------------------------------------------------------------------------------------------------
enum MarkerKind_t
{
	MARKER_SCHEMA,
	MARKER_NOSCHEMA,
	MARKER_META,
	MARKER_TYPEMETA,
	MARKER_USE_CODEGEN,

	MARKER_COUNT,
};

struct Marker_t
{
	MarkerKind_t m_eKind = MARKER_SCHEMA;
	unsigned m_nOffset = 0;
	unsigned m_nEnd = 0;
	int m_nLine = 0;
	int m_nColumn = 0;
	CUtlString m_sArgs;
	mutable bool m_bUsed = false;
};

// Markers of one declaration, rarely more than a few
using MarkerList_t = CUtlLeanVectorFixedGrowable< const Marker_t *, 8 >;

struct FileMarkers_t
{
	CUtlString m_sPath;
	CUtlLeanVector< Marker_t > m_Markers;
	bool m_bSorted = true;

	void Add( const Marker_t &marker )
	{
		for ( const Marker_t &existing : m_Markers )
		{
			// valve_schema expands to schema, both report the same spot
			if ( existing.m_nOffset == marker.m_nOffset && existing.m_eKind == marker.m_eKind )
				return;
		}

		m_bSorted = m_bSorted && ( m_Markers.Count() == 0 || m_Markers.Tail().m_nOffset <= marker.m_nOffset );
		m_Markers.AddToTail( marker );
	}

	void Sort()
	{
		if ( m_bSorted )
			return;

		Vector_Sort( m_Markers, []( const Marker_t &a, const Marker_t &b ) { return a.m_nOffset < b.m_nOffset; } );
		m_bSorted = true;
	}
};

struct MetaTagDecl_t
{
	CUtlString m_sName;
	unsigned m_nLocations = 0;
	bool m_bValue = false;
	CUtlString m_sStorage;
	bool m_bCodegen = false;
	SchemaDiagLocation_t m_Location;
};

struct CodegenTagDecl_t
{
	CUtlString m_sName;
	CUtlString m_sScript;
	unsigned m_nLocations = 0;
	SchemaDiagLocation_t m_Location;
};

// SchemaMetaTagLocation_t of schemasystem/schemametatag.h
enum
{
	META_TAG_ON_CLASS = 1 << 0,
	META_TAG_ON_FIELD = 1 << 1,
	META_TAG_ON_METHOD = 1 << 2,
	META_TAG_ON_ENUM = 1 << 3,
	META_TAG_ON_ENUMERATOR = 1 << 4,
	META_TAG_ON_ATOMIC = 1 << 5,
};

const char *LocationName( unsigned nLocation )
{
	switch ( nLocation )
	{
		case META_TAG_ON_CLASS: return "a class";
		case META_TAG_ON_FIELD: return "a field";
		case META_TAG_ON_METHOD: return "a method";
		case META_TAG_ON_ENUM: return "an enum";
		case META_TAG_ON_ENUMERATOR: return "an enumerator";
		case META_TAG_ON_ATOMIC: return "an atomic";
		default: return "this location";
	}
}

// Class tags CS2 keeps as SCHEMA_CF1_INFO_TAG_* bits of m_nFlags1 instead of metadata entries
const char *const s_FlagTags[] =
{
	"MNetworkAssumeNotNetworkable",
	"MNetworkNoBase",
	"MIgnoreTypeScopeMetaChecks",
	"MDisableDataDescValidation",
	"MClassHasEntityLimitedDataDesc",
	"MClassHasCustomAlignedNewDelete",
	"MNonConstructibleClassBase",
	"MConstructibleClassBase",
	"MHasKV3TransferPolymorphicClassname",
};

bool IsFlagTag( const char *pszName )
{
	for ( const char *pszTag : s_FlagTags )
	{
		if ( !V_strcmp( pszName, pszTag ) )
			return true;
	}

	return false;
}

// Markers per file, meta and codegen tag declarations
class CMarkupSet
{
public:
	~CMarkupSet()
	{
		FOR_EACH_MAP_FAST( m_Files, i )
			delete m_Files[ i ];
	}

	FileMarkers_t &File( const CUtlString &sPath )
	{
		auto i = m_Files.Find( sPath );

		if ( m_Files.IsValidIndex( i ) )
			return *m_Files[ i ];

		FileMarkers_t *pFile = new FileMarkers_t;

		pFile->m_sPath = sPath;
		m_Files.Insert( sPath, pFile );

		return *pFile;
	}

	FileMarkers_t *FindFile( const CUtlString &sPath ) const
	{
		auto i = m_Files.Find( sPath );

		return m_Files.IsValidIndex( i ) ? m_Files[ i ] : nullptr;
	}

	CSchemaStringMap< FileMarkers_t * > m_Files;
	CSchemaStringMap< MetaTagDecl_t > m_Tags;
	CSchemaStringMap< CodegenTagDecl_t > m_CodegenTags;
};

//--------------------------------------------------------------------------------------------------
// Markup model of a PCH prefix
//--------------------------------------------------------------------------------------------------
void SetLocation( ModelValue_t *pTable, const SchemaDiagLocation_t &location )
{
	Model_SetMemberString( pTable, MODEL_KEY_FILE, location.m_sFile.Get() );
	Model_SetMemberInt( pTable, MODEL_KEY_LINE, location.m_nLine );
	Model_SetMemberInt( pTable, MODEL_KEY_COLUMN, location.m_nColumn );
}

SchemaDiagLocation_t GetLocation( const ModelValue_t *pTable )
{
	return { CUtlString( Model_GetMemberString( pTable, MODEL_KEY_FILE ) ), static_cast< int >( Model_GetMemberInt( pTable, MODEL_KEY_LINE ) ), static_cast< int >( Model_GetMemberInt( pTable, MODEL_KEY_COLUMN ) ) };
}

void SaveMarkup( const CMarkupSet &markup, ModelValue_t *pRoot )
{
	Model_SetTable( pRoot );

	ModelValue_t *pFiles = Model_MemberArray( pRoot, MODEL_KEY_FILES );

	for ( auto i = markup.m_Files.FirstInorder(); markup.m_Files.IsValidIndex( i ); i = markup.m_Files.NextInorder( i ) )
	{
		const FileMarkers_t *pFile = markup.m_Files[ i ];
		ModelValue_t *pFileModel = Model_Append( pFiles );

		Model_SetTable( pFileModel );
		Model_SetMemberString( pFileModel, MODEL_KEY_PATH, pFile->m_sPath.Get() );

		ModelValue_t *pMarkers = Model_MemberArray( pFileModel, MODEL_KEY_MARKERS );

		for ( const Marker_t &marker : pFile->m_Markers )
		{
			ModelValue_t *pMarker = Model_Append( pMarkers );

			Model_SetTable( pMarker );
			Model_SetMemberInt( pMarker, MODEL_KEY_KIND, marker.m_eKind );
			Model_SetMemberInt( pMarker, MODEL_KEY_OFFSET, marker.m_nOffset );
			Model_SetMemberInt( pMarker, MODEL_KEY_END, marker.m_nEnd );
			Model_SetMemberInt( pMarker, MODEL_KEY_LINE, marker.m_nLine );
			Model_SetMemberInt( pMarker, MODEL_KEY_COLUMN, marker.m_nColumn );
			Model_SetMemberString( pMarker, MODEL_KEY_ARGS, marker.m_sArgs.Get() );
		}
	}

	ModelValue_t *pTags = Model_MemberArray( pRoot, MODEL_KEY_TAGS );

	for ( auto i = markup.m_Tags.FirstInorder(); markup.m_Tags.IsValidIndex( i ); i = markup.m_Tags.NextInorder( i ) )
	{
		const MetaTagDecl_t &tag = markup.m_Tags[ i ];
		ModelValue_t *pTag = Model_Append( pTags );

		Model_SetTable( pTag );
		Model_SetMemberString( pTag, MODEL_KEY_NAME, tag.m_sName.Get() );
		Model_SetMemberInt( pTag, MODEL_KEY_LOCATIONS, tag.m_nLocations );
		Model_SetMemberBool( pTag, MODEL_KEY_VALUE, tag.m_bValue );
		Model_SetMemberString( pTag, MODEL_KEY_STORAGE, tag.m_sStorage.Get() );
		Model_SetMemberBool( pTag, MODEL_KEY_CODEGEN, tag.m_bCodegen );
		SetLocation( pTag, tag.m_Location );
	}

	ModelValue_t *pCodegenTags = Model_MemberArray( pRoot, MODEL_KEY_CODEGEN_TAGS );

	for ( auto i = markup.m_CodegenTags.FirstInorder(); markup.m_CodegenTags.IsValidIndex( i ); i = markup.m_CodegenTags.NextInorder( i ) )
	{
		const CodegenTagDecl_t &tag = markup.m_CodegenTags[ i ];
		ModelValue_t *pTag = Model_Append( pCodegenTags );

		Model_SetTable( pTag );
		Model_SetMemberString( pTag, MODEL_KEY_NAME, tag.m_sName.Get() );
		Model_SetMemberString( pTag, MODEL_KEY_SCRIPT, tag.m_sScript.Get() );
		Model_SetMemberInt( pTag, MODEL_KEY_LOCATIONS, tag.m_nLocations );
		SetLocation( pTag, tag.m_Location );
	}
}

void LoadMarkup( const ModelValue_t *pRoot, CMarkupSet &markup )
{
	const ModelValue_t *pFiles = Model_Find( pRoot, MODEL_KEY_FILES );

	for ( int i = 0; i < Model_Count( pFiles ); ++i )
	{
		const ModelValue_t *pFile = Model_At( pFiles, i );
		FileMarkers_t &file = markup.File( CUtlString( Model_GetMemberString( pFile, MODEL_KEY_PATH ) ) );
		const ModelValue_t *pMarkers = Model_Find( pFile, MODEL_KEY_MARKERS );

		for ( int j = 0; j < Model_Count( pMarkers ); ++j )
		{
			const ModelValue_t *pMarker = Model_At( pMarkers, j );

			Marker_t marker;

			marker.m_eKind = static_cast< MarkerKind_t >( Model_GetMemberInt( pMarker, MODEL_KEY_KIND ) );
			marker.m_nOffset = static_cast< unsigned >( Model_GetMemberInt( pMarker, MODEL_KEY_OFFSET ) );
			marker.m_nEnd = static_cast< unsigned >( Model_GetMemberInt( pMarker, MODEL_KEY_END ) );
			marker.m_nLine = static_cast< int >( Model_GetMemberInt( pMarker, MODEL_KEY_LINE ) );
			marker.m_nColumn = static_cast< int >( Model_GetMemberInt( pMarker, MODEL_KEY_COLUMN ) );
			marker.m_sArgs = Model_GetMemberString( pMarker, MODEL_KEY_ARGS );

			file.Add( marker );
		}

		file.Sort();
	}

	const ModelValue_t *pTags = Model_Find( pRoot, MODEL_KEY_TAGS );

	for ( int i = 0; i < Model_Count( pTags ); ++i )
	{
		const ModelValue_t *pTag = Model_At( pTags, i );

		MetaTagDecl_t tag;

		tag.m_sName = Model_GetMemberString( pTag, MODEL_KEY_NAME );
		tag.m_nLocations = static_cast< unsigned >( Model_GetMemberInt( pTag, MODEL_KEY_LOCATIONS ) );
		tag.m_bValue = Model_GetMemberBool( pTag, MODEL_KEY_VALUE );
		tag.m_sStorage = Model_GetMemberString( pTag, MODEL_KEY_STORAGE );
		tag.m_bCodegen = Model_GetMemberBool( pTag, MODEL_KEY_CODEGEN );
		tag.m_Location = GetLocation( pTag );

		markup.m_Tags.InsertOrReplace( tag.m_sName, tag );
	}

	const ModelValue_t *pCodegenTags = Model_Find( pRoot, MODEL_KEY_CODEGEN_TAGS );

	for ( int i = 0; i < Model_Count( pCodegenTags ); ++i )
	{
		const ModelValue_t *pTag = Model_At( pCodegenTags, i );

		CodegenTagDecl_t tag;

		tag.m_sName = Model_GetMemberString( pTag, MODEL_KEY_NAME );
		tag.m_sScript = Model_GetMemberString( pTag, MODEL_KEY_SCRIPT );
		tag.m_nLocations = static_cast< unsigned >( Model_GetMemberInt( pTag, MODEL_KEY_LOCATIONS ) );
		tag.m_Location = GetLocation( pTag );

		markup.m_CodegenTags.InsertOrReplace( tag.m_sName, tag );
	}
}

//--------------------------------------------------------------------------------------------------
// Preprocessor callbacks: positions and raw argument tokens of the markup macros
//--------------------------------------------------------------------------------------------------
class CSchemaPPCallbacks : public PPCallbacks
{
public:
	CSchemaPPCallbacks( Preprocessor &pp, CMarkupSet &markup, CSchemaStringList &deps ) :
		m_PP( pp ),
		m_SM( pp.getSourceManager() ),
		m_Markup( markup ),
		m_Deps( deps )
	{
	}

	void FileChanged( SourceLocation loc, FileChangeReason eReason, SrcMgr::CharacteristicKind, FileID ) override
	{
		if ( eReason != EnterFile )
			return;

		// The real path: ".." after a symlink, e.g. /lib -> /usr/lib, does not collapse textually
		if ( OptionalFileEntryRef file = m_SM.getFileEntryRefForID( m_SM.getFileID( loc ) ) )
		{
			const llvm::StringRef sRealPath = file->getFileEntry().tryGetRealPathName();

			m_Deps.AddToTail( StoredPath( sRealPath.empty() ? file->getName() : sRealPath ) );
		}
	}

	void MacroExpands( const Token &macroNameToken, const MacroDefinition &, SourceRange range, const MacroArgs *pArgs ) override
	{
		const IdentifierInfo *pIdentifier = macroNameToken.getIdentifierInfo();

		if ( !pIdentifier )
			return;

		const llvm::StringRef sName = pIdentifier->getName();

		if ( sName == "schema" )
			AddMarker( MARKER_SCHEMA, range, nullptr );
		else if ( sName == "noschema" )
			AddMarker( MARKER_NOSCHEMA, range, nullptr );
		else if ( sName == "META" )
			AddMarker( MARKER_META, range, pArgs );
		else if ( sName == "TYPEMETA" )
			AddMarker( MARKER_TYPEMETA, range, pArgs );
		else if ( sName == "META_USE_CODEGEN_TAG" )
			AddMarker( MARKER_USE_CODEGEN, range, pArgs );
		else if ( sName == "DECLARE_SCHEMA_META_TAG" )
			AddMetaTag( range, pArgs );
		else if ( sName == "DECLARE_SCHEMA_CODEGEN_TAG" )
			AddCodegenTag( range, pArgs );
	}

private:
	const char *ArgText( const MacroArgs *pArgs, unsigned nArg, CBufferString &sOut ) const
	{
		sOut.Clear();

		if ( !pArgs || nArg >= pArgs->getNumMacroArguments() )
			return sOut.Get();

		for ( const Token *pToken = pArgs->getUnexpArgument( nArg ); pToken->isNot( tok::eof ); ++pToken )
		{
			if ( sOut.Length() > 0 && pToken->hasLeadingSpace() )
				sOut.Append( " " );

			llvm::SmallString< 64 > spelling;

			sOut.Append( m_PP.getSpelling( *pToken, spelling ).data(), static_cast< int >( m_PP.getSpelling( *pToken, spelling ).size() ) );
		}

		return sOut.Get();
	}

	SchemaDiagLocation_t Location( SourceLocation loc ) const
	{
		const PresumedLoc presumed = m_SM.getPresumedLoc( m_SM.getExpansionLoc( loc ) );

		if ( presumed.isInvalid() )
			return {};

		return { StoredPath( presumed.getFilename() ), static_cast< int >( presumed.getLine() ), static_cast< int >( presumed.getColumn() ) };
	}

	void AddMarker( MarkerKind_t eKind, SourceRange range, const MacroArgs *pArgs )
	{
		const SourceLocation begin = m_SM.getExpansionLoc( range.getBegin() );
		const SourceLocation end = m_SM.getExpansionLoc( range.getEnd() );
		const OptionalFileEntryRef file = m_SM.getFileEntryRefForID( m_SM.getFileID( begin ) );

		if ( !file )
			return;

		Marker_t marker;

		marker.m_eKind = eKind;
		marker.m_nOffset = m_SM.getFileOffset( begin );
		marker.m_nEnd = m_SM.getFileOffset( end ) + Lexer::MeasureTokenLength( end, m_SM, m_PP.getLangOpts() );

		const PresumedLoc presumed = m_SM.getPresumedLoc( begin );

		marker.m_nLine = presumed.isValid() ? static_cast< int >( presumed.getLine() ) : 0;
		marker.m_nColumn = presumed.isValid() ? static_cast< int >( presumed.getColumn() ) : 0;

		CBufferStringN< 512 > sArgs;

		marker.m_sArgs = ArgText( pArgs, 0, sArgs );

		m_Markup.File( StoredPath( file->getName() ) ).Add( marker );
	}

	// META_TAG_ON_CLASS | META_TAG_ON_FIELD and plain numbers
	static unsigned EvaluateLocations( const char *pszText )
	{
		static const struct
		{
			const char *m_pszName;
			unsigned m_nBit;
		}
		s_Names[] =
		{
			{ "META_TAG_ON_CLASS", META_TAG_ON_CLASS },
			{ "META_TAG_ON_FIELD", META_TAG_ON_FIELD },
			{ "META_TAG_ON_METHOD", META_TAG_ON_METHOD },
			{ "META_TAG_ON_ENUM", META_TAG_ON_ENUM },
			{ "META_TAG_ON_ENUMERATOR", META_TAG_ON_ENUMERATOR },
			{ "META_TAG_ON_ATOMIC", META_TAG_ON_ATOMIC },
		};

		unsigned nLocations = 0;
		const char *p = pszText;

		while ( *p )
		{
			if ( V_isalpha( *p ) || *p == '_' )
			{
				const char *pEnd = p;

				while ( V_isalnum( *pEnd ) || *pEnd == '_' )
					++pEnd;

				for ( const auto &name : s_Names )
				{
					if ( static_cast< int >( pEnd - p ) == V_strlen( name.m_pszName ) && !V_strncmp( p, name.m_pszName, static_cast< int >( pEnd - p ) ) )
						nLocations |= name.m_nBit;
				}

				p = pEnd;
			}
			else if ( V_isdigit( *p ) )
			{
				char *pEnd = nullptr;

				nLocations |= static_cast< unsigned >( strtoul( p, &pEnd, 0 ) );
				p = pEnd;
			}
			else
			{
				++p;
			}
		}

		return nLocations;
	}

	void AddMetaTag( SourceRange range, const MacroArgs *pArgs )
	{
		if ( !pArgs || pArgs->getNumMacroArguments() < 3 )
			return;

		MetaTagDecl_t tag;
		CBufferStringN< 256 > sArg;

		tag.m_sName = ArgText( pArgs, 0, sArg );
		tag.m_nLocations = EvaluateLocations( ArgText( pArgs, 1, sArg ) );
		tag.m_Location = Location( range.getBegin() );

		// META_TAG_ONLY() or META_VALUE( type )
		ArgText( pArgs, 2, sArg );

		if ( String_StartsWith( sArg.Get(), "META_VALUE" ) )
		{
			const char *pOpen = strchr( sArg.Get(), '(' );
			const char *pClose = strrchr( sArg.Get(), ')' );

			tag.m_bValue = true;

			if ( pOpen && pClose && pClose > pOpen )
			{
				CSchemaNameString sStorage;

				tag.m_sStorage = String_Trim( pOpen + 1, static_cast< int >( pClose - pOpen - 1 ), sStorage );
			}
		}

		// DECLARE_SCHEMA_CODEGEN_TAG declares its tag through this macro too
		auto i = m_Markup.m_Tags.Find( tag.m_sName );

		if ( m_Markup.m_Tags.IsValidIndex( i ) )
		{
			tag.m_bCodegen = m_Markup.m_Tags[ i ].m_bCodegen;
			m_Markup.m_Tags[ i ] = tag;
		}
		else
		{
			tag.m_bCodegen = m_Markup.m_CodegenTags.IsValidIndex( m_Markup.m_CodegenTags.Find( tag.m_sName ) );
			m_Markup.m_Tags.Insert( tag.m_sName, tag );
		}
	}

	void AddCodegenTag( SourceRange range, const MacroArgs *pArgs )
	{
		if ( !pArgs || pArgs->getNumMacroArguments() < 3 )
			return;

		CodegenTagDecl_t tag;
		CBufferStringN< 256 > sArg;

		tag.m_sName = ArgText( pArgs, 0, sArg );
		tag.m_nLocations = EvaluateLocations( ArgText( pArgs, 1, sArg ) );

		// The script keeps its spaces and line breaks, take it from the file
		const Token *pFirst = pArgs->getUnexpArgument( 2 );

		if ( pFirst->is( tok::eof ) )
			return;

		const Token *pLast = pFirst;

		for ( const Token *pToken = pFirst; pToken->isNot( tok::eof ); ++pToken )
			pLast = pToken;

		const SourceLocation begin = m_SM.getSpellingLoc( pFirst->getLocation() );
		const SourceLocation end = m_SM.getSpellingLoc( pLast->getLocation() ).getLocWithOffset( pLast->getLength() );

		bool bInvalid = false;
		llvm::StringRef sText = Lexer::getSourceText( CharSourceRange::getCharRange( begin, end ), m_SM, m_PP.getLangOpts(), &bInvalid );

		if ( bInvalid )
			return;

		// ( script )
		sText = sText.trim();

		if ( sText.starts_with( "(" ) && sText.ends_with( ")" ) )
			sText = sText.drop_front().drop_back();

		tag.m_sScript.SetDirect( sText.data(), static_cast< int >( sText.size() ) );

		// The position of the script itself, errors inside it add their offset
		const PresumedLoc presumed = m_SM.getPresumedLoc( begin );

		tag.m_Location = presumed.isValid() ? SchemaDiagLocation_t{ StoredPath( presumed.getFilename() ), static_cast< int >( presumed.getLine() ), static_cast< int >( presumed.getColumn() ) + 1 } : Location( range.getBegin() );

		auto i = m_Markup.m_CodegenTags.Find( tag.m_sName );

		if ( m_Markup.m_CodegenTags.IsValidIndex( i ) )
		{
			if ( m_Markup.m_CodegenTags[ i ].m_sScript != tag.m_sScript )
			{
				g_Diagnostics.Error( SC_DUPLICATE, tag.m_Location, "codegen tag '%s' is declared again with a different script", tag.m_sName.Get() );
				g_Diagnostics.Note( m_Markup.m_CodegenTags[ i ].m_Location, "previous declaration is here" );
			}

			return;
		}

		m_Markup.m_CodegenTags.Insert( tag.m_sName, tag );

		auto iTag = m_Markup.m_Tags.Find( tag.m_sName );

		if ( m_Markup.m_Tags.IsValidIndex( iTag ) )
			m_Markup.m_Tags[ iTag ].m_bCodegen = true;
	}

	Preprocessor &m_PP;
	SourceManager &m_SM;
	CMarkupSet &m_Markup;
	CSchemaStringList &m_Deps;
};

//--------------------------------------------------------------------------------------------------
// Shared, read-only state of a parse
//--------------------------------------------------------------------------------------------------
struct FrontendContext_t
{
	const SchemaProject_t *m_pProject = nullptr;
	const SchemaAtomicMap_t *m_pAtomics = nullptr;
	const FrontendOptions_t *m_pOptions = nullptr;
	const FrontendPrefix_t *m_pPrefix = nullptr;

	CSchemaStringMap< int > m_InputIndices;		// Normalized input path -> index
	CMarkupSet m_PrefixMarkup;
};

const SchemaAtomicDesc_t *FindAtomic( const SchemaAtomicMap_t &atomics, const char *pszName )
{
	auto i = atomics.Find( CUtlString( pszName ) );

	return atomics.IsValidIndex( i ) ? &atomics[ i ] : nullptr;
}

// The Clang driver takes a std::vector< std::string >, the one place STL strings are needed
std::vector< std::string > BuildArguments( const FrontendContext_t &context, const char *pszMainFile, bool bForPch )
{
	const SchemaProject_t &project = *context.m_pProject;
	const bool bMsvc = project.m_eCompiler == SCHEMA_COMPILER_MSVC;

	std::vector< std::string > args;
	CSchemaPathString sArg;

	auto Add = [ &args ]( const char *pszArg ) { args.emplace_back( pszArg ); };

	if ( bMsvc )
	{
		Add( "clang-cl" );
		Add( "--driver-mode=cl" );
		Add( "/TP" );
		sArg.Format( "/std:c++%s", project.m_sStandard.Get() );
		Add( sArg.Get() );
		Add( "/w" );
		Add( "-Xclang" );
		Add( "-ferror-limit=20" );
	}
	else
	{
		Add( "clang++" );
		Add( "-x" );
		Add( bForPch ? "c++-header" : "c++" );
		sArg.Format( "-std=c++%s", project.m_sStandard.Get() );
		Add( sArg.Get() );
		Add( "-w" );
		Add( "-ferror-limit=20" );
	}

	if ( !bForPch )
		Add( "-fsyntax-only" );

	Add( "-DCOMPILING_SCHEMA" );
	Add( "-DSCHEMA_COMPILER_CLANG" );
	Add( "-Dschema=" );

	for ( const CUtlString &sDefine : project.m_Defines )
	{
		sArg.Format( "%s%s", bMsvc ? "/D" : "-D", sDefine.Get() );
		Add( sArg.Get() );
	}

	for ( const CUtlString &sInclude : project.m_Includes )
	{
		sArg.Format( "%s%s", bMsvc ? "/I" : "-I", sInclude.Get() );
		Add( sArg.Get() );
	}

	// Only options that change what the headers see
	for ( const CUtlString &sOption : project.m_Options )
	{
		const char *pszOption = sOption.Get();

		if ( String_StartsWith( pszOption, "-m" ) || String_StartsWith( pszOption, "/arch:" ) || String_StartsWith( pszOption, "-arch:" ) || String_StartsWith( pszOption, "/Zc:" ) || String_StartsWith( pszOption, "-fms-" ) || String_StartsWith( pszOption, "-fno-ms-" ) || sOption == "/permissive-" || sOption == "-funsigned-char" || sOption == "-fsigned-char" || sOption == "/J" )
			Add( pszOption );
	}

	if ( !context.m_pOptions->m_sResourceDir.IsEmpty() )
	{
		Add( "-resource-dir" );
		Add( context.m_pOptions->m_sResourceDir.Get() );
	}

	const FrontendPrefix_t *pPrefix = context.m_pPrefix;

	if ( bForPch )
	{
		Add( "-Xclang" );
		Add( "-emit-pch" );
		Add( "-o" );
		Add( pPrefix->m_sPchFile.Get() );
	}
	else if ( pPrefix && pPrefix->m_bTextual )
	{
		for ( const CUtlString &sInclude : pPrefix->m_TextualIncludes )
		{
			if ( bMsvc )
			{
				sArg.Format( "/FI%s", sInclude.Get() );
				Add( sArg.Get() );
			}
			else
			{
				Add( "-include" );
				Add( sInclude.Get() );
			}
		}
	}
	else if ( pPrefix && !pPrefix->m_sPchFile.IsEmpty() )
	{
		Add( "-Xclang" );
		Add( "-include-pch" );
		Add( "-Xclang" );
		Add( pPrefix->m_sPchFile.Get() );
	}

	Add( pszMainFile );

	return args;
}

//--------------------------------------------------------------------------------------------------
// One translation unit
//--------------------------------------------------------------------------------------------------
class CSchemaUnit
{
public:
	CSchemaUnit( const FrontendContext_t &context, FrontendUnit_t &unit ) :
		m_Context( context ),
		m_Unit( unit )
	{
		for ( int i = 0; i < unit.m_Inputs.Count(); ++i )
			m_UnitInputs.Insert( context.m_pProject->m_Inputs[ unit.m_Inputs[ i ] ].m_sInput, i );
	}

	CMarkupSet &Markup() { return m_Markup; }
	CSchemaStringList &Deps() { return m_Unit.m_Deps; }

	void AddTopLevelDecl( Decl *pDecl ) { m_TopLevelDecls.AddToTail( pDecl ); }

	void Process( ASTContext &ast );

	// Inputs of this unit; the visitor skips everything else in O(1) per file id
	int UnitInputOf( FileID fileId )
	{
		const unsigned nKey = fileId.getHashValue();
		auto i = m_FileUnitInputs.Find( nKey );

		if ( m_FileUnitInputs.IsValidIndex( i ) )
			return m_FileUnitInputs[ i ];

		auto iInput = m_UnitInputs.Find( FilePath( fileId ) );
		const int nInput = m_UnitInputs.IsValidIndex( iInput ) ? m_UnitInputs[ iInput ] : -1;

		m_FileUnitInputs.Insert( nKey, nInput );

		return nInput;
	}

	SourceManager &SM() { return *m_pSM; }

	void VisitTag( TagDecl *pTag );
	void VisitClassTemplate( ClassTemplateDecl *pTemplate );

private:
	// Path of a file id, cached
	const CUtlString &FilePath( FileID fileId )
	{
		const unsigned nKey = fileId.getHashValue();
		auto i = m_FilePaths.Find( nKey );

		if ( m_FilePaths.IsValidIndex( i ) )
			return m_FilePaths[ i ];

		CUtlString sPath;

		if ( OptionalFileEntryRef file = m_pSM->getFileEntryRefForID( fileId ) )
			sPath = StoredPath( file->getName() );

		return m_FilePaths[ m_FilePaths.Insert( nKey, sPath ) ];
	}

	bool IsProjectInput( const Decl *pDecl )
	{
		const SourceLocation loc = m_pSM->getExpansionLoc( pDecl->getBeginLoc() );

		return m_Context.m_InputIndices.IsValidIndex( m_Context.m_InputIndices.Find( FilePath( m_pSM->getFileID( loc ) ) ) );
	}

	FileMarkers_t *MarkersOf( const CUtlString &sPath )
	{
		if ( FileMarkers_t *pFile = m_Markup.FindFile( sPath ) )
		{
			pFile->Sort();
			return pFile;
		}

		return m_Context.m_PrefixMarkup.FindFile( sPath );
	}

	const MetaTagDecl_t *FindTag( const char *pszName ) const
	{
		const CUtlString sName( pszName );
		auto i = m_Markup.m_Tags.Find( sName );

		if ( m_Markup.m_Tags.IsValidIndex( i ) )
			return &m_Markup.m_Tags[ i ];

		auto iPrefix = m_Context.m_PrefixMarkup.m_Tags.Find( sName );

		return m_Context.m_PrefixMarkup.m_Tags.IsValidIndex( iPrefix ) ? &m_Context.m_PrefixMarkup.m_Tags[ iPrefix ] : nullptr;
	}

	bool IsCodegenTag( const char *pszName ) const
	{
		const CUtlString sName( pszName );

		return m_Markup.m_CodegenTags.IsValidIndex( m_Markup.m_CodegenTags.Find( sName ) ) || m_Context.m_PrefixMarkup.m_CodegenTags.IsValidIndex( m_Context.m_PrefixMarkup.m_CodegenTags.Find( sName ) );
	}

	SchemaDiagLocation_t Location( SourceLocation loc ) const
	{
		const PresumedLoc presumed = m_pSM->getPresumedLoc( m_pSM->getExpansionLoc( loc ) );

		if ( presumed.isInvalid() )
			return {};

		return { StoredPath( presumed.getFilename() ), static_cast< int >( presumed.getLine() ), static_cast< int >( presumed.getColumn() ) };
	}

	static SchemaDiagLocation_t Location( const CUtlString &sPath, const Marker_t &marker )
	{
		return { sPath, marker.m_nLine, marker.m_nColumn };
	}

	// Only whitespace, comments and the allowed characters between two offsets of a file
	bool GapIsClean( FileID fileId, unsigned nBegin, unsigned nEnd, const char *pszAllowed, bool bAllowWords )
	{
		if ( nBegin > nEnd )
			return false;

		bool bInvalid = false;
		const llvm::StringRef sBuffer = m_pSM->getBufferData( fileId, &bInvalid );

		if ( bInvalid || nEnd > sBuffer.size() )
			return false;

		for ( unsigned i = nBegin; i < nEnd; ++i )
		{
			const char c = sBuffer[ i ];

			if ( c == ' ' || c == '\t' || c == '\r' || c == '\n' )
				continue;

			if ( c == '/' && i + 1 < nEnd && sBuffer[ i + 1 ] == '/' )
			{
				while ( i < nEnd && sBuffer[ i ] != '\n' )
					++i;

				continue;
			}

			if ( c == '/' && i + 1 < nEnd && sBuffer[ i + 1 ] == '*' )
			{
				i += 2;

				while ( i + 1 < nEnd && !( sBuffer[ i ] == '*' && sBuffer[ i + 1 ] == '/' ) )
					++i;

				++i;
				continue;
			}

			if ( strchr( pszAllowed, c ) )
				continue;

			// Attributes, alignment macros and template heads before a declaration
			if ( bAllowWords && c != ';' && c != '{' && c != '}' )
				continue;

			return false;
		}

		return true;
	}

	// The marker of a kind right before a declaration, e.g. "schema class", "noschema int"
	const Marker_t *PrecedingMarker( SourceLocation declBegin, MarkerKind_t eKind )
	{
		const SourceLocation loc = m_pSM->getExpansionLoc( declBegin );
		const FileID fileId = m_pSM->getFileID( loc );
		FileMarkers_t *pMarkers = MarkersOf( FilePath( fileId ) );

		if ( !pMarkers )
			return nullptr;

		const unsigned nOffset = m_pSM->getFileOffset( loc );
		const Marker_t *pFound = nullptr;

		for ( const Marker_t &marker : pMarkers->m_Markers )
		{
			if ( marker.m_nOffset >= nOffset )
				break;

			if ( marker.m_eKind == eKind )
				pFound = &marker;
		}

		if ( !pFound || !GapIsClean( fileId, pFound->m_nEnd, nOffset, "", true ) )
			return nullptr;

		return pFound;
	}

	// META markers right after a declaration, e.g. "int m_n; META( ... )" or "A = 0, META( ... )"
	void FollowingMetaMarkers( SourceLocation declEnd, MarkerList_t &markers )
	{
		const SourceLocation loc = m_pSM->getExpansionLoc( declEnd );
		const FileID fileId = m_pSM->getFileID( loc );
		FileMarkers_t *pMarkers = MarkersOf( FilePath( fileId ) );

		if ( !pMarkers )
			return;

		// A field declared by a macro ends where the macro invocation ends
		const SourceLocation last = m_pSM->getExpansionRange( declEnd ).getEnd();
		unsigned nOffset = m_pSM->getFileOffset( last ) + Lexer::MeasureTokenLength( last, *m_pSM, m_pAST->getLangOpts() );

		for ( const Marker_t &marker : pMarkers->m_Markers )
		{
			if ( marker.m_nOffset < nOffset )
				continue;

			if ( marker.m_eKind != MARKER_META || !GapIsClean( fileId, nOffset, marker.m_nOffset, ";,", false ) )
				break;

			markers.AddToTail( &marker );
			nOffset = marker.m_nEnd;
		}
	}

	// Markers of a kind directly in the braces of a tag, not in its nested tags
	void BodyMarkers( TagDecl *pTag, MarkerKind_t eKind, MarkerList_t &markers )
	{
		const SourceRange braces = pTag->getBraceRange();

		if ( braces.isInvalid() )
			return;

		const SourceLocation begin = m_pSM->getExpansionLoc( braces.getBegin() );
		const SourceLocation end = m_pSM->getExpansionLoc( braces.getEnd() );
		const FileID fileId = m_pSM->getFileID( begin );
		FileMarkers_t *pMarkers = MarkersOf( FilePath( fileId ) );

		if ( !pMarkers || m_pSM->getFileID( end ) != fileId )
			return;

		const unsigned nBegin = m_pSM->getFileOffset( begin ), nEnd = m_pSM->getFileOffset( end );

		CUtlLeanVectorFixedGrowable< unsigned, 16 > nested;

		for ( Decl *pDecl : pTag->decls() )
		{
			TagDecl *pNested = dyn_cast< TagDecl >( pDecl );

			if ( auto *pTemplate = dyn_cast< ClassTemplateDecl >( pDecl ) )
				pNested = pTemplate->getTemplatedDecl();

			if ( !pNested || pNested->getBraceRange().isInvalid() )
				continue;

			nested.AddToTail( m_pSM->getFileOffset( m_pSM->getExpansionLoc( pNested->getBraceRange().getBegin() ) ) );
			nested.AddToTail( m_pSM->getFileOffset( m_pSM->getExpansionLoc( pNested->getBraceRange().getEnd() ) ) );
		}

		for ( const Marker_t &marker : pMarkers->m_Markers )
		{
			if ( marker.m_eKind != eKind || marker.m_nOffset <= nBegin || marker.m_nOffset >= nEnd )
				continue;

			bool bNested = false;

			for ( int i = 0; i + 1 < nested.Count(); i += 2 )
				bNested |= marker.m_nOffset > nested[ i ] && marker.m_nOffset < nested[ i + 1 ];

			if ( !bNested )
				markers.AddToTail( &marker );
		}
	}

	bool IsSchemaDecl( const Decl *pDecl )
	{
		if ( const auto *pSpecialization = dyn_cast< ClassTemplateSpecializationDecl >( pDecl ) )
		{
			pDecl = pSpecialization->getSpecializedTemplate();
		}
		else if ( const auto *pRecord = dyn_cast< CXXRecordDecl >( pDecl ) )
		{
			if ( const ClassTemplateDecl *pTemplate = pRecord->getDescribedClassTemplate() )
				pDecl = pTemplate;
		}

		const TagDecl *pTag = dyn_cast< TagDecl >( pDecl );

		if ( pTag && pTag->getDefinition() )
			pDecl = pTag->getDefinition();

		return PrecedingMarker( pDecl->getBeginLoc(), MARKER_SCHEMA ) != nullptr;
	}

	// Name in the schema system: enclosing classes joined by "::", without namespaces
	static const char *SchemaName( const NamedDecl *pDecl, CBufferString &sOut )
	{
		SetText( pDecl->getName(), sOut );

		for ( const DeclContext *pContext = pDecl->getDeclContext(); pContext; pContext = pContext->getParent() )
		{
			if ( const auto *pTag = dyn_cast< TagDecl >( pContext ) )
			{
				sOut.Insert( 0, "::" );
				sOut.Insert( 0, pTag->getName().data(), static_cast< int >( pTag->getName().size() ) );
			}
		}

		return sOut.Get();
	}

	const char *CppName( QualType type, CBufferString &sOut ) const
	{
		PrintingPolicy policy( m_pAST->getLangOpts() );

		policy.SuppressTagKeyword = true;
		policy.SuppressScope = false;
		policy.FullyQualifiedName = true;
		policy.Bool = true;

		// SDK spelling inside template brackets: "CVariantBase< ::CVariantDefaultAllocator >"
		const std::string sName = TypeName::getFullyQualifiedName( type, *m_pAST, policy, true );

		sOut.Clear();

		for ( const char *p = sName.c_str(); *p; ++p )
		{
			if ( *p == '<' )
			{
				while ( p[ 1 ] == ' ' )
					++p;

				sOut.Append( p[ 1 ] == '>' ? "<" : "< " );
			}
			else if ( *p == '>' )
			{
				while ( sOut.Length() > 0 && sOut.Get()[ sOut.Length() - 1 ] == ' ' )
					sOut.TruncateAt( sOut.Length() - 1 );

				sOut.Append( sOut.Length() > 0 && sOut.Get()[ sOut.Length() - 1 ] == '<' ? ">" : " >" );
			}
			else
			{
				sOut.Append( p, 1 );
			}
		}

		return sOut.Get();
	}

	static const char *DeclName( const NamedDecl *pDecl, CBufferString &sOut )
	{
		return SetText( pDecl->getName(), sOut );
	}

	ModelValue_t *InputModel( int nUnitInput ) { return m_Unit.m_Models[ nUnitInput ]; }

	// First type argument of a transparent template, found on the record or one of its bases
	bool UnwrapTransparent( const CXXRecordDecl *pRecord, QualType &wrapped ) const
	{
		if ( const auto *pSpecialization = dyn_cast< ClassTemplateSpecializationDecl >( pRecord ) )
		{
			CSchemaNameString sTemplate;
			const SchemaAtomicDesc_t *pAtomic = FindAtomic( *m_Context.m_pAtomics, DeclName( pSpecialization->getSpecializedTemplate(), sTemplate ) );

			if ( pAtomic && pAtomic->m_eKind == SCHEMA_ATOMIC_KIND_TRANSPARENT )
			{
				const TemplateArgumentList &args = pSpecialization->getTemplateArgs();

				if ( args.size() > 0 && args[ 0 ].getKind() == TemplateArgument::Type )
				{
					wrapped = args[ 0 ].getAsType();
					return true;
				}
			}
		}

		if ( !pRecord->hasDefinition() )
			return false;

		for ( const CXXBaseSpecifier &base : pRecord->bases() )
		{
			const CXXRecordDecl *pBase = base.getType()->getAsCXXRecordDecl();

			if ( pBase && UnwrapTransparent( pBase, wrapped ) )
				return true;
		}

		return false;
	}

	bool ClassifyType( QualType type, ModelValue_t *pOut, const SchemaDiagLocation_t &location, const char *pszWhat, bool bLenient );
	bool ClassifyAtomic( const CXXRecordDecl *pRecord, const SchemaAtomicDesc_t &atomic, QualType type, ModelValue_t *pOut, const SchemaDiagLocation_t &location, const char *pszWhat );

	// META( a; b = c ) entries
	struct MetaEntry_t
	{
		CUtlString m_sName;
		CUtlString m_sValue;
		bool m_bValue = false;
	};

	using MetaEntryList_t = CUtlLeanVectorFixedGrowable< MetaEntry_t, 4 >;

	static void SplitMeta( const char *pszArgs, MetaEntryList_t &entries );
	bool WriteMeta( ModelValue_t *pOwner, const MarkerList_t &markers, const CUtlString &sPath, unsigned nLocation, ModelValue_t *pFlagTags );

	void BuildRecord( CXXRecordDecl *pRecord, int nUnitInput );
	void BuildEnum( EnumDecl *pEnum, int nUnitInput );
	void BuildAtomic( NamedDecl *pDecl, bool bTemplate, int nUnitInput );

	using FieldList_t = CUtlLeanVectorFixedGrowable< FieldDecl *, 32 >;
	using CoveredList_t = CUtlLeanVectorFixedGrowable< uint64, 64 >;

	void WriteDefaults( CXXRecordDecl *pRecord, ModelValue_t *pFields, const FieldList_t &fields );
	void WriteGaps( CXXRecordDecl *pRecord, ModelValue_t *pType, const CoveredList_t &covered );

	void ReportUnusedMarkers();
	void WriteCodegenTags();

	const FrontendContext_t &m_Context;
	FrontendUnit_t &m_Unit;
	CMarkupSet m_Markup;

	CSchemaStringMap< int > m_UnitInputs;
	CUtlMap< unsigned, CUtlString > m_FilePaths;
	CUtlMap< unsigned, int > m_FileUnitInputs;
	CUtlLeanVector< Decl * > m_TopLevelDecls;

	ASTContext *m_pAST = nullptr;
	SourceManager *m_pSM = nullptr;
};

//--------------------------------------------------------------------------------------------------
// Visitor: only declarations from the inputs of the unit, no template instances, no bodies
//--------------------------------------------------------------------------------------------------
class CSchemaVisitor : public RecursiveASTVisitor< CSchemaVisitor >
{
public:
	CSchemaVisitor( CSchemaUnit &unit ) : m_Unit( unit ) {}

	bool shouldVisitTemplateInstantiations() const { return false; }
	bool shouldVisitImplicitCode() const { return false; }
	bool shouldWalkTypesOfTypeLocs() const { return false; }

	bool TraverseDecl( Decl *pDecl )
	{
		if ( !pDecl )
			return true;

		if ( isa< FunctionDecl >( pDecl ) || isa< FunctionTemplateDecl >( pDecl ) || isa< VarDecl >( pDecl ) )
			return true;

		if ( !isa< TranslationUnitDecl >( pDecl ) )
		{
			const SourceLocation loc = m_Unit.SM().getExpansionLoc( pDecl->getLocation() );

			if ( loc.isInvalid() || m_Unit.UnitInputOf( m_Unit.SM().getFileID( loc ) ) < 0 )
				return true;
		}

		return RecursiveASTVisitor< CSchemaVisitor >::TraverseDecl( pDecl );
	}

	bool VisitTagDecl( TagDecl *pTag )
	{
		m_Unit.VisitTag( pTag );
		return true;
	}

	bool VisitClassTemplateDecl( ClassTemplateDecl *pTemplate )
	{
		m_Unit.VisitClassTemplate( pTemplate );
		return true;
	}

private:
	CSchemaUnit &m_Unit;
};

void CSchemaUnit::Process( ASTContext &ast )
{
	m_pAST = &ast;
	m_pSM = &ast.getSourceManager();

	FOR_EACH_MAP_FAST( m_Markup.m_Files, i )
		m_Markup.m_Files[ i ]->Sort();

	CSchemaVisitor visitor( *this );

	for ( Decl *pDecl : m_TopLevelDecls )
		visitor.TraverseDecl( pDecl );

	ReportUnusedMarkers();
	WriteCodegenTags();
}

void CSchemaUnit::VisitTag( TagDecl *pTag )
{
	if ( !pTag->isThisDeclarationADefinition() || pTag->isImplicit() || isa< ClassTemplateSpecializationDecl >( pTag ) )
		return;

	auto *pRecord = dyn_cast< CXXRecordDecl >( pTag );

	// Templates are atomics, VisitClassTemplate takes them
	if ( pRecord && ( pRecord->getDescribedClassTemplate() || pRecord->isLambda() ) )
		return;

	const Marker_t *pMarker = PrecedingMarker( pTag->getBeginLoc(), MARKER_SCHEMA );

	if ( !pMarker )
		return;

	pMarker->m_bUsed = true;

	const int nUnitInput = UnitInputOf( m_pSM->getFileID( m_pSM->getExpansionLoc( pTag->getBeginLoc() ) ) );

	if ( auto *pEnum = dyn_cast< EnumDecl >( pTag ) )
	{
		BuildEnum( pEnum, nUnitInput );
		return;
	}

	if ( !pRecord )
		return;

	CSchemaNameString sName;

	if ( pRecord->isUnion() )
	{
		g_Diagnostics.Error( SC_FIELD_TYPE, Location( pRecord->getBeginLoc() ), "union '%s' cannot be a schema type", DeclName( pRecord, sName ) );
		return;
	}

	if ( FindAtomic( *m_Context.m_pAtomics, DeclName( pRecord, sName ) ) )
	{
		BuildAtomic( pRecord, false, nUnitInput );
		return;
	}

	BuildRecord( pRecord, nUnitInput );
}

void CSchemaUnit::VisitClassTemplate( ClassTemplateDecl *pTemplate )
{
	CXXRecordDecl *pRecord = pTemplate->getTemplatedDecl();

	if ( !pRecord || !pRecord->isThisDeclarationADefinition() )
		return;

	const Marker_t *pMarker = PrecedingMarker( pTemplate->getBeginLoc(), MARKER_SCHEMA );

	if ( !pMarker )
		return;

	pMarker->m_bUsed = true;

	const int nUnitInput = UnitInputOf( m_pSM->getFileID( m_pSM->getExpansionLoc( pTemplate->getBeginLoc() ) ) );

	CSchemaNameString sName;

	if ( !FindAtomic( *m_Context.m_pAtomics, DeclName( pTemplate, sName ) ) )
	{
		g_Diagnostics.Error( SC_UNKNOWN_ATOMIC, Location( pTemplate->getBeginLoc() ), "schema template '%s' is not an atomic of atomic_types.kv3; add it there", sName.Get() );
		return;
	}

	BuildAtomic( pTemplate, true, nUnitInput );
}

void CSchemaUnit::SplitMeta( const char *pszArgs, MetaEntryList_t &entries )
{
	int nDepth = 0;
	char cQuote = 0;
	const char *pPart = pszArgs;

	auto AddEntry = [ &entries ]( const char *pBegin, const char *pEnd )
	{
		CBufferStringN< 512 > sEntry;

		String_Trim( pBegin, static_cast< int >( pEnd - pBegin ), sEntry );

		if ( sEntry.Length() == 0 )
			return;

		MetaEntry_t &entry = entries[ entries.AddToTail() ];
		const char *pEquals = strchr( sEntry.Get(), '=' );

		// The first '=' that is not part of '=='
		if ( pEquals && pEquals[ 1 ] != '=' )
		{
			CBufferStringN< 512 > sTrimmed;

			entry.m_sName = String_Trim( sEntry.Get(), static_cast< int >( pEquals - sEntry.Get() ), sTrimmed );
			entry.m_sValue = String_Trim( pEquals + 1, -1, sTrimmed );
			entry.m_bValue = true;
		}
		else
		{
			entry.m_sName = sEntry.Get();
		}
	};

	for ( const char *p = pszArgs; *p; ++p )
	{
		const char c = *p;

		if ( cQuote )
		{
			if ( c == '\\' && p[ 1 ] )
				++p;
			else if ( c == cQuote )
				cQuote = 0;

			continue;
		}

		if ( c == '"' || c == '\'' )
		{
			cQuote = c;
		}
		else if ( c == '(' || c == '[' || c == '{' )
		{
			++nDepth;
		}
		else if ( c == ')' || c == ']' || c == '}' )
		{
			--nDepth;
		}
		else if ( c == ';' && nDepth == 0 )
		{
			AddEntry( pPart, p );
			pPart = p + 1;
		}
	}

	AddEntry( pPart, pszArgs + V_strlen( pszArgs ) );
}

bool CSchemaUnit::WriteMeta( ModelValue_t *pOwner, const MarkerList_t &markers, const CUtlString &sPath, unsigned nLocation, ModelValue_t *pFlagTags )
{
	bool bSuccess = true;
	ModelValue_t *pMeta = Model_MemberArray( pOwner, MODEL_KEY_META );

	for ( const Marker_t *pMarker : markers )
	{
		pMarker->m_bUsed = true;

		MetaEntryList_t entries;

		SplitMeta( pMarker->m_sArgs.Get(), entries );

		for ( const MetaEntry_t &entry : entries )
		{
			const SchemaDiagLocation_t location = Location( sPath, *pMarker );
			const char *pszName = entry.m_sName.Get();

			CSchemaNameString sIdentifier;

			if ( V_strcmp( Name_ToIdentifier( pszName, sIdentifier ), pszName ) )
			{
				g_Diagnostics.Error( SC_UNKNOWN_TAG, location, "'%s' is not a meta tag name", pszName );
				bSuccess = false;
				continue;
			}

			const MetaTagDecl_t *pTag = FindTag( pszName );

			if ( !pTag )
			{
				g_Diagnostics.Error( SC_UNKNOWN_TAG, location, "unknown schema meta tag '%s'; declare it with DECLARE_SCHEMA_META_TAG", pszName );
				bSuccess = false;
				continue;
			}

			if ( !( pTag->m_nLocations & nLocation ) )
			{
				g_Diagnostics.Error( SC_TAG_LOCATION, location, "meta tag '%s' is not allowed on %s", pszName, LocationName( nLocation ) );
				g_Diagnostics.Note( pTag->m_Location, "'%s' is declared here", pszName );
				bSuccess = false;
				continue;
			}

			if ( entry.m_bValue && !pTag->m_bValue )
			{
				g_Diagnostics.Error( SC_TAG_VALUE, location, "meta tag '%s' is declared with META_TAG_ONLY() and takes no value", pszName );
				g_Diagnostics.Note( pTag->m_Location, "'%s' is declared here", pszName );
				bSuccess = false;
				continue;
			}

			if ( pFlagTags && IsFlagTag( pszName ) )
			{
				Model_SetString( Model_Append( pFlagTags ), pszName );
				continue;
			}

			ModelValue_t *pEntry = Model_Append( pMeta );

			Model_SetTable( pEntry );
			Model_SetMemberString( pEntry, MODEL_KEY_NAME, pszName );

			// The generated unit includes it for Storage_t
			Model_SetMemberString( pEntry, MODEL_KEY_FILE, pTag->m_Location.m_sFile.Get() );

			if ( entry.m_bValue )
				Model_SetMemberString( pEntry, MODEL_KEY_VALUE, entry.m_sValue.Get() );

			// Codegen tags (MEmit*) steer scripts and never reach the records
			if ( pTag->m_bCodegen || String_StartsWith( pszName, "MEmit" ) )
				Model_SetMemberBool( pEntry, MODEL_KEY_CODEGEN, true );
		}
	}

	return bSuccess;
}

bool CSchemaUnit::ClassifyAtomic( const CXXRecordDecl *pRecord, const SchemaAtomicDesc_t &atomic, QualType type, ModelValue_t *pOut, const SchemaDiagLocation_t &location, const char *pszWhat )
{
	CSchemaNameString sCpp;

	Model_SetMemberString( pOut, MODEL_KEY_CATEGORY, "atomic" );
	Model_SetMemberString( pOut, MODEL_KEY_ATOMIC, atomic.m_sName.Get() );
	Model_SetMemberString( pOut, MODEL_KEY_ATOMIC_KIND, AtomicKind_Name( atomic.m_eKind ) );
	Model_SetMemberString( pOut, MODEL_KEY_CPP, CppName( type, sCpp ) );

	if ( !atomic.m_sManipulator.IsEmpty() )
		Model_SetMemberString( pOut, MODEL_KEY_MANIPULATOR, atomic.m_sManipulator.Get() );

	if ( atomic.m_eKind == SCHEMA_ATOMIC_KIND_PLAIN )
		return true;

	const auto *pSpecialization = dyn_cast< ClassTemplateSpecializationDecl >( pRecord );

	if ( !pSpecialization )
	{
		g_Diagnostics.Error( SC_UNKNOWN_ATOMIC, location, "%s: atomic '%s' of kind %s must be a template", pszWhat, atomic.m_sName.Get(), AtomicKind_Name( atomic.m_eKind ) );
		return false;
	}

	int nTypes = 0, nIntegers = 0;

	switch ( atomic.m_eKind )
	{
		case SCHEMA_ATOMIC_KIND_T: nTypes = 1; break;
		case SCHEMA_ATOMIC_KIND_TT: nTypes = 2; break;
		case SCHEMA_ATOMIC_KIND_I: nIntegers = 1; break;
		case SCHEMA_ATOMIC_KIND_COLLECTION: nTypes = 1; break;
		case SCHEMA_ATOMIC_KIND_COLLECTION_FIXED: nTypes = 1; nIntegers = 1; break;
		default: break;
	}

	ModelValue_t *pArgs = Model_MemberArray( pOut, MODEL_KEY_ARGS );
	const TemplateArgumentList &templateArgs = pSpecialization->getTemplateArgs();

	for ( unsigned i = 0; i < templateArgs.size() && ( nTypes > 0 || nIntegers > 0 ); ++i )
	{
		const TemplateArgument &arg = templateArgs[ i ];

		if ( arg.getKind() == TemplateArgument::Type && nTypes > 0 )
		{
			--nTypes;

			ModelValue_t *pArg = Model_Append( pArgs );

			Model_SetTable( pArg );
			Model_SetMemberString( pArg, MODEL_KEY_CPP, CppName( arg.getAsType(), sCpp ) );

			if ( !ClassifyType( arg.getAsType(), Model_MemberTable( pArg, MODEL_KEY_TYPE ), location, pszWhat, true ) )
				return false;
		}
		else if ( arg.getKind() == TemplateArgument::Integral && nIntegers > 0 )
		{
			--nIntegers;

			ModelValue_t *pArg = Model_Append( pArgs );

			Model_SetTable( pArg );
			Model_SetMemberInt( pArg, MODEL_KEY_INT, arg.getAsIntegral().getExtValue() );
		}
	}

	if ( nTypes > 0 || nIntegers > 0 )
	{
		g_Diagnostics.Error( SC_UNKNOWN_ATOMIC, location, "%s: atomic '%s' of kind %s has fewer template arguments than its kind needs", pszWhat, atomic.m_sName.Get(), AtomicKind_Name( atomic.m_eKind ) );
		return false;
	}

	return true;
}

bool CSchemaUnit::ClassifyType( QualType type, ModelValue_t *pOut, const SchemaDiagLocation_t &location, const char *pszWhat, bool bLenient )
{
	Model_SetTable( pOut );

	CSchemaNameString sName, sCpp;

	// A plain atomic may be a typedef, e.g. fltx4 of a compiler vector type; the canonical type loses its name
	for ( QualType sugared = type; ; )
	{
		if ( const auto *pElaborated = dyn_cast< ElaboratedType >( sugared.getTypePtr() ) )
		{
			sugared = pElaborated->getNamedType();
			continue;
		}

		const auto *pTypedef = dyn_cast< TypedefType >( sugared.getTypePtr() );

		if ( !pTypedef )
			break;

		const SchemaAtomicDesc_t *pAtomic = FindAtomic( *m_Context.m_pAtomics, DeclName( pTypedef->getDecl(), sName ) );

		if ( pAtomic && pAtomic->m_eKind == SCHEMA_ATOMIC_KIND_PLAIN )
		{
			Model_SetMemberString( pOut, MODEL_KEY_CATEGORY, "atomic" );
			Model_SetMemberString( pOut, MODEL_KEY_ATOMIC, pAtomic->m_sName.Get() );
			Model_SetMemberString( pOut, MODEL_KEY_ATOMIC_KIND, AtomicKind_Name( pAtomic->m_eKind ) );
			Model_SetMemberString( pOut, MODEL_KEY_CPP, CppName( sugared.getUnqualifiedType(), sCpp ) );
			return true;
		}

		sugared = pTypedef->desugar();
	}

	const QualType canonical = type.getCanonicalType().getUnqualifiedType();
	const Type *pType = canonical.getTypePtr();

	if ( const auto *pBuiltin = dyn_cast< BuiltinType >( pType ) )
	{
		const char *pszBuiltin = nullptr;
		const uint64 nBits = m_pAST->getTypeSize( canonical );

		switch ( pBuiltin->getKind() )
		{
			case BuiltinType::Void: pszBuiltin = "VOID"; break;
			case BuiltinType::Bool: pszBuiltin = "BOOL"; break;
			case BuiltinType::Char_S:
			case BuiltinType::Char_U: pszBuiltin = "CHAR"; break;
			case BuiltinType::SChar: pszBuiltin = "INT8"; break;
			case BuiltinType::UChar: pszBuiltin = "UINT8"; break;
			case BuiltinType::Float: pszBuiltin = "FLOAT32"; break;
			case BuiltinType::Double: pszBuiltin = "FLOAT64"; break;

			case BuiltinType::Short:
			case BuiltinType::Int:
			case BuiltinType::Long:
			case BuiltinType::LongLong:
			{
				pszBuiltin = nBits == 16 ? "INT16" : nBits == 32 ? "INT32" : nBits == 64 ? "INT64" : nullptr;
				break;
			}

			case BuiltinType::UShort:
			case BuiltinType::UInt:
			case BuiltinType::ULong:
			case BuiltinType::ULongLong:
			{
				pszBuiltin = nBits == 16 ? "UINT16" : nBits == 32 ? "UINT32" : nBits == 64 ? "UINT64" : nullptr;
				break;
			}

			default:
				break;
		}

		if ( !pszBuiltin )
		{
			g_Diagnostics.Error( SC_FIELD_TYPE, location, "%s: builtin type '%s' has no schema equivalent; mark the field noschema", pszWhat, canonical.getAsString().c_str() );
			return false;
		}

		Model_SetMemberString( pOut, MODEL_KEY_CATEGORY, "builtin" );
		Model_SetMemberString( pOut, MODEL_KEY_BUILTIN, pszBuiltin );
		return true;
	}

	if ( pType->isPointerType() )
	{
		const QualType pointee = pType->getPointeeType();

		if ( pointee->isFunctionType() )
		{
			g_Diagnostics.Error( SC_FIELD_TYPE, location, "%s: function pointers have no schema type; mark the field noschema", pszWhat );
			return false;
		}

		Model_SetMemberString( pOut, MODEL_KEY_CATEGORY, "ptr" );

		return ClassifyType( pointee, Model_MemberTable( pOut, MODEL_KEY_INNER ), location, pszWhat, true );
	}

	if ( pType->isReferenceType() || pType->isMemberPointerType() )
	{
		g_Diagnostics.Error( SC_FIELD_TYPE, location, "%s: references and member pointers have no schema type; mark the field noschema", pszWhat );
		return false;
	}

	if ( const ConstantArrayType *pArray = m_pAST->getAsConstantArrayType( canonical ) )
	{
		Model_SetMemberString( pOut, MODEL_KEY_CATEGORY, "array" );
		Model_SetMemberInt( pOut, MODEL_KEY_COUNT, static_cast< int64 >( pArray->getSize().getZExtValue() ) );
		Model_SetMemberString( pOut, MODEL_KEY_CPP, CppName( pArray->getElementType(), sCpp ) );

		return ClassifyType( pArray->getElementType(), Model_MemberTable( pOut, MODEL_KEY_INNER ), location, pszWhat, bLenient );
	}

	if ( const auto *pEnumType = dyn_cast< EnumType >( pType ) )
	{
		const EnumDecl *pEnum = pEnumType->getDecl();

		// A plain enum is its underlying integer, as CS2 records flag enums (CEntityIdentity::m_flags is uint32)
		if ( !IsSchemaDecl( pEnum ) )
			return ClassifyType( pEnum->getIntegerType(), pOut, location, pszWhat, bLenient );

		Model_SetMemberString( pOut, MODEL_KEY_CATEGORY, "enum" );
		Model_SetMemberString( pOut, MODEL_KEY_NAME, SchemaName( pEnum, sName ) );
		Model_SetMemberString( pOut, MODEL_KEY_CPP, CppName( canonical, sCpp ) );
		Model_SetMemberBool( pOut, MODEL_KEY_EXTERNAL, !IsProjectInput( pEnum->getDefinition() ? pEnum->getDefinition() : pEnum ) );
		return true;
	}

	if ( const auto *pRecordType = dyn_cast< RecordType >( pType ) )
	{
		const auto *pRecord = dyn_cast< CXXRecordDecl >( pRecordType->getDecl() );

		if ( !pRecord )
		{
			g_Diagnostics.Error( SC_FIELD_TYPE, location, "%s: type '%s' has no schema type; mark the field noschema", pszWhat, canonical.getAsString().c_str() );
			return false;
		}

		// Network var wrappers and the like stand for their first type argument
		QualType wrapped;

		if ( UnwrapTransparent( pRecord, wrapped ) )
			return ClassifyType( wrapped, pOut, location, pszWhat, bLenient );

		CSchemaNameString sAtomicName;

		if ( const auto *pSpecialization = dyn_cast< ClassTemplateSpecializationDecl >( pRecord ) )
			DeclName( pSpecialization->getSpecializedTemplate(), sAtomicName );
		else
			DeclName( pRecord, sAtomicName );

		if ( const SchemaAtomicDesc_t *pAtomic = FindAtomic( *m_Context.m_pAtomics, sAtomicName.Get() ) )
			return ClassifyAtomic( pRecord, *pAtomic, canonical, pOut, location, pszWhat );

		if ( isa< ClassTemplateSpecializationDecl >( pRecord ) )
		{
			if ( IsSchemaDecl( pRecord ) )
				g_Diagnostics.Error( SC_UNKNOWN_ATOMIC, location, "%s: template '%s' is not an atomic of atomic_types.kv3; add it there or mark the field noschema", pszWhat, sAtomicName.Get() );
			else
				g_Diagnostics.Error( SC_FIELD_TYPE, location, "%s: template '%s' is not a schema atomic; mark the field noschema", pszWhat, sAtomicName.Get() );

			return false;
		}

		const CXXRecordDecl *pDefinition = pRecord->getDefinition();
		const bool bSchema = pDefinition && IsSchemaDecl( pDefinition );

		SchemaName( pRecord, sName );

		// A pointer to a declared-only class names it, its module registers it
		if ( pDefinition && !bSchema && !bLenient )
		{
			g_Diagnostics.Error( SC_FIELD_TYPE, location, "%s: class '%s' is not a schema type; mark it schema or the field noschema", pszWhat, sName.Get() );
			return false;
		}

		if ( pDefinition && !bSchema && pDefinition->isUnion() )
		{
			g_Diagnostics.Error( SC_FIELD_TYPE, location, "%s: union '%s' has no schema type; mark the field noschema", pszWhat, sName.Get() );
			return false;
		}

		Model_SetMemberString( pOut, MODEL_KEY_CATEGORY, "class" );
		Model_SetMemberString( pOut, MODEL_KEY_NAME, sName.Get() );
		Model_SetMemberString( pOut, MODEL_KEY_CPP, CppName( canonical, sCpp ) );
		Model_SetMemberBool( pOut, MODEL_KEY_EXTERNAL, !bSchema || !IsProjectInput( pDefinition ) );
		return true;
	}

	g_Diagnostics.Error( SC_FIELD_TYPE, location, "%s: type '%s' has no schema type; mark the field noschema", pszWhat, canonical.getAsString().c_str() );

	return false;
}

void CSchemaUnit::BuildRecord( CXXRecordDecl *pRecord, int nUnitInput )
{
	CSchemaNameString sName, sId, sCpp;

	SchemaName( pRecord, sName );

	const SchemaDiagLocation_t location = Location( pRecord->getLocation() );
	const CUtlString &sPath = location.m_sFile;

	if ( pRecord->getName().empty() )
	{
		g_Diagnostics.Error( SC_FIELD_TYPE, location, "an anonymous class cannot be a schema type" );
		return;
	}

	ModelValue_t *pType = Model_Append( Model_MemberArray( InputModel( nUnitInput ), MODEL_KEY_TYPES ) );

	Model_SetTable( pType );
	Model_SetMemberString( pType, MODEL_KEY_KIND, "class" );
	Model_SetMemberString( pType, MODEL_KEY_NAME, sName.Get() );
	Model_SetMemberString( pType, MODEL_KEY_CPP, CppName( m_pAST->getRecordType( pRecord ), sCpp ) );
	Model_SetMemberString( pType, MODEL_KEY_ID, Name_ToIdentifier( sName.Get(), sId ) );
	SetLocation( pType, location );
	Model_SetMemberBool( pType, MODEL_KEY_POLYMORPHIC, pRecord->isPolymorphic() );
	Model_SetMemberBool( pType, MODEL_KEY_ABSTRACT, pRecord->isAbstract() );

	bool bDataClass = false;
	const char *pszDynamicBinding = "none";

	for ( Decl *pDecl : pRecord->decls() )
	{
		if ( auto *pAlias = dyn_cast< TypeAliasDecl >( pDecl ) )
			bDataClass |= pAlias->getName() == "__schema_class_marker_data__";

		auto *pMethod = dyn_cast< CXXMethodDecl >( pDecl );

		if ( !pMethod || pMethod->getName() != "Schema_DynamicBinding" || pMethod->getNumParams() != 0 )
			continue;

		if ( pMethod->isPureVirtual() )
			pszDynamicBinding = "pure";
		else if ( pMethod->doesThisDeclarationHaveABody() || pMethod->hasSkippedBody() || pMethod->isDefaulted() )
			pszDynamicBinding = "defined";
		else
			pszDynamicBinding = "declared";

		Model_SetMemberString( pType, MODEL_KEY_RETURN_TYPE, CppName( pMethod->getReturnType(), sCpp ) );
	}

	Model_SetMemberBool( pType, MODEL_KEY_DATA_CLASS, bDataClass );
	Model_SetMemberString( pType, MODEL_KEY_DYNAMIC_BINDING, pszDynamicBinding );

	// Bases: only schema classes are in the record, the first is the primary one
	ModelValue_t *pBases = Model_MemberArray( pType, MODEL_KEY_BASES );

	for ( const CXXBaseSpecifier &base : pRecord->bases() )
	{
		const CXXRecordDecl *pBase = base.getType()->getAsCXXRecordDecl();

		if ( !pBase )
			continue;

		if ( base.isVirtual() )
		{
			g_Diagnostics.Error( SC_FIELD_TYPE, Location( base.getBeginLoc() ), "schema class '%s' has a virtual base, the schema system cannot describe it", sName.Get() );
			continue;
		}

		if ( !IsSchemaDecl( pBase ) )
			continue;

		CSchemaNameString sBase, sBaseId;
		ModelValue_t *pBaseModel = Model_Append( pBases );

		SchemaName( pBase, sBase );

		Model_SetTable( pBaseModel );
		Model_SetMemberString( pBaseModel, MODEL_KEY_NAME, sBase.Get() );
		Model_SetMemberString( pBaseModel, MODEL_KEY_CPP, CppName( m_pAST->getRecordType( pBase ), sCpp ) );
		Model_SetMemberString( pBaseModel, MODEL_KEY_ID, Name_ToIdentifier( sBase.Get(), sBaseId ) );
		Model_SetMemberBool( pBaseModel, MODEL_KEY_EXTERNAL, !IsProjectInput( pBase ) );
		Model_SetMemberString( pBaseModel, MODEL_KEY_ACCESS, base.getAccessSpecifier() == AS_public ? "public" : "private" );
	}

	// Fields
	ModelValue_t *pFields = Model_MemberArray( pType, MODEL_KEY_FIELDS );
	const ASTRecordLayout &layout = m_pAST->getASTRecordLayout( pRecord );
	FieldList_t schemaFields;
	CoveredList_t covered;

	for ( FieldDecl *pField : pRecord->fields() )
	{
		const uint64 nOffset = layout.getFieldOffset( pField->getFieldIndex() ) / 8;
		const uint64 nSize = pField->isBitField() ? 0 : m_pAST->getTypeSizeInChars( pField->getType() ).getQuantity();

		CSchemaNameString sField;

		DeclName( pField, sField );

		MarkerList_t meta;

		FollowingMetaMarkers( pField->getEndLoc(), meta );

		if ( const Marker_t *pNoSchema = PrecedingMarker( pField->getBeginLoc(), MARKER_NOSCHEMA ) )
		{
			pNoSchema->m_bUsed = true;

			// Kept in the model for the dump, the codegen skips it
			ModelValue_t *pNoSchemaField = Model_Append( pFields );

			Model_SetTable( pNoSchemaField );
			Model_SetMemberString( pNoSchemaField, MODEL_KEY_NAME, sField.Get() );
			Model_SetMemberBool( pNoSchemaField, MODEL_KEY_NOSCHEMA, true );
			Model_SetMemberInt( pNoSchemaField, MODEL_KEY_OFFSET, static_cast< int64 >( nOffset ) );

			for ( const Marker_t *pMeta : meta )
			{
				pMeta->m_bUsed = true;
				g_Diagnostics.Error( SC_MARKER, Location( sPath, *pMeta ), "META on a noschema field" );
			}

			continue;
		}

		const SchemaDiagLocation_t fieldLocation = Location( pField->getLocation() );
		CBufferStringN< 512 > sWhat;

		sWhat.Format( "field '%s::%s'", sName.Get(), sField.Length() > 0 ? sField.Get() : "<anonymous>" );

		if ( sField.Length() == 0 || pField->isAnonymousStructOrUnion() )
		{
			g_Diagnostics.Error( SC_FIELD_TYPE, fieldLocation, "%s has no name; mark it noschema", sWhat.Get() );
			continue;
		}

		ModelValue_t *pFieldModel = Model_Append( pFields );

		Model_SetTable( pFieldModel );
		Model_SetMemberString( pFieldModel, MODEL_KEY_NAME, sField.Get() );
		Model_SetMemberString( pFieldModel, MODEL_KEY_ACCESS, pField->getAccess() == AS_public ? "public" : "private" );
		Model_SetMemberString( pFieldModel, MODEL_KEY_CPP, CppName( pField->getType(), sCpp ) );
		Model_SetMemberInt( pFieldModel, MODEL_KEY_LINE, fieldLocation.m_nLine );
		Model_SetMemberInt( pFieldModel, MODEL_KEY_COLUMN, fieldLocation.m_nColumn );

		ModelValue_t *pFieldType = Model_MemberTable( pFieldModel, MODEL_KEY_TYPE );

		if ( pField->isBitField() )
		{
			Model_SetMemberString( pFieldType, MODEL_KEY_CATEGORY, "bitfield" );
			Model_SetMemberInt( pFieldType, MODEL_KEY_BITS, pField->getBitWidthValue( *m_pAST ) );

			// offsetof cannot take a bit-field, the storage unit offset comes from the layout
			Model_SetMemberInt( pFieldModel, MODEL_KEY_OFFSET, static_cast< int64 >( nOffset ) );
		}
		else
		{
			ClassifyType( pField->getType(), pFieldType, fieldLocation, sWhat.Get(), false );

			covered.AddToTail( nOffset );
			covered.AddToTail( nOffset + nSize );
		}

		WriteMeta( pFieldModel, meta, sPath, META_TAG_ON_FIELD, nullptr );

		schemaFields.AddToTail( pField );
	}

	// Class tags
	ModelValue_t *pFlagTags = Model_MemberArray( pType, MODEL_KEY_FLAG_TAGS );
	MarkerList_t typeMeta;

	BodyMarkers( pRecord, MARKER_TYPEMETA, typeMeta );
	WriteMeta( pType, typeMeta, sPath, META_TAG_ON_CLASS, pFlagTags );

	ModelValue_t *pCodegen = Model_MemberArray( pType, MODEL_KEY_CODEGEN );
	MarkerList_t codegenUses;

	BodyMarkers( pRecord, MARKER_USE_CODEGEN, codegenUses );

	for ( const Marker_t *pUse : codegenUses )
	{
		pUse->m_bUsed = true;

		CSchemaNameString sTag;

		String_Trim( pUse->m_sArgs.Get(), -1, sTag );

		if ( !IsCodegenTag( sTag.Get() ) )
		{
			g_Diagnostics.Error( SC_UNKNOWN_TAG, Location( sPath, *pUse ), "unknown codegen tag '%s'; declare it with DECLARE_SCHEMA_CODEGEN_TAG", sTag.Get() );
			continue;
		}

		Model_SetString( Model_Append( pCodegen ), sTag.Get() );
	}

	// Default values for codegen scripts
	if ( Model_Count( pCodegen ) > 0 )
		WriteDefaults( pRecord, pFields, schemaFields );

	WriteGaps( pRecord, pType, covered );
}

// Bytes no schema field covers, padding or noschema members; for the dump
void CSchemaUnit::WriteGaps( CXXRecordDecl *pRecord, ModelValue_t *pType, const CoveredList_t &covered )
{
	const ASTRecordLayout &layout = m_pAST->getASTRecordLayout( pRecord );

	CUtlLeanVectorFixedGrowable< int, 32 > order;

	for ( int i = 0; i + 1 < covered.Count(); i += 2 )
		order.AddToTail( i );

	Vector_Sort( order, [ &covered ]( int a, int b ) { return covered[ a ] < covered[ b ]; } );

	uint64 nCursor = pRecord->isPolymorphic() && !layout.getPrimaryBase() ? m_pAST->getTypeSizeInChars( m_pAST->VoidPtrTy ).getQuantity() : 0;

	if ( const CXXRecordDecl *pPrimary = layout.getPrimaryBase() )
	{
		nCursor = m_pAST->getASTRecordLayout( pPrimary ).getDataSize().getQuantity();
	}
	else if ( pRecord->getNumBases() > 0 )
	{
		const CXXRecordDecl *pFirstBase = pRecord->bases_begin()->getType()->getAsCXXRecordDecl();

		if ( pFirstBase && layout.getBaseClassOffset( pFirstBase ).isZero() )
			nCursor = Max< uint64 >( nCursor, m_pAST->getASTRecordLayout( pFirstBase ).getDataSize().getQuantity() );
	}

	ModelValue_t *pGaps = Model_MemberArray( pType, MODEL_KEY_GAPS );
	const uint64 nSize = layout.getSize().getQuantity();

	auto AddGap = [ pGaps ]( uint64 nBegin, uint64 nEnd )
	{
		ModelValue_t *pGap = Model_Append( pGaps );

		Model_SetTable( pGap );
		Model_SetMemberInt( pGap, MODEL_KEY_OFFSET, static_cast< int64 >( nBegin ) );
		Model_SetMemberInt( pGap, MODEL_KEY_SIZE, static_cast< int64 >( nEnd - nBegin ) );
	};

	for ( int i : order )
	{
		if ( covered[ i ] > nCursor )
			AddGap( nCursor, covered[ i ] );

		nCursor = Max( nCursor, covered[ i + 1 ] );
	}

	if ( nSize > nCursor )
		AddGap( nCursor, nSize );

	Model_SetMemberInt( pType, MODEL_KEY_SIZE, static_cast< int64 >( nSize ) );
}

// Field defaults: the in-class initializer, else the default constructor's mem-initializer, else T()
void CSchemaUnit::WriteDefaults( CXXRecordDecl *pRecord, ModelValue_t *pFields, const FieldList_t &fields )
{
	CSchemaStringMap< CUtlString > ctorInits;

	for ( CXXConstructorDecl *pCtor : pRecord->ctors() )
	{
		if ( !pCtor->isDefaultConstructor() || pCtor->isImplicit() || pCtor->isDeleted() )
			continue;

		// Bodies are skipped, so the mem-initializers are read from the source text
		const SourceRange range( m_pSM->getExpansionLoc( pCtor->getBeginLoc() ), m_pSM->getExpansionLoc( pCtor->getEndLoc() ) );
		CBufferStringN< 2048 > sText;

		SetText( Lexer::getSourceText( CharSourceRange::getTokenRange( range ), *m_pSM, m_pAST->getLangOpts() ), sText );

		const char *pParams = strchr( sText.Get(), ')' );
		const char *pColon = pParams ? strchr( pParams, ':' ) : nullptr;
		const char *pBody = pParams ? strchr( pParams, '{' ) : nullptr;

		if ( !pColon || ( pBody && pBody < pColon ) )
			break;

		// "m_a( 1 ), m_b{ 2 }" until the body brace that follows a closed initializer
		const char *pInitsBegin = pColon + 1;
		const char *pInitsEnd = pInitsBegin;
		char cLast = 0;
		int nDepth = 0;

		for ( ; *pInitsEnd; ++pInitsEnd )
		{
			const char c = *pInitsEnd;

			if ( c == '{' && nDepth == 0 && ( cLast == ')' || cLast == '}' ) )
				break;

			if ( c == '(' || c == '{' )
				++nDepth;
			else if ( c == ')' || c == '}' )
				--nDepth;

			if ( !V_isspace( static_cast< unsigned char >( c ) ) )
				cLast = c;
		}

		for ( const char *p = pInitsBegin; p < pInitsEnd; )
		{
			const char *pOpen = strpbrk( p, "({" );

			if ( !pOpen || pOpen >= pInitsEnd )
				break;

			int nLevel = 0;
			const char *pClose = pOpen;

			for ( ; pClose < pInitsEnd; ++pClose )
			{
				if ( *pClose == '(' || *pClose == '{' )
				{
					++nLevel;
				}
				else if ( ( *pClose == ')' || *pClose == '}' ) && --nLevel == 0 )
				{
					break;
				}
			}

			if ( pClose >= pInitsEnd )
				break;

			CSchemaNameString sMember, sValue;

			String_Trim( p, static_cast< int >( pOpen - p ), sMember );
			String_Trim( pOpen + 1, static_cast< int >( pClose - pOpen - 1 ), sValue );

			ctorInits.InsertOrReplace( CUtlString( sMember.Get() ), CUtlString( sValue.Get() ) );

			const char *pComma = strchr( pClose, ',' );

			p = pComma && pComma < pInitsEnd ? pComma + 1 : pInitsEnd;
		}

		break;
	}

	for ( FieldDecl *pField : fields )
	{
		CSchemaNameString sField;

		DeclName( pField, sField );

		ModelValue_t *pFieldModel = nullptr;

		for ( int j = 0; j < Model_Count( pFields ); ++j )
		{
			ModelValue_t *pCandidate = const_cast< ModelValue_t * >( Model_At( pFields, j ) );

			if ( !V_strcmp( sField.Get(), Model_GetMemberString( pCandidate, MODEL_KEY_NAME ) ) && !Model_GetMemberBool( pCandidate, MODEL_KEY_NOSCHEMA ) )
				pFieldModel = pCandidate;
		}

		if ( !pFieldModel )
			continue;

		CBufferStringN< 512 > sDefault, sError;

		if ( const Expr *pInit = pField->getInClassInitializer() )
		{
			SetText( Lexer::getSourceText( CharSourceRange::getTokenRange( pInit->getSourceRange() ), *m_pSM, m_pAST->getLangOpts() ), sDefault );

			// Calls and this make the default a runtime value
			struct CComplexFinder : RecursiveASTVisitor< CComplexFinder >
			{
				bool m_bComplex = false;

				bool VisitCallExpr( CallExpr * ) { m_bComplex = true; return false; }
				bool VisitCXXThisExpr( CXXThisExpr * ) { m_bComplex = true; return false; }
			}
			finder;

			finder.TraverseStmt( const_cast< Expr * >( pInit ) );

			if ( finder.m_bComplex )
				sError.Format( "the initializer of '%s' calls a function or uses this", sField.Get() );

			// "{ x }" and "= x" both arrive as the expression
			if ( sDefault.Length() >= 2 && sDefault.Get()[ 0 ] == '{' && sDefault.Get()[ sDefault.Length() - 1 ] == '}' )
			{
				CBufferStringN< 512 > sInner;

				String_Trim( sDefault.Get() + 1, sDefault.Length() - 2, sInner );
				sDefault.Set( sInner.Get() );
			}
		}
		else
		{
			auto i = ctorInits.Find( CUtlString( sField.Get() ) );

			if ( ctorInits.IsValidIndex( i ) )
			{
				sDefault.Set( ctorInits[ i ].Get() );

				if ( strchr( sDefault.Get(), '(' ) || V_strstr( sDefault.Get(), "this" ) )
					sError.Format( "the constructor initializes '%s' with a call or this", sField.Get() );
			}
		}

		const char *pszCpp = Model_GetMemberString( pFieldModel, MODEL_KEY_CPP );
		CBufferStringN< 512 > sValue;

		if ( sDefault.Length() == 0 )
			sValue.Format( "%s()", pszCpp );
		else
			sValue.Format( "static_cast< %s >( %s )", pszCpp, sDefault.Get() );

		Model_SetMemberString( pFieldModel, MODEL_KEY_DEFAULT_VALUE, sValue.Get() );

		if ( sError.Length() > 0 )
			Model_SetMemberString( pFieldModel, MODEL_KEY_DEFAULT_ERROR, sError.Get() );
	}
}

void CSchemaUnit::BuildEnum( EnumDecl *pEnum, int nUnitInput )
{
	CSchemaNameString sName, sId, sCpp;

	SchemaName( pEnum, sName );

	const SchemaDiagLocation_t location = Location( pEnum->getLocation() );
	const CUtlString &sPath = location.m_sFile;

	if ( pEnum->getName().empty() )
	{
		g_Diagnostics.Error( SC_FIELD_TYPE, location, "an anonymous enum cannot be a schema enum" );
		return;
	}

	ModelValue_t *pType = Model_Append( Model_MemberArray( InputModel( nUnitInput ), MODEL_KEY_TYPES ) );

	Model_SetTable( pType );
	Model_SetMemberString( pType, MODEL_KEY_KIND, "enum" );
	Model_SetMemberString( pType, MODEL_KEY_NAME, sName.Get() );
	Model_SetMemberString( pType, MODEL_KEY_CPP, CppName( m_pAST->getEnumType( pEnum ), sCpp ) );
	Model_SetMemberString( pType, MODEL_KEY_ID, Name_ToIdentifier( sName.Get(), sId ) );
	SetLocation( pType, location );
	Model_SetMemberInt( pType, MODEL_KEY_SIZE, static_cast< int64 >( m_pAST->getTypeSizeInChars( m_pAST->getEnumType( pEnum ) ).getQuantity() ) );

	ModelValue_t *pEnumerators = Model_MemberArray( pType, MODEL_KEY_ENUMERATORS );

	for ( EnumConstantDecl *pEnumerator : pEnum->enumerators() )
	{
		CSchemaNameString sEnumerator;
		ModelValue_t *pEnumeratorModel = Model_Append( pEnumerators );

		Model_SetTable( pEnumeratorModel );
		Model_SetMemberString( pEnumeratorModel, MODEL_KEY_NAME, DeclName( pEnumerator, sEnumerator ) );
		Model_SetMemberInt( pEnumeratorModel, MODEL_KEY_VALUE, pEnumerator->getInitVal().getExtValue() );

		MarkerList_t meta;

		FollowingMetaMarkers( pEnumerator->getEndLoc(), meta );
		WriteMeta( pEnumeratorModel, meta, sPath, META_TAG_ON_ENUMERATOR, nullptr );
	}

	MarkerList_t typeMeta;

	BodyMarkers( pEnum, MARKER_TYPEMETA, typeMeta );
	WriteMeta( pType, typeMeta, sPath, META_TAG_ON_ENUM, nullptr );
}

void CSchemaUnit::BuildAtomic( NamedDecl *pDecl, bool bTemplate, int nUnitInput )
{
	CSchemaNameString sName, sId;

	DeclName( pDecl, sName );

	const SchemaDiagLocation_t location = Location( pDecl->getLocation() );

	ModelValue_t *pType = Model_Append( Model_MemberArray( InputModel( nUnitInput ), MODEL_KEY_TYPES ) );

	Model_SetTable( pType );
	Model_SetMemberString( pType, MODEL_KEY_KIND, "atomic" );
	Model_SetMemberString( pType, MODEL_KEY_NAME, sName.Get() );
	Model_SetMemberString( pType, MODEL_KEY_ID, Name_ToIdentifier( sName.Get(), sId ) );
	Model_SetMemberBool( pType, MODEL_KEY_TEMPLATE, bTemplate );
	Model_SetMemberString( pType, MODEL_KEY_ATOMIC_KIND, AtomicKind_Name( FindAtomic( *m_Context.m_pAtomics, sName.Get() )->m_eKind ) );
	SetLocation( pType, location );

	TagDecl *pTag = bTemplate ? cast< ClassTemplateDecl >( pDecl )->getTemplatedDecl() : cast< TagDecl >( pDecl );

	MarkerList_t typeMeta;

	BodyMarkers( pTag, MARKER_TYPEMETA, typeMeta );
	WriteMeta( pType, typeMeta, location.m_sFile, META_TAG_ON_ATOMIC, nullptr );

	// Fields of an atomic are not described, their markup only documents them
	for ( MarkerKind_t eKind : { MARKER_NOSCHEMA, MARKER_META } )
	{
		MarkerList_t inner;

		BodyMarkers( pTag, eKind, inner );

		for ( const Marker_t *pInner : inner )
			pInner->m_bUsed = true;
	}
}

void CSchemaUnit::ReportUnusedMarkers()
{
	for ( auto i = m_Markup.m_Files.FirstInorder(); m_Markup.m_Files.IsValidIndex( i ); i = m_Markup.m_Files.NextInorder( i ) )
	{
		const FileMarkers_t *pFile = m_Markup.m_Files[ i ];

		if ( !m_UnitInputs.IsValidIndex( m_UnitInputs.Find( pFile->m_sPath ) ) )
			continue;

		for ( const Marker_t &marker : pFile->m_Markers )
		{
			if ( marker.m_bUsed )
				continue;

			const char *pszMessage = "";

			switch ( marker.m_eKind )
			{
				case MARKER_SCHEMA: pszMessage = "schema is not followed by a class, struct or enum definition"; break;
				case MARKER_NOSCHEMA: pszMessage = "noschema is not on a field of a schema class"; break;
				case MARKER_META: pszMessage = "META does not follow a field of a schema class or an enumerator of a schema enum"; break;
				case MARKER_TYPEMETA: pszMessage = "TYPEMETA is not inside a schema class, enum or atomic"; break;
				case MARKER_USE_CODEGEN: pszMessage = "META_USE_CODEGEN_TAG is not inside a schema class"; break;
				default: break;
			}

			g_Diagnostics.Error( SC_MARKER, { pFile->m_sPath, marker.m_nLine, marker.m_nColumn }, "%s", pszMessage );
		}
	}
}

// Every codegen script the unit saw, the codegen picks those its classes use
void CSchemaUnit::WriteCodegenTags()
{
	CSchemaStringMap< const CodegenTagDecl_t * > tags;

	for ( auto i = m_Context.m_PrefixMarkup.m_CodegenTags.FirstInorder(); m_Context.m_PrefixMarkup.m_CodegenTags.IsValidIndex( i ); i = m_Context.m_PrefixMarkup.m_CodegenTags.NextInorder( i ) )
		tags.InsertOrReplace( m_Context.m_PrefixMarkup.m_CodegenTags.Key( i ), &m_Context.m_PrefixMarkup.m_CodegenTags[ i ] );

	for ( auto i = m_Markup.m_CodegenTags.FirstInorder(); m_Markup.m_CodegenTags.IsValidIndex( i ); i = m_Markup.m_CodegenTags.NextInorder( i ) )
		tags.InsertOrReplace( m_Markup.m_CodegenTags.Key( i ), &m_Markup.m_CodegenTags[ i ] );

	for ( ModelValue_t *pModel : m_Unit.m_Models )
	{
		ModelValue_t *pTags = Model_MemberArray( pModel, MODEL_KEY_CODEGEN_TAGS );

		for ( auto i = tags.FirstInorder(); tags.IsValidIndex( i ); i = tags.NextInorder( i ) )
		{
			const CodegenTagDecl_t *pTag = tags[ i ];
			ModelValue_t *pTagModel = Model_Append( pTags );

			Model_SetTable( pTagModel );
			Model_SetMemberString( pTagModel, MODEL_KEY_NAME, pTag->m_sName.Get() );
			Model_SetMemberString( pTagModel, MODEL_KEY_SCRIPT, pTag->m_sScript.Get() );
			Model_SetMemberInt( pTagModel, MODEL_KEY_LOCATIONS, pTag->m_nLocations );
			SetLocation( pTagModel, pTag->m_Location );
		}
	}
}

//--------------------------------------------------------------------------------------------------
// Frontend actions
//--------------------------------------------------------------------------------------------------
class CSchemaConsumer : public ASTConsumer
{
public:
	CSchemaConsumer( CSchemaUnit &unit ) : m_Unit( unit ) {}

	bool HandleTopLevelDecl( DeclGroupRef group ) override
	{
		for ( Decl *pDecl : group )
			m_Unit.AddTopLevelDecl( pDecl );

		return true;
	}

	void HandleTranslationUnit( ASTContext &ast ) override
	{
		if ( !ast.getDiagnostics().hasErrorOccurred() )
			m_Unit.Process( ast );
	}

private:
	CSchemaUnit &m_Unit;
};

class CSchemaAction : public ASTFrontendAction
{
public:
	CSchemaAction( CSchemaUnit &unit ) : m_Unit( unit ) {}

	bool BeginInvocation( CompilerInstance &ci ) override
	{
		ci.getFrontendOpts().SkipFunctionBodies = true;
		return true;
	}

	bool BeginSourceFileAction( CompilerInstance &ci ) override
	{
		Preprocessor &pp = ci.getPreprocessor();

		pp.addPPCallbacks( std::make_unique< CSchemaPPCallbacks >( pp, m_Unit.Markup(), m_Unit.Deps() ) );

		return true;
	}

	std::unique_ptr< ASTConsumer > CreateASTConsumer( CompilerInstance &, llvm::StringRef ) override
	{
		return std::make_unique< CSchemaConsumer >( m_Unit );
	}

private:
	CSchemaUnit &m_Unit;
};

// PCH of the prefix, recording its markup on the way
class CSchemaPchAction : public GeneratePCHAction
{
public:
	CSchemaPchAction( CMarkupSet &markup, CSchemaStringList &deps ) : m_Markup( markup ), m_Deps( deps ) {}

	bool BeginInvocation( CompilerInstance &ci ) override
	{
		ci.getFrontendOpts().SkipFunctionBodies = true;
		return GeneratePCHAction::BeginInvocation( ci );
	}

	bool BeginSourceFileAction( CompilerInstance &ci ) override
	{
		Preprocessor &pp = ci.getPreprocessor();

		pp.addPPCallbacks( std::make_unique< CSchemaPPCallbacks >( pp, m_Markup, m_Deps ) );

		return GeneratePCHAction::BeginSourceFileAction( ci );
	}

private:
	CMarkupSet &m_Markup;
	CSchemaStringList &m_Deps;
};

// Per worker thread: the arena of its models and a file manager that keeps its stat cache
struct WorkerState_t
{
	ModelArena_t *m_pArena = nullptr;
	llvm::IntrusiveRefCntPtr< llvm::vfs::InMemoryFileSystem > m_pMemoryFS;
	llvm::IntrusiveRefCntPtr< FileManager > m_pFiles;
	int m_nVirtualFiles = 0;
};

void CreateWorker( WorkerState_t &worker )
{
	worker.m_pArena = Model_CreateArena();
	worker.m_pMemoryFS = llvm::makeIntrusiveRefCnt< llvm::vfs::InMemoryFileSystem >();

	auto pOverlay = llvm::makeIntrusiveRefCnt< llvm::vfs::OverlayFileSystem >( llvm::vfs::getRealFileSystem() );

	pOverlay->pushOverlay( worker.m_pMemoryFS );

	worker.m_pFiles = llvm::makeIntrusiveRefCnt< FileManager >( FileSystemOptions(), pOverlay );
}

void AddVirtualFile( WorkerState_t &worker, const char *pszPath, const CBufferString &sSource )
{
	worker.m_pMemoryFS->addFile( pszPath, 0, llvm::MemoryBuffer::getMemBufferCopy( llvm::StringRef( sSource.Get(), sSource.Length() ), pszPath ) );
}

// The virtual file and the resource headers are no dependencies
void FilterDeps( CSchemaStringList &deps, const char *pszMainFile, const CUtlString &sResourceDir )
{
	CSchemaPathString sMain, sResource;

	Path_Normalize( pszMainFile, sMain );

	// Real path, as the dependencies are
	if ( !sResourceDir.IsEmpty() )
	{
		llvm::SmallString< 512 > sRealResource;

		if ( llvm::sys::fs::real_path( sResourceDir.Get(), sRealResource ) )
			Path_Normalize( sResourceDir.Get(), sResource );
		else
			NormalizePath( sRealResource.str(), sResource );
	}

	for ( int i = deps.Count() - 1; i >= 0; --i )
	{
		if ( !V_strcmp( deps[ i ].Get(), sMain.Get() ) || ( sResource.Length() > 0 && String_StartsWith( deps[ i ].Get(), sResource.Get() ) ) )
			deps.Remove( i );
	}

	StringList_SortUnique( deps );
}

bool RunUnit( const FrontendContext_t &context, FrontendUnit_t &unit, WorkerState_t &worker )
{
	const SchemaProject_t &project = *context.m_pProject;

	ModelValue_t *pRoot = Model_Root( worker.m_pArena );

	if ( !Model_IsArray( pRoot ) )
		Model_SetArray( pRoot );

	unit.m_Models.RemoveAll();

	CBufferStringN< 4096 > sSource;

	for ( int nInput : unit.m_Inputs )
	{
		ModelValue_t *pModel = Model_Append( pRoot );

		Model_SetTable( pModel );
		Model_SetMemberString( pModel, MODEL_KEY_INPUT, project.m_Inputs[ nInput ].m_sRelative.Get() );
		Model_MemberArray( pModel, MODEL_KEY_TYPES );

		unit.m_Models.AddToTail( pModel );

		sSource.AppendFormat( "#include \"%s\"\n", project.m_Inputs[ nInput ].m_sInput.Get() );
	}

	// The unit is a virtual file including its inputs by absolute path
	CSchemaPathString sMainFile;

	sMainFile.Format( "%s/__schema_unit_%d_%d.cpp", project.m_sOutputDir.Get(), worker.m_nVirtualFiles++, unit.m_Inputs[ 0 ] );

	AddVirtualFile( worker, sMainFile.Get(), sSource );

	CSchemaUnit schemaUnit( context, unit );

	tooling::ToolInvocation invocation( BuildArguments( context, sMainFile.Get(), false ), std::make_unique< CSchemaAction >( schemaUnit ), worker.m_pFiles.get() );

	const int nErrorsBefore = g_Diagnostics.ErrorCount();
	const bool bParsed = invocation.run();

	if ( !bParsed )
	{
		for ( int nInput : unit.m_Inputs )
			g_Diagnostics.Error( SC_PARSE, { project.m_Inputs[ nInput ].m_sInput, 0, 0 }, "parse failed, see the errors above" );
	}

	Vector_Append( unit.m_Deps, context.m_pPrefix->m_Deps );

	FilterDeps( unit.m_Deps, sMainFile.Get(), context.m_pOptions->m_sResourceDir );

	return bParsed && g_Diagnostics.ErrorCount() == nErrorsBefore;
}

} // namespace

FrontendArenas_t::~FrontendArenas_t()
{
	for ( ModelArena_t *pArena : m_Arenas )
		Model_DestroyArena( pArena );
}

bool Frontend_BuildPrefix( const SchemaProject_t &project, const FrontendOptions_t &options, const char *pszPchFile, FrontendPrefix_t &prefix )
{
	prefix.m_sPchFile = pszPchFile;
	prefix.m_Deps.RemoveAll();

	if ( !prefix.m_pArena )
		prefix.m_pArena = Model_CreateArena();

	Model_ClearArena( prefix.m_pArena );
	prefix.m_pMarkup = Model_Root( prefix.m_pArena );

	CBufferStringN< 4096 > sSource;

	for ( const CUtlString &sPreInclude : project.m_PreIncludes )
		sSource.AppendFormat( "#include \"%s\"\n", sPreInclude.Get() );

	for ( const CUtlString &sTagHeader : project.m_TagHeaders )
		sSource.AppendFormat( "#include \"%s\"\n", sTagHeader.Get() );

	if ( !project.m_sPch.IsEmpty() )
		sSource.AppendFormat( "#include \"%s\"\n", project.m_sPch.Get() );

	FrontendContext_t context;

	context.m_pProject = &project;
	context.m_pOptions = &options;
	context.m_pPrefix = &prefix;

	WorkerState_t worker;

	CreateWorker( worker );

	CSchemaPathString sMainFile, sPchDirectory;

	sMainFile.Format( "%s/__schema_prefix.h", project.m_sOutputDir.Get() );

	AddVirtualFile( worker, sMainFile.Get(), sSource );

	CMarkupSet markup;
	CSchemaStringList deps;

	Directory_Create( Path_Directory( pszPchFile, sPchDirectory ) );

	tooling::ToolInvocation invocation( BuildArguments( context, sMainFile.Get(), true ), std::make_unique< CSchemaPchAction >( markup, deps ), worker.m_pFiles.get() );

	const bool bSuccess = invocation.run();

	Model_DestroyArena( worker.m_pArena );

	if ( !bSuccess )
	{
		g_Diagnostics.Error( SC_PARSE, { project.m_sPath, 0, 0 }, "the PRE_INCLUDE/PCH prefix does not compile" );
		prefix.m_sPchFile.Clear();
		return false;
	}

	FilterDeps( deps, sMainFile.Get(), options.m_sResourceDir );

	Vector_Copy( prefix.m_Deps, deps );

	SaveMarkup( markup, prefix.m_pMarkup );

	return true;
}

void Frontend_ParseUnits( const SchemaProject_t &project, const SchemaAtomicMap_t &atomics, const FrontendOptions_t &options, const FrontendPrefix_t &prefix, CUtlLeanVector< FrontendUnit_t > &units, FrontendArenas_t &arenas )
{
	FrontendContext_t context;

	context.m_pProject = &project;
	context.m_pAtomics = &atomics;
	context.m_pOptions = &options;
	context.m_pPrefix = &prefix;

	for ( int i = 0; i < project.m_Inputs.Count(); ++i )
		context.m_InputIndices.Insert( project.m_Inputs[ i ].m_sInput, i );

	if ( prefix.m_pMarkup && !prefix.m_bTextual )
		LoadMarkup( prefix.m_pMarkup, context.m_PrefixMarkup );

	if ( units.Count() == 0 )
		return;

	const int nThreads = Min( options.m_nThreads > 0 ? options.m_nThreads : static_cast< int >( llvm::hardware_concurrency().compute_thread_count() ), units.Count() );

	CUtlLeanVector< WorkerState_t > workers;

	workers.SetCount( nThreads );

	for ( WorkerState_t &worker : workers )
	{
		CreateWorker( worker );
		arenas.m_Arenas.AddToTail( worker.m_pArena );
	}

	CInterlockedInt nNextWorker( 0 ), nNextUnit( 0 );

	llvm::DefaultThreadPool pool( llvm::hardware_concurrency( nThreads ) );

	// One task per worker, each takes units until none is left, so a worker keeps its arena and files
	for ( int i = 0; i < nThreads; ++i )
	{
		pool.async( [ & ]()
		{
			WorkerState_t &worker = workers[ nNextWorker++ ];

			for ( int nUnit = nNextUnit++; nUnit < units.Count(); nUnit = nNextUnit++ )
			{
				const double flStart = Frontend_Seconds();

				units[ nUnit ].m_bSuccess = RunUnit( context, units[ nUnit ], worker );
				units[ nUnit ].m_flSeconds = Frontend_Seconds() - flStart;
			}
		} );
	}

	pool.wait();
}

bool Frontend_HashFile( const char *pszPath, uint64 &nHash )
{
	llvm::ErrorOr< std::unique_ptr< llvm::MemoryBuffer > > buffer = llvm::MemoryBuffer::getFile( pszPath, false, false );

	if ( !buffer )
		return false;

	nHash = llvm::xxh3_64bits( llvm::arrayRefFromStringRef( ( *buffer )->getBuffer() ) );

	return true;
}

uint64 Frontend_HashString( const char *pszData, int nLength )
{
	return llvm::xxh3_64bits( llvm::arrayRefFromStringRef( llvm::StringRef( pszData, nLength ) ) );
}

bool Frontend_FileStatus( const char *pszPath, uint64 &nModTime, uint64 &nSize )
{
	llvm::sys::fs::file_status status;

	if ( llvm::sys::fs::status( pszPath, status ) )
		return false;

	nModTime = static_cast< uint64 >( status.getLastModificationTime().time_since_epoch().count() );
	nSize = status.getSize();

	return true;
}

CUtlString Frontend_DefaultResourceDir( const char *pszArgv0 )
{
	// Shipped binaries carry the headers next to them: <dir>/clang/include
	const std::string sExecutable = llvm::sys::fs::getMainExecutable( pszArgv0, reinterpret_cast< void * >( &Frontend_DefaultResourceDir ) );

	if ( !sExecutable.empty() )
	{
		CSchemaPathString sDirectory, sResource, sProbe;

		Path_Directory( sExecutable.c_str(), sDirectory );
		sResource.Format( "%s/clang", sDirectory.Get() );
		sProbe.Format( "%s/include/stddef.h", sResource.Get() );

		if ( File_Exists( sProbe.Get() ) )
			return CUtlString( Path_Normalize( sResource.Get(), sProbe ) );
	}

	return CUtlString( SCHEMACOMPILER_CLANG_RESOURCE_DIR );
}

double Frontend_Seconds()
{
	return Plat_FloatTime();
}

uint64 Frontend_PeakMemory()
{
#if defined( _WIN32 )
	PROCESS_MEMORY_COUNTERS counters{};

	if ( GetProcessMemoryInfo( GetCurrentProcess(), &counters, sizeof( counters ) ) )
		return counters.PeakWorkingSetSize;

	return 0;
#else
	rusage usage{};

	getrusage( RUSAGE_SELF, &usage );

#if defined( __APPLE__ )
	return static_cast< uint64 >( usage.ru_maxrss );
#else
	return static_cast< uint64 >( usage.ru_maxrss ) * 1024;
#endif
#endif
}
