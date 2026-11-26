#ifndef IMPROV_LOCOMOTOR_H
#define IMPROV_LOCOMOTOR_H

#pragma once

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "../../navlib/nav_area.h"

class CNavArea;
class CNavLadder;

// Why a move to a goal failed; the values are not known
enum MoveToFailureType : int;

//-----------------------------------------------------------------------------
// The movement interface the improv path following drives. A second base of CHostage
// and CChicken.
//
// A pure interface without data of its own: in both classes the next base or member starts
// 8 bytes after it, the virtual table pointer alone. Virtual methods in the order of the
// game's table (20 slots on both platforms, no destructor).
//-----------------------------------------------------------------------------
abstract_class CImprovLocomotor
{
public:
	// The centre of the body
	virtual const Vector &GetCentroid() const = 0;
	// The position of the feet
	virtual const Vector &GetFeet() const = 0;
	// The position of the eyes
	virtual const Vector &GetEyes() const = 0;
	// The yaw of the movement
	virtual float GetMoveAngle() const = 0;
	// The nav area the mover was last known to be in
	virtual CNavArea *GetLastKnownArea() const = 0;
	// The height of the ground below a position, with its normal
	virtual bool GetSimpleGroundHeightWithFloor( const Vector &pos, float *height, Vector *normal ) = 0;

	virtual void Crouch() = 0;
	virtual void StandUp() = 0;
	virtual bool IsCrouching() const = 0;

	virtual void Jump() = 0;
	virtual bool IsJumping() const = 0;

	virtual void Run() = 0;
	virtual void Walk() = 0;
	virtual bool IsRunning() const = 0;

	// Starts to use a ladder
	virtual void StartLadder( const CNavLadder *ladder, NavTraverseType how, const Vector &approachPos, const Vector &departPos ) = 0;
	// Moves along a ladder; true when done
	virtual bool TraverseLadder( const CNavLadder *ladder, NavTraverseType how, const Vector &approachPos, const Vector &departPos, float deltaT ) = 0;
	virtual bool IsUsingLadder() const = 0;

	// Moves towards the goal of the path
	virtual void TrackPath( const Vector &pathGoal, float deltaT ) = 0;
	// The goal was reached
	virtual void OnMoveToSuccess( const Vector &goal ) = 0;
	// The goal could not be reached
	virtual void OnMoveToFailure( const Vector &goal, MoveToFailureType reason ) = 0;
};

#endif // IMPROV_LOCOMOTOR_H
