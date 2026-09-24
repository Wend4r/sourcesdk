#include "entityhandle.h"
#include "entityinstance.h"

bool CEntityHandle::operator <( const CEntityInstance *pEntity ) const
{
	uint32 otherIndex = pEntity ? pEntity->GetRefEHandle().m_Index : INVALID_EHANDLE_INDEX;

	return m_Index < otherIndex;
}

const CEntityHandle &CEntityHandle::Set( const CEntityInstance *pEntity )
{
	if ( pEntity )
	{
		*this = pEntity->GetRefEHandle();
	}
	else
	{
		m_Index = INVALID_EHANDLE_INDEX;
	}

	return *this;
}
