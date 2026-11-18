//========= Copyright © Valve Corporation, All rights reserved. ============//
#ifndef FE_MODEL_DESC
#define FE_MODEL_DESC

#include "tier1/utlvector.h"
#include "tier0/utlstring.h"
#include "mathlib/vector2d.h"
#include "mathlib/vector4d.h"
#include "mathlib/transform.h"
#include "mathlib/ssequaternion.h"
#include "tier1/keyvalues3.h"

class CFeModel;
class CResourceStream;
template < class T > class CLockedResource;

// A quaternion stored as four floats, without the alignment of a SIMD quaternion
struct QuaternionStorage
{
	float x, y, z, w;
};

//
// The finite element (cloth) model data of a physics resource, as the schema of CS2 describes it
// (vphysics2 and resourcecompiler, the same on Windows and Linux). The member types are nested so
// that they do not collide with the older copies of some of them in femodel.h. Every name is the
// game's own, nDummy, nReserved and m_nReservedUint8 included.
//
struct PhysFeModelDesc_t
{
	struct alignas( 16 ) FeNodeBase_t
	{
		uint16 nNode;
		uint16 nDummy[ 3 ];
		uint16 nNodeX0;
		uint16 nNodeX1;
		uint16 nNodeY0;
		uint16 nNodeY1;
		QuaternionStorage qAdjust;
	};

	struct alignas( 16 ) FeSimdNodeBase_t
	{
		uint16 nNode[ 4 ];
		uint16 nNodeX0[ 4 ];
		uint16 nNodeX1[ 4 ];
		uint16 nNodeY0[ 4 ];
		uint16 nNodeY1[ 4 ];
		uint16 nDummy[ 4 ];
		FourQuaternions qAdjust;
	};

	struct FeQuad_t
	{
		uint16 nNode[ 4 ];
		float32 flSlack;
		Vector4D vShape[ 4 ];
	};

	struct alignas( 16 ) FeSimdQuad_t
	{
		uint16 nNode[ 4 ][ 4 ];
		fltx4 f4Slack;
		FourVectors vShape[ 4 ];
		fltx4 f4Weights[ 4 ];
	};

	struct alignas( 16 ) FourVectors2D
	{
		fltx4 x;
		fltx4 y;
	};

	struct alignas( 16 ) FeSimdTri_t
	{
		uint32 nNode[ 4 ][ 3 ];
		fltx4 w1;
		fltx4 w2;
		fltx4 v1x;
		FourVectors2D v2;
	};

	struct FePrism_t
	{
		uint16 nNode[ 6 ];
		float32 flVolume;
	};

	struct alignas( 16 ) FeSimdPrism_t
	{
		uint16 nNode[ 4 ][ 6 ];
		fltx4 flVolume;
	};

	struct alignas( 16 ) FeSimdRodConstraint_t
	{
		uint16 nNode[ 4 ][ 2 ];
		fltx4 f4MaxDist;
		fltx4 f4MinDist;
		fltx4 f4Weight0;
		fltx4 f4RelaxationFactor;
	};

	struct alignas( 16 ) FeSimdRodConstraintAnim_t
	{
		uint16 nNode[ 4 ][ 2 ];
		fltx4 f4Weight0;
		fltx4 f4RelaxationFactor;
	};

	struct FeRodConstraint_t
	{
		uint16 nNode[ 2 ];
		float32 flMaxDist;
		float32 flMinDist;
		float32 flWeight0;
		float32 flRelaxationFactor;
	};

	struct FeTwistConstraint_t
	{
		uint16 nNodeOrient;
		uint16 nNodeEnd;
		float32 flTwistRelax;
		float32 flSwingRelax;
	};

	struct FeHingeLimit_t
	{
		uint16 nNode[ 6 ];
		uint32 nFlags;
		float32 flWeight4;
		float32 flWeight5;
		float32 flAngleCenter;
		float32 flAngleExtents;
	};

	struct FeDynKinLink_t
	{
		uint16 m_nParent;
		uint16 m_nChild;
	};

	struct FeBoneMergeLink_t
	{
		uint32 m_nParentHash;
		uint16 m_nChildNode;
	};

