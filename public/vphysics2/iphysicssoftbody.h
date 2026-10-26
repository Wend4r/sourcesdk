//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Softbody (cloth) of a physics world and its function table interface
//
//===========================================================================//

#ifndef IPHYSICSSOFTBODY_H
#define IPHYSICSSOFTBODY_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"
#include "mathlib/vector.h"
#include "mathlib/fltx4.h"
#include "mathlib/aabb.h"
#include "mathlib/transform.h"
#include "rubikon/param_types.h"

class IVPhysics2World;
class CRnSoftbody;
class CFeModel;
class CFeModelReplaceContext;
class CUtlBuffer;
class CBufferString;
class IPhysicsShape;
class KeyValues3;

struct RnCollisionAttr_t;
struct SetNodePositionsParams_t;
struct RnToolQueryHit_t;

// The parameter block of IPhysicsSoftbody::Simulate
struct PhysicsSoftbodySimulateParams_t
{
	// The motion of the frame the softbody moves with over the step: rotation pivot, translation and rotation
	VectorAligned m_vPivot;
	VectorAligned m_vTranslation;
	QuaternionAligned m_qRotation;

	// Velocity of the frame; the nodes are corrected by the angular velocity about m_vPivot and the linear velocity
	VectorAligned m_vLinearVelocity;
	VectorAligned m_vAngularVelocity;

	// Optional previous and current reference transforms (uniform scale in the w of the positions); with m_flReferenceMotionScale
	// above 0 the root motion they describe, scaled by it, is taken out of the frame motion
	const CTransform *m_pReferenceTransforms;
	float m_flReferenceMotionScale;

	float m_flSubstepTime;
	int m_nSubstepCount;
};

// The report IPhysicsSoftbody::SetCollisionReport installs
struct PhysicsSoftbodyCollisionReport_t
{
	// Cleared at the start of every step; nothing in the library increments it
	int m_nCount;
	uint8 m_pad004[ 20 ];

	// The world shape the nodes touched last, cleared when installed
	IPhysicsShape *m_pLastTouchedShape;

	// The ctrl count of the model, stored when installed
	uint16 m_nCtrlCount;
};

// A sample of a SOFTBODY_EFFECT_WIND effect: the position of a particle at one of its trail points and its packed RGBA color (the library
// sets alpha 32 when it spawns the particle)
struct PhysicsSoftbodyEffectSample_t
{
	Vector m_vPosition;
	uint32 m_nColor;
};

// Filled by GetEffectSamples: 512 particles with 4 trail samples each
struct PhysicsSoftbodyEffectSamples_t
{
	PhysicsSoftbodyEffectSample_t *m_pSamples;
	int m_nParticleCount;
	int m_nSamplesPerParticle;
};

