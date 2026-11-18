#ifndef IATTACHMENTOVERRIDE_H
#define IATTACHMENTOVERRIDE_H

#pragma once

#include "tier0/platform.h"
#include "tier0/utlstringtoken.h"

struct matrix3x4_t;

// An attachment of a model by index
typedef uint8 AttachmentHandle_t;

enum AttachmentOverrideResult_t
{
	ATTACHMENT_RESULT_OVERRIDDEN = 0,
	ATTACHMENT_RESULT_USE_DEFAULT = 1,
};

//-----------------------------------------------------------------------------
// Lets an entity replace the attachments of its model: CEconEntity derives from
// it, CCSWeaponBase answers for the weapons.
//
// Virtual methods only, in the order of the game's table (2 slots on both
// platforms, no destructor). Names, arguments and the result enum are the ones
// of Dota 2 2017 (DWARF: CBaseCombatWeapon::LookupOverrideAttachment,
// CBaseViewModel::GetOverrideAttachment).
//-----------------------------------------------------------------------------
abstract_class IAttachmentOverride
{
public:
	// Finds the attachment of that name; ATTACHMENT_RESULT_USE_DEFAULT leaves it to the model
	virtual AttachmentOverrideResult_t LookupOverrideAttachment( CUtlStringToken nAttachmentName, AttachmentHandle_t *pHandle ) const = 0;
	// The transform of an attachment; ATTACHMENT_RESULT_USE_DEFAULT leaves it to the model
	virtual AttachmentOverrideResult_t GetOverrideAttachment( AttachmentHandle_t hAttachment, matrix3x4_t *pAttachmentToWorld, bool *pOk ) const = 0;
};

#endif // IATTACHMENTOVERRIDE_H
