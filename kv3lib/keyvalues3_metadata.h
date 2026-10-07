#ifndef KEYVALUES3_METADATA_H
#define KEYVALUES3_METADATA_H

#ifdef _WIN32
#pragma once
#endif

#include "kv3lib/keyvalues3.h"

#include "tier0/memdbgon.h"

struct KV3MetaData_t
{
	KV3MetaData_t() : m_nLine( 0 ), m_nColumn( 0 ), m_nFlags( 0 ) {}

	void Clear()
	{
		m_nLine = 0;
		m_nColumn = 0;
		m_nFlags = 0;
		m_sName = CUtlSymbolLarge();
		m_Comments.RemoveAll();
	}

	void Purge()
	{
		m_nLine = 0;
		m_nColumn = 0;
		m_nFlags = 0;
		m_sName = CUtlSymbolLarge();
		m_Comments.Purge();
	}

	typedef CUtlOrderedMap<int, CBufferString> CommentsMap_t;

	int 			m_nLine;
	int 			m_nColumn;
	uint			m_nFlags;
	CUtlSymbolLarge m_sName;
	CommentsMap_t 	m_Comments;
};

#include "tier0/memdbgoff.h"

#endif // KEYVALUES3_METADATA_H
