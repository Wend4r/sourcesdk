#include "const.h"
#include "entity2/entitysystem.h"
#include "entity2/entityclass.h"

CEntityIdentity* CEntitySystem::GetEntityIdentity(CEntityIndex entnum)
{
	if (entnum.Get() <= -1 || entnum.Get() >= (MAX_TOTAL_ENTITIES - 1))
		return nullptr;

	CEntityIdentity* pChunkToUse = m_EntityList.m_pIdentityChunks[entnum.Get() / MAX_ENTITIES_IN_LIST];
	if (!pChunkToUse)
		return nullptr;

	CEntityIdentity* pIdentity = &pChunkToUse[entnum.Get() % MAX_ENTITIES_IN_LIST];
	if (!pIdentity)
		return nullptr;

	if (pIdentity->GetEntityIndex() != entnum)
		return nullptr;

	return pIdentity;
}

CEntityIdentity* CEntitySystem::GetEntityIdentity(const CEntityHandle& hEnt)
{
	if (!hEnt.IsValid())
		return nullptr;

	CEntityIdentity* pChunkToUse = m_EntityList.m_pIdentityChunks[hEnt.GetEntryIndex() / MAX_ENTITIES_IN_LIST];
	if (!pChunkToUse)
		return nullptr;

	CEntityIdentity* pIdentity = &pChunkToUse[hEnt.GetEntryIndex() % MAX_ENTITIES_IN_LIST];
	if (!pIdentity)
		return nullptr;

	if (pIdentity->GetRefEHandle() != hEnt)
		return nullptr;

	return pIdentity;
}

CEntityClass* CEntitySystem::FindClassByName(const char* szClassName)
{
	if (!szClassName || !*szClassName)
		return nullptr;

	unsigned short idx = m_entClassesByCPPClassname.Find(szClassName);
	if (idx == m_entClassesByCPPClassname.InvalidIndex())
	{
		// vscript stuff is skipped here
		return nullptr;
	}

	return m_entClassesByCPPClassname[ idx ];
}

CEntityClass* CEntitySystem::FindClassByDesignName(const char* szClassName)
{
	if (!szClassName || !*szClassName)
		return nullptr;

	unsigned short idx = m_entClassesByClassname.Find(szClassName);
	if (idx == m_entClassesByClassname.InvalidIndex())
	{
		// vscript stuff is skipped here
		return nullptr;
	}

	return m_entClassesByClassname[ idx ];
}

CEntityHandle CEntitySystem::FindFirstEntityHandleByName(const char* szName, WorldGroupId_t hWorldGroupId)
{
	if (!szName || !*szName)
		return CEntityHandle();

	EntityInstanceByNameIter_t iter(szName);
	iter.SetWorldGroupId(hWorldGroupId);

	CEntityInstance* pEntity = iter.First();
	if (!pEntity)
		return CEntityHandle();

	return pEntity->GetRefEHandle();
}

CUtlSymbolLarge CEntitySystem::AllocPooledString(const char* pString)
{
	if (!pString || !*pString)
		return CUtlSymbolLarge();

	return m_Symbols.AddString(pString);
}

CUtlSymbolLarge CEntitySystem::FindPooledString(const char* pString)
{
	if (!pString || !*pString)
		return CUtlSymbolLarge();

	return m_Symbols.FindString(pString);
}

void CEntitySystem::AddEntityName( CEntityIdentity *pIdentity )
{
	if ( !pIdentity->m_name.IsValid() )
		return;

	CEntityHandle hEntity = pIdentity->GetRefEHandle();
	auto iName = m_entityNames.Find( pIdentity->m_name );

	if ( iName != m_entityNames.InvalidIndex() )
	{
		EntityNameHandles_t *pHandles = m_entityNames[ iName ];

		if ( pHandles->Find( hEntity ) == pHandles->InvalidIndex() )
			pHandles->AddToTail( hEntity );

		return;
	}

	EntityNameHandles_t *pHandles = Create< EntityNameHandles_t >();

	pHandles->AddRef();
	pHandles->AddToTail( hEntity );

	m_entityNames.Insert( pIdentity->m_name, pHandles );
}