	struct FeAntiTunnelProbe_t
	{
		float32 flWeight;
		uint32 nFlags;
		uint16 nProbeNode;
		uint16 nCount;
		uint32 nBegin;
		float32 flActivationDistance;
		float32 flCurvatureRadius;
		float32 flBias;
	};

	struct alignas( 16 ) FeNodeStrayBox_t
	{
		Vector vMin;
		uint32 nFlags;
		Vector vMax;
		uint16 nNode[ 2 ];
	};

	struct FeAxialEdgeBend_t
	{
		float32 te;
		float32 tv;
		float32 flDist;
		float32 flWeight[ 4 ];
		uint16 nNode[ 6 ];
	};

	struct FeCtrlOffset_t
	{
		Vector vOffset;
		uint16 nCtrlParent;
		uint16 nCtrlChild;
	};

	struct FeCtrlOsOffset_t
	{
		uint16 nCtrlParent;
		uint16 nCtrlChild;
	};

	struct FeFollowNode_t
	{
		uint16 nParentNode;
		uint16 nChildNode;
		float32 flWeight;
	};

	struct RnPlane_t
	{
		Vector m_vNormal;
		float32 m_flOffset;
	};

	struct FeCollisionPlane_t
	{
		uint16 nCtrlParent;
		uint16 nChildNode;
		RnPlane_t m_Plane;
		float32 flStrength;
	};

	struct FeNodeIntegrator_t
	{
		float32 flPointDamping;
		float32 flAnimationForceAttraction;
		float32 flAnimationVertexAttraction;
		float32 flGravity;
	};

	struct FeSpringIntegrator_t
	{
		uint16 nNode[ 2 ];
		float32 flSpringRestLength;
		float32 flSpringConstant;
		float32 flSpringDamping;
		float32 flNodeWeight0;
	};

	struct alignas( 16 ) FeSimdSpringIntegrator_t
	{
		uint16 nNode[ 4 ][ 2 ];
		fltx4 flSpringRestLength;
		fltx4 flSpringConstant;
		fltx4 flSpringDamping;
		fltx4 flNodeWeight0;
	};

	struct FeWorldCollisionParams_t
	{
		float32 flWorldFriction;
		float32 flGroundFriction;
		uint16 nListBegin;
		uint16 nListEnd;
	};

	struct FeTaperedCapsuleStretch_t
	{
		uint16 nNode[ 2 ];
		uint16 nCollisionMask;
		uint16 nDummy;
		float32 flRadius[ 2 ];
	};

	struct alignas( 16 ) FeTaperedCapsuleRigid_t
	{
		fltx4 vSphere[ 2 ];
		uint16 nNode;
		uint16 nCollisionMask;
		uint16 nVertexMapIndex;
		uint16 nFlags;
	};

	struct alignas( 16 ) FeSphereRigid_t
	{
		fltx4 vSphere;
		uint16 nNode;
		uint16 nCollisionMask;
		uint16 nVertexMapIndex;
		uint16 nFlags;
	};

	struct FeTreeChildren_t
	{
		uint16 nChild[ 2 ];
	};

	struct alignas( 16 ) FeFitMatrix_t
	{
		CTransform bone;
		Vector vCenter;
		uint16 nEnd;
		uint16 nNode;
		uint16 nBeginDynamic;
	};

	struct FeFitWeight_t
	{
		float32 flWeight;
		uint16 nNode;
		uint16 nDummy;
	};

	struct FeNodeReverseOffset_t
	{
		Vector vOffset;
		uint16 nBoneCtrl;
		uint16 nTargetNode;
	};

	struct FeAnimStrayRadius_t
	{
		uint16 nNode[ 2 ];
		float32 flMaxDist;
		float32 flRelaxationFactor;
	};

	struct alignas( 16 ) FeSimdAnimStrayRadius_t
	{
		uint16 nNode[ 4 ][ 2 ];
		fltx4 flMaxDist;
		fltx4 flRelaxationFactor;
	};

	struct FeKelagerBend2_t
	{
		float32 flWeight[ 3 ];
		float32 flHeight0;
		uint16 nNode[ 3 ];
		uint16 nReserved;
	};

	struct FeCtrlSoftOffset_t
	{
		uint16 nCtrlParent;
		uint16 nCtrlChild;
		Vector vOffset;
		float32 flAlpha;
	};

