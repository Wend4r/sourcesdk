#ifndef BASEPLAYER_VDATA_H
#define BASEPLAYER_VDATA_H

#ifdef _WIN32
#pragma once
#endif

#include "entitysubclassvdata.h"
#include "resourcesystem/resourcehandle.h"
#include "resourcesystem/resourcehandletypes.h"
#include "resourcesystem/resourcename.h"

#include "schemacompiler/schemaon.h"

// A value per skill level
schema class CSkillFloat
{
public:
	TYPEMETA( MPropertyCustomEditor = "multi_float( 4 )" );

	float32 m_pValue[ 4 ];
};

//-----------------------------------------------------------------------------
// Data of a player pawn subclass. Adds no virtual method to CEntitySubclassVDataBase.
//-----------------------------------------------------------------------------
schema class CBasePlayerVData : public CEntitySubclassVDataBase
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	~CBasePlayerVData() override;
	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	void KV3TransferSave( CKV3TransferSaveContext *pContext ) const override;
	void KV3TransferLoad( CKV3TransferLoadContext *pContext ) override;
	const char *GetKV3TransferClassName() const override;

	// Precaches both models
	void Precache( const CEntityPrecacheContext *pContext ) override;

	CResourceNameTyped< CWeakHandle< InfoForResourceTypeCModel > > m_sModelName; META( MPropertyProvidesEditContextString = "ToolEditContext_ID_VMDL" );
	CResourceNameTyped< CWeakHandle< InfoForResourceTypeCModel > > m_sModelNameAg2Override; META( MPropertyProvidesEditContextString = "ToolEditContext_ID_VMDL" );
	CSkillFloat m_flHeadDamageMultiplier;
	CSkillFloat m_flChestDamageMultiplier;
	CSkillFloat m_flStomachDamageMultiplier;
	CSkillFloat m_flArmDamageMultiplier;
	CSkillFloat m_flLegDamageMultiplier;
	float32 m_flHoldBreathTime; META( MPropertyGroupName = "Water" );
	float32 m_flDrowningDamageInterval; META( MPropertyGroupName = "Water"; MPropertyDescription = "Seconds between drowning ticks" );
	int32 m_nDrowningDamageInitial; META( MPropertyGroupName = "Water"; MPropertyDescription = "Amount of damage done on the first drowning tick (+1 each subsequent interval)" );
	int32 m_nDrowningDamageMax; META( MPropertyGroupName = "Water"; MPropertyDescription = "Max damage done by a drowning tick" );
	int32 m_nWaterSpeed; META( MPropertyGroupName = "Water" );
	float32 m_flUseRange; META( MPropertyGroupName = "Use" );
	float32 m_flUseAngleTolerance; META( MPropertyGroupName = "Use" );
	float32 m_flCrouchTime; META( MPropertyGroupName = "Crouch"; MPropertyDescription = "Time to move between crouch and stand" );
};

#include "schemacompiler/schemaoff.h"

#endif // BASEPLAYER_VDATA_H
