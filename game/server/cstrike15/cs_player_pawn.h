#ifndef CSTRIKE15_CS_PLAYER_PAWN_H
#define CSTRIKE15_CS_PLAYER_PAWN_H

#pragma once

#include "../../shared/cstrike15/cs_player_pawn_base.h"
#include "../../shared/irenderboundsworldspaceoverride.h"
#include "../../shared/ibonetransformmodifier.h"

//-----------------------------------------------------------------------------
// CCSPlayerPawn adds no method of its own. Its second bases are IRenderBoundsWorldSpaceOverride
// and IBoneTransformModifier (RTTI; CAI_ExpresserSink comes with CCSPlayerPawnBase and is not
// overridden). The two methods below override one method of each; the Itanium ABI also enters
// such an override into the primary table, slots 414 and 415, MSVC does not. The second bases
// sit at offsets 8 and 16 here, not where the game has them (Linux 4328 and 4336): do not
// convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CCSPlayerPawnBaseVTable<> >
abstract_class CCSPlayerPawnVTable : public TBase, public IRenderBoundsWorldSpaceOverride, public IBoneTransformModifier
{
public:
	// The bounds -80,-80,-64 .. 80,80,128 around the origin while the pawn is alive, false otherwise; Spawn hands the interface to the scene node, which asks it first for its world space render bounds. Named by what it does. Overrides IRenderBoundsWorldSpaceOverride: a slot of this table on Linux only
	virtual bool GetRenderBoundsWorldSpace( Vector &vecMins, Vector &vecMaxs ) override = 0;
	// Scales the head bone by m_flBigHeadScale when it is not 1 and the game rules allow it; Activate hands the interface to the skeleton, which calls it for every bone. Named by what it does. Overrides IBoneTransformModifier: a slot of this table on Linux only
	virtual bool ModifyBoneTransform( int nParentBone, int nBone, CTransform *pTransform ) override = 0;
};

#endif // CSTRIKE15_CS_PLAYER_PAWN_H
