//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef USERMESSAGES_H
#define USERMESSAGES_H

#pragma once

#include "inetchannelinfo.h"
#include "iloopmode.h"
#include "networksystem/netmessage.h"

#include <usermessages.pb.h>

class CUserMessageAchievementEvent_t         : public CUserMessagePB< UM_AchievementEvent, CUserMessageAchievementEvent > {};
class CUserMessageCurrentTimescale_t         : public CUserMessagePB< UM_CurrentTimescale, CUserMessageCurrentTimescale > {};
class CUserMessageDesiredTimescale_t         : public CUserMessagePB< UM_DesiredTimescale, CUserMessageDesiredTimescale > {};
class CUserMessageFade_t                     : public CUserMessagePB< UM_Fade, CUserMessageFade > {};
class CUserMessageHudMsg_t                   : public CUserMessagePB< UM_HudMsg, CUserMessageHudMsg > {};
class CUserMessageHudText_t                  : public CUserMessagePB< UM_HudText, CUserMessageHudText > {};
class CUserMessageColoredText_t              : public CUserMessagePB< UM_ColoredText, CUserMessageColoredText > {};
class CUserMessageRequestState_t             : public CUserMessagePB< UM_RequestState, CUserMessageRequestState > {};
class CUserMessageResetHUD_t                 : public CUserMessagePB< UM_ResetHUD, CUserMessageResetHUD > {};
class CUserMessageRumble_t                   : public CUserMessagePB< UM_Rumble, CUserMessageRumble > {};
class CUserMessageSayText_t                  : public CUserMessagePB< UM_SayText, CUserMessageSayText > {};
class CUserMessageSayText2_t                 : public CUserMessagePB< UM_SayText2, CUserMessageSayText2 > {};
class CUserMessageSayTextChannel_t           : public CUserMessagePB< UM_SayTextChannel, CUserMessageSayTextChannel > {};
class CUserMessageShake_t                    : public CUserMessagePB< UM_Shake, CUserMessageShake > {};
class CUserMessageShakeDir_t                 : public CUserMessagePB< UM_ShakeDir, CUserMessageShakeDir > {};
class CUserMessageWaterShake_t               : public CUserMessagePB< UM_WaterShake, CUserMessageWaterShake > {};
class CUserMessageTextMsg_t                  : public CUserMessagePB< UM_TextMsg, CUserMessageTextMsg > {};
class CUserMessageScreenTilt_t               : public CUserMessagePB< UM_ScreenTilt, CUserMessageScreenTilt > {};
class CUserMessageVoiceMask_t                : public CUserMessagePB< UM_VoiceMask, CUserMessageVoiceMask > {};
class CUserMessageSendAudio_t                : public CUserMessagePB< UM_SendAudio, CUserMessageSendAudio > {};
class CUserMessageItemPickup_t               : public CUserMessagePB< UM_ItemPickup, CUserMessageItemPickup > {};
class CUserMessageAmmoDenied_t               : public CUserMessagePB< UM_AmmoDenied, CUserMessageAmmoDenied > {};
class CUserMessageShowMenu_t                 : public CUserMessagePB< UM_ShowMenu, CUserMessageShowMenu > {};
class CUserMessageCreditsMsg_t               : public CUserMessagePB< UM_CreditsMsg, CUserMessageCreditsMsg > {};
class CEntityMessagePropagateForce_t         : public CEntityMessagePB< EM_PropagateForce, CEntityMessagePropagateForce > {};
class CUserMessageCloseCaptionPlaceholder_t  : public CUserMessagePB< UM_CloseCaptionPlaceholder, CUserMessageCloseCaptionPlaceholder > {};
class CUserMessageCameraTransition_t         : public CUserMessagePB< UM_CameraTransition, CUserMessageCameraTransition > {};
class CUserMessageAudioParameter_t           : public CUserMessagePB< UM_AudioParameter, CUserMessageAudioParameter > {};
class CUserMsg_ParticleManager_t             : public CUserMessagePB< UM_ParticleManager, CUserMsg_ParticleManager > {};
class CUserMsg_CustomGameEvent_t             : public CUserMessagePB< UM_CustomGameEvent, CBaseCmdKeyValues< CUserMsg_CustomGameEvent > > {};
class CUserMessageHapticsManagerPulse_t      : public CUserMessagePB< UM_HapticsManagerPulse, CUserMessageHapticsManagerPulse > {};
class CUserMessageHapticsManagerEffect_t     : public CUserMessagePB< UM_HapticsManagerEffect, CUserMessageHapticsManagerEffect > {};
class CUserMessageUpdateCssClasses_t         : public CUserMessagePB< UM_UpdateCssClasses, CUserMessageUpdateCssClasses > {};
class CUserMessageServerFrameTime_t          : public CUserMessagePB< UM_ServerFrameTime, CUserMessageServerFrameTime > {};
class CUserMessageLagCompensationError_t     : public CUserMessagePB< UM_LagCompensationError, CUserMessageLagCompensationError > {};
class CUserMessageRequestDllStatus_t         : public CUserMessagePB< UM_RequestDllStatus, CUserMessageRequestDllStatus > {};
class CUserMessageRequestUtilAction_t        : public CUserMessagePB< UM_RequestUtilAction, CUserMessageRequestUtilAction > {};
class CUserMessageRequestInventory_t         : public CUserMessagePB< UM_RequestInventory, CUserMessageRequestInventory > {};
class CUserMessageRequestDiagnostic_t        : public CUserMessagePB< UM_RequestDiagnostic, CUserMessageRequestDiagnostic > {};
class CUserMessage_NotifyResponseFound_t     : public CUserMessagePB< UM_NotifyResponseFound, CUserMessage_NotifyResponseFound > {};
class CUserMessage_PlayResponseConditional_t : public CUserMessagePB< UM_PlayResponseConditional, CUserMessage_PlayResponseConditional > {};

#endif // USERMESSAGES_H
