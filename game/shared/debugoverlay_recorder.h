//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Base of the debug overlay windows that record and play back
//          events ("Grenades").
//
//=============================================================================//

#ifndef DEBUGOVERLAY_RECORDER_H
#define DEBUGOVERLAY_RECORDER_H
#ifdef _WIN32
#pragma once
#endif

#include "debugoverlay_window.h"

#include <tier0/utlvector.h>

// Polymorphic, 56 bytes (RTTI name); the recorder owns one
class CDebugOverlayFilterBase;
// The element types of the recorder's two vectors (24 bytes with a CUtlString at 16; 8-byte pointers
// to records with an int at 12 and a pointer at 16) are not identified yet
struct DebugOverlayRecorderText_t;
struct DebugOverlayRecorderEntry_t;

//-----------------------------------------------------------------------------
// Abstract, 168 bytes.
// No DECLARE_GAME_SYSTEM of its own. 17 own virtual methods (Linux 72-88, Windows 71-87); no source
// names them: the names say what the bodies do, "EmptyHook" ones are empty in the base and in
// CDebugOverlayGrenades.
//-----------------------------------------------------------------------------
abstract_class CDebugOverlayRecorderBase : public CDebugOverlayWindow
{
public:
	CDebugOverlayRecorderBase( const char *pszName );
	virtual ~CDebugOverlayRecorderBase();

	// Draws the recorder window while it is shown
	virtual void DrawOverlay( void *pDebugOverlay ) override;
	// Read / write m_flTopBottomSplitter ("TopBottomSplitter") in m_pSettings
	virtual void LoadSettings() override;
	virtual void SaveSettings() override;

	// Turns the recorder on or off (Grenades: its convar, then host_writeconfig); empty here
	virtual void SetEnabled( bool bEnabled );
	// Returns false here (Grenades: its convar); DrawOverlay caches it in m_bEnabled
	virtual bool IsEnabled();
	virtual void EmptyHook74();
	virtual void EmptyHook75();
	// Returns false
	virtual bool ReturnsFalse76();
	virtual void EmptyHook77();
	// Draws the recorder's UI
	virtual void DrawRecorderUI( void *pUI );
	virtual void EmptyHook79();
	// Return true
	virtual bool ReturnsTrue80();
	virtual bool ReturnsTrue81();
	// Returns false
	virtual bool ReturnsFalse82();
	// The id of the recorded channel (pure here; Grenades returns 7), passed with StartPlayback
	virtual int GetChannelId() = 0;
	// True when an entry or one of its children matches the filter text or the id
	virtual bool PassesFilter( const void *pEntry, const char *pszFilter, uint32 nId );
	virtual void EmptyHook85();
	// Sets m_bPlaying, clears m_pPlaybackEntry and m_pSelectedEntry, tells the recording system
	virtual void StartPlayback( int nFrame );
	virtual void DrawToolbar( void *pUI );
	// Draws the three column checkboxes of the entry list
	virtual void DrawColumnOptions( void *pUI, int nColumn );

public:
	// Named by what the constructor sets (no other server code touches them directly)
	int m_nClearedOnConstruction;
	int m_nOneOnConstruction;
	int m_nSelectedIndex;
	bool m_bClearedOnConstruction;
	int m_nHoveredIndex;
	// 100 after construction
	int m_nMaxEntries;
	// The splitter of the window ("TopBottomSplitter", 0.35)
	float m_flTopBottomSplitter;
	int m_nClearedOnConstruction2;
	// Set by StartPlayback
	bool m_bPlaying;
	// IsEnabled, cached by DrawOverlay
	bool m_bEnabled;
	// Cleared by StartPlayback
	void *m_pPlaybackEntry;
	CUtlVector< DebugOverlayRecorderText_t > m_Texts;
	CUtlVector< DebugOverlayRecorderEntry_t * > m_Entries;
	// Deleted by the destructor
	CDebugOverlayFilterBase *m_pFilter;
	int m_nNotConstructed;
	int m_nClearedOnConstruction3;
	void *m_pNotConstructed;
	// Cleared by StartPlayback
	void *m_pSelectedEntry;
};

#endif // DEBUGOVERLAY_RECORDER_H
