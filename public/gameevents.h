//========= Copyright © Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H

#ifdef _WIN32
#pragma once
#endif

#include <networksystem/netmessage.h>
#include <gameevents.pb.h>

class CMsgSource1LegacyGameEventList_t : public CNetMessagePB< GE_Source1LegacyGameEventList, CMsgSource1LegacyGameEventList, SG_SIGNON > {};
class CMsgSource1LegacyListenEvents_t : public CNetMessagePB< GE_Source1LegacyListenEvents, CMsgSource1LegacyListenEvents, SG_SIGNON > {};
class CMsgSource1LegacyGameEvent_t : public CNetMessagePB< GE_Source1LegacyGameEvent, CMsgSource1LegacyGameEvent, SG_EVENTS > {};

class CMsgSosStartSoundEvent_t : public CNetMessagePB< GE_SosStartSoundEvent, CMsgSource1LegacyGameEvent, SG_SOUNDS > {};
class CMsgSosStopSoundEvent_t : public CNetMessagePB< GE_SosStopSoundEvent, CMsgSosStopSoundEvent, SG_SOUNDS > {};
class CMsgSosSetSoundEventParams_t : public CNetMessagePB< GE_SosSetSoundEventParams, CMsgSosSetSoundEventParams, SG_SOUNDS > {};
class CMsgSosSetLibraryStackFields_t : public CNetMessagePB< GE_SosSetLibraryStackFields, CMsgSosSetLibraryStackFields, SG_SOUNDS > {};
class CMsgSosStopSoundEventHash_t : public CNetMessagePB< GE_SosStopSoundEventHash, CMsgSosStopSoundEventHash, SG_SOUNDS > {};

#endif // GAMEEVENTS_H
