#include "common/assert.h"
#include "common/macros.h"

#include <kv3lib/kv3transfer.h>
#include <tier0/strtools.h>
#include <cstrike15/bot/cs_bot_timers.h>

enum TestKV3TransferMode_t
{
	TEST_KV3TRANSFER_MODE_OFF,
	TEST_KV3TRANSFER_MODE_ON,

	TEST_KV3TRANSFER_MODE_COUNT,
};

BEGIN_KV3TRANSFER_ENUM_HELPERS( TestKV3TransferMode_t, TEST_KV3TRANSFER_MODE_COUNT, TEST_KV3TRANSFER_MODE_OFF )
	KV3TRANSFER_ENUM_HELPER( TEST_KV3TRANSFER_MODE_OFF, "Off" )
	KV3TRANSFER_ENUM_HELPER( TEST_KV3TRANSFER_MODE_ON, "On" )
END_KV3TRANSFER_ENUM_HELPERS()

class CTestKV3TransferShape
{
public:
	CLASS_USES_KV3TRANSFER_VIRTUAL( CTestKV3TransferShape );

	virtual ~CTestKV3TransferShape() = default;
	virtual const char *GetShapeName() const { return "CTestKV3TransferShape"; }

	int m_nSides = 0;
};

class CTestKV3TransferSquare : public CTestKV3TransferShape
{
public:
	CLASS_USES_KV3TRANSFER_VIRTUAL( CTestKV3TransferSquare );

	const char *GetShapeName() const override { return "CTestKV3TransferSquare"; }

	float m_flSize = 0.0f;
};

void CTestKV3TransferShape::KV3TransferPolymorphicClassname( const CTestKV3TransferShape *pObject, CBufferString &sOutClassName )
{
	sOutClassName.Set( pObject->GetShapeName() );
}

CTestKV3TransferShape *CTestKV3TransferShape::KV3TransferAllocateClassInstance( const char *pDerivedClassName, void *pUnk )
{
	if ( !V_strcmp( pDerivedClassName, "CTestKV3TransferShape" ) )
		return new CTestKV3TransferShape;

	if ( !V_strcmp( pDerivedClassName, "CTestKV3TransferSquare" ) )
		return new CTestKV3TransferSquare;

	return nullptr;
}

void CTestKV3TransferShape::KV3TransferSave( CKV3TransferSaveContext *pContext ) const { KV3TransferSave_CTestKV3TransferShape( pContext ); }
void CTestKV3TransferShape::KV3TransferLoad( CKV3TransferLoadContext *pContext ) { KV3TransferLoad_CTestKV3TransferShape( pContext ); }
void CTestKV3TransferShape::KV3TransferSave_CTestKV3TransferShape( CKV3TransferSaveContext *pContext ) const { pContext->SaveValueToMember( "m_nSides", m_nSides ); }
void CTestKV3TransferShape::KV3TransferLoad_CTestKV3TransferShape( CKV3TransferLoadContext *pContext ) { pContext->LoadValueFromMember( "m_nSides", m_nSides ); }

void CTestKV3TransferSquare::KV3TransferSave( CKV3TransferSaveContext *pContext ) const { KV3TransferSave_CTestKV3TransferSquare( pContext ); }
void CTestKV3TransferSquare::KV3TransferLoad( CKV3TransferLoadContext *pContext ) { KV3TransferLoad_CTestKV3TransferSquare( pContext ); }

void CTestKV3TransferSquare::KV3TransferSave_CTestKV3TransferSquare( CKV3TransferSaveContext *pContext ) const
{
	KV3TransferSave_CTestKV3TransferShape( pContext );
	pContext->SaveValueToMember( "m_flSize", m_flSize );
}

void CTestKV3TransferSquare::KV3TransferLoad_CTestKV3TransferSquare( CKV3TransferLoadContext *pContext )
{
	KV3TransferLoad_CTestKV3TransferShape( pContext );
	pContext->LoadValueFromMember( "m_flSize", m_flSize );
}

class CTestKV3TransferData
{
public:
	CLASS_USES_KV3TRANSFER_DATA( CTestKV3TransferData );

	~CTestKV3TransferData() { delete m_pShape; }

	int m_nValue = 0;
	CUtlString m_sName;
	Vector m_vecOrigin = Vector( 0.0f, 0.0f, 0.0f );
	TestKV3TransferMode_t m_nMode = TEST_KV3TRANSFER_MODE_OFF;
	CUtlVector< int > m_Values;
	int m_nFixed[ 2 ] = {};
	char m_szLabel[ 16 ] = {};
	CTestKV3TransferShape *m_pShape = nullptr;
};

