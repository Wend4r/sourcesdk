//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The round end fun fact evaluators.
//
//=============================================================================//

#ifndef CSTRIKE15_FUNFACT_CS_H
#define CSTRIKE15_FUNFACT_CS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"

struct FunFact;

//-----------------------------------------------------------------------------
// FunFactEvaluator, the base of the round end fun fact evaluators (CFunFact_GenericEvalFunction,
// CFunFact_PlayerEvalFunction, CFunFact_StatBest, CFunFact_StatSum override only). 32 bytes; on
// Linux the members of a derived evaluator start in its tail padding, at 28.
//-----------------------------------------------------------------------------
abstract_class FunFactEvaluator
{
public:
	virtual ~FunFactEvaluator() = 0;

	int GetId() const { return m_id; }
	const char *GetLocalizationToken() const { return m_pLocalizationToken; }
	float GetCoolness() const { return m_fCoolness; }

	// Adds the fun facts of the round end reason that apply to results.
	virtual bool Evaluate( int iRoundResult, CUtlVector< FunFact > &results ) const = 0;

public:
	// The id of the fun fact.
	int m_id;
	// The localization token of the fun fact ("#funfact_..."), a string literal.
	const char *m_pLocalizationToken;
	// How interesting the fun fact is.
	float m_fCoolness;
};

#endif // CSTRIKE15_FUNFACT_CS_H
