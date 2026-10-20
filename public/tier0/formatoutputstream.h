#ifndef FORMATOUTPUTSTREAM_H
#define FORMATOUTPUTSTREAM_H

#ifdef _WIN32
#pragma once
#endif

#include <stdio.h>
#include <initializer_list>
#include <string_view>
#include <utility>

#include "tier0/platform.h"
#include "tier0/tier0_stdlib.h"

class CBufferString;
class CUtlBuffer;
class CUtlString;
class CTypeErasedFormattingWrapper;

//-----------------------------------------------------------------------------
// One printf-style conversion of a format string, such as "%-8.3f"
//-----------------------------------------------------------------------------
class CFormatStringElement
{
public:
	CFormatStringElement() : unk000( 0 ) {}
	DLL_CLASS_IMPORT CFormatStringElement( const char *pszElement );

	// Parses the conversion after the '%', returns the text after it
	DLL_CLASS_IMPORT const char *ParseElement( const char *pszFormat );

	// Replaces the conversion type with the first of pszTypes when it is not one of them,
	// and the length modifier with pszSize
	DLL_CLASS_IMPORT void EnforceTypeAndSize( const char *pszTypes, const char *pszSize );

	operator const char *() const { return m_szElement; }

private:
	uint16 unk000;
	char m_szElement[ 12 ];
};

//-----------------------------------------------------------------------------
// Output of the type-erased formatting
//-----------------------------------------------------------------------------
class IFormatOutputStream
{
public:
	enum Flags_t
	{
		FORMAT_UNICODE_OK = ( 1 << 0 ),
		FORMAT_HTML = ( 1 << 1 ),
		FORMAT_MULTI_LINE = ( 1 << 2 ),
	};

	IFormatOutputStream() : m_nFlags( 0 ), m_nCharsWritten( 0 ), unk010( 64 ), unk014( 1 ), unk018( 0x7FFFFFFF ) {}
	virtual ~IFormatOutputStream() {}

	virtual void PutC( char c ) = 0;
	DLL_CLASS_IMPORT virtual void PutS( std::string_view sv );
	DLL_CLASS_IMPORT virtual void PutRepeatedChar( char c, int nCount );
	// "{ a, b }" with "..." for a null element, bTruncated adds " ... " before the brace
	DLL_CLASS_IMPORT virtual void FormatArray( CTypeErasedFormattingWrapper wrapper, CFormatStringElement element, const void **ppElements, int nCount, bool bTruncated );
	DLL_CLASS_IMPORT virtual void Format2DTable( CTypeErasedFormattingWrapper wrapper, CFormatStringElement element, const void **ppElements, int nDim0, int nDim1 );
	DLL_CLASS_IMPORT virtual void FormatEnumHelper( CFormatStringElement element, int nValue, std::initializer_list< std::pair< int, const char * > > names );
	DLL_CLASS_IMPORT virtual void FormatMaskHelper( int nValue, std::initializer_list< std::pair< int, const char * > > names );
	virtual void Terminate() {}
	virtual void BeginItem() {}
	virtual void EndItem() {}
	virtual int RemainingCapacity() const { return 0x7FFFFFFF; }
	virtual bool IsFull() const { return false; }
	DLL_CLASS_IMPORT virtual void OutputIndentation();
	virtual void SetTargetLine( int nLine ) {}
	virtual void SetAnchor() {}

	bool UnicodeOK() const { return ( m_nFlags & FORMAT_UNICODE_OK ) != 0; }
	bool IsHTML() const { return ( m_nFlags & FORMAT_HTML ) != 0; }
	bool IsMultiLine() const { return ( m_nFlags & FORMAT_MULTI_LINE ) != 0; }
	bool IsNonFull() const { return !IsFull(); }

	void NewLineAndIndent()
	{
		PutC( '\n' );
		OutputIndentation();
	}

	int GetCharsWritten() const { return m_nCharsWritten; }

protected:
	uint32 m_nFlags;
	int m_nCharsWritten;
	int unk010;
	int unk014;
	int unk018;
};

//-----------------------------------------------------------------------------
// Writes to a fixed char buffer, always leaving room for the terminator
//-----------------------------------------------------------------------------
class CCharBufferFormatOutputStream : public IFormatOutputStream
{
public:
	CCharBufferFormatOutputStream( char *pBuffer, int nBufferSize ) : m_pCurrent( pBuffer ), m_pEnd( pBuffer + nBufferSize ) {}
	virtual ~CCharBufferFormatOutputStream() {}

	DLL_CLASS_IMPORT virtual void PutC( char c );
	DLL_CLASS_IMPORT virtual void PutS( std::string_view sv );
	DLL_CLASS_IMPORT virtual void Terminate();
	DLL_CLASS_IMPORT virtual int RemainingCapacity() const;
	DLL_CLASS_IMPORT virtual bool IsFull() const;

private:
	char *m_pCurrent;
	char *m_pEnd;
};

