//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Schema data types of the animation library
//
//===========================================================================//

#ifndef ANIMLIB_TYPES_H
#define ANIMLIB_TYPES_H
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

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;

class CNmBitFlags
{
public:
	uint32 m_flags;
};

class CNmBoneWeightList
{
public:
	CResourceString m_skeletonName;
	CUtlVector< CGlobalSymbol > m_boneIDs;
	CUtlVector< float > m_weights;
};

class CNmContactAudioActionVData
{
};

class CNmContactAudioTypeVData
{
};

enum class CNmEventRelevance_t : int32
{
	ClientOnly = 0,
	ServerOnly = 1,
	ClientAndServer = 2,
};

enum class CNmEventTargetEntity_t : int32
{
	Self = 0,
	Weapon = 1,
	HeldItem = 2,
	Custom = 3,
};

struct CNmFloatChannelSet_t
{
	CGlobalSymbol m_ID;
	CUtlLeanVector< CGlobalSymbol > m_channelIDs;
};

class CNmRootMotionData
{
public:
	enum class SamplingMode_t : uint8
	{
		Delta = 0,
		WorldSpace = 1,
	};

	CUtlVector< CTransform > m_transforms;
	int32 m_nNumFrames;
	float m_flAverageLinearVelocity;
	float m_flAverageAngularVelocityRadians;
	CTransform m_totalDelta;
};

struct NmBoneMaskSetDefinition_t
{
	CGlobalSymbol m_ID;
	CNmBoneWeightList m_primaryWeightList;
	CUtlLeanVector< CNmBoneWeightList > m_secondaryWeightLists;
};

enum class NmCachedValueMode_t : int32
{
	OnEntry = 0,
	OnExit = 1,
};

struct NmCompressionSettings_t
{
	struct QuantizationRange_t
	{
		float m_flRangeStart;
		float m_flRangeLength;
	};

	NmCompressionSettings_t::QuantizationRange_t m_translationRangeX;
	NmCompressionSettings_t::QuantizationRange_t m_translationRangeY;
	NmCompressionSettings_t::QuantizationRange_t m_translationRangeZ;
	NmCompressionSettings_t::QuantizationRange_t m_scaleRange;
	int32 m_nTrackReadOffset;
	QuaternionAligned m_constantRotation;
	bool m_bIsRotationStatic;
	bool m_bIsTranslationStatic;
	bool m_bIsScaleStatic;
};

struct NmContactAudioInfo_t
{
	CGlobalSymbol m_audioActionID;
	CGlobalSymbol m_audioTypeID;
	CGlobalSymbol m_soundeventOverrideID;
};

enum class NmEasingFunction_t : uint8
{
	Linear = 0,
	Quad = 1,
	Cubic = 2,
	Quart = 3,
	Quint = 4,
	Sine = 5,
	Expo = 6,
	Circ = 7,
	Back = 8,
};

enum class NmEasingOperation_t : uint8
{
	Linear = 0,
	InQuad = 1,
	OutQuad = 2,
	InOutQuad = 3,
	InCubic = 4,
	OutCubic = 5,
	InOutCubic = 6,
	InQuart = 7,
	OutQuart = 8,
	InOutQuart = 9,
	InQuint = 10,
	OutQuint = 11,
	InOutQuint = 12,
	InSine = 13,
	OutSine = 14,
	InOutSine = 15,
	InExpo = 16,
	OutExpo = 17,
	InOutExpo = 18,
	InCirc = 19,
	OutCirc = 20,
	InOutCirc = 21,
	None = 22,
};

enum class NmEventConditionRules_t : uint8
{
	LimitSearchToSourceState = 0,
	IgnoreInactiveEvents = 1,
	PreferHighestWeight = 2,
	PreferHighestProgress = 3,
	OperatorOr = 4,
	OperatorAnd = 5,
	SearchOnlyGraphEvents = 6,
	SearchOnlyAnimEvents = 7,
	SearchBothGraphAndAnimEvents = 8,
};

struct NmFloatCurveCompressionSettings_t
{
	NmCompressionSettings_t::QuantizationRange_t m_range;
	bool m_bIsStatic;
};

enum class NmFollowBoneMode_t : uint8
{
	RotationAndTranslation = 0,
	RotationOnly = 1,
	TranslationOnly = 2,
};

