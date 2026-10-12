#include "common/assert.h"
#include "common/macros.h"

#include <tier1/generichash.h>
#include <tier1/utltshash.h>

#include <type_traits>

static int FoldTSHashBucket( uint32 nHash, int nBucketMask )
{
	int nFolded = static_cast< int >( nHash );

	if ( nBucketMask <= USHRT_MAX )
	{
		nFolded ^= ( nFolded >> 16 );
	}

	if ( nBucketMask <= UCHAR_MAX )
	{
		nFolded ^= ( nFolded >> 8 );
	}

	return nFolded & nBucketMask;
}

REGISTER_NAMED_TEST( "CUtlTSHashGenericHashMethod.AlternateHash", CUtlTSHashGenericHashMethod_AlternateHash )
{
	// Integer keys are hashed with HashIntAlternate, then folded into the bucket mask.
	const intp nKeys[] = { 0, 1, 42, 0x12345678, 0x7fffffff };
	const int nMasks[] = { 63, 1023, 0x1ffff };

	for ( intp nKey : nKeys )
	{
		for ( int nMask : nMasks )
		{
			TEST_EQ( CUtlTSHashGenericHashMethod< intp >::Hash( nKey, nMask ), FoldTSHashBucket( HashIntAlternate( static_cast< uint32 >( nKey ) ), nMask ) );
		}
	}

	TEST_TRUE( CUtlTSHashGenericHashMethod< intp >::Compare( 7, 7 ) );
	TEST_FALSE( CUtlTSHashGenericHashMethod< intp >::Compare( 7, 8 ) );

	TEST_TRUE( ( std::is_same_v< CUtlTSHashGenericHash< intp >, CUtlTSHashGenericHashMethod< intp > > ) );
	TEST_TRUE( ( std::is_same_v< CUtlTSHashGenericHash<>, CUtlTSHashGenericHashMethod<> > ) );
}

REGISTER_NAMED_TEST( "CUtlTSHash.InsertFind", CUtlTSHash_InsertFind )
{
	CUtlTSHash< int, 64 > hash( 16 );

	bool bDidInsert = false;
	UtlTSHashHandle_t hFirst = hash.Insert( 42, 100, &bDidInsert );

	TEST_TRUE( bDidInsert );
	TEST_TRUE( hFirst != hash.InvalidHandle() );
	TEST_EQ( hash.Insert( 42, 200, &bDidInsert ), hFirst );
	TEST_FALSE( bDidInsert );

	for ( int i = 0; i < 100; ++i )
	{
		hash.Insert( 1000 + i, i );
	}

	TEST_EQ( hash.Count(), 101 );
	TEST_EQ( hash.Element( hash.Find( 42 ) ), 100 );
	TEST_EQ( hash.Element( hash.Find( 1057 ) ), 57 );
	TEST_EQ( hash.GetID( hash.Find( 1057 ) ), static_cast< intp >( 1057 ) );
	TEST_TRUE( hash.Find( 7 ) == hash.InvalidHandle() );

	hash.Commit();
	hash.FindAndRemove( 42 );
	TEST_TRUE( hash.Find( 42 ) == hash.InvalidHandle() );
	TEST_EQ( hash.Count(), 100 );
}
