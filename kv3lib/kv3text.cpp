#include "kv3lib/kv3text.h"
#include "kv3lib/keyvalues3.h"
#include "tier0/utlbuffer.h"
#include "tier0/utlstring.h"
#include "tier1/generichash.h"
#include "tier1/utlhashtable.h"
#include "tier1/utlvector.h"

#include <cerrno>
#include <cinttypes>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

namespace
{

//--------------------------------------------------------------------------------------------------
// Shared helpers
//--------------------------------------------------------------------------------------------------
struct KV3CodecStringSubType_t
{
	const char *m_pszPrefix;
	KV3SubType_t m_eSubType;
};

// Typed value prefixes of the text encoding, e.g. resource:"materials/dev.vmat"
const KV3CodecStringSubType_t s_StringSubTypes[] =
{
	{ "resource", KV3_SUBTYPE_RESOURCE },
	{ "resource_name", KV3_SUBTYPE_RESOURCE_NAME },
	{ "panorama", KV3_SUBTYPE_PANORAMA },
	{ "soundevent", KV3_SUBTYPE_SOUNDEVENT },
	{ "subclass", KV3_SUBTYPE_SUBCLASS },
	{ "entity_name", KV3_SUBTYPE_ENTITY_NAME },
	{ "localize", KV3_SUBTYPE_LOCALIZE },
};

const char *SubTypePrefix( KV3SubType_t eSubType )
{
	for ( const KV3CodecStringSubType_t &subType : s_StringSubTypes )
	{
		if ( subType.m_eSubType == eSubType )
			return subType.m_pszPrefix;
	}

	return nullptr;
}

// The arena symbol table allocates in 2048-byte pages and keeps no empty string
enum
{
	KV3_CODEC_MAX_INTERNED_LENGTH = 1024,
};

bool CanIntern( const char *pszString, int nLength )
{
	return *pszString && nLength < KV3_CODEC_MAX_INTERNED_LENGTH;
}

// Strings of an arena-owned value live in the arena symbol table, long ones and others are copied to the heap
void SetCodecString( KeyValues3 *pKV, const char *pszString, KV3SubType_t eSubType )
{
	CKV3Arena *pArena = pKV->GetContext();

	if ( pArena && CanIntern( pszString, V_strlen( pszString ) ) )
		pKV->SetStringExternal( pArena->AllocString( pszString ), eSubType );
	else
		pKV->SetString( pszString, eSubType );
}

bool IsIdentifierStart( char c )
{
	return ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || c == '_';
}

bool IsIdentifierChar( char c )
{
	return IsIdentifierStart( c ) || ( c >= '0' && c <= '9' ) || c == '.';
}

bool IsBareMemberName( const char *pszName )
{
	if ( !IsIdentifierStart( pszName[ 0 ] ) )
		return false;

	for ( const char *p = pszName + 1; *p; ++p )
	{
		if ( !IsIdentifierChar( *p ) )
			return false;
	}

	// Keywords would read back as values
	return V_strcmp( pszName, "null" ) && V_strcmp( pszName, "true" ) && V_strcmp( pszName, "false" );
}

int HexDigit( char c )
{
	if ( c >= '0' && c <= '9' )
		return c - '0';

	if ( c >= 'a' && c <= 'f' )
		return c - 'a' + 10;

	if ( c >= 'A' && c <= 'F' )
		return c - 'A' + 10;

	return -1;
}

//--------------------------------------------------------------------------------------------------
// Text parser: one pass over the input, values are created in place
//--------------------------------------------------------------------------------------------------
class CKV3TextParser
{
public:
	CKV3TextParser( const char *pszInput, int nLength, const char *pszName, CUtlString *pError ) :
		m_pBegin( pszInput ),
		m_pCur( pszInput ),
		m_pEnd( pszInput + nLength ),
		m_pszName( pszName ? pszName : "<kv3>" ),
		m_pError( pError ),
		m_bFailed( false )
	{
	}

	bool Parse( KeyValues3 *pKV )
	{
		if ( !SkipWhitespace() )
			return false;

		if ( !ParseValue( pKV, 0 ) )
			return false;

		if ( !SkipWhitespace() )
			return false;

		if ( m_pCur != m_pEnd )
			return Fail( "unexpected '%c' after the root value", *m_pCur );

		return true;
	}

private:
	enum
	{
		MAX_DEPTH = 512,
	};

	bool Fail( const char *pszFormat, ... ) FMTFUNCTION( 2, 3 )
	{
		if ( m_bFailed )
			return false;

		m_bFailed = true;

		if ( !m_pError )
			return false;

		int nLine = 1, nColumn = 1;

		for ( const char *p = m_pBegin; p < m_pCur && p < m_pEnd; ++p )
		{
			if ( *p == '\n' )
			{
				++nLine;
				nColumn = 1;
			}
			else
			{
				++nColumn;
			}
		}

		char szMessage[ 512 ];

		va_list args;
		va_start( args, pszFormat );
		V_vsnprintf( szMessage, sizeof( szMessage ), pszFormat, args );
		va_end( args );

		m_pError->Format( "%s:%d:%d: %s", m_pszName, nLine, nColumn, szMessage );

		return false;
	}

	bool AtEnd() const { return m_pCur >= m_pEnd; }
	char Peek( int nOffset = 0 ) const { return m_pCur + nOffset < m_pEnd ? m_pCur[ nOffset ] : '\0'; }

	bool StartsWith( const char *pszText ) const
	{
		const size_t nLength = strlen( pszText );

		return static_cast< size_t >( m_pEnd - m_pCur ) >= nLength && !memcmp( m_pCur, pszText, nLength );
	}

