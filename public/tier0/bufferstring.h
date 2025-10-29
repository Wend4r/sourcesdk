#ifndef BUFFERSTRING_H
#define BUFFERSTRING_H

#ifdef _WIN32
#pragma once
#endif

#define __need_size_t
#include <stddef.h>
#undef __need_size_t

#include "basetypes.h"
#include "dbg.h"
#include "platform.h"
#include "strtools.h"
#include "utlstring.h"
#include "vmath.h"

#include <initializer_list>

#if defined(DEBUG) && defined(BUFFERSTRING_OVERFLOW_CATCH)
// Assertion macro for catching stack buffer overflows in debug mode.
#define Assert_BSO( exp ) { if ( IsStackAllocated() && !CanHeapAllocate() ) Assert( exp ); }
#else // !defined(DEBUG) || !defined(BUFFERSTRING_OVERFLOW_CATCH)
#define Assert_BSO( exp )
#endif // defined(DEBUG) && defined(BUFFERSTRING_OVERFLOW_CATCH)

class CFormatStringElement;
class IFormatOutputStream;

/*
	Main idea of CBufferString is to provide stack allocated string
	with the ability to convert to the heap allocation if allowed.

	By default CBufferString provides 8 bytes of stack allocation and could be increased by
	using CBufferStringN<SIZE> where custom stack SIZE could be used.

	Example usage of CBufferStringN class:
 
	* Basic buffer allocation:
	```
		CBufferStringN buff = "Hello World!";
		buff.ToUpper();
		Msg("Result: %s\n", buff.String());
	```
	additionaly the heap allocation of the bufferstring could be disabled. If the heap allocation is disabled and
	if the bufferstring capacity is not enough to perform the growing operation, the app would exit with an Assert;

	* Most, if not all the functions would ensure the bufferstring capacity and enlarge it when needed.
	In case of stack allocated buffers, if the requested size exceeds stack size, it would switch to heap allocation instead.
*/

class CBufferString
{
public:
	enum EType_t
	{
		BS_TYPE_HEAP = 0,
		BS_TYPE_STACK = 1,
	};

	// Allocation option flags used internally for utlfriends (CUtlBuffer/CUtlString).
	enum EAllocationOption_t : int
	{
		BS_AO_NONE = 0,
		BS_AO_FREE = 1 << 1,

		BS_AO_EXPAND_SMALL_BUFFER = 1 << 8,
		BS_AO_EXPAND_BUFFER_ANYWAY = 1 << 9,

		BS_AO_HAS_HEAP_ALLOCATION = 1 << 31,

		BS_AO_HEAP_ALLOCATION = BS_AO_EXPAND_SMALL_BUFFER | BS_AO_HAS_HEAP_ALLOCATION,
		BS_AO_FREE_HEAP_ALLOCATION = BS_AO_FREE | BS_AO_HAS_HEAP_ALLOCATION,
	};

public:
	// Default constructor - creates an empty buffer.
	CBufferString( EType_t eType = BS_TYPE_STACK, bool bAllowHeapAllocation = true ) : 
	    m_nLength( 0 ), 
	    m_bOverflowed( false ), 
	    m_bFreeHeap( false ), 

	    m_nAllocatedSize( eType * DATA_SIZE ), 
	    m_bStackAllocated( eType ), 
	    m_bAllowHeapAllocation( bAllowHeapAllocation )
	{
	}