void CEntitySystem::RemoveEntityName( CEntityIdentity *pIdentity )
{
	if ( !pIdentity->m_name.IsValid() )
		return;

	auto iName = m_entityNames.Find( pIdentity->m_name );

	if ( iName == m_entityNames.InvalidIndex() )
		return;

	EntityNameHandles_t *pHandles = m_entityNames[ iName ];

	if ( pHandles->Count() > 0 )
	{
		int iHandle = pHandles->Find( pIdentity->GetRefEHandle() );

		if ( iHandle == pHandles->InvalidIndex() )
			return;

		pHandles->FastRemove( iHandle );

		if ( pHandles->Count() > 0 )
			return;
	}

	m_entityNames.RemoveAt( iName );

	pHandles->Release();
}

void CEntitySystem::QueueDestroyEntity( CEntityInstance *pEntity )
{
	if ( pEntity )
		QueueDestroyEntity( pEntity->m_pEntity );
}

void CEntitySystem::QueueDestroyEntity( CEntityIdentity *pIdentity )
{
	if ( !pIdentity )
		return;

	if ( !pIdentity->m_pInstance )
	{
		pIdentity->ReleaseAttributes();
		m_EntityList.FreeIdentity( pIdentity, true );

		return;
	}

	if ( pIdentity->m_flags & EF_IS_ISOLATED_ALLOCATION )
	{
		DestroyEntityImmediate( pIdentity );

		return;
	}

	if ( pIdentity->m_flags & EF_MARKED_FOR_DELETE )
		return;

	pIdentity->m_flags |= EF_MARKED_FOR_DELETE;
	m_queuedDeletions.AddToTail( { pIdentity } );

	AutoExecuteQueuedDeletion();
}

void CEntitySystem::DestroyEntityImmediate( CEntityIdentity *pIdentity )
{
	if ( !pIdentity )
		return;

	if ( pIdentity->m_flags & EF_IS_ISOLATED_ALLOCATION )
	{
		DestroyEntityInstance( pIdentity->m_pInstance );
		pIdentity->ReleaseAttributes();
		m_EntityList.FreeIdentity( pIdentity, true );

		return;
	}

	if ( pIdentity->m_flags & EF_MARKED_FOR_DELETE )
		return;

	if ( m_nSuppressDestroyImmediateCount > 0 )
	{
		QueueDestroyEntity( pIdentity );

		return;
	}

	++m_nSuppressAutoDeletionExecutionCount;

	CEntityInstance *pInstance = pIdentity->m_pInstance;

	pIdentity->m_flags |= EF_DELETE_IN_PROGRESS;

	if ( pInstance )
	{
		if ( !( pIdentity->m_flags & EF_IN_STAGING_LIST ) )
		{
			EntityDeletion_t deletion;

			deletion.m_pEntity = pIdentity;
			UpdateOnRemove( 1, &deletion );
		}

		RemoveFromEntityDatabase( pIdentity );
	}

	DestroyEntityInstance( pInstance );
	pIdentity->ReleaseAttributes();
	m_EntityList.FreeIdentity( pIdentity, true );

	--m_nSuppressAutoDeletionExecutionCount;

	AutoExecuteQueuedDeletion();
}

