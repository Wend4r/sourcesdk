//====== Copyright 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

#ifndef TIER1_STRTOOLS_H
#define TIER1_STRTOOLS_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "basetypes.h"
#include <stdio.h>
#include "color.h"
#include "wchartypes.h"

#include <ctype.h>
#if POSIX
#include <wchar.h>
#include <math.h>
#include <wctype.h>
#endif

#include <stdarg.h>

// Forward declaration
class CBufferString;
class CUtlBuffer;
class Vector;
class Vector2D;
class Vector4D;
class Quaternion;
class Color;
class QAngle;

abstract_class IParsingErrorListener
{
	virtual void OnParsingError(CBufferString &error_msg) = 0;
};

#define PARSING_FLAG_NONE							(0)
#define PARSING_FLAG_ERROR_ASSERT					(1 << 0) // Triggers debug assertion on parsing errors, the default state
#define PARSING_FLAG_SKIP_ASSERT					(1 << 1) // Internal flag that is set when assertion is triggered, could also be used to prevent debug assertions
#define PARSING_FLAG_EMIT_WARNING					(1 << 2) // Emits global console warning on parsing errors, the default state
#define PARSING_FLAG_SKIP_WARNING					(1 << 3) // Internal flag that is set when global warning is emitted, could also be used to prevent warning messages
#define PARSING_FLAG_SILENT							(1 << 4) // Won't call callback when parsing errors are encountered
#define PARSING_FLAG_ERROR_IF_EMPTY					(1 << 5) // Emits parsing error if the input string was empty or NULL
#define PARSING_FLAG_UNK006							(1 << 6)
#define PARSING_FLAG_USE_BASE_AUTO					(1 << 7) // Use auto detection of a number base when parsing (uses https://en.cppreference.com/w/cpp/string/basic_string/stol under the hood, so same base rules applies)
#define PARSING_FLAG_USE_BASE_16					(1 << 8) // Use base of 16 when parsing

#ifdef _WIN64
#define str_size unsigned int
#else
#define str_size size_t
#endif


/// 3d memcpy. Copy (up-to) 3 dimensional data with arbitrary source and destination
/// strides. Optimizes to just a single memcpy when possible. For 2d data, set numslices to 1.
PLATFORM_INTERFACE void V_CopyMemory3D( void *pDestAdr, void const *pSrcAdr,		
				   int nNumCols, int nNumRows, int nNumSlices, // dimensions of copy
				   int nSrcBytesPerRow, int nSrcBytesPerSlice, // strides for source.
				   int nDestBytesPerRow, int nDestBytesPerSlice // strides for dest
	);

	
#define USE_FAST_CASE_CONVERSION 1
#if USE_FAST_CASE_CONVERSION
/// Faster conversion of an ascii char to upper case. This function does not obey locale or any language
/// setting. It should not be used to convert characters for printing, but it is a better choice
/// for internal strings such as used for hash table keys, etc. It's meant to be inlined and used
/// in places like the various dictionary classes. Not obeying locale also protects you from things
/// like your hash values being different depending on the locale setting.
#define FastASCIIToUpper( c ) ( ( ( (c) >= 'a' ) && ( (c) <= 'z' ) ) ? ( (c) - 32 ) : (c) )
/// similar to FastASCIIToLower
#define FastASCIIToLower( c ) ( ( ( (c) >= 'A' ) && ( (c) <= 'Z' ) ) ? ( (c) + 32 ) : (c) )
#else
#define FastASCIIToLower tolower
#define FastASCIIToUpper toupper
#endif


template< class T, class I > class CUtlMemory;
template< class T, class I, class A > class CUtlVector;

PLATFORM_INTERFACE const char *nexttoken(char *token, const char *str, char sep);

#ifdef OSX
size_t strnlen( const char *s, size_t n );
#endif

//-----------------------------------------------------------------------------
// Portable versions of standard string functions
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE void			V_tier0_memset( void *s, int c, size_t n );
PLATFORM_INTERFACE void			V_tier0_memcpy( void *dest, const void *src, size_t n );
PLATFORM_INTERFACE void			V_tier0_memmove( void *dest, const void *src, size_t n );
PLATFORM_INTERFACE int			V_tier0_memcmp( const void *s1, const void *s2, size_t n );

PLATFORM_INTERFACE size_t 		V_tier0_strlen( const char *str );
PLATFORM_INTERFACE int			V_tier0_strlen16( const uint16 *str );
PLATFORM_INTERFACE int			V_tier0_strlen32( const wchar_t *str );

PLATFORM_OVERLOAD size_t		V_strnlen( const char *str, int count );
PLATFORM_OVERLOAD size_t		V_strnlen( const uint16 *str, int count );
PLATFORM_OVERLOAD size_t		V_strnlen( const wchar_t *str, int count );

PLATFORM_INTERFACE void			V_tier0_strcpy( char *dest, const char *src );
PLATFORM_INTERFACE void			V_tier0_strcpy32( wchar_t *dest, const char *src );

PLATFORM_INTERFACE char*		V_tier0_strrchr( const char *s, char c );
PLATFORM_INTERFACE const char*	V_strnchr( const char* pStr, char c, int n );

PLATFORM_INTERFACE int			V_tier0_strcmp( const char *s1, const char *s2 );
PLATFORM_INTERFACE int			V_tier0_wcscmp( const wchar_t *s1, const wchar_t *s2 );
PLATFORM_INTERFACE int			V_strcmp32( const wchar_t *s1, const wchar_t *s2 );
PLATFORM_INTERFACE int			_V_strncmp (const char *s1, const char *s2, int count);
PLATFORM_INTERFACE int			_V_strnicmp_fast( const char *s1, const char *s2, int n );
PLATFORM_INTERFACE int			V_stricmp_fast( const char *s1, const char *s2 );
// A special high-performance case-insensitive compare function that in
// a single call distinguishes between exactly matching strings,
// strings equal in case-insensitive way, and not equal strings:
//   returns 0 if strings match exactly
//   returns >0 if strings match in a case-insensitive way, but do not match exactly
//   returns <0 if strings do not match even in a case-insensitive way
PLATFORM_INTERFACE int			V_stricmp_fast_NegativeForUnequal( const char *s1, const char *s2 );

PLATFORM_INTERFACE char*		V_tier0_strstr( const char *haystack, const char *needle );
PLATFORM_INTERFACE wchar_t*		V_strstr32( const wchar_t *s1, const wchar_t *search );
PLATFORM_OVERLOAD const char*	V_stristr_fast( const char* pStr, const char* pSearch );
PLATFORM_OVERLOAD const char*	V_strnistr_fast( const char* pStr, const char* pSearch, int n );
PLATFORM_OVERLOAD wchar_t*		V_wcsistr( wchar_t* pStr, const wchar_t* pSearch );

PLATFORM_INTERFACE size_t		_V_strcspn( const char *s1, const char *search );

PLATFORM_INTERFACE char*		V_strupper_fast( char *start );
PLATFORM_INTERFACE char*		V_strlower_fast( char *start );

PLATFORM_INTERFACE size_t		V_tier0_wcslen( const wchar_t *pwch );
// PLATFORM_INTERFACE wchar_t*	V_tier0_wcslower ( wchar_t *start);
// PLATFORM_INTERFACE wchar_t*	V_tier0_wcsupr ( wchar_t *start);

#if !defined( _DEBUG ) && defined( _PS3 )

// To avoid cross-prx calls, making the V_* fucntions that don't do anything but debug checks and call through to the non V_* function
// go ahead and call the non-V_* functions directly.
#define V_memset(s, c, n)				V_tier0_memset	((s), (c), (n))
#define V_memcpy(dest, src, n)			V_tier0_memcpy	((dest), (src), (n))
#define V_memmove(dest, src, n)			V_tier0_memmove	((dest), (src), (n))
#define V_memcmp(s1, s2, n)				V_tier0_memcmp	((s1), (s2), (n))

#define V_strlen(str)					V_tier0_strlen	((str))
#define V_strlen16(str)					V_tier0_strlen16	((str))
#define V_strlen32(str)					V_tier0_strlen32	((str))

#define V_strnlen(str, count)			V_strnlen	((str), (count))

#define V_strcpy(dest, src)				V_tier0_strcpy	((dest), (src))
#define V_strcpy32(dest, src)			V_tier0_strcpy32	((dest), (src))

#define V_strrchr(str, c)				V_tier0_strrchr	((str), (c))

#define V_strcmp(s1, s2)				V_tier0_strcmp	((s1), (s2))
#define V_wcscmp(s1, s2)				V_tier0_wcscmp	((s1), (s2))
#define V_strncmp(s1, s2, count)		_V_strncmp	((s1), (s2), (count))
#define V_strnicmp(s1, s2, n)			_V_strnicmp_fast	((s1), (s2), (n))
#define V_stricmp(s1, s2)				V_stricmp_fast	((s1), (s2))
#define V_stricmp_NFU(s1, s2)			V_stricmp_fast_NegativeForUnequal	((s1), (s2))
#define V_strcasecmp(s1, s2)			V_strcmp	((s1), (s2))
#define V_strncasecmp(s1, s2, n)		V_strnicmp	((s1), (s2), (n))

#define V_strstr(haystack, needle)		V_tier0_strstr	((haystack), (needle))
#define V_stristr(pStr, pSearch)		V_stristr_fast	((pStr), (pSearch))
#define V_strnistr(pStr, pSearch, n)	V_strnistr_fast	((pStr), (pSearch), (n))

#define V_strcspn(s1, search)			_V_strcspn	((s1), (search))

#define V_strupper(start)				V_strupper_fast	((start))
#define V_strupr(start)					V_strupper	(start)
#define V_strlower(start)				V_strlower_fast	((start))
#define V_strlwr(start)					V_strlower	(start)

#define V_wcslen(pwch)					V_tier0_wcslen	((pwch));
// #define V_wcslower(start)				V_tier0_wcslower	((start));
// #define V_wcsupr(start)					V_tier0_wcsupr	((start));
#else

#define V_memset(s, c, n)				V_tier0_memset	((s), (c), static_cast<size_t>(n))
#define V_memcpy(dest, src, n)			V_tier0_memcpy	((dest), (src), static_cast<size_t>(n))
#define V_memmove(dest, src, n)			V_tier0_memmove	((dest), (src), static_cast<size_t>(n))
#define V_memcmp(s1, s2, n)				V_tier0_memcmp	((s1), (s2), static_cast<size_t>(n))

#define V_strlen(str)					V_tier0_strlen	((str))
#define V_strlen16(str)					V_tier0_strlen16	((str))
#define V_strlen32(str)					V_tier0_strlen32	((str))

#define V_strnlen(str, count)			V_strnlen	((str), (count))

#define V_strcpy(dest, src)				V_tier0_strcpy	((dest), (src))
#define V_strcpy32(dest, src)			V_tier0_strcpy32	((dest), (src))

#define V_strrchr(str, c)				V_tier0_strrchr	((str), (c))

