//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Input state of a top level panorama window: focus, hover, input hooks
//
//===========================================================================//

#ifndef IUIWINDOWINPUT_H
#define IUIWINDOWINPUT_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "panorama/panoramasymbol.h"

namespace panorama
{

class IUIPanel;
class IInputCapture;
struct InputMessage_t;

// The focus of a window is a pair: the focused panel and the topmost panel of its input context
// (the closest ancestor that is the top of an input context). Key events go to the focused panel
// and then to its parents; a window without a focused panel handles no key event.
abstract_class IUIWindowInput
{
public:
	// Runs the input forwarding target first, then IUIWindow::BAllowInput; key events go to the
	// focused panel, mouse events to the panel under the cursor
	virtual bool InputEvent( InputMessage_t &msg, bool bNewEvent ) = 0;

	virtual void OnMouseMove( float flMouseX, float flMouseY ) = 0;
	virtual void OnMouseMoveSurfaceCoords( float flMouseX, float flMouseY ) = 0;

	// Accumulates a 20 byte structure until the next frame
	virtual bool unk003( const void *pData ) = 0;

	virtual void GetSurfaceMousePosition( float &x, float &y ) = 0;

	// Forwards to IUIWindow::BCursorVisible
	virtual bool BCursorVisible() = 0;

	virtual int GetNumGamepadsConnected() = 0;
	virtual bool BWasGamepadConnectedThisSession() = 0;
	virtual bool BWasGamepadUsedThisSession() = 0;
	virtual bool BWasMouseOrKeyboardUsedThisSession() = 0;

	// Forwards to the input engine
	virtual void unk010() = 0;

	// The source of the last input event: 1 gamepad, 2 keyboard, 3 mouse
	virtual bool BWasGamepadLastInputSource() = 0;
	virtual bool BWasMouseLastInputSource() = 0;
	virtual bool BWasKeyboardOrMouseLastInputSource() = 0;
	virtual uint8 GetLastPanelEventSource() = 0;

	// Adds or removes the window from the focused windows of the input engine
	virtual void GotWindowFocus() = 0;
	virtual void LostWindowFocus() = 0;
	virtual bool BHasWindowFocus() = 0;

	// Forwards to IUIWindow::BAllowInput
	virtual bool BAllowInput( InputMessage_t &msg ) = 0;

	virtual void SetInputFocus( IUIPanel *pPanel, bool bScrollParentToFit, bool bChangeContextIfNeeded ) = 0;

	// Restores the focus saved for the input context of the panel; false when the window has no
	// focus or the context has no saved focus
	virtual bool SetInputFocusContext( IUIPanel *pPanelInContext ) = 0;
	virtual void PopInputContext() = 0;

	// Topmost panel of the input context of the focused panel
	virtual IUIPanel *GetInputFocusContext() = 0;
	virtual IUIPanel *GetInputFocus() = 0;
	virtual IUIPanel *GetMouseHover() = 0;

	virtual void PanelDeleted( IUIPanel *pPanel, IUIPanel *pParent ) = 0;

	virtual void HookPanelInput( IUIPanel *pPanel, IInputCapture *pInputCapture ) = 0;
	virtual void RemovePanelInputHook( IUIPanel *pPanel, IInputCapture *pInputCapture ) = 0;

	virtual void AddMouseTrackingPanel( IUIPanel *pPanel ) = 0;
	virtual void RemoveMouseTrackingPanel( IUIPanel *pPanel ) = 0;

	virtual bool BInSetInputFocusTraverse() = 0;
	virtual void QueuePanelFocusEvent( IUIPanel *pPanel, CPanoramaSymbol symPanelEvent ) = 0;

	// Focused panel when the mouse state was last reset
	virtual IUIPanel *GetFocusOnLastMouseDown() = 0;

	// Every input event of this window is passed to the target first
	virtual void SetInputForwarding( IUIWindowInput *pWindowInputForwarding ) = 0;

	virtual bool BDragInProgress() = 0;
	virtual void SetDragDropEnabled( int nMouseCode, bool bEnabled ) = 0;

	virtual bool unk036() = 0;
	virtual bool unk037() = 0;
	virtual void unk038() = 0;
};

} // namespace panorama

#endif // IUIWINDOWINPUT_H
