#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/quantizedfloatencoder.h>
#include <tier0/strtools.h>
#include <tier0/utlvector.h>

#include <cmath>
#include <cstddef>

REGISTER_NAMED_TEST( "FindOrCreateQuantizedFloatEncoder.Encoder", FindOrCreateQuantizedFloatEncoder_Encoder )
{
	QuantizedFloatEncoderReg_t reg = {};
	FindOrCreateQuantizedFloatEncoder( &reg, true, "m_flSourceSDKTest", 10, QFE_ENCODE_ZERO_EXACTLY, -64.0f, 64.0f );

	// The caller marks the handle registered, tier0 keeps the bit.
	TEST_FALSE( reg.m_bRegistered );

	const QuantizedFloatEncoder_t *pEncoder = GetQuantizedFloatEncoderByReg( &reg );
	TEST_NOT_NULL( pEncoder );
	TEST_EQ( pEncoder->m_flLowValue, -64.0f );
	TEST_EQ( pEncoder->m_flHighValue, 64.0f );
	TEST_EQ( ( int )pEncoder->m_nBitCount, 10 );
	TEST_EQ( ( int )pEncoder->m_nFlags, ( int )QFE_ENCODE_ZERO_EXACTLY );
	TEST_EQ( ( int )pEncoder->m_nDeclaredFlags, ( int )QFE_ENCODE_ZERO_EXACTLY );
	TEST_FALSE( pEncoder->m_bNoScale );
	TEST_TRUE( pEncoder->m_bInitialized );

	// 1023 steps over the range.
	TEST_TRUE( fabs( pEncoder->m_flHighLowMul - 1023.0 / 128.0 ) < 1e-6 );
	TEST_TRUE( fabs( pEncoder->m_flDecodeMul - 1.0 / 1023.0 ) < 1e-9 );

	// A bit count outside [ 1, 31 ] does not scale.
	QuantizedFloatEncoderReg_t noScale = {};
	FindOrCreateQuantizedFloatEncoder( &noScale, true, "m_flSourceSDKNoScale", 0, 0, 0.0f, 1.0f );
	pEncoder = GetQuantizedFloatEncoderByReg( &noScale );
	TEST_EQ( ( int )pEncoder->m_nBitCount, 32 );
	TEST_TRUE( pEncoder->m_bNoScale );
	TEST_TRUE( pEncoder->m_bInitialized );
}

REGISTER_NAMED_TEST( "FindOrCreateQuantizedFloatEncoder.SharedDefinition", FindOrCreateQuantizedFloatEncoder_SharedDefinition )
{
	QuantizedFloatEncoderReg_t first = {}, second = {}, other = {};
	FindOrCreateQuantizedFloatEncoder( &first, true, "m_flSourceSDKSharedA", 12, QFE_ROUNDDOWN, 0.0f, 100.0f );
	FindOrCreateQuantizedFloatEncoder( &second, true, "m_flSourceSDKSharedB", 12, QFE_ROUNDDOWN, 0.0f, 100.0f );
	FindOrCreateQuantizedFloatEncoder( &other, true, "m_flSourceSDKSharedC", 11, QFE_ROUNDDOWN, 0.0f, 100.0f );

	// One encoder per definition, the names stay per field.
	TEST_EQ( first.m_unEncoder, second.m_unEncoder );
	TEST_TRUE( first.m_unEncoder != other.m_unEncoder );
	TEST_TRUE( first.m_nFieldName != second.m_nFieldName );
}

REGISTER_NAMED_TEST( "GetQuantizedFloatEncoderByReg.SameEncoder", GetQuantizedFloatEncoderByReg_SameEncoder )
{
	QuantizedFloatEncoderReg_t first = {}, second = {};
	const QuantizedFloatEncoder_t *pFirst = RegisterQuantizedFloatEncoder( &first, "m_flSourceSDKByRegA", 9, 0, -1.0f, 1.0f );
	TEST_TRUE( RegisterQuantizedFloatEncoder( &second, "m_flSourceSDKByRegB", 9, 0, -1.0f, 1.0f ) == pFirst );
}