	// Constructs buffer initialized with a C-string (optional length and heap allowance; will be allocated on the heap).
	CBufferString( const char *pString, int nLen, bool bAllowHeapAllocation = true ) : 
	    CBufferString( BS_TYPE_HEAP, bAllowHeapAllocation )
	{
		Set( pString, nLen );
	}
	CBufferString(const char *pString, bool bAllowHeapAllocation = true) : CBufferString(pString, V_strlen(pString), bAllowHeapAllocation) {}
	template< size_t N > CBufferString( const char (&str)[N], bool bAllowHeapAllocation = true ) : CBufferString( str, N - 1, bAllowHeapAllocation ) {}
	CBufferString( const CUtlString &str, bool bAllowHeapAllocation = true ) : CBufferString( str, str.Length(), bAllowHeapAllocation ) {}
	explicit CBufferString( std::string_view view, bool bAllowHeapAllocation = true ) : CBufferString( view.data(), static_cast<int>(view.size()), bAllowHeapAllocation ) {}
	CBufferString( const CBufferString &copyFrom, bool bAllowHeapAllocation = true ) : CBufferString( copyFrom, copyFrom.Length(), bAllowHeapAllocation ) {}
	CBufferString( CBufferString &&moveFrom ) noexcept : 
	    m_nLengthStaff( Move( moveFrom.m_nLengthStaff ) ), 
	    m_nAllocatedStaff( Move( moveFrom.m_nAllocatedStaff ) ), 
	    m_Buffer( Move( moveFrom.m_Buffer ) )
	{
	}

	/// Concat-based constructors.
	template< size_t N > FORCEINLINE CBufferString( const char *pPrefix, const char *(&strs)[N], const int (&nLengths)[N], bool bAllowHeapAllocation = true ) : 
	    CBufferString( pPrefix, -1, bAllowHeapAllocation )
	{
		AppendConcatN( strs, nLengths );
	}
	template< size_t N > FORCEINLINE CBufferString( const char *pPrefix, const char *(&strs)[N], bool bAllowHeapAllocation = true ) : 
	    CBufferString( pPrefix, -1, bAllowHeapAllocation )
	{
		AppendConcatN( strs );
	}
	template< size_t N > FORCEINLINE CBufferString( const char *(&strs)[N], bool bAllowHeapAllocation = true ) : 
	    CBufferString( BS_TYPE_HEAP, bAllowHeapAllocation )
	{
		AppendConcatN( strs );
	}
	CBufferString( std::initializer_list< const char * > strs, bool bAllowHeapAllocation = true ) : 
	    CBufferString( bAllowHeapAllocation )
	{
		AppendConcat( static_cast<int>(strs.size()), strs.begin() );
	}

protected:
	// Growable constructor for internal use by derived classes with pre-allocated buffer.
	CBufferString( size_t nAllocatedSize, bool bAllowHeapAllocation = true ) : 
	    m_nLength( 0 ), 
	    m_bOverflowed( false ), 
	    m_bFreeHeap( false ), 

	    m_nAllocatedSize( static_cast<uint32>(nAllocatedSize) ),
	    m_bStackAllocated( true ), 
	    m_bAllowHeapAllocation( bAllowHeapAllocation )
	{
		Assert( nAllocatedSize >= DATA_SIZE );
	}

public:
	~CBufferString() { Purge(); } // Destructor - purges any allocated memory.

	int Length() const { return m_nLength; } // Returns the current string length.
	bool IsOverflowed() const { return m_bOverflowed != 0; } // Returns whether buffer overflow occurred.
	bool ShouldFreeMemory() const { return m_bFreeHeap != 0; } // Returns whether memory must be freed.

	int AllocatedSize() const { return m_nAllocatedSize; } // Returns size of currently allocated memory in bytes.
	bool IsStackAllocated() const { return m_bStackAllocated != 0; } // Returns whether the string uses stack memory.
	bool CanHeapAllocate() const { return m_bAllowHeapAllocation != 0; } // Returns whether heap allocation is allowed.

	bool IsEmpty() const { return Length() == 0; } // Returns whether the string is empty.
	bool IsAllocationEmpty() const { return AllocatedSize() == 0; } // Returns whether buffer allocation is empty.

protected:
	int IsSmall() const { return Length() < DEFAULT_CAPACITY_SIZE; } // Returns true if length is less than default capacity (used internally).
	int ChunkCount() const { return Length() / CHUNK_SIZE; } // Returns number of chunks (used internally).

public:
	int BytesLeft( int nLength ) const { return AllocatedSize() - nLength; } // Returns number of bytes available from a given length.
	int BytesLeft() const { return BytesLeft( m_nLength ); } // Returns number of bytes available from current length.
	bool IsFull() const { return BytesLeft() == 0; } // Returns true if no space is left in the bufferstring.

