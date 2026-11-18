#include "common/assert.h"
#include "common/macros.h"

#include <entity2/entitymetadata.h>
#include <game/server/aidebugsnapshotmetadata.h>
#include <kv3lib/kv3transfer_schema.h>
#include <model/modelmetadata.h>
#include <networksystem/networkvar.h>
#include <particles/particlesmetadata.h>
#include <pulse/pulsemetadata.h>
#include <resourcefile/resourcetype.h>
#include <saverestore/saverestoremetadata.h>
#include <schemasystem/propertyeditormetadata.h>
#include <schemasystem/schema.h>
#include <schemasystem/schemametatag.h>
#include <tools/fgdmetadata.h>
#include <vdata/vdatametadata.h>
#include <vphysics/vphysicsmetadata.h>
#include <tier0/strtools.h>

DECLARE_SCHEMA_META_TAG( MTestSchemaFlag, META_TAG_ON_CLASS | META_TAG_ON_FIELD | META_TAG_ON_ENUM | META_TAG_ON_ENUMERATOR | META_TAG_ON_ATOMIC, META_TAG_ONLY() );
DECLARE_SCHEMA_META_TAG( MTestSchemaName, META_TAG_ON_CLASS | META_TAG_ON_FIELD | META_TAG_ON_ENUM | META_TAG_ON_ENUMERATOR | META_TAG_ON_ATOMIC, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MTestSchemaCount, META_TAG_ON_CLASS | META_TAG_ON_FIELD, META_VALUE( int ) );

DECLARE_SCHEMA_CODEGEN_TAG( MTestSchemaEmit, META_TAG_ON_CLASS,
(
	emit( "// $class_name$" )
) );

// A tag value is the storage the entry points at
static const char *s_pszTestSchemaNameA = "a";
static const char *s_pszTestSchemaNameB = "b";
static const char *s_pszTestSchemaNameBase = "base";
static int s_nTestSchemaCount = 7;

static SchemaMetadataEntryData_t s_TestSchemaMetadata[] =
{
	{ "MTestSchemaName", &s_pszTestSchemaNameA },
	{ "MTestSchemaFlag", nullptr },
	{ "MTestSchemaName", &s_pszTestSchemaNameB },
	{ "MTestSchemaCount", &s_nTestSchemaCount },
};

static SchemaMetadataEntryData_t s_TestSchemaBaseMetadata[] =
{
	{ "MTestSchemaName", &s_pszTestSchemaNameBase },
	{ "MTestSchemaFlag", nullptr },
};

static SchemaMetadataEntryData_t s_TestSchemaFieldMetadata[] =
{
	{ "MTestSchemaCount", &s_nTestSchemaCount },
};

//--------------------------------------------------------------------------------------------------
// CTestDerived : CTestPrimary (at 0), CTestSecondary (at 16); CTestPrimary : CTestRoot (at 4)
//--------------------------------------------------------------------------------------------------
struct TestSchemaHierarchy_t
{
	CSchemaClassInfo m_Root;
	CSchemaClassInfo m_Primary;
	CSchemaClassInfo m_Secondary;
	CSchemaClassInfo m_Derived;

	SchemaBaseClassInfoData_t m_PrimaryBases[ 1 ];
	SchemaBaseClassInfoData_t m_DerivedBases[ 2 ];

	SchemaClassFieldData_t m_RootFields[ 2 ];
	SchemaClassFieldData_t m_SecondaryFields[ 1 ];
	SchemaClassFieldData_t m_DerivedFields[ 1 ];

