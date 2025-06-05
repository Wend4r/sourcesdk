//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef USERMESSAGES_H
#define USERMESSAGES_H

#pragma once

#include "inetchannelinfo.h"
#include "networksystem/netmessage.h"

#include <usermessages.pb.h>

class CUserMessageAchievementEvent_t         : public CNetMessagePB< UM_AchievementEvent, CUserMessageAchievementEvent, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageCloseCaption_t             : public CNetMessagePB< UM_CloseCaption, CUserMessageCloseCaption, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageCloseCaptionDirect_t       : public CNetMessagePB< UM_CloseCaptionDirect, CUserMessageCloseCaptionDirect, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageCurrentTimescale_t         : public CNetMessagePB< UM_CurrentTimescale, CUserMessageCurrentTimescale, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageDesiredTimescale_t         : public CNetMessagePB< UM_DesiredTimescale, CUserMessageDesiredTimescale, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageFade_t                     : public CNetMessagePB< UM_Fade, CUserMessageFade, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageHudMsg_t                   : public CNetMessagePB< UM_HudMsg, CUserMessageHudMsg, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageHudText_t                  : public CNetMessagePB< UM_HudText, CUserMessageHudText, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageColoredText_t              : public CUserMessagePB< UM_ColoredText, CUserMessageColoredText > {};
class CUserMessageRequestState_t             : public CNetMessagePB< UM_RequestState, CUserMessageRequestState, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageResetHUD_t                 : public CNetMessagePB< UM_ResetHUD, CUserMessageResetHUD, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageRumble_t                   : public CNetMessagePB< UM_Rumble, CUserMessageRumble, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageSayText_t                  : public CNetMessagePB< UM_SayText, CUserMessageSayText, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageSayText2_t                 : public CNetMessagePB< UM_SayText2, CUserMessageSayText2, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageSayTextChannel_t           : public CNetMessagePB< UM_SayTextChannel, CUserMessageSayTextChannel, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageShake_t                    : public CNetMessagePB< UM_Shake, CUserMessageShake, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageShakeDir_t                 : public CNetMessagePB< UM_ShakeDir, CUserMessageShakeDir, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageWaterShake_t               : public CUserMessagePB< UM_WaterShake, CUserMessageWaterShake > {};
class CUserMessageTextMsg_t                  : public CUserMessagePB< UM_TextMsg, CUserMessageTextMsg > {};
class CUserMessageScreenTilt_t               : public CNetMessagePB< UM_ScreenTilt, CUserMessageScreenTilt, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageVoiceMask_t                : public CNetMessagePB< UM_VoiceMask, CUserMessageVoiceMask, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageSendAudio_t                : public CNetMessagePB< UM_SendAudio, CUserMessageSendAudio, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageItemPickup_t               : public CNetMessagePB< UM_ItemPickup, CUserMessageItemPickup, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageAmmoDenied_t               : public CNetMessagePB< UM_AmmoDenied, CUserMessageAmmoDenied, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageShowMenu_t                 : public CNetMessagePB< UM_ShowMenu, CUserMessageShowMenu, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageCreditsMsg_t               : public CUserMessagePB< UM_CreditsMsg, CUserMessageCreditsMsg > {};
class CUserMessageCloseCaptionPlaceholder_t  : public CNetMessagePB< UM_CloseCaptionPlaceholder, CUserMessageCloseCaptionPlaceholder, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageCameraTransition_t         : public CNetMessagePB< UM_CameraTransition, CUserMessageCameraTransition, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageAudioParameter_t           : public CUserMessagePB< UM_AudioParameter, CUserMessageAudioParameter > {};
class CUserMessageHapticsManagerPulse_t      : public CNetMessagePB< UM_HapticsManagerPulse, CUserMessageHapticsManagerPulse, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageHapticsManagerEffect_t     : public CNetMessagePB< UM_HapticsManagerEffect, CUserMessageHapticsManagerEffect, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageUpdateCssClasses_t         : public CNetMessagePB< UM_UpdateCssClasses, CUserMessageUpdateCssClasses, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageServerFrameTime_t          : public CNetMessagePB< UM_ServerFrameTime, CUserMessageServerFrameTime, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageLagCompensationError_t     : public CNetMessagePB< UM_LagCompensationError, CUserMessageLagCompensationError, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageRequestDllStatus_t         : public CNetMessagePB< UM_RequestDllStatus, CUserMessageRequestDllStatus, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageRequestUtilAction_t        : public CNetMessagePB< UM_RequestUtilAction, CUserMessageRequestUtilAction, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageRequestInventory_t         : public CNetMessagePB< UM_RequestInventory, CUserMessageRequestInventory, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessageRequestDiagnostic_t        : public CNetMessagePB< UM_RequestDiagnostic, CUserMessageRequestDiagnostic, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessage_NotifyResponseFound_t     : public CNetMessagePB< UM_NotifyResponseFound, CUserMessage_NotifyResponseFound, SG_USERMSG, BUF_RELIABLE, true > {};
class CUserMessage_PlayResponseConditional_t : public CNetMessagePB< UM_PlayResponseConditional, CUserMessage_PlayResponseConditional, SG_USERMSG, BUF_RELIABLE, true > {};

#endif // USERMESSAGES_H
