#include "schemacompiler.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>

#if defined( _WIN32 )
#include <direct.h>
#include <sys/utime.h>
#else
#include <unistd.h>
#include <utime.h>
#endif

CSchemaDiagnostics g_Diagnostics;

//--------------------------------------------------------------------------------------------------
// Diagnostics
//--------------------------------------------------------------------------------------------------
CSchemaDiagnostics::CSchemaDiagnostics() :
	m_bMsvcFormat( false ),
	m_nErrors( 0 )
{
}

void CSchemaDiagnostics::Print( const char *pszLevel, SchemaDiagCode_t eCode, const SchemaDiagLocation_t &location, const char *pszFormat, va_list args )
{
	CBufferStringN< 1024 > sLine;

	if ( location.m_sFile.IsEmpty() )
		sLine.Set( "schemacompiler: " );
	else if ( location.m_nLine <= 0 )
		sLine.Format( "%s: ", location.m_sFile.Get() );
	else if ( m_bMsvcFormat )
		sLine.Format( "%s(%d,%d): ", location.m_sFile.Get(), location.m_nLine, Max( location.m_nColumn, 1 ) );
	else
		sLine.Format( "%s:%d:%d: ", location.m_sFile.Get(), location.m_nLine, Max( location.m_nColumn, 1 ) );

	if ( m_bMsvcFormat && eCode != SC_NONE )
		sLine.AppendFormat( "%s SC%d: ", pszLevel, eCode );
	else
		sLine.AppendFormat( "%s: ", pszLevel );

	sLine.AppendFormatV( pszFormat, args );
	sLine.Append( "\n" );

	AUTO_LOCK( m_Mutex );

	fputs( sLine.Get(), stderr );
	fflush( stderr );
}

void CSchemaDiagnostics::Error( SchemaDiagCode_t eCode, const SchemaDiagLocation_t &location, const char *pszFormat, ... )
{
	++m_nErrors;

	va_list args;
	va_start( args, pszFormat );
	Print( "error", eCode, location, pszFormat, args );
	va_end( args );
}

void CSchemaDiagnostics::Warning( SchemaDiagCode_t eCode, const SchemaDiagLocation_t &location, const char *pszFormat, ... )
{
	va_list args;
	va_start( args, pszFormat );
	Print( "warning", eCode, location, pszFormat, args );
	va_end( args );
}

void CSchemaDiagnostics::Note( const SchemaDiagLocation_t &location, const char *pszFormat, ... )
{
	va_list args;
	va_start( args, pszFormat );
	Print( "note", SC_NONE, location, pszFormat, args );
	va_end( args );
}

//--------------------------------------------------------------------------------------------------
// Paths: '/' separated; a root is "/", "//" or a drive "C:" or "C:/". The parts are offsets into the
// path buffer, nothing goes to the heap for paths of usual length
//--------------------------------------------------------------------------------------------------
namespace
{

struct PathPart_t
{
	int m_nBegin;
	int m_nLength;
};

class CPathParts
{
public:
	CPathParts( const char *pszPath )
	{
		m_sPath.Set( pszPath );
		m_sPath.Replace( '\\', '/' );

		const char *pBase = m_sPath.Get();
		const char *p = pBase;

		if ( p[ 0 ] && p[ 1 ] == ':' )
			p += p[ 2 ] == '/' ? 3 : 2;
		else if ( p[ 0 ] == '/' && p[ 1 ] == '/' )
			p += 2;
		else if ( p[ 0 ] == '/' )
			p += 1;

		m_nRootLength = static_cast< int >( p - pBase );

		while ( *p )
		{
			const char *pEnd = strchr( p, '/' );
			const int nLength = pEnd ? static_cast< int >( pEnd - p ) : V_strlen( p );
			const bool bDot = nLength == 1 && p[ 0 ] == '.';
			const bool bDotDot = nLength == 2 && p[ 0 ] == '.' && p[ 1 ] == '.';

			if ( nLength == 0 || bDot )
			{
				// Nothing
			}
			else if ( bDotDot && m_Parts.Count() > 0 && !IsDotDot( m_Parts.Tail() ) )
			{
				m_Parts.RemoveMultipleFromTail( 1 );
			}
			else if ( !( bDotDot && m_nRootLength > 0 ) )
			{
				m_Parts.AddToTail( { static_cast< int >( p - pBase ), nLength } );
			}

			p += nLength;

			if ( *p == '/' )
				++p;
		}
	}

	bool IsDotDot( const PathPart_t &part ) const { return part.m_nLength == 2 && !V_strncmp( m_sPath.Get() + part.m_nBegin, "..", 2 ); }

	int Count() const { return m_Parts.Count(); }
	const char *Part( int i ) const { return m_sPath.Get() + m_Parts[ i ].m_nBegin; }
	int PartLength( int i ) const { return m_Parts[ i ].m_nLength; }

