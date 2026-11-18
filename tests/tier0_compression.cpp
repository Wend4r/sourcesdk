#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/tier0_compression.h>

#include <cstring>
#include <vector>

// Text that compresses well
static std::vector< char > MakeTestText()
{
	std::vector< char > text;
	const char *pszLine = "The quick brown fox jumps over the lazy dog. 0123456789\n";

	for ( int i = 0; i < 64; i++ )
		text.insert( text.end(), pszLine, pszLine + strlen( pszLine ) );

	return text;
}

REGISTER_NAMED_TEST( "V_ZSTD_compress.RoundTrip", V_ZSTD_compress_RoundTrip )
{
	const std::vector< char > text = MakeTestText();
	std::vector< char > compressed( V_ZSTD_compressBound( text.size() ) );

	const size_t nCompressed = V_ZSTD_compress( compressed.data(), compressed.size(), text.data(), text.size(), V_ZSTD_defaultCLevel() );
	TEST_FALSE( V_ZSTD_isError( nCompressed ) );
	TEST_TRUE( nCompressed < text.size() );

	// The frame knows the size of the content.
	TEST_EQ( V_ZSTD_getFrameContentSize( compressed.data(), nCompressed ), ( unsigned long long )text.size() );

	std::vector< char > decompressed( text.size() );
	const size_t nDecompressed = V_ZSTD_decompress( decompressed.data(), decompressed.size(), compressed.data(), nCompressed );
	TEST_EQ( nDecompressed, text.size() );
	TEST_EQ( memcmp( decompressed.data(), text.data(), text.size() ), 0 );
}

REGISTER_NAMED_TEST( "V_ZSTD_decompress.CorruptInput", V_ZSTD_decompress_CorruptInput )
{
	char szGarbage[] = "not a zstd frame";
	char szOut[ 64 ];
	TEST_TRUE( V_ZSTD_isError( V_ZSTD_decompress( szOut, sizeof( szOut ), szGarbage, sizeof( szGarbage ) ) ) != 0 );
}

REGISTER_NAMED_TEST( "V_ZSTD_compressBound.AtLeastInput", V_ZSTD_compressBound_AtLeastInput )
{
	TEST_TRUE( V_ZSTD_compressBound( 1000 ) >= 1000 );
	TEST_TRUE( V_ZSTD_compressBound( 0 ) > 0 );
}

REGISTER_NAMED_TEST( "V_ZSTD_isError.Codes", V_ZSTD_isError_Codes )
{
	TEST_EQ( V_ZSTD_isError( 0 ), 0u );
	TEST_EQ( V_ZSTD_isError( 100 ), 0u );
	TEST_TRUE( V_ZSTD_isError( ( size_t )-1 ) != 0 );
}

REGISTER_NAMED_TEST( "V_ZSTD_maxCLevel.Levels", V_ZSTD_maxCLevel_Levels )
{
	TEST_TRUE( V_ZSTD_maxCLevel() >= 19 );
	TEST_TRUE( V_ZSTD_defaultCLevel() > 0 );
	TEST_TRUE( V_ZSTD_defaultCLevel() <= V_ZSTD_maxCLevel() );
}

REGISTER_NAMED_TEST( "V_ZSTD_getFrameContentSize.Unknown", V_ZSTD_getFrameContentSize_Unknown )
{
	// ZSTD_CONTENTSIZE_ERROR for a buffer that is no frame.
	const char szGarbage[] = "garbage!";
	TEST_EQ( V_ZSTD_getFrameContentSize( szGarbage, sizeof( szGarbage ) ), ( unsigned long long )-2 );
}

REGISTER_NAMED_TEST( "V_ZSTD_createCCtx.CreateFree", V_ZSTD_createCCtx_CreateFree )
{
	ZSTD_CCtx *pCCtx = V_ZSTD_createCCtx();
	TEST_NOT_NULL( pCCtx );
	TEST_EQ( V_ZSTD_freeCCtx( pCCtx ), ( size_t )0 );
	TEST_EQ( V_ZSTD_freeCCtx( nullptr ), ( size_t )0 );
}