	// Whitespace, // and /* */ comments and the <!-- --> header
	bool SkipWhitespace()
	{
		while ( !AtEnd() )
		{
			const char c = *m_pCur;

			if ( c == ' ' || c == '\t' || c == '\r' || c == '\n' )
			{
				++m_pCur;
			}
			else if ( c == '/' && Peek( 1 ) == '/' )
			{
				while ( !AtEnd() && *m_pCur != '\n' )
					++m_pCur;
			}
			else if ( c == '/' && Peek( 1 ) == '*' )
			{
				const char *pStart = m_pCur;

				m_pCur += 2;

				while ( !AtEnd() && !( *m_pCur == '*' && Peek( 1 ) == '/' ) )
					++m_pCur;

				if ( AtEnd() )
				{
					m_pCur = pStart;
					return Fail( "unterminated comment" );
				}

				m_pCur += 2;
			}
			else if ( StartsWith( "<!--" ) )
			{
				const char *pStart = m_pCur;

				m_pCur += 4;

				while ( !AtEnd() && !StartsWith( "-->" ) )
					++m_pCur;

				if ( AtEnd() )
				{
					m_pCur = pStart;
					return Fail( "unterminated header" );
				}

				m_pCur += 3;
			}
			else
			{
				break;
			}
		}

		return true;
	}

	bool Expect( char c )
	{
		if ( Peek() != c )
			return AtEnd() ? Fail( "expected '%c', got end of input", c ) : Fail( "expected '%c', got '%c'", c, *m_pCur );

		++m_pCur;

		return true;
	}

	bool ParseValue( KeyValues3 *pKV, int nDepth )
	{
		if ( nDepth > MAX_DEPTH )
			return Fail( "nesting is deeper than %d", MAX_DEPTH );

		if ( AtEnd() )
			return Fail( "expected a value, got end of input" );

		const char c = *m_pCur;

		if ( c == '{' )
			return ParseTable( pKV, nDepth );

		if ( c == '[' )
			return ParseArray( pKV, nDepth );

		if ( c == '"' )
			return ParseString( pKV, KV3_SUBTYPE_STRING );

		if ( c == '#' )
			return ParseBinaryBlob( pKV );

		if ( c == '-' || c == '+' || c == '.' || ( c >= '0' && c <= '9' ) )
			return ParseNumber( pKV );

		if ( IsIdentifierStart( c ) )
			return ParseKeywordOrTypedValue( pKV, nDepth );

		return Fail( "unexpected '%c'", c );
	}

	bool ParseTable( KeyValues3 *pKV, int nDepth )
	{
		++m_pCur;

		pKV->SetToEmptyTable();

		for ( ;; )
		{
			if ( !SkipWhitespace() )
				return false;

			if ( Peek() == '}' )
			{
				++m_pCur;
				return true;
			}

			if ( AtEnd() )
				return Fail( "expected '}', got end of input" );

			if ( !ParseMemberName() )
				return false;

			if ( !SkipWhitespace() || !Expect( '=' ) || !SkipWhitespace() )
				return false;

			KeyValues3 *pMember = pKV->FindOrCreateMember( CKV3MemberName( m_Scratch.Base(), m_Scratch.Count() - 1 ) );

			if ( !ParseValue( pMember, nDepth + 1 ) )
				return false;

			if ( !SkipWhitespace() )
				return false;

			// Commas between members are tolerated
			if ( Peek() == ',' )
				++m_pCur;
		}
	}

	bool ParseMemberName()
	{
		m_Scratch.RemoveAll();

		if ( Peek() == '"' )
			return ParseQuotedString();

		if ( !IsIdentifierStart( Peek() ) )
			return Fail( "expected a member name, got '%c'", Peek() );

		while ( !AtEnd() && IsIdentifierChar( *m_pCur ) )
			m_Scratch.AddToTail( *m_pCur++ );

		m_Scratch.AddToTail( '\0' );

		return true;
	}

	bool ParseArray( KeyValues3 *pKV, int nDepth )
	{
		++m_pCur;

		pKV->SetToEmptyKV3Array();

		for ( ;; )
		{
			if ( !SkipWhitespace() )
				return false;

			if ( Peek() == ']' )
			{
				++m_pCur;
				return true;
			}

			if ( AtEnd() )
				return Fail( "expected ']', got end of input" );

			if ( !ParseValue( pKV->ArrayAddElementToTail(), nDepth + 1 ) )
				return false;

			if ( !SkipWhitespace() )
				return false;

			if ( Peek() == ',' )
				++m_pCur;
			else if ( Peek() != ']' )
				return AtEnd() ? Fail( "expected ',' or ']', got end of input" ) : Fail( "expected ',' or ']', got '%c'", Peek() );
		}
	}

	// Single line "..." with escapes; the result is in m_Scratch, null-terminated
	bool ParseQuotedString()
	{
		const char *pStart = m_pCur;

		++m_pCur;

		while ( !AtEnd() && *m_pCur != '"' )
		{
			char c = *m_pCur++;

			if ( c == '\n' )
			{
				m_pCur = pStart;
				return Fail( "newline in a string, use \"\"\" for multi-line strings" );
			}

			if ( c == '\\' )
			{
				if ( AtEnd() )
					break;

				switch ( *m_pCur++ )
				{
					case 'n': c = '\n'; break;
					case 't': c = '\t'; break;
					case 'r': c = '\r'; break;
					case '"': c = '"'; break;
					case '\'': c = '\''; break;
					case '\\': c = '\\'; break;
					case '?': c = '?'; break;
					default:
					{
						--m_pCur;
						return Fail( "unknown escape '\\%c'", *m_pCur );
					}
				}
			}

			m_Scratch.AddToTail( c );
		}

		if ( AtEnd() )
		{
			m_pCur = pStart;
			return Fail( "unterminated string" );
		}

		++m_pCur;

		m_Scratch.AddToTail( '\0' );

		return true;
	}

	// """<newline> lines <newline>""", taken verbatim
	bool ParseMultilineString()
	{
		const char *pStart = m_pCur;

		m_pCur += 3;

		while ( !AtEnd() && ( *m_pCur == ' ' || *m_pCur == '\t' || *m_pCur == '\r' ) )
			++m_pCur;

		if ( Peek() != '\n' )
			return Fail( "expected a newline after \"\"\"" );

		++m_pCur;

		const char *pContent = m_pCur;

		for ( ;; )
		{
			const char *pLine = m_pCur;
			const char *p = pLine;

			while ( p < m_pEnd && ( *p == ' ' || *p == '\t' ) )
				++p;

			if ( m_pEnd - p >= 3 && !memcmp( p, "\"\"\"", 3 ) )
			{
				// The newline before the closing line is not part of the content
				const char *pContentEnd = pLine > pContent ? pLine - 1 : pContent;

				if ( pContentEnd > pContent && pContentEnd[ -1 ] == '\r' )
					--pContentEnd;

				// CRLF line ends read as LF
				m_Scratch.RemoveAll();

				for ( const char *pChar = pContent; pChar < pContentEnd; ++pChar )
				{
					if ( *pChar != '\r' || pChar + 1 >= pContentEnd || pChar[ 1 ] != '\n' )
						m_Scratch.AddToTail( *pChar );
				}

				m_Scratch.AddToTail( '\0' );

				m_pCur = p + 3;

				return true;
			}

			while ( m_pCur < m_pEnd && *m_pCur != '\n' )
				++m_pCur;

			if ( AtEnd() )
			{
				m_pCur = pStart;
				return Fail( "unterminated multi-line string" );
			}

			++m_pCur;
		}
	}

