#include "common/assert.h"
#include "common/macros.h"

#include <kv3lib/kv3transfer_schema.h>
#include <tier0/strtools.h>

#include <cstddef>

enum TestKV3SchemaMode_t : int32
{
	TEST_KV3SCHEMA_MODE_OFF,
	TEST_KV3SCHEMA_MODE_ON,
};

class CTestKV3SchemaPoint
{
public:
	CLASS_USES_KV3TRANSFER_DATA( CTestKV3SchemaPoint );

	int m_nX = 1;
	float m_flY = 2.0f;
	CUtlString m_sName = "point";
	TestKV3SchemaMode_t m_nMode = TEST_KV3SCHEMA_MODE_OFF;
	CUtlVector< int > m_Values;
};

class CTestKV3SchemaPointDerived : public CTestKV3SchemaPoint
{
public:
	CTestKV3SchemaPointDerived() { m_nX = 5; }

	int m_nZ = 3;
};

class CTestKV3SchemaShape
{
public:
	virtual ~CTestKV3SchemaShape() = default;
	virtual const char *GetShapeName() const { return "CTestKV3SchemaShape"; }

	int m_nSides = 0;
};

class CTestKV3SchemaCircle : public CTestKV3SchemaShape
{
public:
	const char *GetShapeName() const override { return "CTestKV3SchemaCircle"; }

	float m_flRadius = 0.5f;
};

class CTestKV3SchemaOuter
{
public:
	CTestKV3SchemaOuter() { m_Point.m_nX = 9; }
	~CTestKV3SchemaOuter() { delete m_pShape; }

	CTestKV3SchemaPoint m_Point;
	CTestKV3SchemaShape *m_pShape = nullptr;
	int m_nOpsValue = 0;
};

// Transfers an int as a decimal string; 0 is empty.
class CTestKV3SchemaIntAsStringOps : public IKV3TransferSaveRestoreOps
{
public:
	void KV3TransferSave( const void *pField, const void *pOwner, CKV3TransferSaveContext *pContext ) override
	{
		char szValue[ 16 ];

		V_snprintf( szValue, sizeof( szValue ), "%d", *static_cast< const int * >( pField ) );
		pContext->TargetObject()->SetString( szValue );
	}

	void KV3TransferLoad( void *pField, void *pOwner, CKV3TransferLoadContext *pContext ) override
	{
		*static_cast< int * >( pField ) = V_atoi( pContext->SourceObject()->GetString() );
	}

	bool IsEmpty( const void *pField ) override { return *static_cast< const int * >( pField ) == 0; }
	void MakeEmpty( void *pField ) override { *static_cast< int * >( pField ) = 0; ++m_nMakeEmptyCount; }

	int m_nMakeEmptyCount = 0;
};

template < class T >
void *TestKV3Schema_Allocate()
{
	return new T;
}

template < class T >
void TestKV3Schema_Deallocate( void *pObject )
{
	delete static_cast< T * >( pObject );
}

template < class T >
void TestKV3Schema_SetAllocator( CKV3TransferSchemaClass &schemaClass )
{
	schemaClass.m_pfnAllocate = &TestKV3Schema_Allocate< T >;
	schemaClass.m_pfnDeallocate = &TestKV3Schema_Deallocate< T >;
}

static const KV3TransferSchemaEnumerator_t s_TestKV3SchemaModeEnumerators[] =
{
	{ "Off", TEST_KV3SCHEMA_MODE_OFF },
	{ "On", TEST_KV3SCHEMA_MODE_ON },
};

static const KV3TransferSchemaEnum_t s_TestKV3SchemaModeEnum = { "TestKV3SchemaMode_t", sizeof( TestKV3SchemaMode_t ), s_TestKV3SchemaModeEnumerators, ARRAYSIZE( s_TestKV3SchemaModeEnumerators ) };

static const KV3TransferSchemaField_t s_TestKV3SchemaPointFields[] =
{
	KV3TransferSchema_ValueField< int >( "m_nX", offsetof( CTestKV3SchemaPoint, m_nX ) ),
	KV3TransferSchema_ValueField< float >( "m_flY", offsetof( CTestKV3SchemaPoint, m_flY ) ),
	KV3TransferSchema_ValueField< CUtlString >( "m_sName", offsetof( CTestKV3SchemaPoint, m_sName ) ),
	KV3TransferSchema_EnumField( "m_nMode", offsetof( CTestKV3SchemaPoint, m_nMode ), &s_TestKV3SchemaModeEnum ),
	KV3TransferSchema_ValueField< CUtlVector< int > >( "m_Values", offsetof( CTestKV3SchemaPoint, m_Values ) ),
};

