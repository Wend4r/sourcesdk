//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The events of a choreographed scene: one class per event type, all derived from
//          CChoreoEvent.
//
//=============================================================================//

#ifndef CHOREOEVENT_H
#define CHOREOEVENT_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier0/bufferstring.h"
#include "tier0/globalsymbol.h"
#include "mathlib/vector.h"
#include "mathlib/piecewisecurve.h"
#include "animlib/graph/animgraph_valuetypes.h"
#include "resourcesystem/resourcehandle.h"
#include "resourcesystem/resourcename.h"
#include "choreoobjects/choreoobjectbase.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CChoreoActor;
class CChoreoChannel;
class CChoreoScene;
class CChoreoEvent;
class CFlexAnimationTrack;

// How a CChoreoEvent_FACE event lets the actor strafe while it turns
enum class ChoreoStrafeMode_t : uint32
{
	DEFAULT = 0,
	ENABLE = 1,
	DISABLE = 2,
};

// The handshake an external animation graph asks for (CChoreoEvent_EXTERNAL_ANIMGRAPH)
enum class HandshakeTagType_t : int32
{
	eInvalid = -1,
	eTask = 0,
	eMovement,
	eCount,
};

// A movement gait (CChoreoEvent_LOCATOR::m_GaitOverride, eInvalid for none)
enum class SharedMovementGait_t : int8
{
	eInvalid = -1,
	eSlow = 0,
	eMedium,
	eFast,
	eVeryFast,
	eCount,
};

//-----------------------------------------------------------------------------
// A resource an event refers to (the animation clip of CChoreoEvent_ANIM_CLIP, the graph of
// CChoreoEvent_EXTERNAL_ANIMGRAPH, the reference clip of CChoreoEvent_LOCATOR) with what the
// event needs to notice a reload of it. Named by what it holds.
//-----------------------------------------------------------------------------
template < class T >
struct ChoreoResourceRef_t
{
	CStrongHandle< T > m_hResource;
	// Set by the constructor and by every assignment of the handle; while it is set the event
	// compares m_nReloadCounter with the binding and refreshes what it derived from the resource
	bool m_bWatchReload;
	// The reload counter of the binding when the handle was assigned or last refreshed
	int m_nReloadCounter;
	// True from the constructor until the handle is first assigned
	bool m_bDefaultConstructed;
};

//-----------------------------------------------------------------------------
// A named point in time of an event, as a fraction of its duration (the playback and the
// shifted tags of CChoreoEvent_GESTURE). 24 bytes.
//-----------------------------------------------------------------------------
struct CEventAbsoluteTag
{
	// "name"
	CUtlString m_Name;
	// "fraction"
	float m_flPercentage;
	bool m_bLocked : 1;
	bool m_bLinear : 1;
	bool m_bEntry : 1;
	bool m_bExit : 1;
	CChoreoEvent *m_pOwner;
};