	struct CFeJiggleBone
	{
		uint32 m_nFlags;
		float32 m_flLength;
		float32 m_flTipMass;
		float32 m_flYawStiffness;
		float32 m_flYawDamping;
		float32 m_flPitchStiffness;
		float32 m_flPitchDamping;
		float32 m_flAlongStiffness;
		float32 m_flAlongDamping;
		float32 m_flAngleLimit;
		float32 m_flMinYaw;
		float32 m_flMaxYaw;
		float32 m_flYawFriction;
		float32 m_flYawBounce;
		float32 m_flMinPitch;
		float32 m_flMaxPitch;
		float32 m_flPitchFriction;
		float32 m_flPitchBounce;
		float32 m_flBaseMass;
		float32 m_flBaseStiffness;
		float32 m_flBaseDamping;
		float32 m_flBaseMinLeft;
		float32 m_flBaseMaxLeft;
		float32 m_flBaseLeftFriction;
		float32 m_flBaseMinUp;
		float32 m_flBaseMaxUp;
		float32 m_flBaseUpFriction;
		float32 m_flBaseMinForward;
		float32 m_flBaseMaxForward;
		float32 m_flBaseForwardFriction;
		float32 m_flRadius0;
		float32 m_flRadius1;
		Vector m_vPoint0;
		Vector m_vPoint1;
		uint16 m_nCollisionMask;
	};

	struct CFeIndexedJiggleBone
	{
		uint32 m_nNode;
		uint32 m_nJiggleParent;
		CFeJiggleBone m_jiggleBone;
	};

	struct FeTri_t
	{
		uint16 nNode[ 3 ];
		float32 w1;
		float32 w2;
		float32 v1x;
		Vector2D v2;
	};

	struct FeSDFRigid_t
	{
		Vector vLocalMin;
		Vector vLocalMax;
		float32 flBounciness;
		uint16 nNode;
		uint16 nCollisionMask;
		uint16 nVertexMapIndex;
		uint16 nFlags;
		CUtlVector< float32 > m_Distances;
		int32 m_nWidth;
		int32 m_nHeight;
		int32 m_nDepth;
	};

	struct alignas( 16 ) FeBoxRigid_t
	{
		CTransform tmFrame2;
		uint16 nNode;
		uint16 nCollisionMask;
		Vector vSize;
		uint16 nVertexMapIndex;
		uint16 nFlags;
	};

	struct FeRigidColliderIndices_t
	{
		uint16 m_nTaperedCapsuleRigidIndex;
		uint16 m_nSphereRigidIndex;
		uint16 m_nBoxRigidIndex;
		uint16 m_nSDFRigidIndex;
		uint16 m_nCollisionPlaneIndex;
	};

	struct FeMorphLayerDepr_t
	{
		CUtlString m_Name;
		uint32 m_nNameHash;
		CUtlVector< uint16 > m_Nodes;
		CUtlVector< Vector > m_InitPos;
		CUtlVector< float32 > m_Gravity;
		CUtlVector< float32 > m_GoalStrength;
		CUtlVector< float32 > m_GoalDamping;
		uint32 m_nFlags;
	};

	struct FeVertexMapDesc_t
	{
		CUtlString sName;
		uint32 nNameHash;
		uint32 nColor;
		uint32 nFlags;
		uint16 nVertexBase;
		uint16 nVertexCount;
		uint32 nMapOffset;
		uint32 nNodeListOffset;
		Vector vCenterOfMass;
		float32 flVolumetricSolveStrength;
		int16 nScaleSourceNode;
		uint16 nNodeListCount;
	};

	struct FeEffectDesc_t
	{
		CUtlString sName;
		uint32 nNameHash;
		int32 nType;
		KeyValues3 m_Params;
	};

	struct FeNodeWindBase_t
	{
		uint16 nNodeX0;
		uint16 nNodeX1;
		uint16 nNodeY0;
		uint16 nNodeY1;
	};

	struct FeModelSelfCollisionLayer_t
	{
		CUtlString m_Name;
		CUtlVector< uint16 > m_Nodes;
		float32 m_flParentReaction;
		uint32 m_nFlags;
		uint32 m_nEndIdx[ 4 ];
	};

