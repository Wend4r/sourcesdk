//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: Tools for grabbing/dumping the stack at runtime
//
// $NoKeywords: $
//=============================================================================//

#ifndef TIER0_STACKTOOLS_H
#define TIER0_STACKTOOLS_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/threadtools.h"

#if (defined( PLATFORM_WINDOWS ) || defined( PLATFORM_X360 )) && !defined( STEAM ) && !defined( _CERT ) && defined( TCHAR_IS_CHAR ) //designed for windows/x360, not built/tested with wide characters, not intended for release builds (but probably wouldn't damage anything)
#	define ENABLE_RUNTIME_STACK_TRANSLATION //uncomment to enable runtime stack translation tools. All of which use on-demand loading of necessary dll's and pdb's
#endif

#if defined( ENABLE_RUNTIME_STACK_TRANSLATION )
//#define ENABLE_THREAD_PARENT_STACK_TRACING 1 //uncomment to actually enable tracking stack traces from threads and jobs to their parent thread. Must also define THREAD_PARENT_STACK_TRACE_SUPPORTED in threadtools.h
#	if defined( ENABLE_THREAD_PARENT_STACK_TRACING )
#		define THREAD_PARENT_STACK_TRACE_LENGTH 32
#	endif
#endif




PLATFORM_INTERFACE int GetCallStack( void **pReturnAddressesOut, int iArrayCount, int iSkipCount );

//ONLY WORKS IF THE CRAWLED PORTION OF THE STACK DISABLES FRAME POINTER OMISSION (/Oy-)  "vpc /nofpo"
PLATFORM_INTERFACE int GetCallStack_Fast( void **pReturnAddressesOut, int iArrayCount, int iSkipCount );

PLATFORM_INTERFACE int Plat_GetRawCallStack( void **pReturnAddressesOut, int nArrayCount, int nSkipCount );

typedef int (*FN_GetCallStack)( void **pReturnAddressesOut, int iArrayCount, int iSkipCount );

//where we'll find our PDB's for win32.
PLATFORM_INTERFACE void SetStackTranslationSymbolSearchPath( const char *szSemicolonSeparatedList = NULL );
PLATFORM_INTERFACE void StackToolsNotify_LoadedLibrary( const char *szLibName );

//maximum output sample "tier0.dll!TranslateStackInfo - u:\Dev\L4D\src\tier0\stacktools.cpp(162) + 4 bytes"
enum TranslateStackInfo_StyleFlags_t
{
	TSISTYLEFLAG_NONE = 0,
	TSISTYLEFLAG_MODULENAME = (1<<0), //start with module			Sample: "tier0.dll!"
	TSISTYLEFLAG_SYMBOLNAME = (1<<1), //include the symbol name		Sample: "TranslateStackInfo"
	TSISTYLEFLAG_FULLPATH = (1<<2), //include full path				Sample: "u:\Dev\L4D\src\tier0\stacktools.cpp"
	TSISTYLEFLAG_SHORTPATH = (1<<3), //only include 2 directories	Sample: "\src\tier0\stacktools.cpp"
	TSISTYLEFLAG_LINE = (1<<4), //file line number					Sample: "(162)"
	TSISTYLEFLAG_LINEANDOFFSET = (1<<5), //file line + offset		Sample: "(162) + 4 bytes"
	TSISTYLEFLAG_LAST = TSISTYLEFLAG_LINEANDOFFSET,
	TSISTYLEFLAG_DEFAULT = (TSISTYLEFLAG_MODULENAME | TSISTYLEFLAG_SYMBOLNAME | TSISTYLEFLAG_FULLPATH | TSISTYLEFLAG_LINEANDOFFSET), //produces sample above
};

