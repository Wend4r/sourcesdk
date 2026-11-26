#ifndef BASEANIMGRAPH_H
#define BASEANIMGRAPH_H

#pragma once

#include "basemodelentity.h"
#include "animlib/graph/animgraph_definition.h"

class CAnimGraphControllerBase;
class CAnimGraphControllerManager;
struct animevent_t;

//-----------------------------------------------------------------------------
// CBaseAnimGraph adds 35 methods to CBaseModelEntity: Linux slots 272-306, Windows 271-305.
//-----------------------------------------------------------------------------
template < class TBase = CBaseModelEntityVTable<> >
abstract_class CBaseAnimGraphVTable : public TBase
{
public:
	// Empty in every class
	virtual void Stub_271() = 0;
	// Empty in every class
	virtual void Stub_272() = 0;
	// Adds the extra graph controllers of the entity to m_graphControllerManager; CCSPlayerPawn creates its CCS2PawnGraphController, CBasePlayerPawn passes the call to its components. CBaseAnimGraph::Connect calls it after CreateMainGraphController. Named by what it does. Called while the entity is constructed, before Connect
	virtual void AddGraphControllers( CAnimGraphControllerManager *pManager ) = 0;
	// Creates the main graph controller, which CBaseAnimGraph::Connect stores in m_pMainGraphController; null here, CDynamicProp creates a CDynamicPropGraphController when m_bGraphControllerEnabled, CChicken a controller of its own. Named by the member it fills. Called by Connect
	virtual CAnimGraphControllerBase *CreateMainGraphController( CAnimGraphControllerManager *pManager ) = 0;
	// 0.015625 (one 64 Hz tick) on the server and the client; SetPlaybackRate of the animation controller subtracts it from the current time when it re-bases the sequence start tick. Named by its caller
	virtual float GetPlaybackRateRebaseTime() = 0;
	// Advances the animation controller: 2 without an animation, 0 in mode 3, for type 2 or without simulation time, else the result of FrameAdvance of the controller; called for every entity of the update list by the AnimGraph tick update (CAnimGraphGameSystem::AnimTickUpdate, whose argument type AnimationAdvanceType_t the client RTTI names). Named by what it does
	virtual int FrameAdvance( int nAdvanceType ) = 0;
	// 0 on the server; the client props return 1 for client-only entities. The value is stored with the entity in the animation update list (AnimUpdateListEntry_t) when its update is scheduled. Named by its caller
	virtual int16 GetAnimUpdateListFlags() = 0;
	// Called by SetPlaybackRate of the animation controller after the rate changed, with the old rate; CDynamicProp (and C_DynamicProp) restarts its think when the direction of play flips. Named by its caller
	virtual void OnPlaybackRateChanged( float flOldRate ) = 0;
	// Empty here; the animation controller calls it when m_nNotifyState is eNotifyTriggered (it resets it to eDoNotNotify first), C_CSGO_PreviewModel starts its desired sequence.
	virtual void OnSequenceFinished( bool bUnknown ) = 0;
	// Overridden by CDynamicProp, CBasePropDoor and CBasePlayerWeapon
	virtual void HandleAnimEvent( animevent_t *pEvent ) = 0;
	// Empty in every class, on the client too; IChoreoServices::CineCleanup calls it with its own argument on the entity AsBaseAnimGraph returns, then clears its target
	virtual void Stub_281( bool bCanceled ) = 0;
	// Allocates a CGameChoreoServices; never overridden. Named by what it does
	virtual void *CreateChoreoServices() = 0;
	// An AnimationAlgorithm_t, which the animation controller stores in m_nAnimationAlgorithm: 2 (animation graph) here, CDynamicProp 1 or 2 by m_bUseAnimGraph, the weapons 3 in some gameplay animation states and 1 otherwise, CRagdollProp 0; the same on the client. Named by the member it fills
	virtual uint8 GetAnimationAlgorithm() = 0;
	// Returns false; CBasePlayerPawn returns true, on the client C_BasePlayerPawn. Named by what it does
	virtual bool IsPlayerPawnAnimGraph() = 0;
	// Writes the time into m_flAnimTime and m_flSimulationTime (the latter 0 when a global mode flag is 1); called by the AnimGraph tick update after the frame advance, for entities driven by the animation graph. Named by what it does
	virtual void UpdateAnimTime( float flTime ) = 0;
	// Returns true in every class
	virtual bool Stub_286() = 0;
	// The position of the attachment of that name, else of the bone of that name, else the origin; never overridden, the same function on the client. Named by what it does
	virtual Vector GetAttachmentOrBonePosition( const char *pszName ) = 0;
	// The entry \"player\" of the footstep table; CBasePlayerPawn asks its movement services first
	virtual const void *GetFootstepEntry() = 0;
	// Null here and on the client; the AG2 contact event consumer (CNmEventConsumerContact) asks it for the entity a contact event refers to. Named by its caller
	virtual CBaseEntity *GetContactEventTarget( const void *pContact ) = 0;
	// Returns null or false in every class.
	virtual void *Stub_290() = 0;
	// False on the server; on the client only C_ClientRagdoll returns true.
	virtual bool IsClientRagdoll() = 0;
	// Sets m_bClientSideRagdoll and m_vecForce when CanBecomeRagdoll or the second argument allows it; called by BecomeRagdoll of the pawn. Never overridden
	virtual bool BecomeRagdollOnClient( const Vector &vecForce, bool bForce ) = 0;
	// True when the model has more than one physics body and FL_TRANSRAGDOLL is not set; CCSPlayerPawn returns true. Called by Event_Killed of the pawn
	virtual bool CanBecomeRagdoll() = 0;
	// Copies the force unchanged; BecomeRagdollOnClient stores the result in m_vecForce. Never overridden. Called by BecomeRagdoll of the pawn
	virtual void ClampRagdollForce( const Vector &vecForce, Vector *pResult ) = 0;
	// Empty in every class
	virtual void Stub_295() = 0;
	// Copies m_collisionAttribute of m_Collision as it is stored; never overridden. GetCollisionAttribute of CBaseEntity returns the same data. Named by what it does
	virtual RnCollisionAttr_t GetStoredCollisionAttribute() = 0;
	// Stores *pValue into m_bRagdollClientSide with the network state change; the client also tears the client ragdoll down when the value turns false. No server caller found. Named by what it does
	virtual void SetRagdollClientSide( void *pUnused, const bool *pValue ) = 0;
	// Empty in every class; called at the end of the function that destroys m_pRagdollControl and the ragdoll and clears m_RagdollPose (from OnNewModel and UpdateOnRemove). Named by its caller
	virtual void OnRagdollControlDestroyed() = 0;
	// Called by the AnimGraph tick update before the update of the graph controller manager, for entities driven by the animation graph; passes the call to the animation controller, which drops dead handles from m_vecSecondarySkeletons. Named by its caller
	virtual void PreAnimGraphUpdate() = 0;
	// Called by the AnimGraph tick update right after the post-update of the graph controller manager; empty here, CChicken (and C_Chicken) turns by the root motion the graph produced. Named by the chicken override
	virtual void ApplyRootMotion() = 0;
	// Called by the AG2 EvaluateAnimList job after the graph instance was updated and evaluated, with data of the update list entry; passes it to the animation controller (m_pGraphInstanceAG2, m_vecSecondarySkeletons). Named by its caller
	virtual void OnAnimGraphEvaluated( void *pUpdateEntryData ) = 0;
	// Called by the animation controller after it reset m_graphControllerManager and re-added the entity to the update list, when the algorithm is the animation graph; the base sets the global flag the caller checks, on the client C_Chicken and C_CSGO_PreviewPlayer recreate their graph controllers here. Named by its caller
	virtual void OnGraphControllersReset() = 0;
	// Empty in every class
	virtual void Stub_303() = 0;
	// Returns null or false in every class
	virtual void *Stub_304() = 0;
	// Destroys and recreates the AG2 event consumers of the entity (CNmEventConsumerLegacy, Particle, Sound, Attributes, BodyGroup, Pulse); CCSPlayerPawn adds its own. C_CSPlayerPawn::AG2_CreateEventConsumers of the client RTTI
	virtual void AG2_CreateEventConsumers() = 0;
};

#include "schemacompiler/schemaon.h"

// The variation user data of the animation graph of an entity (8 bytes: the virtual table pointer)
schema class CBaseAnimGraphVariationUserData : public CNmGraphVariationUserData
{
public:
	TYPEMETA( MGetKV3ClassDefaults );
};

#include "schemacompiler/schemaoff.h"

#endif // BASEANIMGRAPH_H
