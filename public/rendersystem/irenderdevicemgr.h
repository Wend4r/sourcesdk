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
#include "tier0/utlvector.h"

class KeyValues;
class IRenderDeviceSetup;
class IRenderDeviceEventListener;
struct RenderAdapterInfo_t;
struct RenderDisplayMode_t;
struct RenderDeviceInfo_t;

abstract_class IRenderDeviceMgr : public IAppSystem
{
public:
	virtual int unk011() = 0;

	virtual int GetAdapterCount() = 0;

	virtual void GetAdapterInfo( int nAdapter, RenderAdapterInfo_t &info ) = 0;

	virtual int FindAdapterForRect( int x, int y, int nWidth, int nHeight ) = 0;

	// Display modes of an output of an adapter
	virtual int GetModeCount( int nAdapter, int nOutput ) = 0;
	virtual void GetModeInfo( RenderDisplayMode_t *pMode, int nAdapter, int nOutput, int nMode ) = 0;
	virtual void GetCurrentModeInfo( RenderDisplayMode_t *pMode, int nAdapter, int nOutput ) = 0;

	virtual CreateInterfaceFn CreateDevice( int nAdapter, int nFlags, int nDXLevel ) = 0;

	virtual void AddModeChangeCallback( void ( *pfnCallback )() ) = 0;
	virtual void RemoveModeChangeCallback( void ( *pfnCallback )() ) = 0;

	// Shuts the render device down and forgets it
	virtual void DestroyDevice() = 0;

	virtual void InstallRenderDeviceSetup( IRenderDeviceSetup *pSetup ) = 0;

	virtual void AddDeviceEventListener( IRenderDeviceEventListener *pListener ) = 0;
	virtual void RemoveDeviceEventListener( IRenderDeviceEventListener *pListener ) = 0;

	// The video.cfg settings in use
	virtual KeyValues *GetVideoConfig() = 0;

	virtual KeyValues *unk026() = 0;

	virtual int GetMonitorIndex() = 0;

	virtual void InvokeModeChangeCallbacks( const RenderDeviceInfo_t &mode ) = 0;

	// Writes the video config back to the user settings
	virtual void SaveVideoConfig() = 0;

	virtual void ResetVideoConfigToDefaults() = 0;

	// Fills a CUtlVector of display modes of an adapter output
	virtual void GetAllDisplayModes( CUtlVector< RenderDisplayMode_t > &listOut, int nAdapter, int nOutput, bool bCommonWindowed ) = 0;

	virtual uint32 GetPlatWindowFlags() = 0;

	virtual bool GetVideoMemoryInfo( int nAdapter, uint64 *pUnk1, uint64 *pUnk2, uint64 *pUnk3 ) = 0;

	virtual void AppendMinidumpInfo( void *pComment, uint32 nUnk ) = 0;

	// Highest DX level the device supports (110, 111, 120)
	virtual int GetMaxDXLevel() = 0;
};

#endif // IRENDERDEVICEMGR_H