//Generates a formatted list of function information, returns number of translated entries
//On 360 this generates a string that can be decoded by VXConsole in print functions. Optimal path for translation because it's one way. Other paths require multiple transactions.
PLATFORM_INTERFACE int TranslateStackInfo( const void * const *pCallStack, int iCallStackCount, tchar *szOutput, int iOutBufferSize, const tchar *szEntrySeparator, TranslateStackInfo_StyleFlags_t style = TSISTYLEFLAG_DEFAULT );

PLATFORM_INTERFACE void PreloadStackInformation( void * const *pAddresses, int iAddressCount ); //caches data and reduces communication with VXConsole to speed up 360 decoding when using any of the Get***FromAddress() functions. Nop on PC.
PLATFORM_INTERFACE bool GetFileAndLineFromAddress( const void *pAddress, tchar *pFileNameOut, int iMaxFileNameLength, uint32 &iLineNumberOut, uint32 *pDisplacementOut = NULL );
PLATFORM_INTERFACE bool GetSymbolNameFromAddress( const void *pAddress, tchar *pSymbolNameOut, int iMaxSymbolNameLength, uint64 *pDisplacementOut = NULL );
PLATFORM_INTERFACE bool GetModuleNameFromAddress( const void *pAddress, tchar *pModuleNameOut, int iMaxModuleNameLength );



class PLATFORM_CLASS CCallStackStorage //a helper class to grab a stack trace as close to the leaf code surface as possible, then pass it on to deeper functions intact with less unpredictable inlining pollution
{
public:
	CCallStackStorage( FN_GetCallStack GetStackFunction = GetCallStack, uint32 iSkipCalls = 0 );
	CCallStackStorage( const CCallStackStorage &copyFrom )
	{
		iValidEntries = copyFrom.iValidEntries;
		// warning C6204: Possible buffer overrun in call to 'memcpy': use of unchecked parameter 'src'
		ANALYZE_SUPPRESS( 6204 )
		memcpy( pStack, copyFrom.pStack, sizeof( void * ) * copyFrom.iValidEntries );
	}

	void *pStack[128]; //probably too big, possibly too small for some applications. Don't want to spend the time figuring out how to generalize this without templatizing pollution or mallocs
	uint32 iValidEntries;
};


class CCallStackMarkerBase;
class CCallStackCapture;

//Hold onto one of these to denote the top of a functional stack trace. Also allows us to string together threads to their parents
class PLATFORM_CLASS CStackTop_Base
{
public:
	static CStackTop_Base *GetCurrentTop( bool bUnknown );
	void ChangeMarker( const CCallStackMarkerBase &marker );

protected:
	CStackTop_Base();
	~CStackTop_Base();

#if defined( ENABLE_RUNTIME_STACK_TRANSLATION )
	CStackTop_Base *m_pPrevTop;
	void *m_pStackBase;
	void *m_pReplaceAddress;

	void * const *m_pParentStackTrace;
	int m_iParentStackTraceLength;
#endif
};

//makes a copy of the parent stack
class PLATFORM_CLASS CStackTop_CopyParentStack : public CStackTop_Base
{
public:
	CStackTop_CopyParentStack( const void * const *pParentStackTrace, int iParentStackTraceLength, const CCallStackMarkerBase &marker );
	CStackTop_CopyParentStack( const CCallStackCapture *pParentStack, const CCallStackMarkerBase &marker );
	~CStackTop_CopyParentStack( void );

	void ChangeCopiedStack( const void * const *pParentStackTrace, int iParentStackTraceLength );
};

//just references the parent stack. Assuming that you'll keep that memory around as long as you're keeping this Stack Top marker.
class PLATFORM_CLASS CStackTop_ReferenceParentStack : public CStackTop_Base
{
public:
	CStackTop_ReferenceParentStack( const void * const *pParentStackTrace, int iParentStackTraceLength, const CCallStackMarkerBase &marker );
	CStackTop_ReferenceParentStack( const CCallStackCapture *pParentStack, const CCallStackMarkerBase &marker );
	~CStackTop_ReferenceParentStack( void );

