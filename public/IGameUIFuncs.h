//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef IGAMEUIFUNCS_H
#define IGAMEUIFUNCS_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "inputsystem/buttoncode.h"
#include "tier0/utlvector.h"

struct RenderDisplayMode_t;

abstract_class IGameUIFuncs : public IAppSystem
{
public:
	virtual const char	*GetBindingForButtonCode( ButtonCode_t code ) = 0;
	virtual ButtonCode_t GetButtonCodeForBind( const char *pBind, int userId = -1 ) = 0;
	// Display modes of an output of the current adapter, see IRenderDeviceMgr::GetAllDisplayModes
	virtual void		GetVideoModes( CUtlVector< RenderDisplayMode_t > &modes, int nOutput, bool bCommonWindowed ) = 0;
	virtual void		GetDesktopResolution( int &width, int &height ) = 0;
	virtual void		GetMonitorResolution( int nMonitor, int &width, int &height ) = 0;
	virtual bool		IsConnectedToVACSecureServer() = 0;
};

#define VENGINE_GAMEUIFUNCS_VERSION "VENGINE_GAMEUIFUNCS_VERSION005"

#endif // IGAMEUIFUNCS_H