	TestSchemaHierarchy_t()
	{
		V_memset( this, 0, sizeof( *this ) );

		InitClass( m_Root, "CTestRoot", nullptr, 0, m_RootFields, ARRAYSIZE( m_RootFields ) );
		InitClass( m_Primary, "CTestPrimary", m_PrimaryBases, ARRAYSIZE( m_PrimaryBases ), nullptr, 0 );
		InitClass( m_Secondary, "CTestSecondary", nullptr, 0, m_SecondaryFields, ARRAYSIZE( m_SecondaryFields ) );
		InitClass( m_Derived, "CTestDerived", m_DerivedBases, ARRAYSIZE( m_DerivedBases ), m_DerivedFields, ARRAYSIZE( m_DerivedFields ) );

		m_PrimaryBases[ 0 ] = { 4, &m_Root };
		m_DerivedBases[ 0 ] = { 0, &m_Primary };
		m_DerivedBases[ 1 ] = { 16, &m_Secondary };

		m_RootFields[ 0 ] = { "m_nRootA", nullptr, 0, 0, nullptr };
		m_RootFields[ 1 ] = { "m_nRootB", nullptr, 8, ARRAYSIZE( s_TestSchemaFieldMetadata ), s_TestSchemaFieldMetadata };
		m_SecondaryFields[ 0 ] = { "m_nSecondary", nullptr, 4, 0, nullptr };
		m_DerivedFields[ 0 ] = { "m_nDerived", nullptr, 32, 0, nullptr };

		m_Root.m_nStaticMetadataCount = ARRAYSIZE( s_TestSchemaBaseMetadata );
		m_Root.m_pStaticMetadata = s_TestSchemaBaseMetadata;
		m_Secondary.m_nStaticMetadataCount = ARRAYSIZE( s_TestSchemaMetadata );
		m_Secondary.m_pStaticMetadata = s_TestSchemaMetadata;
	}

	static void InitClass( CSchemaClassInfo &classInfo, const char *pszName, SchemaBaseClassInfoData_t *pBases, int nBaseCount, SchemaClassFieldData_t *pFields, int nFieldCount )
	{
		classInfo.m_pszName = pszName;
		classInfo.m_nBaseClassCount = static_cast< uint8 >( nBaseCount );
		classInfo.m_pBaseClasses = pBases;
		classInfo.m_nFieldCount = static_cast< uint16 >( nFieldCount );
		classInfo.m_pFields = pFields;
	}
};

static CUtlString TestSchema_TraversalString( const CSchemaClassInfo *pClass, SchemaBaseClassTraversal_t nBaseTraversal, const CSchemaClassInfo *pClassToSkip = nullptr )
{
	CUtlString sResult;

	for ( CSchemaInheritanceIterator iterator( pClass, nBaseTraversal, pClassToSkip ); iterator.IsValid(); iterator.Advance() )
	{
		char szEntry[ 64 ];

		V_snprintf( szEntry, sizeof( szEntry ), "%s%s@%u", sResult.IsEmpty() ? "" : " ", iterator.CurrentClass()->m_pszName, iterator.CurrentClassOffset() );
		sResult += szEntry;
	}

	return sResult;
}

REGISTER_NAMED_TEST( "SchemaMetaTag.MetadataSet", SchemaMetaTag_MetadataSet )
{
	const CSchemaMetadataSet metadata( ARRAYSIZE( s_TestSchemaMetadata ), s_TestSchemaMetadata );

	TEST_EQ( metadata.GetEntryCount(), 4 );
	TEST_EQ( V_strcmp( metadata.GetEntryByIndex( 1 )->m_pszName, "MTestSchemaFlag" ), 0 );

	TEST_EQ( metadata.GetPtr( "MTestSchemaName" ), static_cast< void * >( &s_pszTestSchemaNameA ) );
	TEST_NULL( metadata.GetPtr( "MTestSchemaFlag" ) );
	TEST_NULL( metadata.GetPtr( "mtestschemaname" ) );

	TEST_TRUE( metadata.IsPresent( "MTestSchemaFlag" ) );
	TEST_FALSE( metadata.IsPresent( "MTestSchemaMissing" ) );

	TEST_EQ( metadata.GetTagInstanceCount( "MTestSchemaName" ), 2 );
	TEST_EQ( metadata.GetTagInstanceCount( "MTestSchemaMissing" ), 0 );

	CUtlVector< const char ** > names;

	metadata.GetAllPtrs( "MTestSchemaName", names );

	TEST_EQ( names.Count(), 2 );
	TEST_TRUE( names[ 0 ] == &s_pszTestSchemaNameA );
	TEST_TRUE( names[ 1 ] == &s_pszTestSchemaNameB );

	const CSchemaMetadataSet empty( 3, nullptr );

	TEST_EQ( empty.GetEntryCount(), 0 );
	TEST_FALSE( empty.IsPresent( "MTestSchemaFlag" ) );
}