	CUtlVector< uint32 > m_CtrlHash;
	CUtlVector< CUtlString > m_CtrlName;
	uint32 m_nStaticNodeFlags;
	uint32 m_nDynamicNodeFlags;
	float32 m_flLocalForce;
	float32 m_flLocalRotation;
	uint16 m_nNodeCount;
	uint16 m_nStaticNodes;
	uint16 m_nRotLockStaticNodes;
	uint16 m_nFirstPositionDrivenNode;
	uint16 m_nSimdTriCount1;
	uint16 m_nSimdTriCount2;
	uint16 m_nSimdQuadCount1;
	uint16 m_nSimdQuadCount2;
	uint16 m_nQuadCount1;
	uint16 m_nQuadCount2;
	uint16 m_nTreeDepth;
	uint16 m_nNodeBaseJiggleboneDependsCount;
	uint16 m_nRopeCount;
	CUtlVector< uint16 > m_Ropes;
	CUtlVector< FeNodeBase_t > m_NodeBases;
	CUtlVector< FeSimdNodeBase_t > m_SimdNodeBases;
	CUtlVector< FeQuad_t > m_Quads;
	CUtlVector< FeSimdQuad_t > m_SimdQuads;
	CUtlVector< FeSimdTri_t > m_SimdTris;
	CUtlVector< FePrism_t > m_Prisms;
	CUtlVector< FeSimdPrism_t > m_SimdPrisms;
	CUtlVector< FeSimdRodConstraint_t > m_SimdRods;
	CUtlVector< FeSimdRodConstraintAnim_t > m_SimdRodsAnim;
	CUtlVector< CTransform > m_InitPose;
	CUtlVector< FeRodConstraint_t > m_Rods;
	CUtlVector< FeTwistConstraint_t > m_Twists;
	CUtlVector< FeHingeLimit_t > m_HingeLimits;
	CUtlVector< uint32 > m_AntiTunnelBytecode;
	CUtlVector< FeDynKinLink_t > m_DynKinLinks;
	CUtlVector< FeBoneMergeLink_t > m_BoneMergeLinks;
	CUtlVector< FeAntiTunnelProbe_t > m_AntiTunnelProbes;
	CUtlVector< uint16 > m_AntiTunnelTargetNodes;
	CUtlVector< FeNodeStrayBox_t > m_NodeStrayBoxes;
	CUtlVector< FeAxialEdgeBend_t > m_AxialEdges;
	CUtlVector< float32 > m_NodeInvMasses;
	CUtlVector< FeCtrlOffset_t > m_CtrlOffsets;
	CUtlVector< FeCtrlOsOffset_t > m_CtrlOsOffsets;
	CUtlVector< FeFollowNode_t > m_FollowNodes;
	CUtlVector< FeCollisionPlane_t > m_CollisionPlanes;
	CUtlVector< FeNodeIntegrator_t > m_NodeIntegrator;
	CUtlVector< FeSpringIntegrator_t > m_SpringIntegrator;
	CUtlVector< FeSimdSpringIntegrator_t > m_SimdSpringIntegrator;
	CUtlVector< FeWorldCollisionParams_t > m_WorldCollisionParams;
	CUtlVector< float32 > m_LegacyStretchForce;
	CUtlVector< float32 > m_NodeCollisionRadii;
	CUtlVector< float32 > m_DynNodeFriction;
	CUtlVector< float32 > m_LocalRotation;
	CUtlVector< float32 > m_LocalForce;
	CUtlVector< FeTaperedCapsuleStretch_t > m_TaperedCapsuleStretches;
	CUtlVector< FeTaperedCapsuleRigid_t > m_TaperedCapsuleRigids;
	CUtlVector< FeSphereRigid_t > m_SphereRigids;
	CUtlVector< uint16 > m_WorldCollisionNodes;
	CUtlVector< uint16 > m_TreeParents;
	CUtlVector< uint16 > m_TreeCollisionMasks;
	CUtlVector< FeTreeChildren_t > m_TreeChildren;
	CUtlVector< uint16 > m_FreeNodes;
	CUtlVector< FeFitMatrix_t > m_FitMatrices;
	CUtlVector< FeFitWeight_t > m_FitWeights;
	CUtlVector< FeNodeReverseOffset_t > m_ReverseOffsets;
	CUtlVector< FeAnimStrayRadius_t > m_AnimStrayRadii;
	CUtlVector< FeSimdAnimStrayRadius_t > m_SimdAnimStrayRadii;
	CUtlVector< FeKelagerBend2_t > m_KelagerBends;
	CUtlVector< FeCtrlSoftOffset_t > m_CtrlSoftOffsets;
	CUtlVector< CFeIndexedJiggleBone > m_JiggleBones;
	CUtlVector< uint16 > m_SourceElems;
	CUtlVector< uint32 > m_GoalDampedSpringIntegrators;
	CUtlVector< FeTri_t > m_Tris;
	uint16 m_nTriCount1;
	uint16 m_nTriCount2;
	uint8 m_nReservedUint8;
	uint8 m_nExtraPressureIterations;
	uint8 m_nExtraGoalIterations;
	uint8 m_nExtraIterations;
	CUtlVector< FeSDFRigid_t > m_SDFRigids;
	CUtlVector< FeBoxRigid_t > m_BoxRigids;
	CUtlVector< uint8 > m_DynNodeVertexSet;
	CUtlVector< uint32 > m_VertexSetNames;
	CUtlVector< FeRigidColliderIndices_t > m_RigidColliderPriorities;
	CUtlVector< FeMorphLayerDepr_t > m_MorphLayers;
	CUtlVector< uint8 > m_MorphSetData;
	CUtlVector< FeVertexMapDesc_t > m_VertexMaps;
	CUtlVector< uint8 > m_VertexMapValues;
	CUtlVector< FeEffectDesc_t > m_Effects;
	CUtlVector< FeCtrlOffset_t > m_LockToParent;
	CUtlVector< uint16 > m_LockToGoal;
	CUtlVector< int16 > m_SkelParents;
	CUtlVector< FeNodeWindBase_t > m_DynNodeWindBases;
	CUtlVector< FeModelSelfCollisionLayer_t > m_SelfCollisionLayers;
	float32 m_flInternalPressure;
	float32 m_flDefaultTimeDilation;
	float32 m_flWindage;
	float32 m_flWindDrag;
	float32 m_flDefaultSurfaceStretch;
	float32 m_flDefaultThreadStretch;
	float32 m_flDefaultGravityScale;
	float32 m_flDefaultVelAirDrag;
	float32 m_flDefaultExpAirDrag;
	float32 m_flDefaultVelQuadAirDrag;
	float32 m_flDefaultExpQuadAirDrag;
	float32 m_flRodVelocitySmoothRate;
	float32 m_flQuadVelocitySmoothRate;
	float32 m_flAddWorldCollisionRadius;
	float32 m_flDefaultVolumetricSolveAmount;
	float32 m_flMotionSmoothCDT;
	float32 m_flLocalDrag1;
	uint16 m_nRodVelocitySmoothIterations;
	uint16 m_nQuadVelocitySmoothIterations;

