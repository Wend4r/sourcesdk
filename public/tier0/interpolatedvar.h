//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef INTERPOLATEDVAR_H
#define INTERPOLATEDVAR_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/utllinkedlist.h"
#include "tier1/rangecheckedvar.h"
#include "tier1/lerp_functions.h"
#include "tier0/convar.h"
#include "tier1/utldelegate.h"
#include "entity2/entityindex.h"

// Define this to have it measure whether or not the interpolated entity list
// is accurate.
//#define INTERPOLATEDVAR_PARANOID_MEASUREMENT

#define LATCH_ANIMATION_VAR  (1<<0)		// use AnimTime as sample basis
#define LATCH_SIMULATION_VAR (1<<1)		// use SimulationTime as sample basis

#define EXCLUDE_AUTO_LATCH			(1<<2)
#define EXCLUDE_AUTO_INTERPOLATE	(1<<3)

#define INTERPOLATE_LINEAR_ONLY		(1<<4)	// don't do hermite interpolation
#define INTERPOLATE_OMIT_UPDATE_LAST_NETWORKED (1<<5)

#define EXTRA_INTERPOLATION_HISTORY_STORED 0.05f	// It stores this much extra interpolation history,
													// so you can always call Interpolate() this far
													// in the past from your last call and be able to 
													// get an interpolated value.

// this global keeps the last known server packet tick (to avoid calling engine->GetLastTimestamp() all the time)
extern float g_flLastPacketTimestamp;
// when we renormalize hermite spline samples (on the time axis), determines whether we hold the previous sample at a fixed up time or lerp the sample as well as 
//  fix up the time (to get two even time intervals)
extern bool g_bHermiteFix;

inline void Interpolation_SetLastPacketTimeStamp( float timestamp )
{
	Assert( timestamp > 0 );
	g_flLastPacketTimestamp = timestamp;
}


// Before calling Interpolate(), you can use this use this to setup the context if 
// you want to enable extrapolation.
class CInterpolationContext
{
public:
	
	CInterpolationContext()
	{
		m_bOldAllowExtrapolation = s_bAllowExtrapolation;
		m_flOldLastTimeStamp = s_flLastTimeStamp;

		// By default, disable extrapolation unless they call EnableExtrapolation.
		s_bAllowExtrapolation = false;

		// this is the context stack
		m_pNext = s_pHead;
		s_pHead = this;
	}
	
	~CInterpolationContext()
	{
		// restore values from prev stack element
		s_bAllowExtrapolation = m_bOldAllowExtrapolation;
		s_flLastTimeStamp = m_flOldLastTimeStamp;

		Assert( s_pHead == this );
		s_pHead = m_pNext;
	}

	static void EnableExtrapolation(bool state)
	{
		s_bAllowExtrapolation = state;
	}

	static bool IsThereAContext()
	{
		return s_pHead != NULL;
	}

	static bool IsExtrapolationAllowed()
	{
		return s_bAllowExtrapolation;
	}

	static void SetLastTimeStamp(float timestamp)
	{
		s_flLastTimeStamp = timestamp;
	}
	
	static float GetLastTimeStamp()
	{
		return s_flLastTimeStamp;
	}


private:

	CInterpolationContext *m_pNext;
	bool m_bOldAllowExtrapolation;
	float m_flOldLastTimeStamp;

	static CInterpolationContext *s_pHead;
	static bool s_bAllowExtrapolation;
	static float s_flLastTimeStamp;
};



template< class T >
inline T ExtrapolateInterpolatedVarType( const T &oldVal, const T &newVal, float divisor, float flExtrapolationAmount )
{
	return newVal;
}

inline Vector ExtrapolateInterpolatedVarType( const Vector &oldVal, const Vector &newVal, float divisor, float flExtrapolationAmount )
{
	return Lerp( 1.0f + flExtrapolationAmount * divisor, oldVal, newVal );
}

inline float ExtrapolateInterpolatedVarType( const float &oldVal, const float &newVal, float divisor, float flExtrapolationAmount )
{
	return Lerp( 1.0f + flExtrapolationAmount * divisor, oldVal, newVal );
}

inline QAngle ExtrapolateInterpolatedVarType( const QAngle &oldVal, const QAngle &newVal, float divisor, float flExtrapolationAmount )
{
	return Lerp<QAngle>( 1.0f + flExtrapolationAmount * divisor, oldVal, newVal );
}


class CUtlString;