REGISTER_NAMED_TEST( "V_ZSTD_createDCtx.CreateFree", V_ZSTD_createDCtx_CreateFree )
{
	ZSTD_DCtx *pDCtx = V_ZSTD_createDCtx();
	TEST_NOT_NULL( pDCtx );
	TEST_EQ( V_ZSTD_freeDCtx( pDCtx ), ( size_t )0 );
	TEST_EQ( V_ZSTD_freeDCtx( nullptr ), ( size_t )0 );
}

REGISTER_NAMED_TEST( "V_ZSTD_compress_usingCDict.Dictionary", V_ZSTD_compress_usingCDict_Dictionary )
{
	const std::vector< char > text = MakeTestText();
	const char szDictionary[] = "The quick brown fox jumps over the lazy dog. 0123456789\n";

	ZSTD_CDict *pCDict = V_ZSTD_createCDict_byReference( szDictionary, sizeof( szDictionary ) - 1, 3 );
	ZSTD_DDict *pDDict = V_ZSTD_createDDict_byReference( szDictionary, sizeof( szDictionary ) - 1 );
	TEST_NOT_NULL( pCDict );
	TEST_NOT_NULL( pDDict );

	ZSTD_CCtx *pCCtx = V_ZSTD_createCCtx();
	std::vector< char > compressed( V_ZSTD_compressBound( text.size() ) );
	const size_t nCompressed = V_ZSTD_compress_usingCDict( pCCtx, compressed.data(), compressed.size(), text.data(), text.size(), pCDict );
	TEST_FALSE( V_ZSTD_isError( nCompressed ) );

	ZSTD_DCtx *pDCtx = V_ZSTD_createDCtx();
	std::vector< char > decompressed( text.size() );
	const size_t nDecompressed = V_ZSTD_decompress_usingDDict( pDCtx, decompressed.data(), decompressed.size(), compressed.data(), nCompressed, pDDict );
	TEST_EQ( nDecompressed, text.size() );
	TEST_EQ( memcmp( decompressed.data(), text.data(), text.size() ), 0 );

	// A raw content dictionary has no ID, neither has the frame.
	TEST_EQ( V_ZSTD_getDictID_fromDDict( pDDict ), 0u );
	TEST_EQ( V_ZSTD_getDictID_fromFrame( compressed.data(), nCompressed ), 0u );

	V_ZSTD_freeCCtx( pCCtx );
	V_ZSTD_freeDCtx( pDCtx );
}

REGISTER_NAMED_TEST( "V_ZSTD_createCDict_byReference.Create", V_ZSTD_createCDict_byReference_Create )
{
	const char szDictionary[] = "dictionary content of the test";
	TEST_NOT_NULL( V_ZSTD_createCDict_byReference( szDictionary, sizeof( szDictionary ), 1 ) );
}

REGISTER_NAMED_TEST( "V_ZSTD_createDDict_byReference.Create", V_ZSTD_createDDict_byReference_Create )
{
	const char szDictionary[] = "dictionary content of the test";
	ZSTD_DDict *pDDict = V_ZSTD_createDDict_byReference( szDictionary, sizeof( szDictionary ) );
	TEST_NOT_NULL( pDDict );
	TEST_EQ( V_ZSTD_getDictID_fromDDict( pDDict ), 0u );
}