	bool ParseString( KeyValues3 *pKV, KV3SubType_t eSubType )
	{
		bool bMultiline = StartsWith( "\"\"\"" );

		m_Scratch.RemoveAll();

		if ( bMultiline ? !ParseMultilineString() : !ParseQuotedString() )
			return false;

		SetCodecString( pKV, m_Scratch.Base(), eSubType );

		if ( bMultiline )
			pKV->SetFlag( KEYVALUES3_FLAG_MULTILINE_STRING );

		return true;
	}

	bool ParseBinaryBlob( KeyValues3 *pKV )
	{
		++m_pCur;

		if ( !Expect( '[' ) )
			return false;

		CUtlVector< byte > bytes;

		for ( ;; )
		{
			if ( !SkipWhitespace() )
				return false;

			if ( Peek() == ']' )
			{
				++m_pCur;
				break;
			}

			const int nHigh = HexDigit( Peek() ), nLow = HexDigit( Peek( 1 ) );

			if ( nHigh < 0 || nLow < 0 )
				return Fail( "expected a hex byte in a binary blob" );

			bytes.AddToTail( static_cast< byte >( nHigh * 16 + nLow ) );

			m_pCur += 2;
		}

		pKV->SetToBinaryBlob( bytes.Base(), bytes.Count() );

		return true;
	}

	bool ParseNumber( KeyValues3 *pKV )
	{
		const char *pStart = m_pCur;
		bool bFloat = false;

		if ( *m_pCur == '-' || *m_pCur == '+' )
			++m_pCur;

		const bool bHex = Peek() == '0' && ( Peek( 1 ) == 'x' || Peek( 1 ) == 'X' );

		if ( bHex )
			m_pCur += 2;

		while ( !AtEnd() )
		{
			const char c = *m_pCur;

			if ( ( c >= '0' && c <= '9' ) || ( bHex && HexDigit( c ) >= 0 ) )
			{
				++m_pCur;
			}
			else if ( !bHex && ( c == '.' || c == 'e' || c == 'E' ) )
			{
				bFloat = true;
				++m_pCur;

				if ( ( c == 'e' || c == 'E' ) && ( Peek() == '-' || Peek() == '+' ) )
					++m_pCur;
			}
			else
			{
				break;
			}
		}

		char szNumber[ 64 ];
		const int nLength = static_cast< int >( m_pCur - pStart );

		if ( nLength <= 0 || nLength >= static_cast< int >( sizeof( szNumber ) ) )
		{
			m_pCur = pStart;
			return Fail( "malformed number" );
		}

		memcpy( szNumber, pStart, nLength );
		szNumber[ nLength ] = '\0';

		char *pNumberEnd = nullptr;
		errno = 0;

		if ( bFloat )
		{
			const double flValue = strtod( szNumber, &pNumberEnd );

			if ( pNumberEnd != szNumber + nLength )
			{
				m_pCur = pStart;
				return Fail( "malformed number '%s'", szNumber );
			}

			pKV->SetDouble( flValue );
		}
		else if ( szNumber[ 0 ] == '-' )
		{
			const long long nValue = strtoll( szNumber, &pNumberEnd, 0 );

			if ( pNumberEnd != szNumber + nLength || errno == ERANGE )
			{
				m_pCur = pStart;
				return Fail( "integer '%s' is out of range", szNumber );
			}

			pKV->SetInt64( nValue );
		}
		else
		{
			const unsigned long long nValue = strtoull( szNumber, &pNumberEnd, 0 );

			if ( pNumberEnd != szNumber + nLength || errno == ERANGE )
			{
				m_pCur = pStart;
				return Fail( "integer '%s' is out of range", szNumber );
			}

			if ( nValue > static_cast< unsigned long long >( INT64_MAX ) )
				pKV->SetUInt64( nValue );
			else
				pKV->SetInt64( static_cast< int64 >( nValue ) );
		}

		return true;
	}

	bool ParseKeywordOrTypedValue( KeyValues3 *pKV, int nDepth )
	{
		const char *pStart = m_pCur;

		while ( !AtEnd() && ( IsIdentifierChar( *m_pCur ) ) )
			++m_pCur;

		const int nLength = static_cast< int >( m_pCur - pStart );

		auto IsWord = [ & ]( const char *pszWord )
		{
			return static_cast< int >( strlen( pszWord ) ) == nLength && !memcmp( pStart, pszWord, nLength );
		};

		if ( IsWord( "null" ) )
		{
			pKV->SetToNull();
			return true;
		}

		if ( IsWord( "true" ) || IsWord( "false" ) )
		{
			pKV->SetBool( IsWord( "true" ) );
			return true;
		}

		if ( Peek() == ':' )
		{
			for ( const KV3CodecStringSubType_t &subType : s_StringSubTypes )
			{
				if ( !IsWord( subType.m_pszPrefix ) )
					continue;

				++m_pCur;

				if ( !SkipWhitespace() )
					return false;

				if ( subType.m_eSubType == KV3_SUBTYPE_SUBCLASS )
				{
					if ( Peek() != '{' )
						return Fail( "expected '{' after subclass:" );

					// The subclass subtype is not kept, the table reads back as a plain table
					return ParseTable( pKV, nDepth );
				}

				if ( Peek() != '"' )
					return Fail( "expected a string after %s:", subType.m_pszPrefix );

				return ParseString( pKV, subType.m_eSubType );
			}
		}

		m_pCur = pStart;

		return Fail( "unknown value '%.*s'", nLength, pStart );
	}

