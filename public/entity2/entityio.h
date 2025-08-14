#ifndef ENTITYIO_H
#define ENTITYIO_H

#pragma once

#include "datamap.h"
#include "string_t.h"
#include "variant.h"
#include "schemasystem/schematypes.h"
#include "entityhandle.h"
#include "entitysystem.h"

struct EntityIOConnectionDesc_t
{
	string_t m_targetDesc;
	string_t m_targetInput;
	string_t m_valueOverride;
	CEntityHandle m_hTarget;
	EntityIOTargetType_t m_nTargetType;
	int32 m_nTimesToFire;
	float m_flDelay;
};

struct EntityIOConnection_t : EntityIOConnectionDesc_t
{
	bool m_bMarkedForRemoval;
	EntityIOConnection_t* m_pNext;
};

struct EntityIOOutputDesc_t
{
	const char* m_pName;
	uint32 m_nFlags;
	uint32 m_nOutputOffset;
};

class CEntityIOOutput
{
public:
	virtual SchemaMetaInfoHandle_t<CSchemaClassInfo> Schema_DynamicBinding() = 0;
	virtual fieldtype_t ValueFieldType() const { return m_Value.FieldType(); }

public:
	const EntityIOConnection_t *GetConnections() const { return m_pConnections; }
	const EntityIOOutputDesc_t *GetDescription() const { return m_pDesc; }

private:
	EntityIOConnection_t *m_pConnections;
	EntityIOOutputDesc_t *m_pDesc;

protected:
	CVariant m_Value;
};

template < typename T >
class CEntityOutputTemplate : public CEntityIOOutput
{
public:
	T Get() const { return this->m_Value.template Get< T >(); }
};

struct InputData_t
{
	CBaseEntity *pActivator;
	CBaseEntity *pCaller;
	variant_t value;
	int nOutputID;
};

#endif // ENTITYIO_H