#define V_strcmp(s1, s2)				V_tier0_strcmp	((s1), (s2))
#define V_wcscmp(s1, s2)				V_tier0_wcscmp	((s1), (s2))
#define V_strncmp(s1, s2, count)		_V_strncmp	((s1), (s2), (count))
#define V_strnicmp(s1, s2, n)			_V_strnicmp_fast	((s1), (s2), (n))
#define V_stricmp(s1, s2)				V_stricmp_fast	((s1), (s2))
#define V_stricmp_NFU(s1, s2)			V_stricmp_fast_NegativeForUnequal	((s1), (s2))
#define V_strcasecmp(s1, s2)			V_strcmp	((s1), (s2))
#define V_strncasecmp(s1, s2, n)		V_strnicmp	((s1), (s2), (n))

#define V_strstr(haystack, needle)		V_tier0_strstr	((haystack), (needle))
#define V_stristr(pStr, pSearch)		V_stristr_fast	((pStr), (pSearch))
#define V_strnistr(pStr, pSearch, n)	V_strnistr_fast	((pStr), (pSearch), (n))

#define V_strcspn(s1, search)			_V_strcspn	((s1), (search))

#define V_strupper(start)				V_strupper_fast	((start))
#define V_strupr(start)					V_strupper	(start)
#define V_strlower(start)				V_strlower_fast	((start))
#define V_strlwr(start)					V_strlower	(start)

#define V_wcslen(pwch)					V_tier0_wcslen	((pwch))
// #define V_wcslower(start)				V_tier0_wcslower	((start));
// #define V_wcsupr(start)					V_tier0_wcsupr	((start));

#endif

PLATFORM_INTERFACE int			V_atoi (const char *str);
PLATFORM_INTERFACE int64 		V_atoi64(const char *str);
PLATFORM_INTERFACE uint64 		V_atoui64(const char *str);
PLATFORM_INTERFACE int64		V_strtoi64( const char *nptr, char **endptr, int base );
PLATFORM_INTERFACE uint64		V_strtoui64( const char *nptr, char **endptr, int base );
PLATFORM_INTERFACE double		V_atof(const char *str);
PLATFORM_INTERFACE float		V_atofloat32(const char *str);
PLATFORM_INTERFACE double		V_atofloat64(const char *str);
inline double					V_atod(const char *str) { return V_atofloat64(str); }

PLATFORM_INTERFACE void			V_qsort_s( void *base, size_t num, size_t width, int ( *compare )(const void *, const void *, void *), void *context );

// returns string immediately following prefix, (ie str+strlen(prefix)) or NULL if prefix not found
PLATFORM_INTERFACE const char *_V_StringAfterPrefix             ( const char *str, const char *prefix );
PLATFORM_INTERFACE const char *_V_StringAfterPrefixCaseSensitive( const char *str, const char *prefix );
inline const char *StringAfterPrefix             ( const char *str, const char *prefix ) { return _V_StringAfterPrefix             (str, prefix); };
inline const char *StringAfterPrefixCaseSensitive( const char *str, const char *prefix ) { return _V_StringAfterPrefixCaseSensitive(str, prefix); };
inline bool	StringHasPrefix             ( const char *str, const char *prefix ) { return StringAfterPrefix             ( str, prefix ) != NULL; }
inline bool	StringHasPrefixCaseSensitive( const char *str, const char *prefix ) { return StringAfterPrefixCaseSensitive( str, prefix ) != NULL; }

// Does 'pStr' end with 'pSuffix'? (case sensitive/insensitive variants)
PLATFORM_INTERFACE bool _V_strEndsWith( const char *pStr, const char *pSuffix, bool bCaseSensitive = false );
inline bool V_strEndsWith(  const char *pStr, const char *pSuffix ) { return _V_strEndsWith( pStr, pSuffix, true ); }
inline bool V_striEndsWith( const char *pStr, const char *pSuffix ) { return _V_strEndsWith( pStr, pSuffix, false ); }


#define StringIsEmpty( pchString ) ( pchString[ 0 ] == '\0' )

// Normalizes a float string in place.  
// (removes leading zeros, trailing zeros after the decimal point, and the decimal point itself where possible)
PLATFORM_INTERFACE void V_normalizeFloatString( char* pFloat );

// this is locale-unaware and therefore faster version of standard isdigit()
// It also avoids sign-extension errors.
PLATFORM_INTERFACE bool V_isdigit_str( char* str);
#define V_isdigit V_isdigit_str

PLATFORM_INTERFACE bool V_iswdigit( int c );

inline bool V_isempty( const char* pszString ) { return !pszString || !pszString[ 0 ]; }

// The islower/isdigit/etc. functions all expect a parameter that is either
// 0-0xFF or EOF. It is easy to violate this constraint simply by passing
// 'char' to these functions instead of unsigned char.
// The V_ functions handle the char/unsigned char mismatch by taking a
// char parameter and casting it to unsigned char so that chars with the
// sign bit set will be zero extended instead of sign extended.
// Not that EOF cannot be passed to these functions.
//
// These functions could also be used for optimizations if locale
// considerations make some of the CRT functions slow.
#undef isdigit // In case this is implemented as a macro
#define isdigit V_isdigit
#undef isalpha
PLATFORM_INTERFACE bool V_isalpha_str_fast(char c);
#define V_isalpha V_isalpha_str_fast
#define isalpha V_isalpha
#undef isalnum
PLATFORM_INTERFACE bool V_isalnum_str_fast(char c);
#define V_isalnum V_isalnum_str_fast
#define isalnum V_isalnum
inline bool V_isprint(char c) { return isprint( (unsigned char)c ) != 0; }
#undef isprint
#define isprint V_isprint
inline bool V_isxdigit(char c) { return isxdigit( (unsigned char)c ) != 0; }
#undef isxdigit
#define isxdigit V_isxdigit
inline bool V_ispunct(char c) { return ispunct( (unsigned char)c ) != 0; }
#undef ispunct
#define ispunct V_ispunct
inline bool V_isgraph(char c) { return isgraph( (unsigned char)c ) != 0; }
#undef isgraph
#define isgraph V_isgraph
inline bool V_isupper(char c) { return isupper( (unsigned char)c ) != 0; }
#undef isupper
#define isupper V_isupper
inline bool V_islower(char c) { return islower( (unsigned char)c ) != 0; }
#undef islower
#define islower V_islower
inline bool V_iscntrl(char c) { return iscntrl( (unsigned char)c ) != 0; }
#undef iscntrl
#define iscntrl V_iscntrl

inline bool V_isspace(int c)
{
	// The standard white-space characters are the following: space, tab, carriage-return, newline, vertical tab, and form-feed. In the C locale, V_isspace() returns true only for the standard white-space characters. 
	//return c == ' ' || c == 9 /*horizontal tab*/ || c == '\r' || c == '\n' || c == 11 /*vertical tab*/ || c == '\f';
	// codes of whitespace symbols: 9 HT, 10 \n, 11 VT, 12 form feed, 13 \r, 32 space

	// easy to understand version, validated:
	// return ((1 << (c-1)) & 0x80001F00) != 0 && ((c-1)&0xE0) == 0;
	
	// 5% faster on Core i7, 35% faster on Xbox360, no branches, validated:
	#ifdef _X360
	return ((1 << (c-1)) & 0x80001F00 & ~(-int((c-1)&0xE0))) != 0;
	#else
	// this is 11% faster on Core i7 than the previous, VC2005 compiler generates a seemingly unbalanced search tree that's faster
	switch(c)
	{
	case ' ':
	case 9:
	case '\r':
	case '\n':
	case 11:
	case '\f':
		return true;
	default:
		return false;
	}
	#endif
}
#undef isspace
#define isspace V_isspace

// Returns true if V_isspace returns true for any character in the string
PLATFORM_INTERFACE bool V_containsWhitespace( const char *pStr );

//-----------------------------------------------------------------------------
// Purpose: returns true if it's a valid hex string
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE bool V_isvalidhex( char const *in, int inputchars );


//-----------------------------------------------------------------------------
// Purpose: Checks if the string is lower case
// NOTE:	Only works with ASCII strings
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE bool V_isstrlower_fast( const char *pch );
#define V_isstrlower(pch) V_isstrlower_fast((pch))



// These are versions of functions that guarantee NULL termination.
//
// maxLen is the maximum number of bytes in the destination string.
// pDest[maxLen-1] is always NULL terminated if pSrc's length is >= maxLen.
//
// This means the last parameter can usually be a sizeof() of a string.
PLATFORM_INTERFACE void _V_strncpy( char *pDest, const char *pSrc, int maxLenInChars );
PLATFORM_INTERFACE void _V_strncpy32_bytes( wchar_t *pDest32, const wchar_t *pSrc32, int maxLenInChars );
#define V_strncpy(pDest, pSrc, maxLenInChars) _V_strncpy((pDest), (pSrc), (maxLenInChars))
#define V_strncpy32_bytes _V_strncpy32_bytes
#define V_strncpy32 V_strncpy32_bytes

// Ultimate safe strcpy function, for arrays only -- buffer size is inferred by the compiler
template <size_t maxLenInChars> void V_strcpy_safe( OUT_Z_ARRAY char (&pDest)[maxLenInChars], const char *pSrc ) 
{ 
	V_strncpy( pDest, pSrc, static_cast<int>(maxLenInChars) ); 
}

// A function which duplicates a string using new[] to allocate the new string.
inline char *V_strdup( const char *pSrc )
{
	size_t nLen = V_strlen( pSrc );
	char *pResult = new char [ nLen+1 ];
	V_memcpy( pResult, pSrc, nLen+1 );
	return pResult;
}

// Returns the number of characters printed (not including the NULL), or maxLenInChars if truncation occurs.
// pDest is always null-terminated.
PLATFORM_INTERFACE int V_snprintf( OUT_Z_CAP(maxLenInChars) char *pDest, int maxLenInChars, PRINTF_FORMAT_STRING const char *pFormat, ... ) FMTFUNCTION( 3, 4 );
PLATFORM_INTERFACE int V_snprintfcat( OUT_Z_CAP(maxLenInChars) char *pDest, int maxLenInChars, PRINTF_FORMAT_STRING const char *pFormat, ... ) FMTFUNCTION( 3, 4 );
PLATFORM_INTERFACE int V_snprintfWarnTrunc( OUT_Z_CAP(maxLenInCharacters) char *pDest, int maxLenInCharacters, bool bIgnoreWarn, PRINTF_FORMAT_STRING const char *pFormat, va_list params, bool *pbTruncated );
PLATFORM_INTERFACE int V_snprintfcatNoSecurityDoNotUse( OUT_Z_CAP(maxLenInChars) char *pDest, int maxLenInChars, PRINTF_FORMAT_STRING const char *pFormat, ... ) FMTFUNCTION( 3, 4 );

PLATFORM_INTERFACE int V_vsnprintf( OUT_Z_CAP(maxLenInCharacters) char *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const char *pFormat, va_list params );
PLATFORM_INTERFACE int V_vsnprintfcat( OUT_Z_CAP(maxLenInCharacters) char *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const char *pFormat, va_list params );
PLATFORM_INTERFACE int V_vsnprintfRet( OUT_Z_CAP(maxLenInCharacters) char *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const char *pFormat, va_list params, bool *pbTruncated );
PLATFORM_INTERFACE int V_vsnprintfSize( OUT_Z_CAP(maxLenInCharacters) char *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const char *pFrmat, va_list params );
PLATFORM_INTERFACE int V_vsnprintfWarnTrunc( OUT_Z_CAP(maxLenInCharacters) char *pDest, int maxLenInCharacters, bool bIgnoreWarn, PRINTF_FORMAT_STRING const char *pFormat, va_list params, bool *pbTruncated );