	EType_t GetType() const { return static_cast<EType_t>(m_bStackAllocated); };

	// Checks whether a pointer lies within the current buffer range.
	bool IsInputStringUnsafe( const void *pData ) const
	{
		const uintp nBaseDiff = (uintp)Base();
		const uintp nDataDiff = (uintp)pData;

		return ( nDataDiff >= (uintp)this && nDataDiff < (uintp)&m_Buffer.m_pString ) ||
		       ( !IsAllocationEmpty() && nDataDiff >= nBaseDiff && nDataDiff < nBaseDiff + AllocatedSize() );
	}

protected:
	/// Gets internal base buffer pointer (stack or heap).
	char *Base() { return IsStackAllocated() ? m_Buffer.m_szString : m_Buffer.m_pString; }
	const char *Base() const { return const_cast<CBufferString *>( this )->Base(); }

public:
	// Returns as C-string.
	const char *String() const { return IsStackAllocated() ? m_Buffer.m_szString : ( IsAllocationEmpty() ? StringFuncs<char>::EmptyString() : m_Buffer.m_pString ); }
	const char *Get() const { return String(); }
	operator const char *() const { return Get(); }
	operator std::string_view() const { return { Get(), static_cast<size_t>(Length()) }; }

	// Clears the string contents (does not free heap).
	void Clear()
	{
		if ( !IsAllocationEmpty() )
			Base()[0] = '\0';

		m_nLength = 0;
	}

	// Sets contents of buffer from another string.
	const char *Set( const char *pString, int nLen = -1, bool bIgnoreAlignment = false )
	{
		Assert_BSO( nLen < 0 || nLen <= BytesLeft() );

		Clear();

		return Insert( 0, pString, nLen, bIgnoreAlignment );
	}

	/// operator=
	template< size_t N > CBufferString &operator=( const char (&str)[N] ) { Set( str, N - 1 ); return *this; }
	CBufferString &operator=( const CUtlString &str ) { Set( str.String(), str.Length() ); return *this; }
	CBufferString &operator=( const CBufferString &buf ) { Set( buf.String(), buf.Length() ); return *this; }
	CBufferString &operator=( std::string_view view ) { Set( view.data(), static_cast<int>(view.length()) ); return *this; }
	CBufferString &operator=( CBufferString &&moveFrom ) noexcept { MoveFrom( moveFrom ); return *this; }

	// Appends string to current end of buffer.
	const char *Append( const char *pString, int nLen = -1, bool bIgnoreAlignment = false )
	{
		Assert_BSO( nLen < 0 || nLen <= BytesLeft() );

		return Insert( Length(), pString, nLen, bIgnoreAlignment );
	}

