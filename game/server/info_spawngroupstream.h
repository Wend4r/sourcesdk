#ifndef INFO_SPAWNGROUPSTREAM_H
#define INFO_SPAWNGROUPSTREAM_H

#pragma once

#include "baseentity.h"

//-----------------------------------------------------------------------------
// CInfoSpawnGroupLoadUnload adds 4 methods to CLogicalEntity (through CLogicalEntity and CServerOnlyEntity, which add no method): Linux slots 239-242, Windows 238-241.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CInfoSpawnGroupLoadUnloadVTable : public TBase
{
public:
	// Called by the input and by CInfoSpawnGroupLoadUnload_API::StartSpawnGroupLoad, log "SV:  InputStartSpawnGroupLoad(%s)"
	virtual void StartSpawnGroupLoad() = 0;
	// Called by CInfoSpawnGroupLoadUnload_API::StartSpawnGroupUnload, log "SV:  InputStartSpawnGroupUnload(%s)"
	virtual void StartSpawnGroupUnload() = 0;
	// Called by CInfoSpawnGroupLoadUnload_API::ActivateSpawnGroup, log "InputActivateSpawnGroup called with invalid spawn group"
	virtual void ActivateSpawnGroup() = 0;
	// Called by CInfoSpawnGroupLoadUnload_API::SetSpawnGroup; converts the variant to a string into m_iszSpawnGroupName
	virtual void SetSpawnGroup( const variant_t &value ) = 0;
};

#endif // INFO_SPAWNGROUPSTREAM_H
