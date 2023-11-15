#ifndef SPLITSCREENSLOT_H
#define SPLITSCREENSLOT_H

#if _WIN32
#pragma once
#endif

struct CSplitScreenSlot
{
	CSplitScreenSlot( int index )
	{
		m_Data = index;
	}
	
	int Get() const
	{
		return m_Data;
	}
	
	int m_Data;
};

#endif // SPLITSCREENSLOT_H