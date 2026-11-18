//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Events placed on the timeline of an animation clip
//
//===========================================================================//

#ifndef ANIMEVENTS_H
#define ANIMEVENTS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utlbinaryblock.h"
#include "tier0/bufferstring.h"
#include "tier0/globalsymbol.h"
#include "mathlib/vector.h"
#include "mathlib/vector2d.h"
#include "mathlib/transform.h"
#include "mathlib/piecewisecurve.h"
#include "resourcesystem/resourcehandle.h"
#include "schemasystem/schematypes.h"
#include "resourcefile/resourcetype.h"
#include "particles/iparticlesystemmgr.h"
#include "animlib/graph/animgraph_valuetypes.h"
#include "animlib/animlib_types.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;

class CNmEvent
{
public:
	// Declared without bodies: the game implements them, and a definition may hold an event by value
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding();
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext );
	// Name of the most derived event class
	virtual const char *GetKV3TransferClassName() const;

	virtual ~CNmEvent() {}

	virtual bool IsValid() const;
	// ClientAndServer unless the event has a relevance of its own
	virtual CNmEventRelevance_t GetRelevance() const;
	// Self unless the event has a target of its own
	virtual CNmEventTargetEntity_t GetTargetEntity() const;
	virtual CBufferStringN< 256 > GetDebugText() const;

	NmPercent_t m_flStartTime;
	NmPercent_t m_flDuration;
	CGlobalSymbol m_syncID;
};

class CNmFloatCurveEvent : public CNmEvent
{
public:
	CGlobalSymbol m_ID;
	CPiecewiseCurve m_curve;
};

class CNmFootEvent : public CNmEvent
{
public:
	NmFootPhase_t m_phase;
};

class CNmFrameSnapEvent : public CNmEvent
{
public:
	NmFrameSnapEventMode_t m_frameSnapMode;
};

class CNmIDEvent : public CNmEvent
{
public:
	CGlobalSymbol m_ID;
	CGlobalSymbol m_secondaryID;
};

class CNmMaterialAttributeEvent : public CNmEvent
{
public:
	CNmEventTargetEntity_t m_target;
	CUtlString m_attributeName;
	CUtlStringToken m_attributeNameToken;
	CPiecewiseCurve m_x;
	CPiecewiseCurve m_y;
	CPiecewiseCurve m_z;
	CPiecewiseCurve m_w;
};

class CNmOrientationWarpEvent : public CNmEvent
{
};

class CNmParticleEvent : public CNmEvent
{
public:
	enum class Type_t : int32
	{
		Create = 0,
		Create_CFG = 1,
	};

	CNmEventRelevance_t m_relevance;
	CNmParticleEvent::Type_t m_type;
	CNmEventTargetEntity_t m_target;
	CStrongHandle< InfoForResourceTypeIParticleSystemDefinition > m_hParticleSystem;
	CUtlString m_tags;
	bool m_bStopImmediately;
	bool m_bDetachFromOwner;
	bool m_bPlayEndCap;
	CUtlString m_attachmentPoint0;
	ParticleAttachment_t m_attachmentType0;
	CUtlString m_attachmentPoint1;
	ParticleAttachment_t m_attachmentType1;
	CUtlString m_config;
	CUtlString m_effectForConfig;
};

class CNmRootMotionEvent : public CNmEvent
{
public:
	float m_flBlendTimeSeconds;
};

class CNmSoundEvent : public CNmEvent
{
public:
	enum class Position_t : int32
	{
		None = 0,
		World = 1,
		EntityPos = 2,
		EntityEyePos = 3,
		EntityAttachment = 4,
	};

	CNmEventRelevance_t m_relevance;
	CUtlString m_name;
	CNmSoundEvent::Position_t m_position;
	CUtlString m_attachmentName;
	CUtlString m_tags;
	bool m_bContinuePlayingSoundAtDurationEnd;
	float m_flDurationInterruptionThreshold;
};

class CNmTargetWarpEvent : public CNmEvent
{
public:
	NmTargetWarpRule_t m_rule;
	NmTargetWarpAlgorithm_t m_algorithm;
};

class CNmTransitionEvent : public CNmEvent
{
public:
	NmTransitionRule_t m_rule;
	CGlobalSymbol m_ID;
};

class CNmBodyGroupEvent : public CNmEvent
{
public:
	CNmEventTargetEntity_t m_target;
	CUtlString m_groupName;
	CUtlString m_choiceName;
};

class CNmCameraDOFEvent : public CNmEvent
{
public:
	CPiecewiseCurve m_curve;
};

class CNmCameraFOVEvent : public CNmEvent
{
public:
	CPiecewiseCurve m_curve;
};

class CNmClothEvent : public CNmEvent
{
public:
	enum class Type_t : int32
	{
		Stiffen = 0,
		Effect = 1,
	};

	CNmClothEvent::Type_t m_type;
	float m_flStiffness;
	float m_flSpeedIn;
	float m_flSpeedOut;
	float m_flLengthSeconds;
	CUtlString m_vertexSetName;
	CUtlString m_effectName;
};

class CNmContactEvent : public CNmEvent
{
public:
	CGlobalSymbol m_configID;
	CGlobalSymbol m_probeBoneID;
	Vector m_vBoneLocalProbeDir;
	float m_flProbeMaxDist;
	NmContactAudioInfo_t m_audioInfo;
};
#endif // ANIMEVENTS_H
