#ifndef STRUCTUREDREPORT_H
#define STRUCTUREDREPORT_H

#pragma once

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include <tuple>

//-----------------------------------------------------------------------------
// One column of a structured report: its name and a value of the column type. The game sets
// the int to 0 or 1 per column (1 for the ticks, positions, velocities, speed and yaw of its
// reports); nothing in the server reads it or the value.
//-----------------------------------------------------------------------------
template < class T >
struct StructuredReportColumn_t
{
	const char *m_pszName;
	// Named by what it holds.
	int32 m_nColumnOption;
	T m_Value;
};

//-----------------------------------------------------------------------------
// The columns of a structured report in the order of the value types, each one followed by the
// rest; the empty end still takes its byte, so the set ends with 8 bytes nothing uses.
//-----------------------------------------------------------------------------
template < class... TValues >
struct StructuredReportColumns_t
{
};

template < class T, class... TRest >
struct StructuredReportColumns_t< T, TRest... >
{
	StructuredReportColumn_t< T > m_First;
	StructuredReportColumns_t< TRest... > m_Rest;
};

//-----------------------------------------------------------------------------
// A report of rows whose columns are named by the enum TField and typed by TValues; the
// movement code of the game keeps two (CStructuredReport< BhopReportField, false, ... > and
// CStructuredReport< MoveReportField, false, ... >), static objects that nothing in the server
// reads after their construction. The meaning of the bool argument is not known. A row is a
// std::tuple< TValues... > allocated on its own. One slot on both platforms, no virtual
// destructor.
//-----------------------------------------------------------------------------
template < class TField, bool B, class... TValues >
abstract_class CStructuredReport
{
public:
	// Empty on both platforms; no call through the table was found.
	virtual void Stub_0() = 0;

public:
	// Named by what they hold: "bunny hopping" and "bhop", "movement keys" and "movement".
	const char *m_pszDescription;
	const char *m_pszName;
	StructuredReportColumns_t< TValues... > m_Columns;
	CUtlVector< std::tuple< TValues... > * > m_Rows;
	// Zeroed at construction; nothing in the server reads them.
	int32 m_nReserved;
	bool m_bReserved;
};

#endif // STRUCTUREDREPORT_H
