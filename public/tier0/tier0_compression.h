#ifndef TIER0_TIER0_COMPRESSION_H
#define TIER0_TIER0_COMPRESSION_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// The Zstandard, LZ4 (1.9) and xxHash (0.8) libraries tier0 links, exported with the V_ prefix;
// the signatures are the upstream ones. The types are declared unless the upstream headers
// were included first
//-----------------------------------------------------------------------------

// Zstandard
#ifndef ZSTD_H_235446
typedef struct ZSTD_CCtx_s ZSTD_CCtx;
typedef struct ZSTD_DCtx_s ZSTD_DCtx;
typedef ZSTD_DCtx ZSTD_DStream;
typedef struct ZSTD_CDict_s ZSTD_CDict;
typedef struct ZSTD_DDict_s ZSTD_DDict;

typedef struct ZSTD_inBuffer_s
{
	const void *src;
	size_t size;
	size_t pos;
} ZSTD_inBuffer;

typedef struct ZSTD_outBuffer_s
{
	void *dst;
	size_t size;
	size_t pos;
} ZSTD_outBuffer;
#endif // ZSTD_H_235446

PLATFORM_INTERFACE size_t V_ZSTD_compress( void *dst, size_t dstCapacity, const void *src, size_t srcSize, int compressionLevel );
PLATFORM_INTERFACE size_t V_ZSTD_decompress( void *dst, size_t dstCapacity, const void *src, size_t compressedSize );
PLATFORM_INTERFACE unsigned long long V_ZSTD_getFrameContentSize( const void *src, size_t srcSize );
PLATFORM_INTERFACE size_t V_ZSTD_compressBound( size_t srcSize );
PLATFORM_INTERFACE unsigned V_ZSTD_isError( size_t code );
PLATFORM_INTERFACE int V_ZSTD_maxCLevel();
PLATFORM_INTERFACE int V_ZSTD_defaultCLevel();

PLATFORM_INTERFACE ZSTD_CCtx *V_ZSTD_createCCtx();
PLATFORM_INTERFACE size_t V_ZSTD_freeCCtx( ZSTD_CCtx *cctx );
PLATFORM_INTERFACE ZSTD_DCtx *V_ZSTD_createDCtx();
PLATFORM_INTERFACE size_t V_ZSTD_freeDCtx( ZSTD_DCtx *dctx );

// The dictionaries are never freed by tier0, there is no export for it
PLATFORM_INTERFACE ZSTD_CDict *V_ZSTD_createCDict_byReference( const void *dictBuffer, size_t dictSize, int compressionLevel );
PLATFORM_INTERFACE size_t V_ZSTD_compress_usingCDict( ZSTD_CCtx *cctx, void *dst, size_t dstCapacity, const void *src, size_t srcSize, const ZSTD_CDict *cdict );
PLATFORM_INTERFACE ZSTD_DDict *V_ZSTD_createDDict_byReference( const void *dictBuffer, size_t dictSize );
PLATFORM_INTERFACE size_t V_ZSTD_decompress_usingDDict( ZSTD_DCtx *dctx, void *dst, size_t dstCapacity, const void *src, size_t srcSize, const ZSTD_DDict *ddict );
PLATFORM_INTERFACE unsigned V_ZSTD_getDictID_fromDDict( const ZSTD_DDict *ddict );
PLATFORM_INTERFACE unsigned V_ZSTD_getDictID_fromFrame( const void *src, size_t srcSize );

PLATFORM_INTERFACE size_t V_ZSTD_initDStream( ZSTD_DStream *zds );
PLATFORM_INTERFACE size_t V_ZSTD_decompressStream( ZSTD_DStream *zds, ZSTD_outBuffer *output, ZSTD_inBuffer *input );
PLATFORM_INTERFACE size_t V_ZSTD_DStreamInSize();
PLATFORM_INTERFACE size_t V_ZSTD_DStreamOutSize();

// LZ4
#ifndef LZ4_H_2983827168210
typedef union LZ4_stream_u LZ4_stream_t;
typedef union LZ4_streamDecode_u LZ4_streamDecode_t;