// The history an interpolated variable reads or writes: a variable keeps one history or two (the second exists
// only when the flags say so). The methods that take a phase return a default for a missing history.
enum InterpolatedVarPhase_t : int
{
	IV_PHASE_RENDERING = -1,
	IV_PHASE_CLIENT_AUTHORITATIVE = 0,
	IV_PHASE_SERVER_AUTHORITATIVE = 1,
	IV_PHASE_COUNT = 2,
};

// Whether a time passed to the interpolation is shifted back by the interpolation amount.
enum InterpolationBehavior_t : int
{
	OFFSET_TIME_BY_INTERPOLATION_AMOUNT = 0,
	USE_EXACT_TIME_SPECIFIED = 1,
};

enum InterpolationResult_t : int
{
	INTERPOLATION_HAS_FUTURE_SAMPLES = 0,
	INTERPOLATION_HAS_NO_MORE_FUTURE_SAMPLES = 1,
};

enum InterpolatedVarInterpolationTimeType_t : int
{
	INTERPOLATION_TIME_TYPE_NORMAL = 0,
	INTERPOLATION_TIME_TYPE_PREDICTABLES = 1,
	INTERPOLATION_TIME_TYPE_COUNT = 2,
};

enum InterpolatedVarSpewFlags_t : int
{
	INTERPOLATED_VAR_SPEW_NONE = 0,
	INTERPOLATED_VAR_SPEW_ALL_ENTRIES = 1,
	INTERPOLATED_VAR_OMIT_TARGET_TIME_ENTRY = 2,
};

// Why the write delegate of a procedural variable is called.
enum IVWriteOpType_t : int
{
	IV_WRITE_INTERPOLATING = 0,
	IV_WRITE_RESTORING_LAST_LATCHED = 1,
};

class IInterpolatedVar;

// -------------------------------------------------------------------------------------------------------------- //
// The history of one phase of an interpolated variable: a ring of entries (the change tick and the index of the
// value in the data array). Clearing it empties the ring (first element and count) and keeps the sizes.
// -------------------------------------------------------------------------------------------------------------- //
template < class T >
class CInterpolatedVarHistory
{
public:
	struct InterpolatedVarEntry_t
	{
		int32 m_nChangeTick;
		int16 m_nIndex;
		int16 m_nReserved;
	};

	InterpolatedVarEntry_t *m_pElements;
	uint32 m_firstElement : 6;
	uint32 m_nDataArrayCount : 6;
	uint32 m_bIsMemcopyable : 1;
	uint32 m_count : 6;
	uint32 m_maxElement : 6;
	uint32 m_growSize : 6;
	// -1 when the history is cleared.
	uint32 m_nFreeDataElements;
};

// -------------------------------------------------------------------------------------------------------------- //
// IInterpolatedVar interface.
//
// The history of a value and its interpolation. CInterpolatedVarProcedural< T > reads and writes the owner's value
// through two delegates (the procedural delegates); the last methods of the table serve those variables and the
// helper delegate that names the elements of an array.
// -------------------------------------------------------------------------------------------------------------- //