	void ChangeReferencedStack( const void * const *pParentStackTrace, int iParentStackTraceLength );
	void ReleaseParentStackReferences( void ); //in case you need to delete the parent stack trace before this class goes out of scope
};


//-----------------------------------------------------------------------------
// Call stack markers, captured stacks are cut at the frames that hold an enabled marker
//-----------------------------------------------------------------------------
class CCallStackMarkerBase
{
public:
	DLL_CLASS_IMPORT bool TestMarkerAgainstCapturedStackIndex( const void * const *pCapturedStack, int nCapturedStackLength, int nTestIndex ) const;

protected:
	void *m_pMarker;
	uintp m_nRecursionDepth;
};

class CCallStackMarker : public CCallStackMarkerBase
{
public:
	DLL_CLASS_IMPORT CCallStackMarker( bool bEnableMarker = true );
};

class CRecursionCallStackMarker : public CCallStackMarkerBase
{
public:
	DLL_CLASS_IMPORT CRecursionCallStackMarker( bool bEnableMarker = true );
};

class CDeferredCallStackMarker : public CCallStackMarkerBase
{
public:
	DLL_CLASS_IMPORT void Insert_NoStackCheck();
	DLL_CLASS_IMPORT void Remove();
};

class CCallStackMarker_SmallerScope : public CCallStackMarkerBase
{
};

class CCallStackMarker_ThisLine : public CCallStackMarker_SmallerScope
{
public:
	DLL_CLASS_IMPORT CCallStackMarker_ThisLine( bool bEnableMarker = true );
};


#define FOR_EACH_CALLSTACK_FRAME( capture, iter ) \
	for ( int iter = 0; iter < ( int )( capture ).nValidEntries; iter++ )
#define FOR_EACH_CALLSTACK_FRAME_BACK( capture, iter ) \
	for ( int iter = ( int )( capture ).nValidEntries - 1; iter >= 0; iter-- )

//-----------------------------------------------------------------------------
// Captured call stack
//-----------------------------------------------------------------------------
class CCallStackCapture_Base
{
public:
	uint32 nValidEntries;
	void *pStack[ 128 ];
};

class CCallStackCapture : public CCallStackCapture_Base
{
public:
	CCallStackCapture() { nValidEntries = 0; }

	// Copies the first nCount frames of ppFrames, up to the capacity
	CCallStackCapture( const void *const *ppFrames, uint32 nCount )
	{
		nValidEntries = MIN( nCount, ( uint32 )ARRAYSIZE( pStack ) );
		memcpy( pStack, ppFrames, nValidEntries * sizeof( void * ) );
	}

	CCallStackCapture( const CCallStackCapture *pOther )
	{
		nValidEntries = pOther ? pOther->nValidEntries : 0;

		if ( nValidEntries )
			memcpy( pStack, pOther->pStack, nValidEntries * sizeof( void * ) );
	}

	// The stack function of tier0, which takes the thread to capture first, unlike the global FN_GetCallStack
	typedef int (*FN_GetCallStack)( ThreadId_t nThreadId, void **pReturnAddressesOut, int nArrayCount, int nSkipCount );

	DLL_CLASS_IMPORT CCallStackCapture( FN_GetCallStack GetStackFunction, uint32 nSkipCalls, uint32 nMaxCapture );
	DLL_CLASS_IMPORT void Capture( FN_GetCallStack GetStackFunction, uint32 nSkipCalls, uint32 nMaxCapture );
};


//-----------------------------------------------------------------------------
// Encodes the function, file and line of a script virtual machine frame as a native pointer,
// so that captured call stacks can hold and translate them
//-----------------------------------------------------------------------------
class CVirtualMachineCallStackTranslationEncoder;
struct VirtualMachineCallStackTranslationEncoderInternalData_t;

typedef bool (*FN_VMEncodingIndexEnumerationCallback)( CVirtualMachineCallStackTranslationEncoder *pEncoder, uint32 nIndex, const char *szFunctionName, const char *szFileName, uint32 nFileLine, void *pContext );

