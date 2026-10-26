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

class IVPhysics2World;
class CRnSoftbody;
class CFeModel;
class CFeModelReplaceContext;
class CUtlBuffer;
class CBufferString;

struct RnCollisionAttr_t;
struct SetNodePositionsParams_t;

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

	virtual void SetAnimSpace( int nAnimSpace ) = 0;
	virtual int GetAnimSpace() const = 0;

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

	virtual void unk043( float flValue ) = 0;
	virtual float unk044() const = 0;

	// Starts the effect of the model with this name hash after stopping the ones it lists in StopEffects; false when the model has none
	virtual bool StartEffect( uint32 nEffectNameHash, void *pUnknown ) = 0;
	virtual void *FindEffect( int nEffectId ) = 0;
	virtual void RemoveEffect( int nEffectId ) = 0;

	// The step pre-process runs CFeModel::IntegrateSprings while enabled
	virtual void EnableSpringIntegration( bool bEnable ) = 0;
	virtual bool IsSpringIntegrationEnabled() const = 0;

	virtual float GetEnergy( int nEnergy ) const = 0;
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

	virtual void unk089( void *pUnknown ) = 0;

	// Sets or clears one bit of a mask that suppresses collision with the world while not empty
	virtual void DisableWorldCollision( int nBit, bool bClear ) = 0;
	virtual bool IsWorldCollisionEnabled() const = 0;

	virtual void unk092( int nValue ) = 0;
	virtual int unk093() const = 0;

	virtual void SetGroundZ( float flGroundZ ) = 0;
	virtual float GetGroundZ() const = 0;

	// Traces the ground below the origin once it moved far enough from the last trace
	virtual void TraceGroundZ( fltx4 vOrigin ) = 0;

	// Height above the origin the ground trace starts at
	virtual void SetGroundTraceRaise( float flRaise ) = 0;
	virtual float GetGroundTraceRaise() const = 0;

	virtual int CastCone( const Vector &vStart, const Vector &vDir, float flConePitch ) = 0;

	virtual bool unk100() const = 0;
	virtual void unk101( bool bUnknown ) = 0;

	// Always 0
	virtual int unk102() const = 0;
	virtual int unk103() const = 0;

	// Moves the nodes to the animated pose and stops them
	virtual void ResetToPose() = 0;
	virtual void *unk105() = 0;
	virtual void unk106( const void *pUnknown, int nUnknown ) = 0;
	// Returns whether it simulated
	virtual uint Step( int nIterations, float flTimeStep ) = 0;
	// Steps up to the given time, then filters the transforms; returns false
	virtual bool SimulateAndFilterTransforms( void *pParams, const float *pTiming ) = 0;

	virtual float GetTimeStep() const = 0;

	virtual void EnableSleep( bool bEnable ) = 0;
	virtual bool IsSleepEnabled() const = 0;
	virtual void SetDormancyDisabled( bool bDisabled ) = 0;
	virtual bool IsDormancyDisabled() const = 0;

	virtual void unk114( void *pUnknown, bool bUnknown ) = 0;

	// Two slots
	virtual void SetUserData( uint nIndex, void *pData ) = 0;
	virtual void *GetUserData( uint nIndex ) const = 0;

	// Five blended parameters per vertex set, described as "pas", "pab", "vas", "vab" and an unnamed one;
	// nVertexSetHash is the name hash of a vertex set, 0 for the whole softbody
	virtual void SetClothParam( uint8 nParam, float flValue, float flBlendTime, uint32 nVertexSetHash ) = 0;
	// Queued after the changes already pending
	virtual void QueueClothParam( uint8 nParam, float flDelay, float flValue, float flBlendTime, uint32 nVertexSetHash ) = 0;
	virtual float GetClothParam( uint8 nParam, uint32 nVertexSetHash ) const = 0;
	// Appends the start times of the queued changes
	virtual void GetQueuedClothParamTimes( uint8 nParam, CUtlVector< float > &times, uint32 nVertexSetHash ) = 0;
	// False while the simulation is disabled, the second parameter of the whole softbody reached 1, or it may sleep and settled
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

	virtual void unk007( uint nUnknown, bool bUnknown ) = 0;

	// Computes the node positions of the animated pose into an aligned array
	virtual void GetPoseNodePositions( VectorAligned *pOut ) const = 0;

	// 0 simulates, 1 holds the nodes at the animated pose, other values leave them as they are
	virtual void SetSimulationMode( int nMode ) = 0;
	virtual void unk010( int *pUnknown, const void *pUnknown2, const void *pUnknown3 ) = 0;
	virtual void unk011( void *pUnknown, float flUnknown ) = 0;

	// Empty in release builds
	virtual void unk012() = 0;

	virtual void SetDebugDrawTreeBeginLevel( int8 nLevel ) = 0;
	virtual int8 GetDebugDrawTreeBeginLevel() const = 0;
	virtual void SetDebugDrawTreeEndLevel( int8 nLevel ) = 0;
	virtual int8 GetDebugDrawTreeEndLevel() const = 0;
	virtual void SetDebugDrawTreeFlags( uint8 nFlags ) = 0;
	virtual uint8 GetDebugDrawTreeFlags() const = 0;

	virtual void EnableDebugDraw( bool bEnable ) = 0;
	virtual void EnableDebugRendering( bool bEnable ) = 0;
	virtual bool IsDebugDrawEnabled() const = 0;
	// Builds a debug render mesh of the triangles in a color
	virtual void *CreateDebugRenderMesh( const fltx4 &vColor ) = 0;

	// Active state, false without a softbody
	virtual bool IsActive() const = 0;

	virtual void unk024( int nValue ) = 0;

	// Empty in release builds; GetDebugName returns null there
	virtual void unk025() = 0;
	virtual const char *GetDebugName() const = 0;

	// Appends a one line description of the softbody and its state
	virtual void AppendDebugInfo( CUtlString &description ) const = 0;

	// Index of the softbody in its world, -1 without a softbody
	virtual int GetIndexInWorld() const = 0;

	virtual uint unk029() const = 0;
	virtual void unk030( bool bUnknown ) = 0;

	// A unique number taken from a global counter when the softbody is created, tagged with 0x40000000
	virtual uint GetSoftbodyId() const = 0;

	// Consecutive steps in which the nodes barely moved; 30 and more count as resting
	virtual int GetRestingStepCount() const = 0;
	virtual int GetEffectCount() const = 0;
	virtual void unk034( int nIndex, void *pUnknown ) = 0;
	// Time the timed simulation steps up to
	virtual void SetSimulationTime( float flTime ) = 0;
	virtual void unk036( bool bEnable ) = 0;

	// Empty in release builds
	virtual void unk037() = 0;
	virtual void unk038() = 0;

	virtual AABB_t ComputeBounds() const = 0;

	// Softbodies linked to this one
	virtual int GetLinkedSoftbodyCount() const = 0;
	virtual IPhysicsSoftbody *GetLinkedSoftbody( int nIndex ) const = 0;
	virtual void UnlinkAllSoftbodies() = 0;

	virtual int GetStepCount() const = 0;
	virtual void unk044( void *pUnknown ) = 0;
	// Rebuilds the simulated ctrl transforms from the current node positions
	virtual void UpdateSimulatedTransforms() = 0;

	// Moves the nodes and the transforms by an offset
	virtual void Translate( fltx4 vDelta ) = 0;
	virtual bool unk047( const VectorAligned *pUnknown, fltx4 vUnknown ) = 0;

	// Empty in release builds
	virtual void unk048() = 0;
};

#endif // IPHYSICSSOFTBODY_H
