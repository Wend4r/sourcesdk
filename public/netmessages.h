//========= Copyright © 1996-2018, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef NETMESSAGES_H
#define NETMESSAGES_H

#ifdef _WIN32
#pragma once
#endif

#include "inetchannelinfo.h"
#include "networksystem/netmessage.h"

#include <netmessages.pb.h>
#include <networkbasetypes.pb.h>
#include <networksystem_protomessages.pb.h>

class NetMessageSplitscreenUserChanged_t :   public CNetMessagePB< -1, NetMessageSplitscreenUserChanged, SG_SYSTEM > {};
class NetMessagePacketStart_t :              public CNetMessagePB< -1, NetMessagePacketStart, SG_SYSTEM > {};
class NetMessagePacketEnd_t :                public CNetMessagePB< -1, NetMessagePacketEnd, SG_SYSTEM > {};
class NetMessageConnectionClosed_t :         public CNetMessagePB< -1, NetMessageConnectionClosed, SG_SYSTEM > {};
class NetMessageConnectionCrashed_t :        public CNetMessagePB< -1, NetMessageConnectionCrashed, SG_SYSTEM > {};

class CNETMsg_Tick_t :                       public CNetMessagePB< net_Tick, CNETMsg_Tick, SG_ENGINE, BUF_UNRELIABLE > {};
class CNETMsg_StringCmd_t :                  public CNetMessagePB< net_StringCmd, CNETMsg_StringCmd, SG_STRINGCMD > {};
class CNETMsg_SetConVar_t :                  public CNetMessagePB< net_SetConVar, CNETMsg_SetConVar, SG_STRINGCMD > {};
class CNETMsg_SignonState_t :                public CNetMessagePB< net_SignonState, CNETMsg_SignonState, SG_SIGNON > {};
class CNETMsg_SpawnGroup_Load_t :            public CNetMessagePB< net_SpawnGroup_Load, CNETMsg_SpawnGroup_Load, SG_SPAWNGROUPS > {};
class CNETMsg_SpawnGroup_ManifestUpdate_t :  public CNetMessagePB< net_SpawnGroup_ManifestUpdate, CNETMsg_SpawnGroup_ManifestUpdate, SG_SPAWNGROUPS > {};
class CNETMsg_SpawnGroup_SetCreationTick_t : public CNetMessagePB< net_SpawnGroup_SetCreationTick, CNETMsg_SpawnGroup_SetCreationTick, SG_SPAWNGROUPS > {};
class CNETMsg_SpawnGroup_Unload_t :          public CNetMessagePB< net_SpawnGroup_Unload, CNETMsg_SpawnGroup_Unload, SG_SPAWNGROUPS > {};
class CNETMsg_SpawnGroup_LoadCompleted_t :   public CNetMessagePB< net_SpawnGroup_LoadCompleted, CNETMsg_SpawnGroup_LoadCompleted, SG_SPAWNGROUPS > {};
class CNETMsg_DebugOverlay_t :               public CNetMessagePB< net_DebugOverlay, CNETMsg_DebugOverlay, SG_ENGINE > {};

class CCLCMsg_ClientInfo_t :                 public CNetMessagePB< clc_ClientInfo, CCLCMsg_ClientInfo, SG_SIGNON > {};
class CCLCMsg_Move_t :                       public CNetMessagePB< clc_Move, CCLCMsg_Move, SG_MOVE, BUF_UNRELIABLE > {};
class CCLCMsg_VoiceData_t :                  public CNetMessagePB< clc_VoiceData, CCLCMsg_VoiceData, SG_VOICE > {};
class CCLCMsg_BaselineAck_t :                public CNetMessagePB< clc_BaselineAck, CCLCMsg_BaselineAck, SG_ENGINE > {};
class CCLCMsg_RespondCvarValue_t :           public CNetMessagePB< clc_RespondCvarValue, CCLCMsg_RespondCvarValue > {};
class CCLCMsg_LoadingProgress_t :            public CNetMessagePB< clc_LoadingProgress, CCLCMsg_LoadingProgress, SG_SIGNON > {};
class CCLCMsg_SplitPlayerConnect_t :         public CNetMessagePB< clc_SplitPlayerConnect, CCLCMsg_SplitPlayerConnect, SG_SIGNON > {};
class CCLCMsg_SplitPlayerDisconnect_t :      public CNetMessagePB< clc_SplitPlayerDisconnect, CCLCMsg_SplitPlayerDisconnect, SG_SIGNON > {};
class CCLCMsg_ServerStatus_t :               public CNetMessagePB< clc_ServerStatus, CCLCMsg_ServerStatus > {};
class CCLCMsg_RequestPause_t :               public CNetMessagePB< clc_RequestPause, CCLCMsg_RequestPause > {};
class CCLCMsg_CmdKeyValues_t :               public CNetMessagePB< clc_CmdKeyValues, CCLCMsg_CmdKeyValues, SG_STRINGCMD > {};
class CCLCMsg_RconServerDetails_t :          public CNetMessagePB< clc_RconServerDetails, CCLCMsg_RconServerDetails > {};
class CCLCMsg_HltvReplay_t :                 public CNetMessagePB< clc_HltvReplay, CCLCMsg_HltvReplay, SG_HLTVREPLAY > {};
class CCLCMsg_Diagnostic_t :                 public CNetMessagePB< clc_Diagnostic, CCLCMsg_Diagnostic > {};

