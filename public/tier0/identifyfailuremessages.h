#ifndef IDENTIFYFAILUREMESSAGES_H
#define IDENTIFYFAILUREMESSAGES_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"

//-----------------------------------------------------------------------------
// Scans log text line by line for failed assertions and fatal errors
//-----------------------------------------------------------------------------
class CIdentifyFailureMessages
{
public:
	enum State_t : int
	{
		// At the start of a line, before its log prefix
		STATE_LINE_START = 0,

		// In the text of a line after the prefix
		STATE_LINE_TEXT,

		// After a "<file> (<line>) : Assertion Failed in function" line, the message takes
		// the next line as well
		STATE_ASSERTION,

		// After a "<file> (<line>) : AssertMsg Failed in function" line, the message takes
		// the next two lines as well
		STATE_ASSERTMSG,
		STATE_ASSERTMSG_TEXT,

		// A failure message was found, the next ProcessText starts over
		STATE_ASSERTION_FOUND,
		STATE_FATAL_ERROR_FOUND,

		// The line holds no failure message, the rest of it is skipped
		STATE_SKIP_LINE,

		// The text was empty
		STATE_NO_TEXT,
	};

	CIdentifyFailureMessages() : unk000( false ), m_State( STATE_LINE_START ), m_nLine( 0 ) {}

	// Feeds nLength chars of pszText (up to the terminator when negative), returns the state
	// after it and the end of the processed text in ppszEnd
	DLL_CLASS_IMPORT State_t ProcessText( const char *pszText, int nLength, const char **ppszEnd );

	State_t GetState() const { return m_State; }
	bool HasFoundFailure() const { return m_State == STATE_ASSERTION_FOUND || m_State == STATE_FATAL_ERROR_FOUND; }

	// The failed assertion from "Assertion Failed" or "AssertMsg Failed" on with the lines it
	// takes, or the text after "FATAL ERROR: ", and the file and the line in front of an assertion
	const char *GetMessage() const { return m_Message.Get(); }
	const char *GetFile() const { return m_File.Get(); }
	int GetLine() const { return m_nLine; }

private:
	// The log prefix of the lines: "<digits>(...):  " when false, an 11 char "...s: " when true
	bool unk000;
	State_t m_State;
	CUtlString m_Message;
	CUtlString m_File;
	int m_nLine;
};

#endif // IDENTIFYFAILUREMESSAGES_H
