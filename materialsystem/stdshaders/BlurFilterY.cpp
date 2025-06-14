//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
//===========================================================================//

#include "BaseVSShader.h"
#include "blurfilter_vs20.inc"
#include "blurfilter_ps20.inc"
#include "blurfilter_ps20b.inc"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_VS_SHADER_FLAGS( BlurFilterY, "Help for BlurFilterY", SHADER_NOT_EDITABLE )
	BEGIN_SHADER_PARAMS
        SHADER_PARAM( BLOOMAMOUNT, SHADER_PARAM_TYPE_FLOAT, "1.0", "" )
		SHADER_PARAM( FRAMETEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_SmallHDR0", "" )
		SHADER_PARAM( KERNEL, SHADER_PARAM_TYPE_INTEGER, "0", "Kernel type" )
		SHADER_PARAM( ENABLECLEARCOLOR, SHADER_PARAM_TYPE_BOOL, "0", "clear RGB channels to a solid color" )
		SHADER_PARAM( CLEARCOLOR, SHADER_PARAM_TYPE_VEC3, "[0 0 0]", "clear color" )
	END_SHADER_PARAMS

	SHADER_INIT
	{
		if ( params[BASETEXTURE]->IsDefined() )
		{
			LoadTexture( BASETEXTURE );
		}
		if ( !( params[BLOOMAMOUNT]->IsDefined() ) )
		{
			params[BLOOMAMOUNT]->SetFloatValue(1.0);
		}

		if ( !( params[ KERNEL ]->IsDefined() ) )
		{
			params[ KERNEL ]->SetIntValue( 0 );
		}

		if ( !( params[ ENABLECLEARCOLOR ]->IsDefined() ) )
		{
			params[ ENABLECLEARCOLOR ]->SetIntValue( 0 );
		}

		if ( !( params[ CLEARCOLOR ]->IsDefined() ) )
		{
			params[ CLEARCOLOR ]->SetVecValue( 0.0f, 0.0f, 0.0f );
		}

	}
	
	SHADER_FALLBACK
	{
		return 0;
	}

	SHADER_DRAW
	{
		SHADOW_STATE
		{
			pShaderShadow->EnableDepthWrites( false );
			pShaderShadow->EnableAlphaWrites( true );
			pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );
			pShaderShadow->VertexShaderVertexFormat( VERTEX_POSITION, 1, 0, 0 );

			//avoid srgb conversions to alleviate some of the srgb texture lookup problems
			pShaderShadow->EnableSRGBRead( SHADER_SAMPLER0, false );
			pShaderShadow->EnableSRGBWrite( false );

			DECLARE_STATIC_VERTEX_SHADER( blurfilter_vs20 );
			SET_STATIC_VERTEX_SHADER_COMBO( KERNEL, params[ KERNEL ]->GetIntValue() ? 1 : 0 );
			SET_STATIC_VERTEX_SHADER( blurfilter_vs20 );

			if( g_pHardwareConfig->SupportsPixelShaders_2_b() )
			{
				DECLARE_STATIC_PIXEL_SHADER( blurfilter_ps20b );
				SET_STATIC_PIXEL_SHADER_COMBO( KERNEL, params[ KERNEL ]->GetIntValue() );
				SET_STATIC_PIXEL_SHADER_COMBO( CLEAR_COLOR, params[ ENABLECLEARCOLOR ]->GetIntValue() );
				SET_STATIC_PIXEL_SHADER( blurfilter_ps20b );
			}
			else
			{
				DECLARE_STATIC_PIXEL_SHADER( blurfilter_ps20 );
				SET_STATIC_PIXEL_SHADER_COMBO( KERNEL, params[ KERNEL ]->GetIntValue() );
				SET_STATIC_PIXEL_SHADER_COMBO( CLEAR_COLOR, params[ ENABLECLEARCOLOR ]->GetIntValue() );
				SET_STATIC_PIXEL_SHADER( blurfilter_ps20 );
			}

			if ( IS_FLAG_SET( MATERIAL_VAR_ADDITIVE ) )
				EnableAlphaBlending( SHADER_BLEND_ONE, SHADER_BLEND_ONE );
		}

		DYNAMIC_STATE
		{
			BindTexture( SHADER_SAMPLER0, BASETEXTURE, -1 );

			// The temp buffer is 1/4 back buffer size
			ITexture *src_texture = params[BASETEXTURE]->GetTextureValue();
			int height = src_texture->GetActualHeight();
			float dY = 1.0f / height;
//			dY *= 0.4;
			float v[4];

			// Tap offsets
			v[0] = 0.0f;
			v[1] = 1.3366f * dY;
			v[2] = 0;
			v[3] = 0;
			pShaderAPI->SetVertexShaderConstant( VERTEX_SHADER_SHADER_SPECIFIC_CONST_0, v, 1 );
			v[0] = 0.0f;
			v[1] = 3.4295f * dY;
			pShaderAPI->SetVertexShaderConstant( VERTEX_SHADER_SHADER_SPECIFIC_CONST_1, v, 1 );
			v[0] = 0.0f;
			v[1] = 5.4264f * dY;
			pShaderAPI->SetVertexShaderConstant( VERTEX_SHADER_SHADER_SPECIFIC_CONST_2, v, 1 );

			v[0] = 0.0f;
			v[1] = 7.4359f * dY;
			pShaderAPI->SetPixelShaderConstant( 0, v, 1 );
			v[0] = 0.0f;
			v[1] = 9.4436f * dY;
			pShaderAPI->SetPixelShaderConstant( 1, v, 1 );
			v[0] = 0.0f;
			v[1] = 11.4401f * dY;
			pShaderAPI->SetPixelShaderConstant( 2, v, 1 );

			v[0] = v[1] = v[2] = params[BLOOMAMOUNT]->GetFloatValue();
			pShaderAPI->SetPixelShaderConstant( 3, v, 1 );

			v[0] = v[1] = v[2] = v[3] = 0.0;
			v[1] = dY;
			pShaderAPI->SetPixelShaderConstant( 4, v, 1 );

			params[CLEARCOLOR]->GetVecValue( v, 3 );
			v[3] = 0.0f;
			pShaderAPI->SetPixelShaderConstant( 5, v, 1 );

			DECLARE_DYNAMIC_VERTEX_SHADER( blurfilter_ps20 );
			SET_DYNAMIC_VERTEX_SHADER( blurfilter_ps20 );

			if( g_pHardwareConfig->SupportsPixelShaders_2_b() )
			{
				DECLARE_DYNAMIC_PIXEL_SHADER( blurfilter_ps20b );
				SET_DYNAMIC_PIXEL_SHADER( blurfilter_ps20b );
			}
			else
			{
				DECLARE_DYNAMIC_PIXEL_SHADER( blurfilter_ps20 );
				SET_DYNAMIC_PIXEL_SHADER( blurfilter_ps20 );
			}
		}
		Draw();
	}
END_SHADER