REGISTER_NAMED_TEST( "V_ZSTD_decompressStream.Chunks", V_ZSTD_decompressStream_Chunks )
{
	const std::vector< char > text = MakeTestText();
	std::vector< char > compressed( V_ZSTD_compressBound( text.size() ) );
	const size_t nCompressed = V_ZSTD_compress( compressed.data(), compressed.size(), text.data(), text.size(), 1 );
	TEST_FALSE( V_ZSTD_isError( nCompressed ) );

	TEST_TRUE( V_ZSTD_DStreamInSize() > 0 );
	TEST_TRUE( V_ZSTD_DStreamOutSize() > 0 );

	ZSTD_DStream *pStream = V_ZSTD_createDCtx();
	TEST_FALSE( V_ZSTD_isError( V_ZSTD_initDStream( pStream ) ) );

	// Feed the frame in pieces of 16 bytes, take the output in pieces of 100.
	std::vector< char > decompressed;
	size_t nFed = 0;
	char out[ 100 ];

	while ( nFed < nCompressed )
	{
		ZSTD_inBuffer input = { compressed.data() + nFed, nCompressed - nFed < 16 ? nCompressed - nFed : 16, 0 };

		while ( input.pos < input.size )
		{
			ZSTD_outBuffer output = { out, sizeof( out ), 0 };
			TEST_FALSE( V_ZSTD_isError( V_ZSTD_decompressStream( pStream, &output, &input ) ) );
			decompressed.insert( decompressed.end(), out, out + output.pos );
		}

		nFed += input.size;
	}

	// Flush what is left.
	for ( ;; )
	{
		ZSTD_inBuffer input = { nullptr, 0, 0 };
		ZSTD_outBuffer output = { out, sizeof( out ), 0 };
		V_ZSTD_decompressStream( pStream, &output, &input );
		decompressed.insert( decompressed.end(), out, out + output.pos );

		if ( output.pos == 0 )
			break;
	}

	TEST_EQ( decompressed.size(), text.size() );
	TEST_EQ( memcmp( decompressed.data(), text.data(), text.size() ), 0 );
	V_ZSTD_freeDCtx( pStream );
}

REGISTER_NAMED_TEST( "V_ZSTD_initDStream.Restarts", V_ZSTD_initDStream_Restarts )
{
	ZSTD_DStream *pStream = V_ZSTD_createDCtx();
	const size_t nHint = V_ZSTD_initDStream( pStream );
	TEST_FALSE( V_ZSTD_isError( nHint ) );
	TEST_TRUE( nHint > 0 );
	TEST_EQ( V_ZSTD_initDStream( pStream ), nHint );
	V_ZSTD_freeDCtx( pStream );
}

REGISTER_NAMED_TEST( "V_ZSTD_DStreamInSize.Sizes", V_ZSTD_DStreamInSize_Sizes )
{
	// A block plus its header, and a whole block.
	TEST_TRUE( V_ZSTD_DStreamInSize() > ( size_t )( 128 * 1024 ) );
	TEST_EQ( V_ZSTD_DStreamOutSize(), ( size_t )( 128 * 1024 ) );
}

REGISTER_NAMED_TEST( "V_LZ4_compress_fast.RoundTrip", V_LZ4_compress_fast_RoundTrip )
{
	const std::vector< char > text = MakeTestText();
	const int nBound = V_LZ4_compressBound( ( int )text.size() );
	TEST_TRUE( nBound >= ( int )text.size() );

	std::vector< char > compressed( nBound );
	const int nCompressed = V_LZ4_compress_fast( text.data(), compressed.data(), ( int )text.size(), nBound, 1 );
	TEST_TRUE( nCompressed > 0 );
	TEST_TRUE( nCompressed < ( int )text.size() );

	std::vector< char > decompressed( text.size() );
	TEST_EQ( V_LZ4_decompress_safe( compressed.data(), decompressed.data(), nCompressed, ( int )decompressed.size() ), ( int )text.size() );
	TEST_EQ( memcmp( decompressed.data(), text.data(), text.size() ), 0 );
}

REGISTER_NAMED_TEST( "V_LZ4_compressBound.Limits", V_LZ4_compressBound_Limits )
{
	TEST_TRUE( V_LZ4_compressBound( 100 ) > 100 );

	// Above LZ4_MAX_INPUT_SIZE.
	TEST_EQ( V_LZ4_compressBound( 0x7F000000 ), 0 );
}

REGISTER_NAMED_TEST( "V_LZ4_decompress_safe.CorruptInput", V_LZ4_decompress_safe_CorruptInput )
{
	const char garbage[] = { ( char )0xF0, 0, 0, 0 };
	char out[ 16 ];
	TEST_TRUE( V_LZ4_decompress_safe( garbage, out, sizeof( garbage ), sizeof( out ) ) < 0 );
}

