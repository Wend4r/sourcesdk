
#ifndef CSTRIKE15_USERMESSAGES_H
#define CSTRIKE15_USERMESSAGES_H

#pragma once

#include "inetchannelinfo.h"
#include "networksystem/netmessage.h"

// Included
//   steammessages.proto
//   engine_gcmessages.proto
//   gcsdk_gcmessages.proto
//   cstrike15_gcmessages.proto
#include <cstrike15_usermessages.pb.h>

class CCSUsrMsg_VGUIMenu_t                        : public CUserMessagePB< CS_UM_VGUIMenu, CCSUsrMsg_VGUIMenu > {};
class CCSUsrMsg_Geiger_t                          : public CUserMessagePB< CS_UM_Geiger, CCSUsrMsg_Geiger > {};
class CCSUsrMsg_Train_t                           : public CUserMessagePB< CS_UM_Train, CCSUsrMsg_Train > {};
class CCSUsrMsg_HudText_t                         : public CUserMessagePB< CS_UM_HudText, CCSUsrMsg_HudText > {};
class CCSUsrMsg_SendAudio_t                       : public CUserMessagePB< CS_UM_SendAudio, CCSUsrMsg_SendAudio > {};
class CCSUsrMsg_RawAudio_t                        : public CUserMessagePB< CS_UM_RawAudio, CCSUsrMsg_RawAudio > {};
class CCSUsrMsg_Damage_t                          : public CUserMessagePB< CS_UM_Damage, CCSUsrMsg_Damage > {};
class CCSUsrMsg_RadioText_t                       : public CUserMessagePB< CS_UM_RadioText, CCSUsrMsg_RadioText > {};
class CCSUsrMsg_HintText_t                        : public CUserMessagePB< CS_UM_HintText, CCSUsrMsg_HintText > {};
class CCSUsrMsg_KeyHintText_t                     : public CUserMessagePB< CS_UM_KeyHintText, CCSUsrMsg_KeyHintText > {};
class CCSUsrMsg_ProcessSpottedEntityUpdate_t      : public CUserMessagePB< CS_UM_ProcessSpottedEntityUpdate, CCSUsrMsg_ProcessSpottedEntityUpdate > {};
class CCSUsrMsg_AdjustMoney_t                     : public CUserMessagePB< CS_UM_AdjustMoney, CCSUsrMsg_AdjustMoney > {};
class CCSUsrMsg_KillCam_t                         : public CUserMessagePB< CS_UM_KillCam, CCSUsrMsg_KillCam > {};
class CCSUsrMsg_MatchEndConditions_t              : public CUserMessagePB< CS_UM_MatchEndConditions, CCSUsrMsg_MatchEndConditions > {};
class CCSUsrMsg_DisconnectToLobby_t               : public CUserMessagePB< CS_UM_DisconnectToLobby, CCSUsrMsg_DisconnectToLobby > {};
class CCSUsrMsg_PlayerStatsUpdate_t               : public CUserMessagePB< CS_UM_PlayerStatsUpdate, CCSUsrMsg_PlayerStatsUpdate > {};
class CCSUsrMsg_WarmupHasEnded_t                  : public CUserMessagePB< CS_UM_WarmupHasEnded, CCSUsrMsg_WarmupHasEnded > {};
class CCSUsrMsg_CallVoteFailed_t                  : public CUserMessagePB< CS_UM_CallVoteFailed, CCSUsrMsg_CallVoteFailed > {};
class CCSUsrMsg_VoteStart_t                       : public CUserMessagePB< CS_UM_VoteStart, CCSUsrMsg_VoteStart > {};
class CCSUsrMsg_VotePass_t                        : public CUserMessagePB< CS_UM_VotePass, CCSUsrMsg_VotePass > {};
class CCSUsrMsg_VoteFailed_t                      : public CUserMessagePB< CS_UM_VoteFailed, CCSUsrMsg_VoteFailed > {};
class CCSUsrMsg_VoteSetup_t                       : public CUserMessagePB< CS_UM_VoteSetup, CCSUsrMsg_VoteSetup > {};
class CCSUsrMsg_ServerRankRevealAll_t             : public CUserMessagePB< CS_UM_ServerRankRevealAll, CCSUsrMsg_ServerRankRevealAll > {};
class CCSUsrMsg_SendLastKillerDamageToClient_t    : public CUserMessagePB< CS_UM_SendLastKillerDamageToClient, CCSUsrMsg_SendLastKillerDamageToClient > {};
class CCSUsrMsg_ServerRankUpdate_t                : public CUserMessagePB< CS_UM_ServerRankUpdate, CCSUsrMsg_ServerRankUpdate > {};
class CCSUsrMsg_GlowPropTurnOff_t                 : public CUserMessagePB< CS_UM_GlowPropTurnOff, CCSUsrMsg_GlowPropTurnOff > {};
class CCSUsrMsg_SendPlayerItemDrops_t             : public CUserMessagePB< CS_UM_SendPlayerItemDrops, CCSUsrMsg_SendPlayerItemDrops > {};
class CCSUsrMsg_RoundBackupFilenames_t            : public CUserMessagePB< CS_UM_RoundBackupFilenames, CCSUsrMsg_RoundBackupFilenames > {};
class CCSUsrMsg_SendPlayerItemFound_t             : public CUserMessagePB< CS_UM_SendPlayerItemFound, CCSUsrMsg_SendPlayerItemFound > {};
class CCSUsrMsg_ReportHit_t                       : public CUserMessagePB< CS_UM_ReportHit, CCSUsrMsg_ReportHit > {};
class CCSUsrMsg_XpUpdate_t                        : public CUserMessagePB< CS_UM_XpUpdate, CCSUsrMsg_XpUpdate > {};
class CCSUsrMsg_QuestProgress_t                   : public CUserMessagePB< CS_UM_QuestProgress, CCSUsrMsg_QuestProgress > {};
class CCSUsrMsg_ScoreLeaderboardData_t            : public CUserMessagePB< CS_UM_ScoreLeaderboardData, CCSUsrMsg_ScoreLeaderboardData > {};
class CCSUsrMsg_PlayerDecalDigitalSignature_t     : public CUserMessagePB< CS_UM_PlayerDecalDigitalSignature, CCSUsrMsg_PlayerDecalDigitalSignature > {};
class CCSUsrMsg_WeaponSound_t                     : public CUserMessagePB< CS_UM_WeaponSound, CCSUsrMsg_WeaponSound > {};
class CCSUsrMsg_UpdateScreenHealthBar_t           : public CUserMessagePB< CS_UM_UpdateScreenHealthBar, CCSUsrMsg_UpdateScreenHealthBar > {};
class CCSUsrMsg_EntityOutlineHighlight_t          : public CUserMessagePB< CS_UM_EntityOutlineHighlight, CCSUsrMsg_EntityOutlineHighlight > {};
class CCSUsrMsg_SSUI_t                            : public CUserMessagePB< CS_UM_SSUI, CCSUsrMsg_SSUI > {};
class CCSUsrMsg_SurvivalStats_t                   : public CUserMessagePB< CS_UM_SurvivalStats, CCSUsrMsg_SurvivalStats > {};
class CCSUsrMsg_EndOfMatchAllPlayersData_t        : public CUserMessagePB< CS_UM_EndOfMatchAllPlayersData, CCSUsrMsg_EndOfMatchAllPlayersData > {};
class CCSUsrMsg_PostRoundDamageReport_t           : public CUserMessagePB< CS_UM_PostRoundDamageReport, CCSUsrMsg_PostRoundDamageReport > {};
class CCSUsrMsg_RoundEndReportData_t              : public CUserMessagePB< CS_UM_RoundEndReportData, CCSUsrMsg_RoundEndReportData > {};
class CCSUsrMsg_CurrentRoundOdds_t                : public CUserMessagePB< CS_UM_CurrentRoundOdds, CCSUsrMsg_CurrentRoundOdds > {};
class CCSUsrMsg_DeepStats_t                       : public CUserMessagePB< CS_UM_DeepStats, CCSUsrMsg_DeepStats > {};
class CCSUsrMsg_ShootInfo_t                       : public CUserMessagePB< CS_UM_ShootInfo, CCSUsrMsg_ShootInfo > {};
class CCSUsrMsg_CounterStrafe_t                   : public CUserMessagePB< CS_UM_CounterStrafe, CCSUsrMsg_CounterStrafe > {};


#endif // CSTRIKE15_USERMESSAGES_H
