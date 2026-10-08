//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Media Foundation movie recording (Windows only)
//
//===========================================================================//

#ifndef IMEDIAFOUNDATION_H
#define IMEDIAFOUNDATION_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/iappsystem.h"

struct BGR888_t;

struct MediaFoundationParams_t
{
	char m_szFileName[ 256 ];

	int m_nAudio;
	int m_nVideo;
	int m_nUnk264;

	// fps = m_nFrameRateNumerator / m_nFrameRateDenominator
	int m_nFrameRateNumerator;
	int m_nFrameRateDenominator;

	int m_nWidth;
	int m_nHeight;

	// Bits per second; 0 derives it from the frame size
	int m_nVideoBitRate;

	int m_nSampleRate;
	int m_nSampleBits;
	int m_nNumChannels;

	// Bits per second; 0 means 20000
	int m_nAudioBitRate;
};
COMPILE_TIME_ASSERT( sizeof( MediaFoundationParams_t ) == 304 );

abstract_class IMediaFoundation : public IAppSystem
{
public:
	virtual bool StartMovie( const MediaFoundationParams_t &params ) = 0;
	virtual void EndMovie() = 0;

	// Frame is converted to YUY2 before being written
	virtual void AppendMovieFrame( const BGR888_t *pRGBData ) = 0;
	virtual void AppendMovieSound( const void *pData, uint32 nBytes ) = 0;
};

#endif // IMEDIAFOUNDATION_H
