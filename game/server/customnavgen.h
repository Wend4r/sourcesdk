#ifndef _CUSTOM_NAV_GEN_H_
#define _CUSTOM_NAV_GEN_H_

#pragma once

#include "interfaces/interfaces.h"
#include "tier3/tier3.h"

class CNavMesh;
class IVPhysics2World;

class ICustomNavGen : public IAppSystem
{
public:
	virtual bool CreateAndLoadNavMeshFromVPK( CNavMesh *&pOutMesh, const char *pszMapPath ) = 0;
};

class CCustomNavGenSystem : public CTier3AppSystem< ICustomNavGen >
{
public:
};

#define CUSTOMNAVSYSTEM_INTERFACE_VERSION				"customnavsystem001"
DECLARE_TIER3_INTERFACE( ICustomNavGen, g_pCustomNavGen ); // "CSGO Compute AI Data" / "CustomNavBuild" section from gameinfo.gi

#endif // !defined( _CUSTOM_NAV_GEN_H_ )
