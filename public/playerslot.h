#ifndef PLAYERSLOT_H
#define PLAYERSLOT_H

#if _WIN32
#pragma once
#endif

#include "const.h"

#define INVALID_PLAYER_SLOT_INDEX -1
#define INVALID_PLAYER_SLOT CPlayerSlot( INVALID_PLAYER_SLOT_INDEX )

class CPlayerSlot
{
public:
	CPlayerSlot( int slot = INVALID_PLAYER_SLOT_INDEX ) : m_Data( slot ) {}

	void Invalidate() { m_Data = INVALID_PLAYER_SLOT_INDEX; }
	bool IsValid() const { return m_Data >= 0 && m_Data < ABSOLUTE_PLAYER_LIMIT; }
	int Get() const { return m_Data; }

	int GetEntityIndex() const { return m_Data + 1; }
	int GetClientIndex() const { return GetEntityIndex(); }

	static int InvalidIndex() { return INVALID_PLAYER_SLOT; }

	operator int() const { return m_Data; }
	bool operator==( const CPlayerSlot &other ) const { return other.m_Data == m_Data; }
	bool operator!=( const CPlayerSlot &other ) const { return other.m_Data != m_Data; }
	CPlayerSlot& operator++() { ++m_Data; return *this; }
	CPlayerSlot operator++( int ) { CPlayerSlot temp = *this; ++m_Data; return temp; }

private:
	int m_Data;
};

#endif // PLAYERSLOT_H