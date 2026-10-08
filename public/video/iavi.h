//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AVI recording and AVI-backed materials (Windows only)
//
//===========================================================================//

#ifndef IAVI_H
#define IAVI_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/iappsystem.h"
#include "materialsystem2/imaterial2.h"

struct BGR888_t;
class IRenderContext;

struct AVIParams_t
{
	// The ".avi" extension is forced onto the file name
	char m_pFileName[ 256 ];
	char m_pPathID[ 256 ];

	// fps = m_nFrameRate / m_nFrameScale
	int m_nFrameRate;
	int m_nFrameScale;

	int m_nWidth;
	int m_nHeight;

	// Sound/.wav info
	int m_nSampleRate;
	int m_nSampleBits;
	int m_nNumChannels;

	bool m_bGetCodecFromUser;
};

typedef unsigned short AVIHandle_t;
enum
{
	AVIHANDLE_INVALID = ( AVIHandle_t )~0
};

typedef unsigned short AVIMaterial_t;
enum
{
	AVIMATERIAL_INVALID = ( AVIMaterial_t )~0
};

abstract_class IAvi : public IAppSystem
{
public:
	virtual void SetMainWindow( void *hWnd ) = 0;

	// Start/stop recording an AVI
	virtual AVIHandle_t StartAVI( const AVIParams_t &params ) = 0;
	virtual void FinishAVI( AVIHandle_t handle ) = 0;

	virtual void AppendMovieSound( AVIHandle_t h, short *buf, size_t bufsize ) = 0;
	virtual void AppendMovieFrame( AVIHandle_t h, const BGR888_t *pRGBData ) = 0;

	virtual AVIMaterial_t CreateAVIMaterial( const char *pMaterialName, const char *pFileName, const char *pPathID ) = 0;
	virtual void DestroyAVIMaterial( AVIMaterial_t hMaterial ) = 0;

	virtual HMaterialWeak SetTime( AVIMaterial_t hMaterial, float flTime, IRenderContext *pRenderContext, void *pUnk ) = 0;

	virtual HMaterialWeak GetMaterial( AVIMaterial_t hMaterial ) = 0;

	virtual void GetTexCoordRange( AVIMaterial_t hMaterial, float *pMaxU, float *pMaxV ) = 0;

	virtual void GetFrameSize( AVIMaterial_t hMaterial, int *pWidth, int *pHeight ) = 0;

	virtual void GetTextureSize( AVIMaterial_t hMaterial, int *pWidth, int *pHeight ) = 0;

	virtual int GetFrameRate( AVIMaterial_t hMaterial ) = 0;
	virtual int GetFrameCount( AVIMaterial_t hMaterial ) = 0;

	// Same as SetTime, but takes a frame index instead of seconds
	virtual HMaterialWeak SetFrame( AVIMaterial_t hMaterial, float flFrame, IRenderContext *pRenderContext, void *pUnk ) = 0;

	virtual bool IsFinished( AVIMaterial_t hMaterial ) = 0;
};

#endif // IAVI_H