	const char *m_pBegin;
	const char *m_pCur;
	const char *m_pEnd;
	const char *m_pszName;
	CUtlString *m_pError;
	bool m_bFailed;

	CUtlVector< char > m_Scratch;
};

//--------------------------------------------------------------------------------------------------
// Text writer
//--------------------------------------------------------------------------------------------------
class CKV3TextWriter
{
public:
	CKV3TextWriter( CUtlBuffer &output ) : m_Output( output ) {}

	void Write( const char *pszText ) { m_Output.Put( pszText, static_cast< int >( strlen( pszText ) ) ); }
	void Write( const char *pText, int nLength ) { m_Output.Put( pText, nLength ); }
	void WriteChar( char c ) { m_Output.Put( &c, 1 ); }

	void WriteIndent( int nIndent )
	{
		for ( int i = 0; i < nIndent; ++i )
			WriteChar( '\t' );
	}

	void WriteFormat( const char *pszFormat, ... ) FMTFUNCTION( 2, 3 )
	{
		char szBuffer[ 128 ];

		va_list args;
		va_start( args, pszFormat );
		const int nLength = V_vsnprintf( szBuffer, sizeof( szBuffer ), pszFormat, args );
		va_end( args );

		Write( szBuffer, nLength );
	}

	void WriteQuoted( const char *pszString )
	{
		WriteChar( '"' );

		for ( const char *p = pszString; *p; ++p )
		{
			switch ( *p )
			{
				case '"': Write( "\\\"" ); break;
				case '\\': Write( "\\\\" ); break;
				case '\n': Write( "\\n" ); break;
				case '\t': Write( "\\t" ); break;
				case '\r': Write( "\\r" ); break;
				default: WriteChar( *p ); break;
			}
		}

		WriteChar( '"' );
	}

	// Shortest representation that reads back to the same double, always with a '.' or an exponent
	void WriteDouble( double flValue )
	{
		char szBuffer[ 64 ];

		// Whole numbers without an exponent, e.g. 10.0
		if ( flValue == static_cast< double >( static_cast< int64 >( flValue ) ) && flValue > -1e15 && flValue < 1e15 )
		{
			WriteFormat( "%.1f", flValue );
			return;
		}

		for ( int nPrecision = 1; nPrecision <= 17; ++nPrecision )
		{
			V_snprintf( szBuffer, sizeof( szBuffer ), "%.*g", nPrecision, flValue );

			if ( strtod( szBuffer, nullptr ) == flValue )
				break;
		}

		Write( szBuffer );

		if ( !strpbrk( szBuffer, ".eEn" ) )
			Write( ".0" );
	}

	static bool IsScalar( const KeyValues3 *pKV )
	{
		const KV3Type_t eType = pKV->GetType();

		return eType != KV3_TYPE_ARRAY && eType != KV3_TYPE_TABLE && !( eType == KV3_TYPE_STRING && pKV->HasFlag( KEYVALUES3_FLAG_MULTILINE_STRING ) );
	}

	// Written on the line of its member name
	static bool IsInline( const KeyValues3 *pKV )
	{
		switch ( pKV->GetType() )
		{
			case KV3_TYPE_TABLE:
				return pKV->GetMemberCount() <= 0;

			case KV3_TYPE_ARRAY:
			{
				const int nCount = pKV->GetArrayElementCount();

				if ( !pKV->IsKV3Array() || nCount <= 0 )
					return true;

				if ( nCount > 16 )
					return false;

				for ( int i = 0; i < nCount; ++i )
				{
					if ( !IsScalar( pKV->GetArrayElement( i ) ) )
						return false;
				}

				return true;
			}

			default:
				return true;
		}
	}

	void WriteString( const KeyValues3 *pKV, int nIndent )
	{
		const char *pszPrefix = SubTypePrefix( pKV->GetSubType() );

		if ( pszPrefix )
		{
			Write( pszPrefix );
			WriteChar( ':' );
		}

		const char *pszString = pKV->GetString();

		// The closing quotes start a line, tier0 rejects them indented
		if ( pKV->HasFlag( KEYVALUES3_FLAG_MULTILINE_STRING ) && !strstr( pszString, "\"\"\"" ) )
		{
			Write( "\"\"\"\n" );
			Write( pszString );
			Write( "\n\"\"\"" );
			return;
		}

		WriteQuoted( pszString );
	}

	void WritePackedArray( const KeyValues3 *pKV )
	{
		const KeyValues3Array_t *pArray = pKV->GetArray();
		const int nCount = pKV->GetArrayElementCount();

		Write( "[ " );

		for ( int i = 0; i < nCount; ++i )
		{
			if ( i )
				Write( ", " );

			switch ( pKV->GetTypeEx() )
			{
				case KV3_TYPEEX_ARRAY_FLOAT32: WriteDouble( pArray->m_f32[ i ] ); break;
				case KV3_TYPEEX_ARRAY_FLOAT64: WriteDouble( pArray->m_f64[ i ] ); break;
				case KV3_TYPEEX_ARRAY_INT16: WriteFormat( "%d", pArray->m_i16[ i ] ); break;
				case KV3_TYPEEX_ARRAY_INT32: WriteFormat( "%d", pArray->m_i32[ i ] ); break;
				case KV3_TYPEEX_ARRAY_UINT8_SHORT: WriteFormat( "%u", pArray->m_u8Short[ i ] ); break;
				case KV3_TYPEEX_ARRAY_INT16_SHORT: WriteFormat( "%d", pArray->m_i16Short[ i ] ); break;
				default: Write( "null" ); break;
			}
		}

		Write( nCount ? " ]" : "]" );
	}

	void WriteArray( const KeyValues3 *pKV, int nIndent )
	{
		if ( !pKV->IsKV3Array() )
		{
			WritePackedArray( pKV );
			return;
		}

		const int nCount = pKV->GetArrayElementCount();

		if ( !nCount )
		{
			Write( "[]" );
			return;
		}

		if ( IsInline( pKV ) )
		{
			Write( "[ " );

			for ( int i = 0; i < nCount; ++i )
			{
				if ( i )
					Write( ", " );

				WriteValue( pKV->GetArrayElement( i ), nIndent );
			}

			Write( " ]" );
			return;
		}

		Write( "[\n" );

		for ( int i = 0; i < nCount; ++i )
		{
			WriteIndent( nIndent + 1 );
			WriteValue( pKV->GetArrayElement( i ), nIndent + 1 );
			Write( ",\n" );
		}

		WriteIndent( nIndent );
		WriteChar( ']' );
	}