// The decoder state the caller allocates, LZ4_STREAMDECODE_MINSIZE
union LZ4_streamDecode_u
{
	char minStateSize[ 32 ];
	unsigned long long table[ 4 ];
};
#endif // LZ4_H_2983827168210

#ifndef LZ4_HC_H_19834876238432
typedef union LZ4_streamHC_u LZ4_streamHC_t;
#endif // LZ4_HC_H_19834876238432

PLATFORM_INTERFACE int V_LZ4_compressBound( int inputSize );
PLATFORM_INTERFACE int V_LZ4_compress_fast( const char *src, char *dst, int srcSize, int dstCapacity, int acceleration );
PLATFORM_INTERFACE int V_LZ4_decompress_safe( const char *src, char *dst, int compressedSize, int dstCapacity );

PLATFORM_INTERFACE LZ4_stream_t *V_LZ4_createStream();
PLATFORM_INTERFACE int V_LZ4_freeStream( LZ4_stream_t *streamPtr );
PLATFORM_INTERFACE void V_LZ4_resetStream_fast( LZ4_stream_t *streamPtr );
PLATFORM_INTERFACE void V_LZ4_attach_dictionary( LZ4_stream_t *workingStream, const LZ4_stream_t *dictionaryStream );
PLATFORM_INTERFACE int V_LZ4_compress_fast_continue( LZ4_stream_t *streamPtr, const char *src, char *dst, int srcSize, int dstCapacity, int acceleration );

PLATFORM_INTERFACE int V_LZ4_decoderRingBufferSize( int maxBlockSize );
PLATFORM_INTERFACE int V_LZ4_decompress_safe_continue( LZ4_streamDecode_t *LZ4_streamDecode, const char *src, char *dst, int srcSize, int dstCapacity );

// The level is clamped to [ 3, 12 ], 12 for a level of 0 or below
PLATFORM_INTERFACE int V_LZ4_compress_HC( const char *src, char *dst, int srcSize, int dstCapacity, int compressionLevel );
PLATFORM_INTERFACE LZ4_streamHC_t *V_LZ4_createStreamHC();
PLATFORM_INTERFACE int V_LZ4_freeStreamHC( LZ4_streamHC_t *streamHCPtr );
PLATFORM_INTERFACE void V_LZ4_resetStreamHC_fast( LZ4_streamHC_t *streamHCPtr, int compressionLevel );
PLATFORM_INTERFACE void V_LZ4_attach_HC_dictionary( LZ4_streamHC_t *working_stream, const LZ4_streamHC_t *dictionary_stream );
PLATFORM_INTERFACE int V_LZ4_compress_HC_continue( LZ4_streamHC_t *streamHCPtr, const char *src, char *dst, int srcSize, int maxDstSize );

// xxHash
#ifndef XXHASH_H_5627135585666179
typedef enum
{
	XXH_OK = 0,
	XXH_ERROR
} XXH_errorcode;

typedef unsigned long long XXH64_hash_t;
typedef struct XXH64_state_s XXH64_state_t;
#endif // XXHASH_H_5627135585666179

PLATFORM_INTERFACE XXH64_hash_t V_XXH64( const void *input, size_t length, XXH64_hash_t seed );
PLATFORM_INTERFACE XXH64_state_t *V_XXH64_createState();
PLATFORM_INTERFACE XXH_errorcode V_XXH64_freeState( XXH64_state_t *statePtr );
PLATFORM_INTERFACE void V_XXH64_copyState( XXH64_state_t *dst_state, const XXH64_state_t *src_state );
PLATFORM_INTERFACE XXH_errorcode V_XXH64_reset( XXH64_state_t *statePtr, XXH64_hash_t seed );
PLATFORM_INTERFACE XXH_errorcode V_XXH64_update( XXH64_state_t *statePtr, const void *input, size_t length );
PLATFORM_INTERFACE XXH64_hash_t V_XXH64_digest( const XXH64_state_t *statePtr );

#endif // TIER0_TIER0_COMPRESSION_H