REGISTER_NAMED_TEST( "SchemaMetaTag.TagOwners", SchemaMetaTag_TagOwners )
{
	TEST_EQ( V_strcmp( MTestSchemaName::TagName(), "MTestSchemaName" ), 0 );
	TEST_EQ( MTestSchemaCount::MetaFlags(), static_cast< uint >( META_TAG_ON_CLASS | META_TAG_ON_FIELD ) );

	CSchemaClassField field = {};
	field.m_pszName = "m_nField";
	field.m_nStaticMetadataCount = ARRAYSIZE( s_TestSchemaMetadata );
	field.m_pStaticMetadata = s_TestSchemaMetadata;

	TEST_TRUE( MTestSchemaFlag::IsPresent( &field ) );
	TEST_EQ( V_strcmp( MTestSchemaName::GetValue( &field, "default" ), "a" ), 0 );
	TEST_EQ( *MTestSchemaCount::GetPtr( &field ), 7 );

	CSchemaStaticField staticField = {};

	TEST_FALSE( MTestSchemaFlag::IsPresent( &staticField ) );
	TEST_NULL( MTestSchemaName::GetPtr( &staticField ) );
	TEST_EQ( V_strcmp( MTestSchemaName::GetValue( &staticField, "default" ), "default" ), 0 );

	CSchemaEnumInfo enumInfo = {};
	enumInfo.m_nStaticMetadataCount = ARRAYSIZE( s_TestSchemaBaseMetadata );
	enumInfo.m_pStaticMetadata = s_TestSchemaBaseMetadata;

	TEST_EQ( V_strcmp( MTestSchemaName::GetValue( &enumInfo, nullptr ), "base" ), 0 );

	CSchemaEnumeratorInfo enumeratorInfo = {};
	enumeratorInfo.m_nStaticMetadataCount = ARRAYSIZE( s_TestSchemaMetadata );
	enumeratorInfo.m_pStaticMetadata = s_TestSchemaMetadata;

	CUtlVector< const char ** > names;

	MTestSchemaName::GetAllPtrs( &enumeratorInfo, names );

	TEST_EQ( names.Count(), 2 );
	TEST_EQ( V_strcmp( *names[ 1 ], "b" ), 0 );

	SchemaAtomicTypeInfo_t atomicInfo = {};
	atomicInfo.m_nStaticMetadataCount = ARRAYSIZE( s_TestSchemaBaseMetadata );
	atomicInfo.m_pStaticMetadata = s_TestSchemaBaseMetadata;

	TEST_TRUE( MTestSchemaFlag::IsPresent( &atomicInfo ) );
	TEST_TRUE( MTestSchemaFlag::IsPresent( SchemaMetadataSetOf( &atomicInfo ) ) );
}