void CTestKV3TransferData::KV3TransferSave( CKV3TransferSaveContext *pContext ) const { KV3TransferSave_CTestKV3TransferData( pContext ); }
void CTestKV3TransferData::KV3TransferLoad( CKV3TransferLoadContext *pContext ) { KV3TransferLoad_CTestKV3TransferData( pContext ); }

void CTestKV3TransferData::KV3TransferSave_CTestKV3TransferData( CKV3TransferSaveContext *pContext ) const
{
	pContext->SaveValueToMember( "m_nValue", m_nValue );
	pContext->SaveValueToMember( "m_sName", m_sName );
	pContext->SaveValueToMember( "m_vecOrigin", m_vecOrigin );
	pContext->SaveValueToMember( "m_nMode", m_nMode );
	pContext->SaveValueToMember( "m_Values", m_Values );
	pContext->SaveValueToMember( "m_nFixed", m_nFixed );
	pContext->SaveValueToMember( "m_szLabel", m_szLabel );
	pContext->SaveValueToMember( "m_pShape", m_pShape );
}

void CTestKV3TransferData::KV3TransferLoad_CTestKV3TransferData( CKV3TransferLoadContext *pContext )
{
	pContext->LoadValueFromMember( "m_nValue", m_nValue );
	pContext->LoadValueFromMember( "m_sName", m_sName );
	pContext->LoadValueFromMember( "m_vecOrigin", m_vecOrigin );
	pContext->LoadValueFromMember( "m_nMode", m_nMode );
	pContext->LoadValueFromMember( "m_Values", m_Values );
	pContext->LoadValueFromMember( "m_nFixed", m_nFixed );
	pContext->LoadValueFromMember( "m_szLabel", m_szLabel );
	pContext->LoadValueFromMember( "m_pShape", m_pShape );
	pContext->LoadValueFromMemberOrDefault( "m_nMissing", m_nValue, "7" );
}

REGISTER_NAMED_TEST( "KV3Transfer.SaveLoadRoundTrip", KV3Transfer_SaveLoadRoundTrip )
{
	CTestKV3TransferData source;

	source.m_nValue = 42;
	source.m_sName = "transfer";
	source.m_vecOrigin = Vector( 1.0f, 2.0f, 3.0f );
	source.m_nMode = TEST_KV3TRANSFER_MODE_ON;
	source.m_Values.AddToTail( 5 );
	source.m_Values.AddToTail( 6 );
	source.m_nFixed[ 0 ] = 8;
	source.m_nFixed[ 1 ] = 9;
	V_strncpy( source.m_szLabel, "label", sizeof( source.m_szLabel ) );

	CTestKV3TransferSquare *pSquare = new CTestKV3TransferSquare;

	pSquare->m_nSides = 4;
	pSquare->m_flSize = 2.5f;
	source.m_pShape = pSquare;

	KeyValues3 root;
	CKV3TransferSaveContext saveContext;

	saveContext.SaveClassPointer( &source, &root );

	TEST_EQ( saveContext.GetResult(), KV3TRANSFER_SUCCESS );
	TEST_NULL( saveContext.TargetObject() );
	TEST_TRUE( root.IsTable() );
	TEST_EQ( V_strcmp( root.GetMemberString( "m_nMode" ), "On" ), 0 );
	TEST_EQ( V_strcmp( root.FindMember( "m_pShape" )->GetMemberString( KV3TRANSFER_CLASSNAME_MEMBER ), "CTestKV3TransferSquare" ), 0 );

	CTestKV3TransferData dest;
	CKV3TransferLoadContext loadContext;

	loadContext.LoadClassInstance( &dest, &root );

	TEST_EQ( loadContext.GetResult(), KV3TRANSFER_SUCCESS );
	TEST_NULL( loadContext.SourceObject() );
	TEST_EQ( dest.m_nValue, 7 );
	TEST_EQ( V_strcmp( dest.m_sName.Get(), "transfer" ), 0 );
	TEST_TRUE( dest.m_vecOrigin == Vector( 1.0f, 2.0f, 3.0f ) );
	TEST_EQ( dest.m_nMode, TEST_KV3TRANSFER_MODE_ON );
	TEST_EQ( dest.m_Values.Count(), 2 );
	TEST_EQ( dest.m_Values[ 1 ], 6 );
	TEST_EQ( dest.m_nFixed[ 1 ], 9 );
	TEST_EQ( V_strcmp( dest.m_szLabel, "label" ), 0 );
	TEST_NOT_NULL( dest.m_pShape );
	TEST_EQ( V_strcmp( dest.m_pShape->GetShapeName(), "CTestKV3TransferSquare" ), 0 );
	TEST_EQ( dest.m_pShape->m_nSides, 4 );
	TEST_TRUE( static_cast< CTestKV3TransferSquare * >( dest.m_pShape )->m_flSize == 2.5f );
}