//-----------------------------------------------------------------------------
// The base of every event. Its members are written and read as KV3 by CChoreoEvent_CommonBase
// (the keys are named in the comments). 184 bytes on both platforms; the last member ends at 180,
// so the Itanium layout puts the members of CChoreoEvent_LOOP and CChoreoEvent_REQUIREMENTS there.
//-----------------------------------------------------------------------------
abstract_class CChoreoEvent : public CChoreoObjectBase
{
public:
	// Named by what they do: write / read the event members (start_time, eventID, active,
	// end_time, param, ...)
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual ~CChoreoEvent() = 0;
	// Named by what it does: copies the members of other into this event, every level after its base
	virtual void CopyFrom( const CChoreoEvent &other ) = 0;
	// The event type number (20 for CChoreoEvent_MOOD, 31 for CChoreoEvent_ANIM_CLIP)
	virtual int GetType() const = 0;
	// Named by what it does: a one-line description ("Mood: %s", "Face %s", "Look at %s")
	virtual void GetDescription( CUtlString &sOut ) const = 0;
	// Named by what it does: a new event of the same class, a copy of this one
	virtual CChoreoEvent *Clone( void *pOwner ) const = 0;
	virtual void SetParameters( const char *pszParam ) = 0;
	virtual CUtlString GetParameters() const = 0;
	virtual CUtlString GetParameters2() const = 0;
	virtual CUtlString GetParameters3() const = 0;
	// Named by what it does: the symbol of the tool helper of the event ("choreohelper_default",
	// "choreohelper_animclip", "choreohelper_speak", ...)
	virtual CGlobalSymbol GetHelperName() const = 0;
	// Returns 0 by default, 1 in CChoreoEvent_REQUIREMENTS only; nothing in the server calls it
	virtual bool Stub_15() const = 0;
	// Whether the event is a requirement the scene waits for: true in CChoreoEvent_PULSE_REQUIREMENT
	// and CChoreoEvent_LOCATOR, the "make_requirement" attribute in CChoreoEvent_SEQUENCE, a test of
	// two of its flags in CChoreoEvent_ANIM_CLIP, false otherwise. Named by that attribute
	virtual bool IsRequirement() const = 0;
	// Snaps the start time, and the end time unless the event has a fixed length, to the frame
	// rate of the scene; a fixed-length event moves its end time with its start time. The events
	// with ramps and tracks snap those too. Named by what it does
	virtual void SnapTimes() = 0;
	// Called by SetStartTime and SetEndTime when the time changes: tells the requirements event
	// the event belongs to (OnRequiredEventTimeChanged); the gesture, sequence and speak events
	// also rescale their ramp to the new duration
	virtual void OnEndTimeChanged() = 0;
	// Clears the processing flag of the event; CChoreoEvent_LOOP also restores the remaining loop
	// count from the loop count. Named by what it does
	virtual void ResetProcessing() = 0;
	// Empty by default. CChoreoEvent_REQUIREMENTS drops the event from its list of required events
	// (and clears the event's link to it) when the event no longer starts within 0.2 s of it.
	// Named by what it does
	virtual void OnRequiredEventTimeChanged( CChoreoEvent *pEvent ) = 0;
	// Return 0 by default; CChoreoEvent_ANIM_CLIP and CChoreoEvent_EXTERNAL_ANIMGRAPH override both
	// with their resource (the animation clip / the graph): Stub_21 searches an array of 32-byte
	// entries for it, Stub_22 appends it to a list of 224-byte resource entries when it is not
	// loaded yet and returns whether it did. Nothing in the server calls them
	virtual bool Stub_21( const void *pEntries, int nCount ) const = 0;
	virtual bool Stub_22( void *pResourceList ) const = 0;

public:
	// "name"
	CUtlString m_Name;
	// "constrainedEventID": the id of the event this one is constrained to (0 for none); that
	// event lists this one in its m_ConstrainedEvents
	int m_nConstrainedEventID;
	// "eventID"
	int m_nEventID;
	// "start_time"
	float m_flStartTime;
	// "end_time"; -1 for an event without an end
	float m_flEndTime;
	CChoreoActor *m_pActor;
	CChoreoChannel *m_pChannel;
	// Copied by CopyFrom; the constructor clears them and sets the second one to -1. No other
	// reader or writer was found, so their meaning is not established
	int m_nReservedCopied[ 5 ];
	CChoreoScene *m_pScene;
	// The events a pause event (CChoreoEvent_SECTION) held back; the scene starts them when it
	// resumes
	CUtlVector< CChoreoEvent * > m_Dependencies;
	// The element type is not established: only the destructor touches this vector (it frees its
	// memory)
	CUtlVector< void * > m_ReservedVector;
	// Named by what it holds: the events whose m_nConstrainedEventID is this event's id; the
	// destructor clears their link
	CUtlVector< CChoreoEvent * > m_ConstrainedEvents;
	// Cleared by the constructor; nothing else reads or writes it
	uint8 m_Reserved[ 8 ];
	// "fixedlength"
	bool m_bFixedLength;
	// Whether the flex controllers of the tracks of a CChoreoEvent_FLEXANIMATION are looked up;
	// cleared when the tracks are loaded
	bool m_bTrackLookupSet;
	// Set when the scene starts the event, cleared by ResetProcessing
	bool m_bProcessing;
	// "active"; the tool attribute "Disabled" is its inverse
	bool m_bActive;
	// "holdevent" (written only by the events whose SupportsHoldEvent returns true)
	bool m_bHoldEvent;
	// "linked_requirement_id": the id of the CChoreoEvent_REQUIREMENTS event that lists this
	// event (-1 for none)
	int m_nLinkedRequirementID;
};