	void WriteTable( const KeyValues3 *pKV, int nIndent )
	{
		const int nCount = pKV->GetMemberCount();

		if ( pKV->GetSubType() == KV3_SUBTYPE_SUBCLASS )
			Write( "subclass:" );

		if ( nCount <= 0 )
		{
			Write( "{}" );
			return;
		}

		Write( "{\n" );

		for ( KV3MemberId_t id = 0; id < nCount; ++id )
		{
			const char *pszName = pKV->GetMemberName( id );
			const KeyValues3 *pMember = pKV->GetMember( id );

			WriteIndent( nIndent + 1 );

			if ( IsBareMemberName( pszName ) )
				Write( pszName );
			else
				WriteQuoted( pszName );

			if ( IsInline( pMember ) )
			{
				Write( " = " );
			}
			else
			{
				Write( " =\n" );
				WriteIndent( nIndent + 1 );
			}

			WriteValue( pMember, nIndent + 1 );
			WriteChar( '\n' );
		}

		WriteIndent( nIndent );
		WriteChar( '}' );
	}

	void WriteValue( const KeyValues3 *pKV, int nIndent )
	{
		switch ( pKV->GetType() )
		{
			case KV3_TYPE_BOOL: Write( pKV->GetBool() ? "true" : "false" ); break;
			case KV3_TYPE_INT: WriteFormat( "%" PRId64, static_cast< int64_t >( pKV->GetInt64() ) ); break;
			case KV3_TYPE_UINT: WriteFormat( "%" PRIu64, static_cast< uint64_t >( pKV->GetUInt64() ) ); break;
			case KV3_TYPE_DOUBLE: WriteDouble( pKV->GetDouble() ); break;
			case KV3_TYPE_STRING: WriteString( pKV, nIndent ); break;
			case KV3_TYPE_ARRAY: WriteArray( pKV, nIndent ); break;
			case KV3_TYPE_TABLE: WriteTable( pKV, nIndent ); break;

			case KV3_TYPE_BINARY_BLOB:
			{
				const byte *pBlob = pKV->GetBinaryBlob();
				const int nSize = pKV->GetBinaryBlobSize();

				Write( "#[" );

				for ( int i = 0; i < nSize; ++i )
					WriteFormat( " %02X", pBlob[ i ] );

				Write( nSize ? " ]" : "]" );
				break;
			}

			default: Write( "null" ); break;
		}
	}

private:
	CUtlBuffer &m_Output;
};

void WriteKV3ID( CKV3TextWriter &writer, const char *pszKind, const KV3ID_t &id )
{
	const uint32 nData1 = static_cast< uint32 >( id.m_data1 );
	const uint16 nData2 = static_cast< uint16 >( id.m_data1 >> 32 ), nData3 = static_cast< uint16 >( id.m_data1 >> 48 );

	writer.WriteFormat( "%s:%s:version{%08x-%04x-%04x-", pszKind, id.m_name, nData1, nData2, nData3 );

	for ( int i = 0; i < 8; ++i )
	{
		if ( i == 2 )
			writer.WriteChar( '-' );

		writer.WriteFormat( "%02x", static_cast< uint8 >( id.m_data2 >> ( i * 8 ) ) );
	}

	writer.WriteChar( '}' );
}

//--------------------------------------------------------------------------------------------------
// Binary encoding
//--------------------------------------------------------------------------------------------------
const char s_szBinaryMagic[ 4 ] = { 'K', 'V', '3', 'b' };

enum
{
	KV3_CODEC_BINARY_VERSION = 1,
};

struct KV3CodecStringHash_t
{
	unsigned int operator()( const char *pszString ) const { return MurmurHash2( pszString, static_cast< int >( strlen( pszString ) ), 0x31415926 ); }
};

struct KV3CodecStringEqual_t
{
	bool operator()( const char *pszA, const char *pszB ) const { return !V_strcmp( pszA, pszB ); }
};

class CKV3BinaryWriter
{
public:
	CKV3BinaryWriter( CUtlBuffer &output ) : m_Output( output ) {}

	void Save( const KeyValues3 *pKV )
	{
		CollectStrings( pKV );

		m_Output.Put( s_szBinaryMagic, sizeof( s_szBinaryMagic ) );
		PutUInt32( KV3_CODEC_BINARY_VERSION );
		PutUInt32( m_Strings.Count() );

		for ( const char *pszString : m_Strings )
		{
			const uint32 nLength = static_cast< uint32 >( strlen( pszString ) );

			PutUInt32( nLength );
			m_Output.Put( pszString, nLength );
		}

		WriteValue( pKV );
	}

private:
	void PutUInt32( uint32 nValue )
	{
		uint8 bytes[ 4 ];

		for ( int i = 0; i < 4; ++i )
			bytes[ i ] = static_cast< uint8 >( nValue >> ( i * 8 ) );

		m_Output.Put( bytes, sizeof( bytes ) );
	}

	void PutUInt64( uint64 nValue )
	{
		uint8 bytes[ 8 ];

		for ( int i = 0; i < 8; ++i )
			bytes[ i ] = static_cast< uint8 >( nValue >> ( i * 8 ) );

		m_Output.Put( bytes, sizeof( bytes ) );
	}

	uint32 AddString( const char *pszString )
	{
		bool bInserted = false;
		const UtlHashHandle_t h = m_StringIndices.Insert( pszString, m_Strings.Count(), &bInserted );

		if ( bInserted )
			m_Strings.AddToTail( pszString );

		return static_cast< uint32 >( m_StringIndices.Element( h ) );
	}

