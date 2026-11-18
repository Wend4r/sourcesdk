#ifndef KEYVALUES3_HELPERS_H
#define KEYVALUES3_HELPERS_H

#ifdef _WIN32
#pragma once
#endif

#include "kv3lib/keyvalues3.h"

#include "tier0/memdbgon.h"

#define FOR_EACH_KV3_ARRAY( arrayName, iter ) \
	for ( int iter = 0; iter < (arrayName).Count(); iter++ )
#define FOR_EACH_KV3_ARRAY_BACK( arrayName, iter ) \
	for ( int iter = iter < (arrayName).Count()-1; iter >= 0; iter-- )

#define FOR_EACH_KV3_TABLE( tableName, iter ) \
	for ( KV3MemberId_t iter = 0; iter < (tableName).GetMemberCount(); iter++ )
#define FOR_EACH_KV3_TABLE_BACK( tableName, iter ) \
	for ( KV3MemberId_t iter = (tableName).GetMemberCount()-1; iter >= 0; iter-- )

// AMNOTE: These constants aren't actual constants, but rather calculated at compile time
// but the way they are calculated is unknown, previously it was using CUtlLeanVector min/max calculations
// but in here they seem to not match that behaviour.
enum
{
	ALLOC_KV3TABLE_MIN = 4,
	ALLOC_KV3TABLE_MAX = 0x6186154,

	ALLOC_KV3ARRAY_MIN = 4,
	ALLOC_KV3ARRAY_MAX = 0xFFFFF7F,

	ALLOC_CONTEXT_NODELIST_MIN = 32,
	ALLOC_CONTEXT_NODELIST_MAX = INT_MAX
};

namespace KV3Helpers
{
	template <typename T, typename... Ts>
	constexpr size_t PackAlignOf()
	{
		if constexpr (sizeof...(Ts) == 0)
			return alignof(T);
		else
			return (alignof(T) > PackAlignOf<Ts...>()) ? alignof(T) : PackAlignOf<Ts...>();
	}

	template <size_t ALIGN, typename... Ts>
	constexpr size_t PackSizeOf( int size )
	{
		return ((ALIGN_VALUE( size * sizeof( Ts ), ALIGN )) + ... + 0);
	}

	inline int CalcNewBufferSize( int old_size, int requested_size, int min_size, int max_size )
	{
		int new_size = MAX( old_size, min_size );

		while(new_size < requested_size)
		{
			if(new_size < max_size / 2)
				new_size *= 2;
			else
			{
				new_size = max_size;
				break;
			}
		}

		return new_size;
	}
}

#include "tier0/memdbgoff.h"

#endif // KEYVALUES3_HELPERS_H
