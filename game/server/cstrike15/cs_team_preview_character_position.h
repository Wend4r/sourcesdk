#ifndef CSTRIKE15_CS_TEAM_PREVIEW_CHARACTER_POSITION_H
#define CSTRIKE15_CS_TEAM_PREVIEW_CHARACTER_POSITION_H

#pragma once

#include "../baseentity.h"

//-----------------------------------------------------------------------------
// CCSGO_TeamPreviewCharacterPosition adds 2 methods to CBaseEntity: Linux slots 239-240, Windows 238-239.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CCSGO_TeamPreviewCharacterPositionVTable : public TBase
{
public:
	// Pure: no own implementation. The team select positions return 3, the team intro positions 7, the wingman intro positions 8, the rush intro positions 9; no caller on the server
	virtual int Unk_238() = 0;
	// Returns false, the team select positions too; the intro positions (team, wingman, rush) return true. No caller on the server
	virtual bool Unk_239() = 0;
};

#endif // CSTRIKE15_CS_TEAM_PREVIEW_CHARACTER_POSITION_H
