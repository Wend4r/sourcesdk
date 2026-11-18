//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A top level panorama window
//
//===========================================================================//

#ifndef IUIWINDOW_H
#define IUIWINDOW_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/platwindow.h"
#include "tier0/utllinkedlist.h"

template < typename T > class CStrongHandle;
class InfoForResourceTypeCTextureBase;

class ISceneView;
class ISceneLayer;
class IRenderContext;

namespace panorama
{

class IUIPanel;
class IUIWindowInput;
class IUIRenderEngine;
class IUIRenderDevice;
class IUIImageManager;
struct InputMessage_t;

// The slots after the destructor belong to the engine's top level window class
// (CTopLevelWindowSource2). On Linux the destructor takes two slots and three more slots follow
// BInitializeSurface: the overrides of the class's second base (GetKeyFocusedObject,
// GetFocusedObject and GetAssociatedPlatWindow in Dota 2), which this interface leaves out.
abstract_class IUIWindow
{
public:
	// Draws the window from the procedural layer of a scene view
	virtual void Paint( ISceneView *pView, IRenderContext *pRenderContext, ISceneLayer *pLayer ) = 0;

	// Lays out and paints the panels into the commands Paint draws; the game runs it as a job per view,
	// its procedural layer calls it directly when there is none
	virtual void LayoutAndPaint() = 0;

	// While true the window's input releases every held mouse button IInputSystem::IsButtonDown
	// reports as up, each frame
	virtual bool BUseAutoMouseUpBehavior() = 0;

	// Deletes the window object
	virtual void Delete() = 0;

	virtual IUIWindowInput *UIWindowInput() = 0;
	virtual IUIRenderEngine *UIRenderEngine() = 0;
	virtual IUIRenderDevice *UIRenderDevice() = 0;

	// Re-lays out every panel of the window
	virtual void SetWindowScaleFactor( float flScale ) = 0;
	virtual float GetWindowScaleFactor() = 0;

	virtual bool BCursorVisible() = 0;

	virtual uint32 GetSurfaceWidth() = 0;
	virtual uint32 GetSurfaceHeight() = 0;
	virtual uint32 GetWindowWidth() = 0;
	virtual uint32 GetWindowHeight() = 0;
	virtual void GetClientDimensions( float &flWidth, float &flHeight ) = 0;
	virtual void OnWindowResize( int nWidth, int nHeight ) = 0;

	// Does nothing on this window type
	virtual void SetWindowPosition( float x, float y ) = 0;
	virtual void GetPosition( float *pX, float *pY ) = 0;

	// Brings the platform window of the window to the front
	virtual void Activate( bool bSendInput ) = 0;
	virtual bool IsWindowFocused() = 0;

	virtual uint32 GetNumVisibleTopLevelPanels() = 0;
	virtual const CUtlLinkedList< IUIPanel * > &GetTopLevelVisiblePanels() = 0;

	virtual bool IsMouseOver() = 0;

	// Takes an EMouseCursors value
	virtual void SetMouseCursor( int eCursor ) = 0;

	virtual bool BIsWindowInLayoutPass() = 0;

	// Named by what they do: true while the second layout pass runs; the request makes the next
	// layout run a second pass
	virtual bool BIsWindowInSecondLayoutPass() = 0;
	virtual void RequestSecondLayoutPass() = 0;

	// An opaque pointer shared by the code that owns the window
	virtual void SetContextPtr( void *pContext ) = 0;
	virtual void *GetContextPtr() const = 0;

	// Classes applied to every top level panel of the window
	virtual void AddClass( const char *pszName ) = 0;
	virtual void RemoveClass( const char *pszName ) = 0;
	virtual void SetHasClass( const char *pszName, bool bHasClass ) = 0;

	virtual void ForceFullRepaint() = 0;

	// Blocks every input event except the escape key
	virtual void SetInhibitInput( bool bInhibitInput ) = 0;

	// Named by what it does: BAllowInput drops every input event whose source is the gamepad
	virtual void SetInhibitGamepadInput( bool bInhibitInput ) = 0;

	// Named by what they do: three window flags, each read by the first and set for good by the
	// second slot of a pair; panorama itself never sets or reads them, only the window's owner
	virtual bool BLatchedFlag0() = 0;
	virtual void SetLatchedFlag0() = 0;
	virtual bool BLatchedFlag1() = 0;
	virtual void SetLatchedFlag1() = 0;
	virtual bool BLatchedFlag2() = 0;
	virtual void SetLatchedFlag2() = 0;

	// Name from CS:GO IUIWindow; nothing in CS2 panorama reads the stored value
	virtual void SetMinFPS( float flMinFPS ) = 0;

	// Forward to the window input (its gamepad controller)
	virtual bool BWasGamepadConnectedThisSession() = 0;
	virtual bool BWasGamepadUsedThisSession() = 0;

	// An invisible window is neither painted nor hit-tested
	virtual bool IsVisible() = 0;
	virtual void SetVisible( bool bVisible ) = 0;

	// Takes and returns an EWindowFocusBehavior value
	virtual uint8 GetFocusBehavior() = 0;
	virtual void SetFocusBehavior( uint8 eBehavior ) = 0;

	virtual void OnDeviceLost() = 0;
	virtual void OnDeviceRestored() = 0;
	virtual bool BDeviceLost() = 0;
	virtual void ClearGPUResourcesBeforeNextFrame() = 0;

	// Order of the window among the views of the game UI: a higher level is drawn above
	// and offered input first, equal levels are ordered by address
	virtual void SetLayerLevel( int nLevel ) = 0;
	virtual int GetLayerLevel() = 0;

	// Named by what they do: the scale that fits the window height to a 1080 pixel layout, or its
	// width when the window is narrower than the aspect ratio the setter stores (0 turns that off)
	virtual float GetAutoScaleFactor() = 0;
	virtual void SetAutoScaleAspectRatio( float flAspectRatio ) = 0;

	// The platform window input events must name to reach the window
	virtual void SetPlatWindow( PlatWindow_t hWindow ) = 0;

	// Set once the platform window reports it was closed; a closed window ignores input
	virtual bool IsClosed() = 0;

	virtual void LayoutAndPaintIfNeeded() = 0;

	// Named by what they do: set on the windows IUIEngine::CreateNewSurfacelessUIWindow creates
	virtual bool BIsSurfaceless() = 0;
	virtual void SetSurfaceless( bool bSurfaceless ) = 0;

	// Named by what it does: takes the polymorphic object *ppObject points to (and clears the
	// pointer); the window deletes it after its next layout pass
	virtual void DeleteAfterNextLayout( void **ppObject ) = 0;

	// Named by what they do: the render target texture of the window's surface (an empty handle
	// when the surface draws to the back buffer) and the bDrawToBackBuffer of BInitializeSurface
	virtual CStrongHandle< InfoForResourceTypeCTextureBase > GetRenderTargetTexture() = 0;
	virtual bool BDrawToBackBuffer() = 0;

	virtual ~IUIWindow() {}

	virtual bool FinishInitialization() = 0;
	virtual void RunPlatformFrame() = 0;
	virtual void GetWindowBounds( float &flLeft, float &flTop, float &flRight, float &flBottom ) = 0;
	virtual bool BAllowInput( InputMessage_t &msg ) = 0;

	// Paints an empty frame so the render threads only release their data
	virtual void PaintEmptyFrameAndForceLaterRepaint() = 0;

	virtual IUIImageManager *AccessImageManager() = 0;

	// Name the window was created with
	virtual const char *GetName() = 0;

	virtual void Shutdown() = 0;
	virtual bool BInitializeSurface( int x, int y, int nWidth, int nHeight, bool bFixedSurfaceSize, bool bEnforceWindowAspectRatio, bool bAcceptKBandMouse, bool bDrawToBackBuffer ) = 0;
};

} // namespace panorama

#endif // IUIWINDOW_H