//-----------------------------------------------------------------------------
// Writes to a stdio file
//-----------------------------------------------------------------------------
class CStdIOFormatOutputStream : public IFormatOutputStream
{
public:
	CStdIOFormatOutputStream( FILE *pFile, bool bFlush ) : m_pFile( pFile ), m_bFlush( bFlush ) {}
	virtual ~CStdIOFormatOutputStream() {}

	DLL_CLASS_IMPORT virtual void PutC( char c );
	DLL_CLASS_IMPORT virtual void PutS( std::string_view sv );

	void FlushIfDesired()
	{
		if ( m_bFlush )
		{
			V_fflush( m_pFile );
		}
	}

private:
	FILE *m_pFile;
	bool m_bFlush;
};

//-----------------------------------------------------------------------------
// Appends to a CUtlBuffer
//-----------------------------------------------------------------------------
class CUtlBufferFormatOutputStream : public IFormatOutputStream
{
public:
	CUtlBufferFormatOutputStream( CUtlBuffer &buffer ) : m_pBuffer( &buffer ) {}
	virtual ~CUtlBufferFormatOutputStream() {}

	DLL_CLASS_IMPORT virtual void PutC( char c );

private:
	CUtlBuffer *m_pBuffer;
};

//-----------------------------------------------------------------------------
// Appends to a CBufferString
//-----------------------------------------------------------------------------
class CBufferStringFormatOutputStream : public IFormatOutputStream
{
public:
	CBufferStringFormatOutputStream() : m_pString( nullptr ) {}
	CBufferStringFormatOutputStream( CBufferString &string ) : m_pString( &string ) {}
	virtual ~CBufferStringFormatOutputStream() {}

	DLL_CLASS_IMPORT virtual void PutC( char c );

	void SetString( CBufferString &string ) { m_pString = &string; }

private:
	CBufferString *m_pString;
};

// Changes the indentation level of the thread that OutputIndentation() writes
PLATFORM_OVERLOAD void Formatting_AdjustIndentation( int nDelta );

// Writes pszFormat to pOutputStream, formatting the next of the nArgs arguments at each conversion
PLATFORM_OVERLOAD void FormatArgumentsToBuffer( IFormatOutputStream *pOutputStream, const char *pszFormat, const CTypeErasedFormattingWrapper *pArgs, int nArgs );

// The formatting of the basic types with the conversion of element: bool and char are written
// as is, a null string as "*nullptr*"
PLATFORM_OVERLOAD void FormatTo( IFormatOutputStream *pOutputStream, CFormatStringElement element, bool bValue );
PLATFORM_OVERLOAD void FormatTo( IFormatOutputStream *pOutputStream, CFormatStringElement element, char cValue );
PLATFORM_OVERLOAD void FormatTo( IFormatOutputStream *pOutputStream, CFormatStringElement element, int nValue );
PLATFORM_OVERLOAD void FormatTo( IFormatOutputStream *pOutputStream, CFormatStringElement element, unsigned int nValue );
PLATFORM_OVERLOAD void FormatTo( IFormatOutputStream *pOutputStream, CFormatStringElement element, long nValue );
PLATFORM_OVERLOAD void FormatTo( IFormatOutputStream *pOutputStream, CFormatStringElement element, unsigned long nValue );
PLATFORM_OVERLOAD void FormatTo( IFormatOutputStream *pOutputStream, CFormatStringElement element, long long nValue );
PLATFORM_OVERLOAD void FormatTo( IFormatOutputStream *pOutputStream, CFormatStringElement element, unsigned long long nValue );
PLATFORM_OVERLOAD void FormatTo( IFormatOutputStream *pOutputStream, CFormatStringElement element, float flValue );
PLATFORM_OVERLOAD void FormatTo( IFormatOutputStream *pOutputStream, CFormatStringElement element, double flValue );
PLATFORM_OVERLOAD void FormatTo( IFormatOutputStream *pOutputStream, CFormatStringElement element, const void *pValue );
PLATFORM_OVERLOAD void FormatTo( IFormatOutputStream *pOutputStream, CFormatStringElement element, const char *pszValue );
PLATFORM_OVERLOAD void FormatTo( IFormatOutputStream *pOutputStream, CFormatStringElement element, const wchar_t *pwszValue );
PLATFORM_OVERLOAD void FormatTo( IFormatOutputStream *pOutputStream, CFormatStringElement element, std::string_view sv );
PLATFORM_OVERLOAD void FormatTo( IFormatOutputStream *pOutputStream, CFormatStringElement element, const CUtlString &value );

#endif // FORMATOUTPUTSTREAM_H