	/// operator+=
	template< size_t N > CBufferString &operator+=( const char (&str)[N] ) { Append( str, N - 1 ); return *this; }
	CBufferString &operator+=( const CUtlString &str ) { Append( str.String(), str.Length() ); return *this; }
	CBufferString &operator+=( const CBufferString &buf ) { Append( buf.String(), buf.Length() ); return *this; }
	CBufferString &operator+=( std::string_view view ) { Append( view.data(), static_cast<int>(view.length()) ); return *this; }
	CBufferString &operator+=( char value )
	{
		const int nLength = Length();
		constexpr int nCharsNeeded = 1;

		Assert_BSO( BytesLeft() >= nCharsNeeded );

		char *pBufferString = GetInsertPtr( nLength, nCharsNeeded );

		pBufferString[0] = value;
		pBufferString[1] = '\0';

		return *this;
	}
	CBufferString &operator+=( int value )
	{
		const int nLength = Length();
		const int nCharsNeeded = value ? static_cast<int>( V_log10f( V_fabsf( value ) ) ) + ( value < 0 ) + 1 : 1;

		Assert_BSO( BytesLeft( nLength ) >= nCharsNeeded );
		V_snprintf( GetInsertPtr( nLength, nCharsNeeded ), nCharsNeeded + 1, "%d", value );

		return *this;
	}
	CBufferString &operator+=( double value )
	{
		const int nLength = Length();
		const int nCharsNeeded = V_snprintf( NULL, 0, "%lg", value );

		Assert_BSO( BytesLeft( nLength ) >= nCharsNeeded );
		V_snprintf( GetInsertPtr( nLength, nCharsNeeded ), nCharsNeeded + 1, "%lg", value );

		return *this;
	}

protected:
	// Enables/disables heap allocation dynamically.
	void SetHeapAllocation( bool bNewState = true ) { m_bAllowHeapAllocation = bNewState; }

public:
	/// Compare string contents with a given C-string (optionally using length).
	bool Compare( const char *pString, int nLen ) const { return ( nLen < 0 ? V_strcmp( String(), pString ) : V_strncmp( String(), pString, nLen ) ) == 0; }
	bool Compare( const CUtlString &str ) const { return Compare( str.String(), str.Length() ); }
	bool Compare( const CBufferString &buf ) const { return Compare( buf.String(), buf.Length() ); }
	bool Compare( std::string_view view ) const { return Compare( view.data(), static_cast<int>(view.length()) ); }

	/// operator==
	/// Tests for equality, both are case sensitive.
	bool operator==( const char *pString ) const { return Compare( pString, -1 ); }
	bool operator==( const CUtlString &str ) const { return Compare( str ); }
	bool operator==( const CBufferString &buf ) const { return Compare( buf ); }
	bool operator==( std::string_view view ) const { return Compare( view ); }

	/// operator!=
	bool operator!=( const char *pString ) const { return !operator==( pString ); }
	bool operator!=( const CUtlString &str ) const { return !operator==( str ); }
	bool operator!=( const CBufferString &buf ) const { return !operator==( buf ); }
	bool operator!=( std::string_view view ) const { return !operator==( view ); }

	/// operator[]
	char &operator[]( int elem ) { Assert( 0 <= elem && elem < Length() && elem < AllocatedSize() ); return Base()[elem]; }
	char operator[]( int elem ) const { return const_cast<CBufferString *>(this)->operator[]( elem ); }

	// Inserts the nCount bytes of data from pBuf buffer at nIndex position.
	// If nCount is -1, it would count the bytes of the input buffer manualy.
	// Returns the resulting char buffer (Same as to what CBufferString::String() returns).
	DLL_CLASS_IMPORT const char *Insert( int nIndex, const char *pString, int nCount = -1, bool bIgnoreAlignment = false );

	/// Prepares the bufferstring for writing new characters.
	DLL_CLASS_IMPORT char *GetInsertPtr( int nIndex, int nCharsNedded, bool bIgnoreAlignment = false, int *pNewCapacity = nullptr );
	DLL_CLASS_IMPORT char *GetReplacePtr( int nIndex, int nOldChars, int nCharsNedded, bool bIgnoreAlignment = false, int *pNewCapacity = nullptr );

	// Allocates additional space.
	DLL_CLASS_IMPORT int GrowByChunks( int nChunkSize, int nGrowSize = -1 );

	/// Appends multiple strings into one (e.g., {"Hello", " ", "World"} -> "Hello World"_bs)
	DLL_CLASS_IMPORT const char *AppendConcat( int nCount, const char * const *pStrings, const int *pLengths = nullptr, bool bIgnoreAlignment = false );
	template< size_t N > FORCEINLINE const char *AppendConcatN( const char *(&strs)[N], const int (&nLengths)[N] ) { return AppendConcat( N, strs, nLengths ); }
	template< size_t N > FORCEINLINE const char *AppendConcatN( const char *(&strs)[N] ) { return AppendConcat( N, strs ); }

	/// Appends formatted C-string using printf-style formatting.
	DLL_CLASS_IMPORT int AppendFormat( const char *pFormat, ... ) FMTFUNCTION(2, 3);
	DLL_CLASS_IMPORT int AppendFormatV( const char *pFormat, va_list paramList );

