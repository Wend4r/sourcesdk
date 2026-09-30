#ifndef CSTRIKE15_INBUTTONSTATE_H
#define CSTRIKE15_INBUTTONSTATE_H

#pragma once

#include "basetypes.h"
#include "schemasystem/schematypes.h"

// The input bits and their per-command state, apart from the user command itself so that
// entity code can use them without the protobuf message.
enum InputBitMask_t : int64
{
	IN_NONE = 0,
	IN_ALL = ~0,

	IN_ATTACK = 1 << 0,
	IN_JUMP = 1 << 1,
	IN_DUCK = 1 << 2,
	IN_FORWARD = 1 << 3,
	IN_BACK = 1 << 4,
	IN_USE = 1 << 5,
	IN_TURNLEFT = 1 << 7,
	IN_TURNRIGHT = 1 << 8,
	IN_MOVELEFT = 1 << 9,
	IN_MOVERIGHT = 1 << 10,
	IN_ATTACK2 = 1 << 11,
	IN_RELOAD = 1 << 13,
	IN_SPEED = 1 << 16,
	IN_JOYAUTOSPRINT = 1 << 17,

	IN_FIRST_MOD_SPECIFIC_BIT = 1ll << 32,
	IN_USEORRELOAD = 1ll << 32,
	IN_SCORE = 1ll << 33,
	IN_ZOOM = 1ll << 34,
	IN_LOOK_AT_WEAPON = 1ll << 35,
};

enum EInButtonState : uint64
{
	IN_BUTTON_UP = 0,
	IN_BUTTON_DOWN = 1,
	IN_BUTTON_DOWN_UP = 2,
	IN_BUTTON_UP_DOWN = 3,
	IN_BUTTON_UP_DOWN_UP = 4,
	IN_BUTTON_DOWN_UP_DOWN = 5,
	IN_BUTTON_DOWN_UP_DOWN_UP = 6,
	IN_BUTTON_UP_DOWN_UP_DOWN = 7,
	IN_BUTTON_STATE_COUNT = 8,
};

class CInButtonState
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() { return {}; };

	EInButtonState GetButtonState( uint64 button )
	{
		return static_cast< EInButtonState >( ( !!( m_nValue & button ) + !!( m_nValueChanged & button ) * 2 + !!( m_nValueScroll & button ) * 4 ) );
	};

	bool IsButtonNewlyPressed( uint64 button )
	{
		return GetButtonState( button ) >= 3;
	}

public:
	uint64 m_nValue = 0;
	uint64 m_nValueChanged = 0;
	uint64 m_nValueScroll = 0;
};

#endif // CSTRIKE15_INBUTTONSTATE_H
