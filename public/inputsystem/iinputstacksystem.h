//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: This is input priority system, allowing various clients to
// cause input messages / cursor control to be routed to them as opposed to
// other clients.
//
//===========================================================================//

#ifndef IINPUTCLIENTSTACK_H
#define IINPUTCLIENTSTACK_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "inputsystem/iinputsystem.h"


///-----------------------------------------------------------------------------
/// A handle to an input context. These are arranged in a priority-based
/// stack; the top context on the stack which is also enabled wins.
///-----------------------------------------------------------------------------
DECLARE_POINTER_HANDLE( InputContextHandle_t );
#define INPUT_CONTEXT_HANDLE_INVALID ( (InputContextHandle_t)0 )


///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
enum InputContextStateFlags_t
{
	INPUT_CONTEXT_STATE_CURSOR				= 0x1,	// Cursor icon and visibility
	INPUT_CONTEXT_STATE_MOUSE_CAPTURE		= 0x2,
	INPUT_CONTEXT_STATE_CURSOR_CLIP			= 0x4,
	INPUT_CONTEXT_STATE_RELATIVE_MOUSE		= 0x8,
	INPUT_CONTEXT_STATE_STANDARD_CURSORS	= 0x10,	// Overrides of the well-known cursor icons
	INPUT_CONTEXT_STATE_IME					= 0x40,
};


///-----------------------------------------------------------------------------
/// Purpose: This is input priority system, allowing various clients to
/// cause input messages / cursor control to be routed to them as opposed to
/// other clients.
///-----------------------------------------------------------------------------
abstract_class IInputStackSystem : public IAppSystem
{
public:
	/// Allocates an input context, pushing it on top of the input stack,
	virtual InputContextHandle_t PushInputContext( const char *pName, uint32 nStateFlags ) = 0;

	/// Pops the top input context off the input stack, and destroys it.
	virtual void PopInputContext() = 0;

	virtual bool DestroyInputContext( InputContextHandle_t hContext ) = 0;

	/// Moves hContext directly above or below hOther in the stack
	virtual bool MoveInputContextAbove( InputContextHandle_t hContext, InputContextHandle_t hOther ) = 0;
	virtual bool MoveInputContextBelow( InputContextHandle_t hContext, InputContextHandle_t hOther ) = 0;

	/// Enables/disables an input context, allowing something lower on the
	/// stack to have control of input.
	virtual void EnableInputContext( InputContextHandle_t hContext, bool bEnable ) = 0;

	/// Allows a context to make the cursor visible;
	/// the topmost enabled context wins
	virtual void SetCursorVisible( InputContextHandle_t hContext, bool bVisible ) = 0;

	/// Allows a context to set the cursor icon;
	/// the topmost enabled context wins
	virtual void SetCursorIcon( InputContextHandle_t hContext, InputCursorHandle_t hCursor ) = 0;

	/// Sets the cursor icon of every context that owns the cursor
	virtual void SetCursorIconAllContexts( InputCursorHandle_t hCursor ) = 0;

	virtual void SetMouseCapture( InputContextHandle_t hContext, PlatWindow_t hWnd ) = 0;

	virtual bool IsTopmostEnabledContext( InputContextHandle_t hContext, uint32 nStateFlags ) const = 0;

	virtual void SetCursorClip( InputContextHandle_t hContext, PlatWindow_t hWnd ) = 0;

	/// Allows a context to switch relative mouse mode
	virtual void SetRelativeMouseMode( InputContextHandle_t hContext, bool bEnable ) = 0;
	virtual bool GetRelativeMouseMode( InputContextHandle_t hContext ) const = 0;

	/// Per-context overrides of the well-known cursor icons
	virtual InputCursorHandle_t GetStandardCursorOverride( InputContextHandle_t hContext, InputStandardCursor_t id ) = 0;
	virtual void SetStandardCursorOverride( InputContextHandle_t hContext, InputStandardCursor_t id, InputCursorHandle_t hCursor ) = 0;

	/// Allows a context to allow or disallow IME text input
	virtual void SetIMEAllowed( InputContextHandle_t hContext, bool bAllowed ) = 0;

	virtual void UpdateState( uint32 nStateFlags ) = 0;

	virtual void PrintInputStack() = 0;
};

DECLARE_TIER2_INTERFACE( IInputStackSystem, g_pInputStackSystem );


#endif // IINPUTCLIENTSTACK_H