static const KV3TransferSchemaField_t s_TestKV3SchemaPointDerivedFields[] =
{
	KV3TransferSchema_ValueField< int >( "m_nZ", offsetof( CTestKV3SchemaPointDerived, m_nZ ) ),
};

static const KV3TransferSchemaField_t s_TestKV3SchemaShapeFields[] =
{
	KV3TransferSchema_ValueField< int >( "m_nSides", offsetof( CTestKV3SchemaShape, m_nSides ) ),
};

static const KV3TransferSchemaField_t s_TestKV3SchemaCircleFields[] =
{
	KV3TransferSchema_ValueField< float >( "m_flRadius", offsetof( CTestKV3SchemaCircle, m_flRadius ) ),
};

static CKV3TransferSchemaClass s_TestKV3SchemaPointClass( "CTestKV3SchemaPoint", KV3TRANSFER_CLASS_AS_SIMPLE_TABLE, s_TestKV3SchemaPointFields, ARRAYSIZE( s_TestKV3SchemaPointFields ) );
static CKV3TransferSchemaClass s_TestKV3SchemaPointDerivedClass( "CTestKV3SchemaPointDerived", KV3TRANSFER_CLASS_AS_SIMPLE_TABLE, s_TestKV3SchemaPointDerivedFields, ARRAYSIZE( s_TestKV3SchemaPointDerivedFields ) );
static CKV3TransferSchemaClass s_TestKV3SchemaShapeClass( "CTestKV3SchemaShape", KV3TRANSFER_CLASS_AS_POLYMORPHIC_TABLE, s_TestKV3SchemaShapeFields, ARRAYSIZE( s_TestKV3SchemaShapeFields ) );
static CKV3TransferSchemaClass s_TestKV3SchemaCircleClass( "CTestKV3SchemaCircle", KV3TRANSFER_CLASS_AS_POLYMORPHIC_TABLE, s_TestKV3SchemaCircleFields, ARRAYSIZE( s_TestKV3SchemaCircleFields ) );

static CTestKV3SchemaIntAsStringOps s_TestKV3SchemaIntAsStringOps;

static const KV3TransferSchemaField_t s_TestKV3SchemaOuterFields[] =
{
	KV3TransferSchema_ClassField( "m_Point", offsetof( CTestKV3SchemaOuter, m_Point ), &s_TestKV3SchemaPointClass ),
	KV3TransferSchema_ClassPointerField( "m_pShape", offsetof( CTestKV3SchemaOuter, m_pShape ), &s_TestKV3SchemaShapeClass ),
	KV3TransferSchema_SaveRestoreOpsField( "m_nOpsValue", offsetof( CTestKV3SchemaOuter, m_nOpsValue ), &s_TestKV3SchemaIntAsStringOps ),
};

static CKV3TransferSchemaClass s_TestKV3SchemaOuterClass( "CTestKV3SchemaOuter", KV3TRANSFER_CLASS_AS_SIMPLE_TABLE, s_TestKV3SchemaOuterFields, ARRAYSIZE( s_TestKV3SchemaOuterFields ) );

static void TestKV3Schema_ShapeClassname( const void *pObject, CBufferString &sOutClassName )
{
	sOutClassName.Set( static_cast< const CTestKV3SchemaShape * >( pObject )->GetShapeName() );
}

static const CKV3TransferSchemaClass *TestKV3Schema_FindShapeClass( const char *pszClassName )
{
	if ( !V_strcmp( pszClassName, s_TestKV3SchemaCircleClass.m_pszName ) )
		return &s_TestKV3SchemaCircleClass;

	return nullptr;
}

