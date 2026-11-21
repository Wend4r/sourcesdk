//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef TE_MESSAGES_H
#define TE_MESSAGES_H

#pragma once

#include "inetchannelinfo.h"
#include "networksystem/netmessage.h"

#include <te.pb.h>
#include <cs_gameevents.pb.h>

// Temp entity events: the "Events" group, unreliable
template< NetworkMessageId ID, typename PROTO_TYPE >
using CTempEntityMessagePB = CNetMessagePB< ID, PROTO_TYPE, SG_EVENTS, BUF_UNRELIABLE, true >;

class TEEffectDispatch_GameEvent_t   : public CTempEntityMessagePB< TE_EffectDispatchId,   CMsgTEEffectDispatch > {};
class TEArmorRicochet_GameEvent_t    : public CTempEntityMessagePB< TE_ArmorRicochetId,    CMsgTEArmorRicochet > {};
class TEBeamEntPoint_GameEvent_t     : public CTempEntityMessagePB< TE_BeamEntPointId,     CMsgTEBeamEntPoint > {};
class TEBeamEnts_GameEvent_t         : public CTempEntityMessagePB< TE_BeamEntsId,         CMsgTEBeamEnts > {};
class TEBeamPoints_GameEvent_t       : public CTempEntityMessagePB< TE_BeamPointsId,       CMsgTEBeamPoints > {};
class TEBeamRing_GameEvent_t         : public CTempEntityMessagePB< TE_BeamRingId,         CMsgTEBeamRing > {};
class TEBubbles_GameEvent_t          : public CTempEntityMessagePB< TE_BubblesId,          CMsgTEBubbles > {};
class TEBubbleTrail_GameEvent_t      : public CTempEntityMessagePB< TE_BubbleTrailId,      CMsgTEBubbleTrail > {};
class TEDecal_GameEvent_t            : public CTempEntityMessagePB< TE_DecalId,            CMsgTEDecal > {};
class TEWorldDecal_GameEvent_t       : public CTempEntityMessagePB< TE_WorldDecalId,       CMsgTEWorldDecal > {};
class TEEnergySplash_GameEvent_t     : public CTempEntityMessagePB< TE_EnergySplashId,     CMsgTEEnergySplash > {};
class TEFizz_GameEvent_t             : public CTempEntityMessagePB< TE_FizzId,             CMsgTEFizz > {};
// Neither the server nor the client registers this message
class TEShatterSurface_GameEvent_t   : public CTempEntityMessagePB< TE_ShatterSurfaceId,   CMsgTEShatterSurface > {};
class TEGlowSprite_GameEvent_t       : public CTempEntityMessagePB< TE_GlowSpriteId,       CMsgTEGlowSprite > {};
class TEImpact_GameEvent_t           : public CTempEntityMessagePB< TE_ImpactId,           CMsgTEImpact > {};
class TEMuzzleFlash_GameEvent_t      : public CTempEntityMessagePB< TE_MuzzleFlashId,      CMsgTEMuzzleFlash > {};
class TEBloodStream_GameEvent_t      : public CTempEntityMessagePB< TE_BloodStreamId,      CMsgTEBloodStream > {};
class TEExplosion_GameEvent_t        : public CTempEntityMessagePB< TE_ExplosionId,        CMsgTEExplosion > {};
class TEDust_GameEvent_t             : public CTempEntityMessagePB< TE_DustId,             CMsgTEDust > {};
class TELargeFunnel_GameEvent_t      : public CTempEntityMessagePB< TE_LargeFunnelId,      CMsgTELargeFunnel > {};
class TESparks_GameEvent_t           : public CTempEntityMessagePB< TE_SparksId,           CMsgTESparks > {};
class TEPhysicsProp_GameEvent_t      : public CTempEntityMessagePB< TE_PhysicsPropId,      CMsgTEPhysicsProp > {};
class TESmoke_GameEvent_t            : public CTempEntityMessagePB< TE_SmokeId,            CMsgTESmoke > {};
class TEFirebullets_GameEvent_t      : public CTempEntityMessagePB< GE_FireBulletsId,      CMsgTEFireBullets > {};

#endif // TE_MESSAGES_H