abstract_class IInterpolatedVar
{
public:
	virtual ~IInterpolatedVar() {}

	// Stores the low three bits of nFlags in the flags and allocates the value buffer for the maximum entry count;
	// pValue is not read.
	virtual void Setup( void *pValue, int nFlags ) = 0;
	// The interpolation amount of a history and the latch interval (in ticks) stored next to it.
	virtual void SetInterpolationAmount( InterpolatedVarPhase_t ivPhase, float flSeconds, int nLatchIntervalTickCount ) = 0;
	// 1/64 when the history does not exist.
	virtual float GetInterpolationAmount( InterpolatedVarPhase_t ivPhase ) const = 0;
	// Writes the newest entry of the history to the owner through the write delegate.
	virtual void RestoreLastSimulationValue( InterpolatedVarPhase_t ivPhase ) = 0;
	// Empties every history (through ClearHistory).
	virtual void Reset() = 0;
	// Named by what it does: empties one history.
	virtual void ClearHistory( InterpolatedVarPhase_t ivPhase ) = 0;
	// Computes the interpolated value into the value buffer; with an empty history the buffer gets the current
	// value. Calls GetContext when a derived class overrides it. The game reads one byte of sideEffectFlags.
	virtual InterpolationResult_t Interpolate( InterpolatedVarPhase_t ivPhase, float flTime, InterpolationBehavior_t behavior, uint32 sideEffectFlags ) = 0;
	// Whether the history has an entry later than flInterpolationTime (shifted by the interpolation amount unless
	// USE_EXACT_TIME_SPECIFIED).
	virtual bool ValueChangesAfterThisTime( InterpolatedVarPhase_t ivPhase, float flInterpolationTime, InterpolationBehavior_t behavior ) const = 0;
	// Copies the histories of another variable of the same type.
	virtual void Copy( IInterpolatedVar *pInSrc ) = 0;
	// The name of an element; an empty string in every instantiation of the server.
	virtual CUtlString GetDebugName( int nElement ) const = 0;
	// The current value (read through the read delegate) as text.
	virtual CUtlString GetDebugValue() const = 0;
	// Pure here, empty in the procedural classes.
	virtual void SetArrayIndexNameHelperDelegate( const void *pArrayIndexNameHelper ) = 0;
	// Prints the history ("interp" lines).
	virtual void SpewInterpolatedVar( InterpolatedVarPhase_t ivPhase, float flTime, InterpolationBehavior_t behavior, InterpolatedVarSpewFlags_t nSpewFlags ) const = 0;
	// Stores the phase in three bits of the third flag byte and returns the previous one.
	virtual InterpolatedVarPhase_t SetInterpolatedVarContains( InterpolatedVarPhase_t ivPhase ) = 0;
	// True once Setup allocated the value buffer.
	virtual bool HasBeenSetup() const = 0;
	// Reads the current value through the read delegate and compares it with the interpolated history value.
	virtual bool DoesCurrentValueMatchInterpolatedValue( InterpolatedVarPhase_t ivPhase, float flInterpolationTime, InterpolationBehavior_t behavior ) const = 0;
	// True when the newest entry plus the interpolation amount is later than flInterpolationTime.
	virtual bool CheckIfInterpolationTimeIsBeforeFullInterpolationInterval( InterpolatedVarPhase_t ivPhase, float flInterpolationTime ) const = 0;
	// The size of the interpolated type.
	virtual int GetInterpolatedVarSize() const = 0;
	// Reads the value through the read delegate into pValue, returns GetInterpolatedVarSize().
	virtual int ReadCurrentValue( void *pValue ) const = 0;
	// Writes pValue through the write delegate, returns GetInterpolatedVarSize().
	virtual int WriteCurrentValue( void *pValue ) = 0;
	// True in the procedural instantiations.
	virtual bool IsProcedural() const = 0;
	// Returns HasReadWriteDelegates().
	virtual bool HasProceduralDelegates() const = 0;
	// The latch interval SetInterpolationAmount stores; 1 when the history does not exist.
	virtual int GetExpectedLatchInterval( InterpolatedVarPhase_t ivPhase ) const = 0;
	// Sets the high flag bit from bIsClientAuthoritative and switches between one and two histories, reallocating
	// them.
	virtual void SetInterpolationTimeType( bool bIsClientAuthoritative, InterpolatedVarInterpolationTimeType_t wivType ) = 0;
	// The prefix of the debug lines of the variable (its name and the phase) into pPrefix.
	virtual void ComputePrefixString( const char *pszPrefix, InterpolatedVarPhase_t ivPhase, CUtlString *pPrefix ) const = 0;
	// Prints a debug line with that prefix.
	virtual void PrefixSpewMsg( const char *pszPrefix, InterpolatedVarPhase_t ivPhase ) const = 0;
	// Where flTime lies between the two entries around it (0..1).
	virtual float ComputeInterpolationFrac( InterpolatedVarPhase_t ivPhase, float flTime, InterpolationBehavior_t behavior ) const = 0;
	// Returns -1.
	virtual int GetValueFixedWidth() const = 0;
	// Reads the current value through the read delegate and adds it to a history (through LatchChangedTick).
	virtual bool NoteChangedTick( InterpolatedVarPhase_t ivPhase, int nChangeTick ) = 0;
	// Adds pValue to a history.
	virtual bool LatchChangedTick( InterpolatedVarPhase_t ivPhase, const void *pValue, int nChangeTick ) = 0;
	// True when both procedural delegates are set.
	virtual bool HasReadWriteDelegates() const = 0;
	// The delegates that read and write the owner's value.
	virtual const void *GetReadDelegate() const = 0;
	virtual const void *GetWriteDelegate() const = 0;
	// Copies both delegates from pDelegates.
	virtual void SetProceduralDelegates( const void *pDelegates ) = 0;
	// False in the procedural classes.
	virtual bool HasArrayIndexHelperDelegate() const = 0;
	// A static empty delegate in the procedural classes.
	virtual const void *GetArrayIndexNameHelperDelegate() const = 0;
	// Null here.
	virtual void *GetContext() const = 0;
};

