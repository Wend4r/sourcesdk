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

class IInputHandler;
class CUtlBuffer;

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

	virtual void SetBinding( ButtonCode_t code, CSplitScreenSlot nSlot, const char *pszBinding, bool bTemporary, int nChord ) = 0;
	virtual const char *GetBinding( ButtonCode_t code, CSplitScreenSlot nSlot, int nChord ) = 0;
	virtual void SetBinding( AnalogCode_t code, CSplitScreenSlot nSlot, const char *pszBinding, bool bTemporary, int nChord ) = 0;
	virtual const char *GetBinding( AnalogCode_t code, CSplitScreenSlot nSlot, int nChord ) = 0;

	virtual const char *NameForBinding( const char *pszBinding, CSplitScreenSlot nSlot, int nStartCount, int nLookupFlags ) = 0;

	// Whether code is bound to toggleconsole (no modifiers held)
	virtual bool IsToggleConsoleButton( ButtonCode_t code, int nModifiers ) = 0;
	// KEY_ESCAPE without modifiers
	virtual bool IsEscapeButton( ButtonCode_t code, int nModifiers ) = 0;

	virtual bool HasMouseFocus() = 0;

	virtual void RegisterInputValue( void *pInputValue ) = 0;
	virtual void UnregisterInputValue( void *pInputValue ) = 0;
	virtual void ForEachInputValue( void *pDelegate ) = 0;

	// The key binding processing does nothing while it is disabled (Dota 2 2018 name)
	virtual void EnableKeyBindingSystem( bool bEnable ) = 0;
	// Named by what it does: while set, the commands that execute a script file over time and perftest.cfg do
	// nothing; cleared when the service deactivates
	virtual void BlockTimedScriptExecution( bool bBlock ) = 0;
	// Counter incremented by each client input processing pass (Dota 2 2018 name)
	virtual int GetFrameCount() = 0;

	// Counted lock of all command buffers
	virtual void LockCommandBuffers( bool bLock ) = 0;

	virtual void LoadCheatCodes( CUtlBuffer &buf ) = 0;
	virtual void ClearCheatCodes() = 0;

	// nSlot < 0 clears every slot
	virtual void UnbindAllKeys( int nSlot ) = 0;

	virtual void AddBindingListener( void *pListener ) = 0;
	virtual void RemoveBindingListener( void *pListener ) = 0;

	virtual bool WasButtonPressed( ButtonCode_t code ) = 0;
	virtual bool WasButtonDoubleClicked( ButtonCode_t code ) = 0;
	virtual bool WasButtonReleased( ButtonCode_t code ) = 0;
	virtual bool IsButtonDown( ButtonCode_t code ) = 0;
	// IsButtonDown, except that the shift, alt, control and win keys are answered from a stored modifier mask while the modifier override is active
	virtual bool IsButtonDownWithModifierOverride( ButtonCode_t code ) = 0;

	// Cursor position in the engine window
	virtual void GetCursorPosition( float *pX, float *pY ) = 0;
	virtual void SetCursorPosition( float x, float y ) = 0;

	virtual CUtlString GetUserSettingsFileName( const char *pszName, bool *pbExists, bool bNoCfgDir ) = 0;
	virtual void AppendChordModifiers( CUtlString &sName, int nChord ) = 0;

	// Named by what it does: while set, the convar processing logs the values it sets
	virtual void SetPrintConVarChanges( bool bPrint ) = 0;
	// AMNOTE: Empty on both platforms; no source names them (Dota 2018 had the cursor confinement methods
	// elsewhere in the table)
	virtual void EmptyInputCallback1() = 0;
	virtual void EmptyInputCallback2() = 0;

	// 8 with Engine2/AllowKeyChordBindings, 1 otherwise
	virtual int GetNumChords() = 0;

	// Named by what it does: stores both pointers in two parallel lists
	virtual void RegisterInputCallback( void *pCallback, void *pContext ) = 0;
};

#endif // IINPUTSERVICE_H
