#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/jsonparser.h>
#include <tier0/memalloc.h>
#include <tier0/strtools.h>

#include <cstddef>
#include <cstring>

// The values the parser reports, in order
struct TestJSONEvents_t
{
	int m_nTypes[ 32 ];
	JSON_int_t m_nLastInteger;
	double m_flLastFloat;
	char m_szLastString[ 64 ];
	int m_nCount;
};

static int TestJSONCallback( void *pContext, int nType, const JSON_value *pValue )
{
	TestJSONEvents_t *pEvents = static_cast< TestJSONEvents_t * >( pContext );

	if ( pEvents->m_nCount < 32 )
		pEvents->m_nTypes[ pEvents->m_nCount ] = nType;

	pEvents->m_nCount++;

	if ( nType == JSON_T_INTEGER )
		pEvents->m_nLastInteger = pValue->vu.integer_value;
	else if ( nType == JSON_T_FLOAT )
		pEvents->m_flLastFloat = pValue->vu.float_value;
	else if ( nType == JSON_T_STRING || nType == JSON_T_KEY )
		V_strncpy( pEvents->m_szLastString, pValue->vu.str.value, sizeof( pEvents->m_szLastString ) );

	return 1;
}

REGISTER_NAMED_TEST( "V_InitJSONConfig.Defaults", V_InitJSONConfig_Defaults )
{
	JSON_config config;
	memset( &config, 0xff, sizeof( config ) );
	V_InitJSONConfig( &config );

	TEST_NULL( config.callback );
	TEST_NULL( config.callback_ctx );
	TEST_EQ( config.depth, 127 );
	TEST_EQ( config.allow_comments, 0 );
	TEST_EQ( config.handle_floats_manually, 0 );
	TEST_TRUE( config.malloc == ( JSON_malloc_t )MemAlloc_AllocFunc );
	TEST_TRUE( config.free == ( JSON_free_t )MemAlloc_FreeFunc );
}

REGISTER_NAMED_TEST( "V_ParseJSONBuffer.ValidAndInvalid", V_ParseJSONBuffer_ValidAndInvalid )
{
	// Null takes the defaults.
	TEST_TRUE( V_ParseJSONString( nullptr, "{ \"a\": [ 1, 2.5, \"x\", true, false, null ] }" ) );
	TEST_FALSE( V_ParseJSONString( nullptr, "{ \"a\": [ 1, 2 }" ) );
	TEST_FALSE( V_ParseJSONString( nullptr, "{ \"a\": 1" ) );

	// A complete value of any kind is a document.
	TEST_TRUE( V_ParseJSONString( nullptr, "[]" ) );
}

REGISTER_NAMED_TEST( "V_ParseJSONBuffer.Callback", V_ParseJSONBuffer_Callback )
{
	TestJSONEvents_t events;
	memset( &events, 0, sizeof( events ) );

	JSON_config config;
	V_InitJSONConfig( &config, TestJSONCallback, &events );
	TEST_EQ( config.depth, 127 );

	TEST_TRUE( V_ParseJSONString( &config, "{\"key\":[42,1.5,\"text\"]}" ) );

	const int nExpected[] = { JSON_T_OBJECT_BEGIN, JSON_T_KEY, JSON_T_ARRAY_BEGIN, JSON_T_INTEGER, JSON_T_FLOAT, JSON_T_STRING, JSON_T_ARRAY_END, JSON_T_OBJECT_END };
	TEST_EQ( events.m_nCount, 8 );

	for ( int i = 0; i < 8; i++ )
		TEST_EQ( events.m_nTypes[ i ], nExpected[ i ] );

	TEST_EQ( events.m_nLastInteger, ( JSON_int_t )42 );
	TEST_EQ( events.m_flLastFloat, 1.5 );
	TEST_EQ( V_strcmp( events.m_szLastString, "text" ), 0 );
}

REGISTER_NAMED_TEST( "V_ParseJSONBuffer.Depth", V_ParseJSONBuffer_Depth )
{
	JSON_config config;
	V_InitJSONConfig( &config );
	config.depth = 2;

	TEST_TRUE( V_ParseJSONString( &config, "[[1]]" ) );
	TEST_FALSE( V_ParseJSONString( &config, "[[[1]]]" ) );
}

REGISTER_NAMED_TEST( "V_ParseJSONBuffer.Comments", V_ParseJSONBuffer_Comments )
{
	JSON_config config;
	V_InitJSONConfig( &config );
	TEST_FALSE( V_ParseJSONString( &config, "[1 /* note */]" ) );

	config.allow_comments = 1;
	TEST_TRUE( V_ParseJSONString( &config, "[1 /* note */]" ) );
}

REGISTER_NAMED_TEST( "JSON_config.Layout", JSON_config_Layout )
{
	TEST_EQ( sizeof( JSON_config ), ( size_t )48 );
	TEST_EQ( offsetof( JSON_config, depth ), ( size_t )16 );
	TEST_EQ( offsetof( JSON_config, malloc ), ( size_t )32 );
	TEST_EQ( sizeof( JSON_value ), ( size_t )16 );
	TEST_EQ( sizeof( JSON_int_t ), ( size_t )8 );
	TEST_EQ( ( int )JSON_T_KEY, 11 );
	TEST_EQ( ( int )JSON_T_MAX, 12 );
}
