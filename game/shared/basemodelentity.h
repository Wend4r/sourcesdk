#ifndef BASEMODELENTITY_H
#define BASEMODELENTITY_H

#pragma once

#include "../server/baseentity.h"

class CSceneEventInfo;
class CTakeDamageInfo;
struct CTakeDamageResult;
struct gamevcollisionevent_t;

//-----------------------------------------------------------------------------
// CBaseModelEntity adds 33 methods to CBaseEntity: Linux slots 239-271, Windows 238-270.
//
// Damage runs through two methods of CBaseEntity. RejectDamage asks RejectDamage_Common and then, by
// the life state, RejectDamage_Alive, RejectDamage_Dying or RejectDamage_Dead; OnTakeDamage
// calls OnTakeDamage_Common and then OnTakeDamage_Alive, OnTakeDamage_Dying or OnTakeDamage_Dead.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CBaseModelEntityVTable : public TBase
{
public:
	// Empty in every class, on the client too; called by CBodyGameSystem::NotifyResourceStatusChange for the owner of a body component when its model is reloaded (RESOURCE_LOAD_RELOAD). Named by its caller
	virtual void OnModelReloaded() = 0;
	// Adds key values of its own to a break piece being spawned from this entity; CRagdollProp writes massScale and buoyancyScale. Named by what it does
	virtual void AddBreakPieceKeyValues( CEntityKeyValues *pKeyValues ) = 0;
	// Called on the broken entity after a break piece entity was spawned, with the 144-byte record of the break list and the piece; CPhysicsProp gives the piece the NoPhysicsDust attribute when it has it itself, CRagdollProp overrides with an empty body. Named by what it does
	virtual void OnBreakPieceCreated( const void *pPieceInfo, CBaseEntity *pPiece ) = 0;
	// Null; CBaseAnimGraph looks up the CPhysicsBodyGameMarkupData of the model by the bone name of the body, the same code on the client. No caller found, the argument type is inferred. Named by what it does
	virtual const void *GetPhysicsBodyGameMarkup( void *pPhysicsBody ) = 0;
	// Starts (phase 0), processes (1) or clears (2) a choreo scene event; CBaseAnimGraph runs it through its choreo component, false elsewhere. CChoreoComponent::AddSceneEvent drops the event when phase 0 fails, as CBaseFlex::AddSceneEvent of Source 1 does with StartSceneEvent; the flags follow ClearSceneEvent of Source 1. Named by what it does
	virtual bool HandleSceneEvent( CSceneEventInfo *pInfo, int16 nPhase, bool bFastKill, bool bCanceled ) = 0;
	// Returns true in every class, on the client too; CChoreoComponent::AddSceneEvent asks it first and adds the event only when it is true. Named by its caller
	virtual bool AllowsSceneEvents() = 0;
	// Called by the think CBaseModelEntity::ProcessSceneEventsThink: false (stop) on the server, the client decays the flex weights by 0.95 while any is above zero. CBaseFlex::ProcessSceneEvents of Source 1 and Dota 2 2018
	virtual bool ProcessSceneEvents() = 0;
	// Allocates the choreo component, null when m_pChoreoComponent is set; never overridden. Named by what it does
	virtual void *CreateChoreoComponent() = 0;
	// Returns true in every class, on the client too; the destructible parts ask it when a part reaches a damage level of type 1, and false keeps one point of health on that level. Named by its caller
	virtual bool AllowDestructiblePartDamageLevel( int nHitGroup, int nDamageLevel, const void *pDamageLevelInfo, void *pUnknown ) = 0;
	// Empty in every class
	virtual void Stub_247() = 0;
	// Stores the arguments and fires m_OnDestructibleHitGroupDamageLevelChanged; overridden by CBaseAnimGraph with the same code. Named by what it does
	virtual void FireOnDestructibleHitGroupDamageLevelChanged( const void *pArgs ) = 0;
	// Never overridden
	virtual void SetModelScale( float flScale ) = 0;
	// Does nothing unless Stub_251 answers true; then it applies the negated contact impulses of the event back to the physics body when that body is dynamic. Called by VPhysicsCollision of CBreakable, CPhysicsProp and CShatterGlassShardPhysics; never overridden. The name of an earlier pass, which the body does not confirm
	virtual void VPhysicsCollisionDestructibleParts( int nIndex, gamevcollisionevent_t *pEvent ) = 0;
	// Returns false in every class; VPhysicsCollisionDestructibleParts asks it first and does nothing while it is false
	virtual bool Stub_251() = 0;
	// What RejectDamage asks first, whatever the life state: non-zero rejects the damage. The tank train and the rope answer by their flags. Named by what it does
	virtual int RejectDamage_Common( const CTakeDamageInfo &info ) = 0;
	// Called by OnTakeDamage before the life state methods; hands the damage to the destructible parts, CCSPlayerPawn records the damage and the bullet force. Named by what it does
	virtual void OnTakeDamage_Common( const CTakeDamageResult &result ) = 0;
	// What RejectDamage asks about an entity that is alive, as OnTakeDamage calls OnTakeDamage_Alive. Named by what it does
	virtual int RejectDamage_Alive( const CTakeDamageInfo &info ) = 0;
	// The base class only notes that it was reached, which OnTakeDamage expects of every override
	virtual void OnTakeDamage_Alive( CTakeDamageResult *pResult ) = 0;
	// What RejectDamage asks about an entity that is dying; never overridden. Named by what it does. Called for a chicken that was shot while it was dying
	virtual int RejectDamage_Dying( const CTakeDamageInfo &info ) = 0;
	// Empty, never overridden
	virtual void OnTakeDamage_Dying( CTakeDamageResult *pResult ) = 0;
	// What RejectDamage asks about a dead entity; never overridden. Named by what it does
	virtual int RejectDamage_Dead( const CTakeDamageInfo &info ) = 0;
	// Empty, never overridden
	virtual void OnTakeDamage_Dead( CTakeDamageResult *pResult ) = 0;
	// The name of the decal a hit leaves: BulletProof, ManhackCut, GlassBreak
	virtual CGlobalSymbol DamageDecal( int bitsDamageType, int gameMaterial ) = 0;
	// True; false in CModelPointEntity (and C_ModelPointEntity, C_PlayerSprayDecal, C_PointWorldText on the client). SpawnShared reads the mins and maxs key values into the collision bounds only when it is true. Named by its caller
	virtual bool UsesKeyValueCollisionBounds() = 0;
	// Called at the end of the model setter SetModel uses: restarts the attached_soundevents of the model; CCSWeaponBase runs the same code, the client overrides re-cache what depends on the model. Dota 2018 names the client method OnNewModel, a name CBaseEntity already uses for its slot 52. Named by its caller
	virtual void OnModelChanged() = 0;
	// True unless EFL 0x20 is set or, in a single-player game, the local player sees the entity and m_bAllowFadeInView is off. CBaseEntity::SUB_AllowedToFade of Source 1, CBaseModelEntity::SUB_AllowedToFade of Dota 2 2018
	virtual bool SUB_AllowedToFade() = 0;
	// Returns false; CBaseAnimGraph returns m_bClientSideRagdoll, CRagdollProp true
	virtual bool IsRagdoll() = 0;
	// Ignite goes on only when this is true or its flags carry 4; CBreakableProp and CPhysicsProp answer by the flammable and ignite-at-half-health interactions of their prop data unless already on fire. CAI_BaseNPC::AllowedToIgnite of Source 1, which CBaseAnimating::Ignite asks
	virtual bool AllowedToIgnite() = 0;
	// The Ignite inputs call it
	virtual void Ignite( uint32 nFlags, CBaseEntity *pUnknown, float flLifetime, float flUnknown1, float flUnknown2, float flUnknown3 ) = 0;
	// Calls Ignite with this lifetime; never overridden
	virtual void IgniteLifetime( float flFlameLifetime ) = 0;
	// Calls Ignite with a lifetime of 30 seconds; never overridden
	virtual void IgniteUseCheapEffect( bool bUseCheapEffect ) = 0;
	// Removes FL_ONFIRE; never overridden
	virtual void Extinguish() = 0;
	// Returns -1 in every class
	virtual int Stub_270() = 0;
};

#endif // BASEMODELENTITY_H
