#ifndef TIER0_TIER0_UNICODE_H
#define TIER0_TIER0_UNICODE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/basetypes.h"

// Unicode string conversion policies - what to do if an illegal sequence is encountered
enum EStringConvertErrorPolicy
{
	_STRINGCONVERTFLAG_SKIP =		1,
	_STRINGCONVERTFLAG_FAIL =		2,
	_STRINGCONVERTFLAG_ASSERT =		4,
	_STRINGCONVERTFLAG_UNK001 =		8,

	STRINGCONVERT_REPLACE =			0,
	STRINGCONVERT_SKIP =			_STRINGCONVERTFLAG_SKIP,
	STRINGCONVERT_FAIL =			_STRINGCONVERTFLAG_FAIL,

	STRINGCONVERT_ASSERT_REPLACE =	_STRINGCONVERTFLAG_ASSERT + STRINGCONVERT_REPLACE,
	STRINGCONVERT_ASSERT_SKIP =		_STRINGCONVERTFLAG_ASSERT + STRINGCONVERT_SKIP,
	STRINGCONVERT_ASSERT_FAIL =		_STRINGCONVERTFLAG_ASSERT + STRINGCONVERT_FAIL,
};

//
// This utility class is for performing UTF-8 <-> UTF-16 conversion.
// It is intended for use with function/method parameters.
//
// For example, you can call
//     FunctionTakingUTF16( CStrAutoEncode( utf8_string ).ToUChar16() )
// or
//     FunctionTakingUTF8( CStrAutoEncode( utf16_string ).ToString() )
//
// The converted string is allocated off the heap, and destroyed when
// the object goes out of scope.
//
// if the string cannot be converted, NULL is returned.
//
// This class doesn't have any conversion operators; the intention is
// to encourage the developer to get used to having to think about which
// encoding is desired.
//
class CStrAutoEncode
{
public:
	explicit CStrAutoEncode( const char *buf )
	{
		InitEmpty();

		m_pch = buf;
		m_bHasUTF8 = true;
	}

	explicit CStrAutoEncode( const uchar16 *buf )
	{
		InitEmpty();

		m_char16 = buf;
		m_bHasUTF16 = true;
	}

	explicit CStrAutoEncode( const uchar32 *buf )
	{
		InitEmpty();

		m_char32 = buf;
		m_bHasUTF32 = true;
	}

	CStrAutoEncode( const CStrAutoEncode &other )
	{
		Copy( other );
	}

	// returns the UTF-8 string, converting on the fly.
	const char *ToUTF8() { return ToString(); }
	const char* ToString()
	{
		PopulateUTF8();
		return m_pch;
	}

	// returns the UTF-16 string, converting on the fly.
	const uchar16 *ToUTF16() { return ToUChar16(); }
	const uchar16 *ToUChar16()
	{
		PopulateUTF16();
		return m_char16;
	}

	// returns the UTF-32 string, converting on the fly.
	const uchar32 *ToUTF32() { return ToUChar32(); }
	const uchar32 *ToUChar32()
	{
		PopulateUTF32();
		return m_char32;
	}

	~CStrAutoEncode()
	{
		Clear();
	}

	CStrAutoEncode& operator =(CStrAutoEncode& other)
	{
		Copy( other );
		return *this;
	}

	PLATFORM_CLASS void Clear();
	PLATFORM_CLASS CStrAutoEncode &Copy( const CStrAutoEncode &other );

	// Creates a copy and stores it
	PLATFORM_CLASS void SetCopy( const char *buf, int nCount = -1 );
	PLATFORM_CLASS void SetCopy( const uchar16 *buf, int nCount = -1 );
	PLATFORM_CLASS void SetCopy( const uchar32 *buf, int nCount = -1 );

	// Creates a copy and coverts to UTF8 then stores it
	PLATFORM_CLASS void SetUTF8Copy( const uchar32 *buf );
	PLATFORM_CLASS void SetUTF8Copy( const uchar16 *buf );

	// Creates a copy and coverts to UTF16 then stores it
	PLATFORM_CLASS void SetUTF16Copy( const char *buf );
	PLATFORM_CLASS void SetUTF16Copy( const uchar32 *buf );

	// Creates a copy and coverts to UTF32 then stores it
	PLATFORM_CLASS void SetUTF32Copy( const char *buf );
	PLATFORM_CLASS void SetUTF32Copy( const uchar16 *buf );

private:
	PLATFORM_CLASS void InitEmpty();

	PLATFORM_CLASS void PopulateUTF8() const;
	PLATFORM_CLASS void PopulateUTF16() const;
	PLATFORM_CLASS void PopulateUTF32() const;

	// one of these pointers is an owned pointer; whichever
	// one is the encoding OTHER than the one we were initialized
	// with is the pointer we've allocated and must free.
	const char *m_pch;
	const uchar16 *m_char16;
	const uchar32 *m_char32;

	bool m_bHasUTF8 : 1;
	bool m_bOwnUTF8 : 1;

	bool m_bHasUTF16 : 1;
	bool m_bOwnUTF16 : 1;

	bool m_bHasUTF32 : 1;
	bool m_bOwnUTF32 : 1;

	bool m_bOriginal : 1;
};

// Number of bytes the UTF-8 form of nStringUnits units of pString takes
PLATFORM_OVERLOAD int V_GetUTF8BytesNeeded( const char *pString, int nStringUnits );
PLATFORM_OVERLOAD int V_GetUTF8BytesNeeded( const uchar16 *pString, int nStringUnits );
PLATFORM_OVERLOAD int V_GetUTF8BytesNeeded( const uchar32 *pString, int nStringUnits );

// Reports a string that failed to convert between the Unicode forms
PLATFORM_OVERLOAD void ReportInvalidStringConversion( const char *pString );
PLATFORM_OVERLOAD void ReportInvalidStringConversion( const uchar16 *pString );
PLATFORM_OVERLOAD void ReportInvalidStringConversion( const uchar32 *pString );

#ifdef _WIN32
class CBufferString;

// Converts the text of buf from one Windows code page to another in place and returns it
PLATFORM_OVERLOAD const char *V_ConvertBetweenStringEncodings( CBufferString &buf, uint32 nFromCodePage, uint32 nToCodePage );
PLATFORM_OVERLOAD const char *V_ConvertWin32ANSILatinToUTF8( CBufferString &buf );
#endif

#endif // TIER0_TIER0_UNICODE_H
