#ifndef _UI
#define _UI

#include "struct.fx"
#include "func.fx"

struct VS_IN
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPostion : SV_POSITION;
    float2 vUV : TEXCOORD;
};

// ==================================
// UIShader
// RasterizerState   : None
// BlendState        : ALPHA_BLEND
// DepthStencilState : NO_TEST_NO_WRITE
//
// Paramter
// g_int_0          : Color Change
// g_vec4_0         : default color
// g_vec4_1         : changed color
// g_float_0        : alpha value
// g_tex_0          : Output Texture
//===================================

VS_OUT VS_UIShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPostion = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;

    return output;
};

float4 PS_UIShader(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    if (g_btex_0)
    {
        vOutColor = g_tex_0.Sample(g_sam_1, _in.vUV);
    }
    else
    {
        vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    }
    
    if (g_int_0 == 1)
    {
        if (vOutColor.r == g_vec4_0.r &&
            vOutColor.g == g_vec4_0.g &&
            vOutColor.b == g_vec4_0.b)
        {
            vOutColor.rgba = g_vec4_1.rgba;
        }
    }
    
    if (g_float_0 != 0.f)
    {
        vOutColor.a = g_float_0;
    }
    
    if (0.f == vOutColor.a)
    {
        discard;
    }

    return vOutColor;
}

#endif