//====== Copyright (c), Valve Corporation, All rights reserved. =======
//
// Purpose: Shared objects of the CS2 SO cache that wrap a protobuf message
//
//=============================================================================

#ifndef CSTRIKE15_GCMESSAGES_H
#define CSTRIKE15_GCMESSAGES_H
#ifdef _WIN32
#pragma once
#endif

#include "econ/econ_item_constants.h"
#include "gcsdk/sharedobject.h"
#include "gcsdk/protobufsharedobject.h"

#include <cstrike15_gcmessages.pb.h>

//-----------------------------------------------------------------------------
// Every class below is the protobuf shared object template and nothing else: the game
// registers each one under its own name, with the eleven virtuals of the template and the
// message as the only member.
// The server registers CEconPersonaDataPublic, CEconRecurringMission,
// CEconGameAccountSteamChina, CEconAccountSeasonalOperationGS and CEconQuestProgress; the
// others exist in the client alone.
//-----------------------------------------------------------------------------
class CEconPersonaDataPublic : public GCSDK::CProtoBufSharedObject< CSOPersonaDataPublic, k_EEconTypePersonaDataPublic >
{
public:
	const static int k_nTypeID = k_EEconTypePersonaDataPublic;
	virtual int GetTypeID() const { return k_nTypeID; }
};

class CEconAccountItemPersonalStore : public GCSDK::CProtoBufSharedObject< CSOAccountItemPersonalStore, k_EEconTypeAccountItemPersonalStore >
{
public:
	const static int k_nTypeID = k_EEconTypeAccountItemPersonalStore;
	virtual int GetTypeID() const { return k_nTypeID; }
};

class CEconAccountXpShop : public GCSDK::CProtoBufSharedObject< CSOAccountXpShop, k_EEconTypeAccountXpShop >
{
public:
	const static int k_nTypeID = k_EEconTypeAccountXpShop;
	virtual int GetTypeID() const { return k_nTypeID; }
};

class CEconAccountKeychainRemoveToolCharges : public GCSDK::CProtoBufSharedObject< CSOAccountKeychainRemoveToolCharges, k_EEconTypeAccountKeychainRemoveToolCharges >
{
public:
	const static int k_nTypeID = k_EEconTypeAccountKeychainRemoveToolCharges;
	virtual int GetTypeID() const { return k_nTypeID; }
};

class CEconRecurringMission : public GCSDK::CProtoBufSharedObject< CSOAccountRecurringMission, k_EEconTypeRecurringMission >
{
public:
	const static int k_nTypeID = k_EEconTypeRecurringMission;
	virtual int GetTypeID() const { return k_nTypeID; }
};

class CEconGameAccountSteamChina : public GCSDK::CProtoBufSharedObject< CSOGameAccountSteamChina, k_EEconTypeGameAccountSteamChina >
{
public:
	const static int k_nTypeID = k_EEconTypeGameAccountSteamChina;
	virtual int GetTypeID() const { return k_nTypeID; }
};

class CEconAccountXpShopBids : public GCSDK::CProtoBufSharedObject< CSOAccountXpShopBids, k_EEconTypeAccountXpShopBids >
{
public:
	const static int k_nTypeID = k_EEconTypeAccountXpShopBids;
	virtual int GetTypeID() const { return k_nTypeID; }
};

class CEconVolatileItemOffer : public GCSDK::CProtoBufSharedObject< CSOVolatileItemOffer, k_EEconTypeVolatileItemOffer >
{
public:
	const static int k_nTypeID = k_EEconTypeVolatileItemOffer;
	virtual int GetTypeID() const { return k_nTypeID; }
};

class CEconVolatileItemClaimedRewards : public GCSDK::CProtoBufSharedObject< CSOVolatileItemClaimedRewards, k_EEconTypeVolatileItemClaimedRewards >
{
public:
	const static int k_nTypeID = k_EEconTypeVolatileItemClaimedRewards;
	virtual int GetTypeID() const { return k_nTypeID; }
};

class CEconAccountRecurringSubscription : public GCSDK::CProtoBufSharedObject< CSOAccountRecurringSubscription, k_EEconTypeAccountRecurringSubscription >
{
public:
	const static int k_nTypeID = k_EEconTypeAccountRecurringSubscription;
	virtual int GetTypeID() const { return k_nTypeID; }
};

// The client's copy of the seasonal operation and the game server's one carry the same
// message under two type ids.
class CEconAccountSeasonalOperation : public GCSDK::CProtoBufSharedObject< CSOAccountSeasonalOperation, k_EEconTypeAccountSeasonalOperation >
{
public:
	const static int k_nTypeID = k_EEconTypeAccountSeasonalOperation;
	virtual int GetTypeID() const { return k_nTypeID; }
};

class CEconAccountSeasonalOperationGS : public GCSDK::CProtoBufSharedObject< CSOAccountSeasonalOperation, k_EEconTypeAccountSeasonalOperationGS >
{
public:
	const static int k_nTypeID = k_EEconTypeAccountSeasonalOperationGS;
	virtual int GetTypeID() const { return k_nTypeID; }
};

class CEconCoupon : public GCSDK::CProtoBufSharedObject< CSOEconCoupon, k_EEconTypeCoupon >
{
public:
	const static int k_nTypeID = k_EEconTypeCoupon;
	virtual int GetTypeID() const { return k_nTypeID; }
};

class CEconQuestProgress : public GCSDK::CProtoBufSharedObject< CSOQuestProgress, k_EEconTypeQuest >
{
public:
	const static int k_nTypeID = k_EEconTypeQuest;
	virtual int GetTypeID() const { return k_nTypeID; }
};

#endif // CSTRIKE15_GCMESSAGES_H