REGISTER_NAMED_TEST( "KV3Transfer.LoadErrorHasContextPath", KV3Transfer_LoadErrorHasContextPath )
{
	KeyValues3 root;

	root.SetToEmptyTable();
	root.FindOrCreateMember( "m_pShape" )->SetToEmptyTable();

	CTestKV3TransferData dest;
	CKV3TransferLoadContext loadContext;

	loadContext.LoadClassInstance( &dest, &root );

	TEST_EQ( loadContext.GetResult(), KV3TRANSFER_FAIL );
	TEST_NULL( dest.m_pShape );
	TEST_EQ( V_strcmp( loadContext.GetErrorMessage(), "m_pShape: Tried to load a polymorphic pointer with no '_class' key" ), 0 );

	CKV3TransferLoadContext namedContext( "file.kv3" );

	namedContext.LoadClassInstance( &dest, &root );

	TEST_EQ( V_strcmp( namedContext.GetErrorMessage(), "file.kv3|m_pShape: Tried to load a polymorphic pointer with no '_class' key" ), 0 );
}

REGISTER_NAMED_TEST( "KV3Transfer.DoubleSaveFails", KV3Transfer_DoubleSaveFails )
{
	KeyValues3 root;

	root.SetToEmptyTable();

	CKV3TransferSaveContext saveContext;

	saveContext.PushTarget( &root );
	saveContext.SaveValueToMember( "m_nValue", 1 );
	saveContext.SaveValueToMember( "m_nValue", 2 );
	saveContext.PopTarget();

	TEST_EQ( saveContext.GetResult(), KV3TRANSFER_FAIL );
	TEST_EQ( V_strcmp( saveContext.GetErrorMessage(), "Double-save to Member 'm_nValue'" ), 0 );
	TEST_EQ( root.FindMember( "m_nValue" )->GetInt(), 2 );
}

class CTestKV3TransferGameTimeSave : public IKV3TransferInterface_GameTime_Save
{
public:
	float64 ToSavedTime( float64 flGameTime ) override { return flGameTime - 100.0; }
};

class CTestKV3TransferGameTimeLoad : public IKV3TransferInterface_GameTime_Load
{
public:
	float64 FromSavedTime( float64 flSavedTime ) override { return flSavedTime + 100.0; }
};

class CTestKV3TransferWorldGroupIdSave : public IKV3TransferInterface_WorldGroupId_Save
{
public:
	void Save( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const WorldGroupId_t *pWorldGroupId ) override { pSaveToValue->SetUInt( pWorldGroupId->GetHashCode() ); }
};

class CTestKV3TransferWorldGroupIdLoad : public IKV3TransferInterface_WorldGroupId_Load
{
public:
	void Load( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, WorldGroupId_t *pWorldGroupId ) override { *pWorldGroupId = WorldGroupId_t( pLoadFromValue->GetUInt() ); }
};