//-----------------------------------------------------------------------------
// Writes and reads the members of CChoreoEvent as KV3. No virtual method and no member of its
// own; the event classes CChoreoEvent_BaseLocalEvent and CChoreoEvent_BaseGlobalEvent derive
// from it.
//-----------------------------------------------------------------------------
abstract_class CChoreoEvent_CommonBase : public CChoreoEvent
{
};

//-----------------------------------------------------------------------------
// The base of the events that act on the actor locally (gestures, expressions, sequences,
// speech, look at, ...). No member of its own.
//-----------------------------------------------------------------------------
abstract_class CChoreoEvent_BaseLocalEvent : public CChoreoEvent_CommonBase
{
public:
	// Whether the event has the "holdevent" option: KV3TransferSave / KV3TransferLoad write and
	// read it only when this returns true. False by default; true in CChoreoEvent_LOOKAT, _FACE,
	// _LOCK_FACING, _DISABLE_BLINK, _IGNORE_EXTERNAL_LOOKATS, _PLAY_RESPONSE and the mood events.
	// Named by what it does
	virtual bool SupportsHoldEvent() const = 0;
};

//-----------------------------------------------------------------------------
// The base of the events of the scene itself (loops, pauses, stop points, requirements). No
// virtual method and no member of its own.
//-----------------------------------------------------------------------------
abstract_class CChoreoEvent_BaseGlobalEvent : public CChoreoEvent_CommonBase
{
};

//-----------------------------------------------------------------------------
// A local event with an intensity ramp over its duration. 248 bytes.
//-----------------------------------------------------------------------------
abstract_class CChoreoEvent_BaseLocalEventWithRamp : public CChoreoEvent_BaseLocalEvent
{
public:
	// "event_ramp_curve"
	CPiecewiseCurve m_Ramp;
};

//-----------------------------------------------------------------------------
// The base of the mood events (CChoreoEvent_MOOD, CChoreoEvent_MOOD_BODY). 192 bytes.
//-----------------------------------------------------------------------------
abstract_class CChoreoEvent_BaseMood : public CChoreoEvent_BaseLocalEvent
{
public:
	// Pure in the base; false in CChoreoEvent_MOOD, true in CChoreoEvent_MOOD_BODY. Nothing in the
	// server calls it
	virtual bool Stub_24() const = 0;

public:
	// "param": the mood name
	CUtlString m_Parameters;
};

//-----------------------------------------------------------------------------
// The concrete events. They override virtual methods only; each declares its own members.
//-----------------------------------------------------------------------------