// gcc insists on only having format annotations on declarations, not definitions, which is why I have both.
template <size_t maxLenInChars> int V_sprintf_safe( OUT_Z_ARRAY char (&pDest)[maxLenInChars], PRINTF_FORMAT_STRING const char *pFormat, ... ) FMTFUNCTION( 2, 3 );
template <size_t maxLenInChars> int V_sprintf_safe( OUT_Z_ARRAY char (&pDest)[maxLenInChars], const char *pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	int result = V_vsnprintf( pDest, maxLenInChars, pFormat, params );
	va_end( params );
	return result;
}

// gcc insists on only having format annotations on declarations, not definitions, which is why I have both.
// Append formatted text to an array in a safe manner -- always null-terminated, truncation rather than buffer overrun.
template <size_t maxLenInChars> int V_sprintfcat_safe( INOUT_Z_ARRAY char (&pDest)[maxLenInChars], PRINTF_FORMAT_STRING const char *pFormat, ... ) FMTFUNCTION( 2, 3 );
template <size_t maxLenInChars> int V_sprintfcat_safe( INOUT_Z_ARRAY char (&pDest)[maxLenInChars], PRINTF_FORMAT_STRING const char *pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	size_t usedLength = V_strlen(pDest);
	// This code is here to check against buffer overruns when uninitialized arrays are passed in.
	// It should never be executed. Unfortunately we can't assert in this header file.
	if ( usedLength >= maxLenInChars )
		usedLength = 0;
	int result = V_vsnprintf( pDest + usedLength, maxLenInChars - usedLength, pFormat, params );
	va_end( params );
	return result;
}

PLATFORM_INTERFACE void _V_wcsncpy_bytes( OUT_Z_BYTECAP(maxLenInBytes) wchar_t *pDest, wchar_t const *pSrc, int maxLenInBytes );
#define V_wcsncpy_bytes _V_wcsncpy_bytes
#define V_wcsncpy V_wcsncpy_bytes
template <size_t maxLenInChars> void V_wcscpy_safe( OUT_Z_ARRAY wchar_t (&pDest)[maxLenInChars], wchar_t const *pSrc ) 
{ 
	V_wcsncpy( pDest, pSrc, maxLenInChars * sizeof(*pDest) ); 
}

#define COPY_ALL_CHARACTERS -1
PLATFORM_INTERFACE char *_V_strncat( INOUT_Z_CAP(maxLenInBytes) char *, const char *, size_t maxLenInBytes, int nMaxCharsToCopy=COPY_ALL_CHARACTERS );
#define V_strncat _V_strncat
template <size_t cchDest> char *V_strcat_safe( INOUT_Z_ARRAY char (&pDest)[cchDest], const char *pSrc, int nMaxCharsToCopy=COPY_ALL_CHARACTERS )
{ 
	return V_strncat( pDest, pSrc, static_cast<int>(cchDest), nMaxCharsToCopy ); 
}

PLATFORM_INTERFACE wchar_t *_V_wcsncat_cch( INOUT_Z_BYTECAP(maxLenInBytes) wchar_t *, const wchar_t *, int maxLenInBytes, int nMaxCharsToCopy=COPY_ALL_CHARACTERS );
#define V_wcsncat _V_wcsncat_cch
template <size_t cchDest> wchar_t *V_wcscat_safe( INOUT_Z_ARRAY wchar_t (&pDest)[cchDest], const wchar_t *pSrc, int nMaxCharsToCopy=COPY_ALL_CHARACTERS )
{ 
	return V_wcsncat( pDest, pSrc, static_cast<int>(cchDest), nMaxCharsToCopy ); 
}
// char *V_strnlwr(char *, size_t);

// Unicode string conversion policies - what to do if an illegal sequence is encountered
enum EStringConvertErrorPolicy
{
	_STRINGCONVERTFLAG_SKIP =		1,
	_STRINGCONVERTFLAG_FAIL =		2,
	_STRINGCONVERTFLAG_ASSERT =		4,

	STRINGCONVERT_REPLACE =			0,
	STRINGCONVERT_SKIP =			_STRINGCONVERTFLAG_SKIP,
	STRINGCONVERT_FAIL =			_STRINGCONVERTFLAG_FAIL,

	STRINGCONVERT_ASSERT_REPLACE =	_STRINGCONVERTFLAG_ASSERT + STRINGCONVERT_REPLACE,
	STRINGCONVERT_ASSERT_SKIP =		_STRINGCONVERTFLAG_ASSERT + STRINGCONVERT_SKIP,
	STRINGCONVERT_ASSERT_FAIL =		_STRINGCONVERTFLAG_ASSERT + STRINGCONVERT_FAIL,
}; 

// Unicode (UTF-8, UTF-16, UTF-32) fundamental conversion functions.
PLATFORM_INTERFACE bool V_IsValidUChar32( uchar32 uValue );
PLATFORM_INTERFACE int V_UChar32ToUTF8( uchar32 uValue, char *pOut );
PLATFORM_INTERFACE int V_UChar32ToUTF8Len( uchar32 uValue );
PLATFORM_INTERFACE int V_UChar32ToUTF16( uchar32 uValue, uchar16 *pOut );
PLATFORM_INTERFACE int V_UChar32ToUTF16Len( uchar32 uValue );

#define Q_IsValidUChar32 V_IsValidUChar32
#define Q_UChar32ToUTF8 V_UChar32ToUTF8
#define Q_UChar32ToUTF8Len V_UChar32ToUTF8Len
#define Q_UChar32ToUTF16 V_UChar32ToUTF16
#define Q_UChar32ToUTF16Len V_UChar32ToUTF16Len

// Validate that a Unicode string is well-formed and contains only valid code points
PLATFORM_OVERLOAD bool V_UnicodeValidate( const char *pUTF8 );
PLATFORM_OVERLOAD bool V_UnicodeValidate( const uchar16 *pUTF16 );
PLATFORM_OVERLOAD bool V_UnicodeValidate( const uchar32 *pUTF32 );
#define Q_UnicodeValidate V_UnicodeValidate

// Returns length of string in Unicode code points (printed glyphs or non-printing characters)
PLATFORM_OVERLOAD int V_UnicodeLength( const char *pUTF8 );
PLATFORM_OVERLOAD int V_UnicodeLength( const uchar16 *pUTF16 );
PLATFORM_OVERLOAD int V_UnicodeLength( const uchar32 *pUTF32 );
#define Q_UnicodeLength V_UnicodeLength


// Returns length of string in elements, not characters! These are analogous to Q_strlen and Q_wcslen
inline int Q_strlen16( const uchar16 *puc16 ) { int nElems = 0; while ( puc16[nElems] ) ++nElems; return nElems; }
inline int Q_strlen32( const uchar32 *puc32 ) { int nElems = 0; while ( puc32[nElems] ) ++nElems; return nElems; }


// Repair invalid Unicode strings by dropping truncated characters and fixing improperly-double-encoded UTF-16 sequences.
// Unlike conversion functions which replace with '?' by default, a repair operation assumes that you know that something
// is wrong with the string (eg, mid-sequence truncation) and you just want to do the best possible job of cleaning it up.
// You can pass a REPLACE or FAIL policy if you would prefer to replace characters with '?' or clear the entire string.
// Returns nonzero on success, or 0 if the policy is FAIL and an invalid sequence was found.
PLATFORM_OVERLOAD int V_UnicodeRepair( char *pUTF8, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );
PLATFORM_OVERLOAD int V_UnicodeRepair( uchar16 *pUTF16, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );
PLATFORM_OVERLOAD int V_UnicodeRepair( uchar32 *pUTF32, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );
#define Q_UnicodeRepair V_UnicodeRepair

// Advance pointer forward by N Unicode code points (printed glyphs or non-printing characters), stopping at terminating null if encountered.
PLATFORM_OVERLOAD char *V_UnicodeAdvance( char *pUTF8, int nCharacters );
PLATFORM_OVERLOAD uchar16 *V_UnicodeAdvance( uchar16 *pUTF16, int nCharacters );
PLATFORM_OVERLOAD uchar32 *V_UnicodeAdvance( uchar32 *pUTF32, int nCharacters );
#define Q_UnicodeAdvance V_UnicodeAdvance

// Truncate to maximum of N Unicode code points (printed glyphs or non-printing characters)
inline void V_UnicodeTruncate( char *pUTF8, int nCharacters ) { *V_UnicodeAdvance( pUTF8, nCharacters ) = 0; }
inline void V_UnicodeTruncate( uchar16 *pUTF16, int nCharacters ) { *V_UnicodeAdvance( pUTF16, nCharacters ) = 0; }
inline void V_UnicodeTruncate( uchar32 *pUTF32, int nCharacters ) { *V_UnicodeAdvance( pUTF32, nCharacters ) = 0; }
#define Q_UnicodeTruncate V_UnicodeTruncate


// Conversion between Unicode string types (UTF-8, UTF-16, UTF-32). Deals with bytes, not element counts,
// to minimize harm from the programmer mistakes which continue to plague our wide-character string code.
// Returns the number of bytes written to the output, or if output is NULL, the number of bytes required.
PLATFORM_INTERFACE int V_UTF8ToUTF16( const char *pUTF8, OUT_Z_BYTECAP(cubDestSizeInBytes) uchar16 *pUTF16, int cubDestSizeInBytes, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_ASSERT_REPLACE );
template< typename T, typename U > inline int V_UTF8ToUCS2( const char *pUTF8, T cubSrcInBytes, OUT_Z_BYTECAP(cubDestSizeInBytes) ucs2 *pUCS2, U cubDestSizeInBytes )
{
	return V_UTF8ToUTF16( pUTF8, pUCS2, static_cast<int>(cubDestSizeInBytes) );
}

// conversion functions wchar_t <-> char, returning the number of characters converted
PLATFORM_INTERFACE int V_UTF8ToUTF32( const char *pUTF8, OUT_Z_BYTECAP(cubDestSizeInBytes) wchar_t *pwchDest, int cubDestSizeInBytes );
template< typename T > inline int V_UTF8ToUnicode( const char *pUTF8, OUT_Z_BYTECAP(cubDestSizeInBytes) wchar_t *pwchDest, T cubDestSizeInBytes )
{
	return V_UTF8ToUTF32( pUTF8, pwchDest, static_cast<int>(cubDestSizeInBytes) );
}

PLATFORM_INTERFACE int V_UTF32ToUTF8( const wchar_t *pUnicode, OUT_Z_BYTECAP(cubDestSizeInBytes) char *pUTF8, int cubDestSizeInBytes );
template< typename T > inline int V_UnicodeToUTF8( const wchar_t *pUnicode, OUT_Z_BYTECAP(cubDestSizeInBytes) char *pUTF8, T cubDestSizeInBytes )
{
	return V_UTF32ToUTF8( pUnicode, pUTF8, static_cast<int>(cubDestSizeInBytes) );
}