REGISTER_NAMED_TEST( "V_LZ4_createStream.CreateResetFree", V_LZ4_createStream_CreateResetFree )
{
	LZ4_stream_t *pStream = V_LZ4_createStream();
	TEST_NOT_NULL( pStream );
	V_LZ4_resetStream_fast( pStream );
	TEST_EQ( V_LZ4_freeStream( pStream ), 0 );
}

REGISTER_NAMED_TEST( "V_LZ4_compress_fast_continue.StreamBlocks", V_LZ4_compress_fast_continue_StreamBlocks )
{
	const std::vector< char > text = MakeTestText();
	const int nHalf = ( int )text.size() / 2;

	// Two blocks, the second refers to the first.
	LZ4_stream_t *pStream = V_LZ4_createStream();
	std::vector< char > first( V_LZ4_compressBound( nHalf ) ), second( V_LZ4_compressBound( nHalf ) );
	const int nFirst = V_LZ4_compress_fast_continue( pStream, text.data(), first.data(), nHalf, ( int )first.size(), 1 );
	const int nSecond = V_LZ4_compress_fast_continue( pStream, text.data() + nHalf, second.data(), nHalf, ( int )second.size(), 1 );
	TEST_TRUE( nFirst > 0 );
	TEST_TRUE( nSecond > 0 );
	V_LZ4_freeStream( pStream );

	// The decoder keeps the first block for the second.
	LZ4_streamDecode_t decode;
	memset( &decode, 0, sizeof( decode ) );
	std::vector< char > decompressed( text.size() );
	TEST_EQ( V_LZ4_decompress_safe_continue( &decode, first.data(), decompressed.data(), nFirst, nHalf ), nHalf );
	TEST_EQ( V_LZ4_decompress_safe_continue( &decode, second.data(), decompressed.data() + nHalf, nSecond, nHalf ), nHalf );
	TEST_EQ( memcmp( decompressed.data(), text.data(), text.size() ), 0 );
}

