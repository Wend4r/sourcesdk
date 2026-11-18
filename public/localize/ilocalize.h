//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef ILOCALIZE_H
#define ILOCALIZE_H

#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"
#include <tier0/keyvalues.h>
#include "kv3lib/keyvalues3.h"
#include "language.h"

class CBufferString;
class CUtlString;
class KeyValues3;
class IErrorListener;
class ILocVariableSource;
class ILocalizeCompiledString;
class ILocalize_EconProvider;
struct tm;

enum ECurrency : int32;

//-----------------------------------------------------------------------------
// Localization variables, referenced from format strings as {type:name}
//-----------------------------------------------------------------------------
enum LocVariableType_t
{
	LOC_VARIABLE_NONE,		// -
	LOC_VARIABLE_STRING,	// s
	LOC_VARIABLE_TOKEN,		// k, a token looked up with FindSafe
	LOC_VARIABLE_TIME,		// t
	LOC_VARIABLE_MONEY,		// m
	LOC_VARIABLE_INT,		// d
	LOC_VARIABLE_UINT,		// u
	LOC_VARIABLE_FLOAT,		// f
	LOC_VARIABLE_CUSTOM,	// g, formatted by a custom handler
	LOC_VARIABLE_ANY,		// *

	LOC_VARIABLE_TYPE_COUNT
};

typedef CKV3MemberName LocVariableName_t;

struct LocVariableValue_t
{
	LocVariableType_t m_nType;
	union
	{
		const char *m_pszString;
		int64 m_nTime;
		struct
		{
			uint32 m_nAmount;
			ECurrency m_eCurrency;
		} m_Money;
		int64 m_nInt;
		uint64 m_nUInt;
		double m_flValue;
	};
};

// A {type:name} reference parsed from a format string
struct LocVariableReference_t
{
	const char *m_pszName;
	uint32 m_nNameHash;
	LocVariableType_t m_nType;
	int m_nFlags; // From the custom handler's pfnParseFlags
	uint32 m_nHandlerHash;
	const char *m_pszHandlerName;
};

abstract_class ILocVariableSource
{
public:
	virtual ~ILocVariableSource() {}

	// Returns nullptr when the variable isn't set
	virtual const LocVariableValue_t *GetVariable( const LocVariableName_t *pName ) = 0;
};

// The custom handler callbacks, pUserData is the one the handler was installed with
typedef bool ( *LocVariableFormatFn_t )( CBufferString &out, const LocVariableValue_t *pValue, ILocVariableSource *pSource, const LocVariableName_t *pName, int nFlags, void *pUserData );
typedef int ( *LocVariableParseFlagsFn_t )( const char *pszModifiers, void *pUserData );
// Turns a quoted literal into a value
typedef void ( *LocVariableParseLiteralFn_t )( const char *pszLiteral, LocVariableValue_t *pValue, void *pUserData );
// Returns 0 with *pnQuantity or 1 with *pflQuantity set
typedef int ( *LocVariablePluralFn_t )( const LocVariableValue_t *pValue, ILocVariableSource *pSource, const LocVariableName_t *pName, void *pUserData, int64 *pnQuantity, double *pflQuantity );
// Writes the token whose gender the variable takes
typedef bool ( *LocVariableCompatibleTokenFn_t )( const LocVariableValue_t *pValue, ILocVariableSource *pSource, const LocVariableName_t *pName, void *pUserData, CBufferString &token );


//-----------------------------------------------------------------------------
// Interface used to query text size so we can choose the longest one
//-----------------------------------------------------------------------------
abstract_class ILocalizeTextQuery
{
public:
	virtual int ComputeTextWidth( const char *pString ) = 0;
};


//-----------------------------------------------------------------------------
// Callback which is triggered when any localization string changes
// Is not called when a localization string is added
//-----------------------------------------------------------------------------
abstract_class ILocalizationChangeCallback
{
public:
	// pChangedTokens is only passed to callbacks installed with flag 1, otherwise it is nullptr
	virtual void OnLocalizationChanged( CUtlVector< CUtlString > *pChangedTokens ) = 0;
};


