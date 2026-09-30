#ifndef CSTRIKE15_CS_CHAT_H
#define CSTRIKE15_CS_CHAT_H

#pragma once

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// Player chat, radio and pings.
//
// "say" and "say_team" are console commands a client may run. Dispatched with a
// command context for a player slot, they run the game's say handler for that
// player's controller, bots included: the text goes to every listener allowed to
// hear it as a SayText2 message with one of the format tokens below, the gamerules
// are told about the line and it is logged. A line is dropped if the controller's
// m_flLastPlayerTalkTime is less than CS_CHAT_FLOOD_INTERVAL seconds old.
//
// "player_ping" is a console command of the same kind that places a ping where
// the player is looking.
//
// Radio goes through the player's client command, not the console:
//	<name>								coverme, takepoint, holdpos, regroup, followme,
//										takingfire, go, fallback, sticktog, cheer, thanks,
//										compliment, report, roger, enemyspot, needbackup,
//										sectorclear, inposition, reportingin, getout,
//										negative, enemydown
//	playerradio <Radio.Sound> [text]	a Radio.* sound with its stock text, or with the
//										given text
//	playerchatwheel <CW.Sound> <any>	a chat wheel line, looked up by its CW.* sound
//	ignorerad							toggles ignoring radio
//-----------------------------------------------------------------------------

#define CS_CHAT_FLOOD_INTERVAL 0.25f

#define CS_CHAT_FORMAT_ALL "Cstrike_Chat_All"
#define CS_CHAT_FORMAT_ALL_DEAD "Cstrike_Chat_AllDead"
#define CS_CHAT_FORMAT_ALL_SPEC "Cstrike_Chat_AllSpec"
#define CS_CHAT_FORMAT_CT "Cstrike_Chat_CT"
#define CS_CHAT_FORMAT_CT_LOC "Cstrike_Chat_CT_Loc"
#define CS_CHAT_FORMAT_CT_DEAD "Cstrike_Chat_CT_Dead"
#define CS_CHAT_FORMAT_T "Cstrike_Chat_T"
#define CS_CHAT_FORMAT_T_LOC "Cstrike_Chat_T_Loc"
#define CS_CHAT_FORMAT_T_DEAD "Cstrike_Chat_T_Dead"
#define CS_CHAT_FORMAT_SPEC "Cstrike_Chat_Spec"

#endif // CSTRIKE15_CS_CHAT_H