class CVirtualMachineCallStackTranslationEncoder
{
public:
	DLL_CLASS_IMPORT CVirtualMachineCallStackTranslationEncoder( const char *szVirtualMachineName );
	DLL_CLASS_IMPORT ~CVirtualMachineCallStackTranslationEncoder();

	DLL_CLASS_IMPORT void *EncodeAsNativePointer( const char *szFunctionName, const char *szFileName, uint32 nFileLine );
	DLL_CLASS_IMPORT uint32 DecodeFromNativePointer( const void *pEncoding, CBufferString *pFunctionNameOut, CBufferString *pFileNameOut, uint32 *pFileLineOut );

	DLL_CLASS_IMPORT uint64 GetVMID();
	DLL_CLASS_IMPORT const char *GetName();
	DLL_CLASS_IMPORT uint64 GetMappingBaseAddress();
	DLL_CLASS_IMPORT uint32 GetMappingSize();

	DLL_CLASS_IMPORT uint32 GetCurrentIndexCount();
	DLL_CLASS_IMPORT uint64 GetIndexBitMask();
	DLL_CLASS_IMPORT uint32 GetIndexBitShift();
	DLL_CLASS_IMPORT uint64 GetLineOffsetBitMask();
	DLL_CLASS_IMPORT uint32 GetLineOffsetBitShift();
	DLL_CLASS_IMPORT uint64 GetLineOffsetPostShiftedSignBitMask();

	DLL_CLASS_IMPORT void EnumerateEncodingIndices( FN_VMEncodingIndexEnumerationCallback CallBackFunc, void *pContext );

private:
	CVirtualMachineCallStackTranslationEncoder *m_pNext;
	VirtualMachineCallStackTranslationEncoderInternalData_t *m_pInternalData;

	CVirtualMachineCallStackTranslationEncoder( const CVirtualMachineCallStackTranslationEncoder & ) = delete;
	CVirtualMachineCallStackTranslationEncoder &operator=( const CVirtualMachineCallStackTranslationEncoder & ) = delete;
};


//Encodes data so that every byte's most significant bit is a 1. Ensuring no null terminators.
//This puts the encoded data in the 128-255 value range. Leaving all standard ascii characters for control.
//Returns string length (not including the written null terminator as is standard). 
//Or if the buffer is too small. Returns negative of necessary buffer size (including room needed for null terminator)
PLATFORM_INTERFACE int EncodeBinaryToString( const void *pToEncode, int iDataLength, char *pEncodeOut, int iEncodeBufferSize );

//Decodes a string produced by EncodeBinaryToString(). Safe to decode in place if you don't mind trashing your string, binary byte count always less than string byte count.
//Returns:
//	>= 0 is the decoded data size
//	INT_MIN (most negative value possible) indicates an improperly formatted string (not our data)
//	all other negative values are the negative of how much dest buffer size is necessary.
PLATFORM_INTERFACE int DecodeBinaryFromString( const char *pString, void *pDestBuffer, int iDestBufferSize, char **ppParseFinishOut = NULL );




// 360<->VXConsole specific communication definitions
#define XBX_CALLSTACKDECODEPREFIX ":CSDECODE["

enum StackTranslation_BinaryHandler_Command_t
{
	ST_BHC_LOADEDLIBARY,
	ST_BHC_GETTRANSLATIONINFO,
};

#pragma pack(push)
#pragma pack(1)
struct FullStackInfo_t
{
	const void *pAddress;
	char szModuleName[24];
	char szFileName[MAX_PATH/2];
	char szSymbol[64];
	uint32 iLine;
	uint32 iSymbolOffset;

};
#pragma pack(pop)

//-----------------------------------------------------------------------------
// The modules the stack translation knows about. The loaders report modules as they map and
// unmap them; on Windows the stack tools ask the system instead and these do nothing
//-----------------------------------------------------------------------------
struct StackTranslation_LoadedModuleInfo_t;
class CVirtualMachineCallStackTranslationEncoder;
class IModuleMappingListener;