//-----------------------------------------------------------------------------
// Purpose: Handles localization of text
//			looks up string names and returns the localized UTF-8 text
//-----------------------------------------------------------------------------
// direct references to localized strings
class LocalizeStringIndex_t
{
public:
	constexpr LocalizeStringIndex_t() : m_Value( -1 ) {}
	constexpr explicit LocalizeStringIndex_t( int32 nValue ) : m_Value( nValue ) {}

	int32 GetRaw() const { return m_Value; }
	void SetRaw( int32 nValue ) { m_Value = nValue; }

	bool operator==( const LocalizeStringIndex_t &other ) const { return m_Value == other.m_Value; }
	bool operator!=( const LocalizeStringIndex_t &other ) const { return m_Value != other.m_Value; }

private:
	int32 m_Value;
};

constexpr LocalizeStringIndex_t LOCALIZE_INVALID_STRING_INDEX( -1 );

class LocalizeInstanceIndex_t
{
public:
	constexpr LocalizeInstanceIndex_t() : m_Value( -1 ) {}
	constexpr explicit LocalizeInstanceIndex_t( int32 nValue ) : m_Value( nValue ) {}

	int32 GetRaw() const { return m_Value; }
	void SetRaw( int32 nValue ) { m_Value = nValue; }

	bool operator==( const LocalizeInstanceIndex_t &other ) const { return m_Value == other.m_Value; }
	bool operator!=( const LocalizeInstanceIndex_t &other ) const { return m_Value != other.m_Value; }

private:
	int32 m_Value;
};

enum LOC_DATE_FORMAT
{
	LOC_DATE_DAY_MONTH_YEAR_NUMERIC,
	// AMNOTE: Named after the #LOC_Date_Format_ tokens they use
	LOC_DATE_DAY_MONTH_NUMERIC,
	LOC_DATE_MONTH_DAY_YEAR,
	LOC_DATE_DAY_OF_WEEK_MONTH_DAY_YEAR,
	LOC_DATE_HOUR_MINUTE,
	LOC_DATE_HOUR_MINUTE_SECOND,
	LOC_DATE_DAY_OF_WEEK_MONTH_DAY_YEAR_HOUR_MINUTE_SECOND,
	LOC_DATE_DAY_MONTH,
	LOC_DATE_DAY_MONTH_YEAR,
	LOC_DATE_DAY_MONTH_YEAR_HOUR_MINUTE,
	LOC_DATE_DAY_MONTH_YEAR_HOUR_MINUTE_SECOND,
	LOC_DATE_DAY_OF_WEEK_SHORT_MONTH_SHORT_DAY_HOUR_MINUTE_SECOND,
	LOC_DATE_DAY_OF_WEEK,
	LOC_DATE_DAY_OF_WEEK_DAY_MONTH_HOUR_MINUTE,
	LOC_DATE_DAY_MONTH_SHORT,
	LOC_DATE_ECON_MONTH_SHORT_DAY_YEAR_HOUR_MINUTE_SECOND,
	LOC_DATE_ECON_MONTH_SHORT_DAY_YEAR_HOUR_MINUTE_SECOND_GMT,
	LOC_DATE_NEUTRAL_TIMESTAMP,
};

