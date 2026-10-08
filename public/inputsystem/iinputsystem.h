//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
//===========================================================================//

#ifndef IINPUTSYSTEM_H
#define IINPUTSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/platwindow.h"
#include "appframework/iappsystem.h"
#include "inputsystem/InputEnums.h"
#include "inputsystem/ButtonCode.h"
#include "inputsystem/AnalogCode.h"
#include "tier1/utlvector.h"


///-----------------------------------------------------------------------------
/// A handle to a cursor icon
///-----------------------------------------------------------------------------
DECLARE_POINTER_HANDLE( InputCursorHandle_t );
#define INPUT_CURSOR_HANDLE_INVALID ( (InputCursorHandle_t)0 )


///-----------------------------------------------------------------------------
/// An enumeration describing well-known cursor icons
///-----------------------------------------------------------------------------
enum InputStandardCursor_t
{
	INPUT_CURSOR_NONE	= 0,
	INPUT_CURSOR_ARROW,
	INPUT_CURSOR_IBEAM,
	INPUT_CURSOR_HOURGLASS,
	INPUT_CURSOR_CROSSHAIR,
	INPUT_CURSOR_WAITARROW,
	INPUT_CURSOR_UP,
	INPUT_CURSOR_SIZE_NW_SE,
	INPUT_CURSOR_SIZE_NE_SW,
	INPUT_CURSOR_SIZE_W_E,
	INPUT_CURSOR_SIZE_N_S,
	INPUT_CURSOR_SIZE_ALL,
	INPUT_CURSOR_NO,
	INPUT_CURSOR_HAND,

	INPUT_CURSOR_COUNT
};


///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
typedef bool ( *InputSDLEventHookFn_t )( const void *pSDLEvent );


