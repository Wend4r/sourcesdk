//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: Enumerates graphics adapters and display modes
//
//===========================================================================//

#ifndef IRENDERDEVICEMGR_H
#define IRENDERDEVICEMGR_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"

class KeyValues;

abstract_class IRenderDeviceMgr : public IAppSystem
{
public:
	virtual int unk011() = 0;

	virtual int GetAdapterCount() = 0;

	virtual void GetAdapterInfo( int nAdapter, void *pInfo ) = 0;

	virtual int FindAdapterForRect( int x, int y, int nWidth, int nHeight ) = 0;

	// Display modes of an output of an adapter
	virtual int GetModeCount( int nAdapter, int nOutput ) = 0;
	virtual void GetModeInfo( void *pMode, int nAdapter, int nOutput, int nMode ) = 0;
	virtual void GetCurrentModeInfo( void *pMode, int nAdapter, int nOutput ) = 0;

	virtual CreateInterfaceFn CreateDevice( int nAdapter, int nFlags, int nDXLevel ) = 0;

	// Adds/removes a function that unk028 calls
	virtual void unk019( void ( *pfnCallback )() ) = 0;
	virtual void unk020( void ( *pfnCallback )() ) = 0;

	// Shuts the render device down and forgets it
	virtual void DestroyDevice() = 0;

	virtual void unk022( void *p ) = 0;

	virtual void unk023( void *pListener ) = 0;
	virtual void unk024( void *pListener ) = 0;

	// The video.cfg settings in use
	virtual KeyValues *GetVideoConfig() = 0;

	virtual KeyValues *unk026() = 0;

	virtual int GetMonitorIndex() = 0;

	virtual void unk028( void *pData ) = 0;

	// Writes the video config back to the user settings
	virtual void SaveVideoConfig() = 0;

	virtual void ResetVideoConfigToDefaults() = 0;

	// Fills a CUtlVector of display modes of an adapter output
	virtual void Unk_GetModeList( void *p ) = 0;

	virtual int unk032() = 0;

	virtual bool GetVideoMemoryInfo( int nAdapter, uint64 *pUnk1, uint64 *pUnk2, uint64 *pUnk3 ) = 0;

	virtual void AppendMinidumpInfo( void *pComment, uint32 nUnk ) = 0;

	// Highest DX level the device supports (110, 111, 120)
	virtual int GetMaxDXLevel() = 0;
};

#endif // IRENDERDEVICEMGR_H
