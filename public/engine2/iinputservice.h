//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service for input handlers, key bindings, the command buffers and cheat codes
//
//=============================================================================//

#ifndef IINPUTSERVICE_H
#define IINPUTSERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <engine/IEngineService.h>
#include <inputsystem/buttoncode.h>
#include <inputsystem/AnalogCode.h>
#include <splitscreenslot.h>
#include <tier0/utlstring.h>
#include <tier1/utldelegate.h>

class CInputValueBase;
class CUtlBuffer;

// Installed with InstallInputHandler; handlers run by descending priority
abstract_class IInputHandler
{
public:
	virtual InputHandlerResult_t HandleInputEvent( const InputEvent_t &event, CSplitScreenSlot nSlot ) = 0;
};

// Notified by SetBinding with the button whose binding changed
abstract_class IKeybindChangeListener
{
public:
	virtual void unk000( ButtonCode_t code ) = 0;
};

abstract_class IInputService : public IEngineService
{
public:
	virtual void InstallInputHandler( const char *pszName, int nPriority, IInputHandler *pHandler ) = 0;
	virtual void RemoveInputHandler( IInputHandler *pHandler ) = 0;

	virtual void AddCommandText( int nTarget, const char *pszText, int nSource ) = 0;

	virtual bool IsAppActive() = 0;

	// "user_keys" bindings of a split screen slot
	virtual void ReadKeyBindings( CSplitScreenSlot nSlot ) = 0;
	virtual void WriteKeyBindings( CSplitScreenSlot nSlot ) = 0;

	virtual void SetBinding( ButtonCode_t code, CSplitScreenSlot nSlot, const char *pszBinding, bool bIsDefaultBinding, int nChord ) = 0;
	virtual const char *GetBinding( ButtonCode_t code, CSplitScreenSlot nSlot, int nChord ) = 0;
	virtual void SetBinding( AnalogCode_t code, CSplitScreenSlot nSlot, const char *pszBinding, bool bIsDefaultBinding, int nChord ) = 0;
	virtual const char *GetBinding( AnalogCode_t code, CSplitScreenSlot nSlot, int nChord ) = 0;

	virtual const char *NameForBinding( const char *pszBinding, CSplitScreenSlot nSlot, int nStartCount, int nAllowJoystick ) = 0;

	// Whether code is bound to toggleconsole (no modifiers held)
	virtual bool IsToggleConsoleButton( ButtonCode_t code, int nModifiers ) = 0;
	// KEY_ESCAPE without modifiers
	virtual bool IsEscapeButton( ButtonCode_t code, int nModifiers ) = 0;

	virtual bool HasMouseFocus() = 0;

	virtual void RegisterInputValue( CInputValueBase *pInputValue ) = 0;
	virtual void UnregisterInputValue( CInputValueBase *pInputValue ) = 0;
	// Stops when the callback returns false
	virtual void ForEachInputValue( const CUtlDelegate< bool ( CInputValueBase * ) > &callback ) = 0;

	virtual void EnableKeyBindingSystem( bool bEnable ) = 0;
	virtual void unk041( bool bUnk ) = 0;
	// Counter incremented by each client input processing pass
	virtual int GetFrameCount() = 0;

	// Counted lock of all command buffers
	virtual void LockCommandBuffers( bool bLock ) = 0;

	virtual void LoadCheatCodes( CUtlBuffer &buf ) = 0;
	virtual void ClearCheatCodes() = 0;

	// nSlot < 0 clears every slot
	virtual void UnbindAllKeys( int nSlot ) = 0;

	virtual void AddBindingListener( IKeybindChangeListener *pListener ) = 0;
	virtual void RemoveBindingListener( IKeybindChangeListener *pListener ) = 0;

	virtual bool WasButtonPressed( ButtonCode_t code ) = 0;
	virtual bool WasButtonDoubleClicked( ButtonCode_t code ) = 0;
	virtual bool WasButtonReleased( ButtonCode_t code ) = 0;
	virtual bool IsButtonDown( ButtonCode_t code ) = 0;
	virtual bool unk053( ButtonCode_t code ) = 0;

	// Cursor position in the engine window
	virtual void GetCursorPosition( float *pX, float *pY ) = 0;
	virtual void SetCursorPosition( float x, float y ) = 0;

	virtual CUtlString GetUserSettingsFileName( const char *pszName, bool *pbExists, bool bNoCfgDir ) = 0;
	virtual void AppendChordModifiers( CUtlString &sName, int nChord ) = 0;

	// When set, an input convar value that fails to parse asserts instead of warning
	virtual void unk058( bool bUnk ) = 0;
	// Empty in this build
	virtual void unk059() = 0;
	virtual void unk060() = 0;

	// 8 with Engine2/AllowKeyChordBindings, 1 otherwise
	virtual int GetNumChords() = 0;

	// Stores both pointers in two parallel lists; the client passes its input frame counter and the static next to it
	virtual void unk062( int *pUnk1, void *pUnk2 ) = 0;
};

#endif // IINPUTSERVICE_H