PLATFORM_INTERFACE void StackToolsNotify_BeginLoadingModule();
PLATFORM_INTERFACE void StackToolsNotify_EndLoadingModule();
PLATFORM_INTERFACE void StackToolsNotify_LoadedModule( const char *szModuleName, PlatModule_t hModule, void *pBaseAddress, uint32 nSize );
PLATFORM_INTERFACE void StackToolsNotify_UnloadedModule( PlatModule_t hModule );
PLATFORM_INTERFACE void StackToolsNotify_SetSymbolSearchPath( const char *szSemicolonSeparatedList );

// Calls the callback for the known modules and the virtual machine encoders
typedef bool ( *FN_ModuleEnumerationCallback )( const StackTranslation_LoadedModuleInfo_t *, uint32, void *pContext );
PLATFORM_INTERFACE uint32 StackTools_EnumerateKnownLoadedModules( FN_ModuleEnumerationCallback CallBackFunc, void *pContext );

typedef bool ( *FN_VMEncoderEnumerationCallback )( CVirtualMachineCallStackTranslationEncoder *pEncoder, void *pContext );
PLATFORM_INTERFACE void StackTools_EnumerateVirtualMachineEncoders( FN_VMEncoderEnumerationCallback CallBackFunc, void *pContext );

PLATFORM_INTERFACE void StackTools_InstallModuleMappingListener( IModuleMappingListener *pListener );
PLATFORM_INTERFACE void StackTools_RemoveModuleMappingListener( IModuleMappingListener *pListener );
PLATFORM_INTERFACE void StackTools_VerifyLoadedModuleMappings();

// Module, symbol and source line of an address, and the same for a call that was inlined into
// it; tier0 has no data for them in CS2, both only empty the outputs and return false
PLATFORM_INTERFACE bool GetAddressTranslationData( const void *pAddress, CBufferString *pModuleNameOut, CBufferString *pSymbolNameOut, CBufferString *pFileNameOut, uint32 *pLineNumberOut, uint64 *pSymbolDisplacementOut, uint64 *pLineDisplacementOut );
PLATFORM_INTERFACE bool GetAddressInlineCallData( const void *pAddress, uint32 nInlineIndex, CBufferString *pModuleNameOut, CBufferString *pSymbolNameOut, CBufferString *pFileNameOut, uint32 *pLineNumberOut, uint64 *pSymbolDisplacementOut, uint64 *pLineDisplacementOut );

class IStack_Annotation;
class IStack_VM_Segment_Marker;

// The markers of the stack of the current thread, to move them to another one and back
struct StackMarkupState_t
{
	StackMarkupState_t() { Clear(); }

	void Clear()
	{
		pStackTop = NULL;
		pDeferredMarkers = NULL;
		pStackAnnotation = NULL;
		pStackVMSegmentMarker = NULL;
	}

	CStackTop_Base *pStackTop;
	CCallStackMarkerBase *pDeferredMarkers;
	IStack_Annotation *pStackAnnotation;
	IStack_VM_Segment_Marker *pStackVMSegmentMarker;
};

PLATFORM_INTERFACE void StackMarkup_SaveState( StackMarkupState_t &saveDest );
PLATFORM_INTERFACE void StackMarkup_RestoreState( const StackMarkupState_t &restoreSource );

// Calls the callback with each annotation of a captured stack: the address, its index and the text
typedef bool ( *FN_IterateStackAnnotationCallback )( const void *pAddress, int nIndex, const char *pszAnnotation, void *pContext );
PLATFORM_INTERFACE void IterateCapturedStackAnnotations( const void *const *pCapturedStack, int nCapturedCount, FN_IterateStackAnnotationCallback pCallback, void *pContext );

#endif //#ifndef TIER0_STACKTOOLS_H
