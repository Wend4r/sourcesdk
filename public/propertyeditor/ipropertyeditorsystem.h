//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Creates property editor widgets and keeps the attribute editor registry
//
//===========================================================================//

#ifndef IPROPERTYEDITORSYSTEM_H
#define IPROPERTYEDITORSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlsymbol.h"
#include "tier0/utlsymbollarge.h"
#include "tier1/utlvector.h"
#include "appframework/iappsystem.h"
#include "tier3/tier3.h"

class QWidget;
class IPropertyEditor;
class IPropertyAttributeEditorFactory;
class IPropertyCustomCommandFactory;
class CQPropertyEditor;

// Flags of IPropertyEditorSystem::CreatePropertyEditor; with both of the first two the editor has no
// attribute filter bar.
// No "(Property Name Filter)" box in the filter bar
#define PROPERTY_EDITOR_NO_NAME_FILTER ( 1 << 0 )
// No settings button (Show Hidden Attributes, Raw Edit Mode, Show Groups, expansion commands)
#define PROPERTY_EDITOR_NO_SETTINGS_MENU ( 1 << 1 )
// Hides the second label of the header row
#define PROPERTY_EDITOR_HIDE_HEADER_DETAIL ( 1 << 2 )
// The row list's height is fixed to the height of its rows instead of scrolling
#define PROPERTY_EDITOR_FIT_TO_ROWS ( 1 << 7 )
// The row list is drawn without a border
#define PROPERTY_EDITOR_NO_ROW_LIST_BORDER ( 1 << 8 )
// The row widgets are chained into the tab order after the editor itself
#define PROPERTY_EDITOR_TAB_ORDER_FROM_EDITOR ( 1 << 10 )

abstract_class IPropertyEditorSystem : public IAppSystem
{
public:
	// nFlags holds PROPERTY_EDITOR_* bits
	virtual IPropertyEditor *CreatePropertyEditor( QWidget *pParent, uint32 nFlags ) = 0;
	virtual void DestroyPropertyEditor( IPropertyEditor *pEditor ) = 0;

	// Act on every editor whose domain equals pDomain
	virtual void RefreshEditorsForDomain( void *pDomain ) = 0;
	virtual void RebuildEditorsForDomain( void *pDomain ) = 0;

	// Symbols of the expanded property groups, shared by every property editor
	virtual CUtlSymbolTableLarge *GetExpansionStateSymbols() = 0;

	// Factories are keyed by the name their first virtual returns
	virtual void RegisterAttributeEditorFactory( IPropertyAttributeEditorFactory *pFactory ) = 0;
	virtual void UnregisterAttributeEditorFactory( IPropertyAttributeEditorFactory *pFactory ) = 0;

	// Custom command factories, keyed by the name their first virtual returns
	virtual void RegisterCustomCommandFactory( IPropertyCustomCommandFactory *pFactory ) = 0;
	virtual IPropertyCustomCommandFactory *FindCustomCommandFactory( const char *pName ) = 0;
	virtual void UnregisterCustomCommandFactory( IPropertyCustomCommandFactory *pFactory ) = 0;

	// Whether the text parses as KV3 with a non-empty table at the root
	virtual bool IsNonEmptyKV3Table( const char *pKV3Text ) = 0;

	// Detaches the domain from every editor that shows it
	virtual void ClearEditorsDomain( void *pDomain ) = 0;

	virtual ~IPropertyEditorSystem() {}
};

class CPropertyEditorSystem : public CTier3AppSystem< IPropertyEditorSystem >
{
public:
	// Editors made by CreatePropertyEditor
	CUtlVector< CQPropertyEditor * > m_PropertyEditors;

	// Attribute editor factories, indexed by the symbol of their name
	CUtlVector< IPropertyAttributeEditorFactory * > m_AttributeEditorFactories;
	CUtlSymbolTable m_AttributeEditorFactoryNames;

	// Custom command factories, indexed by the symbol of their name
	CUtlVector< IPropertyCustomCommandFactory * > m_CustomCommandFactories;
	CUtlSymbolTable m_CustomCommandFactoryNames;

	// Returned by GetExpansionStateSymbols
	CUtlSymbolTableLarge m_ExpansionStateSymbols;
};

#endif // IPROPERTYEDITORSYSTEM_H