// Nodes are the simulated points of the finite element model, ctrls (particles) are the transforms bound to model bones;
// the transform array holds the animated ctrl transforms followed by the simulated ones
abstract_class IPhysicsSoftbody
{
public:
	virtual ~IPhysicsSoftbody() {}

	virtual IVPhysics2World *GetWorld() const = 0;

	// Scales the collision radii and the bone transforms of the model
	virtual void SetModelScale( float flModelScale ) = 0;
	virtual float GetModelScale() const = 0;

	virtual void ReplaceFeModel( CFeModelReplaceContext &context ) = 0;

	virtual const CFeModel *GetFeModel() const = 0;
	virtual CFeModel *GetFeModel() = 0;

	// CRC32 of the node positions of both frames
	virtual uint32 GetStateHash() const = 0;
	virtual void TouchAnimatedTransforms() = 0;

	// Feeds the animated ctrl transforms from the model bone transforms; pBoneMap is the model data mapping ctrls to bones
	virtual void SetAnimatedTransforms( const void *pBoneMap, const CTransform *pBoneTransforms ) = 0;
	virtual void SetAnimatedTransforms( const void *pParams ) = 0;

	// nFlags takes PhysicsSoftbodySimTransformsFlags_t bits
	virtual void *GetParticleTransforms( const VectorAligned *pInputNodePos = NULL, uint nFlags = 0 ) = 0;

	virtual const CTransform *GetAnimatedTransforms() const = 0;
	virtual CTransform *GetAnimatedTransforms() = 0;
	virtual const CTransform *GetSimulatedTransforms() const = 0;
	virtual CTransform *GetSimulatedTransforms() = 0;

	virtual bool BeforeFilterTransforms() = 0;
	// Writes the simulated transforms back to the model bones
	virtual void FilterTransforms( void *pParams ) = 0;
	// 1 is the current transform, 3 and 2 the transforms of the last two steps, 0 the transform of the last simulated state
	virtual CTransform GetRootTransform( int nIndex ) const = 0;

	// Teleporting carries the nodes along rigidly
	virtual void SetAbsTransform( const CTransform &transform, bool bTeleport ) = 0;

	virtual void SetAnimSpace( PhysicsSoftbodyAnimSpace_t nAnimSpace ) = 0;
	virtual PhysicsSoftbodyAnimSpace_t GetAnimSpace() const = 0;

	virtual void SetPose( const CTransform *pPose ) = 0;
	virtual void SetPose( const CTransform &tm, const CTransform *pPose ) = 0;
	virtual void InitializeTransforms( const void *pBoneMap, const CTransform *pBoneTransforms ) = 0;

	virtual const VectorAligned *GetNodePositions( int nFrame = 1 ) const = 0;
	virtual VectorAligned *GetNodePositions( int nFrame = 1 ) = 0;
	// Node positions of the animated pose
	virtual Vector *GetPoseNodePositions() = 0;

	virtual void SetNodePositions( const VectorAligned *pPos, int nCount, int nFrame = 1 ) = 0;
	virtual void SetNodePositions( const SetNodePositionsParams_t &params ) = 0;

	// Blends the node positions of the previous and the current frame
	virtual void ComputeInterpolatedNodePositions( float flFactor, VectorAligned *pPosOut ) = 0;

	// Offset every (one) dynamic node of the previous frame, changing its velocity, unless frozen or dormant
	virtual void AddVelocity( fltx4 vVelocity, float flScale ) = 0;
	virtual void AddNodeVelocity( uint nNode, const VectorAligned &vVelocity, float flScale ) = 0;

	virtual uint GetNodeCount() const = 0;
	virtual Vector GetNodeVelocity( uint nNode ) const = 0;
	// Scales the node velocities and clamps their per step displacement; a non-positive maximum stops the nodes
	virtual void LimitVelocities( float flMaxDisplacement, float flScale ) = 0;
	virtual uint GetCtrlCount() const = 0;
	virtual void ParseParticleState( CUtlBuffer &buf, float flTimeStep ) = 0;
	virtual CUtlString PrintParticleState() const = 0;
	virtual int GetParticleCount() const = 0;
	virtual void SetCtrl( int nParticle, const CTransform &tm ) = 0;
	virtual Vector GetCtrlVelocity( uint nCtrl ) const = 0;

	// Initialized from the model default that precedes the gravity scale and the air drag defaults, which is the thread stretch in
	// the order of PhysFeModelDesc_t; the dampen velocity effect divides its damping by it for some nodes
	virtual void SetThreadStretch( float flStretch ) = 0;
	virtual float GetThreadStretch() const = 0;

	// Starts the effect of the model with this name hash after stopping the ones it lists in StopEffects; false when the model has none.
	// The effect reads its parameters from pOverrides first and falls back to the definition in the model
	virtual bool StartEffect( uint32 nEffectNameHash, const KeyValues3 *pOverrides ) = 0;
	virtual void *FindEffect( int nEffectId ) = 0;
	virtual void RemoveEffect( int nEffectId ) = 0;

	// The step pre-process runs CFeModel::IntegrateSprings while enabled
	virtual void EnableSpringIntegration( bool bEnable ) = 0;
	virtual bool IsSpringIntegrationEnabled() const = 0;

	virtual float GetEnergy( PhysicsSoftbodyEnergyTypeEnum_t nEnergy ) const = 0;
	virtual float GetElasticEnergy() const = 0;
	virtual float GetPotentialEnergy() const = 0;
	virtual float GetKinematicEnergy() const = 0;

	virtual void SetDampingMultiplier( float flMul ) = 0;
	virtual float GetDampingMultiplier() const = 0;

	virtual void GoWakeup() = 0;
	virtual bool IsActive() const = 0;

	virtual bool IsFrozen() const = 0;
	virtual void SetFrozen( bool bFrozen ) = 0;

	virtual float GetVelAirDrag() const = 0;
	virtual void SetVelAirDrag( float flVelAirDrag ) = 0;
	virtual float GetExpAirDrag() const = 0;
	virtual void SetExpAirDrag( float flExpAirDrag ) = 0;
	virtual float GetVelQuadAirDrag() const = 0;
	virtual void SetVelQuadAirDrag( float flVelQuadAirDrag ) = 0;
	virtual float GetExpQuadAirDrag() const = 0;
	virtual void SetExpQuadAirDrag( float flExpQuadAirDrag ) = 0;
	virtual float GetQuadVelocitySmoothRate() const = 0;
	virtual void SetQuadVelocitySmoothRate( float flRate ) = 0;
	// Reads the rod iteration count in the shipped implementation
	virtual uint16 GetQuadVelocitySmoothIterations() const = 0;
	virtual void SetQuadVelocitySmoothIterations( uint16 nIterations ) = 0;
	virtual float GetRodVelocitySmoothRate() const = 0;
	virtual void SetRodVelocitySmoothRate( float flRate ) = 0;
	virtual uint16 GetRodVelocitySmoothIterations() const = 0;
	virtual void SetRodVelocitySmoothIterations( uint16 nIterations ) = 0;

	virtual void EnableGravity( bool bEnableGravity ) = 0;
	virtual bool IsGravityEnabled() const = 0;

	virtual bool IsDormant() const = 0;
	virtual void GoDormant() = 0;
	virtual bool AdvanceSleepCounter() = 0;

	// Steps skip the simulation; IsSimulationDisabled is also true while locked to the transform
	virtual void SetSimulationDisabled( bool bDisabled ) = 0;
	virtual bool IsSimulationDisabled() const = 0;
	// Stops simulating and carries the nodes rigidly with the transform
	virtual void SetLockedToTransform( bool bLocked ) = 0;
	virtual bool IsLockedToTransform() const = 0;

	virtual AABB_t BuildBounds() const = 0;

	virtual const RnCollisionAttr_t &GetCollisionAttributes() const = 0;
	virtual void SetCollisionAttributes( const RnCollisionAttr_t &attr ) = 0;

	// Always 0
	virtual uint GetContactCount() const = 0;

	virtual void SetCollisionReport( PhysicsSoftbodyCollisionReport_t *pReport ) = 0;

	// Sets or clears one bit of a mask that suppresses collision with the world while not empty
	virtual void DisableWorldCollision( int nBit, bool bClear ) = 0;
	virtual bool IsWorldCollisionEnabled() const = 0;

	// What SetAbsTransform does with the ground
	virtual void SetGroundMode( PhysicsSoftbodyGroundMode_t nMode ) = 0;
	virtual PhysicsSoftbodyGroundMode_t GetGroundMode() const = 0;

	virtual void SetGroundZ( float flGroundZ ) = 0;
	virtual float GetGroundZ() const = 0;

	// Traces the ground below the origin once it moved far enough from the last trace
	virtual void TraceGroundZ( fltx4 vOrigin ) = 0;

	// Height above the origin the ground trace starts at
	virtual void SetGroundTraceRaise( float flRaise ) = 0;
	virtual float GetGroundTraceRaise() const = 0;

	virtual int CastCone( const Vector &vStart, const Vector &vDir, float flConePitch ) = 0;

	// The last relaxation iteration of a step runs the follow-the-leader pass instead of the regular constraints; initialized from
	// a flag of the model
	virtual bool IsFtlPassEnabled() const = 0;
	virtual void EnableFtlPass( bool bEnable ) = 0;

	// Always 0
	virtual int unk102() const = 0;
	virtual int unk103() const = 0;

	// Moves the nodes to the animated pose and stops them
	virtual void ResetToPose() = 0;
	// The root transforms of the last two steps (GetRootTransform indices 2 and 3)
	virtual CTransform *GetLastStepTransforms() = 0;

	// Runs m_nSubstepCount substeps of m_flSubstepTime
	virtual void Simulate( const PhysicsSoftbodySimulateParams_t *pParams ) = 0;
	// Returns whether it simulated
	virtual uint Step( int nIterations, float flTimeStep ) = 0;
	// Steps up to the given time, then filters the transforms; returns false
	virtual bool SimulateAndFilterTransforms( void *pParams, const float *pTiming ) = 0;

	virtual float GetTimeStep() const = 0;

	virtual void EnableSleep( bool bEnable ) = 0;
	virtual bool IsSleepEnabled() const = 0;
	virtual void SetDormancyDisabled( bool bDisabled ) = 0;
	virtual bool IsDormancyDisabled() const = 0;

	// Writes the static node positions of the animated pose into pNodePositions, relaxes the constraints once and, with bCollide,
	// collides the nodes
	virtual void RelaxNodePositions( VectorAligned *pNodePositions, bool bCollide ) = 0;

	// Two slots
	virtual void SetUserData( uint nIndex, void *pData ) = 0;
	virtual void *GetUserData( uint nIndex ) const = 0;

	// Five blended parameters per vertex set, nParam is a PhysicsSoftbodyClothParam_t; nVertexSetHash is the name hash of a vertex set,
	// 0 for the whole softbody
	virtual void SetClothParam( uint8 nParam, float flValue, float flBlendTime, uint32 nVertexSetHash ) = 0;
	// Queued after the changes already pending
	virtual void QueueClothParam( uint8 nParam, float flDelay, float flValue, float flBlendTime, uint32 nVertexSetHash ) = 0;
	virtual float GetClothParam( uint8 nParam, uint32 nVertexSetHash ) const = 0;
	// Appends the start times of the queued changes
	virtual void GetQueuedClothParamTimes( uint8 nParam, CUtlVector< float > &times, uint32 nVertexSetHash ) = 0;
	// False while the simulation is disabled, SOFTBODY_CLOTH_PARAM_PAB of the whole softbody reached 1, or it may sleep and settled
	virtual bool IsSimulating() = 0;

	// The world skips stepping it
	virtual void SetManualStep( bool bManual ) = 0;
	virtual bool IsManualStep() const = 0;

	virtual void LinkSoftbody( IPhysicsSoftbody *pOther ) = 0;
	virtual void UnlinkSoftbody( IPhysicsSoftbody *pOther ) = 0;
	virtual void UnlinkAllSoftbodies() = 0;
	virtual void SetLinkParent( IPhysicsSoftbody *pParent ) = 0;

	// IsActive that is false without a simulation object
	virtual bool IsSoftbodyActive() const = 0;

	// Empty in release builds
	virtual void SetDebugName( const char *pName ) = 0;
	virtual const char *GetDebugName() const = 0;

	virtual uint GetIndexInWorld() const = 0;
	virtual CRnSoftbody *GetSoftbody() const = 0;

	// Empty in release builds
	virtual void RunUnitTest() = 0;
};