// Plays an animation clip. 600 bytes.
abstract_class CChoreoEvent_ANIM_CLIP : public CChoreoEvent_BaseLocalEventWithRamp
{
public:
	// "m_Retiming": how the clip is stretched over the event
	struct Retiming_t
	{
		float m_flSimplePlaybackRate;
		// Number of times the looping part of the clip plays
		int m_nLoopCount;
		float m_flIntroPlaybackRate;
		float m_flLoopPlaybackRate;
		float m_flOutroPlaybackRate;
	};

	// Named by what it holds: one piece of the clip timeline built from m_Retiming
	struct ClipSegment_t
	{
		// Seconds from the start of the event
		float m_flStartTime;
		float m_flStartCycle;
		float m_flEndCycle;
	};

	// Named by what it holds: the pieces of the clip over the event and their total duration
	struct ClipSegments_t
	{
		CUtlVector< ClipSegment_t > m_Segments;
		float m_flDuration;
	};

	// "m_hAnimClip"
	ChoreoResourceRef_t< InfoForResourceTypeCNmClip > m_hAnimClip;
	Retiming_t m_Retiming;
	// "animclip_public_name"; "external_animclip_name" by default
	CUtlString m_sAnimClipPublicName;
	// "clip_is_public"
	bool m_bClipIsPublic;
	// "is_requirement_condition" (the tool attribute "make_requirement")
	bool m_bIsRequirementCondition;
	// "animclip_is_looping"
	bool m_bAnimClipIsLooping;
	// Named by what it holds: the name of the clip resource, refreshed from m_hAnimClip when asked
	CResourceName m_AnimClipName;
	ClipSegments_t m_ClipSegments;
	// The rest is derived from the clip when it changes (m_bClipInfoDirty). Named by what they
	// hold: whether the clip is public (its length is the event's)
	bool m_bTimedByEvent;
	// Whether the clip has an "AE_CHOREO_LOOP" event (m_flLoopStartCycle .. m_flLoopEndCycle)
	bool m_bHasLoopEvent;
	// Whether the channel is "AnimClips:choreo_fullbody_takeover"
	bool m_bFullBodyTakeover;
	float m_flClipDuration;
	float m_flLoopStartCycle;
	float m_flLoopEndCycle;
	// 1 - m_flLoopEndCycle
	float m_flOutroCycleFraction;
	// m_flLoopEndCycle - m_flLoopStartCycle
	float m_flLoopCycleFraction;
	// Set by SnapTimes, OnEndTimeChanged and CopyFrom: the values above are computed again
	bool m_bClipInfoDirty;
};

// Drives an animation graph controller parameter. 200 bytes.
abstract_class CChoreoEvent_ANIMGRAPH_CONTROLLER : public CChoreoEvent_BaseLocalEvent
{
public:
	// "param"
	CUtlString m_Parameters;
	// "param2"
	CUtlString m_Parameters2;
};

// Disables the automatic blinking of the actor. No member of its own (184 bytes).
abstract_class CChoreoEvent_DISABLE_BLINK : public CChoreoEvent_BaseLocalEvent
{
};

// A facial expression. 264 bytes.
abstract_class CChoreoEvent_EXPRESSION : public CChoreoEvent_BaseLocalEventWithRamp
{
public:
	// "param": the expression group
	CUtlString m_Parameters;
	// "param2": the expression name
	CUtlString m_Parameters2;
};

// Runs an external animation graph in a slot of the actor's graph. 464 bytes.
abstract_class CChoreoEvent_EXTERNAL_ANIMGRAPH : public CChoreoEvent_BaseLocalEvent
{
public:
	// "animgraph_resource"
	ChoreoResourceRef_t< InfoForResourceTypeCNmGraphDefinition > m_hAnimGraphResource;
	// "animgraph_public_name"; "external_animgraph_name" by default
	CUtlString m_sAnimGraphPublicName;
	// "animgraph_slot_id"; "external_graph_choreo_a" by default
	CUtlString m_sAnimGraphSlotID;
	// "copy_unbound_params_from_actor"
	CUtlString m_sCopyUnboundParamsFromActor;
	// "graph_is_public"
	bool m_bGraphIsPublic;
	// "wants_handshake"; true by default
	bool m_bWantsHandshake;
	// "handshake_enum"
	HandshakeTagType_t m_HandshakeEnum;
	// Named by what it holds: the name of the graph resource, refreshed from the handle
	CResourceName m_AnimGraphName;
};

// Controls an external animation graph started by CChoreoEvent_EXTERNAL_ANIMGRAPH. 208 bytes.
abstract_class CChoreoEvent_EXTERNAL_ANIMGRAPH_CONTROL : public CChoreoEvent_BaseLocalEvent
{
public:
	// "animgraph_choreo_control_state"; "None" by default
	CUtlString m_sAnimGraphChoreoControlState;
	// "animgraph_slot_id"; "external_graph_choreo_a" by default
	CUtlString m_sAnimGraphSlotID;
	// "reset_on_end"
	bool m_bResetOnEnd;
};

