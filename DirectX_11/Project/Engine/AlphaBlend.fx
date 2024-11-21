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
    float3 vWorldPos : POSITION;
};

// ==================================
// AlphaBlendLight
// RasterizerState   : None
// BlendState        : ALPHA_BLEND
// DepthStencilState : Less

// Paramter
#define bAnimUse    g_int_0
#define bColorChange g_int_1
#define bAlphaChange g_int_2
#define bLightOff   g_int_3
#define LeftTop     g_vec2_0
#define Slice       g_vec2_1
#define Offset      g_vec2_2
#define BackSize    g_vec2_3
#define AlphaDown   g_float_1
// g_int_0           : Anim Use
// g_vec2_0          : AnimAtlas LeftTop
// g_vec2_1          : AnimAtlas Slice
// g_vec2_2          : AnimAtlas Offset
// g_vec2_3          : AnimAtlas BackSize
// g_tex_0           : Output Texture
// Color Change
// g_int_1          : ChangeUse
// g_vec4_0         : Filter Color
// g_int_2          : AlphaChange Use
// g_float_0        : AlphaValue
//===================================

VS_OUT VS_ABLight(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPostion = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    output.vWorldPos = mul(float4(_in.vLocalPos, 1.f), g_matWorld).xyz;

    return output;
};

float4 PS_ABLight(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    if (3.f == g_mat_3._11)
        return float4(1.f, 0.f, 0.f, 1.f);
    
    if (g_btex_0)
    {
        if (bAnimUse)
        {
            float2 vUV = LeftTop + (BackSize * _in.vUV);
            vUV -= ((BackSize - Slice) / 2.f);
            vUV -= Offset;
            
            if (LeftTop.x < vUV.x && vUV.x < LeftTop.x + Slice.x &&
                LeftTop.y < vUV.y && vUV.y < LeftTop.y + Slice.y)
            {
                vOutColor = g_tex_0.Sample(g_sam_0, vUV);
            }
            else
            {
                discard;
            }
        }
        else
        {
            vOutColor = g_tex_0.Sample(g_sam_1, _in.vUV);
        }
    }
    else
    {
        vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    }

    if (0.f == vOutColor.a)
    {
        discard;
    }
    
    if (bColorChange == 1)
    {
        vOutColor *= g_vec4_0;
    }
    
    if (bAlphaChange == 1)
    {
        vOutColor.a = g_float_0;
    }
    
    // Lighting Ã³¸®
    if (0 == bLightOff)
    {
        tLightColor LightColor = (tLightColor) 0.f;
        CalcLight2D(_in.vWorldPos, LightColor);
        vOutColor.rgb *= (LightColor.vDiffuse.rgb + LightColor.vAmbient.rgb);
    }
    
    if(0.f != AlphaDown)
    {
        vOutColor.a *= AlphaDown;
    }
    
    return vOutColor;
}