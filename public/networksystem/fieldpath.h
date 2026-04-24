#ifndef FIELDPATH_H
#define FIELDPATH_H

#pragma once

class CFieldPath
{
public:
	enum
	{
		MAX_PATH_DEPTH = 10
	};

	int Count() const { return m_nCount; }
	const int16 *Base() const { return m_bReadOnly ? m_pPath : m_Path; }

public:
	union
	{
		int16 *m_pPath;
		int16 m_Path[12];
	};

	int16 m_nCount;
	bool m_bReadOnly;
	byte m_pad;
};

COMPILE_TIME_ASSERT( sizeof( CFieldPath ) == 32 );

#endif // !defined( FIELDPATH_H )
