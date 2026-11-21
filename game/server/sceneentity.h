//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The flex scene file manager. Factory "FlexSceneFileManager". Only this
//			game system is declared here.
//
//=============================================================================//

#ifndef SCENEENTITY_H
#define SCENEENTITY_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "tier0/utldict.h"

class CBaseFlex;
struct flexsettinghdr_t;

abstract_class IFlexSceneFileManager
{
public:
	// Returns the loaded .vfe file (flexsettinghdr_t), loading it when bAllowBlockingIO is set
	virtual const void *FindSceneFile( CBaseFlex *pInstance, const char *pFilename, bool bAllowBlockingIO ) = 0;
};

//-----------------------------------------------------------------------------
// Static instance, 72 bytes on both platforms. libserver.so repeats the
// FindSceneFile override at the end of the main table (slot 67).
//-----------------------------------------------------------------------------
class CFlexSceneFileManager : public CAutoGameSystem, public IFlexSceneFileManager
{
public:
	DECLARE_GAME_SYSTEM();

	// Loads the phoneme files ("phonemes", "phonemes_weak", "phonemes_strong"), then every
	// .vfe under "expressions"
	virtual bool Init() override;
	// Releases the loaded files and empties m_FileList
	virtual void Shutdown() override;

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// Loads every .vfe file under the folder (NULL: "expressions") and its subfolders
	virtual bool InitRecursive( const char *pFolder );

	// IFlexSceneFileManager
	virtual const void *FindSceneFile( CBaseFlex *pInstance, const char *pFilename, bool bAllowBlockingIO ) override;

public:
	// File name (allocated copy) -> loaded file
	CUtlDict< const flexsettinghdr_t *, int > m_FileList;
};

#endif // SCENEENTITY_H
