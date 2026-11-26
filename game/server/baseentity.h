#ifndef BASEENTITY_H
#define BASEENTITY_H

#pragma once

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier0/utlsymbollarge.h"
#include "tier0/globalsymbol.h"
#include "mathlib/vector.h"
#include "mathlib/mathlib.h"
#include "playerslot.h"
#include "entity2/entityinstance.h"
#include "resourcesystem/resourcehandle.h"
#include "resourcesystem/resourcehandletypes.h"

class CBaseEntity;
class CBaseModelEntity;
class CBaseAnimGraph;
class CBaseCombatCharacter;
class CBasePlayerWeapon;
class CCollisionProperty;
class CEntityKeyValues;
class CCheckTransmitInfo;
class CGameTrace;
class CPulseGraphInstance_ServerEntity;
class CResponseCriteriaSet;
class CTakeDamageInfo;
class INextBot;
class IResponseSystem;
class ServerClass;
struct CTakeDamageResult;
struct InputData_t;
struct RnCollisionAttr_t;
struct gamevcollisionevent_t;

//-----------------------------------------------------------------------------
// Virtual table of the server CBaseEntity: the methods it adds to
// CEntityInstance, 190 of them.
//
// The class declares virtual methods only and carries a VTable suffix, see
// playerpawn_component.h for how a module derives its own CBaseEntity from it.
//
// The methods are in declaration order, which is the order of the Linux table,
// where they take slots 49-238. The Windows table has them at 48-237: the slot
// is the Linux one plus one, except that
//   - two methods declared near the end sit second and fourth on Windows;
//   - the two overloads of a method swap places on Windows, as the compiler
//     lays overloads out in reverse.
// The numbers in the Unk_ names are Windows slots.
//
// The Unk_ methods are the ones without a name.
//-----------------------------------------------------------------------------
abstract_class CBaseEntityVTable : public CEntityInstance
{
public:
	// The server class of the entity class, or of the nearest base class that has one; nearly every class overrides it
	virtual ServerClass *GetServerClass() = 0;
	// Opens the name Stub_49 here for MSVC; never defined, see the end of the class
private:
	void Stub_49( int ) const;
public:
	// Reads disableshadows, disablereceiveshadows, nodamageforces and ownername
	virtual void SpawnShared( const CEntityKeyValues *pKeyValues ) = 0;
	// Opens the name Stub_51 here for MSVC; never defined, see the end of the class
private:
	void Stub_51( int ) const;
public:
	virtual void PopulatePoseParameters() = 0;
	// Called by CModelState::SetupModel
	virtual void OnNewModel( CWeakHandle< InfoForResourceTypeCModel > hOldModel, CWeakHandle< InfoForResourceTypeCModel > hNewModel ) = 0;
	virtual void SetOwnerEntity( CBaseEntity *pOwner ) = 0;
	virtual CBaseModelEntity *GetBaseModelEntity() = 0;
	virtual const CBaseModelEntity *GetBaseModelEntity() const = 0;
	virtual CBaseAnimGraph *GetBaseAnimGraph() = 0;
	virtual const CBaseAnimGraph *GetBaseAnimGraph() const = 0;
	// Returns null in every class, on the client too: the game has no class that answers it.
	virtual void *GetBaseAnimatingOverlay() = 0;
	// The const overload, laid out before the other one on Windows; null in every class.
	virtual const void *GetBaseAnimatingOverlay() const = 0;
	// Returns null; CBaseAnimGraph returns itself, as from GetBaseAnimGraph, on the client too. The anim graph components reach it through the owner of their scene node and read the result as a CBaseAnimGraph (the graph controller manager, the ragdoll). Named by what it does: what tells this pair from GetBaseAnimGraph is not known. Called every tick for pawns and weapons
	virtual CBaseAnimGraph *AsBaseAnimGraph() = 0;
	// The const overload, laid out before the other one on Windows. GetPhysicsBodyIndex and the first-physics-body helper call it when their ragdoll flag is set and search the ragdoll physics of the result (the pointer before m_pRagdollControl); C_BaseEntity::GetPhysicsBodyIndex of the client does the same with its slot. Called every tick for pawns and weapons
	virtual const CBaseAnimGraph *AsBaseAnimGraph() const = 0;
	// Returns null or false in every class. A pair that returns null
	virtual void *Stub_63() = 0;
	// Returns null or false in every class
	virtual void *Stub_64() = 0;
	virtual int Classify() = 0;
	// 4 without a target
	virtual int IRelationType( CBaseEntity *pTarget ) = 0;
	virtual int IRelationPriority( CBaseEntity *pTarget ) = 0;
	// A row of the class relationship table; overridden by the hostage and the pawns
	virtual void *FindEntityRelationship( CBaseEntity *pTarget ) = 0;
	// Returns m_pCollision
	virtual CCollisionProperty *CollisionProp() = 0;
	virtual bool ShouldSavePhysics() = 0;
	virtual bool CreateVPhysics() = 0;
	// Builds a scene object out of the collision shapes; CBaseModelEntity queues the call
	virtual void DrawVCollide( bool bUnknown, float flDuration ) = 0;
	// Drops the physics of the scene node; called by UpdateOnRemove of the pawns
	virtual void VPhysicsDestroyObject() = 0;
	virtual void VPhysicsUpdate( void *pPhysics ) = 0;
	// The index of a physics body among the bodies of the entity, -1 when it is not one of them; never overridden. Named by what it does. Asked by SetGroundEntity on the ground entity, the answer goes to m_nGroundBodyIndex
	virtual int GetPhysicsBodyIndex( void *pPhysicsBody, bool bUnknown ) = 0;
	virtual void VPhysicsTakeDamage( const CTakeDamageResult &result ) = 0;
	// The mass of the body divided by the total mass of the moveable bodies: the share of the damage impulse VPhysicsTakeDamage gives that body; the ragdolls return a convar instead when it is not negative. Named by what it does
	virtual float GetDamageForceMassFraction( void *pPhysicsBody, float flTotalMass ) = 0;
	// Empty; CBaseCombatCharacter deals the crush damage here
	virtual void VPhysicsShadowCollision( int nIndex, gamevcollisionevent_t *pEvent ) = 0;
	// Calls VPhysicsCollisionEffects, then clears a flag of both entities once they have come to rest
	virtual void VPhysicsCollision( int nIndex, gamevcollisionevent_t *pEvent ) = 0;
	// The collision sound, screen shake and dust; never overridden. Named by what it does
	virtual void VPhysicsCollisionEffects( int nIndex, gamevcollisionevent_t *pEvent ) = 0;
	// Empty in every class. The contact callback CPhysicsGameSystem::GameWorldInit registers with the physics world calls it on the entity of each body of a reporting contact, with the index of the body (0 or 1) and the contact.
	virtual void Stub_81( int nIndex, const void *pContact ) = 0;
	virtual void UpdatePhysicsShadowToCurrentPosition( float flDeltaTime ) = 0;
	virtual bool ForceVPhysicsCollide( CBaseEntity *pEntity ) = 0;
	// Returns an empty handle in every class. CModelState::DoSetupPhysics uses it, when not empty, as the physics aggregate data instead of the one of the model and releases it through the physics system. The return value is an 8-byte reference-counted handle returned through a hidden pointer; CUtlString stands in for it in the ABI. Named by what it does
	virtual CUtlString GetPhysicsAggregateDataOverride() = 0;
	// Named by what it does: the pawn of a player returns m_nInteractsAs 0x60000 and m_nInteractsWith 0x2C3011. Copies the interaction layers and ids out of the collision property, never overridden
	virtual RnCollisionAttr_t GetCollisionAttribute() = 0;
	// Refreshes the cached move type, never overridden. Named by what it does. Called by SetMoveType and when the hierarchy of the scene node changes
	virtual void UpdateActualMoveType() = 0;
	// True for move type 5 (VPHYSICS), asked by SetAbsOrigin before it moves the physics; overridden by the ragdolls. Named by what it does
	virtual bool IsPhysicsMoveType( uint8 nMoveType ) = 0;
	// Returns null or false in every class. Every caller skips a position change when it returns true: SetAbsOrigin, the Pulse binding SetPosition and the transform update helpers
	virtual bool Stub_88() = 0;
	virtual Vector WorldSpaceCenter() const = 0;
	// Empty; overridden by sprites and lights
	virtual void ComputeWorldSpaceSurroundingBox( Vector *pWorldMins, Vector *pWorldMaxs ) = 0;
	// Empty; CCSPlayerPawn refreshes its physics here. Called by CCollisionProperty::SetCollisionBounds after the new mins and maxs are stored. Named by what it does
	virtual void OnCollisionBoundsChanged() = 0;
	// Stores m_nPlatformType; CBaseAnimGraph passes it on
	virtual void SetPlatformType( uint8 nPlatformType ) = 0;
	// Returns null in every class: the entity whose frame the physics of this one moves in. CPhysicsProp::VPhysicsUpdate sets m_bAttachedToReferenceFrame when the move parent is this entity, a ragdoll follows its teleports, Teleport keeps the ground entity while it is set. Named after m_bAttachedToReferenceFrame
	virtual CBaseEntity *GetPhysicsReferenceFrame() = 0;
	virtual int UpdateTransmitState() = 0;
	virtual int ShouldTransmit( const CCheckTransmitInfo *pInfo ) = 0;
	virtual void SetTransmit( CCheckTransmitInfo *pInfo, bool bAlways ) = 0;
	// Returns -1; func_mover and func_movelinear return an id when they build a movable nav mesh. Named by what it does
	virtual int GetMovableNavMeshId() = 0;
	// The transform of the scene node, never overridden. Named by what it does
	virtual matrix3x4_t EntityToWorldTransform() = 0;
	// World space bounds out of the collision property, the origin twice without one; never overridden. Named by what it does
	virtual void WorldSpaceAABB( Vector *pWorldMins, Vector *pWorldMaxs ) = 0;
	// Empty; overridden by entities that wait for a player: the ones that resolve !player, sound event entities, movers
	virtual void PostClientActive( CPlayerSlot slot ) = 0;
	// Follows the move parents up to the topmost one
	virtual CBaseEntity *GetRootMoveParent() = 0;
	// False; true for the lights and for a dynamic prop that owns a nav obstacle. When the scene node moves, the entities it is true for are queued and get ReportPositionChanged once the nodes are updated
	virtual bool ShouldReportPositionChanged() = 0;
	// Empty; the lights refresh their light component, a dynamic prop its nav obstacle. Called for the entities ShouldReportPositionChanged queued, after the scene nodes are updated
	virtual void ReportPositionChanged() = 0;
	// Empty; CGameSceneNode::PerformInvalidatePhysicsRecursive calls it on the owner of a node whose transform changed.
	virtual void OnBoneTransformsChanged() = 0;
	virtual void ChangeTeam( int iTeamNum ) = 0;
	virtual bool InSameTeam( CBaseEntity *pEntity ) = 0;
	// Called by ChangeTeam once m_iTeamNum is set; empty, overridden by the pawns
	virtual void OnTeamChanged( int nOldTeam, int nNewTeam ) = 0;
	// Returns null; CBaseModelEntity returns itself
	virtual CBaseModelEntity *GetGlowEntity() = 0;
	// Returns null; overridden by the hostage and the pawns
	virtual IResponseSystem *GetResponseSystem() = 0;
	// Empty; the hostage and the pawns speak the concept
	virtual void DispatchResponse( const char *pszConceptName ) = 0;
	// Returns null or false in every class
	virtual void *Stub_111() = 0;
	// Returns false in every class; CGameEntitySystem::OnAddEntity counts the non-networked entities it is true for. Called when the entity is added to the entity system
	virtual bool IsSaveNonNetworkable() = 0;
	// False; true for the lights, the sky, world text and world UI panels, the barn light returns m_bEnabled. No caller on the server
	virtual bool ShouldDraw() = 0;
	// Debug overlay drawing chosen by the bits. Named by what it does
	virtual void DrawDebugGeometryOverlays( uint64 nDebugBits ) = 0;
	// False; true for dynamic props, movers, func_movelinear, paths with dynamic nodes, point_template and ragdolls. Spawn keeps the hammerUniqueId key in m_sUniqueHammerID when it is true or in tools mode. Named by what it does. Asked by Spawn
	virtual bool RequiresUniqueHammerID() = 0;
	// Parses name:value:duration lists into the response contexts
	virtual void AddContext( const char *pszNameAndValue ) = 0;
	virtual int FindContextByName( const char *pszName ) = 0;
	// Drops the response contexts
	virtual void RemoveAllContexts() = 0;
	virtual void DeathNotice( CBaseEntity *pVictim ) = 0;
	virtual bool ShouldAttractAutoAim( CBaseEntity *pAimingEntity ) = 0;
	virtual float GetAutoAimRadius() = 0;
	virtual Vector GetAutoAimCenter() = 0;
	// Returns WorldSpaceCenter and zeroes *pIndex when given; the vector is not read. Never overridden, no caller on the server; the client has no auto-aim methods and no such slot. Named by what it does
	virtual Vector GetWorldSpaceCenterClearIndex( Vector vecUnused, int *pIndex ) = 0;
	virtual bool PassesDamageFilter( const CTakeDamageInfo &info ) = 0;
	virtual bool CanBeHitByMeleeAttack( CBaseEntity *pAttacker ) = 0;
	virtual int TakeHealth( float flHealth, int bitsDamageType ) = 0;
	// Health divided by the maximum, within 0..1
	virtual float GetHealthFraction() = 0;
	// Asked by TakeDamage, 1 rejects the damage. Returns 0; CBaseModelEntity asks a method of its own by the life state
	virtual int RejectDamage( const CTakeDamageInfo &info ) = 0;
	// Empty, never overridden; TakeDamage calls it between RejectDamage and OnTakeDamage
	virtual void ModifyDamage( CTakeDamageInfo &info ) = 0;
	// Called by TakeDamage with the damage that got through: VPhysicsTakeDamage unless a damage flag forbids it; CBaseModelEntity adds the wound decal and calls OnTakeDamage_Alive, _Dying or _Dead
	virtual void OnTakeDamage( const CTakeDamageResult &result ) = 0;
	// Tells the attacker through Event_KilledOther and returns true; the grenades and CBaseCombatCharacter return false
	virtual bool Event_Killed( const CTakeDamageResult &result ) = 0;
	// Stops taking damage, marks the entity dead and removes it: the end of a kill. Named by what it does. Called by TakeDamage
	virtual void Event_KilledRemove( const CTakeDamageResult &result ) = 0;
	virtual void Event_KilledOther( CBaseEntity *pVictim, const CTakeDamageInfo &info ) = 0;
	virtual bool IsTriggered( CBaseEntity *pActivator ) = 0;
	virtual void ImpactTrace( CGameTrace *pTrace, int iDamageType, const char *pszCustomImpactName ) = 0;
	// ( m_iEFlags & 2 ) == 0, never overridden, the same body on the client. The constructor of CCSWeaponBase sets that flag; ImpactTrace (server and client) sets IMPACT_NODECAL when this answers false, and the decal trace helper refuses entities with the flag. Named by what it does
	virtual bool AcceptsDecals() = 0;
	virtual bool OnControls( CBaseEntity *pControls ) = 0;
	virtual bool HasTarget( CUtlSymbolLarge targetName ) = 0;
	virtual bool IsNetClient() = 0;
	virtual void *GetServerVehicle() = 0;
	virtual bool IsViewable() = 0;
	virtual void OnEntityEvent( int nEvent, void *pEventData ) = 0;
	virtual bool CanStandOn( CBaseEntity *pSurface ) = 0;
	virtual CBaseEntity *GetEnemy() = 0;
	// Empty; CBaseAnimGraph and CCSWeaponBase override it. Called last by CGameSceneNode::UpdateEntityForHierarchyChange with the entity of the previous parent node. Named by what it does. Called for weapons
	virtual void OnParentChanged( CBaseEntity *pOldParent ) = 0;
	// Empty in every class.
	virtual void Stub_146() = 0;
	virtual void Use( InputData_t *pInput ) = 0;
	// Returns true in every class. the last check of CPlayer_UseServices::IsUseableEntity, which passes its pawn. Asked about doors and pawns the player looks at
	virtual bool Stub_148( CBaseEntity *pPawn ) = 0;
	// Returns the same constant in every class. The constant is 1.0; no call on an entity found
	virtual float Stub_149() = 0;
	virtual void StartTouch( CBaseEntity *pOther ) = 0;
	virtual void Touch( CBaseEntity *pOther ) = 0;
	virtual void EndTouch( CBaseEntity *pOther ) = 0;
	// True; false for the triggers. StartTouch, Touch and EndTouch pass the touch on to the move parent when the parent answers true. Named by what it does. Asked by Touch and EndTouch
	virtual bool AcceptsTouchesFromChildren() = 0;
	virtual void StartBlocked( CBaseEntity *pOther ) = 0;
	virtual void Blocked( CBaseEntity *pOther ) = 0;
	virtual void EndBlocked() = 0;
	// A pair that returns an empty symbol; the buttons return m_sOverrideAnticipationName and "button". Named by what it does
	virtual CGlobalSymbol GetOverrideAnticipationName() = 0;
	// Returns an empty symbol; CBaseButton returns "button" unless spawnflag 32768 "Skip Player Animation" is set. The name is the one of base.fgd of the game, override_anticipation_name of func_button: "Specify an override for the GetUsePreviewAction for this button (the anticipation type)"; the override is what GetOverrideAnticipationName returns. No caller on the server, no counterpart on the client
	virtual CGlobalSymbol GetUsePreviewAction() = 0;
	// Calls m_pfnMoveDone
	virtual void MoveDone() = 0;
	// Calls m_think
	virtual void Think() = 0;
	virtual void PhysicsSimulate() = 0;
	// Empty; called by UpdateOnRemove
	virtual void StopLoopingSounds() = 0;
	// Overridden by the grenade projectiles
	virtual void ResolveFlyCollisionCustom( CGameTrace &trace, Vector &vecVelocity ) = 0;
	virtual void NotifySystemEvent( CBaseEntity *pNotify, int nEventType, const void *pParams ) = 0;
	virtual void Teleport( const Vector *pNewPosition, const QAngle *pNewAngles, const Vector *pNewVelocity ) = 0;
	virtual int BloodColor() = 0;
	// Stores m_nBloodType, never overridden
	virtual void SetBloodType( int nBloodType ) = 0;
	// Empty; the ragdolls register themselves with g_pNotify as watchers of the entity (INotify::AddEntity). Named by what it does
	virtual void StartWatchingEntity( CBaseEntity *pEntity ) = 0;
	// Empty; the ragdolls unregister themselves with g_pNotify (INotify::RemoveEntity). Named by what it does
	virtual void StopWatchingEntity( CBaseEntity *pEntity ) = 0;
	virtual bool IsAlive() = 0;
	virtual bool IsPlayerPawn() = 0;
	virtual bool IsPlayerController() = 0;
	// The result of IsPlayerPawn, never overridden
	virtual bool IsControlledByPlayer() = 0;
	// Overridden by CWorld only
	virtual bool IsWorld() = 0;
	// False; true for scripted sequences and for the dynamic props that have a name. The command ent_actornames shows the names of the entities it is true for, by the help text of the command
	virtual bool ShouldDisplayInActorNames() = 0;
	virtual bool IsBaseCombatCharacter() = 0;
	virtual CBaseCombatCharacter *MyCombatCharacterPointer() = 0;
	virtual const CBaseCombatCharacter *MyCombatCharacterPointer() const = 0;
	virtual bool IsWeapon() = 0;
	virtual CBasePlayerWeapon *MyCombatWeaponPointer() = 0;
	virtual const CBasePlayerWeapon *MyCombatWeaponPointer() const = 0;
	virtual bool IsBaseTrain() = 0;
	// Returns null in every class; the anim graph trace filter skips entities that return non-null
	virtual INextBot *MyNextBotPointer() = 0;
	// Returns false in every class: the game has no master entity fields left.
	virtual bool IsLockedByMaster() = 0;
	virtual int GetMaxHealth() = 0;
	virtual void ModifyOrAppendCriteria( CResponseCriteriaSet &set ) = 0;
	virtual void ModifyOrAppendDerivedCriteria( CResponseCriteriaSet &set ) = 0;
	virtual void CollisionRulesChanged() = 0;
	virtual Vector EyePosition() = 0;
	virtual QAngle EyeAngles() = 0;
	virtual QAngle LocalEyeAngles() = 0;
	virtual Vector EarPosition() = 0;
	// EyePosition by default, never overridden; the eye point FVisible and FInViewCone look from. Named by what it does. Asked by FInViewCone
	virtual Vector EyePositionForVisibility() = 0;
	// The angles of the entity; the pawns return EyeAngles. BodyDirection2D and BodyDirection3D of CBaseCombatCharacter are built on it
	virtual QAngle BodyAngles() = 0;
	// Empty; called once SetGravityScale or the gravity inputs have recomputed m_flActualGravityScale
	virtual void OnGravityChanged() = 0;
	// Stores m_flTimeScale; CBaseAnimGraph passes it on
	virtual void SetTimeScale( float flTimeScale ) = 0;
	// False; true for CBaseToggle and CBasePropDoor. The rotating push of the pusher physics asks it: true adds m_vecAngVelocity times the frame time to the angles, false turns the angles through quaternions. Named by what it does. Asked for a moving door when m_vecAngVelocity is applied to its angles: true adds the angles, false rotates through a quaternion
	virtual bool RotatesByEulerAngles() = 0;
	virtual Vector BodyTarget( const Vector &vecPosSrc, bool bNoisy ) = 0;
	virtual Vector HeadTarget( const Vector &vecPosSrc ) = 0;
	virtual Vector GetViewOffset() = 0;
	// m_vecViewOffset as stored, a zero vector below CBaseModelEntity. Named by what it does
	virtual const void *GetNetworkedViewOffset() = 0;
	virtual Vector GetSmoothedVelocity() = 0;
	virtual bool IsMoving() = 0;
	virtual CBaseEntity *GetGroundEntity() = 0;
	virtual const CBaseEntity *GetGroundEntity() const = 0;
	// Calls OnEntityLeftGround of the old ground and OnEntityLandedOnGround of the new one
	virtual void OnGroundChanged( CBaseEntity *pOldGround, CBaseEntity *pNewGround ) = 0;
	// Empty, never overridden. The entity no longer stands on this one; called by OnGroundChanged and SetGroundEntity. Named by what it does
	virtual void OnEntityLeftGround( CBaseEntity *pEntity ) = 0;
	// Empty, never overridden. The entity now stands on this one; called by OnGroundChanged and SetGroundEntity. Named by what it does
	virtual void OnEntityLandedOnGround( CBaseEntity *pEntity ) = 0;
	// Overridden by func_conveyor
	virtual void GetGroundVelocityToApply( Vector &vecGroundVel ) = 0;
	// Returns true in every class
	virtual bool Stub_210() = 0;
	// The pawns forward it to their water services
	virtual void OnWaterLevelChanged( float flOldLevel, float flNewLevel ) = 0;
	virtual bool FVisible( CBaseEntity *pEntity, int nTraceMask, CBaseEntity **ppBlocker ) = 0;
	virtual bool FVisible( const Vector &vecTarget, int nTraceMask, CBaseEntity **ppBlocker ) = 0;
	// Empty; overridden by the grenade projectiles
	virtual void Splash() = 0;
	// WorldSpaceCenter, never overridden
	virtual Vector GetSoundEmissionOrigin() = 0;
	// Never overridden
	virtual void ScriptEmitSound( const char *pszSound ) = 0;
	// Stores m_fEffects; overridden by CBaseCombatCharacter
	virtual void InternalSetEffects( int nEffects ) = 0;
	// False for the move types NONE and VPHYSICS and for PUSH without a move in progress; the pawns return true, the controllers false
	virtual bool WillSimulateGamePhysics() = 0;
	// Empty in every class; CBaseTrigger::StartTouch calls it on the entity it adds to its touching list, passing the trigger. Named by what it does. Called by CBaseTrigger::StartTouch on the entity that touches the trigger
	virtual void OnStartTouchTrigger( CBaseEntity *pTrigger ) = 0;
	// Empty in every class; CBaseTrigger::EndTouch calls it on the entity it removes from its touching list, passing the trigger. Named by what it does. Called by CBaseTrigger::EndTouch on the entity that leaves the trigger
	virtual void OnEndTouchTrigger( CBaseEntity *pTrigger ) = 0;
	// Returns true in every class; asked on the root of a pusher about each entity in its way, false blocks the push. Asked on a moving door about every entity in its way: false marks the move as blocked
	virtual bool CanPushEntity( CBaseEntity *pOther ) const = 0;
	// Returns null; breakables and ragdolls return the attacker set within the last flTime seconds
	virtual CBaseEntity *HasPhysicsAttacker( float flTime ) = 0;
	// Empty; overridden by breakables and ragdolls
	virtual void SetPhysicsAttacker( CBaseEntity *pEntity, float flTime ) = 0;
	// Returns null; the weapons and the precipitation return the name of their VData class
	virtual const char *GetSubclassVDataClassName() = 0;
	// An EntitySubclassScope_t: -1, weapons 1, precipitation 0
	virtual int GetSubclassScope() = 0;
	// The subclass data was set (1) or reloaded (2)
	virtual void OnSubclassVDataChanged( uint32 nChangeType ) = 0;
	// Empty in every class, no caller on the server.
	virtual void OnCreatedServerRagdoll( CBaseModelEntity *pRagdoll ) = 0;
	// Empty; overridden by the grenade projectiles and the bomb
	virtual void PerformCustomPhysics( Vector *pNewPosition, Vector *pNewVelocity, QAngle *pNewAngles, QAngle *pNewAngVelocity ) = 0;
	// Keeps the Spawn of CEntityInstance visible next to the one below
	using CEntityInstance::Spawn;
	// Empty; overridden by the weapons, the beams, the sprites and the ladders
	virtual void Spawn() = 0;

	// The two below are 229 and 230 on Linux and 49 and 51 on Windows: MSVC lays out the
	// virtual methods of a name where the class first declares the name, and the
	// declarations after GetServerClass and SpawnShared above do that
	// Returns null or false in every class
	virtual bool Stub_49() = 0;
	// Returns null or false in every class
	virtual void *Stub_51() = 0;
	// Returns null or false in every class. Not an overload of Spawn (Windows 229 is Spawn); its Linux body is not folded with the bool of 229, so its type is not bool f()
	virtual bool Stub_230() = 0;

	// Returns 0 in every class. Returns 0
	virtual float Stub_231() = 0;
	// Returns 0 in every class. Returns 0
	virtual float Stub_232() = 0;
	// Returns 0 in every class. Returns 0
	virtual float Stub_233() = 0;
	// Empty in every class
	virtual void Stub_234() = 0;
	// Empty in every class
	virtual void Stub_235() = 0;
	// Returns true in every class; a choreo scene speaking for this actor uses a CPASAttenuationFilter when true and a CBroadcastRecipientFilter when false. Named by what it does
	virtual bool ShouldAttenuateSpeech() = 0;
	// Allocates the object the caller stores in m_pPulseGraphInstance; overridden by the Pulse blackboard entity
	virtual CPulseGraphInstance_ServerEntity *CreatePulseGraphInstance() = 0;
};

#endif // BASEENTITY_H