REGISTER_NAMED_TEST( "KV3Transfer.CountdownTimer", KV3Transfer_CountdownTimer )
{
	CTestKV3TransferGameTimeSave gameTimeSave;
	CTestKV3TransferGameTimeLoad gameTimeLoad;
	CTestKV3TransferWorldGroupIdSave worldGroupIdSave;
	CTestKV3TransferWorldGroupIdLoad worldGroupIdLoad;

	CountdownTimer source;

	source.m_duration = 5.0f;
	source.m_timestamp = GameTime_t( 10.0f );
	source.m_timescale = 2.0f;
	source.m_nWorldGroupId = WorldGroupId_t( 3u );

	KeyValues3 root;

	root.SetToEmptyTable();

	CKV3TransferSaveContext saveContext;

	saveContext.AddInterface( KV3TRANSFER_INTERFACE_NAME( IKV3TransferInterface_GameTime_Save ), static_cast< IKV3TransferInterface_GameTime_Save * >( &gameTimeSave ) );
	saveContext.AddInterface( KV3TRANSFER_INTERFACE_NAME( IKV3TransferInterface_WorldGroupId_Save ), static_cast< IKV3TransferInterface_WorldGroupId_Save * >( &worldGroupIdSave ) );
	saveContext.PushTarget( &root );
	source.KV3TransferSave( &saveContext );
	saveContext.PopTarget();

	TEST_EQ( saveContext.GetResult(), KV3TRANSFER_SUCCESS );
	TEST_TRUE( root.FindMember( "m_timestamp" )->GetDouble() == -90.0 );
	TEST_EQ( root.FindMember( "m_nWorldGroupId" )->GetUInt(), 3u );

	CountdownTimer dest;

	dest.m_duration = 0.0f;
	dest.m_timestamp = GameTime_t( -1.0f );
	dest.m_timescale = 1.0f;
	dest.m_nWorldGroupId = WorldGroupId_t( 0u );

	CKV3TransferLoadContext loadContext;

	loadContext.AddInterface( KV3TRANSFER_INTERFACE_NAME( IKV3TransferInterface_GameTime_Load ), static_cast< IKV3TransferInterface_GameTime_Load * >( &gameTimeLoad ) );
	loadContext.AddInterface( KV3TRANSFER_INTERFACE_NAME( IKV3TransferInterface_WorldGroupId_Load ), static_cast< IKV3TransferInterface_WorldGroupId_Load * >( &worldGroupIdLoad ) );
	loadContext.LoadClassInstance( &dest, &root );

	TEST_EQ( loadContext.GetResult(), KV3TRANSFER_SUCCESS );
	TEST_TRUE( dest.m_duration == 5.0f );
	TEST_TRUE( dest.m_timestamp.GetTime() == 10.0f );
	TEST_TRUE( dest.m_timescale == 2.0f );
	TEST_EQ( dest.m_nWorldGroupId.GetHashCode(), 3u );

	// Missing members get the values of a default constructed timer
	KeyValues3 empty;

	empty.SetToEmptyTable();

	CKV3TransferLoadContext defaultsContext;

	defaultsContext.LoadClassInstance( &dest, &empty );

	TEST_EQ( defaultsContext.GetResult(), KV3TRANSFER_SUCCESS );
	TEST_TRUE( dest.m_duration == 0.0f );
	TEST_TRUE( dest.m_timestamp.GetTime() == 0.0f );
	TEST_TRUE( dest.m_timescale == 1.0f );
	TEST_EQ( dest.m_nWorldGroupId.GetHashCode(), ~0u );

	// A running timer needs the game time interface
	CKV3TransferSaveContext noInterfaceContext;
	KeyValues3 noInterfaceRoot;

	noInterfaceRoot.SetToEmptyTable();
	noInterfaceContext.PushTarget( &noInterfaceRoot );
	source.KV3TransferSave( &noInterfaceContext );
	noInterfaceContext.PopTarget();

	TEST_EQ( noInterfaceContext.GetResult(), KV3TRANSFER_FAIL );
	TEST_TRUE( noInterfaceRoot.FindMember( "m_timestamp" )->IsNull() );
}

REGISTER_NAMED_TEST( "KV3Transfer.Interfaces", KV3Transfer_Interfaces )
{
	int nInterface = 0;

	CKV3TransferSaveContext optionalContext;

	optionalContext.AddInterface( "IKV3TransferInterface_Test", &nInterface );

	TEST_EQ( optionalContext.FindInterface( "IKV3TransferInterface_Test" ), static_cast< void * >( &nInterface ) );
	TEST_NULL( optionalContext.FindInterface( "IKV3TransferInterface_Missing" ) );
	TEST_EQ( optionalContext.GetResult(), KV3TRANSFER_SUCCESS );

	CKV3TransferSaveContext requiredContext( false );
	CKV3TransferContextBase::ResultState_t state;

	requiredContext.SaveResult( &state );

	TEST_NULL( requiredContext.FindInterface( "IKV3TransferInterface_Missing" ) );
	TEST_EQ( requiredContext.GetResult(), KV3TRANSFER_FAIL );
	TEST_EQ( V_strcmp( requiredContext.GetErrorMessageSince( state ), "Missing required transfer interface: IKV3TransferInterface_Missing" ), 0 );

	requiredContext.RestoreResult( state );

	TEST_EQ( requiredContext.GetResult(), KV3TRANSFER_FAIL );
}
