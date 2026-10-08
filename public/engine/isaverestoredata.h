//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Holder of the save/restore state shared by the client and the server
//
//=============================================================================//

#ifndef ISAVERESTOREDATA_H
#define ISAVERESTOREDATA_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>

abstract_class ISaveRestoreDataMgr : public IAppSystem
{
public:
	virtual ~ISaveRestoreDataMgr() {}

	virtual void SetSaveRestoreData( void *pData ) = 0;
	virtual void *GetSaveRestoreData() = 0;
};

#endif // ISAVERESTOREDATA_H