REGISTER_NAMED_TEST( "GetQuantizedFloatEncoderNameByReg.FieldName", GetQuantizedFloatEncoderNameByReg_FieldName )
{
	QuantizedFloatEncoderReg_t reg = {};
	RegisterQuantizedFloatEncoder( &reg, "m_flSourceSDKNamed", 8, 0, 0.0f, 10.0f );
	TEST_EQ( V_strcmp( GetQuantizedFloatEncoderNameByReg( &reg ), "m_flSourceSDKNamed" ), 0 );
}

REGISTER_NAMED_TEST( "GetQuantizedFloatEncodersSharingReg.FieldNames", GetQuantizedFloatEncodersSharingReg_FieldNames )
{
	QuantizedFloatEncoderReg_t first = {}, second = {};
	RegisterQuantizedFloatEncoder( &first, "m_flSourceSDKSharingA", 13, QFE_ROUNDUP, -5.0f, 5.0f );
	RegisterQuantizedFloatEncoder( &second, "m_flSourceSDKSharingB", 13, QFE_ROUNDUP, -5.0f, 5.0f );

	CUtlVector< const char * > names;
	GetQuantizedFloatEncodersSharingReg( &first, &names );
	TEST_EQ( names.Count(), 2 );

	bool bFoundA = false, bFoundB = false;
	for ( const char *pszName : names )
	{
		bFoundA = bFoundA || V_strcmp( pszName, "m_flSourceSDKSharingA" ) == 0;
		bFoundB = bFoundB || V_strcmp( pszName, "m_flSourceSDKSharingB" ) == 0;
	}

	TEST_TRUE( bFoundA && bFoundB );
}

REGISTER_NAMED_TEST( "RegisterQuantizedFloatEncoder.Once", RegisterQuantizedFloatEncoder_Once )
{
	QuantizedFloatEncoderReg_t reg = {};
	const QuantizedFloatEncoder_t *pEncoder = RegisterQuantizedFloatEncoder( &reg, "m_flSourceSDKOnce", 6, 0, 0.0f, 2.0f );
	TEST_TRUE( reg.m_bRegistered );
	TEST_EQ( ( int )pEncoder->m_nBitCount, 6 );

	// A registered handle keeps its encoder.
	TEST_TRUE( RegisterQuantizedFloatEncoder( &reg, "m_flSourceSDKOnce", 12, 0, 0.0f, 9.0f ) == pEncoder );
}

static const QuantizedFloatEncoder_t *GetTestMacroEncoder( float flHighValue )
{
	return QUANTIZED_FLOAT_ENCODER( "m_flSourceSDKMacro", 5, 0, 0.0f, flHighValue );
}

REGISTER_NAMED_TEST( "QUANTIZED_FLOAT_ENCODER.HandleOfThePlace", QUANTIZED_FLOAT_ENCODER_HandleOfThePlace )
{
	// The first call at the place registers, the later ones reuse it.
	const QuantizedFloatEncoder_t *pEncoder = GetTestMacroEncoder( 3.0f );
	TEST_NOT_NULL( pEncoder );
	TEST_EQ( pEncoder->m_flHighValue, 3.0f );
	TEST_TRUE( GetTestMacroEncoder( 7.0f ) == pEncoder );
}

REGISTER_NAMED_TEST( "RegisterQuantizedFloatEncoderAlias.Once", RegisterQuantizedFloatEncoderAlias_Once )
{
	QuantizedFloatEncoderDef_t def = { 9, 0, -3.0f, 3.0f, false };
	QuantizedFloatEncoderReg_t reg = {};
	const QuantizedFloatEncoder_t *pEncoder = RegisterQuantizedFloatEncoderAlias( &reg, "m_flSourceSDKAliasOnce", "sourcesdk_alias_once", &def );
	TEST_TRUE( reg.m_bRegistered );
	TEST_EQ( pEncoder->m_flLowValue, -3.0f );

	QuantizedFloatEncoderDef_t found = {};
	TEST_TRUE( FindQuantizedFloatEncoderAlias( "sourcesdk_alias_once", &found ) >= 0 );
	TEST_EQ( found.m_nBits, 9 );
	TEST_EQ( FindQuantizedFloatEncoderAlias( "sourcesdk_alias_missing", &found ), -1 );
}