template < class T > class CGlobalInterpolationLerpFuncs;
template < class T, class OWNER > class COwnerInterpolationLerpFuncs;

// -------------------------------------------------------------------------------------------------------------- //
// CInterpolatedVarArrayBase< T, IS_ARRAY, IS_PROCEDURAL, LERP >: the implementation of IInterpolatedVar for one
// type. IS_PROCEDURAL is true for the variables that reach the owner's value through the procedural delegates
// (CInterpolatedVarProcedural, CInterpolatedVarProceduralUsingOwnerLerp); LERP is CGlobalInterpolationLerpFuncs< T >
// or COwnerInterpolationLerpFuncs< T, owner >. It adds no virtual method; it implements every method of the interface
// except SetArrayIndexNameHelperDelegate and the delegate accessors (HasReadWriteDelegates to
// GetArrayIndexNameHelperDelegate), which stay pure. The game implements them.
// -------------------------------------------------------------------------------------------------------------- //

template < typename Type, bool IS_ARRAY, bool IS_PROCEDURAL, class LERP >
abstract_class CInterpolatedVarArrayBase : public IInterpolatedVar
{
public:
	virtual void Setup( void *pValue, int nFlags ) override;
	virtual void SetInterpolationAmount( InterpolatedVarPhase_t ivPhase, float flSeconds, int nLatchIntervalTickCount ) override;
	virtual float GetInterpolationAmount( InterpolatedVarPhase_t ivPhase ) const override;
	virtual void RestoreLastSimulationValue( InterpolatedVarPhase_t ivPhase ) override;
	virtual void Reset() override;
	virtual void ClearHistory( InterpolatedVarPhase_t ivPhase ) override;
	virtual InterpolationResult_t Interpolate( InterpolatedVarPhase_t ivPhase, float flTime, InterpolationBehavior_t behavior, uint32 sideEffectFlags ) override;
	virtual bool ValueChangesAfterThisTime( InterpolatedVarPhase_t ivPhase, float flInterpolationTime, InterpolationBehavior_t behavior ) const override;
	virtual void Copy( IInterpolatedVar *pInSrc ) override;
	virtual CUtlString GetDebugName( int nElement ) const override;
	virtual CUtlString GetDebugValue() const override;
	virtual void SpewInterpolatedVar( InterpolatedVarPhase_t ivPhase, float flTime, InterpolationBehavior_t behavior, InterpolatedVarSpewFlags_t nSpewFlags ) const override;
	virtual InterpolatedVarPhase_t SetInterpolatedVarContains( InterpolatedVarPhase_t ivPhase ) override;
	virtual bool HasBeenSetup() const override;
	virtual bool DoesCurrentValueMatchInterpolatedValue( InterpolatedVarPhase_t ivPhase, float flInterpolationTime, InterpolationBehavior_t behavior ) const override;
	virtual bool CheckIfInterpolationTimeIsBeforeFullInterpolationInterval( InterpolatedVarPhase_t ivPhase, float flInterpolationTime ) const override;
	virtual int GetInterpolatedVarSize() const override;
	virtual int ReadCurrentValue( void *pValue ) const override;
	virtual int WriteCurrentValue( void *pValue ) override;
	virtual bool IsProcedural() const override;
	virtual bool HasProceduralDelegates() const override;
	virtual int GetExpectedLatchInterval( InterpolatedVarPhase_t ivPhase ) const override;
	virtual void SetInterpolationTimeType( bool bIsClientAuthoritative, InterpolatedVarInterpolationTimeType_t wivType ) override;
	virtual void ComputePrefixString( const char *pszPrefix, InterpolatedVarPhase_t ivPhase, CUtlString *pPrefix ) const override;
	virtual void PrefixSpewMsg( const char *pszPrefix, InterpolatedVarPhase_t ivPhase ) const override;
	virtual float ComputeInterpolationFrac( InterpolatedVarPhase_t ivPhase, float flTime, InterpolationBehavior_t behavior ) const override;
	virtual int GetValueFixedWidth() const override;
	virtual bool NoteChangedTick( InterpolatedVarPhase_t ivPhase, int nChangeTick ) override;
	virtual bool LatchChangedTick( InterpolatedVarPhase_t ivPhase, const void *pValue, int nChangeTick ) override;
	virtual void *GetContext() const override;

public:
	// One history of the variable: IV_PHASE_CLIENT_AUTHORITATIVE always, IV_PHASE_SERVER_AUTHORITATIVE too when
	// m_bHasServerAuthoritativeVersion is set.
	struct PerPhaseData_t
	{
		CInterpolatedVarHistory< Type > m_VarHistory;
		int m_nLastActualChangeTick;
		int m_nExpectedLatchIntervalTicks;
		float m_flInterpolationAmount;
	};

	// Copied by Copy.
	uint32 m_nLooping;
	// Printed in the debug lines of the variable.
	CEntityIndex m_nEntityIndex;
	// The low three bits of the flags Setup is given.
	uint8 m_nFlags : 3;
	uint8 m_nInWrappedVarLatchList : 2;
	// Two histories (one per authority) instead of one.
	uint8 m_bHasServerAuthoritativeVersion : 1;
	uint8 m_bIsMemcopyable : 1;
	// InterpolatedVarInterpolationTimeType_t.
	uint8 m_nInterpolationTimeType : 1;
	// The entry count Setup allocates the value buffer for.
	uint8 m_nMaxCount : 6;
	uint8 m_nNeedsToInterpolate : 2;
	// InterpolatedVarPhase_t, through SetInterpolatedVarContains.
	int8 m_nInterpolatedVarContains : 3;
	int8 m_eVarTypeBit : 3;
	uint8 m_nDebugPhase : 2;
	uint8 m_bDebug : 1;
	uint8 m_bAutoSpew : 1;
	// The element index GetDebugName is given.
	int8 m_nSpewIndex : 6;
	// The value buffer, m_nMaxCount values, allocated by Setup with new[].
	Type *m_pValue;
	// One or two (m_bHasServerAuthoritativeVersion) histories, allocated with new[].
	PerPhaseData_t *m_pPerPhaseData;
};

