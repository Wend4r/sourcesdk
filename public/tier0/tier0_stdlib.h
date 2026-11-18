#ifndef TIER0_TIER0_STDLIB_H
#define TIER0_TIER0_STDLIB_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

#include <locale.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>

//-----------------------------------------------------------------------------
// The C runtime of tier0, for modules that must share its file handles, errno and locale
// instead of using their own runtime
//-----------------------------------------------------------------------------

PLATFORM_INTERFACE FILE *V_Get_stdin();
PLATFORM_INTERFACE FILE *V_Get_stdout();
PLATFORM_INTERFACE FILE *V_Get_stderr();

PLATFORM_INTERFACE int V_GetErrno();
PLATFORM_INTERFACE void V_SetErrno( int err );

PLATFORM_INTERFACE struct lconv *V_localeconv();

// Paths are in the runtime's encoding; the UTF8 variants convert the path first on Windows
PLATFORM_INTERFACE FILE *V_fopen( const char *filename, const char *mode );
PLATFORM_INTERFACE FILE *V_fopenUTF8( const char *filename, const char *mode );
PLATFORM_INTERFACE FILE *V_freopen( const char *filename, const char *mode, FILE *stream );
PLATFORM_INTERFACE FILE *V_fdopen( int nFileDescriptor, const char *mode );
PLATFORM_INTERFACE int V_fclose( FILE *stream );
PLATFORM_INTERFACE int V_fflush( FILE *stream );
PLATFORM_INTERFACE int V_setvbuf( FILE *stream, char *buffer, int mode, size_t size );
PLATFORM_INTERFACE int V_fileno( FILE *stream );

PLATFORM_INTERFACE size_t V_fread( void *buffer, size_t nElementSize, size_t nCount, FILE *stream );
PLATFORM_INTERFACE size_t V_fwrite( const void *buffer, size_t nElementSize, size_t nCount, FILE *stream );
PLATFORM_INTERFACE int V_fgetc( FILE *stream );
PLATFORM_INTERFACE int V_ungetc( int c, FILE *stream );
PLATFORM_INTERFACE char *V_fgets( char *str, int n, FILE *stream );
PLATFORM_INTERFACE int V_fputc( int c, FILE *stream );
PLATFORM_INTERFACE int V_fputs( const char *str, FILE *stream );
PLATFORM_INTERFACE int V_fprintf( FILE *stream, const char *format, ... ) FMTFUNCTION( 2, 3 );
PLATFORM_INTERFACE int V_vfprintf( FILE *stream, const char *format, va_list argptr );
PLATFORM_INTERFACE int V_fscanf( FILE *stream, const char *format, ... );

PLATFORM_INTERFACE int V_fseek( FILE *stream, long offset, int origin );
PLATFORM_INTERFACE int V_fseeki64( FILE *stream, int64 offset, int origin );
PLATFORM_INTERFACE long V_ftell( FILE *stream );
PLATFORM_INTERFACE int64 V_ftelli64( FILE *stream );
PLATFORM_INTERFACE void V_rewind( FILE *stream );
PLATFORM_INTERFACE int V_feof( FILE *stream );
PLATFORM_INTERFACE int V_ferror( FILE *stream );
PLATFORM_INTERFACE void V_clearerr( FILE *stream );

PLATFORM_INTERFACE int V_sscanf( const char *buffer, const char *pFormat, ... );
PLATFORM_INTERFACE int V_swscanf( const wchar_t *buffer, const wchar_t *pFormat, ... );

// File descriptors; the flags and the mode of V_open are those of open()
PLATFORM_INTERFACE int V_open( const char *pPath, int nFlags, int nMode );
PLATFORM_INTERFACE int V_openUTF8( const char *pPath, int nFlags, int nMode );
PLATFORM_INTERFACE int V_close( int nFileDescriptor );
PLATFORM_INTERFACE int V_dup( int nFileDescriptor );
PLATFORM_INTERFACE int V_fstat( int nFileDescriptor, struct stat *pStat );

// mkstemps() and mkostemps(): opens a new file from szTemplate, whose XXXXXX in front of the
// last suffixlen chars is replaced, and returns its descriptor
PLATFORM_INTERFACE int V_mkstemps( char *szTemplate, int suffixlen );
PLATFORM_INTERFACE int V_mkostemps( char *szTemplate, int suffixlen, int flags );

PLATFORM_INTERFACE void V_qsort( void *base, size_t num, size_t width, int ( *compare )( const void *, const void * ) );
PLATFORM_INTERFACE void V_qsort_s( void *base, size_t num, size_t width, int ( *compare )( void *, const void *, const void * ), void *context );

PLATFORM_INTERFACE float V_strtof( const char *str, char **endptr );

// CRC-32 of the buffer, continuing from nStartCrc (0 for a new one)
PLATFORM_INTERFACE uint32 V_tier0_crc32( uint32 nStartCrc, const void *pBuf, size_t nBufLen );

PLATFORM_INTERFACE void V_abort();

// setjmp/longjmp of tier0 across the module boundary. V_setjmp saves the context of its own frame,
// which is gone once it returns, so a jump back to it is not usable; V_longjmp works with a
// buffer filled by the setjmp of the caller
typedef struct _jmp_buf_holder
{
	jmp_buf buf;
} jmp_buf_holder;

PLATFORM_INTERFACE int V_setjmp( jmp_buf_holder *env );
PLATFORM_INTERFACE void V_longjmp( jmp_buf_holder *env, int value );

// The <ctype.h> classification and case conversion of tier0
PLATFORM_INTERFACE int VCRT_isalnum( int c );
PLATFORM_INTERFACE int VCRT_isalpha( int c );
PLATFORM_INTERFACE int VCRT_isdigit( int c );
PLATFORM_INTERFACE int VCRT_isprint( int c );
PLATFORM_INTERFACE int VCRT_tolower( int c );
PLATFORM_INTERFACE int VCRT_toupper( int c );

// Plat_CreateOpenTempFile opening the file with pMode, which must start with "w"; null on failure
class CBufferString;
PLATFORM_INTERFACE FILE *Plat_CreateFOpenTempFile( const char *pMode, CBufferString *pOutPath, const char *pPrefix, const char *pDirectory );

#ifdef _WIN32
PLATFORM_INTERFACE int V_stat( const char *pPath, struct stat *pStat );
PLATFORM_INTERFACE int V_setmode( int nFileDescriptor, int nMode );
PLATFORM_INTERFACE intptr_t V_get_osfhandle( int nFileDescriptor );
PLATFORM_INTERFACE int V_open_osfhandle( intptr_t hOSFile, int nFlags );
PLATFORM_INTERFACE char *V_tmpnam( char *pBuffer );
PLATFORM_INTERFACE int V_sscanf_s( const char *buffer, const char *pFormat, ... );
#else
PLATFORM_INTERFACE int ( *V_fcntl )( int nFileDescriptor, int nCommand, ... );
#endif

#endif // TIER0_TIER0_STDLIB_H
