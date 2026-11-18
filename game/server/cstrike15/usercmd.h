#ifndef CSTRIKE15_SERVER_USERCMD_H
#define CSTRIKE15_SERVER_USERCMD_H

#pragma once

#include "../../shared/cstrike15/usercmd.h"

//-----------------------------------------------------------------------------
// Server side user command, as laid out by the server module (RTTI: CCSGOUserCmd derives from
// CUserCmdBaseHost< CSGOUserCmdPB >, CUserCmd from CCSGOUserCmd).
//-----------------------------------------------------------------------------
class CCSGOUserCmd : public CUserCmdBaseHost< CSGOUserCmdPB >
{
public:
	void ButtonsToMessage() override
	{
		if ( m_ButtonStates.m_pButtonStates[ 0 ] || m_ButtonStates.m_pButtonStates[ 1 ] || m_ButtonStates.m_pButtonStates[ 2 ] )
		{
			CInButtonStatePB *pButtons = MutableBase()->mutable_buttons_pb();
			pButtons->set_buttonstate1( m_ButtonStates.m_pButtonStates[ 0 ] );
			pButtons->set_buttonstate2( m_ButtonStates.m_pButtonStates[ 1 ] );
			pButtons->set_buttonstate3( m_ButtonStates.m_pButtonStates[ 2 ] );
		}
		else
		{
			MutableBase()->clear_buttons_pb();
		}
	}

	void ButtonsFromMessage() override
	{
		const CInButtonStatePB &buttons = GetBase().buttons_pb();
		m_ButtonStates.m_pButtonStates[ 0 ] = buttons.buttonstate1();
		m_ButtonStates.m_pButtonStates[ 1 ] = buttons.buttonstate2();
		m_ButtonStates.m_pButtonStates[ 2 ] = buttons.buttonstate3();
	}

public:
	// Named by what is known: -1 by default, copied by the copy constructor; the controller keeps
	// a copy of it after running the command.
	int32 m_nControllerTrackedValue;
};

//-----------------------------------------------------------------------------
// The command the server queues and the bots build: the server keeps its pending commands in
// an intrusive list. Under the Itanium ABI m_nFlags starts in the tail padding of CCSGOUserCmd,
// as in libserver.so; MSVC does not reuse it.
//-----------------------------------------------------------------------------
class CUserCmd : public CCSGOUserCmd
{
public:
	// Also clears m_nFlags.
	void ButtonsFromMessage() override
	{
		CCSGOUserCmd::ButtonsFromMessage();
		m_nFlags = 0;
	}

public:
	int32 m_nFlags;

	CUserCmd *m_pPrev;
	CUserCmd *m_pNext;
};

#endif // CSTRIKE15_SERVER_USERCMD_H