enum LOC_DURATION_FORMAT
{
	LOC_DURATION_DAYS_SHORTEST_OPT_HOURS_MINUTES_SECONDS,
	LOC_DURATION_DAYS_HOURS_MINUTES_SECONDS,
	LOC_DURATION_HOURS_MINUTES_SECONDS,
	LOC_DURATION_HOURS_OPT_MINUTES_SECONDS,
	LOC_DURATION_MINUTES_SECONDS,
	LOC_DURATION_DAYS,
	LOC_DURATION_LARGEST_NEEDED_DAYS_HOURS_MINUTES_SECONDS_SUFFIXED,
	LOC_DURATION_LARGEST_NEEDED_DAYS_HOURS_MINUTES_SUFFIXED,
	LOC_DURATION_LARGEST_NEEDED_HOURS_MINUTES_SECONDS_SUFFIXED,
	LOC_DURATION_LARGEST_NEEDED_HOURS_MINUTES_SUFFIXED,
	LOC_DURATION_NONZERO_HOURS_MINUTES,
	LOC_DURATION_NONZERO_HOURS_MINUTES_SECONDS,
	LOC_DURATION_NONZERO_HOURS_MINUTES_SECONDS_EXTRA,
	LOC_DURATION_NONZERO_DAYS_HOURS_MINUTES,
	LOC_DURATION_NONZERO_DAYS_HOURS_MINUTES_SECONDS,
	LOC_DURATION_NONZERO_DAYS_HOURS_MINUTES_SECONDS_EXTRA,
};

// AMNOTE: The floating-point ConstructNumberString also takes 4, which formats the absolute value
enum LOC_NUMBER_FORMAT
{
	LOC_NUMBER_SIGNED,
	LOC_NUMBER_UNSIGNED,
	LOC_NUMBER_SIGNED_PREFER_INTEGRAL,
	LOC_NUMBER_MONEY,
};