REGISTER_NAMED_TEST( "SchemaMetaTag.InheritanceTraversal", SchemaMetaTag_InheritanceTraversal )
{
	const TestSchemaHierarchy_t hierarchy;
	const CSchemaClassInfo *pDerived = &hierarchy.m_Derived;

	TEST_EQ( V_strcmp( TestSchema_TraversalString( pDerived, SCHEMA_BASE_TRAVERSAL_NONE ).Get(), "CTestDerived@0" ), 0 );
	TEST_EQ( V_strcmp( TestSchema_TraversalString( pDerived, SCHEMA_BASE_TRAVERSAL_PRIMARY_ONLY ).Get(), "CTestDerived@0 CTestPrimary@0 CTestRoot@4" ), 0 );
	TEST_EQ( V_strcmp( TestSchema_TraversalString( pDerived, SCHEMA_BASE_TRAVERSAL_FULL ).Get(), "CTestDerived@0 CTestPrimary@0 CTestRoot@4 CTestSecondary@16" ), 0 );
	TEST_EQ( V_strcmp( TestSchema_TraversalString( pDerived, SCHEMA_BASE_TRAVERSAL_PRIMARY_ONLY_IN_MEMORY_ORDER ).Get(), "CTestRoot@4 CTestPrimary@0 CTestDerived@0" ), 0 );
	TEST_EQ( V_strcmp( TestSchema_TraversalString( pDerived, SCHEMA_BASE_TRAVERSAL_FULL_IN_MEMORY_ORDER ).Get(), "CTestRoot@4 CTestPrimary@0 CTestSecondary@16 CTestDerived@0" ), 0 );

	// A skipped class skips its bases too
	TEST_EQ( V_strcmp( TestSchema_TraversalString( pDerived, SCHEMA_BASE_TRAVERSAL_FULL, &hierarchy.m_Primary ).Get(), "CTestDerived@0 CTestSecondary@16" ), 0 );
	TEST_EQ( TestSchema_TraversalString( pDerived, SCHEMA_BASE_TRAVERSAL_NONE, pDerived ).Length(), 0 );
	TEST_EQ( TestSchema_TraversalString( nullptr, SCHEMA_BASE_TRAVERSAL_FULL ).Length(), 0 );

	// A null base is skipped
	SchemaBaseClassInfoData_t nullBase = { 8, nullptr };
	CSchemaClassInfo withNullBase = hierarchy.m_Root;
	withNullBase.m_nBaseClassCount = 1;
	withNullBase.m_pBaseClasses = &nullBase;

	TEST_EQ( V_strcmp( TestSchema_TraversalString( &withNullBase, SCHEMA_BASE_TRAVERSAL_FULL ).Get(), "CTestRoot@0" ), 0 );
	TEST_TRUE( MTestSchemaFlag::IsPresent( &withNullBase, SCHEMA_BASE_TRAVERSAL_FULL ) );
	TEST_FALSE( MTestSchemaCount::IsPresent( &withNullBase, SCHEMA_BASE_TRAVERSAL_FULL ) );

	const CSchemaInheritanceIterator full( pDerived, SCHEMA_BASE_TRAVERSAL_FULL );

	TEST_EQ( full.m_ClassesToTraverse.Count(), 4 );
	TEST_EQ( full.m_ClassesToTraverse[ 0 ].m_nDerivedClassIndex, -1 );
	TEST_EQ( full.m_ClassesToTraverse[ 1 ].m_nDerivedClassIndex, 0 );
	TEST_EQ( full.m_ClassesToTraverse[ 2 ].m_nDerivedClassIndex, 1 );
	TEST_EQ( full.m_ClassesToTraverse[ 3 ].m_nDerivedClassIndex, 0 );

	const CSchemaInheritanceIterator memoryOrder( pDerived, SCHEMA_BASE_TRAVERSAL_FULL_IN_MEMORY_ORDER );

	TEST_EQ( memoryOrder.m_ClassesToTraverse[ 0 ].m_nDerivedClassIndex, 1 );
	TEST_EQ( memoryOrder.m_ClassesToTraverse[ 1 ].m_nDerivedClassIndex, 3 );
	TEST_EQ( memoryOrder.m_ClassesToTraverse[ 2 ].m_nDerivedClassIndex, 3 );
	TEST_EQ( memoryOrder.m_ClassesToTraverse[ 3 ].m_nDerivedClassIndex, -1 );
}