// Turns the actor to face a target. 264 bytes.
abstract_class CChoreoEvent_FACE : public CChoreoEvent_BaseLocalEventWithRamp
{
public:
	// "param": the target
	CUtlString m_Parameters;
	// "strafe_mode"
	ChoreoStrafeMode_t m_StrafeMode;
};

// Fires a named trigger. 192 bytes.
abstract_class CChoreoEvent_FIRETRIGGER : public CChoreoEvent_BaseLocalEvent
{
public:
	// "param": the trigger name
	CUtlString m_Parameters;
};

// Flex controller animation tracks. 272 bytes.
abstract_class CChoreoEvent_FLEXANIMATION : public CChoreoEvent_BaseLocalEventWithRamp
{
public:
	// "flexanimations": the tracks, owned by the event
	CUtlVector< CFlexAnimationTrack * > m_FlexAnimationTracks;
};

// Plays a gesture. 320 bytes.
abstract_class CChoreoEvent_GESTURE : public CChoreoEvent_BaseLocalEventWithRamp
{
public:
	// "sequenceduration"
	float m_flGestureSequenceDuration;
	// "playback_time" and "shifted_time"
	CUtlVector< CEventAbsoluteTag > m_AbsoluteTags[ 2 ];
	// "param": the gesture name
	CUtlString m_Parameters;
	// "synctofollowinggesture"
	bool m_bSyncToFollowingGesture;
};

// Makes the actor ignore look-at requests from outside the scene. No member of its own (248 bytes).
abstract_class CChoreoEvent_IGNORE_EXTERNAL_LOOKATS : public CChoreoEvent_BaseLocalEventWithRamp
{
};

// Interrupts the scene. 192 bytes.
abstract_class CChoreoEvent_INTERRUPT : public CChoreoEvent_BaseLocalEvent
{
public:
	// "param"
	CUtlString m_Parameters;
};

// Locks the body facing of the actor. No member of its own (184 bytes).
abstract_class CChoreoEvent_LOCK_FACING : public CChoreoEvent_BaseLocalEvent
{
};

// Makes the actor look at a target. 360 bytes.
abstract_class CChoreoEvent_LOOKAT : public CChoreoEvent_BaseLocalEvent
{
public:
	// "param": the target
	CUtlString m_Parameters;
	// "param2": the look type ("chest" by default)
	CUtlString m_Parameters2;
	// "param3": the speed ("medium" by default)
	CUtlString m_Parameters3;
	// "additive"
	bool m_bAdditive;
	// "event_ramp_curve"
	CPiecewiseCurve m_Ramp;
	// "separate_eye_ramp"
	bool m_bUseSeparateEyeRamp;
	// "eye_ramp"
	CPiecewiseCurve m_EyeRamp;
	// "max_head_weight"; 0.7 by default
	float m_flMaxHeadWeight;
	// "max_eye_weight"; 1 by default
	float m_flMaxEyeWeight;
};

// Loops the scene back to a time. 192 bytes.
abstract_class CChoreoEvent_LOOP : public CChoreoEvent_BaseGlobalEvent
{
public:
	// "loopcount"; -1 loops forever
	int m_nNumLoops;
	// Restored from m_nNumLoops by ResetProcessing
	int m_nLoopsRemaining;
};

// Moods: no member of their own (192 bytes).
abstract_class CChoreoEvent_MOOD : public CChoreoEvent_BaseMood
{
};

abstract_class CChoreoEvent_MOOD_BODY : public CChoreoEvent_BaseMood
{
};

// Lets the actor answer with responses while it lasts. No member of its own (184 bytes).
abstract_class CChoreoEvent_PERMIT_RESPONSES : public CChoreoEvent_BaseLocalEvent
{
};

// Plays a response of the response system. 200 bytes.
abstract_class CChoreoEvent_PLAY_RESPONSE : public CChoreoEvent_BaseLocalEvent
{
public:
	// "concept"
	CUtlString m_sConcept;
	// "criteria"
	CUtlString m_sCriteria;
};

