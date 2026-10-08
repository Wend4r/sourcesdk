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

abstract_class IPropertyEditorSystem : public IAppSystem
{
public:
	virtual IPropertyEditor *CreatePropertyEditor( QWidget *pParent, uint32 nUnknown ) = 0;
	virtual void DestroyPropertyEditor( IPropertyEditor *pEditor ) = 0;

	// Act on every editor whose owner object equals pOwner
	virtual void unk013( void *pOwner ) = 0;
	virtual void unk014( void *pOwner ) = 0;

	virtual void *unk015() = 0;

	// Factories are keyed by the name their first virtual returns
	virtual void RegisterAttributeEditorFactory( IPropertyAttributeEditorFactory *pFactory ) = 0;
	virtual void UnregisterAttributeEditorFactory( IPropertyAttributeEditorFactory *pFactory ) = 0;

	virtual void unk018( void *pObject ) = 0;
	virtual void *unk019( const char *pName ) = 0;
	virtual void unk020( void *pObject ) = 0;

	virtual bool unk021( const char *pKV3Text ) = 0;

	virtual void unk022( void *pOwner ) = 0;

	virtual ~IPropertyEditorSystem() {}
};

#endif // IPROPERTYEDITORSYSTEM_H
