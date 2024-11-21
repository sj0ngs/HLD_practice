#ifndef _POSTPROCESS
#define _POSTPROCESS

#include "value.fx"

struct VS_IN
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

// ====================
// GrayShader
// Mesh : RectMesh
// Domain : PostProcess
// ====================

VS_OUT VS_GrayShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    output.vPosition = float4(_in.vLocalPos * 2.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_GrayShader(VS_OUT _in) : SV_Target
{
    float4 vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    
    float vAver = (vColor.r + vColor.g + vColor.b) / 3.f;
    vColor = float4(vAver, vAver, vAver, 1.f);
    return vColor;
}

// ====================
// Distortion
// Mesh : RectMesh
// Domain : PostProcess
// g_tex_0 : RenderTarget Copy
// g_tex_1 : Noise Tex
// ====================

VS_OUT VS_DistortionShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;

    return output;
}

float4 PS_DistortionShader(VS_OUT _in) : SV_Target
{
    float2 vUV = _in.vPosition.xy / g_Resolution;
    
    if (g_btex_1)
    {
        float2 vNoiseUV = float2(_in.vUV.x - (g_AccTime * 0.2f), _in.vUV.y);
        float4 vNoise = g_tex_1.Sample(g_sam_0, vNoiseUV);

        vNoise = (vNoise - 0.5f) * 0.02f;

        vUV += vNoise.r;
        //float2 vNoiseUV = _in.vUV;
        //vNoiseUV.x -= g_AccTime * 0.2f;
        //float4 vNoise = g_tex_1.Sample(g_sam_0, vNoiseUV) - 0.5f;
        //vNoise *= 0.02f;
        //vUV += vNoise.r;
    }
    else
    {
        float fChange = cos((vUV.x - g_AccTime * 0.05f / 0.15f) * 2 * PI) * 0.05f;
        vUV.y += fChange;
    }
    
    float4 vColor = g_tex_0.Sample(g_sam_0, vUV);

    return vColor;
}

// ====================
// Fade
// Mesh : RectMesh
// Domain : PostProcess
// g_tex_0 : RenderTarget Copy
// g_float_0 : Black Value
// g_vec4_0 : Mix Color
// ====================

VS_OUT VS_FadeShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    output.vPosition = float4(_in.vLocalPos * 2.f, 1.f);
    output.vUV = _in.vUV;

    return output;
}

float4 PS_FadeShader(VS_OUT _in) : SV_Target
{
    float4 vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    float fValue = saturate(g_float_0);
    vColor = g_vec4_0 * g_float_0 + vColor * (1.f - g_float_0);
    
    return vColor;
}

// ====================
// LetterBox
// Mesh : RectMesh
// Domain : PostProcess
// g_tex_0 : RenderTarget Copy
// g_float_0 : LetterBox Ratio
// g_vec4_0 : Box Color
// ====================

VS_OUT VS_LetterBoxShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    output.vPosition = float4(_in.vLocalPos * 2.f, 1.f);
    output.vUV = _in.vUV;

    return output;
}

float4 PS_LetterBoxShader(VS_OUT _in) : SV_Target
{
    float4 vColor = (float4)0.f;
    if (g_float_0 >= _in.vUV.y || (1.f - g_float_0) <= _in.vUV.y)
        vColor = g_vec4_0;
    else
        vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    return vColor;
}
#endif