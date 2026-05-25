#include "common/assert.h"
#include "common/macros.h"

#include <tier0/utlbuffer.h>

#include <cmath>
#include <cstring>

REGISTER_NAMED_TEST( "CUtlBuffer.BinaryReadWrite", CUtlBuffer_BinaryReadWrite )
{
	// Binary buffers should preserve primitive writes in raw storage order.
	CUtlBuffer buffer( 0, 0, CUtlBuffer::NONE );

	buffer.PutInt( 1234 );
	buffer.PutFloat( 12.5f );
	buffer.PutString( "value" );

	TEST_TRUE( buffer.TellPut() > 0 );
	TEST_TRUE( buffer.Size() >= buffer.TellPut() );

	const unsigned char *pBase = ( const unsigned char * )buffer.Base();
	int nValue = 0;
	float flValue = 0.0f;

	memcpy( &nValue, pBase, sizeof( nValue ) );
	memcpy( &flValue, pBase + sizeof( nValue ), sizeof( flValue ) );
	const char *pString = ( const char * )( pBase + sizeof( nValue ) + sizeof( flValue ) );

	TEST_EQ( nValue, 1234 );
	TEST_TRUE( std::fabs( flValue - 12.5f ) < 0.001f );
	TEST_EQ( V_strcmp( pString, "value" ), 0 );
}

REGISTER_NAMED_TEST( "CUtlBuffer.SeekClear", CUtlBuffer_SeekClear )
{
	// Seek and clear operations should update the get and put cursors predictably.
	CUtlBuffer buffer( 0, 0, CUtlBuffer::NONE );

	buffer.PutInt( 1 );
	buffer.PutInt( 2 );

	TEST_EQ( buffer.TellPut(), ( int )( sizeof( int ) * 2 ) );

	buffer.SeekGet( CUtlBuffer::SEEK_HEAD, sizeof( int ) );
	TEST_EQ( buffer.TellGet(), ( int )sizeof( int ) );

	buffer.SeekPut( CUtlBuffer::SEEK_HEAD, 0 );
	buffer.PutInt( 3 );
	int nValue = 0;

	memcpy( &nValue, buffer.Base(), sizeof( nValue ) );
	TEST_EQ( nValue, 3 );

	buffer.Clear();
	TEST_EQ( buffer.TellGet(), 0 );
	TEST_EQ( buffer.TellPut(), 0 );
}

REGISTER_NAMED_TEST( "CUtlBuffer.TextMode", CUtlBuffer_TextMode )
{
	// Text-mode buffers should still accept string writes and report text state.
	CUtlBuffer buffer( 0, 0, CUtlBuffer::TEXT_BUFFER );

	TEST_TRUE( buffer.IsText() );
	buffer.PutString( "text" );
	TEST_TRUE( buffer.TellPut() > 0 );
}

REGISTER_NAMED_TEST( "CUtlBuffer.CapacitySwapCopyAndPurge", CUtlBuffer_CapacitySwapCopyAndPurge )
{
	// Capacity changes, copy-style swaps and purge should leave both buffers usable.
	CUtlBuffer left( 0, 0, CUtlBuffer::NONE );

	left.EnsureCapacity( 32 );
	left.PutInt( 77 );

	CUtlBuffer right( 0, 0, CUtlBuffer::TEXT_BUFFER );

	right.PutString( "right" );

	left.SwapCopy( right );
	TEST_TRUE( left.IsText() );
	TEST_TRUE( left.TellPut() > 0 );
	TEST_TRUE( right.TellPut() > 0 );

	right.Purge();
	TEST_EQ( right.TellPut(), 0 );
	TEST_EQ( right.Size(), 0 );
}

REGISTER_NAMED_TEST( "CUtlBuffer.CopyAndMove", CUtlBuffer_CopyAndMove )
{
	// Copy and move paths should keep the readable payload intact.
	CUtlBuffer source( 0, 0, CUtlBuffer::NONE );

	source.PutString( "copy-data" );

	CUtlBuffer copied( 0, 0, CUtlBuffer::NONE );

	copied.CopyBuffer( source );
	TEST_EQ( copied.TellPut(), source.TellPut() );
	TEST_EQ( V_strcmp( ( const char * )copied.Base(), "copy-data" ), 0 );

	CUtlBuffer moved( Move( copied ) );

	TEST_EQ( V_strcmp( ( const char * )moved.Base(), "copy-data" ), 0 );

	CUtlBuffer assigned( 0, 0, CUtlBuffer::NONE );

	assigned = Move( moved );
	TEST_EQ( V_strcmp( ( const char * )assigned.Base(), "copy-data" ), 0 );
}