void CEntitySystem::ExecuteQueuedDeletion( bool bDeallocateImmediately )
{
	m_nExecuteQueuedDeletionDepth = 1;

	if ( bDeallocateImmediately )
		ExecuteQueuedDeallocation();

	CUtlVectorFixedGrowable< EntityDeletion_t, 256 > deletions;

	// Entities queued by UpdateOnRemove are handled in the next pass
	for ( int iFirst = 0, iLast = m_queuedDeletions.Count(); iFirst < iLast; iFirst = iLast, iLast = m_queuedDeletions.Count() )
	{
		deletions.RemoveAll();
		deletions.EnsureCapacity( iLast - iFirst );

		for ( int i = iFirst; i < iLast; ++i )
		{
			CEntityIdentity *pIdentity = m_queuedDeletions[ i ].m_pEnt;

			pIdentity->m_flags |= EF_DELETE_IN_PROGRESS;

			if ( pIdentity->m_pInstance && !( pIdentity->m_flags & EF_IN_STAGING_LIST ) )
			{
				EntityDeletion_t deletion;

				deletion.m_pEntity = pIdentity;
				deletions.AddToTail( deletion );
			}
		}

		if ( deletions.Count() )
			UpdateOnRemove( deletions.Count(), deletions.Base() );

		for ( int i = iFirst; i < iLast; ++i )
			RemoveFromEntityDatabase( m_queuedDeletions[ i ].m_pEnt );

		m_queuedDeallocations.AddMultipleToTail( iLast - iFirst, m_queuedDeletions.Base() + iFirst );

		if ( bDeallocateImmediately )
			ExecuteQueuedDeallocation();
	}

	m_queuedDeletions.RemoveAll();

	--m_nExecuteQueuedDeletionDepth;
}

void CEntitySystem::ExecuteQueuedDeallocation()
{
	for ( int i = 0, nCount = m_queuedDeallocations.Count(); i < nCount; ++i )
	{
		CEntityIdentity *pIdentity = m_queuedDeallocations[ i ].m_pEnt;

		DestroyEntityInstance( pIdentity->m_pInstance );
		pIdentity->ReleaseAttributes();
		m_EntityList.FreeIdentity( pIdentity, true );
	}

	m_queuedDeallocations.RemoveAll();
}

void CEntitySystem::AutoExecuteQueuedDeletion()
{
	if ( m_nSuppressAutoDeletionExecutionCount || !m_bEnableAutoDeletionExecution || m_nExecuteQueuedDeletionDepth )
		return;

	if ( m_nExecuteQueuedCreationDepth )
		m_bQueuedDeletionDeferred = true;
	else
		ExecuteQueuedDeletion( false );
}

void CEntitySystem::RemoveFromEntityDatabase( CEntityIdentity *pIdentity )
{
	if ( pIdentity->m_flags & ( EF_IS_ISOLATED_ALLOCATION | EF_IS_ANONYMOUS_ALLOCATION ) )
		return;

	OnRemoveEntity( pIdentity->m_pInstance, pIdentity->GetRefEHandle() );
	RemoveEntityName( pIdentity );
	pIdentity->m_pClass->UnlinkFromClassList( pIdentity );
}

void CEntitySystem::DestroyEntityInstance( CEntityInstance *pInstance )
{
	CEntityIdentity *pIdentity = pInstance->m_pEntity;
	CEntityClass *pClass = pIdentity->m_pClass;

	pIdentity->m_pInstance->Disconnect();

	if ( pClass->m_nAllHelpersFlags & ENTITY_COMPONENT_HELPER_FREE )
	{
		for ( int i = pClass->m_AllHelpers.Count() - 1; i >= 0; --i )
		{
			const CEntityClass::ComponentHelper_t &helper = pClass->m_AllHelpers[ i ];

			if ( helper.m_pComponentHelper->m_flags & ENTITY_COMPONENT_HELPER_FREE )
				helper.m_pComponentHelper->Free( pIdentity, reinterpret_cast< byte * >( pInstance ) + helper.m_nOffset );
		}
	}

	pClass->DestructInstance( pInstance );
	pClass->FreeInstance( pInstance );

	pIdentity->m_pInstance = nullptr;
}