	// Append repeated character N times.
	DLL_CLASS_IMPORT const char *AppendRepeat( char cChar, int nTimes, bool bIgnoreAlignment = false );

	// Given a path and a filename, composes "path\filename", inserting the (OS correct) separator if necessary
	DLL_CLASS_IMPORT const char *ComposeFileName( const char *pPath, const char *pFile, char cSeparator );

	// Converts wide characters or UTF-32 characters to buffer.
	DLL_CLASS_IMPORT const char *ConvertIn( unsigned int const *pData, int nSize, bool bIgnoreAlignment = false );
	DLL_CLASS_IMPORT const char *ConvertIn( wchar_t const *pData, int nSize, bool bIgnoreAlignment = false );

	// Makes path end with extension if it doesn't already have an extension
	DLL_CLASS_IMPORT const char *DefaultExtension( const char *pExtension );

	/// Check if string ends with a given suffix (case-sensitive and fast-insensitive versions)
	DLL_CLASS_IMPORT bool EndsWith( const char *pSuffix ) const;
	DLL_CLASS_IMPORT bool EndsWith_FastCaseInsensitive( const char *pSuffix ) const;

	/// Ensures capacity for N bytes (returns pointer and actual capacity)
	/// Ensures the nCapacity condition is met and grows the local buffer if needed.
	/// Returns pResultingBuffer pointer to the newly allocated data, as well as resulting capacity that was allocated in bytes.
	DLL_CLASS_IMPORT int EnsureCapacity( int nCapacity, char **pResultingBuffer, bool bIgnoreAlignment = false, bool bForceGrow = false );
	DLL_CLASS_IMPORT int EnsureAddedCapacity( int nCapacity, char **pResultingBuffer, bool bIgnoreAlignment = false, bool bForceGrow = false );
	DLL_CLASS_IMPORT char *EnsureLength( int nCapacity, bool bIgnoreAlignment = false, int *pNewCapacity = nullptr );

	// Ensures buffer is using owned heap allocation.
	DLL_CLASS_IMPORT char *EnsureOwnedAllocation( CBufferString::EAllocationOption_t eAlloc );

	// Ensures trailing slash character at end of string.
	DLL_CLASS_IMPORT const char *EnsureTrailingSlash( char cSeparator, bool bDontAppendIfEmpty = true );

	// Expands current path with additional directory (with separator).
	DLL_CLASS_IMPORT const char *ExtendPath( const char *pPath, char cSeparator );

	// Extract filename without extension.
	DLL_CLASS_IMPORT const char *ExtractFileBase( const char *pPath );

	// Copies out the file extension into dest
	DLL_CLASS_IMPORT const char *ExtractFileExtension( const char *pPath );

	// Copies out the path except for the stuff after the final pathseparator
	DLL_CLASS_IMPORT const char *ExtractFilePath( const char *pPath, bool bIgnoreEmptyPath = false );
	DLL_CLASS_IMPORT const char *ExtractFirstDir( const char *pPath );
	DLL_CLASS_IMPORT const char *FixSlashes( char cSeparator = CORRECT_PATH_SEPARATOR ); // Force slashes of either type to be = separator character.

	// Fix up a filename, removing dot slashes, fixing slashes, converting to lowercase, etc.
	DLL_CLASS_IMPORT const char *FixupPathName( char cSeparator );

	/// Classic C-like formatting into the bufferstring.
	DLL_CLASS_IMPORT int Format( const char *pFormat, ... ) FMTFUNCTION(2, 3);
	DLL_CLASS_IMPORT void FormatTo( IFormatOutputStream* pOutputStream, CFormatStringElement element ) const;

	// Returns aligned size based on capacity requested.
	DLL_CLASS_IMPORT static int GetAllocChars( int nSize, int nCapacity );

	// If pPath is a relative path, this function makes it into an absolute path
	// using the current working directory as the base, or pStartingDir if it's non-NULL.
	// Returns NULL if it runs out of room in the string, or if pPath tries to ".." past the root directory.
	DLL_CLASS_IMPORT const char *MakeAbsolutePath( const char *pPath, const char *pStartingDir );