REGISTER_NAMED_TEST( "SchemaMetaTag.ClassLookup", SchemaMetaTag_ClassLookup )
{
	const TestSchemaHierarchy_t hierarchy;
	const CSchemaClassInfo *pDerived = &hierarchy.m_Derived;

	TEST_FALSE( MTestSchemaFlag::IsPresent( pDerived, SCHEMA_BASE_TRAVERSAL_NONE ) );
	TEST_TRUE( MTestSchemaFlag::IsPresent( pDerived, SCHEMA_BASE_TRAVERSAL_PRIMARY_ONLY ) );

	// Only the start class is skipped, never a base
	TEST_FALSE( MTestSchemaFlag::IsPresent( pDerived, SCHEMA_BASE_TRAVERSAL_FULL, pDerived ) );
	TEST_TRUE( MTestSchemaFlag::IsPresent( pDerived, SCHEMA_BASE_TRAVERSAL_PRIMARY_ONLY, &hierarchy.m_Primary ) );

	// Only SCHEMA_BASE_TRAVERSAL_PRIMARY_ONLY stops at the first base
	TEST_FALSE( MTestSchemaCount::IsPresent( pDerived, SCHEMA_BASE_TRAVERSAL_PRIMARY_ONLY ) );
	TEST_TRUE( MTestSchemaCount::IsPresent( pDerived, SCHEMA_BASE_TRAVERSAL_PRIMARY_ONLY_IN_MEMORY_ORDER ) );
	TEST_TRUE( MTestSchemaCount::IsPresent( pDerived, SCHEMA_BASE_TRAVERSAL_FULL ) );

	// The first visited class with the tag wins
	TEST_EQ( V_strcmp( MTestSchemaName::GetValue( pDerived, "default", SCHEMA_BASE_TRAVERSAL_FULL ), "base" ), 0 );
	TEST_EQ( V_strcmp( MTestSchemaName::GetValue( pDerived, "default", SCHEMA_BASE_TRAVERSAL_FULL, &hierarchy.m_Primary ), "a" ), 0 );
	TEST_EQ( V_strcmp( MTestSchemaName::GetValue( pDerived, "default", SCHEMA_BASE_TRAVERSAL_NONE ), "default" ), 0 );
	TEST_NULL( MTestSchemaCount::GetPtr( pDerived, SCHEMA_BASE_TRAVERSAL_PRIMARY_ONLY ) );

	CUtlVector< const char ** > names;

	MTestSchemaName::GetAllPtrs( pDerived, names, SCHEMA_BASE_TRAVERSAL_FULL_IN_MEMORY_ORDER );

	TEST_EQ( names.Count(), 3 );
	TEST_EQ( V_strcmp( *names[ 0 ], "base" ), 0 );
	TEST_EQ( V_strcmp( *names[ 1 ], "a" ), 0 );
	TEST_EQ( V_strcmp( *names[ 2 ], "b" ), 0 );
}

REGISTER_NAMED_TEST( "SchemaMetaTag.FieldIterator", SchemaMetaTag_FieldIterator )
{
	const TestSchemaHierarchy_t hierarchy;

	CUtlString sFields;
	int nCountTags = 0;

	for ( CSchemaFieldIterator iterator( &hierarchy.m_Derived, SCHEMA_BASE_TRAVERSAL_FULL_IN_MEMORY_ORDER ); iterator.IsValid(); iterator.Advance() )
	{
		char szEntry[ 64 ];

		V_snprintf( szEntry, sizeof( szEntry ), "%s%s@%d", sFields.IsEmpty() ? "" : " ", iterator.GetFullyQualifiedFieldName().Get(), iterator.GetFieldMemoryOffset() );
		sFields += szEntry;

		if ( MTestSchemaCount::IsPresent( iterator ) )
		{
			TEST_EQ( MTestSchemaCount::GetValue( iterator, 0 ), 7 );
			++nCountTags;
		}
	}

	// CTestPrimary has no fields of its own
	TEST_EQ( V_strcmp( sFields.Get(), "CTestRoot::m_nRootA@4 CTestRoot::m_nRootB@12 CTestSecondary::m_nSecondary@20 CTestDerived::m_nDerived@32" ), 0 );
	TEST_EQ( nCountTags, 1 );

	CSchemaFieldIterator none( &hierarchy.m_Primary, SCHEMA_BASE_TRAVERSAL_NONE );

	TEST_FALSE( none.IsValid() );
}

static PropertyAttrState_t TestSchema_GetAttrState( void *pObject )
{
	return PROPERTY_ATTR_STATE_READONLY;
}