// -------------------------------------------------------------------------------------------------------------- //
// The variables that reach the owner's value through the procedural delegates: the delegates and their accessors.
// The game implements the virtual methods.
// -------------------------------------------------------------------------------------------------------------- //
template < typename Type, bool IS_ARRAY, bool IS_PROCEDURAL, class LERP >
abstract_class CInterpolatedVarProceduralBase : public CInterpolatedVarArrayBase< Type, IS_ARRAY, IS_PROCEDURAL, LERP >
{
public:
	virtual bool HasReadWriteDelegates() const override;
	virtual const void *GetReadDelegate() const override;
	virtual const void *GetWriteDelegate() const override;
	virtual void SetProceduralDelegates( const void *pDelegates ) override;

public:
	// Read the owner's value (pValue, element count 1, the variable) and write the interpolated value back to it.
	CUtlDelegate< void ( Type *, int, const IInterpolatedVar * ) > m_readDelegate;
	CUtlDelegate< void ( const Type *, int, const IInterpolatedVar *, IVWriteOpType_t ) > m_writeDelegate;
};

// -------------------------------------------------------------------------------------------------------------- //
// Adds the array index name helper delegate accessors (empty for the procedural variables of the game). The game
// implements the virtual methods.
// -------------------------------------------------------------------------------------------------------------- //
template < typename Type, bool IS_ARRAY, bool IS_PROCEDURAL, class LERP >
abstract_class CInterpolatedVarBase : public CInterpolatedVarProceduralBase< Type, IS_ARRAY, IS_PROCEDURAL, LERP >
{
public:
	// Empty
	virtual void SetArrayIndexNameHelperDelegate( const void *pArrayIndexNameHelper ) override;
	// Returns false
	virtual bool HasArrayIndexHelperDelegate() const override;
	// Returns a static empty delegate
	virtual const void *GetArrayIndexNameHelperDelegate() const override;
};

//-----------------------------------------------------------------------------
// An interpolated variable that reads and writes the value of its owner through the two procedural delegates.
// The game implements the virtual methods. Never construct one; read the members of the game's object.
//-----------------------------------------------------------------------------
template < class T >
class CInterpolatedVarProcedural : public CInterpolatedVarBase< T, false, true, CGlobalInterpolationLerpFuncs< T > >
{
public:
	virtual ~CInterpolatedVarProcedural();
};

// A plain interpolated variable.
template < typename Type >
abstract_class CInterpolatedVar : public CInterpolatedVarArrayBase< Type, false, false, CGlobalInterpolationLerpFuncs< Type > >
{
};

#endif // INTERPOLATEDVAR_H