	// Make fixed absolute path and normalize separators.
	// Same as FixupPathName, but also does separator fixup.
	DLL_CLASS_IMPORT const char *MakeFixedAbsolutePath( const char *pPath, const char *pStartingDir, char cSeparator = CORRECT_PATH_SEPARATOR );
	
	// Creates a relative path given two full paths
	// The first is the full path of the file to make a relative path for.
	// The second is the full path of the directory to make the first file relative to
	// Returns NULL if they can't be made relative (on separate drives, for example)
	DLL_CLASS_IMPORT const char *MakeRelativePath( const char *pFullPath, const char *pDirectory );

	// Transfers data from buffer and then purges it.
	DLL_CLASS_IMPORT void MoveFrom( CBufferString &moveFrom );

	// Purge (heap) memory, optionally preserving some capacity.
	DLL_CLASS_IMPORT void Purge( int nAllocatedBytesToPreserve = 0 );

	// Relinquishes internal pointer and transfer ownership (for utlfriends).
	DLL_CLASS_IMPORT char *Relinquish( CBufferString::EAllocationOption_t eAlloc );

	/// Removes characters at specific index.
	DLL_CLASS_IMPORT const char *RemoveAt( int nIndex, int nChars );
	DLL_CLASS_IMPORT const char *RemoveAtUTF8( int nByteIndex, int nCharacters );

	// Removes dot slashes (also handling 'ugc:' & 'vpk:') in path.
	DLL_CLASS_IMPORT const char *RemoveDotSlashes( char cSeparator );

	// Removes whitespaces (including '\t', '\n', '\v', '\f' & '\r') from string.
	DLL_CLASS_IMPORT int RemoveWhitespace();

	/// Removes full file path portion from path
	DLL_CLASS_IMPORT const char *RemoveFilePath();
	DLL_CLASS_IMPORT const char *RemoveFirstDir( CBufferString *pRemovedDir = nullptr );
	DLL_CLASS_IMPORT const char *RemoveToFileBase();

	/// Fixes broken UTF8 encoding at the end of string
	DLL_CLASS_IMPORT bool RemovePartialUTF8Tail( bool bDefault = false );
	DLL_CLASS_IMPORT const char *RemoveTailUTF8( int nIndex );

	/// Replaces characters or substrings.
	DLL_CLASS_IMPORT int Replace( char cFrom, char cTo );
	DLL_CLASS_IMPORT int Replace( const char *pMatch, const char *pToString, bool bDontUseStrStr = false );

	/// Replace contents at given index with new string.
	DLL_CLASS_IMPORT const char *ReplaceAt( int nStartIndex, int nOldChars, const char *pToString, int nLen = -1, bool bIgnoreAlignment = false );
	DLL_CLASS_IMPORT const char *ReplaceAt( int nStartIndex, const char *pToString, int nLen = -1, bool bIgnoreAlignment = false );

	DLL_CLASS_IMPORT const char *ReverseChars( int nStartIndex, int nChars );

	// Strips any current extension from path and ensures that extension is the new extension.
	DLL_CLASS_IMPORT const char *SetExtension( const char *pString );

	// Adjusts string length manually.
	DLL_CLASS_IMPORT char *SetLength( int nLength, bool bIgnoreAlignment = false, int *pNewCapacity = nullptr );

	// Sets internal pointer manually (used in special cases).
	DLL_CLASS_IMPORT void SetPtr( char *pBuf, int nBufferChars, int nOldLength = 0, bool bFreeHeap = false, bool bHeapAllocation = false );

	// Marks buffer as unusable and destroy contents.
	// Frees the bufferstring (if it was heap allocated) and writes "~DSTRCT" to the local buffer.
	DLL_CLASS_IMPORT void SetUnusable();

	// Shorten path by removing last component.
	DLL_CLASS_IMPORT const char *ShortenPath( bool bRecusriveEnsure = false );

