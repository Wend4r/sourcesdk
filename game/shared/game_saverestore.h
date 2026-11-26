#ifndef GAME_SAVERESTORE_H
#define GAME_SAVERESTORE_H

#pragma once

#include "tier0/platform.h"

class CSaveRestoreData;
class CGameSaveRestoreInfo;
class CSaveRestoreLoggingInfo;

//-----------------------------------------------------------------------------
// The state the save and the restore of a game share: the first base of CSave (ISave sits after
// it) and of CRestore (IRestore after it). 40 bytes on both platforms. Only the virtual
// destructor is declared, its only virtual method.
//-----------------------------------------------------------------------------
abstract_class CSaveRestoreShared
{
public:
	virtual ~CSaveRestoreShared() {}

public:
	// The logging of the save restore game system, when there is one (ISave / IRestore
	// GetSaveRestoreLogging)
	CSaveRestoreLoggingInfo *m_pSaveRestoreLogging;
	// GetSaveRestoreData
	CSaveRestoreData *m_pData;
	// The game save restore info part of m_pData (GetGameSaveRestoreInfo)
	CGameSaveRestoreInfo *m_pGameInfo;
	// Named by what it holds: the skip fields mode of a save, the global mode of a restore (the same
	// setter and getter serve both)
	bool m_bSkipFieldsMode;
};

#endif // GAME_SAVERESTORE_H
