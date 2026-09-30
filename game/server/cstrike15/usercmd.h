#ifndef CSTRIKE15_SERVER_USERCMD_H
#define CSTRIKE15_SERVER_USERCMD_H

#pragma once

#include "../../shared/cstrike15/usercmd.h"

//-----------------------------------------------------------------------------
// Server side user command, as laid out by the server module.
//
// The server keeps its pending commands in an intrusive list, so the tail of
// the class differs from the client one.
//-----------------------------------------------------------------------------
class CCSGOUserCmd : public CUserCmdBaseHost< CSGOUserCmdPB >
{
public:
	void ButtonsToMessage() override
	{
		if ( m_ButtonStates.m_nValue || m_ButtonStates.m_nValueChanged || m_ButtonStates.m_nValueScroll )
		{
			CInButtonStatePB *pButtons = MutableBase()->mutable_buttons_pb();
			pButtons->set_buttonstate1( m_ButtonStates.m_nValue );
			pButtons->set_buttonstate2( m_ButtonStates.m_nValueChanged );
			pButtons->set_buttonstate3( m_ButtonStates.m_nValueScroll );
		}
		else
		{
			MutableBase()->clear_buttons_pb();
		}
	}

	void ButtonsFromMessage() override
	{
		const CInButtonStatePB &buttons = GetBase().buttons_pb();
		m_ButtonStates.m_nValue = buttons.buttonstate1();
		m_ButtonStates.m_nValueChanged = buttons.buttonstate2();
		m_ButtonStates.m_nValueScroll = buttons.buttonstate3();
		m_nFlags = 0;
	}

public:
	CInButtonState m_ButtonStates;

	// Defaults to -1. The controller keeps a copy of it after running the command.
	int32 m_nCSGOUserCmdUnknown;

#ifdef _WIN32
private:
	// Tail padding of the intermediate class, which MSVC does not reuse.
	uint8 m_unk7C[ 4 ];

public:
#endif
	int32 m_nFlags;

	CCSGOUserCmd *m_pPrev;
	CCSGOUserCmd *m_pNext;
};

#endif // CSTRIKE15_SERVER_USERCMD_H