///-----------------------------------------------------------------------------
/// Main interface for input. This is a low-level interface, creating an
/// OS-independent queue of low-level input events which were sampled since
/// the last call to PollInputState. It also contains facilities for cursor
/// control and creation.
///-----------------------------------------------------------------------------
abstract_class IInputSystem : public IAppSystem
{
public:
	virtual void AttachToWindow( void *hWnd ) = 0;
	virtual void DetachFromWindow( void *hWnd ) = 0;

	/// Enables/disables input. PollInputState will not update current
	/// button/analog states when it is called if the system is disabled.
	virtual void EnableInput( bool bEnable ) = 0;

	/// Enables/disables the windows message pump. PollInputState will not
	/// Peek/Dispatch messages if this is disabled
	virtual void EnableMessagePump( bool bEnable ) = 0;

	virtual void PollInputState( bool bUnknown ) = 0;

	/// Gets the time of the last polling in ms
	virtual int GetPollTick() const = 0;

	/// Plat_FloatTime() of the last device sample
	virtual double GetPollTime() const = 0;

	/// Is any button currently down?
	virtual bool IsAnyButtonDown() const = 0;

	/// Is a button down? "Buttons" are binary-state input devices (mouse buttons, keyboard keys)
	virtual bool IsButtonDown( ButtonCode_t code ) const = 0;

	virtual bool WasButtonPressed( ButtonCode_t code ) const = 0;
	virtual bool WasButtonDoubleClicked( ButtonCode_t code ) const = 0;
	virtual bool WasButtonReleased( ButtonCode_t code ) const = 0;

	/// Returns the tick at which the button was pressed and released
	virtual int GetButtonPressedTick( ButtonCode_t code ) const = 0;
	virtual int GetButtonReleasedTick( ButtonCode_t code ) const = 0;

	/// Gets the value of an analog input device this frame
	/// Includes joysticks, mousewheel, mouse
	virtual float GetAnalogValue( AnalogCode_t code ) const = 0;

	/// Gets the change in a particular analog input device this frame
	/// Includes joysticks, mousewheel, mouse
	virtual float GetAnalogDelta( AnalogCode_t code ) const = 0;

	/// Returns the input events since the last poll
	virtual int GetEventCount() const = 0;
	virtual const InputEvent_t *GetEventData() const = 0;
	virtual const CUtlVector< InputEvent_t > *GetEvents() const = 0;

	/// Posts a user-defined event into the event queue; this is expected
	/// to be called in overridden wndprocs connected to the root panel.
	virtual void PostUserEvent( const InputEvent_t &event ) = 0;

	/// Returns the number of joysticks
	virtual int GetJoystickCount() const = 0;

	/// Enable/disable joystick, it has perf costs
	virtual void EnableJoystickInput( int nJoystick, bool bEnable ) = 0;

	/// Enable/disable diagonal joystick POV (simultaneous POV buttons down)
	virtual void EnableJoystickDiagonalPOV( int nJoystick, bool bEnable ) = 0;

	/// Sample the joystick and append events to the input queue
	virtual void SampleDevices( void ) = 0;

	/// Force-feedback, both are no-ops
	virtual void SetRumble( float fLeftMotor, float fRightMotor, int userId = INVALID_USER_ID ) = 0;
	virtual void StopRumble( int userId = INVALID_USER_ID ) = 0;

	/// Resets the input state
	virtual void ResetInputState() = 0;

	virtual void QueueResetInputState() = 0;

	virtual const char *CodeToString( ButtonCode_t code ) const = 0;
	virtual const char *CodeToString( AnalogCode_t code ) const = 0;
	virtual ButtonCode_t StringToButtonCode( const char *pString ) const = 0;
	virtual AnalogCode_t StringToAnalogCode( const char *pString ) const = 0;

	/// Sleeps until input happens. Pass a negative number to sleep infinitely
	virtual void SleepUntilInput( int nMaxSleepTimeMS = -1 ) = 0;

	/// Convert back + forth between virtual codes + button codes
	virtual ButtonCode_t VirtualKeyToButtonCode( int nVirtualKey ) const = 0;
	virtual int ButtonCodeToVirtualKey( ButtonCode_t code ) const = 0;

	/// How many times have we called PollInputState?
	virtual int GetPollCount() const = 0;

	virtual void GetCursorPosition( float *pX, float *pY, PlatWindow_t hWnd ) = 0;
	virtual void SetCursorPosition( float x, float y, PlatWindow_t hWnd ) = 0;

	/// Is the window attached through AttachToWindow?
	virtual bool IsAttachedToWindow( PlatWindow_t hWnd ) const = 0;

	/// Returns the cursor loaded for a well-known cursor icon
	virtual InputCursorHandle_t GetStandardCursor( InputStandardCursor_t id ) = 0;

	virtual InputCursorHandle_t GetStandardCursorOverride( InputStandardCursor_t id ) = 0;
	virtual void SetStandardCursorOverride( InputStandardCursor_t id, InputCursorHandle_t hCursor ) = 0;

	virtual bool CanSetCursorScale() const = 0;
	virtual bool IsAutoCursorScaleEnabled() const = 0;
	virtual void EnableAutoCursorScale( bool bEnable ) = 0;

	/// A scale of 0.001 or less turns automatic scaling on
	virtual void SetCursorScale( float flScale ) = 0;

	/// Loads a cursor defined in a file
	virtual InputCursorHandle_t LoadCursorFromFile( const char *pFileName, const char *pPathID = NULL ) = 0;

	virtual void ReleaseCursor( InputCursorHandle_t hCursor ) = 0;

	/// Sets the cursor icon; a NULL cursor hides it
	virtual void SetCursorIcon( InputCursorHandle_t hCursor, bool bForce = false ) = 0;
	virtual InputCursorHandle_t GetCursorIcon() const = 0;

	/// Master switch for showing the OS cursor
	virtual void SetCursorVisible( bool bVisible ) = 0;
	virtual bool IsCursorVisible() const = 0;

	virtual bool GetCursorCoordinateBias( PlatWindow_t *pWnd, float *pBiasX, float *pBiasY, float *pScaleX, float *pScaleY ) = 0;
	virtual bool SetCursorCoordinateBias( PlatWindow_t hWnd, float flBiasX, float flBiasY, float flScaleX, float flScaleY ) = 0;

	/// Mouse capture
	virtual void EnableMouseCapture( PlatWindow_t hWnd ) = 0;
	virtual void DisableMouseCapture() = 0;
	virtual bool IsMouseCaptured() const = 0;

	/// Does one of the attached windows have focus?
	virtual bool IsWindowFocused() const = 0;

	/// Returns the name of an InputEventType_t value
	virtual const char *InputEventTypeToString( int nType ) const = 0;

	/// Is the application active?
	virtual bool IsAppActive() const = 0;

	/// Is at least one joystick connected?
	virtual bool HasJoysticks() const = 0;

	virtual void StartWindowDrag( void *hWnd ) = 0;

	/// Confines the cursor to the window; NULL releases it
	virtual void SetCursorClip( PlatWindow_t hWnd ) = 0;

	virtual void unk074( bool bEnable ) = 0;

	virtual void unk075( uint32 nFlags, bool bSet ) = 0;

	/// Relative mouse mode on all attached windows
	virtual void SetRelativeMouseMode( bool bEnable ) = 0;
	virtual bool GetRelativeMouseMode() const = 0;

	/// Prints the SDL and OS cursor, capture and clip state
	virtual void PrintCursorState() = 0;

	/// Converts a key event to the character it types, or 0
	virtual char KeyEventToChar( const InputEvent_t &event ) = 0;

	/// Convert back + forth between SDL key codes + button codes
	virtual int ButtonCodeToKeyCode( ButtonCode_t code ) const = 0;
	virtual ButtonCode_t KeyCodeToButtonCode( int nKeyCode ) const = 0;

	virtual int GetScanCodeCount() const = 0;

	/// Convert back + forth between scan codes + button codes
	virtual int ButtonCodeToScanCode( ButtonCode_t code ) const = 0;
	virtual ButtonCode_t ScanCodeToButtonCode( int nScanCode ) const = 0;

	/// Convert back + forth between scan codes + their SDL names
	virtual const char *ScanCodeToString( int nScanCode ) const = 0;
	virtual int StringToScanCode( const char *pString ) const = 0;

	virtual const char *ButtonCodeToDisplayString( ButtonCode_t code ) const = 0;
	virtual ButtonCode_t DisplayStringToButtonCode( const char *pString ) const = 0;

	virtual void RefreshButtonCodeDisplayStrings() = 0;

	virtual bool GetButtonCodeIsScanCode() const = 0;
	virtual void SetButtonCodeIsScanCode( bool bScanCode ) = 0;

	/// Recomputes the automatic cursor scale
	virtual void UpdateCursorScale() = 0;

	/// IME (text input) support
	virtual bool IsIMEAllowed() const = 0;
	virtual void SetIMEAllowed( bool bAllowed ) = 0;
	virtual void SetIMETextInputRect( int x, int y, int w, int h ) = 0;
	virtual void ResetIME() = 0;
	virtual void CreateIMEWindow() = 0;

	virtual void PostButtonPressedEvent( PlatWindow_t hWnd, int nType, int nTick, ButtonCode_t code, int nData2, bool bDoubleClick, uint64 nUnk ) = 0;
	virtual void PostButtonReleasedEvent( PlatWindow_t hWnd, int nType, int nTick, ButtonCode_t code, int nData2, uint64 nUnk ) = 0;

#ifdef _WIN32
	virtual void unk100( void *p ) = 0;
	virtual void unk101( void *p ) = 0;
#endif

	virtual void AddSDLEventHook( InputSDLEventHookFn_t pfnHook ) = 0;
	virtual void RemoveSDLEventHook( InputSDLEventHookFn_t pfnHook ) = 0;

	virtual double SDLTimestampToFloatTime( uint64 nTimestamp ) = 0;

	inline const char *ButtonCodeToString( ButtonCode_t code ) const { return CodeToString( code ); }
	inline const char *AnalogCodeToString( AnalogCode_t code ) const { return CodeToString( code ); }
};

DECLARE_TIER2_INTERFACE( IInputSystem, g_pInputSystem );


#endif // IINPUTSYSTEM_H