REGISTER_NAMED_TEST( "FindOrCreateQuantizedFloatEncoderAlias.Definition", FindOrCreateQuantizedFloatEncoderAlias_Definition )
{
	const int nAliases = GetQuantizedFloatEncoderAliasCount();

	QuantizedFloatEncoderDef_t def = { 8, 0, 0.0f, 1.0f, false };
	QuantizedFloatEncoderReg_t reg = {};
	FindOrCreateQuantizedFloatEncoderAlias( &reg, "m_flSourceSDKAliased", "sourcesdk_alias_definition", &def );
	TEST_EQ( GetQuantizedFloatEncoderAliasCount(), nAliases + 1 );

	const QuantizedFloatEncoder_t *pEncoder = GetQuantizedFloatEncoderByReg( &reg );
	TEST_EQ( ( int )pEncoder->m_nBitCount, 8 );
	TEST_EQ( pEncoder->m_flHighValue, 1.0f );

	// A later field of the alias keeps the first definition.
	QuantizedFloatEncoderDef_t otherDef = { 10, 0, 0.0f, 4.0f, false };
	QuantizedFloatEncoderReg_t second = {};
	FindOrCreateQuantizedFloatEncoderAlias( &second, "m_flSourceSDKAliased2", "sourcesdk_alias_definition", &otherDef );
	TEST_EQ( GetQuantizedFloatEncoderAliasCount(), nAliases + 1 );
	TEST_EQ( ( int )GetQuantizedFloatEncoderByReg( &second )->m_nBitCount, 8 );
}

REGISTER_NAMED_TEST( "GetQuantizedFloatEncoderAlias.NameAndDefinition", GetQuantizedFloatEncoderAlias_NameAndDefinition )
{
	QuantizedFloatEncoderDef_t def = { 7, ( int )QFE_ROUNDDOWN, -2.0f, 2.0f, false };
	QuantizedFloatEncoderReg_t reg = {};
	FindOrCreateQuantizedFloatEncoderAlias( &reg, "m_flSourceSDKGetAlias", "sourcesdk_alias_get", &def );

	QuantizedFloatEncoderDef_t found = {};
	TEST_TRUE( FindQuantizedFloatEncoderAlias( "sourcesdk_alias_get", &found ) >= 0 );
	TEST_EQ( found.m_nBits, 7 );
	TEST_EQ( found.m_nFlags, ( int )QFE_ROUNDDOWN );
	TEST_EQ( found.m_flLowValue, -2.0f );
	TEST_EQ( found.m_flHighValue, 2.0f );

	// Out of range.
	TEST_FALSE( GetQuantizedFloatEncoderAlias( -1, nullptr, &found ) );
	TEST_FALSE( GetQuantizedFloatEncoderAlias( GetQuantizedFloatEncoderAliasCount(), nullptr, &found ) );
}

REGISTER_NAMED_TEST( "GetQuantizedFloatEncoderAliasCount.Grows", GetQuantizedFloatEncoderAliasCount_Grows )
{
	const int nAliases = GetQuantizedFloatEncoderAliasCount();
	TEST_TRUE( nAliases >= 0 );

	QuantizedFloatEncoderDef_t def = { 6, 0, 0.0f, 8.0f, false };
	QuantizedFloatEncoderReg_t reg = {};
	FindOrCreateQuantizedFloatEncoderAlias( &reg, "m_flSourceSDKCount", "sourcesdk_alias_count", &def );
	TEST_EQ( GetQuantizedFloatEncoderAliasCount(), nAliases + 1 );
}

REGISTER_NAMED_TEST( "OverrideQuantizedFloatEncoderAlias.ReplacesDefinition", OverrideQuantizedFloatEncoderAlias_ReplacesDefinition )
{
	QuantizedFloatEncoderDef_t def = { 8, 0, 0.0f, 1.0f, false };
	QuantizedFloatEncoderReg_t reg = {};
	FindOrCreateQuantizedFloatEncoderAlias( &reg, "m_flSourceSDKOverride", "sourcesdk_alias_override", &def );

	QuantizedFloatEncoderDef_t override = { 12, 0, 0.0f, 2.0f, false };
	OverrideQuantizedFloatEncoderAlias( "sourcesdk_alias_override", &override );

	QuantizedFloatEncoderDef_t found = {};
	TEST_TRUE( FindQuantizedFloatEncoderAlias( "sourcesdk_alias_override", &found ) >= 0 );
	TEST_EQ( found.m_nBits, 12 );
	TEST_EQ( found.m_flHighValue, 2.0f );

	// New fields of the alias take the override.
	QuantizedFloatEncoderReg_t second = {};
	FindOrCreateQuantizedFloatEncoderAlias( &second, "m_flSourceSDKOverride2", "sourcesdk_alias_override", &def );
	TEST_EQ( ( int )GetQuantizedFloatEncoderByReg( &second )->m_nBitCount, 12 );

	// An unusable definition is ignored with a warning.
	QuantizedFloatEncoderDef_t unusable = { 40, 0, 0.0f, 1.0f, false };
	OverrideQuantizedFloatEncoderAlias( "sourcesdk_alias_override", &unusable );
	TEST_TRUE( FindQuantizedFloatEncoderAlias( "sourcesdk_alias_override", &found ) >= 0 );
	TEST_EQ( found.m_nBits, 12 );

	ResetQuantizedFloatEncoderAliasOverrides();
}

