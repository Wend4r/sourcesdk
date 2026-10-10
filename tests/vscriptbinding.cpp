#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <mathlib/vector.h>
#include <vscript/ivscript.h>

#include <cstring>

static int ScriptBindingTest_Add( int a, int b )
{
	return a + b;
}

static Vector ScriptBindingTest_MakeVector( float x )
{
	return Vector( x, x * 2.0f, x * 3.0f );
}

class CScriptBindingTestObject
{
public:
	int Scale( int nValue ) { return nValue * m_nScale; }

	int m_nScale = 3;
};

// Stands in for IScriptVM, ScriptRegisterFunction only needs RegisterFunction
struct ScriptBindingTestVM_t
{
	void RegisterFunction( ScriptFunctionBinding_t *pBinding ) { m_pBinding = pBinding; m_nRegistered++; }

	ScriptFunctionBinding_t *m_pBinding = nullptr;
	int m_nRegistered = 0;
};

REGISTER_NAMED_TEST( "ScriptFunctionBinding.InitFunctionBinding", ScriptFunctionBinding_InitFunctionBinding )
{
	// The binding macros should set every member of the binding, even over garbage.
	ScriptFunctionBinding_t binding;
	memset( static_cast< void * >( &binding ), 0xCC, sizeof( binding ) );

	ScriptInitFunctionBindingNamed( &binding, ScriptBindingTest_Add, "Add" );

	TEST_NULL( binding.m_pClassDesc );
	TEST_EQ( binding.m_flags, SF_MEMBER_NONE );
	TEST_NOT_NULL( binding.m_pfnBinding );
	TEST_EQ( binding.m_pFunction, (void *)&ScriptBindingTest_Add );
	TEST_EQ( std::strcmp( binding.m_desc.m_pszScriptName, "Add" ), 0 );
	TEST_EQ( binding.m_desc.m_iParamCount, 2 );
	TEST_EQ( binding.m_desc.m_ReturnType, FIELD_INT32 );
	TEST_EQ( binding.m_desc.m_Parameters[ 0 ], FIELD_INT32 );
	TEST_EQ( binding.m_desc.m_Parameters[ 1 ], FIELD_INT32 );

	ScriptVariant_t args[ 2 ] = { ScriptVariant_t( 2 ), ScriptVariant_t( 5 ) };
	ScriptVariant_t result;

	TEST_TRUE( binding.m_pfnBinding( binding.m_pFunction, nullptr, args, 2, &result ) );
	TEST_EQ( result.FieldType(), FIELD_INT32 );
	TEST_EQ( static_cast< int >( result ), 7 );
}

REGISTER_NAMED_TEST( "ScriptFunctionBinding.RegisterFunction", ScriptFunctionBinding_RegisterFunction )
{
	// ScriptRegisterFunction should describe the function and hand its binding to the VM.
	ScriptBindingTestVM_t vm;

	ScriptRegisterFunctionNamed( ( &vm ), ScriptBindingTest_Add, "Add", "Adds two integers" );

	TEST_EQ( vm.m_nRegistered, 1 );
	TEST_NOT_NULL( vm.m_pBinding );
	TEST_EQ( std::strcmp( vm.m_pBinding->m_desc.m_pszDescription, "Adds two integers" ), 0 );
	TEST_EQ( vm.m_pBinding->m_desc.m_iParamCount, 2 );
	TEST_NULL( vm.m_pBinding->m_pClassDesc );
}

REGISTER_NAMED_TEST( "ScriptFunctionBinding.VectorReturn", ScriptFunctionBinding_VectorReturn )
{
	// A returned Vector should be copied into the variant, not point at the destroyed temporary.
	ScriptFunctionBinding_t binding;

	ScriptInitFunctionBindingNamed( &binding, ScriptBindingTest_MakeVector, "MakeVector" );
	TEST_EQ( binding.m_desc.m_ReturnType, FIELD_VECTOR );

	ScriptVariant_t arg( 2.0f );
	ScriptVariant_t result;

	TEST_TRUE( binding.m_pfnBinding( binding.m_pFunction, nullptr, &arg, 1, &result ) );

	// Reuse the stack the temporary lived on.
	volatile char clobber[ 256 ];
	memset( const_cast< char * >( clobber ), 0x7F, sizeof( clobber ) );

	TEST_EQ( result.FieldType(), FIELD_VECTOR );

	const Vector &vec = result;
	TEST_EQ( vec.x, 2.0f );
	TEST_EQ( vec.y, 4.0f );
	TEST_EQ( vec.z, 6.0f );
}

REGISTER_NAMED_TEST( "ScriptFunctionBinding.ClassDescMember", ScriptFunctionBinding_ClassDescMember )
{
	// A member function added to a class desc should point back at it and call through the object.
	ScriptClassDesc_t desc;

	ScriptInitClassDescNoBase( &desc, CScriptBindingTestObject );
	ScriptAddFunctionToClassDesc( &desc, CScriptBindingTestObject, Scale, "Scales a value" );

	TEST_EQ( desc.m_FunctionBindings.Count(), 1 );

	ScriptFunctionBinding_t &binding = desc.m_FunctionBindings[ 0 ];

	TEST_EQ( binding.m_pClassDesc, &desc );
	TEST_EQ( binding.m_flags, SF_MEMBER_FUNC );
	TEST_EQ( binding.m_desc.m_iParamCount, 1 );

	CScriptBindingTestObject object;
	ScriptVariant_t arg( 4 );
	ScriptVariant_t result;

	TEST_TRUE( binding.m_pfnBinding( binding.m_pFunction, &object, &arg, 1, &result ) );
	TEST_EQ( static_cast< int >( result ), 12 );
}