PLATFORM_INTERFACE int V_UTF16ToUTF32( const ucs2 *pUCS2, OUT_Z_BYTECAP(cubDestSizeInBytes) wchar_t *pUnicode, int cubDestSizeInBytes );
template< typename T > inline int V_UTF16ToUnicode( const ucs2 *pUCS2, OUT_Z_BYTECAP(cubDestSizeInBytes) wchar_t *pUnicode, T cubDestSizeInBytes )
{
	return V_UTF16ToUTF32( pUCS2, pUnicode, static_cast<int>(cubDestSizeInBytes) );
}

PLATFORM_INTERFACE int V_UTF16ToUTF8( const ucs2 *pUCS2, OUT_Z_BYTECAP(cubDestSizeInBytes) char *pUTF8, int cubDestSizeInBytes );
template< typename T > inline int V_UCS2ToUTF8( const ucs2 *pUCS2, OUT_Z_BYTECAP(cubDestSizeInBytes) char *pUTF8, T cubDestSizeInBytes )
{
	return V_UTF16ToUTF8( pUCS2, pUTF8, static_cast<int>(cubDestSizeInBytes) );
}

PLATFORM_INTERFACE int V_UTF32ToUTF16( const wchar_t *pUnicode, int cubSrcInBytes, OUT_Z_BYTECAP(cubDestSizeInBytes) char *pUCS2, int cubDestSizeInBytes );
template< typename T, typename U > inline int V_UnicodeToUCS2( const wchar_t *pUnicode, T cubSrcInBytes, OUT_Z_BYTECAP(cubDestSizeInBytes) char *pUCS2, U cubDestSizeInBytes )
{
	return V_UTF32ToUTF16( pUnicode, static_cast<int>(cubSrcInBytes), pUCS2, static_cast<int>(cubDestSizeInBytes) );
}

#define Q_UTF8ToUTF16 V_UTF8ToUTF16
#define Q_UTF8ToUTF32 V_UTF8ToUTF32
#define Q_UTF16ToUTF8 V_UTF16ToUTF8
#define Q_UTF16ToUTF32 V_UTF16ToUTF32
#define Q_UTF32ToUTF8 V_UTF32ToUTF8
#define Q_UTF32ToUTF16 V_UTF32ToUTF16

// This is disgusting and exist only easily to facilitate having 16-bit and 32-bit wchar_t's on different platforms
PLATFORM_INTERFACE int V_UTF32ToUTF32( const uchar32 *pUTF32Source, OUT_Z_BYTECAP(cubDestSizeInBytes) uchar32 *pUTF32Dest, int cubDestSizeInBytes, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_ASSERT_REPLACE );
#define Q_UTF32ToUTF32 V_UTF32ToUTF32

