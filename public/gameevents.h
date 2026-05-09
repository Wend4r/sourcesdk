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

class CMsgVDebugGameSessionIDEvent_t   : public CNetMessagePB< GE_VDebugGameSessionIDEvent,   CMsgVDebugGameSessionIDEvent,   SG_EVENTS,   BUF_RELIABLE, true > {};
class CMsgPlaceDecalEvent_t            : public CNetMessagePB< GE_PlaceDecalEvent,            CMsgPlaceDecalEvent,            SG_DECALS,   BUF_RELIABLE, true > {};
class CMsgClearWorldDecalsEvent_t      : public CNetMessagePB< GE_ClearWorldDecalsEvent,      CMsgClearWorldDecalsEvent,      SG_DECALS,   BUF_RELIABLE, true > {};
class CMsgClearEntityDecalsEvent_t     : public CNetMessagePB< GE_ClearEntityDecalsEvent,     CMsgClearEntityDecalsEvent,     SG_DECALS,   BUF_RELIABLE, true > {};
class CMsgClearDecalsForEntityEvent_t  : public CNetMessagePB< GE_ClearDecalsForEntityEvent,  CMsgClearDecalsForEntityEvent,  SG_DECALS,   BUF_RELIABLE, true > {};
class CMsgSource1LegacyGameEventList_t : public CNetMessagePB< GE_Source1LegacyGameEventList, CMsgSource1LegacyGameEventList, SG_SIGNON,   BUF_RELIABLE, true > {};
class CMsgSource1LegacyListenEvents_t  : public CNetMessagePB< GE_Source1LegacyListenEvents,  CMsgSource1LegacyListenEvents,  SG_SIGNON,   BUF_RELIABLE, true > {};
class CMsgSource1LegacyGameEvent_t     : public CNetMessagePB< GE_Source1LegacyGameEvent,     CMsgSource1LegacyGameEvent,     SG_EVENTS,   BUF_RELIABLE, true > {};
class CMsgSosStartSoundEvent_t         : public CNetMessagePB< GE_SosStartSoundEvent,         CMsgSosStartSoundEvent,         SG_SOUNDS,   BUF_RELIABLE, true > {};
class CMsgSosStopSoundEvent_t          : public CNetMessagePB< GE_SosStopSoundEvent,          CMsgSosStopSoundEvent,          SG_SOUNDS,   BUF_RELIABLE, true > {};
class CMsgSosSetSoundEventParams_t     : public CNetMessagePB< GE_SosSetSoundEventParams,     CMsgSosSetSoundEventParams,     SG_SOUNDS,   BUF_RELIABLE, true > {};
class CMsgSosSetLibraryStackFields_t   : public CNetMessagePB< GE_SosSetLibraryStackFields,   CMsgSosSetLibraryStackFields,   SG_SOUNDS,   BUF_RELIABLE, true > {};
class CMsgSosStopSoundEventHash_t      : public CNetMessagePB< GE_SosStopSoundEventHash,      CMsgSosStopSoundEventHash,      SG_SOUNDS,   BUF_RELIABLE, true > {};

#endif // GAMEEVENTS_H