static struct TestKV3SchemaClassesInit_t
{
	TestKV3SchemaClassesInit_t()
	{
		TestKV3Schema_SetAllocator< CTestKV3SchemaPoint >( s_TestKV3SchemaPointClass );
		TestKV3Schema_SetAllocator< CTestKV3SchemaPointDerived >( s_TestKV3SchemaPointDerivedClass );
		TestKV3Schema_SetAllocator< CTestKV3SchemaShape >( s_TestKV3SchemaShapeClass );
		TestKV3Schema_SetAllocator< CTestKV3SchemaCircle >( s_TestKV3SchemaCircleClass );
		TestKV3Schema_SetAllocator< CTestKV3SchemaOuter >( s_TestKV3SchemaOuterClass );

		s_TestKV3SchemaPointDerivedClass.m_pBaseClass = &s_TestKV3SchemaPointClass;
		s_TestKV3SchemaCircleClass.m_pBaseClass = &s_TestKV3SchemaShapeClass;

		for ( CKV3TransferSchemaClass *pClass : { &s_TestKV3SchemaShapeClass, &s_TestKV3SchemaCircleClass } )
		{
			pClass->m_pfnPolymorphicClassname = &TestKV3Schema_ShapeClassname;
			pClass->m_pfnFindClass = &TestKV3Schema_FindShapeClass;
		}
	}
} s_TestKV3SchemaClassesInit;

KV3TRANSFER_SCHEMA_CLASS_BODIES( CTestKV3SchemaPoint, &s_TestKV3SchemaPointClass )

REGISTER_NAMED_TEST( "KV3TransferSchema.Layout", KV3TransferSchema_Layout )
{
	TEST_EQ( sizeof( KV3TransferDefaultKeys_t ), static_cast< size_t >( 0x10 ) );
	TEST_EQ( sizeof( IKV3TransferSaveRestoreOps ), sizeof( void * ) );
}

REGISTER_NAMED_TEST( "KV3TransferSchema.SaveLoadRoundTrip", KV3TransferSchema_SaveLoadRoundTrip )
{
	CTestKV3SchemaPoint source;

	source.m_nX = 42;
	source.m_flY = 4.5f;
	source.m_sName = "transfer";
	source.m_nMode = TEST_KV3SCHEMA_MODE_ON;
	source.m_Values.AddToTail( 5 );
	source.m_Values.AddToTail( 6 );

	// Through the value helpers, which call the bodies the description defines
	KeyValues3 root;
	CKV3TransferSaveContext saveContext;

	saveContext.SaveClassPointer( &source, &root );

	TEST_EQ( saveContext.GetResult(), KV3TRANSFER_SUCCESS );
	TEST_TRUE( root.IsTable() );
	TEST_EQ( root.FindMember( "m_nX" )->GetInt(), 42 );
	TEST_EQ( V_strcmp( root.GetMemberString( "m_sName" ), "transfer" ), 0 );
	TEST_EQ( V_strcmp( root.GetMemberString( "m_nMode" ), "On" ), 0 );
	TEST_EQ( root.FindMember( "m_Values" )->GetArrayElementCount(), 2 );

	CTestKV3SchemaPoint dest;
	CKV3TransferLoadContext loadContext;

	loadContext.LoadClassInstance( &dest, &root );

	TEST_EQ( loadContext.GetResult(), KV3TRANSFER_SUCCESS );
	TEST_EQ( dest.m_nX, 42 );
	TEST_TRUE( dest.m_flY == 4.5f );
	TEST_EQ( V_strcmp( dest.m_sName.Get(), "transfer" ), 0 );
	TEST_EQ( dest.m_nMode, TEST_KV3SCHEMA_MODE_ON );
	TEST_EQ( dest.m_Values.Count(), 2 );
	TEST_EQ( dest.m_Values[ 1 ], 6 );

	// An integer that is not an enumerator
	root.FindMember( "m_nMode" )->SetInt( 7 );

	loadContext.LoadClassInstance( &dest, &root );

	TEST_EQ( static_cast< int >( dest.m_nMode ), 7 );
}

