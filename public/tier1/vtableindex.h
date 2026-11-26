//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Finds the virtual table index of a method from a pointer to it.
//
//=============================================================================//

#ifndef VTABLEINDEX_H
#define VTABLEINDEX_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// Each method returns its own index: a pointer to a member, called on an object of this class, gives the
// virtual table index of the method it points to.
//-----------------------------------------------------------------------------
abstract_class IVTableIndexCalculator
{
public:
	virtual int GetIndex_0() = 0;
	virtual int GetIndex_1() = 0;
	virtual int GetIndex_2() = 0;
	virtual int GetIndex_3() = 0;
	virtual int GetIndex_4() = 0;
	virtual int GetIndex_5() = 0;
	virtual int GetIndex_6() = 0;
	virtual int GetIndex_7() = 0;
	virtual int GetIndex_8() = 0;
	virtual int GetIndex_9() = 0;
	virtual int GetIndex_10() = 0;
	virtual int GetIndex_11() = 0;
	virtual int GetIndex_12() = 0;
	virtual int GetIndex_13() = 0;
	virtual int GetIndex_14() = 0;
	virtual int GetIndex_15() = 0;
	virtual int GetIndex_16() = 0;
	virtual int GetIndex_17() = 0;
	virtual int GetIndex_18() = 0;
	virtual int GetIndex_19() = 0;
	virtual int GetIndex_20() = 0;
	virtual int GetIndex_21() = 0;
	virtual int GetIndex_22() = 0;
	virtual int GetIndex_23() = 0;
	virtual int GetIndex_24() = 0;
	virtual int GetIndex_25() = 0;
	virtual int GetIndex_26() = 0;
	virtual int GetIndex_27() = 0;
	virtual int GetIndex_28() = 0;
	virtual int GetIndex_29() = 0;
	virtual int GetIndex_30() = 0;
	virtual int GetIndex_31() = 0;
	virtual int GetIndex_32() = 0;
	virtual int GetIndex_33() = 0;
	virtual int GetIndex_34() = 0;
	virtual int GetIndex_35() = 0;
	virtual int GetIndex_36() = 0;
	virtual int GetIndex_37() = 0;
	virtual int GetIndex_38() = 0;
	virtual int GetIndex_39() = 0;
	virtual int GetIndex_40() = 0;
	virtual int GetIndex_41() = 0;
	virtual int GetIndex_42() = 0;
	virtual int GetIndex_43() = 0;
	virtual int GetIndex_44() = 0;
	virtual int GetIndex_45() = 0;
	virtual int GetIndex_46() = 0;
	virtual int GetIndex_47() = 0;
	virtual int GetIndex_48() = 0;
	virtual int GetIndex_49() = 0;
	virtual int GetIndex_50() = 0;
	virtual int GetIndex_51() = 0;
	virtual int GetIndex_52() = 0;
	virtual int GetIndex_53() = 0;
	virtual int GetIndex_54() = 0;
	virtual int GetIndex_55() = 0;
	virtual int GetIndex_56() = 0;
	virtual int GetIndex_57() = 0;
	virtual int GetIndex_58() = 0;
	virtual int GetIndex_59() = 0;
	virtual int GetIndex_60() = 0;
	virtual int GetIndex_61() = 0;
	virtual int GetIndex_62() = 0;
	virtual int GetIndex_63() = 0;
	virtual int GetIndex_64() = 0;
};

#endif // VTABLEINDEX_H
