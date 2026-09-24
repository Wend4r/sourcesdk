#include "scriptkeyvalues.h"
#include "entitykeyvalues.h"
#include "tier1/keyvalues3.h"
#include "vscript/ivscript.h"

BEGIN_SCRIPTDESC_ROOT_NAMED( CScriptKeyValues, "CScriptKeyValues", "Container to hold keyvalues published to spawn functions in script" )
	DEFINE_SCRIPTFUNC_NAMED( GetValue, "GetValue", "Reads a spawn key" )
END_SCRIPTDESC()

HSCRIPT CScriptKeyValues::RegisterScriptInstance()
{
	return g_pScriptVM->RegisterInstance( ::GetScriptDesc( this ), this );
}

CVariant CScriptKeyValues::GetValue( const char *pszKey )
{
	CVariant value;

	// The game hashes an empty key to 0 instead of hashing the empty string.
	EntityKeyId_t key = ( pszKey && *pszKey ) ? EntityKeyId_t( pszKey, -1 ) : EntityKeyId_t( 0u, UTL_INVAL_SYMBOL_LARGE, pszKey ? pszKey : "" );
	const KeyValues3 *pKV = m_pKeyValues->GetKeyValue( key );

	if ( !pKV )
		return value;

	switch ( pKV->GetType() )
	{
		case KV3_TYPE_INT:
		{
			value = CVariant( static_cast< float64 >( pKV->GetInt64() ) );

			break;
		}

		case KV3_TYPE_UINT:
		{
			value = CVariant( pKV->GetSubType() == KV3_SUBTYPE_POINTER ? 0.0 : static_cast< float64 >( pKV->GetUInt64() ) );

			break;
		}

		default:
		{
			KeyValues3ToVariant( pKV, &value );

			break;
		}
	}

	return value;
}

bool CScriptKeyValues::KeyValues3ToVariant( const KeyValues3 *pKV, CVariant *pValue )
{
	switch ( pKV->GetType() )
	{
		case KV3_TYPE_NULL:
		{
			*pValue = CVariant();

			return true;
		}

		case KV3_TYPE_BOOL:
		{
			*pValue = CVariant( pKV->GetBool() );

			return true;
		}

		case KV3_TYPE_INT:
		{
			int64 nValue = pKV->GetInt64();

			if ( nValue < INT32_MIN || nValue > INT32_MAX )
				*pValue = CVariant( nValue );
			else
				*pValue = CVariant( static_cast< int32 >( nValue ) );

			return true;
		}

		case KV3_TYPE_UINT:
		{
			switch ( pKV->GetSubType() )
			{
				case KV3_SUBTYPE_POINTER:
				{
					*pValue = CVariant( static_cast< uint32 >( 0 ) );

					break;
				}

				case KV3_SUBTYPE_EHANDLE:
				{
					*pValue = CVariant( pKV->GetEHandle() );

					break;
				}

				default:
				{
					uint64 nValue = pKV->GetUInt64();

					if ( nValue >> 32 )
						*pValue = CVariant( nValue );
					else
						*pValue = CVariant( static_cast< uint32 >( nValue ) );

					break;
				}
			}

			return true;
		}

		case KV3_TYPE_DOUBLE:
		{
			*pValue = CVariant( pKV->GetDouble() );

			return true;
		}

		case KV3_TYPE_STRING:
		{
			*pValue = CVariant( pKV->GetString(), false );

			return true;
		}

		case KV3_TYPE_ARRAY:
		{
			// Only an array of exactly three doubles, generic or packed as floats or doubles, becomes a vector.
			if ( pKV->GetArrayElementCount() != 3 )
				return false;

			switch ( pKV->GetTypeEx() )
			{
				case KV3_TYPEEX_ARRAY:
				{
					for ( int i = 0; i < 3; ++i )
					{
						if ( pKV->GetArrayElement( i )->GetType() != KV3_TYPE_DOUBLE )
							return false;
					}

					break;
				}

				case KV3_TYPEEX_ARRAY_FLOAT32:
				case KV3_TYPEEX_ARRAY_FLOAT64:
				{
					break;
				}

				default:
				{
					return false;
				}
			}

			*pValue = CVariant( pKV->GetVector() );

			return true;
		}

		default:
		{
			return false;
		}
	}
}
