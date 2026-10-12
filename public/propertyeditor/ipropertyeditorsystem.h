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
class CQPropertyEditor;

abstract_class IPropertyEditorSystem : public IAppSystem
{
public:
	virtual IPropertyEditor *CreatePropertyEditor( QWidget *pParent, uint32 nUnknown ) = 0;
	virtual void DestroyPropertyEditor( IPropertyEditor *pEditor ) = 0;

	// Act on every editor whose domain equals pDomain
	virtual void RefreshEditorsForDomain( void *pDomain ) = 0;
	virtual void RebuildEditorsForDomain( void *pDomain ) = 0;

	virtual CUtlSymbolTableLarge *unk015() = 0;

	// Factories are keyed by the name their first virtual returns
	virtual void RegisterAttributeEditorFactory( IPropertyAttributeEditorFactory *pFactory ) = 0;
	virtual void UnregisterAttributeEditorFactory( IPropertyAttributeEditorFactory *pFactory ) = 0;

	virtual void unk018( void *pObject ) = 0;
	virtual void *unk019( const char *pName ) = 0;
	virtual void unk020( void *pObject ) = 0;

	virtual bool unk021( const char *pKV3Text ) = 0;

	// Detaches the domain from every editor that shows it
	virtual void ClearEditorsDomain( void *pDomain ) = 0;

	virtual ~IPropertyEditorSystem() {}
};

class CPropertyEditorSystem : public CTier3AppSystem< IPropertyEditorSystem >
{
public:
	CUtlVector< CQPropertyEditor * > m_vecUnk028;

	// Attribute editor factories, indexed by the symbol of their name
	CUtlVector< IPropertyAttributeEditorFactory * > m_vecUnk040;
	CUtlSymbolTable m_unk058;

	// Objects of unk018, indexed by the symbol of their name
	CUtlVector< void * > m_vecUnk0B8;
	CUtlSymbolTable m_unk0D0;

	CUtlSymbolTableLarge m_unk130;
};

#endif // IPROPERTYEDITORSYSTEM_H