	void CollectStrings( const KeyValues3 *pKV )
	{
		switch ( pKV->GetType() )
		{
			case KV3_TYPE_STRING:
			{
				AddString( pKV->GetString() );
				break;
			}

			case KV3_TYPE_ARRAY:
			{
				if ( pKV->IsKV3Array() )
				{
					for ( int i = 0; i < pKV->GetArrayElementCount(); ++i )
						CollectStrings( pKV->GetArrayElement( i ) );
				}

				break;
			}

			case KV3_TYPE_TABLE:
			{
				for ( KV3MemberId_t id = 0; id < pKV->GetMemberCount(); ++id )
				{
					AddString( pKV->GetMemberName( id ) );
					CollectStrings( pKV->GetMember( id ) );
				}

				break;
			}

			default:
				break;
		}
	}

	void WriteHeader( KV3TypeEx_t eTypeEx, KV3SubType_t eSubType, uint8 nFlags )
	{
		const uint8 header[ 3 ] = { static_cast< uint8 >( eTypeEx ), static_cast< uint8 >( eSubType ), nFlags };

		m_Output.Put( header, sizeof( header ) );
	}

	void WriteValue( const KeyValues3 *pKV )
	{
		const uint8 nFlags = static_cast< uint8 >( pKV->GetAllFlags() );

		switch ( pKV->GetType() )
		{
			case KV3_TYPE_BOOL:
			{
				WriteHeader( KV3_TYPEEX_BOOL, pKV->GetSubType(), nFlags );

				const uint8 nValue = pKV->GetBool() ? 1 : 0;
				m_Output.Put( &nValue, 1 );
				break;
			}

			case KV3_TYPE_INT:
			{
				WriteHeader( KV3_TYPEEX_INT, pKV->GetSubType(), nFlags );
				PutUInt64( static_cast< uint64 >( pKV->GetInt64() ) );
				break;
			}

			case KV3_TYPE_UINT:
			{
				// Pointers are process-local, keep them as plain unsigned values
				WriteHeader( KV3_TYPEEX_UINT, pKV->GetSubType() == KV3_SUBTYPE_POINTER ? KV3_SUBTYPE_UINT64 : pKV->GetSubType(), nFlags );
				PutUInt64( pKV->GetSubType() == KV3_SUBTYPE_POINTER ? reinterpret_cast< uintp >( pKV->GetPointer() ) : pKV->GetUInt64() );
				break;
			}

			case KV3_TYPE_DOUBLE:
			{
				const double flValue = pKV->GetDouble();
				uint64 nBits;

				memcpy( &nBits, &flValue, sizeof( nBits ) );

				WriteHeader( KV3_TYPEEX_DOUBLE, pKV->GetSubType(), nFlags );
				PutUInt64( nBits );
				break;
			}

			case KV3_TYPE_STRING:
			{
				WriteHeader( KV3_TYPEEX_STRING, pKV->GetSubType(), nFlags );
				PutUInt32( AddString( pKV->GetString() ) );
				break;
			}

			case KV3_TYPE_BINARY_BLOB:
			{
				const int nSize = pKV->GetBinaryBlobSize();

				WriteHeader( KV3_TYPEEX_BINARY_BLOB, pKV->GetSubType(), nFlags );
				PutUInt32( static_cast< uint32 >( nSize ) );
				m_Output.Put( pKV->GetBinaryBlob(), nSize );
				break;
			}

			case KV3_TYPE_ARRAY:
			{
				WriteArray( pKV, nFlags );
				break;
			}

			case KV3_TYPE_TABLE:
			{
				const int nCount = pKV->GetMemberCount();

				WriteHeader( KV3_TYPEEX_TABLE, pKV->GetSubType(), nFlags );
				PutUInt32( static_cast< uint32 >( nCount ) );

				for ( KV3MemberId_t id = 0; id < nCount; ++id )
				{
					PutUInt32( AddString( pKV->GetMemberName( id ) ) );
					WriteValue( pKV->GetMember( id ) );
				}

				break;
			}

			default:
			{
				WriteHeader( KV3_TYPEEX_NULL, KV3_SUBTYPE_NULL, nFlags );
				break;
			}
		}
	}

	// Packed arrays are written element by element and read back as KV3 arrays
	void WriteArray( const KeyValues3 *pKV, uint8 nFlags )
	{
		const int nCount = pKV->GetArrayElementCount();

		WriteHeader( KV3_TYPEEX_ARRAY, pKV->GetSubType(), nFlags );
		PutUInt32( static_cast< uint32 >( nCount ) );

		if ( pKV->IsKV3Array() )
		{
			for ( int i = 0; i < nCount; ++i )
				WriteValue( pKV->GetArrayElement( i ) );

			return;
		}

		const KeyValues3Array_t *pArray = pKV->GetArray();

		for ( int i = 0; i < nCount; ++i )
		{
			switch ( pKV->GetTypeEx() )
			{
				case KV3_TYPEEX_ARRAY_FLOAT32:
				case KV3_TYPEEX_ARRAY_FLOAT64:
				{
					const double flValue = pKV->GetTypeEx() == KV3_TYPEEX_ARRAY_FLOAT32 ? pArray->m_f32[ i ] : pArray->m_f64[ i ];
					uint64 nBits;

					memcpy( &nBits, &flValue, sizeof( nBits ) );

					WriteHeader( KV3_TYPEEX_DOUBLE, pKV->GetTypeEx() == KV3_TYPEEX_ARRAY_FLOAT32 ? KV3_SUBTYPE_FLOAT32 : KV3_SUBTYPE_FLOAT64, 0 );
					PutUInt64( nBits );
					break;
				}

				default:
				{
					int64 nValue = 0;

					switch ( pKV->GetTypeEx() )
					{
						case KV3_TYPEEX_ARRAY_INT16: nValue = pArray->m_i16[ i ]; break;
						case KV3_TYPEEX_ARRAY_INT32: nValue = pArray->m_i32[ i ]; break;
						case KV3_TYPEEX_ARRAY_UINT8_SHORT: nValue = pArray->m_u8Short[ i ]; break;
						case KV3_TYPEEX_ARRAY_INT16_SHORT: nValue = pArray->m_i16Short[ i ]; break;
						default: break;
					}

					WriteHeader( KV3_TYPEEX_INT, KV3_SUBTYPE_INT64, 0 );
					PutUInt64( static_cast< uint64 >( nValue ) );
					break;
				}
			}
		}
	}