EntityIOQueuePrioritizedEvent_t::EntityIOQueuePrioritizedEvent_t( WorldGroupId_t worldGroupId, GameTime_t flFireTime, CEntityInstance *pActivator, CEntityInstance *pCaller ) :
	m_WorldGroupId( worldGroupId ),
	m_flFireTime( flFireTime ),
	m_hActivator( pActivator && pActivator->m_pEntity ? pActivator->m_pEntity->GetRefEHandle() : CEntityHandle() ),
	m_hCaller( pCaller && pCaller->m_pEntity ? pCaller->m_pEntity->GetRefEHandle() : CEntityHandle() )
{
}

void EntityIOQueuePrioritizedEvent_t::Init( const CVariant &value, const CPulseArgumentPack *pArgs, const CPulseInputParamMap *pParamMap )
{
	if ( pArgs )
	{
		if ( value.m_type != FIELD_VOID )
			m_variantValue.CopyFrom( value );
		else
			pArgs->GetParamAsVariant( m_variantValue );

		m_PulseArguments = *pArgs;
	}
	else
	{
		m_variantValue.CopyFrom( value );
		m_PulseArguments.SetParamFromVariant( value );
	}

	if ( pParamMap )
	{
		m_paramMap.m_KV3 = pParamMap->m_KV3;
		m_paramMap.m_bForwardAllArgs = pParamMap->m_bForwardAllArgs;
	}
}

void CEventQueue::AddEvent( EntityIOQueuePrioritizedEvent_t *pEvent )
{
	m_Mutex.Lock();

	// Events with the same fire time keep the order they were added in
	EntityIOQueuePrioritizedEvent_t *pPrev = &m_Events;
	EntityIOQueuePrioritizedEvent_t *pNext = m_Events.m_pNext;

	while ( pNext && !( pNext->m_flFireTime > pEvent->m_flFireTime ) )
	{
		pPrev = pNext;
		pNext = pNext->m_pNext;
	}

	pEvent->m_pNext = pNext;
	pEvent->m_pPrev = pPrev;
	pPrev->m_pNext = pEvent;

	pEvent->m_variantValue.ConvertToCopiedData( true );

	if ( pEvent->m_pNext )
		pEvent->m_pNext->m_pPrev = pEvent;

	m_Mutex.Unlock();
}

void CEventQueue::RemoveEvent( EntityIOQueuePrioritizedEvent_t *pEvent )
{
	m_Mutex.Lock();

	pEvent->m_pPrev->m_pNext = pEvent->m_pNext;

	if ( pEvent->m_pNext )
		pEvent->m_pNext->m_pPrev = pEvent->m_pPrev;

	m_Mutex.Unlock();
}

EntityIOQueuePrioritizedEvent_t *CEntitySystem::CreateEvent( CEntityInstance *pActivator, CEntityInstance *pCaller, float flDelay, const CVariant &value, const CPulseArgumentPack *pArgs, const CPulseInputParamMap *pParamMap )
{
	const WorldGroupId_t worldGroupId = pCaller ? pCaller->m_pEntity->m_worldGroupId : WorldGroupId_t();
	const float flCurTime = GetCurTime( worldGroupId );

	EntityIOQueuePrioritizedEvent_t *pEvent = Create< EntityIOQueuePrioritizedEvent_t >( worldGroupId, GameTime_t( flDelay + flCurTime ), pActivator, pCaller );

	pEvent->Init( value, pArgs, pParamMap );

	return pEvent;
}

void CEntitySystem::AddEvent( EntityIOTargetType_t targetType, CUtlSymbolLarge sTarget, CUtlSymbolLarge sInput, CEntityInstance *pActivator, CEntityInstance *pCaller, float flDelay, const CVariant &value, const CPulseArgumentPack *pArgs, const CPulseInputParamMap *pParamMap )
{
	EntityIOQueuePrioritizedEvent_t *pEvent = CreateEvent( pActivator, pCaller, flDelay, value, pArgs, pParamMap );

	pEvent->m_targetType = targetType;
	pEvent->m_pTarget = sTarget;
	pEvent->m_pTargetInput = sInput;

	m_EventQueue.AddEvent( pEvent );
}

