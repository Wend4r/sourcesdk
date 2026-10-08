#ifndef SCHEMAREGISTRATION_H
#define SCHEMAREGISTRATION_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CBufferString;
class ISchemaSystem;

enum SchemaRegistrationPhase_t
{
	SCHEMA_REGISTRATION_PHASE_TYPE_TABLES = 0,
	SCHEMA_REGISTRATION_PHASE_ATOMIC_TYPES_AND_ENUMS,
	SCHEMA_REGISTRATION_PHASE_CLASSES,
	SCHEMA_REGISTRATION_PHASE_VALIDATION,

	SCHEMA_REGISTRATION_PHASE_COUNT
};

//-----------------------------------------------------------------------------
// Per-module list of schema binding registrars, walked by InstallSchemaBindings()
// Each phase is retried until every registrar succeeds; the failure reason is only
// requested on the last attempt
//-----------------------------------------------------------------------------
class CSchemaRegistration
{
public:
	CSchemaRegistration() :
		m_pNext( s_pSchemaRegistrationList )
	{
		s_pSchemaRegistrationList = this;
	}

	virtual bool RegisterAllBindings( ISchemaSystem *pSchemaSystem, SchemaRegistrationPhase_t ePhase, CBufferString *pFailureReason ) = 0;

	static void RegisterAllModuleData( ISchemaSystem *pSchemaSystem );

	static CSchemaRegistration *s_pSchemaRegistrationList;

	CSchemaRegistration *m_pNext;
};

#endif // SCHEMAREGISTRATION_H