REGISTER_NAMED_TEST( "RegisterQuantizedFloatEncoderAliasTuning.UnderOverrides", RegisterQuantizedFloatEncoderAliasTuning_UnderOverrides )
{
	QuantizedFloatEncoderDef_t def = { 8, 0, 0.0f, 1.0f, false };
	QuantizedFloatEncoderReg_t reg = {};
	FindOrCreateQuantizedFloatEncoderAlias( &reg, "m_flSourceSDKTuning", "sourcesdk_alias_tuning", &def );

	QuantizedFloatEncoderDef_t override = { 12, 0, 0.0f, 2.0f, false };
	QuantizedFloatEncoderDef_t tuning = { 16, 0, 0.0f, 3.0f, false };
	OverrideQuantizedFloatEncoderAlias( "sourcesdk_alias_tuning", &override );
	RegisterQuantizedFloatEncoderAliasTuning( "sourcesdk_alias_tuning", &tuning );

	// The override wins over the tuning.
	QuantizedFloatEncoderDef_t found = {};
	TEST_TRUE( FindQuantizedFloatEncoderAlias( "sourcesdk_alias_tuning", &found ) >= 0 );
	TEST_EQ( found.m_nBits, 12 );

	// Without the overrides the tuning remains.
	ResetQuantizedFloatEncoderAliasOverrides();
	TEST_TRUE( FindQuantizedFloatEncoderAlias( "sourcesdk_alias_tuning", &found ) >= 0 );
	TEST_EQ( found.m_nBits, 16 );
	TEST_EQ( found.m_flHighValue, 3.0f );
}

REGISTER_NAMED_TEST( "ResetQuantizedFloatEncoderAliasOverrides.BackToDefinition", ResetQuantizedFloatEncoderAliasOverrides_BackToDefinition )
{
	QuantizedFloatEncoderDef_t def = { 9, 0, 0.0f, 1.0f, false };
	QuantizedFloatEncoderReg_t reg = {};
	FindOrCreateQuantizedFloatEncoderAlias( &reg, "m_flSourceSDKReset", "sourcesdk_alias_reset", &def );

	QuantizedFloatEncoderDef_t override = { 14, 0, 0.0f, 2.0f, false };
	OverrideQuantizedFloatEncoderAlias( "sourcesdk_alias_reset", &override );
	ResetQuantizedFloatEncoderAliasOverrides();

	QuantizedFloatEncoderDef_t found = {};
	TEST_TRUE( FindQuantizedFloatEncoderAlias( "sourcesdk_alias_reset", &found ) >= 0 );
	TEST_EQ( found.m_nBits, 9 );
	TEST_EQ( found.m_flHighValue, 1.0f );
}

REGISTER_NAMED_TEST( "QuantizedFloatEncoder_t.Layout", QuantizedFloatEncoder_t_Layout )
{
	TEST_EQ( sizeof( QuantizedFloatEncoder_t ), ( size_t )32 );
	TEST_EQ( offsetof( QuantizedFloatEncoder_t, m_flHighLowMul ), ( size_t )8 );
	TEST_EQ( offsetof( QuantizedFloatEncoder_t, m_flOffset ), ( size_t )24 );
	TEST_EQ( offsetof( QuantizedFloatEncoder_t, m_nBitCount ), ( size_t )28 );
	TEST_EQ( sizeof( QuantizedFloatEncoderDef_t ), ( size_t )20 );
	TEST_EQ( sizeof( QuantizedFloatEncoderReg_t ), ( size_t )4 );
	TEST_EQ( ( int )QFE_ENCODE_INTEGERS_EXACTLY, 8 );
}
