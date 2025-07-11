//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "BaseVSShader.h"

#include "screenspaceeffect_vs20.inc"
#include "floatcombine_ps20.inc"
#include "floatcombine_ps20b.inc"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"


BEGIN_VS_SHADER_FLAGS( floatcombine, "Help for floatcombine", SHADER_NOT_EDITABLE )
	BEGIN_SHADER_PARAMS
		SHADER_PARAM( BLOOMTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "" )
		SHADER_PARAM( SHARPNESS, SHADER_PARAM_TYPE_FLOAT, "1", "" )
	    SHADER_PARAM( WOODCUT, SHADER_PARAM_TYPE_FLOAT, "0", "" )
		SHADER_PARAM( VIGNETTE_MIN_BRIGHT,SHADER_PARAM_TYPE_FLOAT,"1","")
		SHADER_PARAM( VIGNETTE_POWER,SHADER_PARAM_TYPE_FLOAT,"4","")
		SHADER_PARAM( EDGE_SOFTNESS,SHADER_PARAM_TYPE_FLOAT,"0","")
	    SHADER_PARAM( BLOOMAMOUNT, SHADER_PARAM_TYPE_FLOAT, "1.0", "" )
		SHADER_PARAM( BLOOMEXPONENT, SHADER_PARAM_TYPE_FLOAT, "2.0", "" )
		SHADER_PARAM( ALPHASHARPENFACTOR, SHADER_PARAM_TYPE_FLOAT, "0.0", "" )
	END_SHADER_PARAMS

	SHADER_INIT
	{
		if( params[BASETEXTURE]->IsDefined() )
		{
			LoadTexture( BASETEXTURE );
		}
		if( params[BLOOMTEXTURE]->IsDefined() )
		{
			LoadTexture( BLOOMTEXTURE );
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

			pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );
			pShaderShadow->EnableTexture( SHADER_SAMPLER1, true );
			int fmt = VERTEX_POSITION;
			pShaderShadow->VertexShaderVertexFormat( fmt, 1, 0, 0 );

			// convert from linear to gamma on write.
			pShaderShadow->EnableSRGBWrite( true );

			// Pre-cache shaders
			DECLARE_STATIC_VERTEX_SHADER( screenspaceeffect_vs20 );
			SET_STATIC_VERTEX_SHADER( screenspaceeffect_vs20 );

			if( g_pHardwareConfig->SupportsPixelShaders_2_b() )
			{
				DECLARE_STATIC_PIXEL_SHADER( floatcombine_ps20b );
				SET_STATIC_PIXEL_SHADER( floatcombine_ps20b );
			}
			else
			{
				DECLARE_STATIC_PIXEL_SHADER( floatcombine_ps20 );
				SET_STATIC_PIXEL_SHADER( floatcombine_ps20 );
			}
		}

		DYNAMIC_STATE
		{
			float c0[4]={params[SHARPNESS]->GetFloatValue(),
						 params[WOODCUT]->GetFloatValue(),
						 params[BLOOMAMOUNT]->GetFloatValue(),
						 params[ALPHASHARPENFACTOR]->GetFloatValue()};
			float c1[4]={params[BLOOMEXPONENT]->GetFloatValue(),
						 params[VIGNETTE_MIN_BRIGHT]->GetFloatValue(),
						 params[VIGNETTE_POWER]->GetFloatValue(),
						 params[EDGE_SOFTNESS]->GetFloatValue()};
			pShaderAPI->SetPixelShaderConstant( 0, c0, 1 );
			pShaderAPI->SetPixelShaderConstant( 1, c1, 1 );
			BindTexture( SHADER_SAMPLER0, BASETEXTURE, -1 );
			BindTexture( SHADER_SAMPLER1, BLOOMTEXTURE, -1 );

			ITexture *base_texture=params[BASETEXTURE]->GetTextureValue();
			ITexture *bloom_texture=params[BLOOMTEXTURE]->GetTextureValue();

			float v0[4]={1.0/base_texture->GetActualWidth(),1.0/base_texture->GetActualHeight(),
						 1.0/bloom_texture->GetActualWidth(),1.0/bloom_texture->GetActualHeight()};
			pShaderAPI->SetVertexShaderConstant( VERTEX_SHADER_SHADER_SPECIFIC_CONST_0, v0, 1 );

			DECLARE_DYNAMIC_VERTEX_SHADER( screenspaceeffect_vs20 );
			SET_DYNAMIC_VERTEX_SHADER( screenspaceeffect_vs20 );

			if( g_pHardwareConfig->SupportsPixelShaders_2_b() )
			{
				DECLARE_DYNAMIC_PIXEL_SHADER( floatcombine_ps20b );
				SET_DYNAMIC_PIXEL_SHADER( floatcombine_ps20b );
			}
			else
			{
				DECLARE_DYNAMIC_PIXEL_SHADER( floatcombine_ps20 );
				SET_DYNAMIC_PIXEL_SHADER( floatcombine_ps20 );
			}
		}
		Draw();
	}
END_SHADER
