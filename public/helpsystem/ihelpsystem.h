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

abstract_class IHelpSystem : public IAppSystem
{
public:
	virtual void LoadHelpContext( const char *pContextName, int nUnknown ) = 0;

	virtual void RegisterElementHelp( void *p ) = 0;

	virtual IHelpDisplayWidget *CreateHelpDisplayWidget( QWidget *pParent ) = 0;

	// Schedules the widget for deletion
	virtual void DestroyHelpDisplayWidget( IHelpDisplayWidget *pWidget ) = 0;

	virtual void AddHelpChangeListener( IHelpChangeListener *pListener ) = 0;
	virtual void RemoveHelpChangeListener( IHelpChangeListener *pListener ) = 0;

	virtual void SetObjectHelp( QObject *pObject, const char *pContextName, const char *pHelpId ) = 0;
	virtual bool GetObjectHelp( QObject *pObject, CUtlString &sContextName, CUtlString &sHelpId ) = 0;
	virtual void ClearObjectHelp( QObject *pObject ) = 0;

	// pUnknown is shown in brackets to the right of the entry name
	virtual bool GetHelpText( const char *pContextName, const char *pHelpId, const char *pUnknown, CUtlString &sText, bool bUnknown ) = 0;

	virtual void EditHelp( QWidget *pParent, const char *pContextName, const char *pHelpId ) = 0;

	// Shows the entry in a help dialog
	virtual void ShowHelpDialog( QWidget *pParent, const char *pContextName, const char *pHelpId ) = 0;

	// Case-insensitive search of the entry's texts
	virtual bool HelpEntryContainsText( const char *pContextName, const char *pHelpId, const char *pSearch ) = 0;

	// nField: 0 name, 1 short help, 2 help
	virtual void SetHelpEntryText( const char *pContextName, const char *pHelpId, int nField, const char *pText ) = 0;

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
