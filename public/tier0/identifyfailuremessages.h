#ifndef IDENTIFYFAILUREMESSAGES_H
#define IDENTIFYFAILUREMESSAGES_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"

//-----------------------------------------------------------------------------
// Scans log text line by line for fatal error messages
//-----------------------------------------------------------------------------
class CIdentifyFailureMessages
{
public:
	enum State_t : int;

	CIdentifyFailureMessages() : unk000( false ), m_State( State_t( 0 ) ), unk018( 0 ) {}

	// Feeds nLength chars of pszText (up to the terminator when negative), returns the state
	// after it and the end of the processed text in ppszEnd
	DLL_CLASS_IMPORT State_t ProcessText( const char *pszText, int nLength, const char **ppszEnd );

	State_t GetState() const { return m_State; }
	const char *GetMessage() const { return m_Message.Get(); }

private:
	bool unk000;
	State_t m_State;
	CUtlString m_Message;
	CUtlString unk010;
	int unk018;
};

#endif // IDENTIFYFAILUREMESSAGES_H