	CUtlBuffer &m_Output;
	CUtlVector< const char * > m_Strings;
	CUtlHashtable< const char *, int, KV3CodecStringHash_t, KV3CodecStringEqual_t > m_StringIndices;
};

class CKV3BinaryReader
{
public:
	CKV3BinaryReader( const void *pInput, int nLength, const char *pszName, CUtlString *pError ) :
		m_pBegin( static_cast< const uint8 * >( pInput ) ),
		m_pCur( static_cast< const uint8 * >( pInput ) ),
		m_pEnd( static_cast< const uint8 * >( pInput ) + nLength ),
		m_pszName( pszName ? pszName : "<kv3>" ),
		m_pError( pError )
	{
	}

	bool Load( KeyValues3 *pKV )
	{
		if ( m_pEnd - m_pCur < 12 || memcmp( m_pCur, s_szBinaryMagic, sizeof( s_szBinaryMagic ) ) )
			return Fail( "not a kv3lib binary encoding" );

		m_pCur += sizeof( s_szBinaryMagic );

		uint32 nVersion, nStrings;

		if ( !GetUInt32( nVersion ) || nVersion != KV3_CODEC_BINARY_VERSION )
			return Fail( "unsupported binary encoding version %u", nVersion );

		if ( !GetUInt32( nStrings ) || nStrings > static_cast< uint32 >( m_pEnd - m_pCur ) )
			return Fail( "corrupt string table" );

		CKV3Arena *pArena = pKV->GetContext();

		m_Strings.EnsureCapacity( nStrings );
		m_Interned.EnsureCapacity( nStrings );
		m_Storage.EnsureCapacity( nStrings );

		CUtlVector< char > scratch;

		for ( uint32 i = 0; i < nStrings; ++i )
		{
			uint32 nLength;

			if ( !GetUInt32( nLength ) || nLength > static_cast< uint32 >( m_pEnd - m_pCur ) )
				return Fail( "corrupt string %u", i );

			scratch.SetCount( nLength + 1 );
			memcpy( scratch.Base(), m_pCur, nLength );
			scratch[ nLength ] = '\0';
			m_pCur += nLength;

			if ( pArena && CanIntern( scratch.Base(), static_cast< int >( nLength ) ) )
			{
				m_Strings.AddToTail( pArena->AllocString( scratch.Base() ) );
				m_Interned.AddToTail( true );
			}
			else
			{
				// Copied by the values that use them, the storage only has to outlive the load
				m_Storage.AddToTail( CUtlString( scratch.Base() ) );
				m_Strings.AddToTail( m_Storage.Tail().Get() );
				m_Interned.AddToTail( false );
			}
		}

		if ( !ReadValue( pKV, 0 ) )
			return false;

		if ( m_pCur != m_pEnd )
			return Fail( "trailing data" );

		return true;
	}

private:
	bool Fail( const char *pszFormat, ... ) FMTFUNCTION( 2, 3 )
	{
		if ( m_pError )
		{
			char szMessage[ 256 ];

			va_list args;
			va_start( args, pszFormat );
			V_vsnprintf( szMessage, sizeof( szMessage ), pszFormat, args );
			va_end( args );

			m_pError->Format( "%s:%d: %s", m_pszName, static_cast< int >( m_pCur - m_pBegin ), szMessage );
		}

		return false;
	}

	bool GetUInt32( uint32 &nValue )
	{
		if ( m_pEnd - m_pCur < 4 )
			return false;

		nValue = 0;

		for ( int i = 0; i < 4; ++i )
			nValue |= static_cast< uint32 >( m_pCur[ i ] ) << ( i * 8 );

		m_pCur += 4;

		return true;
	}

	bool GetUInt64( uint64 &nValue )
	{
		if ( m_pEnd - m_pCur < 8 )
			return false;

		nValue = 0;

		for ( int i = 0; i < 8; ++i )
			nValue |= static_cast< uint64 >( m_pCur[ i ] ) << ( i * 8 );

		m_pCur += 8;

		return true;
	}

	bool GetString( const char *&pszString, bool *pInterned = nullptr )
	{
		uint32 nIndex;

		if ( !GetUInt32( nIndex ) || nIndex >= static_cast< uint32 >( m_Strings.Count() ) )
			return false;

		pszString = m_Strings[ nIndex ];

		if ( pInterned )
			*pInterned = m_Interned[ nIndex ];

		return true;
	}

	bool ReadValue( KeyValues3 *pKV, int nDepth )
	{
		if ( nDepth > 512 || m_pEnd - m_pCur < 3 )
			return Fail( "truncated value" );

		const KV3TypeEx_t eTypeEx = static_cast< KV3TypeEx_t >( m_pCur[ 0 ] );
		const KV3SubType_t eSubType = static_cast< KV3SubType_t >( m_pCur[ 1 ] );
		const uint8 nFlags = m_pCur[ 2 ];

		m_pCur += 3;

		switch ( eTypeEx )
		{
			case KV3_TYPEEX_NULL:
			{
				pKV->SetToNull();
				break;
			}

			case KV3_TYPEEX_BOOL:
			{
				if ( m_pCur >= m_pEnd )
					return Fail( "truncated bool" );

				pKV->SetBool( *m_pCur++ != 0 );
				break;
			}

			case KV3_TYPEEX_INT:
			case KV3_TYPEEX_UINT:
			case KV3_TYPEEX_DOUBLE:
			{
				uint64 nBits;

				if ( !GetUInt64( nBits ) )
					return Fail( "truncated number" );

				SetNumber( pKV, eTypeEx, eSubType, nBits );
				break;
			}

			case KV3_TYPEEX_STRING:
			{
				const char *pszString;
				bool bInterned = false;

				if ( !GetString( pszString, &bInterned ) )
					return Fail( "bad string index" );

				// Strings of the arena symbol table are shared, the others are copied
				if ( bInterned )
					pKV->SetStringExternal( pszString, eSubType );
				else
					pKV->SetString( pszString, eSubType );

				break;
			}

			case KV3_TYPEEX_BINARY_BLOB:
			{
				uint32 nSize;

				if ( !GetUInt32( nSize ) || nSize > static_cast< uint32 >( m_pEnd - m_pCur ) )
					return Fail( "truncated binary blob" );

				pKV->SetToBinaryBlob( m_pCur, static_cast< int >( nSize ) );
				m_pCur += nSize;
				break;
			}

			case KV3_TYPEEX_ARRAY:
			{
				uint32 nCount;

				if ( !GetUInt32( nCount ) || nCount > static_cast< uint32 >( m_pEnd - m_pCur ) / 3 )
					return Fail( "truncated array" );

				pKV->SetToEmptyKV3Array();

				for ( uint32 i = 0; i < nCount; ++i )
				{
					if ( !ReadValue( pKV->ArrayAddElementToTail(), nDepth + 1 ) )
						return false;
				}

				break;
			}

			case KV3_TYPEEX_TABLE:
			{
				uint32 nCount;

				if ( !GetUInt32( nCount ) || nCount > static_cast< uint32 >( m_pEnd - m_pCur ) / 7 )
					return Fail( "truncated table" );

				pKV->SetToEmptyTable();

				for ( uint32 i = 0; i < nCount; ++i )
				{
					const char *pszName;

					if ( !GetString( pszName ) )
						return Fail( "bad member name index" );

					if ( !ReadValue( pKV->FindOrCreateMember( CKV3MemberName( pszName, static_cast< int >( strlen( pszName ) ) ) ), nDepth + 1 ) )
						return false;
				}

				break;
			}

			default:
				return Fail( "unknown value type %d", eTypeEx );
		}

		pKV->SetAllFlags( static_cast< KeyValues3Flag_t >( nFlags ) );

		return true;
	}

