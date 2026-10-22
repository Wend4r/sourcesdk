#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "inputsystem/iinputsystem.h"
#include "tier0/bitvec.h"
#include "tier0/threadtools.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringmap.h"
#include "tier1/utlvector.h"
#include "tier2/tier2.h"

class CInputSystem : public CTier2AppSystem< IInputSystem >
{
public:
	virtual ~CInputSystem() {}

	struct InputWindow_t
	{
		void *m_hAttachedHWnd;
		void *m_ChainedWndProc;
	};

	// The engine's button and analog code tables are larger than the ButtonCode_t and AnalogCode_t enums
	struct InputState_t
	{
		CBitVec< 0x204 > m_ButtonState;

		// Read by WasButtonPressed, WasButtonDoubleClicked and WasButtonReleased
		CBitVec< 0x204 > m_ButtonPressed;
		CBitVec< 0x204 > m_ButtonDoubleClicked;
		CBitVec< 0x204 > m_ButtonReleased;

		int m_ButtonPressedTick[ 0x204 ];
		int m_ButtonReleasedTick[ 0x204 ];
		float m_pAnalogDelta[ 27 ];
		float m_pAnalogValue[ 27 ];
		CUtlVector< InputEvent_t > m_Events;

		// Copies of the WM_COPYDATA payloads that IE_CopyData events point to, freed through g_pMemAlloc when the
		// state is cleared or copied over
		CUtlVector< void * > m_CopyDataBuffers;
		bool m_bDirty;
	};

	// One poll of a joystick
	struct JoystickState_t
	{
		int16 m_nAxisValues[ MAX_JOYSTICK_AXES ];

		// One bit per pressed button, per pressed POV direction and per axis direction past the threshold
		uint32 m_nButtons;
		uint32 m_nPOVButtons;
		uint32 m_nAxisButtons;
	};

	struct JoystickInfo_t
	{
		// SDL id from the device event that added the joystick, printed by "initializing SDL joystick" and matched by the removal event
		uint32 m_nSDLJoystickID;

		// SDL joystick opened for that id; polled for the axes and buttons and closed on removal
		void *m_pJoystick;

		JoystickState_t m_State;

		// The previous poll; the button events are posted for the bits that differ from it
		JoystickState_t m_PrevState;

		bool m_bDiagonalPOVControlEnabled;
	};

	struct CursorEntry_t;

	struct UserCursor_t
	{
		CursorEntry_t *pCursorEntry;
	};

	// Window of the last posted IE_IMESetWindow event
	PlatWindow_t m_hLastIMEHWnd;

	CUtlVector< InputWindow_t > m_Windows;
	bool m_bEnabled;
	bool m_bPumpEnabled;

	// Index of the m_InputState that PostUserEvent and the button events write
	uint8 m_nCurrentInputState;

	uint8 m_pad04B;
	bool m_bHasFocus;
	bool m_bIsActivated;
	bool m_bEnablePowerManagement;

	// Set by -disable_cursor_clip, keeps SetCursorClip from grabbing the window
	bool m_bDisableCursorClip;

	bool m_bButtonCodeIsScanCode;

	// "InputSystem/LockButtonCodeIsScanCode"; SetButtonCodeIsScanCode ignores changes while it is set
	bool m_bLockButtonCodeIsScanCode;

	bool m_bIMEAllowed;
	bool m_bCursorVisible;
	bool m_bMouseRelativeMode;
	float m_flCursorBiasX;
	float m_flCursorBiasY;
	float m_flCursorScaleX;
	float m_flCursorScaleY;
	PlatWindow_t m_hCursorCoordinateBiasWindow;
	uint32 m_nInputSystemCommandEnableFlags;
	InputState_t m_InputState[ 2 ];
	uint32 m_StartupTimeTick;
	int m_nLastPollTick;
	int m_nLastSampleTick;
	int m_nPollCount;

	// Plat_FloatTime() of the last SampleDevices, returned by GetPollTime and stamped on posted events
	double m_flLastPollTime;

	uint32 m_uiMouseWheel;

	// From the joystick command line parameters or cl_joystick_enabled
	bool m_bJoystickEnabled;

	// Changed by EnableJoystickInput, one bit per joystick
	uint16 m_nEnabledJoystickMask;

	int m_nJoystickCount;

	// Set once the SDL game controller subsystem is initialized ("Couldn't initialize joysticks" otherwise)
	bool m_bJoysticksInitialized;

	JoystickInfo_t m_pJoystickInfo[ 4 ];

	// EnableMouseCapture, DisableMouseCapture and IsMouseCaptured
	PlatWindow_t m_hMouseCaptureWindow;

	PlatWindow_t m_hCursorClipWindow;

	// Created from the SDL system cursors, returned by GetStandardCursor
	InputCursorHandle_t m_pSystemCursors[ 26 ];

	// Copy of m_pSystemCursors, changed by SetStandardCursorOverride
	InputCursorHandle_t m_pDefaultCursors[ 26 ];

	CUtlStringMap< UserCursor_t > m_UserCursors;
	float m_fLoadedCursorScale;

	// EnableAutoCursorScale and IsAutoCursorScaleEnabled
	bool m_bAutoCursorScaleEnabled;

	// Set by -cursor_scale_percent, which also sets m_fLoadedCursorScale; CanSetCursorScale returns false then
	bool m_bCursorScalePercent;

	InputCursorHandle_t m_hCursor;
	CUtlString m_strPrevSDLAltF4Hint;
	bool m_bAllowAltF4;
	bool m_bPass_VK_PROCESSKEY;
	uint32 m_unLastFingerDownTime;
	void *m_pSDLHaptic;
	int64 m_curFingerID;
	bool m_curFingerActive;
	int m_nFingersDown;

	// Incremented by QueueResetInputState, consumed by PollInputState ("Applying deferred input state reset")
	CInterlockedInt m_nDeferredInputStateResets;

	// Set to 3 by the "OnEventThatMightCauseFrameRateHitch" handler; each SDL event pump while it is positive decrements it
	// and passes m_strFrameTimeAmnestyReason to IEngineServiceMgr::SetFrameTimeAmnesty
	int m_nFrameTimeAmnestyPumps;
	CUtlString m_strFrameTimeAmnestyReason;

#ifdef _WIN32
	CUtlVector< InputWindowMessageHookFn_t > m_WindowMessageHooks;
#endif
	CUtlVector< InputSDLEventHookFn_t > m_SDLEventHooks;
};

#endif // INPUTSYSTEM_H