REGISTER_NAMED_TEST( "SchemaMetaTag.GameTags", SchemaMetaTag_GameTags )
{
	PropertyAttrStateCallbackFn_t pfnGetAttrState = &TestSchema_GetAttrState;
	PropertyClassAsStringFuncs_t classAsString = { nullptr, nullptr };
	ResourceType_t nResourceType = 0x6d6c76;
	int nSaveBehavior = 1;

	SchemaMetadataEntryData_t fieldMetadata[] = { { "MPropertyAttrStateCallback", &pfnGetAttrState }, { "MSaveBehavior", &nSaveBehavior }, { "MPropertyHideField", nullptr } };
	SchemaMetadataEntryData_t classMetadata[] = { { "MPropertyEditClassAsString", &classAsString }, { "MResourceTypeForInfoType", &nResourceType } };

	CSchemaClassField field = {};
	field.m_nStaticMetadataCount = ARRAYSIZE( fieldMetadata );
	field.m_pStaticMetadata = fieldMetadata;

	CSchemaClassInfo classInfo = {};
	classInfo.m_nStaticMetadataCount = ARRAYSIZE( classMetadata );
	classInfo.m_pStaticMetadata = classMetadata;

	TEST_EQ( static_cast< int >( MPropertyAttrStateCallback::GetValue( &field, nullptr )( nullptr ) ), static_cast< int >( PROPERTY_ATTR_STATE_READONLY ) );
	TEST_EQ( MSaveBehavior::GetValue( &field, 0 ), 1 );
	TEST_TRUE( MPropertyHideField::IsPresent( &field ) );
	TEST_FALSE( MNotSaved::IsPresent( &field ) );

	TEST_TRUE( MPropertyEditClassAsString::GetPtr( &classInfo, SCHEMA_BASE_TRAVERSAL_NONE ) == &classAsString );
	TEST_EQ( MResourceTypeForInfoType::GetValue( &classInfo, 0, SCHEMA_BASE_TRAVERSAL_NONE ), nResourceType );
}

static const KV3TransferDefaultKeys_t *TestSchema_GetDefaultKeys()
{
	static const KV3TransferDefaultKeys_t s_DefaultKeys = { nullptr, nullptr };

	return &s_DefaultKeys;
}

REGISTER_NAMED_TEST( "SchemaMetaTag.KV3TransferTags", SchemaMetaTag_KV3TransferTags )
{
	CKV3TransferSchemaClass::GetDefaultKeysFn_t pfnGetDefaultKeys = &TestSchema_GetDefaultKeys;
	const char *pszSaveOps = "GetTestSaveRestoreOps";

	SchemaMetadataEntryData_t classMetadata[] = { { "MGetKV3ClassDefaults", &pfnGetDefaultKeys } };
	SchemaMetadataEntryData_t fieldMetadata[] = { { "MNotSaved", nullptr }, { "MKV3TransferSaveOpsForField", &pszSaveOps } };

	CSchemaClassInfo classInfo = {};
	classInfo.m_nStaticMetadataCount = ARRAYSIZE( classMetadata );
	classInfo.m_pStaticMetadata = classMetadata;

	CSchemaClassField field = {};
	field.m_nStaticMetadataCount = ARRAYSIZE( fieldMetadata );
	field.m_pStaticMetadata = fieldMetadata;

	TEST_TRUE( MGetKV3ClassDefaults::GetValue( &classInfo, nullptr, SCHEMA_BASE_TRAVERSAL_NONE ) == &TestSchema_GetDefaultKeys );
	TEST_TRUE( MNotSaved::IsPresent( &field ) );
	TEST_EQ( V_strcmp( MKV3TransferSaveOpsForField::GetValue( &field, "" ), "GetTestSaveRestoreOps" ), 0 );

	// Codegen tags never reach the runtime metadata
	TEST_EQ( V_strcmp( MEmitKV3Transfer::TagName(), "MEmitKV3Transfer" ), 0 );
	TEST_EQ( MTestSchemaEmit::MetaFlags(), static_cast< uint >( META_TAG_ON_CLASS ) );
	TEST_FALSE( MEmitKV3Transfer::IsPresent( &field ) );
}
