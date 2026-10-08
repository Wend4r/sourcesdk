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

abstract_class IHelpSystem : public IAppSystem
{
public:
	virtual void LoadHelpContext( const char *pContextName, int nUnknown ) = 0;

	virtual void Unk_RegisterElementHelp( void *p ) = 0;

	virtual IHelpDisplayWidget *CreateHelpDisplayWidget( QWidget *pParent ) = 0;

	// Schedules the widget for deletion
	virtual void DestroyHelpDisplayWidget( IHelpDisplayWidget *pWidget ) = 0;

	virtual void AddHelpChangeListener( IHelpChangeListener *pListener ) = 0;
	virtual void RemoveHelpChangeListener( IHelpChangeListener *pListener ) = 0;

	virtual void SetObjectHelp( QObject *pObject, const char *pContextName, const char *pHelpId ) = 0;
	virtual bool GetObjectHelp( QObject *pObject, CUtlString &sContextName, CUtlString &sHelpId ) = 0;
	virtual void ClearObjectHelp( QObject *pObject ) = 0;

	virtual bool GetHelpText( const char *pContextName, const char *pHelpId, int nUnknown, CUtlString &sText, bool bUnknown ) = 0;

	virtual void EditHelp( QWidget *pParent, const char *pContextName, const char *pHelpId ) = 0;

	// Shows the entry in a help dialog
	virtual void ShowHelpDialog( QWidget *pParent, const char *pContextName, const char *pHelpId ) = 0;

	// Case-insensitive search of the entry's texts
	virtual bool HelpEntryContainsText( const char *pContextName, const char *pHelpId, const char *pSearch ) = 0;

	// nField: 0 name, 1 short help, 2 help
	virtual void SetHelpEntryText( const char *pContextName, const char *pHelpId, int nField, const char *pText ) = 0;

	// Writes the context back to its help file
	virtual void SaveHelpContext( const char *pContextName ) = 0;

	virtual bool unk026( const char *pContextName, const char *pHelpId, const char *pDefault, CUtlString &sResult ) = 0;

	// Sets three text fields of the entry at once
	virtual void unk027( const char *pContextName, const char *pHelpId, const char *pText1, const char *pText2, const char *pText3 ) = 0;

	virtual bool HasHelpEntry( const char *pContextName, const char *pHelpId ) = 0;

	virtual ~IHelpSystem() {}
};

#endif // IHELPSYSTEM_H
