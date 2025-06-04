#ifndef PLAYERSLOT_H
#define PLAYERSLOT_H

#if _WIN32
#pragma once
#endif

#define INVALID_PLAYER_SLOT_INDEX -1
#define INVALID_PLAYER_SLOT CPlayerSlot( INVALID_PLAYER_SLOT_INDEX )

class CPlayerSlot
{
public:
	CPlayerSlot( int slot = INVALID_PLAYER_SLOT_INDEX ) : m_Data( slot ) {}

	operator int() const { return m_Data; }
	bool operator==( const CPlayerSlot &other ) const { return other.m_Data == m_Data; }
	bool operator!=( const CPlayerSlot &other ) const { return other.m_Data != m_Data; }

	int Get() const { return m_Data; }
	int GetEntityIndex() const { return m_Data + 1; }
	int GetClientIndex() const { return GetEntityIndex(); }

	static int InvalidIndex() { return INVALID_PLAYER_SLOT; }
	bool IsValid() const { return m_Data != InvalidIndex(); }

private:
	int m_Data;
};

#endif // PLAYERSLOT_H