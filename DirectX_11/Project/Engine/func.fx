#ifndef _FUNC
#define _FUNC

#include "value.fx"

void CalcLight2D(float3 _vWorldPos, inout tLightColor _Light)
{
    for (int i = 0; i < (int)g_Light2DCount; ++i)
    {
        if (0 == g_Light2DBuffer[i].LightType)
        {
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb;
            _Light.vAmbient.rgb += g_Light2DBuffer[i].Color.vAmbient.rgb;
            
        }
        else if (1 == g_Light2DBuffer[i].LightType)
        {
            float3 vLightWorldPos = float3(g_Light2DBuffer[i].vWorldPos.xy, 0.f);
            float3 vWorldPos = float3(_vWorldPos.xy, 0.f);
        
            float fDistance = distance(vWorldPos, vLightWorldPos);
            float fPow = saturate(1.f - (fDistance / g_Light2DBuffer[i].Radius));
            
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fPow;
        }
        else if (2 == g_Light2DBuffer[i].LightType)
        {
            float3 vLightWorldPos = float3(g_Light2DBuffer[i].vWorldPos.xy, 0.f);
            float3 vWorldPos = float3(_vWorldPos.xy, 0.f);
            float3 vDir = normalize(float3(vWorldPos - vLightWorldPos));
            float3 vLightDir = float3(g_Light2DBuffer[i].vWorldDir.xyz);
            
            float fRad = acos(dot(vDir, vLightDir));
            float fAngle = radians(g_Light2DBuffer[i].Angle / 2.f);

            if (fRad <= fAngle)
            {
                float fFadePow = saturate(1.f - fRad / fAngle);
                float fDistance = distance(vWorldPos, vLightWorldPos);
                float fPow = saturate(1.f - (fDistance / g_Light2DBuffer[i].Radius));
                _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fPow * fFadePow;
            }
        }
    }
    
}

void CalcLight2D(float3 _vWorldPos, float3 _vNormalDir, inout tLightColor _Light)
{
    for (int i = 0; i < (int)g_Light2DCount; ++i)
    {
        if (0 == g_Light2DBuffer[i].LightType)
        {
            float fDiffusePow = saturate(dot(-g_Light2DBuffer[i].vWorldDir.xyz, _vNormalDir));
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fDiffusePow;
            _Light.vAmbient.rgb += g_Light2DBuffer[i].Color.vAmbient.rgb;
        }
        else if (1 == g_Light2DBuffer[i].LightType)
        {
            float3 vLightWorldPos = float3(g_Light2DBuffer[i].vWorldPos.xy, 0.f);
            float3 vWorldPos = float3(_vWorldPos.xy, 0.f);
            
            float3 vLight = normalize(vWorldPos - vLightWorldPos);
            float fDiffusePow = saturate(dot(-vLight, _vNormalDir));
        
            float fDistance = distance(vWorldPos, vLightWorldPos);
            float fDistPow = saturate(1.f - fDistance / g_Light2DBuffer[i].Radius);
            
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fDistPow * fDiffusePow;
        }
        else if (2 == g_Light2DBuffer[i].LightType)
        {
            float3 vLightWorldPos = float3(g_Light2DBuffer[i].vWorldPos.xy, 0.f);
            float3 vWorldPos = float3(_vWorldPos.xy, 0.f);
            float3 vDir = normalize(float3(vWorldPos - vLightWorldPos));
            float3 vLightDir = float3(g_Light2DBuffer[i].vWorldDir.xyz);
            
            float fRad = acos(dot(vDir, vLightDir));
            float fAngle = radians(g_Light2DBuffer[i].Angle / 2.f);

            if (fRad <= fAngle)
            {
                float3 vLight = normalize(vWorldPos - vLightWorldPos);
                float fDiffusePow = saturate(dot(-vLight, _vNormalDir));
                float fDistance = distance(vWorldPos, vLightWorldPos);
                float fPow = saturate(1.f - (fDistance / g_Light2DBuffer[i].Radius));
                _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fPow * fDiffusePow;
            }
        }
    }
}

// ======
// Random
// ======
static float GaussianFilter[5][5] =
{
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.0219f, 0.0983f, 0.1621f, 0.0983f, 0.0219f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
};

void GaussianSample(in Texture2D _NoiseTex, float2 _vResolution, float _NormalizedThreadID, out float3 _vOut)
{
    float2 vUV = float2(_NormalizedThreadID, 0.5f);
    vUV.x += g_AccTime * 0.5f;
    
    // Sin 그래프를 이용한 샘플링 UV 계산
    vUV.y -= (sin((_NormalizedThreadID - g_AccTime) * 2.f * 3.1415926535f * 10.f) / 2.f);
    
    if (1.f < vUV.x)
        vUV.x = frac(vUV.x);
    else if (0.f > vUV.x)
        vUV.x = 1.f + frac(vUV.x);
    
    if (1.f < vUV.y)
        vUV.y = frac(vUV.y);
    else if (0.f > vUV.y)
        vUV.y = 1.f + frac(vUV.y);

    int2 pixel = vUV * _vResolution;
    int2 offset = int2(-2, -2);
    
    float3 vOut = (float3) 0.f;
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            vOut += _NoiseTex[pixel + offset + int2(j, i)].xyz * GaussianFilter[i][j];
        }
    }
    
    _vOut = vOut;
}

float4 GaussianBlur(in Texture2D _Texture, float2 _vResolution, float2 _vUV)
{
    float4 vOutColor = (float4) 0.f;
    
    int2 pixel = _vUV * _vResolution;
    
    if (0 == _Texture[pixel].a)
        return vOutColor;
    
    int2 offset = int2(-2, -2);
    
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            vOutColor += _Texture[pixel + offset + int2(j, i)] * GaussianFilter[i][j];
        }
    }
    
    return vOutColor;
}

// Dissolve Effect
// g_tex_2           : Dissolve Texture
// g_float_0         : Edge Size
// g_float_1         : Dissolve Amount   
// g_vec4_0          : Edge Color

float4 Dissolve(float4 _vTexColor, float4 _vDissColor)
{
    float4 vOutColor = (float4) 0.f;
    
    float4 vLow = step(g_float_0, _vDissColor);
    float4 vHigh = step(g_float_0 + g_float_1, _vDissColor);
    float4 vEdge = vLow - vHigh;
    vEdge.a = _vTexColor.a;

    float4 vEdgeColor = vEdge * g_vec4_0;
    _vTexColor.a = vLow.r;
    vOutColor = lerp(_vTexColor, vEdgeColor, vEdge.r);
    
    if (0.f == vOutColor.a)
        discard;

    return vOutColor;
}
#endif