REGISTER_NAMED_TEST( "V_LZ4_decompress_safe_continue.SingleBlock", V_LZ4_decompress_safe_continue_SingleBlock )
{
	const char szText[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	char compressed[ 128 ];
	const int nCompressed = V_LZ4_compress_fast( szText, compressed, sizeof( szText ), sizeof( compressed ), 1 );

	LZ4_streamDecode_t decode;
	memset( &decode, 0, sizeof( decode ) );
	char out[ sizeof( szText ) ];
	TEST_EQ( V_LZ4_decompress_safe_continue( &decode, compressed, out, nCompressed, sizeof( out ) ), ( int )sizeof( szText ) );
	TEST_EQ( memcmp( out, szText, sizeof( szText ) ), 0 );
}

REGISTER_NAMED_TEST( "V_LZ4_attach_dictionary.Dictionary", V_LZ4_attach_dictionary_Dictionary )
{
	const std::vector< char > text = MakeTestText();
	const int nDictionary = 128;

	// A stream that saw the dictionary, attached to a working one.
	LZ4_stream_t *pDictionary = V_LZ4_createStream();
	std::vector< char > scratch( V_LZ4_compressBound( nDictionary ) );
	V_LZ4_compress_fast_continue( pDictionary, text.data(), scratch.data(), nDictionary, ( int )scratch.size(), 1 );

	LZ4_stream_t *pWorking = V_LZ4_createStream();
	V_LZ4_attach_dictionary( pWorking, pDictionary );
	V_LZ4_attach_dictionary( pWorking, nullptr );
	V_LZ4_freeStream( pWorking );
	V_LZ4_freeStream( pDictionary );
}

REGISTER_NAMED_TEST( "V_LZ4_decoderRingBufferSize.Sizes", V_LZ4_decoderRingBufferSize_Sizes )
{
	// 64 KB plus 14 bytes plus the block, at least 16.
	TEST_EQ( V_LZ4_decoderRingBufferSize( 1024 ), 65536 + 14 + 1024 );
	TEST_EQ( V_LZ4_decoderRingBufferSize( 1 ), 65536 + 14 + 16 );
	TEST_EQ( V_LZ4_decoderRingBufferSize( 0x7F000001 ), 0 );
}

REGISTER_NAMED_TEST( "V_LZ4_compress_HC.RoundTrip", V_LZ4_compress_HC_RoundTrip )
{
	const std::vector< char > text = MakeTestText();
	std::vector< char > compressed( V_LZ4_compressBound( ( int )text.size() ) );

	// A level of 0 or below is the maximum one.
	const int nCompressed = V_LZ4_compress_HC( text.data(), compressed.data(), ( int )text.size(), ( int )compressed.size(), 0 );
	TEST_TRUE( nCompressed > 0 );

	std::vector< char > decompressed( text.size() );
	TEST_EQ( V_LZ4_decompress_safe( compressed.data(), decompressed.data(), nCompressed, ( int )decompressed.size() ), ( int )text.size() );
	TEST_EQ( memcmp( decompressed.data(), text.data(), text.size() ), 0 );
}

REGISTER_NAMED_TEST( "V_LZ4_createStreamHC.CreateResetFree", V_LZ4_createStreamHC_CreateResetFree )
{
	LZ4_streamHC_t *pStream = V_LZ4_createStreamHC();
	TEST_NOT_NULL( pStream );
	V_LZ4_resetStreamHC_fast( pStream, 9 );
	TEST_EQ( V_LZ4_freeStreamHC( pStream ), 0 );
}

REGISTER_NAMED_TEST( "V_LZ4_compress_HC_continue.StreamBlocks", V_LZ4_compress_HC_continue_StreamBlocks )
{
	const std::vector< char > text = MakeTestText();
	const int nHalf = ( int )text.size() / 2;

	LZ4_streamHC_t *pStream = V_LZ4_createStreamHC();
	V_LZ4_resetStreamHC_fast( pStream, 9 );
	std::vector< char > first( V_LZ4_compressBound( nHalf ) ), second( V_LZ4_compressBound( nHalf ) );
	const int nFirst = V_LZ4_compress_HC_continue( pStream, text.data(), first.data(), nHalf, ( int )first.size() );
	const int nSecond = V_LZ4_compress_HC_continue( pStream, text.data() + nHalf, second.data(), nHalf, ( int )second.size() );
	TEST_TRUE( nFirst > 0 );
	TEST_TRUE( nSecond > 0 );
	V_LZ4_freeStreamHC( pStream );

	LZ4_streamDecode_t decode;
	memset( &decode, 0, sizeof( decode ) );
	std::vector< char > decompressed( text.size() );
	TEST_EQ( V_LZ4_decompress_safe_continue( &decode, first.data(), decompressed.data(), nFirst, nHalf ), nHalf );
	TEST_EQ( V_LZ4_decompress_safe_continue( &decode, second.data(), decompressed.data() + nHalf, nSecond, nHalf ), nHalf );
	TEST_EQ( memcmp( decompressed.data(), text.data(), text.size() ), 0 );
}

REGISTER_NAMED_TEST( "V_LZ4_attach_HC_dictionary.Dictionary", V_LZ4_attach_HC_dictionary_Dictionary )
{
	LZ4_streamHC_t *pDictionary = V_LZ4_createStreamHC();
	LZ4_streamHC_t *pWorking = V_LZ4_createStreamHC();
	V_LZ4_resetStreamHC_fast( pDictionary, 9 );
	V_LZ4_resetStreamHC_fast( pWorking, 9 );
	V_LZ4_attach_HC_dictionary( pWorking, pDictionary );
	V_LZ4_attach_HC_dictionary( pWorking, nullptr );
	V_LZ4_freeStreamHC( pWorking );
	V_LZ4_freeStreamHC( pDictionary );
}

REGISTER_NAMED_TEST( "V_XXH64.KnownValues", V_XXH64_KnownValues )
{
	// The test vector of xxHash for the empty input.
	TEST_EQ( V_XXH64( "", 0, 0 ), ( XXH64_hash_t )0xEF46DB3751D8E999ull );

	// The seed changes the hash.
	TEST_TRUE( V_XXH64( "abc", 3, 0 ) != V_XXH64( "abc", 3, 1 ) );
}

REGISTER_NAMED_TEST( "V_XXH64_createState.CreateFree", V_XXH64_createState_CreateFree )
{
	XXH64_state_t *pState = V_XXH64_createState();
	TEST_NOT_NULL( pState );
	TEST_EQ( ( int )V_XXH64_freeState( pState ), ( int )XXH_OK );
}

REGISTER_NAMED_TEST( "V_XXH64_update.MatchesOneShot", V_XXH64_update_MatchesOneShot )
{
	const std::vector< char > text = MakeTestText();
	XXH64_state_t *pState = V_XXH64_createState();
	TEST_EQ( ( int )V_XXH64_reset( pState, 42 ), ( int )XXH_OK );

	// Pieces of any size give the hash of the whole.
	size_t nOffset = 0;
	for ( size_t nPiece = 1; nOffset < text.size(); nPiece = nPiece * 2 + 1 )
	{
		const size_t nBytes = text.size() - nOffset < nPiece ? text.size() - nOffset : nPiece;
		TEST_EQ( ( int )V_XXH64_update( pState, text.data() + nOffset, nBytes ), ( int )XXH_OK );
		nOffset += nBytes;
	}

	TEST_EQ( V_XXH64_digest( pState ), V_XXH64( text.data(), text.size(), 42 ) );
	V_XXH64_freeState( pState );
}

REGISTER_NAMED_TEST( "V_XXH64_reset.Seed", V_XXH64_reset_Seed )
{
	XXH64_state_t *pState = V_XXH64_createState();
	V_XXH64_reset( pState, 0 );
	TEST_EQ( V_XXH64_digest( pState ), ( XXH64_hash_t )0xEF46DB3751D8E999ull );

	V_XXH64_reset( pState, 7 );
	TEST_EQ( V_XXH64_digest( pState ), V_XXH64( "", 0, 7 ) );
	V_XXH64_freeState( pState );
}

REGISTER_NAMED_TEST( "V_XXH64_digest.KeepsState", V_XXH64_digest_KeepsState )
{
	XXH64_state_t *pState = V_XXH64_createState();
	V_XXH64_reset( pState, 0 );
	V_XXH64_update( pState, "ab", 2 );

	// A digest does not end the stream.
	TEST_EQ( V_XXH64_digest( pState ), V_XXH64( "ab", 2, 0 ) );
	V_XXH64_update( pState, "c", 1 );
	TEST_EQ( V_XXH64_digest( pState ), V_XXH64( "abc", 3, 0 ) );
	V_XXH64_freeState( pState );
}

REGISTER_NAMED_TEST( "V_XXH64_copyState.Continues", V_XXH64_copyState_Continues )
{
	XXH64_state_t *pState = V_XXH64_createState();
	XXH64_state_t *pCopy = V_XXH64_createState();
	V_XXH64_reset( pState, 3 );
	V_XXH64_update( pState, "hello ", 6 );

	// The copy goes on from the same point.
	V_XXH64_copyState( pCopy, pState );
	V_XXH64_update( pCopy, "world", 5 );
	TEST_EQ( V_XXH64_digest( pCopy ), V_XXH64( "hello world", 11, 3 ) );
	TEST_EQ( V_XXH64_digest( pState ), V_XXH64( "hello ", 6, 3 ) );

	V_XXH64_freeState( pCopy );
	V_XXH64_freeState( pState );
}

REGISTER_NAMED_TEST( "V_XXH64_freeState.Null", V_XXH64_freeState_Null )
{
	TEST_EQ( ( int )V_XXH64_freeState( nullptr ), ( int )XXH_OK );
}

REGISTER_NAMED_TEST( "LZ4_streamDecode_t.Layout", LZ4_streamDecode_t_Layout )
{
	TEST_EQ( sizeof( LZ4_streamDecode_t ), ( size_t )32 );
	TEST_EQ( sizeof( ZSTD_inBuffer ), 3 * sizeof( size_t ) );
	TEST_EQ( sizeof( ZSTD_outBuffer ), 3 * sizeof( size_t ) );
}