abstract_class ILocalize : public IAppSystem
{
public:
	virtual ~ILocalize() {}

	// adds the contents of a file to the localization table
	virtual bool AddFile( const char *szFileName, const char *pPathID = NULL ) = 0;

	// Remove all strings from the table
	virtual void RemoveAll() = 0;
	virtual void RemoveAllFileStrings() = 0;

	// Finds the localized text for pToken. Returns NULL if none is found.
	virtual const char *FindUnsafe( const char *pToken ) = 0;
	virtual const char *FindUnsafeLocal( const char *pToken ) = 0;

	// Like FindUnsafe(), but as a failsafe, returns an error message instead of NULL if the string isn't found.
	virtual const char *FindSafe( const char *pToken ) = 0;

	virtual const char *FindSubLanguageUnsafe( const char *pToken ) = 0;
	virtual const char *FindSubLanguageSafe( const char *pToken ) = 0;
	virtual const char *GetLocalizedString( const char *szText, CBufferString &out ) = 0;

	// finds the index of a token by token name, LOCALIZE_INVALID_STRING_INDEX if not found
	virtual LocalizeStringIndex_t FindIndex( const char *pToken ) = 0;

	virtual const char *FindLongestInstance( const char *pToken, ILocalizeTextQuery *pQuery, LocalizeInstanceIndex_t *pInstanceIndex, LocalizeStringIndex_t *pStringIndex ) = 0;

	// k_Lang_None returns the first instance of any language
	virtual LocalizeInstanceIndex_t GetPrimaryInstanceByIndex( LocalizeStringIndex_t index, ELanguage eLanguage ) = 0;
	virtual LocalizeInstanceIndex_t GetPrimaryInstanceByToken( const char *pToken, ELanguage eLanguage ) = 0;
	virtual LocalizeInstanceIndex_t GetNextInstanceIndex( LocalizeInstanceIndex_t index ) = 0;
	virtual const char *GetValueByInstanceIndex( LocalizeInstanceIndex_t index ) = 0;

	// builds a localized formatted string
	virtual const char *ConstructString( CBufferString &out, const char *formatString, ILocVariableSource *localizationVariables, bool bAppend ) = 0;
	virtual const char *ConstructMFString( CBufferString &out, const char *formatString, ILocVariableSource *localizationVariables, bool bAppend ) = 0;

	virtual const char *ConstructDateString( CBufferString &out, LOC_DATE_FORMAT format, tm *pTime, bool bAppend ) = 0;
	virtual const char *ConstructDateString( CBufferString &out, LOC_DATE_FORMAT format, uint64 nTime, bool bAppend ) = 0;
	virtual const char *ConstructRelativeDateString( CBufferString &out, LOC_DATE_FORMAT fallbackFormat, uint64 nRelativeTime, bool bAppend ) = 0;
	virtual const char *ConstructRelativeTimeString( CBufferString &out, LOC_DATE_FORMAT timeFormat, LOC_DATE_FORMAT fallbackFormat, uint64 nRelativeTime, bool bAppend ) = 0;
	virtual const char *ConstructDurationString( CBufferString &out, LOC_DURATION_FORMAT format, int nSeconds, bool bAppend ) = 0;
	virtual const char *ConstructNumberString( CBufferString &out, LOC_NUMBER_FORMAT format, uint64 nValue, bool bAppend ) = 0;
	virtual const char *ConstructNumberString( CBufferString &out, LOC_NUMBER_FORMAT format, double flValue, int nPrecision, bool bAppend ) = 0;

	// Turns a number written with the current language's signs and separators back into a plain one
	virtual const char *ParseNumberString( const char *pszNumber, CBufferString &out ) = 0;
	// Also accepts NaN, +Inf and -Inf, returns false and sets flDefault when the string isn't a number
	virtual bool ParseNumber( const char *pszNumber, double *pflValue, double flDefault ) = 0;

	// Construct formatted monetary strings.
	// The integer version's value is scaled by 100 so that fractions are represented exactly.
	// Normally the precision of the value is set according to the currency's conventions but it can be forced with a non-negative nOverridePrecision.
	virtual const char *ConstructMoneyString( CBufferString &out, uint32 nValue, ECurrency eCurrency, int nOverridePrecision, bool bAppend ) = 0;
	virtual const char *ConstructMoneyString( CBufferString &out, uint64 nValue, ECurrency eCurrency, int nOverridePrecision, bool bAppend ) = 0;

	// gets the values by the string index
	virtual const char *GetNameByIndex( LocalizeStringIndex_t index ) = 0;
	virtual const char *GetValueByIndex( LocalizeStringIndex_t index ) = 0;

	// k_Lang_None (-1) for an invalid index
	virtual ELanguage GetLanguageByIndex( LocalizeStringIndex_t index ) = 0;

	virtual const char *GetCurrentLanguage() = 0;
	virtual ELanguage GetCurrentELanguage() = 0;
	virtual const char *GetCurrentSubLanguage() = 0;
	virtual void SetCurrentSubLanguage( const char *pSubLanguage ) = 0;
	virtual const char *SetCurrentSubLanguageFromSystem() = 0;
	virtual int GetLinguisticCaseFlags() = 0;
	virtual uint32 GetLocalizeFlags() const = 0;
	virtual void SetLocalizeFlags( uint32 nFlags ) = 0;

	virtual void CheckForFileChanges() = 0;

	///////////////////////////////////////////////////////////////////
	// the following functions should only be used by localization editors

	// iteration functions
	virtual LocalizeStringIndex_t GetFirstStringIndex() = 0;
	// returns the next index, or LOCALIZE_INVALID_STRING_INDEX if no more strings available
	virtual LocalizeStringIndex_t GetNextStringIndex( LocalizeStringIndex_t index ) = 0;

	// changes the value of a string
	virtual void SetValueByIndex( LocalizeStringIndex_t index, const char *pNewValue ) = 0;

	// saves the entire contents of the token tree to the file
	virtual bool SaveToFile( const char *szFileName ) = 0;

	// adds the file in every language, the %language% in its name is replaced by each language name
	virtual bool AddAllLanguageFiles( const char *pszFileName, const char *pPathID ) = 0;

	// iterates the filenames
	virtual int GetLocalizationFileCount() = 0;
	virtual const char *GetLocalizationFileName( int index ) = 0;

	// returns the name of the file the specified localized string is stored in
	virtual const char *GetFileNameByIndex( LocalizeStringIndex_t index ) = 0;

	// for development only, reloads localization files
	virtual void ReloadLocalizationFiles( uint32, const CUtlVector< int > * ) = 0;

	// Is called when any localization strings change
	virtual void InstallChangeCallback( ILocalizationChangeCallback *pCallback, uint32 nFlags ) = 0;
	virtual void RemoveChangeCallback( ILocalizationChangeCallback *pCallback ) = 0;

	virtual void ReloadLocalizationFilesInLanguage( const char *pLanguage ) = 0;
	virtual bool ReadLocalizationBuffer( const char *pFromFile, void *memBlock, int fileSize, const char *pForLanguage ) = 0;
	virtual bool AddLocalizationKeyValues( const char *pFromFile, KeyValues *pRoot, const char *pForLanguage ) = 0;
	virtual bool AddLocalizationKeyValues( const char *pFromFile, KeyValues3 *pRoot, const char *pForLanguage ) = 0;
	virtual void LocalizedMessageBox( const char *pTitle, const char *pBody ) = 0;

	virtual ILocalize *GetFallbackImpl() const = 0;
	virtual void SetFallbackImpl( ILocalize *pLocalize ) = 0;
	virtual bool GetTrackingMode() const = 0;
	virtual void SetTrackingMode( bool bEnable ) = 0;
	virtual bool DoKoreanJongSungRuleInPlace( char *pUnicodeBuffer, int unicodeBufferSizeInBytes ) = 0;
	virtual void GetStatsString( CBufferString &strBuf ) = 0;

	// The type letter of a localization variable used in format strings (s, k, t, m, d, u, f, g or *), "?" for an unknown type
	virtual const char *GetLocVariableTypeName( LocVariableType_t nType ) = 0;
	// Format a value into out, nType is the type it should have or LOC_VARIABLE_ANY
	virtual bool FormatLocVariableValue( LocVariableType_t nType, const LocVariableValue_t *pValue, int nFlags, CBufferString &out, bool bAppend ) = 0;
	// Looks the referenced variable up in pSource, or takes its quoted literal, and formats it into out
	virtual bool FormatLocVariable( ILocVariableSource *pSource, const LocVariableReference_t *pReference, CBufferString &out, bool bAppend ) = 0;
	// Handlers for the LOC_VARIABLE_CUSTOM variables, keyed by the handler name of the reference
	virtual void InstallCustomLocVariableHandler( const LocVariableName_t &handlerName, LocVariableFormatFn_t pfnFormat, LocVariableParseFlagsFn_t pfnParseFlags, LocVariableParseLiteralFn_t pfnParseLiteral, void *pUserData, int nUserValue ) = 0;
	// Used when resolving plural and gender variants with a variable it handles
	virtual void SetCustomLocVariablePluralHandler( const LocVariableName_t &handlerName, LocVariablePluralFn_t pfnPlural ) = 0;
	virtual void SetCustomLocVariableCompatibleTokenHandler( const LocVariableName_t &handlerName, LocVariableCompatibleTokenFn_t pfnCompatibleToken ) = 0;
	virtual void RemoveCustomLocVariableHandler( const LocVariableName_t &handlerName ) = 0;
	// Returns the nUserValue it was installed with, 0 when there is no such handler
	virtual int GetCustomLocVariableHandlerValue( const LocVariableName_t &handlerName ) = 0;

	virtual ILocalizeCompiledString *CompileString( const char *pString, int, IErrorListener *pErrorListener ) = 0;

	// uses the format strings first: %s1, %s2, ...
	virtual const char *ConstructString( CBufferString &out, const char *formatString, int numFormatParameters, ... ) = 0;
	virtual const char *ConstructStringVArgs( CBufferString &out, const char *formatString, int numFormatParameters, va_list argList, bool bAppend ) = 0;
	virtual const char *ConstructStringArgArray( CBufferString &out, const char *formatString, int numFormatParameters, const char * const *pArgList, bool bAppend ) = 0;

	virtual ILocalize_EconProvider *GetEconProvider() = 0;
	virtual void RunInternalTest_LocVariableParse() = 0;

	virtual void RunInternalTest_LocVariableResolve() = 0;
};


#endif // ILOCALIZE_H
