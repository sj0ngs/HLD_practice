#ifndef _STD2D
#define _STD2D

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
// Std2DShader
// RasterizerState   : None
// BlendState        : Mask
// DepthStencilState : Less
//
// Paramter
// g_int_0           : Anim Use
// g_vec2_0          : AnimAtlas LeftTop
// g_vec2_1          : AnimAtlas Slice
// g_vec2_2          : AnimAtlas BackSize
// g_tex_0           : Output Texture
//===================================

VS_OUT VS_Std2D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPostion = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;

    return output;
};

float4 PS_Std2D(VS_OUT _in) : SV_Target
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
    
    if(0.f == vOutColor.a)
    {
        discard;
    }
    
    if (1.f == vOutColor.r && 0.f == vOutColor.g && 1.f == vOutColor.b)
    {
        discard;
    }

    return vOutColor;
}



// ==================================
// Std2DLightShader
// RasterizerState   : None
// BlendState        : Mask
// DepthStencilState : Less
//
// Paramter
#define bAnimUse    g_int_0
#define LeftTop     g_vec2_0
#define Slice       g_vec2_1
#define Offset      g_vec2_2
#define BackSize    g_vec2_3

// g_int_0           : Anim Use
// g_vec2_0          : AnimAtlas LeftTop
// g_vec2_1          : AnimAtlas Slice
// g_vec2_2          : AnimAtlas Offset
// g_vec2_3          : AnimAtlas BackSize
// g_tex_0           : Output Texture
// g_tex_1           : Normal Texture

// Dissolve Effect
// g_tex_2           : Dissolve Texture
// g_float_0         : Edge Size
// g_float_1         : Dissolve Amount   
// g_vec4_0          : Edge Color

// Color Change
// g_int_1          : Color Change On
// g_vec4_1         : Changed Color
#define Resolution  g_vec4_1
//===================================

struct VS_Light_IN
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_Light_OUT
{
    float4 vPostion : SV_POSITION;
    float2 vUV : TEXCOORD;
    float3 vWorldPos : POSITION;
};


VS_Light_OUT VS_Std2DLight(VS_Light_IN _in)
{
    VS_Light_OUT output = (VS_Light_OUT) 0.f;
    
    output.vPostion = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    output.vWorldPos = mul(float4(_in.vLocalPos, 1.f), g_matWorld).xyz;

    return output;
};

float4 PS_Std2DLight(VS_Light_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    if(3.f == g_mat_3._11)
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
                vOutColor = g_tex_0.Sample(g_sam_1, vUV); 
                //vOutColor = GaussianBlur(g_tex_0, Resolution.xy, vUV);
            }
            else
            {
                //vOutColor = float4(0.f, 1.f, 1.f, 1.f);
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
    
    // Normal Mapping
    float3 vNormal = (float3) 0.f;
    if(g_btex_1)
    {
        // Normal 값 추출
        vNormal = g_tex_1.Sample(g_sam_1, _in.vUV).xyz;
        // 0 ~ 1 범위를 -1 ~ 1 으로 변환
        vNormal = (vNormal * 2.f) - 1.f;
        
        // y z 스왑
        float f = vNormal.y;
        vNormal.y = vNormal.z;
        vNormal.z = f;
        
        vNormal = normalize(mul(float4(vNormal, 0.f), g_matWorld)).xyz;
    }
    
    if (0.f == vOutColor.a)
    {
        discard;
    }
    
    //// 디졸브 처리
    //if (g_btex_2)
    //{
    //    float2 vUV = _in.vUV;
    //    if (bAnimUse)
    //    {
    //        vUV *= (BackSize / Slice);
    //    }
       
    //    float4 vDissColor = g_tex_2.Sample(g_sam_1, vUV);
    //    vOutColor = Dissolve(vOutColor, vDissColor);
    //}
    
    //if (g_int_2 == 1)
    //{
    //    vOutColor.rgb = 1.f;
    //}
    
    
    // 색 변경
    if (g_int_1 == 1)
    {
        vOutColor = g_vec4_1;
    }
    // 특정 영역의 색 강조
    else if (g_int_1 == 2)
    {
        vOutColor *= g_vec4_1;
    }
    
    // Lighting 처리
    tLightColor LightColor = (tLightColor) 0.f;
    
    if (dot(vNormal, vNormal) == 0.f)
    {
        CalcLight2D(_in.vWorldPos, LightColor);
    }
    else
    {
        CalcLight2D(_in.vWorldPos, vNormal, LightColor);
    }
    
    vOutColor.rgb *= (LightColor.vDiffuse.rgb + LightColor.vAmbient.rgb);
    
    return vOutColor;
}


#endif