void CEntitySystem::AddEvent( CEntityHandle hTarget, CUtlSymbolLarge sInput, CEntityInstance *pActivator, CEntityInstance *pCaller, const CVariant &value, float flDelay, const CPulseArgumentPack *pArgs, const CPulseInputParamMap *pParamMap )
{
	EntityIOQueuePrioritizedEvent_t *pEvent = CreateEvent( pActivator, pCaller, flDelay, value, pArgs, pParamMap );

	pEvent->m_targetType = ENTITY_IO_TARGET_EHANDLE;
	pEvent->m_pTargetInput = sInput;
	pEvent->m_hEntTarget = hTarget;

	m_EventQueue.AddEvent( pEvent );
}

void CGameEntitySystem::AddListenerEntity(IEntityListener* pListener)
{
	if (m_entityListeners.Find(pListener) == -1)
	{
		m_entityListeners.AddToTail(pListener);
	}
}

void CGameEntitySystem::RemoveListenerEntity(IEntityListener* pListener)
{
	m_entityListeners.FindAndRemove(pListener);
}

CEntityInstance* CEntityHandle::Get() const
{
	return GameEntitySystem()->GetEntityInstance( *this );
}

CEntityHandle CEntityHandle::FromPackedInt( int packed_int_handle )
{
	if(packed_int_handle == 0xFFFFFF)
		return CEntityHandle();

	CEntityIndex index = packed_int_handle & 0x3FFF;
	auto serial = (packed_int_handle >> 14) & 0x3FF;

	auto entity = GameEntitySystem()->GetEntityInstance( index );
	if(!entity)
		return CEntityHandle();

	auto ent_handle = entity->GetRefEHandle();

	// Since we don't have the full serial part, validate the one we have
	if((ent_handle.GetSerialNumber() & 0x3FF) != serial)
		return CEntityHandle();

	return ent_handle;
}

EntityInstanceIter_t::EntityInstanceIter_t(IEntityFindFilter* pFilter, EntityIterType_t eIterType)
{
	m_pCurrentEnt = nullptr;
	m_pFilter = pFilter;
	m_eIterType = eIterType;
	m_hWorldGroupId = WorldGroupId_t();
}

CEntityInstance* EntityInstanceIter_t::First()
{
	m_pCurrentEnt = nullptr;
	return Next();
}

CEntityInstance* EntityInstanceIter_t::Next()
{
	if (m_pCurrentEnt)
		m_pCurrentEnt = m_pCurrentEnt->m_pNext;
	else
		m_pCurrentEnt = (m_eIterType == ENTITY_ITER_OVER_ACTIVE) ? GameEntitySystem()->m_EntityList.m_pFirstActiveEntity : GameEntitySystem()->m_EntityList.m_dormantList.m_pHead;

	for (; m_pCurrentEnt != nullptr; m_pCurrentEnt = m_pCurrentEnt->m_pNext)
	{
		if ((m_pCurrentEnt->m_flags & EF_MARKED_FOR_DELETE) != 0)
			continue;

		if (m_pFilter && !m_pFilter->ShouldFindEntity(m_pCurrentEnt->m_pInstance))
			continue;

		if (m_hWorldGroupId != WorldGroupId_t() && m_hWorldGroupId != m_pCurrentEnt->m_worldGroupId)
			continue;

		break;
	}

	if (m_pCurrentEnt)
		return m_pCurrentEnt->m_pInstance;

	return nullptr;
}

