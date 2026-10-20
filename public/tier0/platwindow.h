//===== Copyright © 1996-2009, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

#ifndef PLATWINDOW_H
#define PLATWINDOW_H

#ifdef COMPILER_MSVC32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/basetypes.h"

//-----------------------------------------------------------------------------
// Window handle
//-----------------------------------------------------------------------------
DECLARE_POINTER_HANDLE( PlatWindow_t );
#define PLAT_WINDOW_INVALID ( (PlatWindow_t)0 )


//-----------------------------------------------------------------------------
// Window creation
//-----------------------------------------------------------------------------
enum WindowCreateFlags_t
{
	WINDOW_CREATE_FULLSCREEN	= 0x1,
	WINDOW_CREATE_RESIZING		= 0x2,

};

PLATFORM_INTERFACE PlatWindow_t Plat_CreateWindow( void *hInstance, const char *pTitle, int x, int y, int nWidth, int nHeight, int nFlags, float flRefreshRate );


//-----------------------------------------------------------------------------
// Window title
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE void Plat_SetWindowTitle( PlatWindow_t hWindow, const char *pTitle );
PLATFORM_INTERFACE void Plat_SetWindowTitleW( PlatWindow_t hWindow, const wchar_t *pTitle );


//-----------------------------------------------------------------------------
// Window movement
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE void Plat_SetWindowPos( PlatWindow_t hWindow, int x, int y );


//-----------------------------------------------------------------------------
// Gets the desktop resolution
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE bool Plat_GetDesktopResolution( int nDisplayIndex, int *pWidth, int *pHeight, uint32 *pRefreshRate );


//-----------------------------------------------------------------------------
// Gets a window size
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE void Plat_GetWindowClientSize( PlatWindow_t hWindow, int *pWidth, int *pHeight );


//-----------------------------------------------------------------------------
// Is the window minimized?
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE bool Plat_IsWindowMinimized( PlatWindow_t hWindow );


//-----------------------------------------------------------------------------
// Gets the shell window in a console app
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE PlatWindow_t Plat_GetShellWindow( );


//-----------------------------------------------------------------------------
// Convert window -> Screen coordinates and back
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE void Plat_WindowToScreenCoords( PlatWindow_t hWnd, int &x, int &y );
PLATFORM_INTERFACE void Plat_ScreenToWindowCoords( PlatWindow_t hWnd, int &x, int &y );


//-----------------------------------------------------------------------------
// OS specific window handles
//-----------------------------------------------------------------------------
DECLARE_POINTER_HANDLE( OsSpecificWindowHandle_t );

PLATFORM_INTERFACE PlatWindow_t Plat_FindOrCreateWrappedPlatWindow( OsSpecificWindowHandle_t hOsWindow, WindowCreateFlags_t nFlags );
PLATFORM_INTERFACE PlatWindow_t Plat_OsSpecificHandleToPlatWindow( OsSpecificWindowHandle_t hWindow );
PLATFORM_INTERFACE OsSpecificWindowHandle_t Plat_WindowToOsSpecificHandle( PlatWindow_t hWindow );

#ifdef PLATFORM_WINDOWS
PLATFORM_INTERFACE void Plat_SetActiveWindowOsSpecific( OsSpecificWindowHandle_t hWindow );
PLATFORM_INTERFACE void Plat_GetWindowClientSizeOsSpecific( OsSpecificWindowHandle_t hWindow, int *pWidth, int *pHeight );
#endif


//-----------------------------------------------------------------------------
// Window state
//-----------------------------------------------------------------------------
enum PlatShowWindowType_t
{
	PLAT_SHOW_WINDOW_HIDE = 0,
	PLAT_SHOW_WINDOW_SHOW = 1,
	PLAT_SHOW_WINDOW_SHOW_NOACTIVATE = 2,
};

