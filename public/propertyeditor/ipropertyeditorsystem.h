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

abstract_class IPropertyEditorSystem : public IAppSystem
{
public:
	virtual IPropertyEditor *CreatePropertyEditor( QWidget *pParent, uint32 nUnknown ) = 0;
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
