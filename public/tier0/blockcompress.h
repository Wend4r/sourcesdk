#ifndef BLOCKCOMPRESS_H
#define BLOCKCOMPRESS_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CBlockCompress
{
public:
	DLL_CLASS_IMPORT static uint32 FastCompress( const uint8 *pSrcBuf, uint32 nSrcBufSize, uint8 *pDstBuf, uint32 nDstBufSize );
	DLL_CLASS_IMPORT static uint32 FastDecompress( const uint8 *pSrcBuf, uint32 nSrcBufSize, uint8 *pDstBuf, uint32 nDstBufSize );
};

#endif // BLOCKCOMPRESS_H
