#ifndef TIER0_JSONPARSER_H
#define TIER0_JSONPARSER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlbuffer.h"

//-----------------------------------------------------------------------------
// The JSON_parser (JSON_checker) of tier0: a push parser that reports every value
// to a callback, configured by V_InitJSONConfig
//-----------------------------------------------------------------------------
typedef enum
{
	JSON_T_NONE = 0,
	JSON_T_ARRAY_BEGIN,
	JSON_T_ARRAY_END,
	JSON_T_OBJECT_BEGIN,
	JSON_T_OBJECT_END,
	JSON_T_INTEGER,
	JSON_T_FLOAT,
	JSON_T_NULL,
	JSON_T_TRUE,
	JSON_T_FALSE,
	JSON_T_STRING,
	JSON_T_KEY,
	JSON_T_MAX
} JSON_type;

// Integers are read with "%lld"
typedef long long JSON_int_t;

typedef struct JSON_value_struct
{
	union
	{
		JSON_int_t integer_value;
		double float_value;

		struct
		{
			const char *value;
			size_t length;
		} str;
	} vu;
} JSON_value;

// Returns 0 to stop the parsing
typedef int (*JSON_parser_callback)( void *ctx, int type, const JSON_value *value );

typedef void *(*JSON_malloc_t)( size_t n );
typedef void (*JSON_free_t)( void *mem );

typedef struct
{
	JSON_parser_callback callback;
	void *callback_ctx;

	// The nesting limit, 127 by default
	int depth;

	int allow_comments;
	int handle_floats_manually;

	// MemAlloc_AllocFunc and MemAlloc_FreeFunc by default
	JSON_malloc_t malloc;
	JSON_free_t free;
} JSON_config;

// Sets the defaults
PLATFORM_INTERFACE void V_InitJSONConfig( JSON_config *pConfig );

// Feeds the text of pBuffer up to its end or a null character to a parser with pConfig (the
// defaults when null), returns whether it is a complete JSON document
PLATFORM_INTERFACE bool V_ParseJSONBuffer( const JSON_config *pConfig, CUtlBuffer *pBuffer );

// The defaults with a callback
inline void V_InitJSONConfig( JSON_config *pConfig, JSON_parser_callback pfnCallback, void *pContext )
{
	V_InitJSONConfig( pConfig );
	pConfig->callback = pfnCallback;
	pConfig->callback_ctx = pContext;
}

// Same as V_ParseJSONBuffer for a null-terminated text
inline bool V_ParseJSONString( const JSON_config *pConfig, const char *pszText )
{
	CUtlBuffer buffer( 0, 0, CUtlBuffer::TEXT_BUFFER );
	buffer.PutString( pszText );
	return V_ParseJSONBuffer( pConfig, &buffer );
}

#endif // TIER0_JSONPARSER_H