// Conversion between count-limited UTF-n character arrays, including any potential NULL characters.
// Output has a terminating NULL for safety; strip the last character if you want an unterminated string.
// Returns the number of bytes written to the output, or if output is NULL, the number of bytes required.
PLATFORM_INTERFACE int V_UTF8CharsToUTF16( const char *pUTF8, int nElements, OUT_Z_BYTECAP(cubDestSizeInBytes) uchar16 *pUTF16, int cubDestSizeInBytes, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF8CharsToUTF32( const char *pUTF8, int nElements, OUT_Z_BYTECAP(cubDestSizeInBytes) uchar32 *pUTF32, int cubDestSizeInBytes, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF16CharsToUTF8( const uchar16 *pUTF16, int nElements, OUT_Z_BYTECAP(cubDestSizeInBytes) char *pUTF8, int cubDestSizeInBytes, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF16CharsToUTF32( const uchar16 *pUTF16, int nElements, OUT_Z_BYTECAP(cubDestSizeInBytes) uchar32 *pUTF32, int cubDestSizeInBytes, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF32CharsToUTF8( const uchar32 *pUTF32, int nElements, OUT_Z_BYTECAP(cubDestSizeInBytes) char *pUTF8, int cubDestSizeInBytes, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF32CharsToUTF16( const uchar32 *pUTF32, int nElements, OUT_Z_BYTECAP(cubDestSizeInBytes) uchar16 *pUTF16, int cubDestSizeInBytes, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_ASSERT_REPLACE );

// Decode a single UTF-8 character to a uchar32, returns number of UTF-8 bytes parsed
PLATFORM_INTERFACE int V_UTF8ToUChar32( const char *pUTF8_, uchar32 &uValueOut, bool &bErrorOut );

// Decode a single UTF-16 character to a uchar32, returns number of UTF-16 characters (NOT BYTES) consumed
PLATFORM_INTERFACE int V_UTF16ToUChar32( const uchar16 *pUTF16, uchar32 &uValueOut, bool &bErrorOut );

#define Q_UTF8CharsToUTF16 V_UTF8CharsToUTF16
#define Q_UTF8CharsToUTF32 V_UTF8CharsToUTF32
#define Q_UTF16CharsToUTF8 V_UTF16CharsToUTF8
#define Q_UTF16CharsToUTF32 V_UTF16CharsToUTF32
#define Q_UTF32CharsToUTF8 V_UTF32CharsToUTF8
#define Q_UTF32CharsToUTF16 V_UTF32CharsToUTF16

#define Q_UTF8ToUChar32 V_UTF8ToUChar32
#define Q_UTF16ToUChar32 V_UTF16ToUChar32

// NOTE: WString means either UTF32 or UTF16 depending on the platform and compiler settings.
#if defined( _MSC_VER ) || defined( _WIN32 )
#define Q_UTF8ToWString Q_UTF8ToUTF16
#define Q_UTF8CharsToWString Q_UTF8CharsToUTF16
#define Q_UTF32ToWString Q_UTF32ToUTF16
#define Q_WStringToUTF8 Q_UTF16ToUTF8
#define Q_WStringCharsToUTF8 Q_UTF16CharsToUTF8
#define Q_WStringToUTF32 Q_UTF16ToUTF32
#else
#define Q_UTF8ToWString Q_UTF8ToUTF32
#define Q_UTF8CharsToWString Q_UTF8CharsToUTF32
#define Q_UTF32ToWString Q_UTF32ToUTF32
#define Q_WStringToUTF8 Q_UTF32ToUTF8
#define Q_WStringCharsToUTF8 Q_UTF32CharsToUTF8
#define Q_WStringToUTF32 Q_UTF32ToUTF32
#endif

// UNDONE: Find a non-compiler-specific way to do this
#ifdef _WIN32
#ifndef _VA_LIST_DEFINED

#ifdef  _M_ALPHA

struct va_list 
{
    char *a0;       /* pointer to first homed integer argument */
    int offset;     /* byte offset of next parameter */
};

#else  // !_M_ALPHA

typedef char *  va_list;

#endif // !_M_ALPHA

#define _VA_LIST_DEFINED

#endif   // _VA_LIST_DEFINED

#elif POSIX
#include <stdarg.h>
#endif

#ifdef _WIN32
#define CORRECT_PATH_SEPARATOR '\\'
#define CORRECT_PATH_SEPARATOR_S "\\"
#define INCORRECT_PATH_SEPARATOR '/'
#define INCORRECT_PATH_SEPARATOR_S "/"
#define CHARACTERS_WHICH_SEPARATE_DIRECTORY_COMPONENTS_IN_PATHNAMES ":/\\"
#define PATHSEPARATOR(c) ((c) == '\\' || (c) == '/')
#elif POSIX || defined( _PS3 )
#define CORRECT_PATH_SEPARATOR '/'
#define CORRECT_PATH_SEPARATOR_S "/"
#define INCORRECT_PATH_SEPARATOR '\\'
#define INCORRECT_PATH_SEPARATOR_S "\\"
#define CHARACTERS_WHICH_SEPARATE_DIRECTORY_COMPONENTS_IN_PATHNAMES "/"
#define PATHSEPARATOR(c) ((c) == '\\' || (c) == '/')
#endif


PLATFORM_INTERFACE int V_vsnprintf( OUT_Z_CAP(maxLenInCharacters) char *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const char *pFormat, va_list params );
PLATFORM_INTERFACE int V_vsnprintfcat( OUT_Z_CAP(maxLenInCharacters) char *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const char *pFormat, va_list params );
template <size_t maxLenInCharacters> int V_vsprintf_safe( OUT_Z_ARRAY char (&pDest)[maxLenInCharacters], PRINTF_FORMAT_STRING const char *pFormat, va_list params ) { return V_vsnprintf( pDest, maxLenInCharacters, pFormat, params ); }
PLATFORM_INTERFACE int V_vsnprintfRet( OUT_Z_CAP(maxLenInCharacters) char *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const char *pFormat, va_list params, bool *pbTruncated );
template <size_t maxLenInCharacters> int V_vsprintfRet_safe( OUT_Z_ARRAY char (&pDest)[maxLenInCharacters], PRINTF_FORMAT_STRING const char *pFormat, va_list params, bool *pbTruncated ) { return V_vsnprintfRet( pDest, maxLenInCharacters, pFormat, params, pbTruncated ); }
PLATFORM_INTERFACE int V_vsnprintfSize( OUT_Z_CAP(maxLenInCharacters) char *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const char *pFrmat, va_list params );
PLATFORM_INTERFACE int V_vsnprintfWarnTrunc( OUT_Z_CAP(maxLenInCharacters) char *pDest, int maxLenInCharacters, bool bIgnoreWarn, PRINTF_FORMAT_STRING const char *pFormat, va_list params, bool *pbTruncated );

PLATFORM_INTERFACE int V_vsnwprintf_cch( OUT_Z_CAP(maxLenInCharacters) wchar_t *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const wchar_t *pFormat, va_list params );
#define V_vsnwprintf V_vsnwprintf_cch
template <size_t maxLenInCharacters> int V_vswprintf_safe( OUT_Z_ARRAY wchar_t (&pDest)[maxLenInCharacters], PRINTF_FORMAT_STRING const wchar_t *pFormat, va_list params ) { return V_vsnwprintf( pDest, maxLenInCharacters, pFormat, params ); }
PLATFORM_INTERFACE int V_vsnwprintfRet_cch( OUT_Z_CAP(maxLenInCharacters) wchar_t *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const wchar_t *pFormat, va_list params );
#define V_vsnwprintfRet V_vsnwprintfRet_cch
template <size_t maxLenInChars> int V_swprintf_safe( OUT_Z_ARRAY wchar_t (&pDest)[maxLenInChars], PRINTF_FORMAT_STRING const wchar_t *pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	int result = V_vsnwprintf( pDest, maxLenInChars, pFormat, params );
	va_end( params );
	return result;
}

// FMTFUNCTION can only be used on ASCII functions, not wide-char functions.
PLATFORM_INTERFACE int V_snwprintf_bytes( OUT_Z_CAP(maxLenInCharacters) wchar_t *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const wchar_t *pFormat, ... );
PLATFORM_INTERFACE int V_snwprintf_cch( OUT_Z_CAP(maxLenInCharacters) wchar_t *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const wchar_t *pFormat, ... );
#define V_snwprintf V_snwprintf_cch

// Prints out a pretified memory counter string value ( e.g., 7,233.27 Mb, 1,298.003 Kb, 127 bytes )
PLATFORM_INTERFACE char *V_PrettifyMem( float value, int digitsafterdecimal = 2, bool usebinaryonek = false );

// Prints out a pretified integer with comma separators (eg, 7,233,270,000)
PLATFORM_INTERFACE char *V_PrettifyNum( int64 value );
PLATFORM_INTERFACE char *V_PrettifyFloat( float value );

// Prints out a memory dump where stuff that's ascii is human readable, etc.
class CUtlString;
PLATFORM_INTERFACE void V_LogMultiline( bool input, char const *label, const char *data, size_t len, CUtlString &output );
PLATFORM_INTERFACE void V_LogMultilineToArray( bool input, char const *label, const char *data, size_t len, uint &output );

// Functions for converting hexidecimal character strings back into binary data etc.
//
// e.g., 
// int output;
// V_hextobinary( "ffffffff", 8, &output, sizeof( output ) );
// would make output == 0xfffffff or -1
// Similarly,
// char buffer[ 9 ];
// V_binarytohex( &output, sizeof( output ), buffer, sizeof( buffer ) );
// would put "ffffffff" into buffer (note null terminator!!!)
PLATFORM_INTERFACE void _V_hextobinary( char const *in, int numchars, OUT_Z_CAP(maxoutputbytes) byte *out, int maxoutputbytes );
#define V_hextobinary _V_hextobinary

PLATFORM_INTERFACE void V_binarytohex( const byte *in, int inputbytes, char *out, int outsize );
#define V_binarytohex _V_hextobinary

// Tools for working with filenames
// Extracts the base name of a file (no path, no extension, assumes '/' or '\' as path separator)
PLATFORM_INTERFACE void V_FileBase( const char *in, OUT_Z_CAP(maxlen) char *out,int maxlen );
#define V_FileBase _V_hextobinary

// Remove the final characters of ppath if it's '\' or '/'.
PLATFORM_INTERFACE void V_StripTrailingSlash( char *ppath );

// Remove the final characters of ppline if they are whitespace (uses V_isspace)
PLATFORM_INTERFACE void V_StripTrailingWhitespace( char *ppline );

// Remove the initial characters of ppline if they are whitespace (uses V_isspace)
PLATFORM_INTERFACE void V_StripLeadingWhitespace( char *ppline );

// Remove the initial/final characters of ppline if they are " quotes
PLATFORM_INTERFACE void V_StripSurroundingQuotes( char *ppline );

// Remove any extension from in and return resulting string in out
PLATFORM_INTERFACE void _V_StripExtension( const char *in, char *out, int outLen );
#define V_StripExtension _V_StripExtension

// Make path end with extension if it doesn't already have an extension
PLATFORM_INTERFACE void _V_DefaultExtension( char *path, const char *extension, int pathStringLength );
#define V_DefaultExtension V_DefaultExtension

// Strips any current extension from path and ensures that extension is the new extension.
// NOTE: extension string MUST include the . character
PLATFORM_INTERFACE void _V_SetExtension( char *path, const char *extension, int pathStringLength );
#define V_SetExtension _V_SetExtension

// Removes any filename from path ( strips back to previous / or \ character )
PLATFORM_INTERFACE void V_StripFilename( char *path );

// Remove the final directory from the path
PLATFORM_INTERFACE bool _V_StripLastDir( char *dirName, int maxlen );
#define V_StripLastDir _V_StripLastDir

// Returns a pointer to the unqualified file name (no path) of a file name
PLATFORM_INTERFACE const char * V_UnqualifiedFileName( const char * in );
// Given a path and a filename, composes "path\filename", inserting the (OS correct) separator if necessary
PLATFORM_INTERFACE void _V_ComposeFileName( const char *path, const char *filename, char *dest, int destSize );
#define V_ComposeFileName _V_ComposeFileName

// Copy out the path except for the stuff after the final pathseparator
PLATFORM_INTERFACE bool _V_ExtractFilePath( const char *path, char *dest, int destSize );
#define V_ExtractFilePath _V_ExtractFilePath

// Copy out the file extension into dest
PLATFORM_INTERFACE void _V_ExtractFileExtension( const char *path, char *dest, int destSize );
#define V_ExtractFileExtension _V_ExtractFileExtension

// Returns a pointer to the file extension or NULL if one doesn't exist
PLATFORM_INTERFACE const char *V_GetFileExtension( const char *path );

// (everything after the last path seperator)
PLATFORM_INTERFACE const char *V_GetFileExtensionSafe( const char *path );

// Dear branch integrator: V_GetFileName was removed! It was identical to V_UnqualifiedFileName.
inline const char *V_GetFileName( const char * path ) { return V_UnqualifiedFileName( path ); }

// This removes "./" and "../" from the pathname. pFilename should be a full pathname.
// Returns false if it tries to ".." past the root directory in the drive (in which case 
// it is an invalid path).
PLATFORM_INTERFACE bool V_RemoveDotSlashes( char *pFilename, char separator = CORRECT_PATH_SEPARATOR );

// If pPath is a relative path, this function makes it into an absolute path
// using the current working directory as the base, or pStartingDir if it's non-NULL.
// Returns false if it runs out of room in the string, or if pPath tries to ".." past the root directory.
PLATFORM_INTERFACE
#if defined(_MSC_VER) && _MSC_VER >= 1900
bool
#else
void
#endif
_V_MakeAbsolutePath( char *pOut, int outLen, const char *pPath, const char *pStartingDir = NULL, bool bLowercaseName = false );
#define V_MakeAbsolutePath _V_MakeAbsolutePath
PLATFORM_INTERFACE char *V_MakeAbsolutePathBuffer(CBufferString *pBuffer, const char *pPath);
PLATFORM_INTERFACE char *V_MakeAbsolutePathBufferUnfixed(CBufferString *pBuffer, const char *pPath, const char *pPath2 = NULL);

// Creates a relative path given two full paths
// The first is the full path of the file to make a relative path for.
// The second is the full path of the directory to make the first file relative to
// Returns false if they can't be made relative (on separate drives, for example)
PLATFORM_INTERFACE bool _V_MakeRelativePath( const char *pFullPath, const char *pDirectory, char *pRelativePath, int nBufLen );
#define V_MakeRelativePath _V_MakeRelativePath

// Fixes up a file name, replacing ' ' with '_'
PLATFORM_INTERFACE void V_FixupPathSpaceToUnderscore( char *pPath );

// Fixes up a file name, removing dot slashes, fixing slashes, converting to lowercase, etc.
PLATFORM_INTERFACE void _V_FixupPathName( OUT_Z_CAP(nOutLen) char *pOut, int nOutLen, IN_Z const char *pPath );
#define V_FixupPathName _V_FixupPathName

// Adds a path separator to the end of the string if there isn't one already and the string is not empty.
// Triggers a fatal error if it would run out of space.
PLATFORM_INTERFACE void _V_AppendSlash( INOUT_Z_CAP(strSize) char *pStr, int strSize, char separator = CORRECT_PATH_SEPARATOR );
#define V_AppendSlash _V_AppendSlash

// Returns true if the path is an absolute path.
PLATFORM_INTERFACE bool V_IsAbsolutePath( IN_Z const char *pPath );

// Scans pIn and replaces all occurences of pMatch with pReplaceWith.
// Writes the result to pOut.
// Returns true if it completed successfully.
// If it would overflow pOut, it fills as much as it can and returns false.
PLATFORM_INTERFACE bool _V_StrSubst( IN_Z const char *pIn, IN_Z const char *pMatch, IN_Z const char *pReplaceWith, OUT_Z_CAP(outLen) char *pOut, int outLen, bool bCaseSensitive=false );
#define V_StrSubst _V_StrSubst

// Split the specified string on the specified separator.
// Returns a list of strings separated by pSeparator.
// You are responsible for freeing the contents of outStrings (call outStrings.PurgeAndDeleteElements).
// You can pass in a CUtlStringList to hold the results, and it will automatically free the strings,
// but it does this in the destructor so if you make multiple calls to V_SplitString with the same
// object you must call outStrings.PurgeAndDeleteElements between each call.
// If you copy and retain pointers to any of the strings you must zero them before calling
// PurgeAndDeleteElements to avoid having a pointer to freed memory.
PLATFORM_OVERLOAD void V_SplitString( IN_Z const char *pString, IN_Z const char *pSeparator, CUtlVector< char *, int, CUtlMemory< char *, int > > &outStrings );
PLATFORM_OVERLOAD void V_SplitString( const char *pString, const char *pSeparator, CUtlVector< CUtlString, int, CUtlMemory< CUtlString, int > > &outStrings, bool bIncludeEmptyStrings = false );
PLATFORM_OVERLOAD void V_SplitStringInPlace( const char *pString, const char **pSeparators, int nSeparators, CUtlVector< char *, int, CUtlMemory< char * , int > > &outStrings );
PLATFORM_OVERLOAD void V_SplitStringOnSpectators( const char *pString, const char **pSeparators, int nSeparators, CUtlVector< char *, int, CUtlMemory< char * , int > > &outStrings );
PLATFORM_OVERLOAD void V_SplitStringOnSpectators( const char *pString, const char **pSeparators, int nSeparators, CUtlVector< CUtlString, int, CUtlMemory< CUtlString , int > > &outStrings );

// Just like V_SplitString, but it can use multiple possible separators.
PLATFORM_OVERLOAD void V_SplitString2( const char *pString, const char **pSeparators, int nSeparators, CUtlVector< char *, int, CUtlMemory< char * , int > > &outStrings );
PLATFORM_OVERLOAD void V_SplitString2InPlace( const char *pString, const char **pSeparators, int nSeparators, CUtlVector< char *, int, CUtlMemory< char * , int > > &outStrings );

// Split string for wide character strings
PLATFORM_INTERFACE void V_SplitWString( const wchar_t *pString, const wchar_t *pSeparator, CUtlVector< wchar_t *, int, CUtlMemory< wchar_t * , int > > &outStrings );

// Just like V_SplitString, but for wide character strings
PLATFORM_INTERFACE void V_SplitWString2( const wchar_t *pString, const wchar_t **pSeparators, int nSeparators, CUtlVector< wchar_t * , int, CUtlMemory< wchar_t * , int > > &outStrings );

// Returns false if the buffer is not large enough to hold the working directory name.
PLATFORM_INTERFACE bool Plat_GetCurrentDirectory( char *pOut, int maxLen );

// Set the working directory thus.
PLATFORM_INTERFACE bool Plat_SetCurrentDirectory( const char *pDirName );

typedef enum
{
	PATTERN_NONE		= 0x00000000,
	PATTERN_DIRECTORY	= 0x00000001
} TStringPattern;
// String matching using wildcards (*) for partial matches.
inline bool V_StringMatchesPattern( const char* pszSource, const char* pszPattern, int nFlags = 0 )
{
	bool bExact = true;
	while( 1 )
	{
		if ( ( *pszPattern ) == 0 )
		{
			return ( (*pszSource ) == 0 );
		}

		if ( ( *pszPattern ) == '*' )
		{
			pszPattern++;

			if ( ( *pszPattern ) == 0 )
			{
				return true;
			}

			bExact = false;
			continue;
		}

		int nLength = 0;

		while( ( *pszPattern ) != '*' && ( *pszPattern ) != 0 )
		{
			nLength++;
			pszPattern++;
		}

		while( 1 )
		{
			const char *pszStartPattern = pszPattern - nLength;
			const char *pszSearch = pszSource;

			for( int i = 0; i < nLength; i++, pszSearch++, pszStartPattern++ )
			{
				if ( ( *pszSearch ) == 0 )
				{
					return false;
				}

				if ( ( *pszSearch ) != ( *pszStartPattern ) )
				{
					break;
				}
			}

			if ( pszSearch - pszSource == nLength )
			{
				break;
			}

			if ( bExact == true )
			{
				return false;
			}

			if ( ( nFlags & PATTERN_DIRECTORY ) != 0 )
			{
				if ( ( *pszPattern ) != '/' && ( *pszSource ) == '/' )
				{
					return false;
				}
			}

			pszSource++;
		}

		pszSource += nLength;
	}
}

// This function takes a slice out of pStr and stores it in pOut.
// It follows the Python slice convention:
// Negative numbers wrap around the string (-1 references the last character).
// Large numbers are clamped to the end of the string.
PLATFORM_INTERFACE void _V_StrSlice( const char *pStr, int firstChar, int lastCharNonInclusive, char *pOut, int outSize );
#define V_StrSlice _V_StrSlice

// Chop off the left nChars of a string.
PLATFORM_INTERFACE void _V_StrLeft( const char *pStr, int nChars, char *pOut, int outSize );
#define V_StrLeft _V_StrLeft

// Chop off the right nChars of a string.
PLATFORM_INTERFACE void _V_StrRight( const char *pStr, int nChars, char *pOut, int outSize );
#define V_StrRight _V_StrRight

// change "special" characters to have their c-style backslash sequence. like \n, \r, \t, ", etc.
// returns a pointer to a newly allocated string, which you must delete[] when finished with.
PLATFORM_INTERFACE char *V_AddBackSlashesToSpecialChars( char const *pSrc );

// Force slashes of either type to be = separator character
PLATFORM_INTERFACE void V_FixSlashes( char *pname, char separator = CORRECT_PATH_SEPARATOR );

// This function fixes cases of filenames like materials\\blah.vmt or somepath\otherpath\\ and removes the extra double slash.
PLATFORM_INTERFACE void V_FixDoubleSlashes( char *pStr );

// Check if 2 paths are the same, works if slashes are different.
PLATFORM_INTERFACE bool V_PathsMatch( const char *pPath1, const char *pPath2 );

// // Convert multibyte to wchar + back
// // Specify -1 for nInSize for null-terminated string
// PLATFORM_INTERFACE void V_strtowcs( const char *pString, int nInSize, wchar_t *pWString, int nOutSizeInBytes );

// buffer-safe strcat
inline void V_strcat( INOUT_Z_CAP(cchDest) char *dest, const char *src, int cchDest )
{
	V_strncat( dest, src, cchDest, COPY_ALL_CHARACTERS );
}

// Buffer safe wcscat
inline void V_wcscat( INOUT_Z_CAP(cchDest) wchar_t *dest, const wchar_t *src, int cchDest )
{
	V_wcsncat( dest, src, cchDest, COPY_ALL_CHARACTERS );
}

PLATFORM_INTERFACE char *V_Int8ToString_Unsafe(int8 nValue, char *pDest);
PLATFORM_INTERFACE char *V_Int16ToString_Unsafe(int16 nValue, char *pDest);
PLATFORM_INTERFACE char *V_Int32ToString_Unsafe(int32 nValue, char *pDest);
PLATFORM_INTERFACE char *V_Int64ToString_Unsafe(int64 nValue, char *pDest);
PLATFORM_INTERFACE char *V_UInt8ToString_Unsafe(uint8 nValue, char *pDest);
PLATFORM_INTERFACE char *V_UInt16ToString_Unsafe(uint16 nValue, char *pDest);
PLATFORM_INTERFACE char *V_UInt32ToString_Unsafe(uint32 nValue, char *pDest);
PLATFORM_INTERFACE char *V_UInt64ToString_Unsafe(uint64 nValue, char *pDest);
PLATFORM_INTERFACE char *V_FloatToString_Unsafe(float flValue, char *pDest);
PLATFORM_INTERFACE char *V_DoubleToString_Unsafe(double fl64Value, char *pDest);
PLATFORM_INTERFACE char *V_ParseURLQuery_Unsafe(const char *pchURL, CUtlVector< CBufferString, int, CUtlMemory< CBufferString, int > > &pDest);

PLATFORM_INTERFACE void V_StringParseError( int nFlags, IParsingErrorListener *err_listener, const char *pError, ...) FMTFUNCTION( 3, 4 );

// Compares two strings with the support of wildcarding only for the first arg (includes '*' for multiple and '?' for single char usages)
PLATFORM_INTERFACE int V_CompareNameWithWildcards(const char *wildcarded_string, const char *compare_to, bool case_sensitive = false);

// Parses string equivalent of ("true", "false", "yes", "no", "1", "0") to the boolean value
// where default_value is what would be returned if parsing has failed
PLATFORM_INTERFACE bool V_StringToBool(const char *buf, bool default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string into a float array up to the amount of arr_size or up to the string limit, the amount of parsed values is returned
PLATFORM_INTERFACE int V_StringToFloatArray(const char *buf, float *out_arr, int arr_size, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string into an int array up to the amount of arr_size or up to the string limit, the amount of parsed values is returned
PLATFORM_INTERFACE int V_StringToIntArray(const char *buf, int *out_arr, int arr_size, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string into a Vector structure
PLATFORM_INTERFACE void V_StringToVector(const char *buf, Vector &out_vec, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string into a Vector2D structure
PLATFORM_INTERFACE void V_StringToVector2D(const char *buf, Vector2D &out_vec, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string into a Vector4D structure
PLATFORM_INTERFACE void V_StringToVector4D(const char *buf, Vector4D &out_vec, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string into a Color structure
PLATFORM_INTERFACE void V_StringToColor(const char *buf, Color &out_clr, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string into a QAngle structure
PLATFORM_INTERFACE void V_StringToQAngle(const char *buf, QAngle &out_ang, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string into a Quaternion structure
PLATFORM_INTERFACE void V_StringToQuaternion(const char *buf, Quaternion &out_quat, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string as a uint64 value, where if the value exceeds min/max limits (inclusive), the parsing fails and default_value is returned
PLATFORM_INTERFACE uint64 V_StringToUint64Limit(const char *buf, uint64 min, uint64 max, uint64 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string as a int64 value, where if the value exceeds min/max limits (inclusive), the parsing fails and default_value is returned
PLATFORM_INTERFACE int64 V_StringToInt64Limit(const char *buf, int64 min, int64 max, int64 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string as a uint64 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE uint64 V_StringToUint64(const char *buf, uint64 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string as a int64 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE int64 V_StringToInt64(const char *buf, int64 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string as a uint32 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE uint32 V_StringToUint32(const char *buf, uint32 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string as a int32 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE int32 V_StringToInt32(const char *buf, int32 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string as a uint16 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE uint16 V_StringToUint16(const char *buf, uint16 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string as a int16 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE int16 V_StringToInt16(const char *buf, int16 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string as a uint8 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE uint8 V_StringToUint8(const char *buf, uint8 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string as a int8 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE int8 V_StringToInt8(const char *buf, int8 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string as a float64 value, where if the value exceeds min/max limits (inclusive), the parsing fails and default_value is returned
PLATFORM_INTERFACE float64 V_StringToFloat64Limit(const char *buf, float64 min, float64 max, float64 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string as a float64 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE float64 V_StringToFloat64(const char *buf, float64 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string as a float32 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE float32 V_StringToFloat32(const char *buf, float32 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IParsingErrorListener *err_listener = NULL);

// Parses string as a float64 value, if the parsing fails, default_value is returned, doesn't perform error checking/reporting
PLATFORM_INTERFACE float64 V_StringToFloat64Raw(const char *buf, float64 default_value, bool *successful = NULL, char **remainder = NULL);

// Parses string as a float32 value, if the parsing fails, default_value is returned, doesn't perform error checking/reporting
PLATFORM_INTERFACE float32 V_StringToFloat32Raw(const char *buf, float32 default_value, bool *successful = NULL, char **remainder = NULL);

// Convert \r\n (Windows linefeeds) to \n (Unix linefeeds).
PLATFORM_INTERFACE void V_TranslateLineFeedsToUnix( char *pStr );

/// Given two file paths, return the length of the common starting substring containing matching components. Case insensitive, and treats slash and backslash the same
/// Assert( LengthOfMatchingPaths( "d:/a/b", "d:/a/b" ) == 6 );
/// Assert( LengthOfMatchingPaths( "d:/a/b", "d:/a/" ) == 5 );
/// Assert( LengthOfMatchingPaths( "d:/a/b", "d:/a" ) == 5 );
/// Assert( LengthOfMatchingPaths( "d:/a/ba", "d:/a/b" ) == 5 );
PLATFORM_INTERFACE int LengthOfMatchingPaths( char const *pFilenamePath, char const *pMatchPath );

PLATFORM_INTERFACE inline void V_RemoveFormatSpecifications( const char *pszFrom, char *pszTo, size_t sizeDest );

// If pBreakCharacters == NULL, then the tokenizer will split tokens at the following characters:
//    { } ( ) ' : 
// White-space, '//' comments, and quoted strings are always handled.
PLATFORM_INTERFACE char const *V_ParseToken( char const *pStrIn, OUT_Z_CAP(bufsize) char *pToken, int bufsize, bool *pbOverflowed = NULL, struct characterset_t *pTokenBreakCharacters = NULL ); 

// Parses a single line, does not trim any whitespace from start or end.  Does not include the final '\n'.
PLATFORM_INTERFACE char const *V_ParseLine( char const *pStrIn, OUT_Z_CAP(bufsize) char *pToken, int bufsize, bool *pbOverflowed = NULL );

// Returns true if additional data is waiting to be processed on this line
PLATFORM_INTERFACE bool V_TokenWaiting( const char *buffer );

// Performs boyer moore search, returns offset of first occurrence of needle in haystack, or -1 on failure.  Note that haystack and the needle can be binary (non-text) data
PLATFORM_INTERFACE int V_BoyerMooreSearch( const byte *pNeedle, int nNeedleLength, const byte *pHayStack, int nHayStackLength );

// Creates a random ascii string (alphanumeric only) of specified length
PLATFORM_OVERLOAD CUtlString V_RandomString( int nLength );

// Encode a string for display as HTML -- this only encodes ' " & < >, which are the important ones to encode for 
// security and ensuring HTML display doesn't break.  Other special chars like the ? sign and so forth will not
// be encoded
//
// Returns false if there was not enough room in pDest to encode the entire source string, otherwise true
PLATFORM_INTERFACE bool V_BasicHtmlEntityEncode( OUT_Z_CAP( nDestSize ) char *pDest, const int nDestSize, char const *pIn, const int nInSize, bool bPreserveWhitespace = false );

// Decode a string with htmlentities HTML -- this should handle all special chars, not just the ones Q_BasicHtmlEntityEncode uses.
//
// Returns false if there was not enough room in pDest to decode the entire source string, otherwise true
PLATFORM_INTERFACE bool V_HtmlEntityDecodeToUTF8( OUT_Z_CAP( nDestSize ) char *pDest, const int nDestSize, char const *pIn, const int nInSize );

// strips HTML from a string.  Should call Q_HTMLEntityDecodeToUTF8 afterward.
PLATFORM_INTERFACE void V_StripAndPreserveHTML( CUtlBuffer *pbuffer, const char *pchHTML, const char **rgszPreserveTags, uint cPreserveTags, uint cMaxResultSize );
PLATFORM_INTERFACE void V_StripAndPreserveHTMLCore( CUtlBuffer *pbuffer, const char *pchHTML, const char **rgszPreserveTags, uint cPreserveTags, const char **rgszNoCloseTags, uint cNoCloseTags, uint cMaxResultSize );

// Extracts the domain from a URL
PLATFORM_INTERFACE bool V_ExtractDomainFromURL( const char *pchURL, OUT_Z_CAP( cchDomain ) char *pchDomain, int cchDomain );

// returns true if the url passed in is on the specified domain
PLATFORM_INTERFACE bool V_URLContainsDomain( const char *pchURL, const char *pchDomain );

//-----------------------------------------------------------------------------
// returns true if the character is allowed in a URL, false otherwise
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE bool V_IsValidURLCharacter( const char *pch, int *pAdvanceBytes );

//-----------------------------------------------------------------------------
// returns true if the character is allowed in a DNS doman name, false otherwise
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE bool V_IsValidDomainNameCharacter( const char *pch, int *pAdvanceBytes );

 // Converts BBCode tags to HTML tags
PLATFORM_INTERFACE bool V_BBCodeToHTML( OUT_Z_CAP( nDestSize ) char *pDest, const int nDestSize, char const *pIn, const int nInSize );


// helper to identify "mean" spaces, which we don't like in visible identifiers
// such as player Name
PLATFORM_INTERFACE bool V_IsMeanSpaceW( wchar_t wch );

// helper to identify characters which are deprecated in Unicode,
// and we simply don't accept
PLATFORM_INTERFACE bool V_IsDeprecatedW( wchar_t wch );

//-----------------------------------------------------------------------------
// generic unique name helper functions
//-----------------------------------------------------------------------------

// returns -1 if no match, nDefault if pName==prefix, and N if pName==prefix+N
PLATFORM_INTERFACE int V_IndexAfterPrefix( const char *pName, const char *prefix, int nDefault = 0 );

// returns startindex if none found, 2 if "prefix" found, and n+1 if "prefixn" found
template < class NameArray >
int V_GenerateUniqueNameIndex( const char *prefix, const NameArray &nameArray, int startindex = 0 )
{
	if ( !prefix )
		return 0;

	int freeindex = startindex;

	int nNames = nameArray.Count();
	for ( int i = 0; i < nNames; ++i )
	{
		int index = V_IndexAfterPrefix( nameArray[ i ], prefix, 1 ); // returns -1 if no match, 0 for exact match, N for 
		if ( index >= freeindex )
		{
			// TODO - check that there isn't more junk after the index in pElementName
			freeindex = index + 1;
		}
	}

	return freeindex;
}

template < class NameArray >
bool V_GenerateUniqueName( char *name, int memsize, const char *prefix, const NameArray &nameArray )
{
	if ( name == NULL || memsize == 0 )
		return false;

	if ( prefix == NULL )
	{
		name[ 0 ] = '\0';
		return false;
	}

	int prefixLength = V_strlen( prefix );
	if ( prefixLength + 1 > memsize )
	{
		name[ 0 ] = '\0';
		return false;
	}

	int i = V_GenerateUniqueNameIndex( prefix, nameArray );
	if ( i <= 0 )
	{
		V_strncpy( name, prefix, memsize );
		return true;
	}

	int newlen = prefixLength + ( int )log10( ( float )i ) + 1;
	if ( newlen + 1 > memsize )
	{
		V_strncpy( name, prefix, memsize );
		return false;
	}

	V_snprintf( name, memsize, "%s%d", prefix, i );
	return true;
}

static char s_hex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

inline int HexToValue( char hex )
{
	if( hex >= '0' && hex <= '9' )
	{
		return hex - '0';
	}
	if( hex >= 'A' && hex <= 'F' )
	{
		return hex - 'A' + 10;
	}
	if( hex >= 'a' && hex <= 'f' )
	{
		return hex - 'a' + 10;
	}
	// report error here
	return -1;
}

inline bool V_StringToBin( const char*pString, void *pBin, uint nBinSize )
{
	if ( (uint)V_strlen( pString ) != nBinSize * 2 )
	{
		return false;
	}

	for ( uint i = 0; i < nBinSize; ++i )
	{
		int high = HexToValue( pString[i*2+0] );
		int low  = HexToValue( pString[i*2+1] ) ;
		if( high < 0 || low < 0 )
		{
			return false;
		}

		( ( uint8* )pBin )[i] = uint8( ( high << 4 ) | low );
	}
	return true;
}

inline bool V_BinToString( char*pString, void *pBin, uint nBinSize )
{
	for ( uint i = 0; i < nBinSize; ++i )
	{
		pString[i*2+0] = s_hex[( ( uint8* )pBin )[i] >> 4 ];
		pString[i*2+1] = s_hex[( ( uint8* )pBin )[i] & 0xF];
	}
	pString[nBinSize*2] = '\0';

	return true;
}

template<typename T>
struct BinString_t
{
	BinString_t(){}
	explicit BinString_t( const char *pStr )
	{
		V_strncpy( m_string, pStr, sizeof(m_string) );
		ToBin();
	}
	explicit BinString_t( const T & that )
	{
		m_bin = that;
		ToString();
	}
	bool ToBin()
	{
		return V_StringToBin( m_string, &m_bin, sizeof( m_bin ) );
	}
	void ToString()
	{
		V_BinToString( m_string, &m_bin, sizeof( m_bin ) );
	}
	T m_bin;
	char m_string[sizeof(T)*2+2]; // 0-terminated string representing the binary data in hex
};

template <typename T>
inline BinString_t<T> MakeBinString( const T& that )
{
	return BinString_t<T>( that );
}

// Encodes a string (or binary data) in URL encoding format, see rfc1738 section 2.2.
// Dest buffer should be 3 times the size of source buffer to guarantee it has room to encode.
PLATFORM_INTERFACE void V_URLEncodeRaw( OUT_Z_CAP(nDestLen) char *pchDest, int nDestLen, const char *pchSource, int nSourceLen );
#define Q_URLEncodeRaw V_URLEncodeRaw

// Decodes a string (or binary data) from URL encoding format, see rfc1738 section 2.2.
// Dest buffer should be at least as large as source buffer to gurantee room for decode.
// Dest buffer being the same as the source buffer (decode in-place) is explicitly allowed.
//
// Returns the amount of space actually used in the output buffer.  
PLATFORM_INTERFACE size_t V_URLDecodeRaw( OUT_CAP(nDecodeDestLen) char *pchDecodeDest, int nDecodeDestLen, const char *pchEncodedSource, int nEncodedSourceLen );
#define Q_URLDecodeRaw V_URLDecodeRaw

// Encodes a string (or binary data) in URL encoding format, this isn't the strict rfc1738 format, but instead uses + for spaces.  
// This is for historical reasons and HTML spec foolishness that lead to + becoming a de facto standard for spaces when encoding form data.
// Dest buffer should be 3 times the size of source buffer to guarantee it has room to encode.
PLATFORM_INTERFACE void V_URLEncode( OUT_Z_CAP(nDestLen) char *pchDest, int nDestLen, const char *pchSource, int nSourceLen );
#define Q_URLEncode V_URLEncode

// Decodes a string (or binary data) in URL encoding format, this isn't the strict rfc1738 format, but instead uses + for spaces.  
// This is for historical reasons and HTML spec foolishness that lead to + becoming a de facto standard for spaces when encoding form data.
// Dest buffer should be at least as large as source buffer to gurantee room for decode.
// Dest buffer being the same as the source buffer (decode in-place) is explicitly allowed.
//
// Returns the amount of space actually used in the output buffer.  
PLATFORM_INTERFACE size_t V_URLDecode( OUT_CAP(nDecodeDestLen) char *pchDecodeDest, int nDecodeDestLen, const char *pchEncodedSource, int nEncodedSourceLen );
#define Q_URLDecode V_URLDecode

//-----------------------------------------------------------------------------
// Purpose: Utility class to allow parsing of various types of data
//-----------------------------------------------------------------------------

template< class V >
struct TypeParser_t
{
	static int ComputeMaxStringSize( const V *pValue ) { return 1; }
	static bool WriteToString( const V *pValue, OUT_Z_CAP(nBufLen) char *pBuf, int nBufLen ) { *pBuf = 0; return false; }
	static bool ParseFromString( const char *pString, V *pValue ) { return false; }
};

// Used to put TypeParser_t function calls in arrays w/ common prototypes
template< class V >
struct TypeParserAdapter_t
{
	static bool WriteToString( const void *pValue, OUT_Z_CAP(nBufLen) char *pBuf, int nBufLen ) { return TypeParser_t<V>::WriteToString( ( const V* )pValue, pBuf, nBufLen ); }
	static int ComputeMaxStringSize( const void *pValue ) { return TypeParser_t<V>::ComputeMaxStringSize( ( const V* )pValue ); }
	static bool ParseFromString( const char *pString, void *pValue ) { return TypeParser_t<V>::ParseFromString( pString, ( V* )pValue ); }
};

template <> struct TypeParser_t< bool >		
{															
	static int ComputeMaxStringSize( const bool *pValue ) { return 6; } //"false"

	static bool WriteToString( const bool *pValue, OUT_Z_CAP(nBufLen) char *pBuf, int nBufLen )
	{
		int nLenWritten = V_snprintf( pBuf, ( int )nBufLen, (*pValue) ? "true" : "false" );
		return ( nLenWritten < nBufLen );
	}

	static bool ParseFromString( const char *pString, bool *pValue )
	{ 
		if ( !V_stricmp( pString, "true" ) )
		{
			*pValue = true;
			return true;
		}
		if ( !V_stricmp( pString, "false" ) )
		{
			*pValue = false;
			return true;
		}
		int nValue;
		if ( sscanf( pString, "%d", &nValue ) != 1 )
			return false;
		if ( nValue != 0 && nValue != 1 )
			return false;
		*pValue = ( nValue == 1 );
		return true;
	}
};

#define DECLARE_POD_TYPE_PARSE_BASE( type, formatspecifier, maxstringdigits, SCAN ) \
	template <> struct TypeParser_t< type >							\
	{																	\
		static int ComputeMaxStringSize( const type *pValue ) { return maxstringdigits; }	\
																								\
		static bool WriteToString( const type *pValue, OUT_Z_CAP(nBufLen) char *pBuf, int nBufLen )			\
		{																						\
			int nLenWritten = V_snprintf( pBuf, nBufLen, formatspecifier, *pValue );		\
			return ( nLenWritten < nBufLen );													\
		}																						\
		static bool ParseFromString( const char *pString, type *pValue )					\
		{																						\
			SCAN							\
		}																						\
	};


#define DECLARE_POD_TYPE_PARSE( type, formatspecifier, maxstringdigits )  DECLARE_POD_TYPE_PARSE_BASE( type, formatspecifier, maxstringdigits, { return ( sscanf( pString, formatspecifier, pValue ) == 1 ); } )
#define DECLARE_POD_TYPE_PARSE_SMALLINT( type, formatspecifier, maxstringdigits, BIGINT )  DECLARE_POD_TYPE_PARSE_BASE( type, formatspecifier, maxstringdigits, { BIGINT nScannedValue; if( sscanf( pString, formatspecifier, &nScannedValue ) == 1 ) { *pValue = ( type )nScannedValue; return true; } else return false; } )

// NOTE: MS compiler won't allow sscanf of a 1-byte int, and will override memory if you try to sscanf( %d, int8 )

DECLARE_POD_TYPE_PARSE_SMALLINT( int8, "%d", 5, int ); // -127
DECLARE_POD_TYPE_PARSE_SMALLINT( uint8, "%u", 5, uint );

DECLARE_POD_TYPE_PARSE_SMALLINT( int16, "%d", 7, int ); // -32767
DECLARE_POD_TYPE_PARSE_SMALLINT( uint16, "%u", 7, uint );

DECLARE_POD_TYPE_PARSE( int32, "%d", 12 );//MIN_INT
DECLARE_POD_TYPE_PARSE( uint32, "%u", 12 );//UINT_MAX

DECLARE_POD_TYPE_PARSE( int64, "%lld", 21 );  // 18446744073709551615  -9223372036854775808
DECLARE_POD_TYPE_PARSE( uint64, "%llu", 21 );

DECLARE_POD_TYPE_PARSE( float32, "%f", 48 ); // -FLT32_MAX
DECLARE_POD_TYPE_PARSE( float64, "%lf", 318 ); // -FLOAT64_MAX (it's got a lot of zeros)

template <> struct TypeParser_t< Color >		
{															
	static int ComputeMaxStringSize( const Color *pValue ) { return 18; } //{255 255 255 255}

	static bool WriteToString( const Color *pValue, OUT_Z_CAP(nBufLen) char *pBuf, int nBufLen )
	{ 
		int nLenWritten = V_snprintf( pBuf, ( int )nBufLen, "{%d %d %d %d}", pValue->r(), pValue->g(), pValue->b(), pValue->a() );
		return ( nLenWritten < nBufLen );
	}

	static bool ParseFromString( const char *pString, Color *pValue )
	{ 
		int r, g, b, a;
		if ( sscanf( pString, "{%d %d %d %d}", &r, &g, &b, &a ) != 4 )
			return false;
		if ( r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255 || a < 0 || a > 255 )
			return false;
		pValue->SetColor( r, g, b, a );
		return true;
	}
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
// This will return a char* valid for the current expression that will make a number comma separated:
//    Log_Msg( LOG_VFXC, "%s", V_FormatNumber( -1234567 ) );
// will output:
//    -1,234,567
//---------------------------------------------------------------------------------------------------------------------------------------------------
#define V_FormatNumber( nNum ) ( CFormatNumber_DontUseDirectly_UseFormatNumberMacro( ( nNum ) ).AsString() )
class CFormatNumber_DontUseDirectly_UseFormatNumberMacro
{
public:
	CFormatNumber_DontUseDirectly_UseFormatNumberMacro() { m_szBuf[0] = '\0'; pString = m_szBuf; }

	// Standard C formatting
	void FormatNumberInternal( uint64 nNum )
	{
		pString = m_szBuf + sizeof( m_szBuf ) - 1;
		*pString-- = '\0';

		if ( nNum == 0 )
		{
			*pString-- = '0';
		}

		for ( uint64 j = 0; nNum > 0; nNum /= 10, j++ )
		{
			// Add comma
			if ( j && !( j % 3 ) )
			{
				*pString-- = ',';
			}

			// Add digit
			*pString-- = '0' + char( nNum % 10 );
		}

		pString++;
	}

	void FormatNumberInternal64( int64 nNum )
	{
		bool bIsNegative = ( nNum < ( int64 )0 );
		uint64 nUnsigned = ( bIsNegative ) ? ( uint64 )( -nNum ) : ( uint64 )( nNum );
		FormatNumberInternal( ( uint64 )nUnsigned );
		if ( bIsNegative )
		{
			pString--;
			*pString = '-';
		}
	}
	CFormatNumber_DontUseDirectly_UseFormatNumberMacro( int64 nNum ) { FormatNumberInternal64( ( int64 )nNum ); }
	CFormatNumber_DontUseDirectly_UseFormatNumberMacro( int32 nNum ) { FormatNumberInternal64( ( int64 )nNum ); }
	CFormatNumber_DontUseDirectly_UseFormatNumberMacro( int16 nNum ) { FormatNumberInternal64( ( int64 )nNum ); }

	void FormatNumberInternalU64( uint64 nNum )
	{
		FormatNumberInternal( nNum );
	}
	CFormatNumber_DontUseDirectly_UseFormatNumberMacro( uint64 nNum ) { FormatNumberInternalU64( ( uint64 )nNum ); }
	CFormatNumber_DontUseDirectly_UseFormatNumberMacro( uint32 nNum ) { FormatNumberInternalU64( ( uint64 )nNum ); }
	CFormatNumber_DontUseDirectly_UseFormatNumberMacro( uint16 nNum ) { FormatNumberInternalU64( ( uint64 )nNum ); }

	// Use for access
	operator const char *() const { return pString; }
	const char *AsString() { return pString; }

private:
	char m_szBuf[64];
	char *pString;
};

// trim right whitespace
PLATFORM_INTERFACE char* TrimRight( char *pString );

PLATFORM_INTERFACE const char * SkipBlanks( const char *pString );

// NOTE: This is for backward compatability!
// We need to DLL-export the Q methods in vstdlib but not link to them in other projects
#if !defined( VSTDLIB_BACKWARD_COMPAT )

#define Q_memset				V_memset
#define Q_memcpy				V_memcpy
#define Q_memmove				V_memmove
#define Q_memcmp				V_memcmp
#define Q_strlen				V_strlen
#define Q_strcpy				V_strcpy
#define Q_strrchr				V_strrchr
#define Q_strcmp				V_strcmp
#define Q_wcscmp				V_wcscmp
#define Q_stricmp				V_stricmp
#define Q_strstr				V_strstr
#define Q_strupr				V_strupr
#define Q_strlower				V_strlower
#define Q_wcslen				V_wcslen
#define	Q_strncmp				V_strncmp 
#define	Q_strcasecmp			V_strcasecmp
#define	Q_strncasecmp			V_strncasecmp
#define	Q_strnicmp				V_strnicmp
#define	Q_atoi					V_atoi
#define	Q_atoi64				V_atoi64
#define Q_atoui64				V_atoui64
#define	Q_atof					V_atof
#define	Q_stristr				V_stristr
#define	Q_strnistr				V_strnistr
#define	Q_strnchr				V_strnchr
#define Q_normalizeFloatString	V_normalizeFloatString
#define Q_strncpy				V_strncpy
#define Q_wcsncpy				V_wcsncpy
#define Q_snprintf				V_snprintf
#define Q_snwprintf				V_snwprintf
#define Q_wcsncpy				V_wcsncpy
#define Q_strncat				V_strncat
#define Q_wcsncat				V_wcsncat
#define Q_strnlwr				V_strnlwr
#define Q_vsnprintf				V_vsnprintf
#define Q_vsnprintfRet			V_vsnprintfRet
#define Q_pretifymem			V_pretifymem
#define Q_pretifynum			V_pretifynum
#define Q_UTF8ToUnicode			V_UTF8ToUnicode
#define Q_UnicodeToUTF8			V_UnicodeToUTF8
#define Q_hextobinary			V_hextobinary
#define Q_binarytohex			V_binarytohex
#define Q_FileBase				V_FileBase
#define Q_StripTrailingSlash	V_StripTrailingSlash
#define Q_StripExtension		V_StripExtension
#define	Q_DefaultExtension		V_DefaultExtension
#define Q_SetExtension			V_SetExtension
#define Q_StripFilename			V_StripFilename
#define Q_StripLastDir			V_StripLastDir
#define Q_UnqualifiedFileName	V_UnqualifiedFileName
#define Q_ComposeFileName		V_ComposeFileName
#define Q_ExtractFilePath		V_ExtractFilePath
#define Q_ExtractFileExtension	V_ExtractFileExtension
#define Q_GetFileExtension		V_GetFileExtension
#define Q_RemoveDotSlashes		V_RemoveDotSlashes
#define Q_MakeAbsolutePath		V_MakeAbsolutePath
#define Q_AppendSlash			V_AppendSlash
#define Q_IsAbsolutePath		V_IsAbsolutePath
#define Q_StrSubst				V_StrSubst
#define Q_SplitString			V_SplitString
#define Q_SplitString2			V_SplitString2
#define Q_StrSlice				V_StrSlice
#define Q_StrLeft				V_StrLeft
#define Q_StrRight				V_StrRight
#define Q_FixSlashes			V_FixSlashes
#define Q_strtowcs				V_strtowcs
#define Q_wcstostr				V_wcstostr
#define Q_strcat				V_strcat
#define Q_wcscat				V_wcscat
#define Q_MakeRelativePath		V_MakeRelativePath
#define Q_FixupPathName			V_FixupPathName
#define Q_qsort_s				V_qsort_s

#endif // !defined( VSTDLIB_DLL_EXPORT )


#if defined(_PS3) || defined(POSIX)
#define PRI_WS_FOR_WS L"%ls"
#define PRI_WS_FOR_S "%ls"
#define PRI_S_FOR_WS L"%s"
#define PRI_S_FOR_S "%s"
#else
#define PRI_WS_FOR_WS L"%s"
#define PRI_WS_FOR_S "%S"
#define PRI_S_FOR_WS L"%S"
#define PRI_S_FOR_S "%s"
#endif

namespace AsianWordWrap
{
	// Functions used by Asian language line wrapping to determine if a character can end a line, begin a line, or be broken up when repeated (eg: "...")
	bool CanEndLine( wchar_t wcCandidate );
	bool CanBeginLine( wchar_t wcCandidate );
	bool CanBreakRepeated( wchar_t wcCandidate );

	// Used to determine if we can break a line between the first two characters passed; calls the above functions on each character
	bool CanBreakAfter( const wchar_t* wsz );
}

// We use this function to determine where it is permissible to break lines
// of text while wrapping them. On most platforms, the native iswspace() function
// returns FALSE for the "non-breaking space" characters 0x00a0 and 0x202f, and so we don't
// break on them. On the 360, however, iswspace returns TRUE for them. So, on that
// platform, we work around it by defining this wrapper which returns false
// for &nbsp; and calls through to the library function for everything else.
PLATFORM_INTERFACE int V_isbreakablewspace( wchar_t ch );
PLATFORM_INTERFACE int V_isbreakablewspace32( wchar_t ch );


// Strip white space at the beginning and end of a string
PLATFORM_INTERFACE int V_StrTrim( char *pStr );


#endif	// TIER1_STRTOOLS_H