EntityInstanceByNameIter_t::EntityInstanceByNameIter_t(const char* szName, CEntityInstance* pSearchingEntity, CEntityInstance* pActivator, CEntityInstance* pCaller, IEntityFindFilter* pFilter, EntityIterType_t eIterType)
{
	m_pCurrentEnt = nullptr;
	m_pFilter = pFilter;
	m_eIterType = eIterType;
	m_hWorldGroupId = WorldGroupId_t();

	if (szName[0] == '!')
	{
		m_pszEntityName = nullptr;
		m_pEntityHandles = nullptr;
		m_nCurEntHandle = 0;
		m_nNumEntHandles = 0;
		m_pProceduralEnt = GameEntitySystem()->FindEntityProcedural(szName, pSearchingEntity, pActivator, pCaller);
	}
	else if (strchr(szName, '*') || eIterType == ENTITY_ITER_OVER_DORMANT)
	{
		m_pszEntityName = szName;
		m_pEntityHandles = nullptr;
		m_nCurEntHandle = 0;
		m_nNumEntHandles = 0;
		m_pProceduralEnt = nullptr;
	}
	else
	{
		m_pszEntityName = nullptr;
		m_pProceduralEnt = nullptr;

		CUtlSymbolLarge nameSymbol = GameEntitySystem()->FindPooledString(szName);

		unsigned short idx = GameEntitySystem()->m_entityNames.Find(nameSymbol);
		if (idx == GameEntitySystem()->m_entityNames.InvalidIndex())
		{
			m_pEntityHandles = nullptr;
			m_nCurEntHandle = 0;
			m_nNumEntHandles = 0;
		}
		else
		{
			m_pEntityHandles = GameEntitySystem()->m_entityNames[ idx ];
			m_nCurEntHandle = 0;

			if (m_pEntityHandles)
			{
				m_nNumEntHandles = m_pEntityHandles->Count();

				if (!m_nNumEntHandles)
					m_pEntityHandles = nullptr;
			}
			else
			{
				m_nNumEntHandles = 0;
			}
		}
	}
}

CEntityInstance* EntityInstanceByNameIter_t::First()
{
	m_pCurrentEnt = nullptr;
	return Next();
}

CEntityInstance* EntityInstanceByNameIter_t::Next()
{
	if (m_pProceduralEnt)
	{
		if (m_pCurrentEnt)
		{
			m_pCurrentEnt = nullptr;
		}
		else
		{
			if (!m_pFilter || m_pFilter->ShouldFindEntity(m_pProceduralEnt))
				m_pCurrentEnt = m_pProceduralEnt->m_pEntity;
		}
	}
	else if (m_pEntityHandles)
	{
		if ( !m_pCurrentEnt )
			m_nCurEntHandle = m_nNumEntHandles;

		for (--m_nCurEntHandle; m_nCurEntHandle >= 0; --m_nCurEntHandle)
		{
			m_pCurrentEnt = GameEntitySystem()->GetEntityIdentity(m_pEntityHandles->Element(m_nCurEntHandle));

			if ((m_pCurrentEnt->m_flags & EF_MARKED_FOR_DELETE) != 0)
				continue;

			if (m_pFilter && !m_pFilter->ShouldFindEntity(m_pCurrentEnt->m_pInstance))
				continue;

			if (m_hWorldGroupId != WorldGroupId_t() && m_hWorldGroupId != m_pCurrentEnt->m_worldGroupId)
				continue;

			break;
		}

		if (m_nCurEntHandle < 0)
			m_pCurrentEnt = nullptr;
	}
	else if (m_pszEntityName)
	{
		if (m_pCurrentEnt)
			m_pCurrentEnt = m_pCurrentEnt->m_pNext;
		else
			m_pCurrentEnt = (m_eIterType == ENTITY_ITER_OVER_ACTIVE) ? GameEntitySystem()->m_EntityList.m_pFirstActiveEntity : GameEntitySystem()->m_EntityList.m_dormantList.m_pHead;

		for (; m_pCurrentEnt != nullptr; m_pCurrentEnt = m_pCurrentEnt->m_pNext)
		{
			if ((m_pCurrentEnt->m_flags & EF_MARKED_FOR_DELETE) != 0)
				continue;

			if (!m_pCurrentEnt->m_name.IsValid())
				continue;

			if (!m_pCurrentEnt->NameMatches(m_pszEntityName))
				continue;

			if (m_pFilter && !m_pFilter->ShouldFindEntity(m_pCurrentEnt->m_pInstance))
				continue;

			if (m_hWorldGroupId != WorldGroupId_t() && m_hWorldGroupId != m_pCurrentEnt->m_worldGroupId)
				continue;

			break;
		}
	}

	if (m_pCurrentEnt)
		return m_pCurrentEnt->m_pInstance;

	return nullptr;
}

