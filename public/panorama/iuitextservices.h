//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Text layout and font services used by the panorama UI engine
//
//===========================================================================//

#ifndef IUITEXTSERVICES_H
#define IUITEXTSERVICES_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlstring.h"
#include "appframework/iappsystem.h"

namespace panorama
{

class IUITextLayout;
class IUITextTextureCache;
class IUITextLayoutDrawCache;
class IUITextTextureProvider;
class IUITextTextureStorage;

abstract_class IUITextServices : public IAppSystem
{
public:
	virtual ~IUITextServices() {}

	virtual void InitializeTextLayout() = 0;
	virtual void ShutdownTextLayout() = 0;

	virtual bool RegisterCustomFontPath( const char *pszSearchPath, const char *pszPath ) = 0;

	virtual bool RegisterCustomFontFile( const char *pszFullPath, const char *pszFileName ) = 0;

	// nTextBytes includes the terminator, nEncoding is 1 for UTF-8, 2 for UTF-16 and 3 for UTF-32.
	// pFontMetrics, when not NULL, receives the font ascent, descent, line height and maximum advance width (floats),
	// then a fixed-width flag and a flag set when the face metrics were read
	virtual IUITextLayout *CreateTextLayout( const void *pText, int nTextBytes, int nCharCount, uint8 nEncoding, const void *pLayoutParams, void *pFontMetrics ) = 0;
	virtual void FreeTextLayout( IUITextLayout *pLayout ) = 0;

	virtual const CUtlVector< CUtlString > &GetSortedValidFontNames() = 0;

	virtual IUITextTextureCache *CreateTextTextureCache( IUITextTextureProvider *pProvider ) = 0;
	virtual void DestroyTextTextureCache( IUITextTextureCache *pCache ) = 0;

	virtual IUITextLayoutDrawCache *CreateTextLayoutDrawCache( IUITextTextureStorage *pStorage ) = 0;
	virtual void DestroyTextLayoutDrawCache( IUITextLayoutDrawCache *pCache ) = 0;

	virtual void SetJapaneseLineBreaking( bool bEnabled ) = 0;
};

} // namespace panorama

#endif // IUITEXTSERVICES_H