	bool SameRoot( const CPathParts &other ) const
	{
		return m_nRootLength == other.m_nRootLength && PartsEqual( m_sPath.Get(), other.m_sPath.Get(), m_nRootLength );
	}

	bool SamePart( int i, const CPathParts &other, int j ) const
	{
		return PartLength( i ) == other.PartLength( j ) && PartsEqual( Part( i ), other.Part( j ), PartLength( i ) );
	}

	void AppendRoot( CBufferString &sOut ) const
	{
		sOut.Append( m_sPath.Get(), m_nRootLength );
	}

	void AppendParts( CBufferString &sOut, int nFirst ) const
	{
		for ( int i = nFirst; i < Count(); ++i )
		{
			if ( sOut.Length() > 0 && sOut.Get()[ sOut.Length() - 1 ] != '/' )
				sOut.Append( "/" );

			sOut.Append( Part( i ), PartLength( i ) );
		}
	}

	int RootLength() const { return m_nRootLength; }

private:
	static bool PartsEqual( const char *pLeft, const char *pRight, int nLength )
	{
#if defined( _WIN32 )
		return !V_strnicmp( pLeft, pRight, nLength );
#else
		return !V_strncmp( pLeft, pRight, nLength );
#endif
	}

	CSchemaPathString m_sPath;
	int m_nRootLength;
	CUtlLeanVectorFixedGrowable< PathPart_t, 32 > m_Parts;
};

} // namespace

const char *Path_Normalize( const char *pszPath, CBufferString &sOut )
{
	const CPathParts parts( pszPath );

	sOut.Clear();
	parts.AppendRoot( sOut );
	parts.AppendParts( sOut, 0 );

	if ( sOut.Length() == 0 )
		sOut.Set( "." );

	return sOut.Get();
}

const char *Path_Absolute( const char *pszPath, const char *pszBase, CBufferString &sOut )
{
	const CPathParts parts( pszPath );

	if ( parts.RootLength() > 0 )
		return Path_Normalize( pszPath, sOut );

	CSchemaPathString sJoined;

	sJoined.Format( "%s/%s", pszBase, pszPath );

	return Path_Normalize( sJoined.Get(), sOut );
}

bool Path_Relative( const char *pszPath, const char *pszBase, CBufferString &sOut )
{
	const CPathParts path( pszPath ), base( pszBase );

	sOut.Clear();

	if ( !path.SameRoot( base ) )
		return false;

	int nCommon = 0;

	while ( nCommon < path.Count() && nCommon < base.Count() && path.SamePart( nCommon, base, nCommon ) )
		++nCommon;

	for ( int i = nCommon; i < base.Count(); ++i )
		sOut.Append( sOut.Length() > 0 ? "/.." : ".." );

	path.AppendParts( sOut, nCommon );

	if ( sOut.Length() == 0 )
		sOut.Set( "." );

	return true;
}

const char *Path_Directory( const char *pszPath, CBufferString &sOut )
{
	const char *pSlash = strrchr( pszPath, '/' );
	const char *pBackslash = strrchr( pszPath, '\\' );

	if ( pBackslash > pSlash )
		pSlash = pBackslash;

	if ( !pSlash )
		sOut.Set( "." );
	else if ( pSlash == pszPath )
		sOut.Set( "/" );
	else
		sOut.Set( pszPath, static_cast< int >( pSlash - pszPath ) );

	return sOut.Get();
}

bool Path_IsUnder( const char *pszPath, const char *pszDirectory )
{
	CSchemaPathString sRelative;

	return Path_Relative( pszPath, pszDirectory, sRelative ) && V_strcmp( sRelative.Get(), "." ) && !String_StartsWith( sRelative.Get(), ".." );
}

//--------------------------------------------------------------------------------------------------
// Files
//--------------------------------------------------------------------------------------------------
bool File_Read( const char *pszPath, CUtlBuffer &data )
{
	FILE *pFile = fopen( pszPath, "rb" );

	if ( !pFile )
		return false;

	fseek( pFile, 0, SEEK_END );
	const long nSize = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );

	if ( nSize > 0 )
		data.EnsureCapacity( data.TellPut() + static_cast< int >( nSize ) );

	char chunk[ 65536 ];
	size_t nRead;

	while ( ( nRead = fread( chunk, 1, sizeof( chunk ), pFile ) ) > 0 )
		data.Put( chunk, static_cast< int >( nRead ) );

	const bool bSuccess = !ferror( pFile );

	fclose( pFile );

	return bSuccess;
}

bool File_Exists( const char *pszPath )
{
	struct stat status;

	return stat( pszPath, &status ) == 0;
}