	/// Check whether string starts with specified prefix.
	DLL_CLASS_IMPORT bool StartsWith( const char *pPrefix ) const;
	DLL_CLASS_IMPORT bool StartsWith_FastCaseInsensitive( const char *pPrefix ) const;

	/// Same thing as Format/FormatV, but returns C-string.
	DLL_CLASS_IMPORT const char *StrFormat( const char *pFormat, ... ) FMTFUNCTION(2, 3);
	DLL_CLASS_IMPORT const char *StrAppendFormat( const char *pFormat, ... ) FMTFUNCTION(2, 3);

	// Strips extension from filename.
	DLL_CLASS_IMPORT const char *StripExtension();

	// Remove trailing path separator.
	DLL_CLASS_IMPORT const char *StripTrailingSlash();

	/// Convert string to lowercase/uppercase (fast variants).
	DLL_CLASS_IMPORT void ToLowerFast( int nStart = 0 );
	DLL_CLASS_IMPORT void ToUpperFast( int nStart = 0 );
	void ToLower( int nStart = 0 ) { ToLowerFast( nStart ); }
	void ToUpper( int nStart = 0 ) { ToUpperFast( nStart ); }

	/// Trims specified characters from both sides / head / tail.
	DLL_CLASS_IMPORT const char *Trim( const char *pTrimChars = "\t\r\n " );
	DLL_CLASS_IMPORT const char *TrimHead( const char *pTrimChars = "\t\r\n " );
	DLL_CLASS_IMPORT const char *TrimTail( const char *pTrimChars = "\t\r\n " );

	/// Truncates string at given index or substring.
	DLL_CLASS_IMPORT const char *TruncateAt( int nIndex, bool bIgnoreAlignment = false );
	DLL_CLASS_IMPORT const char *TruncateAt( const char *pStr, bool bIgnoreAlignment = false );

	// Performs unicode case conversion with error policy.
	DLL_CLASS_IMPORT int UnicodeCaseConvert( int nFlags, EStringConvertErrorPolicy eErrorPolicy );

private:
	union
	{
		struct
		{
			uint32 m_nLength : 30; // String length (30 bits).
			uint32 m_bOverflowed : 1; // Buffer overflow flag (+1 bit).
			uint32 m_bFreeHeap : 1; // Whether heap memory should be freed (+1 bit).
		};

		uint32 m_nLengthStaff; // To copy/move one.
	};

	union
	{
		struct
		{
			uint32 m_nAllocatedSize : 30; // Allocated buffer size: how many bytes are available in the allocated data (stack/heap; 30 bits).
			uint32 m_bStackAllocated : 1; // Means it uses stack allocated buffer (+1 bit).
			uint32 m_bAllowHeapAllocation : 1; // Allows the bufferstring to grow beyond the static size on the heap (+1 bit).
		};

		uint32 m_nAllocatedStaff; // To copy/move one.
	};

protected:
	union Data_t
	{
		Data_t( char *pString ) : m_pString( pString ) {}
		Data_t() : m_szString{} {}

		char *m_pString; // Heap allocation buffer.
		char m_szString[8]; // Stack allocation buffer + parental class.
	} m_Buffer;

	// Constants for buffer size and alignment.
	static constexpr size_t DATA_SIZE = sizeof( Data_t );
	static constexpr size_t CHUNK_SIZE = sizeof( Data_t );
	static constexpr size_t DEFAULT_CAPACITY_SIZE = 256;
};

//TODO: class CBufferStringFormatOutputStream : public CBufferString, public IFormatOutputStream

// CBufferStringN - templated version of CBufferString with custom stack size 
// Allows predefining larger buffers without heap allocation by default.
template< size_t SIZE >
class CBufferStringN : public CBufferString
{
public:
	using BaseClass = CBufferString;
	using BaseClass::BaseClass;

	static constexpr size_t MIN_DATA_SIZE = BaseClass::DATA_SIZE;
	static_assert( SIZE >= MIN_DATA_SIZE, "Incorrect SIZE, should be >= 8" );
	static constexpr size_t DATA_SIZE = ALIGN_VALUE( SIZE - MIN_DATA_SIZE, BaseClass::CHUNK_SIZE );