class CSVCMsg_ServerInfo_t :                 public CNetMessagePB< svc_ServerInfo, CSVCMsg_ServerInfo, SG_SIGNON > {};
class CSVCMsg_FlattenedSerializer_t :        public CNetMessagePB< svc_FlattenedSerializer, CSVCMsg_FlattenedSerializer, SG_SIGNON > {};
class CSVCMsg_ClassInfo_t :                  public CNetMessagePB< svc_ClassInfo, CSVCMsg_ClassInfo, SG_SIGNON > {};
class CSVCMsg_SetPause_t :                   public CNetMessagePB< svc_SetPause, CSVCMsg_SetPause > {};
class CSVCMsg_CreateStringTable_t :          public CNetMessagePB< svc_CreateStringTable, CSVCMsg_CreateStringTable, SG_SIGNON > {};
class CSVCMsg_UpdateStringTable_t :          public CNetMessagePB< svc_UpdateStringTable, CSVCMsg_UpdateStringTable, SG_STRINGTABLE > {};
class CSVCMsg_VoiceInit_t :                  public CNetMessagePB< svc_VoiceInit, CSVCMsg_VoiceInit, SG_SIGNON > {};
class CSVCMsg_VoiceData_t :                  public CNetMessagePB< svc_VoiceData, CSVCMsg_VoiceData, SG_VOICE > {};
class CSVCMsg_Print_t :                      public CNetMessagePB< svc_Print, CSVCMsg_Print > {};
class CSVCMsg_Sounds_t :                     public CNetMessagePB< svc_Sounds, CSVCMsg_Sounds, SG_SOUNDS > {};
class CSVCMsg_SetView_t :                    public CNetMessagePB< svc_SetView, CSVCMsg_SetView > {};
class CSVCMsg_ClearAllStringTables_t :       public CNetMessagePB< svc_ClearAllStringTables, CSVCMsg_ClearAllStringTables, SG_SIGNON > {};
class CSVCMsg_CmdKeyValues_t :               public CNetMessagePB< svc_CmdKeyValues, CSVCMsg_CmdKeyValues, SG_STRINGCMD > {};
class CSVCMsg_BSPDecal_t :                   public CNetMessagePB< svc_BSPDecal, CSVCMsg_BSPDecal > {};
class CSVCMsg_SplitScreen_t :                public CNetMessagePB< svc_SplitScreen, CSVCMsg_SplitScreen, SG_SIGNON > {};
class CSVCMsg_PacketEntities_t :             public CNetMessagePB< svc_PacketEntities, CSVCMsg_PacketEntities, SG_INVALID > {};
class CSVCMsg_Prefetch_t :                   public CNetMessagePB< svc_Prefetch, CSVCMsg_Prefetch, SG_SOUNDS > {};
class CSVCMsg_Menu_t :                       public CNetMessagePB< svc_Menu, CSVCMsg_Menu > {};
class CSVCMsg_GetCvarValue_t :               public CNetMessagePB< svc_GetCvarValue, CSVCMsg_GetCvarValue > {};
class CSVCMsg_StopSound_t :                  public CNetMessagePB< svc_StopSound, CSVCMsg_StopSound, SG_SOUNDS > {};
class CSVCMsg_PeerList_t :                   public CNetMessagePB< svc_PeerList, CSVCMsg_PeerList > {};
class CSVCMsg_PacketReliable_t :             public CNetMessagePB< svc_PacketReliable, CSVCMsg_PacketReliable > {};
class CSVCMsg_HLTVStatus_t :                 public CNetMessagePB< svc_HLTVStatus, CSVCMsg_HLTVStatus, SG_GENERIC, BUF_RELIABLE, true > {};
class CSVCMsg_ServerSteamID_t :              public CNetMessagePB< svc_ServerSteamID, CSVCMsg_ServerSteamID, SG_GENERIC, BUF_RELIABLE, true > {};
class CSVCMsg_FullFrameSplit_t :             public CNetMessagePB< svc_FullFrameSplit, CSVCMsg_FullFrameSplit > {};
class CSVCMsg_RconServerDetails_t :          public CNetMessagePB< svc_RconServerDetails, CSVCMsg_RconServerDetails > {};
class CSVCMsg_UserMessage_t :                public CNetMessagePB< svc_UserMessage, CSVCMsg_UserMessage, SG_USERMSG > {};
class CSVCMsg_Broadcast_Command_t :          public CNetMessagePB< svc_Broadcast_Command, CSVCMsg_Broadcast_Command, SG_HLTVREPLAY > {};
class CSVCMsg_HltvFixupOperatorStatus_t :    public CNetMessagePB< svc_HltvFixupOperatorStatus, CSVCMsg_HltvFixupOperatorStatus > {};
class CSVCMsg_UserCommands_t :               public CNetMessagePB< svc_UserCmds, CSVCMsg_UserCommands, SG_HLTVREPLAY > {};

#endif // NETMESSAGES_H
