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
#include "tier1/utlvector.h"

class KeyValues;
class CMiniDumpComment;
class IRenderDeviceSetup;
class IRenderDeviceEventListener;
struct RenderAdapterInfo_t;
struct RenderDisplayMode_t;
struct RenderDeviceInfo_t;

typedef void ( *RenderModeChangeCallbackFunc_t )();

abstract_class IRenderDeviceMgr : public IAppSystem
{
public:
	// The device API of the manager (Vulkan returns 1, the value of RENDER_DEVICE_API_VULKAN); a manager
	// that wraps another one forwards the call (named by what it does)
	virtual int GetRenderDeviceAPI() = 0;

	virtual int GetAdapterCount() = 0;
	virtual void GetAdapterInfo( int nAdapter, RenderAdapterInfo_t &info ) = 0;
	virtual int FindAdapterForDesktopRect( int x, int y, int nWidth, int nHeight ) = 0;

	// Display modes of an output of an adapter
	virtual int GetModeCount( int nAdapter, int nOutput ) = 0;
	virtual void GetModeInfo( RenderDisplayMode_t *pMode, int nAdapter, int nOutput, int nMode ) = 0;
	virtual void GetCurrentModeInfo( RenderDisplayMode_t *pMode, int nAdapter, int nOutput ) = 0;

	virtual CreateInterfaceFn CreateDevice( int nAdapter, int nFlags, int nDXLevel ) = 0;

	// Functions InvokeModeChangeCallbacks calls
	virtual void AddModeChangeCallback( RenderModeChangeCallbackFunc_t pfnCallback ) = 0;
	virtual void RemoveModeChangeCallback( RenderModeChangeCallbackFunc_t pfnCallback ) = 0;

	// Shuts the render device down and forgets it
	virtual void DestroyDevice() = 0;

	virtual void InstallRenderDeviceSetup( IRenderDeviceSetup *pSetup ) = 0;
	virtual void AddDeviceEventListener( IRenderDeviceEventListener *pListener ) = 0;
	virtual void RemoveDeviceEventListener( IRenderDeviceEventListener *pListener ) = 0;

	// The video.cfg settings in use
	virtual KeyValues *GetVideoConfig() = 0;

	// A second video.cfg KeyValues, created on first use from the ".bak" copy of the user file
	// (named by what it does)
	virtual KeyValues *GetVideoConfigBackup() = 0;

	// setting.monitor_index of the video config, overridable on the command line
	virtual int GetMonitorIndex() = 0;

	virtual void InvokeModeChangeCallbacks( const RenderDeviceInfo_t &info ) = 0;

	// Writes the video config back to the user settings
	virtual void WriteVideoConfig() = 0;

	virtual void ResetVideoConfig() = 0;

	// Fills modes with the display modes of an adapter output
	virtual void GetAllDisplayModes( CUtlVector< RenderDisplayMode_t > &modes, int nAdapter, int nOutput, bool bIncludeUnsupported ) = 0;

	virtual uint32 GetPlatWindowFlags() = 0;
	virtual bool GetVideoMemoryInfo( int nAdapter, uint64 *pTotal, uint64 *pDedicated, uint64 *pShared ) = 0;

	// Appends the adapter and video mode description to a crash dump comment
	virtual void AppendMinidumpInfo( CMiniDumpComment *pComment, bool bVerbose ) = 0;

	// Highest DX level the device supports (110, 111, 120)
	virtual int GetMaxDXLevel() = 0;
};

//-----------------------------------------------------------------------------
// Second interface of the DX11 device manager, registered as "VRenderDeviceMgrBackdoor001"
// next to the IRenderDeviceMgr of the same object. Not an app system.
//-----------------------------------------------------------------------------
abstract_class IRenderDeviceMgrBackdoor
{
public:
	// Named by what it does: stores the render system type that the caps report as the
	// overriding type and makes Init succeed without creating a render device
	virtual void OverrideRenderSystemType( int nRenderSystemType ) = 0;

	// Named by what it does: creates the sheet cache texture ("sheetcachetexture_.vtex",
	// Rendersystem/SheetCacheTextureRows and SheetCacheTextureCols of gameinfo) when the device
	// has none yet
	virtual void CreateSheetCacheTexture() = 0;
};

#endif // IRENDERDEVICEMGR_H