	uint GetNodeCount() const { return m_InitPose.Count(); }
	uint GetDynamicNodeCount() const { return m_nNodeCount - m_nStaticNodes; }
};

CLockedResource< PhysFeModelDesc_t > Clone( CFeModel *pFeModel, CResourceStream *pStream );
void Clone( const PhysFeModelDesc_t *pFeDesc, intp nOffsetBytes, char **pCtrlNames, CFeModel *pFeModel );


class CFeModel;
class CFeModelReplaceContext
{
public:
	CFeModelReplaceContext( const CFeModel *pOld, const CFeModel *pNew );
	const CFeModel *GetOld() { return m_pOld; }
	const CFeModel *GetNew() { return m_pNew; }
	int OldToNewNode( int i ) { return m_OldToNewNode[ i ]; }
	int NewToOldNode( int i ){ return m_NewToOldNode[ i ]; }
	int OldToNewCtrl( int i ) { return m_OldToNewCtrl[ i ]; }
	int NewToOldCtrl( int i ){ return m_NewToOldCtrl[ i ]; }
protected:
	const CFeModel *m_pOld;
	const CFeModel *m_pNew;
	CUtlVector< int > m_OldToNewNode, m_NewToOldNode;
	CUtlVector< int > m_OldToNewCtrl, m_NewToOldCtrl;
};


#endif
