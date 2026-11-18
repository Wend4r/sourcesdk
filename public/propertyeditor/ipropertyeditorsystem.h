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
#include "appframework/iappsystem.h"

class QWidget;
class IPropertyEditor;
class IPropertyAttributeEditorFactory;

// A property editor shows the objects of a domain ( IPropertyEditor slot 4 sets it, slot 6 returns it ).
// The factories are keyed by the name their first virtual returns.
abstract_class IPropertyEditorSystem : public IAppSystem
{
public:
	// nFlags are the creation flags of CQPropertyEditor ( stored as they are; bits 0-2 pick the layout )
	virtual IPropertyEditor *CreatePropertyEditor( QWidget *pParent, uint32 nFlags ) = 0;
	virtual void DestroyPropertyEditor( IPropertyEditor *pEditor ) = 0;

	// Named by what they do: refresh ( latent while an editor is being edited ) or rebuild every editor
	// that shows pDomain
	virtual void RefreshEditorsForDomain( void *pDomain ) = 0;
	virtual void RebuildEditorsForDomain( void *pDomain ) = 0;

	// Named by what it returns: a string-keyed map owned by the system that every property item keeps a
	// pointer to
	virtual void *GetPropItemSharedData() = 0;

	virtual void RegisterAttributeEditorFactory( IPropertyAttributeEditorFactory *pFactory ) = 0;
	virtual void UnregisterAttributeEditorFactory( IPropertyAttributeEditorFactory *pFactory ) = 0;

	// Named by how they are used: factories of the extra button widgets of an attribute row ( found by the
	// button's factory name; their second virtual creates the widget )
	virtual void RegisterAttributeButtonFactory( void *pFactory ) = 0;
	virtual void *FindAttributeButtonFactory( const char *pName ) = 0;
	virtual void UnregisterAttributeButtonFactory( void *pFactory ) = 0;

	// Named by what it does: true when the text parses as a KV3 table with at least one member
	virtual bool IsNonEmptyKV3Table( const char *pKV3Text ) = 0;

	// Named by what it does: every editor that shows pDomain gets a NULL domain
	virtual void DetachEditorsFromDomain( void *pDomain ) = 0;

	virtual ~IPropertyEditorSystem() {}
};

#endif // IPROPERTYEDITORSYSTEM_H
