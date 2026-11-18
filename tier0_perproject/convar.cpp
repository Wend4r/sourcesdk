
//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basetypes.h"
#include "tier0/characterset.h"
#include "tier0/strtools.h"
#include "tier0/utlbuffer.h"
#include "tier0/convar.h"
#include "tier0/utlvector.h"
#include "tier1/tier1.h"
#include "tier0/icvar.h"
#include "tier0/dbg.h"
#include "color.h"
#if defined( _X360 )
#include "xbox/xbox_console.h"
#endif
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Tokenizer class
//-----------------------------------------------------------------------------
CCommand::CCommand()
{
	EnsureBuffers();
	Reset();
}

CCommand::CCommand( int nArgC, const char **ppArgV ) : CCommand()
{
	Assert( nArgC > 0 );

	char *pBuf = m_ArgvBuffer.Base();
	char *pSBuf = m_ArgSBuffer.Base();
	const char *pBufEnd = pBuf + m_ArgvBuffer.Count();
	const char *pSBufEnd = pSBuf + m_ArgSBuffer.Count();
	for ( int i = 0; i < nArgC; ++i )
	{
		int nLen = V_strlen( ppArgV[i] );
		bool bContainsSpace = strchr( ppArgV[i], ' ' ) != NULL;

		// Each argument is followed by a separator or the terminator in the ArgS buffer
		if ( nLen + 1 > pBufEnd - pBuf || nLen + ( bContainsSpace ? 2 : 0 ) + 1 > pSBufEnd - pSBuf )
		{
			Warning( "CCommand::CCommand: Encountered command which overflows the tokenizer buffer.. Skipping!\n" );
			Reset();
			return;
		}

		m_Args.AddToTail( pBuf );
		memcpy( pBuf, ppArgV[i], nLen+1 );
		pBuf += nLen+1;

		if ( bContainsSpace )
		{
			*pSBuf++ = '\"';
		}
		memcpy( pSBuf, ppArgV[i], nLen );
		pSBuf += nLen;
		if ( bContainsSpace )
		{
			*pSBuf++ = '\"';
		}

		if ( i != nArgC - 1 )
		{
			*pSBuf++ = ' ';
		}

		if ( i == 0 && nArgC > 1 )
		{
			m_nArgv0Size = pSBuf - m_ArgSBuffer.Base();
		}
	}

	*pSBuf = '\0';
}

void CCommand::EnsureBuffers()
{
	m_ArgSBuffer.SetSize( MaxCommandLength() );
	m_ArgvBuffer.SetSize( MaxCommandLength() );
}

void CCommand::Reset()
{
	m_nArgv0Size = 0;
	m_ArgSBuffer.Base()[0] = '\0';
	m_Args.RemoveAll();
}

characterset_t* CCommand::DefaultBreakSet()
{
	return g_pCVar->GetCharacterSet();
}

bool CCommand::Tokenize( const char *pCommand, const characterset_t *pBreakSet )
{
	if(m_ArgSBuffer.Count() == 0)
		EnsureBuffers();

	Reset();

	if ( !pCommand || !*pCommand )
		return false;

	// Use default break set
	if ( !pBreakSet )
	{
		pBreakSet = DefaultBreakSet();
	}

	// Copy the current command into a temp buffer
	// NOTE: This is here to avoid the pointers returned by DequeueNextCommand
	// to become invalid by calling AddText. Is there a way we can avoid the memcpy?
	int nLen = V_strlen( pCommand );
	if ( nLen >= m_ArgSBuffer.Count() - 1 )
	{
		Warning( "CCommand::Tokenize: Encountered command which overflows the tokenizer buffer.. Skipping!\n" );
		return false;
	}

	memmove( m_ArgSBuffer.Base(), pCommand, nLen + 1 );

	// Parse the current command into the current command buffer
	CUtlBuffer bufParse( m_ArgSBuffer.Base(), nLen, static_cast<CUtlBuffer::BufferFlags_t>(CUtlBuffer::TEXT_BUFFER | CUtlBuffer::READ_ONLY) );
	int nArgvBufferSize = 0;
	while ( bufParse.IsValid() )
	{
		char *pArgvBuf = &m_ArgvBuffer[nArgvBufferSize];
		int nMaxLen = m_ArgvBuffer.Count() - nArgvBufferSize;
		int nStartGet = bufParse.TellGet();
		int	nSize = bufParse.ParseToken( pBreakSet, pArgvBuf, nMaxLen, true );
		if ( nSize < 0 )
			break;

		// Check for overflow condition
		if ( nMaxLen == nSize )
		{
			Reset();
			return false;
		}

		if ( m_Args.Count() == 1 )
		{
			// Deal with the case where the arguments were quoted
			m_nArgv0Size = bufParse.TellGet();
			bool bFoundEndQuote = m_ArgSBuffer[m_nArgv0Size-1] == '\"';
			if ( bFoundEndQuote )
			{
				--m_nArgv0Size;
			}
			m_nArgv0Size -= nSize;
			Assert( m_nArgv0Size != 0 );

			// The StartGet check is to handle this case: "foo"bar
			// which will parse into 2 different args. ArgS should point to bar.
			bool bFoundStartQuote = ( m_nArgv0Size > nStartGet ) && ( m_ArgSBuffer[m_nArgv0Size-1] == '\"' );
			Assert( bFoundEndQuote == bFoundStartQuote );
			if ( bFoundStartQuote )
			{
				--m_nArgv0Size;
			}
		}

		m_Args.AddToTail( pArgvBuf );
		nArgvBufferSize += nSize + 1;

		if(nArgvBufferSize >= m_ArgvBuffer.Count())
			break;
	}

	return true;
}


