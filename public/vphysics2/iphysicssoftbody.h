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
#include "mathlib/vector.h"
#include "mathlib/fltx4.h"
#include "mathlib/aabb.h"
#include "mathlib/transform.h"

class IVPhysics2World;
class CRnSoftbody;
class CFeModel;
class CUtlBuffer;
class CBufferString;

struct RnCollisionAttr_t;
struct SetNodePositionsParams_t;

// Nodes are the simulated points of the finite element model, ctrls (particles) are the transforms bound to model bones;
// the transform array holds the animated ctrl transforms followed by the simulated ones
// Methods with an unknown name that MSVC lays out as one overload group keep one shared unkNNN name
abstract_class IPhysicsSoftbody
{
public:
	virtual ~IPhysicsSoftbody() {}

	virtual IVPhysics2World *GetWorld() const = 0;

	virtual void unk003( float flValue ) = 0;
	virtual float unk004() const = 0;

	virtual void unk005( void *pContext ) = 0;

	virtual const CFeModel *GetFeModel() const = 0;
	virtual CFeModel *GetFeModel() = 0;

	// CRC32 of the node positions of both frames
	virtual uint32 GetStateHash() const = 0;
	virtual void TouchAnimatedTransforms() = 0;

	virtual void FilterTransforms( const void *pUnknown, void *pOutput ) = 0;
	virtual void FilterTransforms( const void *pParams ) = 0;

	virtual void *GetParticleTransforms( const VectorAligned *pInputNodePos = NULL, uint nFlags = 0 ) = 0;

	virtual const CTransform *GetAnimatedTransforms() const = 0;
	virtual CTransform *GetAnimatedTransforms() = 0;
	virtual const CTransform *GetSimulatedTransforms() const = 0;
	virtual CTransform *GetSimulatedTransforms() = 0;

	virtual bool unk017() const = 0;
	virtual void unk018( void *pUnknown ) = 0;
	virtual CTransform unk019( int nIndex ) const = 0;

	virtual void SetAbsOrigin( const VectorAligned &vNewOrigin, bool bTeleport ) = 0;

	virtual void SetAnimSpace( int nAnimSpace ) = 0;
	virtual int GetAnimSpace() const = 0;

	virtual void SetPose( const CTransform *pPose ) = 0;
	virtual void SetPose( const CTransform &tm, const CTransform *pPose ) = 0;
	virtual void unk025( const void *pUnknown, void *pOutput ) = 0;

	virtual const VectorAligned *GetNodePositions( int nFrame = 1 ) const = 0;
	virtual VectorAligned *GetNodePositions( int nFrame = 1 ) = 0;
	virtual Vector *unk028() = 0;

	virtual void SetNodePositions( const VectorAligned *pPos, int nCount, int nFrame = 1 ) = 0;
	virtual void SetNodePositions( const SetNodePositionsParams_t &params ) = 0;

	// Blends the node positions of the previous and the current frame
	virtual void ComputeInterpolatedNodePositions( float flFactor, VectorAligned *pPosOut ) = 0;

	// Offset every (one) dynamic node of the previous frame, changing its velocity, unless frozen or dormant
	virtual void unk032( fltx4 vDelta, float flScale ) = 0;
	virtual void unk033( uint nNode, const VectorAligned &vDelta, float flScale ) = 0;

	virtual uint GetNodeCount() const = 0;
	virtual Vector GetNodeVelocity( uint nNode ) const = 0;
	virtual void unk036( float flUnknown1, float flUnknown2 ) = 0;
	virtual uint GetCtrlCount() const = 0;
	virtual void ParseParticleState( CUtlBuffer &buf, float flTimeStep ) = 0;
	virtual CUtlString PrintParticleState() const = 0;
	virtual int GetParticleCount() const = 0;
	virtual void SetCtrl( int nParticle, const CTransform &tm ) = 0;
	virtual Vector GetCtrlVelocity( uint nCtrl ) const = 0;

	virtual void unk043( float flValue ) = 0;
	virtual float unk044() const = 0;

	virtual bool unk045( int nUnknown, void *pUnknown ) = 0;
	virtual void *FindEffect( int nEffectId ) = 0;
	virtual void RemoveEffect( int nEffectId ) = 0;

	virtual void unk048( bool bUnknown ) = 0;
	virtual bool unk049() const = 0;

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

	virtual void unk081( bool bUnknown ) = 0;
	virtual bool unk082() const = 0;
	virtual void unk083( bool bUnknown ) = 0;
	virtual bool unk084() const = 0;

	virtual AABB_t BuildBounds() const = 0;

	virtual const RnCollisionAttr_t &GetCollisionAttributes() const = 0;
	virtual void SetCollisionAttributes( const RnCollisionAttr_t &attr ) = 0;

	// Always 0
	virtual uint GetContactCount() const = 0;

	virtual void unk089( void *pUnknown ) = 0;

	// Sets or clears one bit of a mask; unk091 reports whether the mask is empty
	virtual void unk090( int nBit, bool bClear ) = 0;
	virtual bool unk091() const = 0;

	virtual void unk092( int nValue ) = 0;
	virtual int unk093() const = 0;

	virtual void SetGroundZ( float flGroundZ ) = 0;
	virtual float GetGroundZ() const = 0;

	virtual void unk096( fltx4 vUnknown ) = 0;

	virtual void unk097( float flValue ) = 0;
	virtual float unk098() const = 0;

	virtual int CastCone( const Vector &vStart, const Vector &vDir, float flConePitch ) = 0;

	virtual bool unk100() const = 0;
	virtual void unk101( bool bUnknown ) = 0;

	// Always 0
	virtual int unk102() const = 0;
	virtual int unk103() const = 0;

	virtual void unk104() = 0;
	virtual void *unk105() = 0;
	virtual void unk106( const void *pUnknown, int nUnknown ) = 0;
	virtual void unk107( int nUnknown, float flUnknown ) = 0;
	virtual bool unk108( void *pUnknown, const float *pUnknown2 ) = 0;

	virtual float GetTimeStep() const = 0;

	virtual void unk110( bool bUnknown ) = 0;
	virtual bool unk111() const = 0;
	virtual void SetDormancyDisabled( bool bDisabled ) = 0;
	virtual bool IsDormancyDisabled() const = 0;

	virtual void unk114( void *pUnknown, bool bUnknown ) = 0;

	// Two slots
	virtual void SetUserData( uint nIndex, void *pData ) = 0;
	virtual void *GetUserData( uint nIndex ) const = 0;

	virtual void unk117( uint8 nUnknown, float flUnknown1, float flUnknown2 ) = 0;
	virtual void unk118( uint8 nUnknown, float flUnknown1, float flUnknown2, float flUnknown3 ) = 0;
	virtual float unk119( uint8 nUnknown, int nUnknown2 ) const = 0;
	virtual void unk120( uint8 nUnknown, int *pUnknown, int nUnknown2 ) = 0;
	virtual void unk121() = 0;

	virtual void unk122( bool bUnknown ) = 0;
	virtual bool unk123() const = 0;

	virtual void LinkSoftbody( IPhysicsSoftbody *pOther ) = 0;
	virtual void UnlinkSoftbody( IPhysicsSoftbody *pOther ) = 0;
	virtual void UnlinkAllSoftbodies() = 0;
	virtual void unk127( IPhysicsSoftbody *pOther ) = 0;

	virtual bool unk128() const = 0;

	// Empty in release builds
	virtual void unk129() = 0;
	virtual int unk130() const = 0;

	virtual uint GetIndexInWorld() const = 0;
	virtual CRnSoftbody *GetSoftbody() const = 0;

	// Empty in release builds
	virtual void unk133() = 0;
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
	virtual void Describe( CBufferString *pDescription ) const = 0;

	// Empty in release builds
	virtual void unk005() = 0;

	// Returns zeros
	virtual FnPhysicsSoftbodyUnk006_t unk006() const = 0;

	virtual void unk007( uint nUnknown, bool bUnknown ) = 0;

	// Copies the per node vectors into an aligned array
	virtual void unk008( VectorAligned *pOut ) const = 0;

	virtual void unk009( int nValue ) = 0;
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
	virtual void unk020( uint8 nUnknown ) = 0;
	virtual bool IsDebugDrawEnabled() const = 0;
	virtual void unk022( void *pUnknown ) = 0;

	// Active state, false without a softbody
	virtual bool IsActive() const = 0;

	virtual void unk024( int nValue ) = 0;

	// Empty in release builds
	virtual void unk025() = 0;
	virtual int unk026() const = 0;

	// Appends a one line description of the softbody and its state
	virtual void AppendDebugInfo( CUtlString *pDescription ) const = 0;

	// Index of the softbody in its world, -1 without a softbody
	virtual int GetIndexInWorld() const = 0;

	virtual uint unk029() const = 0;
	virtual void unk030( bool bUnknown ) = 0;
	virtual uint unk031() const = 0;
	virtual int unk032() const = 0;
	virtual int GetEffectCount() const = 0;
	virtual void unk034( int nIndex, void *pUnknown ) = 0;
	virtual void unk035( float flValue ) = 0;
	virtual void unk036( bool bEnable ) = 0;

	// Empty in release builds
	virtual void unk037() = 0;
	virtual void unk038() = 0;

	virtual AABB_t unk039() const = 0;

	// Softbodies linked to this one
	virtual int GetLinkedSoftbodyCount() const = 0;
	virtual IPhysicsSoftbody *GetLinkedSoftbody( int nIndex ) const = 0;
	virtual void UnlinkAllSoftbodies() = 0;

	virtual int unk043() const = 0;
	virtual void unk044( void *pUnknown ) = 0;
	virtual void unk045() = 0;

	// Moves the nodes and the transforms by an offset
	virtual void unk046( fltx4 vDelta ) = 0;
	virtual bool unk047( const VectorAligned *pUnknown, fltx4 vUnknown ) = 0;

	// Empty in release builds
	virtual void unk048() = 0;
};

#endif // IPHYSICSSOFTBODY_H
