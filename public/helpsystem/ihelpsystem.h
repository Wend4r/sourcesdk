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
#include "appframework/iappsystem.h"

class QObject;
class QWidget;
class IHelpDisplayWidget;
class IHelpChangeListener;

// A help entry keeps three texts ( name, short help, help ) twice: the defaults registered by code and the
// edited texts of the help file ( resource/toolhelp_<context>_<language>.txt ).
abstract_class IHelpSystem : public IAppSystem
{
public:
	// nSource: 0 loads the help file from the "MOD" path, 1 from the "GAME" path, 2 loads no file ( help
	// registered by code only )
	virtual void LoadHelpContext( const char *pContextName, int nSource ) = 0;

	// Named by what it does: registers the "Element.<type>" and "Attribute.<type>.<attribute>" entries of
	// every element type of the collection, with their descriptions as default texts
	virtual void RegisterElementHelp( const void *pElementTypes ) = 0;

	virtual IHelpDisplayWidget *CreateHelpDisplayWidget( QWidget *pParent ) = 0;

	// Schedules the widget for deletion
	virtual void DestroyHelpDisplayWidget( IHelpDisplayWidget *pWidget ) = 0;

	virtual void AddHelpChangeListener( IHelpChangeListener *pListener ) = 0;
	virtual void RemoveHelpChangeListener( IHelpChangeListener *pListener ) = 0;

	virtual void SetObjectHelp( QObject *pObject, const char *pContextName, const char *pHelpId ) = 0;
	virtual bool GetObjectHelp( QObject *pObject, CUtlString &sContextName, CUtlString &sHelpId ) = 0;
	virtual void ClearObjectHelp( QObject *pObject ) = 0;

	// Formats the entry as HTML; pTag is shown as "[pTag]" next to the name, bAppendHelpKeyHint adds the
	// "(Help: <key>)" line
	virtual bool GetHelpText( const char *pContextName, const char *pHelpId, const char *pTag, CUtlString &sText, bool bAppendHelpKeyHint ) = 0;

	virtual void EditHelp( QWidget *pParent, const char *pContextName, const char *pHelpId ) = 0;

	// Shows the entry in a help dialog
	virtual void ShowHelpDialog( QWidget *pParent, const char *pContextName, const char *pHelpId ) = 0;

	// Case-insensitive search of the entry's texts
	virtual bool HelpEntryContainsText( const char *pContextName, const char *pHelpId, const char *pSearch ) = 0;

	// nField: 0 name, 1 short help, 2 help ( the edited texts )
	virtual void SetHelpEntryText( const char *pContextName, const char *pHelpId, int nField, const char *pText ) = 0;

	// Writes the context back to its help file
	virtual void SaveHelpContext( const char *pContextName ) = 0;

	// Named by what it does: the edited name of the entry ( true ), else its default name or id, or pDefault
	// when the entry does not exist ( false )
	virtual bool GetHelpEntryName( const char *pContextName, const char *pHelpId, const char *pDefault, CUtlString &sName ) = 0;

	// Named by what it does: sets the three default texts of the entry
	virtual void SetDefaultHelpEntryTexts( const char *pContextName, const char *pHelpId, const char *pName, const char *pHelp, const char *pShortHelp ) = 0;

	virtual bool HasHelpEntry( const char *pContextName, const char *pHelpId ) = 0;

	virtual ~IHelpSystem() {}
};

#endif // IHELPSYSTEM_H
