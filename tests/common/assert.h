#ifndef SOURCESDK_TESTS_COMMON_ASSERT_H
#define SOURCESDK_TESTS_COMMON_ASSERT_H

#include "runner.h"

#include <stdio.h>

template < typename T > inline const char *TestFormatValue( const T &, char *pBuffer, size_t nBufferSize )
{
	snprintf( pBuffer, nBufferSize, "<value>" );
	return pBuffer;
}

inline const char *TestFormatValue( bool bValue, char *pBuffer, size_t nBufferSize )
{
	snprintf( pBuffer, nBufferSize, "%s", bValue ? "true" : "false" );
	return pBuffer;
}

inline const char *TestFormatValue( int nValue, char *pBuffer, size_t nBufferSize )
{
	snprintf( pBuffer, nBufferSize, "%d", nValue );
	return pBuffer;
}

inline const char *TestFormatValue( unsigned int nValue, char *pBuffer, size_t nBufferSize )
{
	snprintf( pBuffer, nBufferSize, "%u", nValue );
	return pBuffer;
}

inline const char *TestFormatValue( long nValue, char *pBuffer, size_t nBufferSize )
{
	snprintf( pBuffer, nBufferSize, "%ld", nValue );
	return pBuffer;
}

inline const char *TestFormatValue( unsigned long nValue, char *pBuffer, size_t nBufferSize )
{
	snprintf( pBuffer, nBufferSize, "%lu", nValue );
	return pBuffer;
}

inline const char *TestFormatValue( long long nValue, char *pBuffer, size_t nBufferSize )
{
	snprintf( pBuffer, nBufferSize, "%lld", nValue );
	return pBuffer;
}

inline const char *TestFormatValue( unsigned long long nValue, char *pBuffer, size_t nBufferSize )
{
	snprintf( pBuffer, nBufferSize, "%llu", nValue );
	return pBuffer;
}

inline const char *TestFormatValue( float flValue, char *pBuffer, size_t nBufferSize )
{
	snprintf( pBuffer, nBufferSize, "%g", ( double )flValue );
	return pBuffer;
}

inline const char *TestFormatValue( double flValue, char *pBuffer, size_t nBufferSize )
{
	snprintf( pBuffer, nBufferSize, "%g", flValue );
	return pBuffer;
}

inline const char *TestFormatValue( const char *pValue, char *, size_t )
{
	return pValue ? pValue : "<null>";
}

inline const char *TestFormatValue( char *pValue, char *, size_t )
{
	return pValue ? pValue : "<null>";
}

template < typename T > inline const char *TestFormatValue( T *pValue, char *pBuffer, size_t nBufferSize )
{
	snprintf( pBuffer, nBufferSize, "%p", ( void * )pValue );
	return pBuffer;
}

#define TEST_FAIL_( expression, expected, actual ) \
	do \
	{ \
		ReportTestFailure( __FILE__, __LINE__, expression, expected, actual ); \
		throw TestFailure(); \
	} while ( 0 )

#define TEST_TRUE( expression ) \
	do \
	{ \
		if ( !( expression ) ) \
		{ \
			TEST_FAIL_( #expression, nullptr, nullptr ); \
		} \
	} while ( 0 )

#define TEST_FALSE( expression ) TEST_TRUE( !( expression ) )

#define TEST_EQ( lhs, rhs ) \
	do \
	{ \
		const auto &testLhs = ( lhs ); \
		const auto &testRhs = ( rhs ); \
		if ( !( testLhs == testRhs ) ) \
		{ \
			char lhsString[64]; \
			char rhsString[64]; \
			TEST_FAIL_( #lhs " == " #rhs, TestFormatValue( testLhs, lhsString, sizeof( lhsString ) ), TestFormatValue( testRhs, rhsString, sizeof( rhsString ) ) ); \
		} \
	} while ( 0 )

#define TEST_NE( lhs, rhs ) \
	do \
	{ \
		const auto &testLhs = ( lhs ); \
		const auto &testRhs = ( rhs ); \
		if ( !( testLhs != testRhs ) ) \
		{ \
			char lhsString[64]; \
			char rhsString[64]; \
			TEST_FAIL_( #lhs " != " #rhs, TestFormatValue( testLhs, lhsString, sizeof( lhsString ) ), TestFormatValue( testRhs, rhsString, sizeof( rhsString ) ) ); \
		} \
	} while ( 0 )

#define TEST_NULL( ptr ) TEST_EQ( ( ptr ), nullptr )
#define TEST_NOT_NULL( ptr ) TEST_NE( ( ptr ), nullptr )

#endif // SOURCESDK_TESTS_COMMON_ASSERT_H
