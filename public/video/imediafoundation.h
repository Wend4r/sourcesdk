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

// Field names from the asserts of valve_wmf.dll ( "params.m_nWidth", ... ) where it has them
struct MediaFoundationParams_t
{
	// The extension is forced to ".mp4" ( video ) or ".m4a" ( audio only )
	char m_szFileName[ 256 ];

	// Named by what they select: 1 records an AAC audio stream / an H.264 video stream
	int m_nAudioCodec;
	int m_nVideoCodec;

	// Named by what it holds: engine2 writes 1 next to the codec selectors; valve_wmf.dll never reads it
	int m_nOutputFormat;

	// fps = m_nVideoFrameRate / m_nVideoFrameScale
	int m_nVideoFrameRate;
	int m_nVideoFrameScale;

	int m_nWidth;
	int m_nHeight;

	// 0 derives it from the frame size
	int m_nVideoBitsPerSec;

	int m_nAudioSampleRate;
	int m_nAudioSampleBits;
	int m_nAudioNumChannels;

	// 0 means 20000
	int m_nAudioBytesPerSec;
};

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
