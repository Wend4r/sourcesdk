//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Header: $
// $NoKeywords: $
//=============================================================================//

//Note: Not upgraded to vs/ps 2.0 fxc's because this shader is unused and there are no test cases to verify against.
#include "BaseVSShader.h"

#if !defined( _X360 )
#include "shadowmodel.inc"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DEFINE_FALLBACK_SHADER( ShadowModel, ShadowModel_DX9 )



#if !defined( _X360 ) //not used for anything at time of 360 ship, and we want to avoid storing/loading assembly shaders

//PC version
BEGIN_VS_SHADER_FLAGS( ShadowModel_DX9, "Help for ShadowModel", SHADER_NOT_EDITABLE )

BEGIN_SHADER_PARAMS
SHADER_PARAM( BASETEXTUREOFFSET, SHADER_PARAM_TYPE_VEC2, "[0 0]", "$baseTexture texcoord offset" )
SHADER_PARAM( BASETEXTURESCALE, SHADER_PARAM_TYPE_VEC2, "[1 1]", "$baseTexture texcoord scale" )
SHADER_PARAM( FALLOFFOFFSET, SHADER_PARAM_TYPE_FLOAT, "0", "Distance at which shadow starts to fade" )
SHADER_PARAM( FALLOFFDISTANCE, SHADER_PARAM_TYPE_FLOAT, "100", "Max shadow distance" )
SHADER_PARAM( FALLOFFAMOUNT, SHADER_PARAM_TYPE_FLOAT, "0.9", "Amount to brighten the shadow at max dist" )
END_SHADER_PARAMS

SHADER_INIT_PARAMS()
{
	if (!params[BASETEXTURESCALE]->IsDefined())
	{
		Vector2D scale(1, 1);
		params[BASETEXTURESCALE]->SetVecValue( scale.Base(), 2 );
	}

	if (!params[FALLOFFDISTANCE]->IsDefined())
		params[FALLOFFDISTANCE]->SetFloatValue( 100.0f );

	if (!params[FALLOFFAMOUNT]->IsDefined())
		params[FALLOFFAMOUNT]->SetFloatValue( 0.9f );
}

SHADER_FALLBACK
{
	return 0;
}

SHADER_INIT
{
	if (params[BASETEXTURE]->IsDefined())
		LoadTexture( BASETEXTURE );
}

SHADER_DRAW
{
	SHADOW_STATE
	{
		// Base texture on stage 0
		pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );

		// Multiplicative blending state...
		EnableAlphaBlending( SHADER_BLEND_DST_COLOR, SHADER_BLEND_ZERO );

		int fmt = VERTEX_POSITION | VERTEX_NORMAL;
		pShaderShadow->VertexShaderVertexFormat( fmt, 1, 0, 0 );

		shadowmodel_Static_Index vshIndex;
		pShaderShadow->SetVertexShader( "ShadowModel", vshIndex.GetIndex() );

		pShaderShadow->SetPixelShader( "ShadowModel" );

		// We need to fog to *white* regardless of overbrighting...
		FogToWhite();

		// The constant color is the shadow color...
		PI_BeginCommandBuffer();
		PI_SetModulationVertexShaderDynamicState();
		PI_EndCommandBuffer();
	}
	DYNAMIC_STATE
	{
		BindTexture( SHADER_SAMPLER0, BASETEXTURE, FRAME );
		SetVertexShaderMatrix3x4( VERTEX_SHADER_SHADER_SPECIFIC_CONST_0, BASETEXTURETRANSFORM );

		SetVertexShaderConstant( VERTEX_SHADER_SHADER_SPECIFIC_CONST_3, BASETEXTUREOFFSET );
		SetVertexShaderConstant( VERTEX_SHADER_SHADER_SPECIFIC_CONST_4, BASETEXTURESCALE );

		Vector4D shadow;
		shadow[0] = params[FALLOFFOFFSET]->GetFloatValue();
		shadow[1] = params[FALLOFFDISTANCE]->GetFloatValue() + shadow[0];
		if (shadow[1] != 0.0f)
			shadow[1] = 1.0f / shadow[1];
		shadow[2] = params[FALLOFFAMOUNT]->GetFloatValue();
		pShaderAPI->SetVertexShaderConstant( VERTEX_SHADER_SHADER_SPECIFIC_CONST_5, shadow.Base(), 1 );

		shadowmodel_Dynamic_Index vshIndex;
		vshIndex.SetDOWATERFOG( pShaderAPI->GetSceneFogMode() == MATERIAL_FOG_LINEAR_BELOW_FOG_Z );
		vshIndex.SetSKINNING( pShaderAPI->GetCurrentNumBones() > 0 );
		pShaderAPI->SetVertexShaderIndex( vshIndex.GetIndex() );
	}
	Draw( );
}
END_SHADER




#else
//360 version

BEGIN_VS_SHADER_FLAGS( ShadowModel_DX9, "Help for ShadowModel", SHADER_NOT_EDITABLE )

BEGIN_SHADER_PARAMS
SHADER_PARAM( BASETEXTUREOFFSET, SHADER_PARAM_TYPE_VEC2, "[0 0]", "$baseTexture texcoord offset" )
SHADER_PARAM( BASETEXTURESCALE, SHADER_PARAM_TYPE_VEC2, "[1 1]", "$baseTexture texcoord scale" )
SHADER_PARAM( FALLOFFOFFSET, SHADER_PARAM_TYPE_FLOAT, "0", "Distance at which shadow starts to fade" )
SHADER_PARAM( FALLOFFDISTANCE, SHADER_PARAM_TYPE_FLOAT, "100", "Max shadow distance" )
SHADER_PARAM( FALLOFFAMOUNT, SHADER_PARAM_TYPE_FLOAT, "0.9", "Amount to brighten the shadow at max dist" )
END_SHADER_PARAMS

SHADER_INIT_PARAMS()
{
}

SHADER_FALLBACK
{
	return 0;
}

SHADER_INIT
{
}

SHADER_DRAW
{
	Draw( false );
}
END_SHADER

#endif