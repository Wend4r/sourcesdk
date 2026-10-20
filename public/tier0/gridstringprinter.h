#ifndef GRIDSTRINGPRINTER_H
#define GRIDSTRINGPRINTER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/logging.h"
#include "tier0/utlstring.h"
#include "tier0/utlsymbollarge.h"
#include "tier0/utlvector.h"

class Color;

#define FOR_EACH_GRID_ROW( grid, iter ) \
	for ( int iter = 0; iter < ( grid ).NumRows(); iter++ )
#define FOR_EACH_GRID_ROW_BACK( grid, iter ) \
	for ( int iter = ( grid ).NumRows() - 1; iter >= 0; iter-- )
#define FOR_EACH_GRID_COLUMN( grid, iter ) \
	for ( int iter = 0; iter < ( grid ).NumColumns(); iter++ )
#define FOR_EACH_GRID_COLUMN_BACK( grid, iter ) \
	for ( int iter = ( grid ).NumColumns() - 1; iter >= 0; iter-- )

//-----------------------------------------------------------------------------
// Prints strings as a table of aligned columns
//-----------------------------------------------------------------------------
class CGridStringPrinter
{
public:
	CGridStringPrinter() :
		unk098( false ), m_bUseOuterSeparators( false ), m_bWordWrapAllColumns( false ), m_bUseHeaderDivider( true ), m_bTrimValues( true ),
		m_nMaxStringLength( -1 ), m_chColumnSeparator( '|' ), m_chHeaderDivider( '-' ) {}

	DLL_CLASS_IMPORT void Clear();

	DLL_CLASS_IMPORT void SetColumnHeader( int nColumn, const char *pszHeader );
	DLL_CLASS_IMPORT void SetColumnJustification( int nColumn, bool bJustification );
	DLL_CLASS_IMPORT void SetWordWrapColumn( int nColumn, bool bWordWrap, bool bWrapAtSpaces );
	DLL_CLASS_IMPORT void SetWordWrapAllColumns( bool bWordWrap );
	DLL_CLASS_IMPORT void SetMaxStringLengthAllColumns( int nMaxLength );
	DLL_CLASS_IMPORT void SetUseHeaderDivider( bool bUseHeaderDivider );
	DLL_CLASS_IMPORT void SetString( int nRow, int nColumn, const char *pszString );

	// Split a formatted line at chDelimiter into the column headers or a new row
	DLL_CLASS_IMPORT void DelimitedHeader( char chDelimiter, PRINTF_FORMAT_STRING const char *pszFormat, ... ) FMTFUNCTION( 3, 4 );
	DLL_CLASS_IMPORT void DelimitedPrint( char chDelimiter, PRINTF_FORMAT_STRING const char *pszFormat, ... ) FMTFUNCTION( 3, 4 );
	DLL_CLASS_IMPORT void DelimitedPrint_SetTrimValues( bool bTrimValues );

	DLL_CLASS_IMPORT CUtlString GetColumnHeader( int nColumn ) const;
	DLL_CLASS_IMPORT CUtlString GetString( int nRow, int nColumn ) const;
	DLL_CLASS_IMPORT int NumColumns() const;
	DLL_CLASS_IMPORT int NumRows() const;

	// bIncludeHeader starts the lines with the column headers and the divider under them
	DLL_CLASS_IMPORT void GetLines( CUtlVector< CUtlString > &lines, bool bIncludeHeader ) const;
	DLL_CLASS_IMPORT void Spew( LoggingChannelID_t channelID, const char *pszPrefix ) const;
	DLL_CLASS_IMPORT void SimpleSpew( LoggingChannelID_t channelID, const char *pszPrefix ) const;
	DLL_CLASS_IMPORT void SimpleSpewColor( LoggingChannelID_t channelID, const Color &color, const char *pszPrefix ) const;

protected:
	struct ColumnHeader_t
	{
		CUtlSymbolLarge m_Name;

		// -1 uses the maximum length set for all columns
		int m_nMaxStringLength;
		bool m_bJustification;
		bool m_bWordWrap;

		// A wrapped line that would split a word ends after the last space instead
		bool m_bWrapAtSpaces;
	};

	DLL_CLASS_IMPORT ColumnHeader_t *GetHeader( int nColumn );
	DLL_CLASS_IMPORT const ColumnHeader_t *GetHeader( int nColumn ) const;

private:
	CUtlVector< CUtlVector< CUtlSymbolLarge > > m_Rows;
	CUtlVector< ColumnHeader_t > m_Headers;
	CUtlSymbolTableLarge m_Symbols;
	bool unk098;

	// Puts the column separator before the first and after the last column too
	bool m_bUseOuterSeparators;
	bool m_bWordWrapAllColumns;
	bool m_bUseHeaderDivider;
	bool m_bTrimValues;
	int m_nMaxStringLength;
	char m_chColumnSeparator;
	char m_chHeaderDivider;
};

#endif // GRIDSTRINGPRINTER_H
