//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A volume of the navigation mesh (CNavVolumeMarkupVolume and the other volume kinds).
//
//=============================================================================//

#ifndef NAV_VOLUME_H
#define NAV_VOLUME_H
#ifdef _WIN32
#pragma once
#endif

#include <functional>

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "schemasystem/schematypes.h"
#include "nav_area.h"

struct Extent;
struct NavCellCoord_t;

//-----------------------------------------------------------------------------
// CNavVolume is a schema class; this class declares its virtual methods only (see
// playerpawn_component.h for the VTable pattern).
//-----------------------------------------------------------------------------
abstract_class CNavVolumeVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual ~CNavVolumeVTable() = 0;
	virtual void ParamsChanged() = 0;
	virtual CNavArea *GetRandomArea() const = 0;
	virtual void GetOrFindAreasOverlapping( CUtlVector< CNavArea * > &areas ) const = 0;
	virtual void ForAreasOverlapping( std::function< NavSearchResult_t( CNavArea & ) > &func ) const = 0;
	virtual bool ContainsArea( const CNavArea &area ) const = 0;
	virtual void GetExtent( Extent *pExtent ) const = 0;
	virtual bool OverlapsExtent( const Extent &extent ) const = 0;
	virtual void CalcCellList( CUtlVectorFixedGrowable< NavCellCoord_t, 1024 > &cells ) const = 0;
	virtual void DrawDebug( float flDuration ) const = 0;
};

#endif // NAV_VOLUME_H
