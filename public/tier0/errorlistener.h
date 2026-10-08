#ifndef TIER0_ERRORLISTENER_H
#define TIER0_ERRORLISTENER_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "bufferstring.h"
#include "utlstring.h"

//-----------------------------------------------------------------------------
// The severity of an ErrorListenerMessage_t. A listener that logs writes a message as LS_MESSAGE and
// the others as LS_WARNING, counting warnings and errors apart.
//-----------------------------------------------------------------------------
enum ErrorListenerSeverity_t
{
	ERROR_LISTENER_SEVERITY_MESSAGE = 0,
	ERROR_LISTENER_SEVERITY_WARNING = 1,
	ERROR_LISTENER_SEVERITY_ERROR = 2,
};

//-----------------------------------------------------------------------------
// Data a message carries about what it refers to; a listener finds the type through dynamic_cast.
// Only the virtual destructor is known.
//-----------------------------------------------------------------------------
class IBaseErrorContext
{
public:
	virtual ~IBaseErrorContext() {}
};

template < typename T >
class CErrorContextValue : public IBaseErrorContext
{
public:
	explicit CErrorContextValue( const T &value ) : m_Value( value ) {}

	T m_Value;
};

//-----------------------------------------------------------------------------
// A message reported to an IErrorListener. It owns m_pContext.
//-----------------------------------------------------------------------------
struct ErrorListenerMessage_t
{
	ErrorListenerMessage_t() : m_nSeverity( ERROR_LISTENER_SEVERITY_MESSAGE ), m_nLine( 0 ), m_nColumn( 0 ), m_pContext( nullptr ) {}
	~ErrorListenerMessage_t() { delete m_pContext; }

	ErrorListenerMessage_t( const ErrorListenerMessage_t & ) = delete;
	ErrorListenerMessage_t &operator=( const ErrorListenerMessage_t & ) = delete;

	CBufferString m_sMessage;

	// ErrorListenerSeverity_t.
	int m_nSeverity;

	// Prefixes the message; IErrorListener::GetSourceName supplies it when empty.
	CUtlString m_sLocation;
	int m_nLine;
	int m_nColumn;

	IBaseErrorContext *m_pContext;
};

//-----------------------------------------------------------------------------
// Receives the messages of a parse or a transfer. The string parsing functions report to one, as do
// the KV3 transfer contexts, which are error listeners themselves. There is no virtual destructor.
//-----------------------------------------------------------------------------
class IErrorListener
{
public:
	virtual void ReportMessage( const ErrorListenerMessage_t &message ) = 0;

	// Appends "<location>[(<line>[,<column>])]|<context path>", dropping the "|" without a context path,
	// then pszSuffix when anything was appended. Returns whether anything was appended.
	virtual bool FormatMessageLocation( const ErrorListenerMessage_t &message, CBufferString &sOut, const char *pszSuffix ) = 0;

	// The location of a message without one. pLineAndColumn holds the line and the column of the message
	// and may be updated.
	virtual const char *GetSourceName( int *pLineAndColumn ) = 0;

	// Appends the context path the messages are reported in. Returns false when there is none.
	virtual bool AppendContextPath( CBufferString &sOut ) = 0;

	// Appends a formatted entry to the context path and returns the length to restore with PopContext;
	// -1 for a listener that keeps no context path.
	virtual int PushContext( PRINTF_FORMAT_STRING const char *pszFormat, ... ) FMTFUNCTION( 2, 3 ) = 0;

	// Truncates the context path to what PushContext returned; a negative length does nothing.
	virtual void PopContext( int nContextLength ) = 0;
};

#endif // TIER0_ERRORLISTENER_H
