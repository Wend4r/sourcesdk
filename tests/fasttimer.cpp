#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/fasttimer.h>

REGISTER_NAMED_TEST( "CCycleCount.Conversions", CCycleCount_Conversions )
{
	// Conversions go through the tick frequency, so one second of ticks reads back exactly.
	const uint64 nFrequency = GetCycleCountFrequency();

	TEST_TRUE( nFrequency > 0 );

	CCycleCount count( nFrequency );

	TEST_TRUE( count.GetSeconds() == 1.0 );
	TEST_TRUE( count.GetMillisecondsF() == 1000.0 );
	TEST_TRUE( count.GetMicrosecondsF() == 1000000.0 );
	TEST_EQ( count.GetMilliseconds(), 1000ul );
	TEST_EQ( count.GetMicroseconds(), 1000000ul );
	TEST_EQ( count.GetUlMicroseconds(), static_cast< uint64 >( 1000000 ) );
	TEST_EQ( CFastTimer::GetClockSpeed(), static_cast< unsigned long >( nFrequency ) );

	count.SetMicroseconds( 2000000 );
	TEST_EQ( count.GetLongCycles(), nFrequency * 2 );

	count.Init( 500.0 );
	TEST_EQ( count.GetLongCycles(), nFrequency / 2 );
}

REGISTER_NAMED_TEST( "CLimitTimer.Limits", CLimitTimer_Limits )
{
	CLimitTimer expired( 0 );

	TEST_TRUE( expired.BLimitReached() );
	TEST_EQ( expired.CMicroSecLeft(), static_cast< uint64 >( 0 ) );

	CLimitTimer pending( static_cast< uint64 >( 60 ) * 1000000 );

	TEST_FALSE( pending.BLimitReached() );
	TEST_EQ( pending.CMicroSecOverage(), 0 );
	TEST_TRUE( pending.CMicroSecLeft() > 0 );
}