class CPhysicsSoftbody : public IPhysicsSoftbody
{
public:
	CRnSoftbody *m_pSoftbody;
	void *m_pUserData[ 2 ];
};

// The zeroed 40-byte struct IFnPhysicsSoftbody::unk006 returns; only its size and 8-byte alignment are known (fields at +16, +24 and +32)
struct FnPhysicsSoftbodyUnk006_t
{
	ALIGN8 uint8 m_Data[ 40 ] ALIGN8_POST;
};

// Flags of IFnPhysicsSoftbody::Benchmark
#define SOFTBODY_BENCHMARK_FLUSH_CACHE ( 1 << 0 ) // Fills a buffer the size of the L3 cache before each step
#define SOFTBODY_BENCHMARK_ANIM_POSE ( 1 << 1 ) // Times moving the nodes to the animated pose instead of a step

// Function table operating on the softbody it is bound to
abstract_class IFnPhysicsSoftbody
{
public:
	virtual void SetSoftbody( IPhysicsSoftbody *pSoftbody ) = 0;
	virtual IPhysicsSoftbody *GetSoftbody() const = 0;
	virtual void SetRnSoftbody( CRnSoftbody *pSoftbody ) = 0;
	virtual CRnSoftbody *GetRnSoftbody() const = 0;

	// Appends the non-default cloth settings
	virtual void Describe( CBufferString &description ) const = 0;

	// Empty in release builds
	virtual void unk005() = 0;

	// Returns zeros
	virtual FnPhysicsSoftbodyUnk006_t unk006() const = 0;

	// Runs nSteps steps on copies of the node positions and returns the microseconds they took; nFlags takes SOFTBODY_BENCHMARK_* flags
	virtual uint64 Benchmark( uint nSteps, uint8 nFlags ) = 0;

	// Computes the node positions of the animated pose into an aligned array
	virtual void GetPoseNodePositions( VectorAligned *pOut ) const = 0;

	virtual void SetSimulationMode( PhysicsSoftbodySimulationMode_t nMode ) = 0;
	// Casts a ray against the nodes, rods and quads for the tools; IVPhysics2World::ToolCastRayMultipleUnfiltered calls it
	virtual void ToolCastRay( CUtlVector< RnToolQueryHit_t > &hits, const Vector &vStart, const Vector &vEnd ) = 0;

	// The velocity one constraint step from rest gives each dynamic node, zero for the static nodes
	virtual void ComputeConstraintVelocities( VectorAligned *pVelocities, float flTimeStep ) = 0;

	// Empty in release builds
	virtual void unk012() = 0;

	virtual void SetDebugDrawTreeBeginLevel( int8 nLevel ) = 0;
	virtual int8 GetDebugDrawTreeBeginLevel() const = 0;
	virtual void SetDebugDrawTreeEndLevel( int8 nLevel ) = 0;
	virtual int8 GetDebugDrawTreeEndLevel() const = 0;
	// PhysicsSoftbodyDebugDrawFlags_t bits
	virtual void SetDebugDrawTreeFlags( uint8 nFlags ) = 0;
	virtual uint8 GetDebugDrawTreeFlags() const = 0;

	virtual void EnableDebugDraw( bool bEnable ) = 0;
	virtual void EnableDebugRendering( bool bEnable ) = 0;
	virtual bool IsDebugDrawEnabled() const = 0;
	// Builds a debug render mesh of the triangles in a color
	virtual void *CreateDebugRenderMesh( const fltx4 &vColor ) = 0;

	// Active state, false without a softbody
	virtual bool IsActive() const = 0;

	// Stored for the debug draw; release builds do not read it
	virtual void SetDebugSelection( int nSelection ) = 0;

	// Release builds ignore the debug name and GetDebugName returns null there
	virtual void SetDebugName( const char *pszName ) = 0;
	virtual const char *GetDebugName() const = 0;

	// Appends a one line description of the softbody and its state
	virtual void AppendDebugInfo( CUtlString &description ) const = 0;

	// Index of the softbody in its world, -1 without a softbody
	virtual int GetIndexInWorld() const = 0;

	// Frames in the recording buffer (6 values per node and one more per frame); release builds only clear it
	virtual uint GetRecordedFrameCount() const = 0;

	// Release builds store the flag without reading it
	virtual void SetRecording( bool bRecord ) = 0;

	// A unique number taken from a global counter when the softbody is created, tagged with 0x40000000
	virtual uint GetSoftbodyId() const = 0;

	// Consecutive steps in which the nodes barely moved; 30 and more count as resting
	virtual int GetRestingStepCount() const = 0;
	virtual int GetEffectCount() const = 0;
	// For a SOFTBODY_EFFECT_WIND effect, allocates its 2048 sample array on first use and fills pSamples
	virtual void GetEffectSamples( int nIndex, PhysicsSoftbodyEffectSamples_t *pSamples ) = 0;
	// Time the timed simulation steps up to
	virtual void SetSimulationTime( float flTime ) = 0;
	// Collides the nodes once before the relaxation instead of in its iterations; initialized from a flag of the model
	virtual void SetSingleCollisionPass( bool bEnable ) = 0;

	// Empty in release builds
	virtual void unk037() = 0;
	virtual void unk038() = 0;

	virtual AABB_t ComputeBounds() const = 0;

	// Softbodies linked to this one
	virtual int GetLinkedSoftbodyCount() const = 0;
	virtual IPhysicsSoftbody *GetLinkedSoftbody( int nIndex ) const = 0;
	virtual void UnlinkAllSoftbodies() = 0;

	virtual int GetStepCount() const = 0;
	// The node positions of the initial pose of the model placed at the current transform
	virtual void GetInitPoseNodePositions( VectorAligned *pOut ) const = 0;
	// Rebuilds the simulated ctrl transforms from the current node positions
	virtual void UpdateSimulatedTransforms() = 0;

	// Moves the nodes and the transforms by an offset
	virtual void Translate( fltx4 vDelta ) = 0;
	// The closest point to vPoint on the tapered capsule colliders of the model; false without colliders
	virtual bool GetClosestPointOnColliders( VectorAligned *pClosest, fltx4 vPoint ) = 0;

	// Empty in release builds
	virtual void unk048() = 0;
};

#endif // IPHYSICSSOFTBODY_H