// A requirement a Pulse graph resolves. 192 bytes.
abstract_class CChoreoEvent_PULSE_REQUIREMENT : public CChoreoEvent_BaseLocalEvent
{
public:
	// "param": the requirement name ("requirement_name" by default)
	CUtlString m_Parameters;
};

// Groups the requirement events the scene waits for. 208 bytes on Linux, 216 on Windows.
abstract_class CChoreoEvent_REQUIREMENTS : public CChoreoEvent_BaseGlobalEvent
{
public:
	// "wait_until_end"
	bool m_bWaitUntilEnd;
	// "linked_events": the ids ("event_id") of the required events
	CUtlVector< int > m_linkedEventIDs;
};

// Pauses the scene. 192 bytes.
abstract_class CChoreoEvent_SECTION : public CChoreoEvent_BaseGlobalEvent
{
public:
	// "param" (the tool attribute "Automated Action")
	CUtlString m_Parameters;
};

// Plays a sequence. 288 bytes.
abstract_class CChoreoEvent_SEQUENCE : public CChoreoEvent_BaseLocalEventWithRamp
{
public:
	// Cleared by the constructor; nothing else reads or writes it
	bool m_bReserved;
	// "animsequence_public_name"; "external_animsequence_name" by default
	CUtlString m_sAnimSequencePublicName;
	// "sequence_is_public"; a missing public sequence is not reported
	bool m_bSequenceIsPublic;
	// "param": the sequence name
	CUtlString m_Parameters;
	// "playoverscript"
	bool m_bPlayOverScript;
	// "is_requirement_condition" (the tool attribute "make_requirement")
	bool m_bIsRequirementCondition;
	// "reset_cycle_on_time_change"; true by default
	bool m_bResetCycleOnTimeChange;
};

// Plays a sound. 272 bytes.
abstract_class CChoreoEvent_SPEAK : public CChoreoEvent_BaseLocalEventWithRamp
{
public:
	// "param": the sound event
	CUtlString m_Parameters;
	// "cc_noattenuate"
	bool m_bSuppressCaptionAttenuation;
	// "hardstopspeakevent"
	bool m_bHardStopSpeakEvent;
	// "startdelay"
	float m_flStartDelay;
	// "volumematcheseventramp"
	bool m_bVolumeMatchesEventRamp;
	// "suppresslipsync"
	bool m_bSuppressLipsync;
};

// Stops the scene. 192 bytes.
abstract_class CChoreoEvent_STOPPOINT : public CChoreoEvent_BaseGlobalEvent
{
public:
	// "param"
	CUtlString m_Parameters;
};

// Moves or teleports the actor to a place relative to an anchor. 320 bytes.
abstract_class CChoreoEvent_LOCATOR : public CChoreoEvent_BaseLocalEvent
{
public:
	// "should_teleport"
	bool m_bShouldTeleport;
	// "anchor_is_external"
	bool m_bAnchorIsExternal;
	// "anchor_actor_name"
	CUtlString m_sAnchorActorName;
	// "relative_position"
	Vector m_vRelativePosition;
	// "relative_orientation"
	QAngle m_angRelativeOrientation;
	// "post_requirement_duration"
	float m_flPostRequirementDuration;
	// The constructor sets it to -1; nothing else reads or writes it
	float m_flReserved;
	// "gait_override"; -1 for none
	SharedMovementGait_t m_GaitOverride;
	// "m_hWorldOffsetReferenceClip"
	ChoreoResourceRef_t< InfoForResourceTypeCNmClip > m_hWorldOffsetReferenceClip;
	// Named by what it holds: the name of the reference clip, refreshed from the handle
	CBufferString m_WorldOffsetReferenceClipName;
	// Set by the constructor and CopyFrom: m_WorldOffset is computed again
	bool m_bWorldOffsetDirty;
	// Named by what it holds: the "world_OFFSET" transform of the reference clip, or the relative
	// position and orientation without a clip
	CTransform m_WorldOffset;
};

#endif // CHOREOEVENT_H
