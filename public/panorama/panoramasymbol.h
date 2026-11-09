//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Names in the panorama symbol table (events, panel types, properties)
//
//===========================================================================//

#ifndef PANORAMASYMBOL_H
#define PANORAMASYMBOL_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlsymbol.h"

namespace panorama
{

// An index into the UI engine's symbol table, made by IUIEngine::MakeSymbol. Trivially copyable: the
// engine passes it by value in a register on both platforms.
class CPanoramaSymbol
{
public:
	CPanoramaSymbol() : m_Id( UTL_INVAL_SYMBOL ) {}
	explicit CPanoramaSymbol( UtlSymId_t id ) : m_Id( id ) {}

	bool operator==( const CPanoramaSymbol &other ) const { return m_Id == other.m_Id; }
	bool operator!=( const CPanoramaSymbol &other ) const { return m_Id != other.m_Id; }
	bool operator<( const CPanoramaSymbol &other ) const { return m_Id < other.m_Id; }

	bool IsValid() const { return m_Id != UTL_INVAL_SYMBOL; }
	operator UtlSymId_t() const { return m_Id; }

private:
	UtlSymId_t m_Id;
};

} // namespace panorama

#endif // PANORAMASYMBOL_H
