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

class CMsgTEEffectDispatch_t   : public CNetMessagePB< TE_EffectDispatchId,   CMsgTEEffectDispatch,   SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEArmorRicochet_t    : public CNetMessagePB< TE_ArmorRicochetId,    CMsgTEArmorRicochet,    SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEBeamEntPoint_t     : public CNetMessagePB< TE_BeamEntPointId,     CMsgTEBeamEntPoint,     SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEBeamEnts_t         : public CNetMessagePB< TE_BeamEntsId,         CMsgTEBeamEnts,         SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEBeamPoints_t       : public CNetMessagePB< TE_BeamPointsId,       CMsgTEBeamPoints,       SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEBeamRing_t         : public CNetMessagePB< TE_BeamRingId,         CMsgTEBeamRing,         SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEBubbles_t          : public CNetMessagePB< TE_BubblesId,          CMsgTEBubbles,          SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEBubbleTrail_t      : public CNetMessagePB< TE_BubbleTrailId,      CMsgTEBubbleTrail,      SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEDecal_t            : public CNetMessagePB< TE_DecalId,            CMsgTEDecal,            SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEWorldDecal_t       : public CNetMessagePB< TE_WorldDecalId,       CMsgTEWorldDecal,       SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEEnergySplash_t     : public CNetMessagePB< TE_EnergySplashId,     CMsgTEEnergySplash,     SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEFizz_t             : public CNetMessagePB< TE_FizzId,             CMsgTEFizz,             SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEShatterSurface_t   : public CNetMessagePB< TE_ShatterSurfaceId,   CMsgTEShatterSurface,   SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEGlowSprite_t       : public CNetMessagePB< TE_GlowSpriteId,       CMsgTEGlowSprite,       SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEImpact_t           : public CNetMessagePB< TE_ImpactId,           CMsgTEImpact,           SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEMuzzleFlash_t      : public CNetMessagePB< TE_MuzzleFlashId,      CMsgTEMuzzleFlash,      SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEBloodStream_t      : public CNetMessagePB< TE_BloodStreamId,      CMsgTEBloodStream,      SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEExplosion_t        : public CNetMessagePB< TE_ExplosionId,        CMsgTEExplosion,        SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEDust_t             : public CNetMessagePB< TE_DustId,             CMsgTEDust,             SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTELargeFunnel_t      : public CNetMessagePB< TE_LargeFunnelId,      CMsgTELargeFunnel,      SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTESparks_t           : public CNetMessagePB< TE_SparksId,           CMsgTESparks,           SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTEPhysicsProp_t      : public CNetMessagePB< TE_PhysicsPropId,      CMsgTEPhysicsProp,      SG_EVENTS, BUF_RELIABLE, true > {};
class CMsgTESmoke_t            : public CNetMessagePB< TE_SmokeId,            CMsgTESmoke,            SG_EVENTS, BUF_RELIABLE, true > {};

#endif // TE_MESSAGES_H