EntityInstanceByClassIter_t::EntityInstanceByClassIter_t(const char* szClassName, IEntityFindFilter* pFilter, EntityIterType_t eIterType)
{
	m_pCurrentEnt = nullptr;
	m_pFilter = pFilter;
	m_eIterType = eIterType;
	m_hWorldGroupId = WorldGroupId_t();

	if (strchr(szClassName, '*') || eIterType == ENTITY_ITER_OVER_DORMANT)
	{
		m_pszClassName = szClassName;
		m_pEntityClass = nullptr;
	}
	else
	{
		m_pEntityClass = GameEntitySystem()->FindClassByDesignName(szClassName);

		if (!m_pEntityClass)
			m_pszClassName = szClassName;
		else
			m_pszClassName = nullptr;
	}
}

EntityInstanceByClassIter_t::EntityInstanceByClassIter_t(CEntityInstance* pStart, const char* szClassName, IEntityFindFilter* pFilter, EntityIterType_t eIterType)
{
	m_pCurrentEnt = pStart ? pStart->m_pEntity : nullptr;
	m_pFilter = pFilter;
	m_eIterType = eIterType;
	m_hWorldGroupId = WorldGroupId_t();
	m_pszClassName = szClassName;
	m_pEntityClass = NULL;
}

CEntityInstance* EntityInstanceByClassIter_t::First()
{
	m_pCurrentEnt = nullptr;
	return Next();
}

CEntityInstance* EntityInstanceByClassIter_t::Next()
{
	if (m_pEntityClass)
	{
		if (m_pCurrentEnt)
			m_pCurrentEnt = m_pCurrentEnt->m_pNextByClass;
		else
			m_pCurrentEnt = m_pEntityClass->m_pFirstEntity;

		for (; m_pCurrentEnt != nullptr; m_pCurrentEnt = m_pCurrentEnt->m_pNextByClass)
		{
			if ((m_pCurrentEnt->m_flags & EF_MARKED_FOR_DELETE) != 0)
				continue;

			if (m_pFilter && !m_pFilter->ShouldFindEntity(m_pCurrentEnt->m_pInstance))
				continue;

			if (m_hWorldGroupId != WorldGroupId_t() && m_hWorldGroupId != m_pCurrentEnt->m_worldGroupId)
				continue;

			break;
		}
	}
	else if (m_pszClassName)
	{
		if (m_pCurrentEnt)
			m_pCurrentEnt = m_pCurrentEnt->m_pNext;
		else
			m_pCurrentEnt = (m_eIterType == ENTITY_ITER_OVER_ACTIVE) ? GameEntitySystem()->m_EntityList.m_pFirstActiveEntity : GameEntitySystem()->m_EntityList.m_dormantList.m_pHead;

		for (; m_pCurrentEnt != nullptr; m_pCurrentEnt = m_pCurrentEnt->m_pNext)
		{
			if ((m_pCurrentEnt->m_flags & EF_MARKED_FOR_DELETE) != 0)
				continue;

			if (!m_pCurrentEnt->m_designerName.IsValid())
				continue;

			if (!m_pCurrentEnt->ClassMatches(m_pszClassName))
				continue;

			if (m_pFilter && !m_pFilter->ShouldFindEntity(m_pCurrentEnt->m_pInstance))
				continue;

			if (m_hWorldGroupId != WorldGroupId_t() && m_hWorldGroupId != m_pCurrentEnt->m_worldGroupId)
				continue;

			break;
		}
	}

	if (m_pCurrentEnt)
		return m_pCurrentEnt->m_pInstance;

	return nullptr;
}