REGISTER_NAMED_TEST( "KV3TransferSchema.MissingFieldsUseDefaults", KV3TransferSchema_MissingFieldsUseDefaults )
{
	KeyValues3 empty;

	empty.SetToEmptyTable();

	CTestKV3SchemaPoint point;

	point.m_nX = 100;
	point.m_flY = 0.0f;
	point.m_sName = "changed";
	point.m_nMode = TEST_KV3SCHEMA_MODE_ON;
	point.m_Values.AddToTail( 7 );

	CKV3TransferLoadContext loadContext;

	KV3Transfer_LoadSchemaClass( &loadContext, &s_TestKV3SchemaPointClass, &point, &empty );

	TEST_EQ( loadContext.GetResult(), KV3TRANSFER_SUCCESS );
	TEST_EQ( point.m_nX, 1 );
	TEST_TRUE( point.m_flY == 2.0f );
	TEST_EQ( V_strcmp( point.m_sName.Get(), "point" ), 0 );
	TEST_EQ( point.m_nMode, TEST_KV3SCHEMA_MODE_OFF );
	TEST_EQ( point.m_Values.Count(), 0 );

	// The defaults of the derived class apply to the fields of its base class
	const KV3TransferDefaultKeys_t *pDefaultKeys = s_TestKV3SchemaPointDerivedClass.GetDefaultKeys();

	TEST_NOT_NULL( pDefaultKeys->m_pDefaultKeys );
	TEST_EQ( pDefaultKeys->FindMember( "m_nX" )->GetInt(), 5 );
	TEST_EQ( pDefaultKeys->FindMember( "m_nZ" )->GetInt(), 3 );
	TEST_TRUE( pDefaultKeys->FindMember( "m_nMissing" )->IsNull() );

	CTestKV3SchemaPointDerived derived;

	derived.m_nX = 100;
	derived.m_nZ = 100;

	KV3Transfer_LoadSchemaClass( &loadContext, &s_TestKV3SchemaPointDerivedClass, &derived, &empty );

	TEST_EQ( loadContext.GetResult(), KV3TRANSFER_SUCCESS );
	TEST_EQ( derived.m_nX, 5 );
	TEST_EQ( derived.m_nZ, 3 );

	// A null value loads the defaults as well
	derived.m_nX = 100;

	KV3Transfer_LoadSchemaClass( &loadContext, &s_TestKV3SchemaPointDerivedClass, &derived, &KeyValues3::GetNullValue() );

	TEST_EQ( derived.m_nX, 5 );

	// A value that is not a table fails each field and loads its default
	KeyValues3 number;

	number.SetInt( 1 );
	derived.m_nX = 100;

	CKV3TransferLoadContext numberContext;

	KV3Transfer_LoadSchemaClass( &numberContext, &s_TestKV3SchemaPointDerivedClass, &derived, &number );

	TEST_EQ( numberContext.GetResult(), KV3TRANSFER_FAIL );
	TEST_EQ( derived.m_nX, 5 );
}