bool Directory_Create( const char *pszPath )
{
	const CPathParts parts( pszPath );
	CSchemaPathString sPath;

	parts.AppendRoot( sPath );

	for ( int i = 0; i < parts.Count(); ++i )
	{
		if ( sPath.Length() > 0 && sPath.Get()[ sPath.Length() - 1 ] != '/' )
			sPath.Append( "/" );

		sPath.Append( parts.Part( i ), parts.PartLength( i ) );

#if defined( _WIN32 )
		_mkdir( sPath.Get() );
#else
		mkdir( sPath.Get(), 0755 );
#endif
	}

	struct stat status;

	return stat( pszPath, &status ) == 0 && ( status.st_mode & S_IFDIR );
}

bool File_WriteIfChanged( const char *pszPath, const void *pData, int nSize, bool *pWritten )
{
	if ( pWritten )
		*pWritten = false;

	CUtlBuffer old;

	if ( File_Read( pszPath, old ) && old.TellPut() == nSize && !memcmp( old.Base(), pData, nSize ) )
		return true;

	CSchemaPathString sDirectory;

	Directory_Create( Path_Directory( pszPath, sDirectory ) );

	// Write a sibling and rename it over, readers never see a half-written file
	CSchemaPathString sTemp;

	sTemp.Format( "%s.tmp", pszPath );

	FILE *pFile = fopen( sTemp.Get(), "wb" );

	if ( !pFile )
		return false;

	const bool bWritten = fwrite( pData, 1, nSize, pFile ) == static_cast< size_t >( nSize );

	if ( fclose( pFile ) != 0 || !bWritten )
	{
		remove( sTemp.Get() );
		return false;
	}

	if ( rename( sTemp.Get(), pszPath ) != 0 )
	{
		// Windows does not replace an existing file
		remove( pszPath );

		if ( rename( sTemp.Get(), pszPath ) != 0 )
			return false;
	}

	if ( pWritten )
		*pWritten = true;

	return true;
}

bool File_WriteIfChanged( const char *pszPath, const CUtlBuffer &data, bool *pWritten )
{
	return File_WriteIfChanged( pszPath, data.Base(), data.TellPut(), pWritten );
}

bool File_Touch( const char *pszPath )
{
	if ( !File_Exists( pszPath ) )
	{
		CSchemaPathString sDirectory;

		Directory_Create( Path_Directory( pszPath, sDirectory ) );

		FILE *pFile = fopen( pszPath, "wb" );

		if ( !pFile )
			return false;

		fclose( pFile );
		return true;
	}

#if defined( _WIN32 )
	return _utime( pszPath, nullptr ) == 0;
#else
	return utime( pszPath, nullptr ) == 0;
#endif
}

//--------------------------------------------------------------------------------------------------
// Strings and names
//--------------------------------------------------------------------------------------------------
const char *Name_ToIdentifier( const char *pszName, CBufferString &sOut )
{
	sOut.Clear();

	for ( const char *p = pszName; *p; ++p )
	{
		const char c = *p;

		if ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || ( c >= '0' && c <= '9' ) || c == '_' )
		{
			sOut.Append( p, 1 );
		}
		else if ( c == ':' && p[ 1 ] == ':' )
		{
			sOut.Append( "__" );
			++p;
		}
		else
		{
			sOut.Append( "_" );
		}
	}

	if ( sOut.Length() == 0 || ( sOut.Get()[ 0 ] >= '0' && sOut.Get()[ 0 ] <= '9' ) )
		sOut.Insert( 0, "_" );

	return sOut.Get();
}

const char *String_Trim( const char *pszString, int nLength, CBufferString &sOut )
{
	if ( nLength < 0 )
		nLength = V_strlen( pszString );

	int nBegin = 0, nEnd = nLength;

	while ( nBegin < nEnd && V_isspace( static_cast< unsigned char >( pszString[ nBegin ] ) ) )
		++nBegin;

	while ( nEnd > nBegin && V_isspace( static_cast< unsigned char >( pszString[ nEnd - 1 ] ) ) )
		--nEnd;

	sOut.Set( pszString + nBegin, nEnd - nBegin );

	return sOut.Get();
}

bool String_StartsWith( const char *pszString, const char *pszPrefix )
{
	return !V_strncmp( pszString, pszPrefix, V_strlen( pszPrefix ) );
}

void StringList_SortUnique( CSchemaStringList &list )
{
	Vector_Sort( list, []( const CUtlString &sLeft, const CUtlString &sRight ) { return V_strcmp( sLeft.Get(), sRight.Get() ) < 0; } );

	for ( int i = list.Count() - 1; i > 0; --i )
	{
		if ( !V_strcmp( list[ i ].Get(), list[ i - 1 ].Get() ) )
			list.Remove( i );
	}
}

bool StringList_Contains( const CSchemaStringList &list, const char *pszString )
{
	for ( const CUtlString &sItem : list )
	{
		if ( !V_strcmp( sItem.Get(), pszString ) )
			return true;
	}

	return false;
}