	CBufferStringN( bool bAllowHeapAllocation = true ) : CBufferString( SIZE, bAllowHeapAllocation ) {}
	CBufferStringN( const char *pString, int nLen, bool bAllowHeapAllocation = true ) : CBufferStringN( bAllowHeapAllocation ) { Set( pString, nLen ); }
	template< size_t N > CBufferStringN( const char (&str)[N], bool bAllowHeapAllocation = true ) : CBufferStringN( str, N - 1, bAllowHeapAllocation ) {}
	explicit CBufferStringN( std::string_view view, bool bAllowHeapAllocation = true ) : CBufferStringN( view.data(), static_cast<int>(view.length()), bAllowHeapAllocation ) {}

	CBufferStringN( const CBufferStringN< SIZE > &copyFrom ) : CBufferString( copyFrom ), m_FixedBuffer( copyFrom.m_FixedBuffer ) {}
	CBufferStringN( CBufferStringN< SIZE > &&moveFrom ) noexcept : CBufferString( Move( moveFrom ) ), m_FixedBuffer( Move( moveFrom.m_FixedBuffer ) ) {}

	/// Concat-based constructors (2).
	template< size_t N > FORCEINLINE CBufferStringN( const char *pPrefix, const char *(&strs)[N], const int (&nLengths)[N], bool bAllowHeapAllocation = true ) : 
	    CBufferStringN( pPrefix, -1, bAllowHeapAllocation )
	{
		AppendConcatN( strs, nLengths );
	}
	template< size_t N > FORCEINLINE CBufferStringN( const char *pPrefix, const char *(&strs)[N], bool bAllowHeapAllocation = true ) : 
	    CBufferStringN( pPrefix, -1, bAllowHeapAllocation )
	{
		AppendConcatN( strs );
	}
	template< size_t N > FORCEINLINE CBufferStringN( const char *(&strs)[N], bool bAllowHeapAllocation = true ) : 
	    CBufferStringN( bAllowHeapAllocation )
	{
		AppendConcatN( strs );
	}
	CBufferStringN( std::initializer_list< const char * > strs, bool bAllowHeapAllocation = true ) : 
	    CBufferStringN( bAllowHeapAllocation )
	{
		AppendConcat( strs.size(), strs.begin() );
	}

	CBufferStringN< SIZE > &operator=( const CBufferStringN< SIZE > &copyFrom )
	{
		BaseClass::operator=( copyFrom );
		m_FixedBuffer = copyFrom.m_FixedBuffer;

		return *this;
	}

	template< size_t N > CBufferStringN< SIZE > &operator=( CBufferStringN< N > &&moveFrom ) noexcept
	{
		BaseClass::operator=( Move( moveFrom ) );

		return *this;
	}

	// ~CBufferString() { BaseClass::Purge( DATA_SIZE ); } // virtual destructor?

private:
	union Data_t
	{
		char m_szString[DATA_SIZE];
	} m_FixedBuffer;
};

using CTinyBufferString    = CBufferStringN<16>;
using CSmallBufferString   = CBufferStringN<32>;
using CMediumBufferString  = CBufferStringN<64>;
using CLargeBufferString   = CBufferStringN<128>;
using CXLargeBufferString  = CBufferStringN<256>;
using CHugeBufferString    = CBufferStringN<512>;
using CMassiveBufferString = CBufferStringN<1024>;

// AMNOTE: CBufferStringN name is preferred to be used, altho CBufferStringGrowable is left as a small bcompat
template< size_t SIZE >
using CBufferStringGrowable = CBufferStringN< SIZE >;

// Literal operator for convenient inline CBufferString creation.
// To optimize on the stack, use CBufferStringN instead.
inline CBufferString operator""_bs( const char *pString, size_t nLen )
{
	return CBufferString( pString, static_cast<int>(nLen) );
}

#endif /* BUFFERSTRING_H */
