#ifndef KEYVALUES3_BINARYBLOB_H
#define KEYVALUES3_BINARYBLOB_H

#ifdef _WIN32
#pragma once
#endif

#include "kv3lib/keyvalues3.h"

#include "tier0/memdbgon.h"

struct KV3BinaryBlob_t
{
	size_t m_nSize;
	union
	{
		const byte*	m_pubData;
		byte		m_ubData[1];
	};
	bool m_bFreeMemory;
};

#include "tier0/memdbgoff.h"

#endif // KEYVALUES3_BINARYBLOB_H
