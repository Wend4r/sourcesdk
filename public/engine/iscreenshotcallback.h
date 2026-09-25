//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Receiver of screenshots captured by the engine screenshot service
//
//===========================================================================//

#ifndef ISCREENSHOTCALLBACK_H
#define ISCREENSHOTCALLBACK_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "bitmap/imageformat.h"

//-----------------------------------------------------------------------------
// Purpose: Receives the images of screenshot requests, see ISource2Engine::RequestScreenshot().
// pContext is the pointer the request was queued with.
//
// The On*Screenshot methods get a buffer allocated with g_pMemAlloc. Returning true
// keeps it (the callback frees it later), false lets the engine free it after the call.
//-----------------------------------------------------------------------------
abstract_class IScreenshotCallback
{
public:
	// Uncompressed pixels, nSize bytes in total.
	virtual bool OnRawScreenshot( int nWidth, int nHeight, ImageFormat format, void *pPixels, int nSize, void *pContext ) = 0;

	// Encoded image files of nSize bytes.
	virtual bool OnJpegScreenshot( int nWidth, int nHeight, void *pData, int nSize, void *pContext ) = 0;
	virtual bool OnPngScreenshot( int nWidth, int nHeight, void *pData, int nSize, void *pContext ) = 0;

	// Asynchronous readback path: gets the GPU readback rows directly (nPitch bytes apart), before the engine copies them.
	// Return true to take them over, then IsReadbackFinished() is polled with the same context until it returns true.
	// Return false to let the engine copy the pixels and continue with OnRawScreenshot().
	virtual bool OnScreenshotReadback( int nWidth, int nHeight, ImageFormat format, const void *pPixels, int nPitch, void *pContext ) = 0;
	virtual bool IsReadbackFinished( void *pContext ) = 0;
};

#endif // ISCREENSHOTCALLBACK_H
