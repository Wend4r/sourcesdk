#ifndef ECON_ENTITY_H
#define ECON_ENTITY_H

#pragma once

#include "../baseanimgraph.h"
#include "ihasattributes.h"
#include "../iattachmentoverride.h"
#include "../ibonetransformoverride.h"

class CAttributeContainer;
class CAttributeList;
class CAttributeManager;

//-----------------------------------------------------------------------------
// CEconEntity adds 9 methods to CBaseAnimGraph, Windows slots 306-314. The Linux table
// has 16, slots 307-322.
//
// The class has three second bases, in the order of its RTTI: IHasAttributes (Linux 3360,
// Windows 2624 bytes into the entity), IAttachmentOverride and IBoneTransformOverride. The
// seven methods marked "Linux only" override methods of the first two: the Itanium ABI also
// enters such an override into the primary table, MSVC keeps it in the table of the second
// base. Schema_DynamicBinding and GetSerializerClassInfo of IHasAttributes are methods of
// CEntityInstance as well, so their overrides take no slot of their own. The second bases sit
// at offsets 8, 16 and 24 here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseAnimGraphVTable<> >
abstract_class CEconEntityVTable : public TBase, public IHasAttributes, public IAttachmentOverride, public IBoneTransformOverride
{
public:
	// Overrides the method of CEntityInstance and the one of IHasAttributes: no slot of its own
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override = 0;
	// Overrides the method of CEntityInstance and the one of IHasAttributes: no slot of its own
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() override = 0;
	// Returns ATTACHMENT_RESULT_USE_DEFAULT; overridden by CCSWeaponBase. Overrides IAttachmentOverride: a slot of this table on Linux only
	virtual AttachmentOverrideResult_t LookupOverrideAttachment( CUtlStringToken nAttachmentName, AttachmentHandle_t *pHandle ) const override = 0;
	// Returns ATTACHMENT_RESULT_USE_DEFAULT; overridden by CCSWeaponBase. Overrides IAttachmentOverride: a slot of this table on Linux only
	virtual AttachmentOverrideResult_t GetOverrideAttachment( AttachmentHandle_t hAttachment, matrix3x4_t *pAttachmentToWorld, bool *pOk ) const override = 0;
	// Empty; overridden by CEconWearable
	virtual void GiveTo( CBaseEntity *pOther ) = 0;
	// Returns 0, never overridden
	virtual int CalculateVisibleClassFor( CBaseEntity *pPlayer ) = 0;
	// m_AttributeManager. Overrides IHasAttributes: a slot of this table on Linux only
	virtual CAttributeManager *GetAttributeManager() override = 0;
	// m_AttributeManager. Overrides IHasAttributes: a slot of this table on Linux only
	virtual CAttributeContainer *GetAttributeContainer() override = 0;
	// The owner entity. Overrides IHasAttributes: a slot of this table on Linux only
	virtual CBaseEntity *GetAttributeOwner() override = 0;
	// The attribute list of the item. Overrides IHasAttributes: a slot of this table on Linux only
	virtual CAttributeList *GetAttributeList() override = 0;
	// Stores m_OriginalOwnerXuidLow and m_OriginalOwnerXuidHigh
	virtual void SetOriginalOwnerXuid( uint32 nLow, uint32 nHigh ) = 0;
	virtual uint64 GetOriginalOwnerXuid() = 0;
	// m_nFallbackPaintKit
	virtual int GetFallbackPaintKit() = 0;
	// m_nFallbackSeed
	virtual int GetFallbackSeed() = 0;
	// m_flFallbackWear as an integer
	virtual int GetFallbackWear() = 0;
	// m_nFallbackStatTrak
	virtual int GetFallbackStatTrak() = 0;
	// The attributes follow a change of the owner. Overrides IHasAttributes: a slot of this table on Linux only
	virtual void ReapplyProvision() override = 0;
	// Never overridden
	virtual bool UpdateBodygroups( CBaseCombatCharacter *pOwner, int nState ) = 0;
};

#endif // ECON_ENTITY_H
