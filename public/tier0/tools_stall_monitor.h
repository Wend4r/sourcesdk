#ifndef TIER0_TOOLS_STALL_MONITOR_H
#define TIER0_TOOLS_STALL_MONITOR_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/threadtools.h"

//-----------------------------------------------------------------------------
// The stall monitor of the tools, which watches the main thread from a thread of its own;
// the functions do nothing while it is not running
//-----------------------------------------------------------------------------
class CToolsStallMonitorScope;

// Tells the monitor that the main thread is still working
PLATFORM_INTERFACE void ToolsStallMonitor_IndicateActivity();

// The native handle of the main window and an RGBA image for the monitor to display
PLATFORM_INTERFACE void ToolsStallMonitor_SetMainThreadWindow( void *hWindow );
PLATFORM_INTERFACE void ToolsStallMonitor_SetDisplayImageRGBA( int nWidth, int nHeight, const byte *pBytes );

#ifdef _WIN32
// Starts the monitor of the calling thread, which reports a stall after nStallThresholdMS without
// activity; its window thread must have created the window before ToolsStallMonitor_Shutdown
PLATFORM_INTERFACE void ToolsStallMonitor_Initialize( uint64 nStallThresholdMS );
#endif

PLATFORM_INTERFACE void ToolsStallMonitor_Shutdown();

// Scopes of the main thread the monitor tracks
PLATFORM_INTERFACE void ToolsStallMonitorInternal_BeginScope( CToolsStallMonitorScope *pScope );
PLATFORM_INTERFACE void ToolsStallMonitorInternal_EndScope( CToolsStallMonitorScope *pScope );

#endif // TIER0_TOOLS_STALL_MONITOR_H