enum class NmFootPhaseCondition_t : uint8
{
	LeftFootDown = 0,
	LeftFootPassing = 1,
	LeftPhase = 4,
	RightFootDown = 2,
	RightFootPassing = 3,
	RightPhase = 5,
	None = 6,
};

enum class NmFootPhase_t : uint8
{
	LeftFootDown = 0,
	RightFootPassing = 1,
	RightFootDown = 2,
	LeftFootPassing = 3,
	None = 4,
};

enum class NmFrameSnapEventMode_t : int32
{
	Floor = 0,
	Round = 1,
};

enum class NmGraphDebugMode_t : int32
{
	Off = 0,
	On = 1,
};

enum class NmGraphEventTypeCondition_t : uint8
{
	Entry = 0,
	FullyInState = 1,
	Exit = 2,
	Timed = 3,
	Generic = 4,
	Any = 5,
};

enum class NmIKBlendMode_t : uint8
{
	Effector = 0,
	Pose = 1,
};

struct NmPercent_t
{
	float m_flValue;
};

enum class NmPoseBlendMode_t : uint8
{
	Overlay = 0,
	Additive = 1,
	ModelSpace = 2,
};

enum class NmRootMotionBlendMode_t : uint8
{
	Blend = 0,
	Additive = 1,
	IgnoreSource = 2,
	IgnoreTarget = 3,
};

struct NmSyncTrackTime_t
{
	int32 m_nEventIdx;
	NmPercent_t m_percentageThrough;
};

enum class NmTargetWarpAlgorithm_t : uint8
{
	Lerp = 0,
	Hermite = 1,
	HermiteFeaturePreserving = 2,
	Bezier = 3,
};

enum class NmTargetWarpRule_t : uint8
{
	WarpXY = 0,
	WarpZ = 1,
	WarpXYZ = 2,
	RotationOnly = 3,
	FixedSection = 4,
};

enum class NmTransitionRuleCondition_t : uint8
{
	AnyAllowed = 0,
	FullyAllowed = 1,
	ConditionallyAllowed = 2,
	Blocked = 3,
};

enum class NmTransitionRule_t : uint8
{
	AllowTransition = 0,
	ConditionallyAllowTransition = 1,
	BlockTransition = 2,
};

enum class ParticleAttachment_t : int32
{
	PATTACH_INVALID = -1,
	PATTACH_ABSORIGIN = 0,
	PATTACH_ABSORIGIN_FOLLOW = 1,
	PATTACH_CUSTOMORIGIN = 2,
	PATTACH_CUSTOMORIGIN_FOLLOW = 3,
	PATTACH_POINT = 4,
	PATTACH_POINT_FOLLOW = 5,
	PATTACH_EYES_FOLLOW = 6,
	PATTACH_OVERHEAD_FOLLOW = 7,
	PATTACH_WORLDORIGIN = 8,
	PATTACH_ROOTBONE_FOLLOW = 9,
	PATTACH_RENDERORIGIN_FOLLOW = 10,
	PATTACH_MAIN_VIEW = 11,
	PATTACH_WATERWAKE = 12,
	PATTACH_CENTER_FOLLOW = 13,
	PATTACH_CUSTOM_GAME_STATE_1 = 14,
	PATTACH_HEALTHBAR = 15,
	MAX_PATTACH_TYPES = 16,
};

class CNmFloatChannelData
{
public:
	struct ChannelSettings_t
	{
		NmCompressionSettings_t::QuantizationRange_t m_range;
		bool m_bIsStatic;
	};

	CStrongHandle< InfoForResourceTypeCNmSkeleton > m_skeleton;
	CGlobalSymbol m_setID;
	CUtlVector< CNmFloatChannelData::ChannelSettings_t > m_channelSettings;
	CUtlVector< uint16 > m_compressedData;
	CUtlVector< uint32 > m_compressedOffsets;
};

class CNmSyncTrack
{
public:
	struct EventMarker_t
	{
		NmPercent_t m_startTime;
		CGlobalSymbol m_ID;
	};

	struct Event_t
	{
		CGlobalSymbol m_ID;
		NmPercent_t m_startTime;
		NmPercent_t m_duration;
	};

	CUtlLeanVectorFixedGrowable< CNmSyncTrack::Event_t, 10 > m_syncEvents;
	int32 m_nStartEventOffset;
};

struct NmSyncTrackTimeRange_t
{
	NmSyncTrackTime_t m_startTime;
	NmSyncTrackTime_t m_endTime;
};
#endif // ANIMLIB_TYPES_H
