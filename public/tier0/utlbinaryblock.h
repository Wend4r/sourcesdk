//====== Copyright © 1996-2004, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef UTLBINARYBLOCK_H
#define UTLBINARYBLOCK_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "memalloc.h"
#include "strtools.h"
#include "tier1/utlmemory.h"

#include "limits.h"

//-----------------------------------------------------------------------------
// Base class, containing simple memory management
//-----------------------------------------------------------------------------
class CUtlBinaryBlock
{
public:
	CUtlBinaryBlock( int growSize = 0, int initSize = 0 );
	~CUtlBinaryBlock()
	{
#ifdef _DEBUG
		m_nActualLength = 0x7BADF00D;
#else
		m_nActualLength = 0;
#endif
	}

	// NOTE: nInitialLength indicates how much of the buffer starts full
	CUtlBinaryBlock( void* pMemory, int nSizeInBytes, int nInitialLength );
	CUtlBinaryBlock( const void* pMemory, int nSizeInBytes );
	
	CUtlBinaryBlock( const CUtlBinaryBlock& src );
	DLL_CLASS_IMPORT CUtlBinaryBlock &operator=( const CUtlBinaryBlock &src );

#if VALVE_CPP11
	CUtlBinaryBlock( CUtlBinaryBlock&& src );
	DLL_CLASS_IMPORT CUtlBinaryBlock &operator=( CUtlBinaryBlock&& src );
#endif

	DLL_CLASS_IMPORT void		Get( void *pValue, int nMaxLen ) const;
	DLL_CLASS_IMPORT void		Set( const void *pValue, int nLen );
	const void	*Get( ) const;
	void		*Get( );

	unsigned char& operator[]( int i );
	const unsigned char& operator[]( int i ) const;

	int			Length() const;
	DLL_CLASS_IMPORT void		SetLength( int nLength );	// Undefined memory will result
	bool		IsEmpty() const;
	void		Clear();
	void		Purge();

	bool		IsReadOnly() const;


	// Test for equality
	DLL_CLASS_IMPORT bool operator==( const CUtlBinaryBlock &src ) const;

private:
	CUtlMemory<unsigned char> m_Memory;
	int m_nActualLength;
};


//-----------------------------------------------------------------------------
// class inlines
//-----------------------------------------------------------------------------

inline CUtlBinaryBlock::CUtlBinaryBlock( int growSize, int initSize ) 
{
	MEM_ALLOC_CREDIT();
	m_Memory.Init( growSize, initSize );

	m_nActualLength = 0;
}

inline CUtlBinaryBlock::CUtlBinaryBlock( void* pMemory, int nSizeInBytes, int nInitialLength ) : m_Memory( (unsigned char*)pMemory, nSizeInBytes )
{
	m_nActualLength = nInitialLength;
}

inline CUtlBinaryBlock::CUtlBinaryBlock( const void* pMemory, int nSizeInBytes ) : m_Memory( (const unsigned char*)pMemory, nSizeInBytes )
{
	m_nActualLength = nSizeInBytes;
}

inline CUtlBinaryBlock::CUtlBinaryBlock( const CUtlBinaryBlock& src )
{
	Set( src.Get(), src.Length() );
}

#if VALVE_CPP11
inline CUtlBinaryBlock::CUtlBinaryBlock( CUtlBinaryBlock&& src )
: m_Memory( Move(src.m_Memory) )
, m_nActualLength( src.m_nActualLength )
{
	src.m_nActualLength = 0;
}
#endif

inline const void *CUtlBinaryBlock::Get( ) const
{
	return m_Memory.Base();
}

inline void *CUtlBinaryBlock::Get( )
{
	return m_Memory.Base();
}

inline int CUtlBinaryBlock::Length() const
{
	return m_nActualLength;
}

inline unsigned char& CUtlBinaryBlock::operator[]( int i )
{
	return m_Memory[i];
}

inline const unsigned char& CUtlBinaryBlock::operator[]( int i ) const
{
	return m_Memory[i];
}

inline bool CUtlBinaryBlock::IsReadOnly() const
{
	return m_Memory.IsReadOnly();
}

inline bool CUtlBinaryBlock::IsEmpty() const
{
	return Length() == 0;
}

inline void CUtlBinaryBlock::Clear()
{
	SetLength( 0 );
}

inline void CUtlBinaryBlock::Purge()
{
	SetLength( 0 );
	m_Memory.Purge();
}


#endif // UTLBINARYBLOCK_H