//-----------------------------------------------------------------------------
// Helper function to parse arguments to commands.
//-----------------------------------------------------------------------------
int CCommand::FindArg( const char *pName ) const
{
	int nArgC = ArgC();
	for ( int i = 1; i < nArgC; i++ )
	{
		if ( !V_stricmp_fast( Arg(i), pName ) )
			return (i+1) < nArgC ? i+1 : -1;
	}
	return -1;
}

int CCommand::FindArgInt( const char *pName, int nDefaultVal ) const
{
	int idx = FindArg( pName );
	if ( idx != -1 )
		return V_atoi( m_Args[idx] );
	else
		return nDefaultVal;
}

void ConCommand::Create( const char* pName, const ConCommandCallbackInfo_t &cb, const char* pHelpString, uint64 flags, const CompletionCallbackInfo_t &completion_cb )
{
	// Name should be static data
	Assert(pName);

	ConCommandCreation_t info;
	info.m_pszName = pName;
	info.m_pszHelpString = pHelpString;
	info.m_nFlags = SanitiseConVarFlags( flags );
	info.m_CBInfo = cb;
	info.m_CompletionCBInfo = completion_cb;

	SetupConCommand( this, info );
}

void ConCommand::Destroy()
{
	DetroyConCommand( this );
}

//-----------------------------------------------------------------------------
//
// Console Variables
//
//-----------------------------------------------------------------------------

int ConVarData::GetMaxSplitScreenSlots() const
{
	if((m_nFlags & FCVAR_PER_USER) != 0)
		return static_cast<CCvar *>( g_pCVar )->GetMaxSplitScreenSlots();

	return 1;
}

CVValue_t *ConVarData::Value( CSplitScreenSlot slot ) const
{
	if(slot.Get() == -1)
		slot = CSplitScreenSlot( 0 );

	if(!IsSlotInRange( slot ))
		return nullptr;

	return (CVValue_t *)&m_Values[GetDataByteSize() * slot.Get()];
}

CVValue_t *ConVarData::ValueOrDefault( CSplitScreenSlot slot ) const
{
	CVValue_t *value = Value( slot );
	return value ? value : m_defaultValue;
}

bool ConVarData::IsAllSetToDefault() const
{
	for(int i = 0; i < GetMaxSplitScreenSlots(); i++)
	{
		if(!IsSetToDefault( i ))
			return false;
	}

	return true;
}

void ConVarData::MinValueToString( CBufferString &buf ) const
{
	if(HasMinValue())
		TypeTraits()->ValueToString( m_minValue, buf );
	else
		buf.Clear();
}

void ConVarData::MaxValueToString( CBufferString &buf ) const
{
	if(HasMaxValue())
		TypeTraits()->ValueToString( m_maxValue, buf );
	else
		buf.Clear();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ConVar_PrintDescription( const ConVarRefAbstract *ref )
{
	Assert( ref );

	static struct
	{
		uint64 m_Flag;
		const char *m_Name;
	} s_FlagsMap[] = {
		{ FCVAR_GAMEDLL, "game" },
		{ FCVAR_CLIENTDLL, "client" },
		{ FCVAR_ARCHIVE, "archive" },
		{ FCVAR_NOTIFY, "notify" },
		{ FCVAR_SPONLY, "singleplayer" },
		{ FCVAR_NOT_CONNECTED, "notconnected" },
		{ FCVAR_CHEAT, "cheat" },
		{ FCVAR_REPLICATED, "replicated" },
		{ FCVAR_SERVER_CAN_EXECUTE, "server_can_execute" },
		{ FCVAR_CLIENTCMD_CAN_EXECUTE, "clientcmd_can_execute" },
		{ FCVAR_USERINFO, "userinfo" },
		{ FCVAR_PER_USER, "per_user" }
	};

	CBufferStringN<4096> desc;
	CBufferString buf;

	ref->GetValueAsString( buf );
	desc.AppendFormat( "\"%s\" = \"%s\"", ref->GetName(), buf.Get() );

	if(!ref->IsSetToDefault())
	{
		ref->GetDefaultAsString( buf );
		desc.AppendFormat( " ( def. \"%s\" )", buf.Get() );
	}

	if(ref->HasMin())
	{
		ref->GetMinAsString( buf );
		desc.AppendFormat( " min. %s", buf.Get() );
	}

	if(ref->HasMax())
	{
		ref->GetMaxAsString( buf );
		desc.AppendFormat( " max. %s", buf.Get() );
	}

	for(size_t i = 0; i < (sizeof( s_FlagsMap ) / sizeof( s_FlagsMap[0] )); i++)
	{
		if(ref->IsFlagSet( s_FlagsMap[i].m_Flag ))
			desc.AppendFormat( " %s", s_FlagsMap[i].m_Name );
	}

	if(ref->HasHelpText())
		ConMsg( "%-120s - %s\n", desc.Get(), ref->GetHelpText() );
	else
		ConMsg( "%-120s\n", desc.Get() );
}
