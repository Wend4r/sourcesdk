#ifndef CSTRIKE15_CS_GAMEMODERULES_H
#define CSTRIKE15_CS_GAMEMODERULES_H

#pragma once

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

class CNetworkSerializerClassInfo;

//-----------------------------------------------------------------------------
// Virtual table of CCSGameModeRules, the per-mode part of the game rules
// (CCSGameRules::m_pGameModeRules), and of the modes derived from it: CCSGameModeRules_Noop,
// CCSGameModeRules_Deathmatch and CCSGameModeRules_ArmsRace, which only override. The class
// carries a VTable suffix and declares virtual methods only: the game classes of these names
// are schema classes, so a module declares its own class with the members it needs and
// derives it from this one.
//
// Slot numbers in the Unk_ / Stub_ names are Windows indexes; on Linux the destructor takes
// two slots, so every slot after it is one further. The modes have no name source for the
// methods after the destructor but Think; each comment says what the bodies do. The parameters are not
// recovered: the declarations describe the slots, not how to call them.
//-----------------------------------------------------------------------------
abstract_class CCSGameModeRulesVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const = 0;

	virtual ~CCSGameModeRulesVTable() = 0;

	// True in the base, false in CCSGameModeRules_Noop
	virtual bool Unk_3() = 0;
	// Empty in the base. Deathmatch and arms race: the per-frame logic that ends the warmup
	// and restarts the round or the match. CCSGameRules::Think calls it. Named by its caller
	virtual void Think() = 0;
	// Empty in the base. Deathmatch and arms race: move the players to their teams when the
	// match or the round is reset
	virtual void Unk_5() = 0;
	// Empty in every mode
	virtual void Stub_6() = 0;
	// Empty in the base. Arms race resets its leader members to -1, deathmatch has its own reset
	virtual void Unk_7() = 0;
	// Empty in the base. Deathmatch: the reward for a kill (points, a chicken, a health shot);
	// arms race: the first connected player and the warmup
	virtual void Unk_8() = 0;
	// Empty in the base. Deathmatch and arms race: checks of the weapon (taser, grenades)
	virtual void Unk_9() = 0;
	// Empty in the base. Arms race: the kill and level-up announcements; deathmatch: its own
	// large handler
	virtual void Unk_10() = 0;
};

#endif // CSTRIKE15_CS_GAMEMODERULES_H
