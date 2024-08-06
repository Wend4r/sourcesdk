//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef ISERVERNETWORKABLE_H
#define ISERVERNETWORKABLE_H
#ifdef _WIN32
#pragma once
#endif


#include "entity2/entityinstance.h"
#include "basetypes.h"
#include "bitvec.h"
#include "const.h"
#include "bspfile.h"



// Entities can span this many clusters before we revert to a slower area checking algorithm
#define	MAX_FAST_ENT_CLUSTERS	4
#define	MAX_ENT_CLUSTERS	64
#define MAX_WORLD_AREAS		8


class ServerClass;
class SendTable;
class vis_info_t;
struct edict_t;
class CBaseEntity;
class CSerialEntity;
class CBaseNetworkable;


class CCheckTransmitInfo
{
public:
	CBitVec<MAX_EDICTS>* m_pTransmitEntity;	// 0
	CBitVec<MAX_EDICTS>* m_pUnkBitVec;		// 8
	CBitVec<MAX_EDICTS>* m_pUnkBitVec2;		// 16
	CBitVec<MAX_EDICTS>* m_pUnkBitVec3;		// 24
	CBitVec<MAX_EDICTS>* m_pTransmitAlways; // 32
	CUtlVector<int> m_unk40;				// 40
	vis_info_t* m_VisInfo;					// 64
	[[maybe_unused]] byte m_unk72[0x200];	// 72
	CEntityIndex m_nClientEntityIndex;		// 584
	bool m_bFullUpdate;						// 588
}; // sizeof unknown(

//-----------------------------------------------------------------------------
// Stores information necessary to perform PVS testing.
//-----------------------------------------------------------------------------
struct PVSInfo_t
{
	// headnode for the entity's bounding box
	short		m_nHeadNode;			

	// number of clusters or -1 if too many
	short		m_nClusterCount;		

	// cluster indices
	unsigned short *m_pClusters;	

	// For dynamic "area portals"
	short		m_nAreaNum;
	short		m_nAreaNum2;

	// current position
	float		m_vCenter[3];

private:
	unsigned short m_pClustersInline[MAX_FAST_ENT_CLUSTERS];

	friend class CVEngineServer;
};


// IServerNetworkable is the interface the engine uses for all networkable data.
class IServerNetworkable
{
// These functions are handled automatically by the server_class macros and CBaseNetworkable.
public:
	// Gets at the entity handle associated with the collideable
	virtual CEntityInstance	*GetEntityHandle() = 0;

	// Tell the engine which class this object is.
	virtual ServerClass*	GetServerClass() = 0;

	virtual edict_t			*GetEdict() const = 0;

	virtual const char*		GetClassName() const = 0;
	virtual void			Release() = 0;

	virtual int				AreaNum() const = 0;

	// In place of a generic QueryInterface.
	virtual CBaseNetworkable* GetBaseNetworkable() = 0;
	virtual CBaseEntity*	GetBaseEntity() = 0; // Only used by game code.
	virtual PVSInfo_t*		GetPVSInfo() = 0; // get current visibilty data

protected:
	// Should never call delete on this! 
	virtual					~IServerNetworkable() {}
};


#endif // ISERVERNETWORKABLE_H
