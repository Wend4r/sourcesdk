//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A sequence of a model.
//
//=============================================================================//

#ifndef ISEQUENCE_H
#define ISEQUENCE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "animationsystem/ianimationsystem.h"

class CAnimationBucket;
class CAnimationBlendList;
class CModel;
class KeyValues3;
class CAnimEventDefinition;
class CFootMotion;
class CSequenceAssetInfo;
class Vector;
class QAngle;

//-----------------------------------------------------------------------------
// A sequence of a model (CEmptySequence is the null object the animation system returns for an invalid
// handle).
//-----------------------------------------------------------------------------
abstract_class ISequence
{
public:
	virtual ~ISequence() {}
	virtual bool IsFrameCacheable() const = 0;
	virtual int MaxFrame( const float *pPoseParameters ) const = 0;
	virtual void DoAnimation( CAnimationBucket &bucket, float flCycle, float flWeight ) const = 0;
	virtual void BuildBlendlist( CAnimationBlendList *pBlendList, const float *pPoseParameters ) const = 0;
	virtual const char *GetName() const = 0;
	virtual const CModel *GetModel() const = 0;
	virtual bool IsLooping() const = 0;
	virtual bool IsMulti() const = 0;
	virtual bool IsDelta() const = 0;
	virtual bool IsHidden() const = 0;
	virtual bool IsAutoplay() const = 0;
	virtual bool IsSnapped() const = 0;
	virtual bool IsAnimation() const = 0;
	virtual bool IsRealTime() const = 0;
	virtual bool IsModelDoc() const = 0;
	virtual const KeyValues3 *GetSequenceKeyValues() const = 0;
	virtual int NumEvents() const = 0;
	virtual const CAnimEventDefinition *GetEvent( int nIndex ) const = 0;
	virtual void GetBounds( Vector *pMins, Vector *pMaxs ) const = 0;
	virtual void GetAllAnimations( CUtlVectorFixedGrowable< int, 16 > &animations ) const = 0;
	virtual void GetAllSequences( CUtlVector< HSequence > &sequences ) const = 0;
	virtual int NumActivities() const = 0;
	virtual const char *GetActivityName( int nIndex ) const = 0;
	virtual int GetActivityWeight( int nIndex ) const = 0;
	virtual int GetActivityType( int nIndex ) const = 0;
	virtual void SetActivityType( int nType ) const = 0;
	virtual void SetActivityType( int nIndex, int nType ) const = 0;
	virtual int NumPoseParameters() const = 0;
	virtual int GetPoseParameter( int nIndex ) const = 0;
	virtual int NumPoseSettings() const = 0;
	virtual const char *GetPoseSettingName( int nIndex ) const = 0;
	virtual float GetPoseSettingValue( int nIndex ) const = 0;
	virtual bool IsFlagSet( int nFlag ) const = 0;
	virtual void SetFlag( int nFlag ) const = 0;
	virtual void ClearFlag( int nFlag ) const = 0;
	virtual float GetFadeInTime() const = 0;
	virtual float GetFadeOutTime() const = 0;
	virtual HSequence GetNextSequence( HSequence hSequence, int *pDirection ) const = 0;
	virtual int GetNodeFlags() const = 0;
	virtual int EntryNode() const = 0;
	virtual int ExitNode() const = 0;
	virtual void SetEntryNode( int nNode ) = 0;
	virtual void SetExitNode( int nNode ) = 0;
	virtual void SetEventIndex( int nEvent, int nIndex ) const = 0;
	virtual void AddEventTypeFlags( int nEvent, int nFlags ) const = 0;
	virtual void RemoveEventTypeFlags( int nEvent, int nFlags ) const = 0;
	virtual float FPS( const float *pPoseParameters ) const = 0;
	virtual float CPS( const float *pPoseParameters ) const = 0;
	virtual float Duration( const float *pPoseParameters ) const = 0;
	virtual bool MovementAndDuration( float flCycleFrom, float flCycleTo, const float *pPoseParameters, Vector &vecDelta ) const = 0;
	virtual bool TotalMovementAndDuration( const float *pPoseParameters, Vector &vecDelta ) const = 0;
	virtual bool Movement( float flCycleFrom, float flCycleTo, const float *pPoseParameters, Vector &vecDelta, QAngle &angDelta ) const = 0;
	virtual float Movement( float flCycleFrom, float flCycleTo ) const = 0;
	virtual bool Velocity( float flCycle, const float *pPoseParameters, Vector &vecVelocity ) const = 0;
	virtual float RelativeVelocity( float flCycle ) const = 0;
	virtual float FindDistance( const float *pPoseParameters, float flDistance ) const = 0;
	virtual int LookupPoseSetting( const char *pszName ) const = 0;
	virtual int GetFootMotionCount() const = 0;
	virtual const CFootMotion *GetFootMotion( int nIndex ) const = 0;
	virtual void GetAssetInfo( CSequenceAssetInfo &info ) const = 0;
};

#endif // ISEQUENCE_H
