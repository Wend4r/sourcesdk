//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Model lookups shared by the client and the server
//
//=============================================================================//

#ifndef IGAMEMODELINFO_H
#define IGAMEMODELINFO_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>
#include <resourcefile/resourcehandle.h>
#include <mathlib/vector.h>

class CModel;

class InfoForResourceTypeCModel
{
public:
	using RuntimeClass_t = CModel;
};

using HModelWeak = CWeakHandle< InfoForResourceTypeCModel >;

abstract_class IGameModelInfo : public IAppSystem
{
public:
	virtual ~IGameModelInfo() {}

	virtual HModelWeak FindModel( const char *pszModelName ) = 0;

	virtual bool GetModelName( ResourceHandle_t hModel, char *pszBuffer, int nBufferLen ) = 0;

	virtual void GetModelBounds( ResourceHandle_t hModel, Vector *pMins, Vector *pMaxs ) = 0;

	virtual void SetPropScreenWidthRange( float flMinPropScreenWidth, float flMaxPropScreenWidth ) = 0;
	virtual void GetPropScreenWidthRange( float *pMinPropScreenWidth, float *pMaxPropScreenWidth ) = 0;
};

#endif // IGAMEMODELINFO_H
