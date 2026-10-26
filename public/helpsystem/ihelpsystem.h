//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Context help for the tools: help files, help widgets and help editing
//
//===========================================================================//

#ifndef IHELPSYSTEM_H
#define IHELPSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier1/utlvector.h"
#include "appframework/iappsystem.h"
#include "tier3/tier3.h"

class QObject;
class QWidget;
class IHelpDisplayWidget;
class IHelpChangeListener;
class CQHelpEditorWidget;
class CQHelpDisplayWidget;

// Where IHelpSystem::LoadHelpContext looks for resource/toolhelp_<name>_<language>.txt
enum HelpContextLocation_t
{
	// The MOD search path
	HELP_CONTEXT_LOCATION_MOD = 0,

	// The GAME search path
	HELP_CONTEXT_LOCATION_GAME = 1,

	// MOD for a "<prefix>/<name>" context name, passing <prefix> to the file lookup
	HELP_CONTEXT_LOCATION_MOD_PREFIXED = 2,
};

// The text of a help entry IHelpSystem::SetHelpEntryText replaces
enum HelpEntryField_t
{
	HELP_ENTRY_FIELD_NAME = 0,
	HELP_ENTRY_FIELD_SHORT_HELP = 1,
	HELP_ENTRY_FIELD_HELP = 2,
};

abstract_class IHelpSystem : public IAppSystem
{
public:
	// Loads resource/toolhelp_<name>_<language>.txt
	virtual void LoadHelpContext( const char *pContextName, HelpContextLocation_t nLocation ) = 0;

	// pGameData is a collection of entity classes (vector of classes first): loads the help contexts it lists and
	// sets the default name and help of "Element.<class>" and of each class variable in the help
	// context of the class
	virtual void RegisterElementHelp( void *pGameData ) = 0;

	virtual IHelpDisplayWidget *CreateHelpDisplayWidget( QWidget *pParent ) = 0;

	// Schedules the widget for deletion
	virtual void DestroyHelpDisplayWidget( IHelpDisplayWidget *pWidget ) = 0;

	virtual void AddHelpChangeListener( IHelpChangeListener *pListener ) = 0;
	virtual void RemoveHelpChangeListener( IHelpChangeListener *pListener ) = 0;

	virtual void SetObjectHelp( QObject *pObject, const char *pContextName, const char *pHelpId ) = 0;
	virtual bool GetObjectHelp( QObject *pObject, CUtlString &sContextName, CUtlString &sHelpId ) = 0;
	virtual void ClearObjectHelp( QObject *pObject ) = 0;

	// pAnnotation is shown in brackets to the right of the entry name; bShowHelpBinding appends the key
	// bound to the help command from the tool framework's input bind manager
	virtual bool GetHelpText( const char *pContextName, const char *pHelpId, const char *pAnnotation, CUtlString &sText, bool bShowHelpBinding ) = 0;

	virtual void EditHelp( QWidget *pParent, const char *pContextName, const char *pHelpId ) = 0;

	// Shows the entry in a help dialog
	virtual void ShowHelpDialog( QWidget *pParent, const char *pContextName, const char *pHelpId ) = 0;

	// Case-insensitive search of the entry's texts
	virtual bool HelpEntryContainsText( const char *pContextName, const char *pHelpId, const char *pSearch ) = 0;

	virtual void SetHelpEntryText( const char *pContextName, const char *pHelpId, HelpEntryField_t nField, const char *pText ) = 0;

	// Writes the context back to its help file
	virtual void SaveHelpContext( const char *pContextName ) = 0;

	// Falls back to the default name, then to the help id, when there is no up-to-date edited name
	virtual bool GetHelpEntryName( const char *pContextName, const char *pHelpId, CUtlString &sName ) = 0;

	// Default texts the edited texts are compared against to detect stale help
	virtual void SetHelpEntryDefaultText( const char *pContextName, const char *pHelpId, const char *pName, const char *pHelp, const char *pShortHelp ) = 0;

	virtual bool HasHelpEntry( const char *pContextName, const char *pHelpId ) = 0;

	virtual ~IHelpSystem() {}
};

class CHelpSystem : public CTier3AppSystem< IHelpSystem >
{
public:
	// Loaded help contexts, searched by name
	CUtlVector< void * > m_HelpContexts;

	// Widgets register themselves on construction
	CUtlVector< CQHelpEditorWidget * > m_HelpEditorWidgets;
	CUtlVector< CQHelpDisplayWidget * > m_HelpDisplayWidgets;

	CUtlVector< IHelpChangeListener * > m_HelpChangeListeners;
};

#endif // IHELPSYSTEM_H