	static void SetNumber( KeyValues3 *pKV, KV3TypeEx_t eTypeEx, KV3SubType_t eSubType, uint64 nBits )
	{
		if ( eTypeEx == KV3_TYPEEX_DOUBLE )
		{
			double flValue;

			memcpy( &flValue, &nBits, sizeof( flValue ) );

			if ( eSubType == KV3_SUBTYPE_FLOAT32 )
				pKV->SetFloat( static_cast< float32 >( flValue ) );
			else
				pKV->SetDouble( flValue );

			return;
		}

		switch ( eSubType )
		{
			case KV3_SUBTYPE_BOOL8: pKV->SetBool( nBits != 0 ); break;
			case KV3_SUBTYPE_CHAR8: pKV->SetChar( static_cast< char8 >( nBits ) ); break;
			case KV3_SUBTYPE_UCHAR32: pKV->SetUChar32( static_cast< uchar32 >( nBits ) ); break;
			case KV3_SUBTYPE_INT8: pKV->SetInt8( static_cast< int8 >( nBits ) ); break;
			case KV3_SUBTYPE_UINT8: pKV->SetUInt8( static_cast< uint8 >( nBits ) ); break;
			case KV3_SUBTYPE_INT16: pKV->SetShort( static_cast< int16 >( nBits ) ); break;
			case KV3_SUBTYPE_UINT16: pKV->SetUShort( static_cast< uint16 >( nBits ) ); break;
			case KV3_SUBTYPE_INT32: pKV->SetInt( static_cast< int32 >( nBits ) ); break;
			case KV3_SUBTYPE_UINT32: pKV->SetUInt( static_cast< uint32 >( nBits ) ); break;
			case KV3_SUBTYPE_STRING_TOKEN: pKV->SetStringToken( CUtlStringToken( static_cast< uint32 >( nBits ) ) ); break;
			case KV3_SUBTYPE_EHANDLE: pKV->SetEHandle( CEntityHandle( static_cast< uint32 >( nBits ) ) ); break;

			default:
			{
				if ( eTypeEx == KV3_TYPEEX_INT )
					pKV->SetInt64( static_cast< int64 >( nBits ) );
				else
					pKV->SetUInt64( nBits );

				break;
			}
		}
	}

	const uint8 *m_pBegin;
	const uint8 *m_pCur;
	const uint8 *m_pEnd;
	const char *m_pszName;
	CUtlString *m_pError;

	CUtlVector< const char * > m_Strings;
	CUtlVector< bool > m_Interned;
	CUtlVector< CUtlString > m_Storage;
};

} // namespace

bool KV3Codec_LoadText( KeyValues3 *pKV, CUtlString *pError, const char *pszInput, int nInputLength, const char *pszName )
{
	return CKV3TextParser( pszInput, nInputLength, pszName, pError ).Parse( pKV );
}

bool KV3Codec_LoadText( KeyValues3 *pKV, CUtlString *pError, const CUtlBuffer &input, const char *pszName )
{
	return KV3Codec_LoadText( pKV, pError, static_cast< const char * >( input.Base() ), input.TellPut(), pszName );
}

bool KV3Codec_SaveText( const KeyValues3 *pKV, CUtlBuffer &output, const KV3ID_t &format, uint nFlags )
{
	CKV3TextWriter writer( output );

	if ( !( nFlags & KV3_CODEC_SAVE_NO_HEADER ) )
	{
		writer.Write( "<!-- kv3 " );
		WriteKV3ID( writer, "encoding", g_KV3Encoding_Text );
		writer.WriteChar( ' ' );
		WriteKV3ID( writer, "format", format );
		writer.Write( " -->\n" );
	}

	writer.WriteValue( pKV, 0 );
	writer.WriteChar( '\n' );

	return true;
}

bool KV3Codec_LoadBinary( KeyValues3 *pKV, CUtlString *pError, const void *pInput, int nInputLength, const char *pszName )
{
	return CKV3BinaryReader( pInput, nInputLength, pszName, pError ).Load( pKV );
}

bool KV3Codec_SaveBinary( const KeyValues3 *pKV, CUtlBuffer &output )
{
	CKV3BinaryWriter( output ).Save( pKV );

	return true;
}

bool KV3Codec_Load( KeyValues3 *pKV, CUtlString *pError, const void *pInput, int nInputLength, const char *pszName )
{
	if ( nInputLength >= static_cast< int >( sizeof( s_szBinaryMagic ) ) && !memcmp( pInput, s_szBinaryMagic, sizeof( s_szBinaryMagic ) ) )
		return KV3Codec_LoadBinary( pKV, pError, pInput, nInputLength, pszName );

	return KV3Codec_LoadText( pKV, pError, static_cast< const char * >( pInput ), nInputLength, pszName );
}
