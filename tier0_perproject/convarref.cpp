
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

ConCommandRef::ConCommandRef( const char *name, bool allow_developer )
{
	*this = g_pCVar->FindConCommand( name, allow_developer );
}

ConCommandData *ConCommandRef::GetRawData()
{
	return g_pCVar->GetConCommandData( *this );
}

void ConCommandRef::Dispatch( const CCommandContext &context, const CCommand &command )
{
	g_pCVar->DispatchConCommand( *this, context, command );
}

ConVarRef::ConVarRef( const char *name, bool allow_developer )
 :  m_ConVar{ kInvalidAccessIndex, 0 }
{
	if ( g_pCVar )
		*this = g_pCVar->FindConVar( name, allow_developer );
}

void ConVarRefAbstract::Init( ConVarRef ref, EConVarType type )
{
	m_ConVarData = nullptr;

	if ( g_pCVar )
		m_ConVarData = g_pCVar->GetConVarData( ref );
	
	if( !m_ConVarData )
		InvalidateConVarData( type );
}

void ConVarRefAbstract::InvalidateConVarData( EConVarType type )
{
	InvalidateRef();

	if(type == EConVarType_Invalid)
		m_ConVarData = GetInvalidConVarData( EConVarType_Invalid );
	else
		m_ConVarData = GetCvarTypeTraits( type )->m_InvalidCvarData;
}

void ConVarRefAbstract::CallChangeCallbacks( CSplitScreenSlot slot, CVValue_t *new_value, CVValue_t *prev_value, const char *new_str, const char *prev_str )
{
	if(slot.Get() == -1)
		slot = CSplitScreenSlot( 0 );

	if( g_pCVar )
	{
		g_pCVar->CallChangeCallback( *this, slot, new_value, prev_value );
		g_pCVar->CallGlobalChangeCallbacks( this, slot, new_str, prev_str );
	}
}

void ConVarRefAbstract::SetOrQueueValueInternal( CSplitScreenSlot slot, CVValue_t *value )
{
	if(m_ConVarData->IsFlagSet( FCVAR_PERFORMING_CALLBACKS ))
		QueueSetValueInternal( slot, value );
	else
		SetValueInternal( slot, value );
}

void ConVarRefAbstract::QueueSetValueInternal( CSplitScreenSlot slot, CVValue_t *value )
{
	if(slot.Get() == -1)
		slot = CSplitScreenSlot( 0 );

	TypeTraits()->Clamp( value, m_ConVarData->MinValue(), m_ConVarData->MaxValue() );

	if(!m_ConVarData->IsEqual( slot, value ))
		g_pCVar->QueueThreadSetValue( this, slot, nullptr, value );
}

void ConVarRefAbstract::SetValueInternal( CSplitScreenSlot slot, CVValue_t *value )
{
	CVValue_t *curr_value = m_ConVarData->ValueOrDefault( slot );

	CVValue_t prev;
	TypeTraits()->Construct( &prev );
	TypeTraits()->Copy( &prev, *curr_value );

	if(g_pCVar->CallFilterCallback( *this, slot, value, &prev ))
	{
		TypeTraits()->Destruct( curr_value );

		TypeTraits()->Construct( curr_value );
		TypeTraits()->Copy( curr_value, *value );
		m_ConVarData->Clamp( slot );

		if(!m_ConVarData->IsEqual( slot, &prev ))
		{
			CBufferString prev_str, new_str;

			TypeTraits()->ValueToString( &prev, prev_str );
			TypeTraits()->ValueToString( curr_value, new_str );

			m_ConVarData->IncrementTimesChanged();

			CallChangeCallbacks( slot, curr_value, &prev, new_str.Get(), prev_str.Get() );
		}
	}

	TypeTraits()->Destruct( &prev );
}

bool ConVarRefAbstract::SetString( CUtlString string, CSplitScreenSlot slot )
{
	CVValue_t *value = m_ConVarData->Value( slot );

	if(!value)
		return true;
	
	if(GetType() != EConVarType_String)
		string.Trim( "\t\n\v\f\r " );

	CVValue_t new_value;
	TypeTraits()->Construct( &new_value );

	bool success = false;
	if(TypeTraits()->StringToValue( string.Get(), &new_value ))
	{
		SetOrQueueValueInternal( slot, &new_value );
		success = true;
	}

	TypeTraits()->Destruct( &new_value );
	return success;
}

void ConVarRefAbstract::Revert( CSplitScreenSlot slot )
{
	CBufferString buf;
	GetDefaultAsString( buf );
	SetString( buf.Get(), slot );
}