PLATFORM_INTERFACE void Plat_DestroyWindow( PlatWindow_t hWindow );
PLATFORM_INTERFACE void Plat_ReconfigureWindow( PlatWindow_t hWindow, int nMonitorIndex, int x, int y, int nWidth, int nHeight, uint32 nWindowCreateFlags, uint32 nReconfigureFlags, float flRefreshRateHz );
PLATFORM_INTERFACE void Plat_ShowWindow( PlatWindow_t hWindow, PlatShowWindowType_t nType );
PLATFORM_INTERFACE void Plat_MinimizeWindow( PlatWindow_t hWindow, bool bMinimize );
PLATFORM_INTERFACE bool Plat_IsWindowVisible( PlatWindow_t hWindow );
PLATFORM_INTERFACE bool Plat_IsWindowFocused( PlatWindow_t hWindow );
PLATFORM_INTERFACE void Plat_SetActiveWindow( PlatWindow_t hWindow );
PLATFORM_INTERFACE void Plat_SetForegroundWindow( PlatWindow_t hWindow );
PLATFORM_INTERFACE bool Plat_FlashWindow( PlatWindow_t hWindow );
PLATFORM_INTERFACE void Plat_SetWindowSize( PlatWindow_t hWindow, int nWidth, int nHeight );
PLATFORM_INTERFACE void Plat_SetWindowBorder( PlatWindow_t hWindow, bool bHasBorder );
PLATFORM_INTERFACE bool Plat_SetWindowIcon( PlatWindow_t hWindow, const char *pIconBmpFullPath );
PLATFORM_INTERFACE void Plat_SetFullscreenMinimizeOnFocusLoss( bool bMinOnFocusLoss );
PLATFORM_INTERFACE void Plat_GetWindowBounds( PlatWindow_t hWindow, int *pX, int *pY, int *pWidth, int *pHeight );
PLATFORM_INTERFACE void Plat_GetWindowClientBounds( PlatWindow_t hWindow, int *pX, int *pY, int *pWidth, int *pHeight );
PLATFORM_INTERFACE int Plat_GetWindowMonitorIndex( PlatWindow_t hWindow );
PLATFORM_INTERFACE bool Plat_IsScreenKeyboardShown( PlatWindow_t hWindow );


//-----------------------------------------------------------------------------
// Monitors
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE int Plat_GetMonitorCount();
PLATFORM_INTERFACE const char *Plat_GetMonitorName( int nMonitorIndex );
PLATFORM_INTERFACE int Plat_GetMonitorIndexFromName( const char *pName );
PLATFORM_INTERFACE bool Plat_GetMonitorResolution( int nMonitorIndex, int *pWidth, int *pHeight );
PLATFORM_INTERFACE bool Plat_GetMonitorBounds( int nMonitorIndex, int *pX, int *pY, int *pWidth, int *pHeight );
PLATFORM_INTERFACE bool Plat_GetDesktopBounds( int nMonitorIndex, int *pX, int *pY, int *pWidth, int *pHeight );
PLATFORM_INTERFACE float Plat_GetMonitorContentsScale( int nMonitorIndex );

// A mode of a monitor, from SDL_DisplayMode; the names are ours
struct PlatDisplayMode_t
{
	int m_nWidth;
	int m_nHeight;
	float m_flRefreshRate;
};

PLATFORM_INTERFACE bool Plat_GetCurrentDisplayMode( int nMonitorIndex, PlatDisplayMode_t *pMode );

// *pModeCount is the size of pModes on input and the number of the modes on output; pModes
// can be null to count them
PLATFORM_INTERFACE bool Plat_GetFullscreenDisplayModes( int nMonitorIndex, int *pModeCount, PlatDisplayMode_t *pModes );

#ifdef PLATFORM_OSX
// The MetalView of the content view of an NSWindow, and the scales of MoltenVK
PLATFORM_INTERFACE void *GetMoltenVKMetalLayer( void *pNSWindow );
PLATFORM_INTERFACE float GetMoltenVKWindowContentsScale( void *pNSWindow );
PLATFORM_INTERFACE float GetMoltenVKMonitorContentsScale();
PLATFORM_INTERFACE bool GetMoltenVKSupportsMetal();
#endif


//-----------------------------------------------------------------------------
// DPI
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE bool Plat_IsHighDPI();
PLATFORM_INTERFACE float Plat_GetDPI();
PLATFORM_INTERFACE void Plat_RefreshDPI();
PLATFORM_INTERFACE float Plat_GetWindowContentsScale( PlatWindow_t hWindow );


//-----------------------------------------------------------------------------
// User notifications
//-----------------------------------------------------------------------------
// Flashes the window on Windows; a desktop notification elsewhere, the UTF-8 overload goes to Plat_DesktopNotify
PLATFORM_INTERFACE void Plat_AlertUser( PlatWindow_t hWindow, const char *pszUTF8Title, const char *pszUTF8Message );
PLATFORM_OVERLOAD void Plat_AlertUser( PlatWindow_t hWindow, const wchar_t *pszTitle, const wchar_t *pszMessage );
PLATFORM_INTERFACE bool Plat_DesktopNotify( const char *pszUTF8Title, const char *pszUTF8Message );
PLATFORM_INTERFACE bool Plat_SetDesktopNotifyIconPath( const char *pFullPath );


#endif // PLATWINDOW_H