REGISTER_NAMED_TEST( "KV3TransferSchema.NestedClassAndPolymorphicPointer", KV3TransferSchema_NestedClassAndPolymorphicPointer )
{
	CTestKV3SchemaOuter source;

	source.m_Point.m_nX = 11;

	CTestKV3SchemaCircle *pCircle = new CTestKV3SchemaCircle;

	pCircle->m_nSides = 1;
	pCircle->m_flRadius = 3.5f;
	source.m_pShape = pCircle;
	source.m_nOpsValue = 42;

	KeyValues3 root;
	CKV3TransferSaveContext saveContext;

	KV3Transfer_SaveSchemaClass( &saveContext, &s_TestKV3SchemaOuterClass, &source, &root );

	TEST_EQ( saveContext.GetResult(), KV3TRANSFER_SUCCESS );
	TEST_NULL( saveContext.TargetObject() );
	TEST_EQ( root.FindMember( "m_Point" )->FindMember( "m_nX" )->GetInt(), 11 );
	TEST_EQ( V_strcmp( root.FindMember( "m_pShape" )->GetMemberString( KV3TRANSFER_CLASSNAME_MEMBER ), "CTestKV3SchemaCircle" ), 0 );
	TEST_EQ( root.FindMember( "m_pShape" )->FindMember( "m_nSides" )->GetInt(), 1 );
	TEST_EQ( V_strcmp( root.GetMemberString( "m_nOpsValue" ), "42" ), 0 );

	CTestKV3SchemaOuter dest;
	CKV3TransferLoadContext loadContext;

	s_TestKV3SchemaIntAsStringOps.m_nMakeEmptyCount = 0;

	KV3Transfer_LoadSchemaClass( &loadContext, &s_TestKV3SchemaOuterClass, &dest, &root );

	TEST_EQ( loadContext.GetResult(), KV3TRANSFER_SUCCESS );
	TEST_NULL( loadContext.SourceObject() );
	TEST_EQ( dest.m_Point.m_nX, 11 );
	TEST_NOT_NULL( dest.m_pShape );
	TEST_EQ( V_strcmp( dest.m_pShape->GetShapeName(), "CTestKV3SchemaCircle" ), 0 );
	TEST_EQ( dest.m_pShape->m_nSides, 1 );
	TEST_TRUE( static_cast< CTestKV3SchemaCircle * >( dest.m_pShape )->m_flRadius == 3.5f );
	TEST_EQ( dest.m_nOpsValue, 42 );
	TEST_EQ( s_TestKV3SchemaIntAsStringOps.m_nMakeEmptyCount, 1 );

	// An empty ops field is not saved, a null pointer saves null
	CTestKV3SchemaOuter empty;
	KeyValues3 emptyRoot;
	CKV3TransferSaveContext emptyContext;

	KV3Transfer_SaveSchemaClass( &emptyContext, &s_TestKV3SchemaOuterClass, &empty, &emptyRoot );

	TEST_EQ( emptyContext.GetResult(), KV3TRANSFER_SUCCESS );
	TEST_NULL( emptyRoot.FindMember( "m_nOpsValue" ) );
	TEST_TRUE( emptyRoot.FindMember( "m_pShape" )->IsNull() );

	// Missing members: the nested class gets the outer default, the ops field keeps its value
	KeyValues3 table;

	table.SetToEmptyTable();

	CTestKV3SchemaOuter missing;

	missing.m_Point.m_nX = 100;
	missing.m_nOpsValue = 5;

	CKV3TransferLoadContext missingContext;

	KV3Transfer_LoadSchemaClass( &missingContext, &s_TestKV3SchemaOuterClass, &missing, &table );

	TEST_EQ( missingContext.GetResult(), KV3TRANSFER_SUCCESS );
	TEST_EQ( missing.m_Point.m_nX, 9 );
	TEST_NULL( missing.m_pShape );
	TEST_EQ( missing.m_nOpsValue, 5 );

	// An owning pointer to the base class
	void *pShape = nullptr;
	CKV3TransferLoadContext pointerContext;

	KV3Transfer_LoadSchemaClassOwningPointer( &pointerContext, &s_TestKV3SchemaShapeClass, pShape, root.FindMember( "m_pShape" ) );

	TEST_EQ( pointerContext.GetResult(), KV3TRANSFER_SUCCESS );
	TEST_NOT_NULL( pShape );
	TEST_EQ( V_strcmp( static_cast< CTestKV3SchemaShape * >( pShape )->GetShapeName(), "CTestKV3SchemaCircle" ), 0 );

	delete static_cast< CTestKV3SchemaShape * >( pShape );
}

REGISTER_NAMED_TEST( "KV3TransferSchema.PolymorphicPointerErrors", KV3TransferSchema_PolymorphicPointerErrors )
{
	KeyValues3 root;

	root.SetToEmptyTable();
	root.FindOrCreateMember( "m_pShape" )->SetToEmptyTable();

	CTestKV3SchemaOuter dest;
	CKV3TransferLoadContext loadContext;

	KV3Transfer_LoadSchemaClass( &loadContext, &s_TestKV3SchemaOuterClass, &dest, &root );

	TEST_EQ( loadContext.GetResult(), KV3TRANSFER_FAIL );
	TEST_NULL( dest.m_pShape );
	TEST_EQ( V_strcmp( loadContext.GetErrorMessage(), "m_pShape: Tried to load a polymorphic pointer with no '_class' key" ), 0 );

	root.FindMember( "m_pShape" )->SetMemberString( KV3TRANSFER_CLASSNAME_MEMBER, "CTestKV3SchemaSquare" );

	CKV3TransferLoadContext unknownContext;

	KV3Transfer_LoadSchemaClass( &unknownContext, &s_TestKV3SchemaOuterClass, &dest, &root );

	TEST_EQ( unknownContext.GetResult(), KV3TRANSFER_FAIL );
	TEST_NULL( dest.m_pShape );
	TEST_EQ( V_strcmp( unknownContext.GetErrorMessage(), "m_pShape: Failed to allocate an instance of class 'CTestKV3SchemaSquare'" ), 0 );
}
