#ifndef ENTITYINDEX_H
#define ENTITYINDEX_H

#pragma once

#define INVALID_ENTITY_INDEX -1

class CEntityIndex
{
public:
	CEntityIndex( int index = INVALID_ENTITY_INDEX ) : m_Data( index ) {}

	void Invalidate() { m_Data = INVALID_ENTITY_INDEX; }

	int Get() const { return m_Data; }
	operator int() const { return m_Data; }

	bool operator< ( int index ) const { return m_Data < index; }
	bool operator< ( const CEntityIndex &other ) const { return m_Data <  other.m_Data; }
	bool operator==( const CEntityIndex &other ) const { return m_Data == other.m_Data; }
	bool operator!=( const CEntityIndex &other ) const { return m_Data != other.m_Data; }

private:
	int m_Data;
};

#endif // !defined( ENTITYINDEX_H )
