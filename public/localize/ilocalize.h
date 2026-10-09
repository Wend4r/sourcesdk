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
#include "language.h"

class CBufferString;
class CUtlString;
class KeyValues3;
class IErrorListener;
class ILocVariableSource;
class ILocalizeCompiledString;
class ILocalize_EconProvider;
struct tm;


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
	virtual const char *GetLocalizedString( const char *szText, CBufferString *pOut ) = 0;

	// finds the index of a token by token name, LOCALIZE_INVALID_STRING_INDEX if not found
	virtual LocalizeStringIndex_t FindIndex( const char *pToken ) = 0;

	virtual const char *FindLongestInstance( const char *pToken, ILocalizeTextQuery *pQuery, LocalizeInstanceIndex_t *pInstanceIndex, LocalizeStringIndex_t *pStringIndex ) = 0;

	// k_Lang_None returns the first instance of any language
	virtual LocalizeInstanceIndex_t GetPrimaryInstanceByIndex( LocalizeStringIndex_t index, ELanguage eLanguage ) = 0;
	virtual LocalizeInstanceIndex_t GetPrimaryInstanceByToken( const char *pToken, ELanguage eLanguage ) = 0;
	virtual LocalizeInstanceIndex_t GetNextInstanceIndex( LocalizeInstanceIndex_t index ) = 0;
	virtual const char *GetValueByInstanceIndex( LocalizeInstanceIndex_t index ) = 0;

	// builds a localized formatted string
	virtual const char *ConstructString( CBufferString *pOut, const char *formatString, ILocVariableSource *localizationVariables, bool bAppend ) = 0;
	virtual const char *ConstructMFString( CBufferString *pOut, const char *formatString, ILocVariableSource *localizationVariables, bool bAppend ) = 0;

	virtual const char *ConstructDateString( CBufferString *pOut, LOC_DATE_FORMAT format, tm *pTime, bool bAppend ) = 0;
	virtual const char *ConstructDateString( CBufferString *pOut, LOC_DATE_FORMAT format, uint64 nTime, bool bAppend ) = 0;
	virtual const char *ConstructRelativeDateString( CBufferString *pOut, LOC_DATE_FORMAT fallbackFormat, uint64 nRelativeTime, bool bAppend ) = 0;
	virtual const char *ConstructRelativeTimeString( CBufferString *pOut, LOC_DATE_FORMAT timeFormat, LOC_DATE_FORMAT fallbackFormat, uint64 nRelativeTime, bool bAppend ) = 0;
	virtual const char *ConstructDurationString( CBufferString *pOut, LOC_DURATION_FORMAT format, int nSeconds, bool bAppend ) = 0;
	virtual const char *ConstructNumberString( CBufferString *pOut, LOC_NUMBER_FORMAT format, uint64 nValue, bool bAppend ) = 0;
	virtual const char *ConstructNumberString( CBufferString *pOut, LOC_NUMBER_FORMAT format, double flValue, int nPrecision, bool bAppend ) = 0;

	virtual const char *unk101( const char *, CBufferString * ) = 0;
	virtual bool unk102( const char *, double *, double ) = 0;
	virtual const char *unk103( CBufferString *, uint32, int, int, bool ) = 0;
	virtual const char *unk103( CBufferString *, uint64, int, int, bool ) = 0;

	// gets the values by the string index
	virtual const char *GetNameByIndex( LocalizeStringIndex_t index ) = 0;
	virtual const char *GetValueByIndex( LocalizeStringIndex_t index ) = 0;

	virtual ELanguage unk201( LocalizeStringIndex_t index ) = 0;

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

	virtual bool unk301( const char *, const char * ) = 0;

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
	virtual void GetStatsString( CBufferString *pStrBuf ) = 0;

	virtual const char *unk401( uint32 ) = 0;
	virtual void unk402() = 0;
	virtual void unk403() = 0;
	virtual void unk404() = 0;
	virtual void unk405() = 0;
	virtual void unk406() = 0;
	virtual void unk407() = 0;
	virtual void unk408() = 0;

	virtual ILocalizeCompiledString *CompileString( const char *pString, int, IErrorListener *pErrorListener ) = 0;

	// uses the format strings first: %s1, %s2, ...
	virtual const char *ConstructString( CBufferString *pOut, const char *formatString, int numFormatParameters, ... ) = 0;
	virtual const char *ConstructStringVArgs( CBufferString *pOut, const char *formatString, int numFormatParameters, va_list argList, bool bAppend ) = 0;
	virtual const char *ConstructStringArgArray( CBufferString *pOut, const char *formatString, int numFormatParameters, const char * const *pArgList, bool bAppend ) = 0;

	virtual ILocalize_EconProvider *unk501() = 0;
	virtual void unk502() = 0;

	virtual void RunInternalTest_LocVariableResolve() = 0;
};


#endif // ILOCALIZE_H
