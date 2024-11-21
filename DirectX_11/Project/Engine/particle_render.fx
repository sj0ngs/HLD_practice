#ifndef _PARTILCE_RENDER
#define _PARTILCE_RENDER

#include "value.fx"
#include "struct.fx"

// ========================
// Particle Render Shader
// mesh : RectMesh

// Parameter
// g_int_0 : Particle Index
// ========================

StructuredBuffer<tParticle> ParticleBuffer : register(t20);
StructuredBuffer<tParticleModule> ParticleModuleData : register(t21);

#define ModuleData          ParticleModuleData[0]

struct VS_IN
{
    float3 vPos : POSITION;
    uint iInstID : SV_InstanceID;
};

struct VS_OUT
{
    float3 vPos : POSITION;
    uint iInstID : SV_InstanceID;
};

VS_OUT VS_ParticleRender(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    output.vPos = _in.vPos;
    output.iInstID = _in.iInstID;
    return output;
}

struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    uint iInstID : SV_InstanceID;
};

[maxvertexcount(6)]
void GS_ParticleRender(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _outstream)
{
    uint id = _in[0].iInstID;
    
    if (0 == ParticleBuffer[id].Active)
        return;

    float3 vParticleViewPos = mul(float4(ParticleBuffer[id].vWorldPos.xyz, 1.f), g_matView).xyz;
    float2 vParticleScale = ParticleBuffer[id].vWorldScale.xy * ParticleBuffer[id].ScaleFactor;
    
    // 0 -- 1
    // |    |
    // 3 -- 2
    float3 NewPos[4] =
    {
        float3(-1.f * vParticleScale.x / 2.f, +1.f * vParticleScale.y / 2.f, 0.f),
        float3(+1.f * vParticleScale.x / 2.f, +1.f * vParticleScale.y / 2.f, 0.f),
        float3(+1.f * vParticleScale.x / 2.f, -1.f * vParticleScale.y / 2.f, 0.f),
        float3(-1.f * vParticleScale.x / 2.f, -1.f * vParticleScale.y / 2.f, 0.f)
    };
    
    if (ModuleData.Render)
    {
        if (ModuleData.VelocityScale)
        {
            float fValue = saturate(length(ParticleBuffer[id].vVelocity) / ModuleData.fMaxSpeed);
            float3 vScale = lerp(float3(1.f, 1.f, 1.f), ModuleData.MaxVelocityScale.xyz, fValue);
            
            NewPos[0].xyz *= vScale;
            NewPos[3].xyz *= vScale;
        }
        
        if (ModuleData.RotationType)
        {
            float fTheta = 0.f;
        
            // 일반 z 축 회전
            if (1 == ModuleData.RotationType)
            {
                fTheta = ParticleBuffer[id].RotData;
            }
            // 진행방향 정렬 회전
            else if (2 == ModuleData.RotationType)
            {
                float3 vVelocity = normalize(ParticleBuffer[id].vVelocity).xyz;
                vVelocity = mul(float4(vVelocity, 0.f), g_matView).xyz;
            
                float3 vRight = float3(1.f, 0.f, 0.f);
                fTheta = acos(dot(vRight, vVelocity));
                
                if (0.f > vVelocity.y)
                {
                    fTheta *= -1.f;
                }
            }
            
            float3x3 matRotZ =
            {
                { cos(fTheta), sin(fTheta), 0.f },
                { sin(fTheta) * -1.f, cos(fTheta), 0.f },
                { 0.f, 0.f, 1.f }
            };
            
            for (int i = 0; i < 4; i++)
            {
                NewPos[i] = mul(NewPos[i], matRotZ);
            }
        }
    }
    
    for (int i = 0; i < 4; i++)
    {
        NewPos[i].xyz += vParticleViewPos.xyz;
    }
    
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    output[0].vPosition = mul(float4(NewPos[0], 1.f), g_matProj);
    output[0].vUV = float2(0.f, 0.f);
    
    output[1].vPosition = mul(float4(NewPos[1], 1.f), g_matProj);
    output[1].vUV = float2(1.f, 0.f);
    
    output[2].vPosition = mul(float4(NewPos[2], 1.f), g_matProj);
    output[2].vUV = float2(1.f, 1.f);
    
    output[3].vPosition = mul(float4(NewPos[3], 1.f), g_matProj);
    output[3].vUV = float2(0.f, 1.f);
    
    for (int j = 0; j < 4; j++)
    {
        output[j].iInstID = id;
    }
 
    _outstream.Append(output[0]);
    _outstream.Append(output[1]);
    _outstream.Append(output[2]);
    _outstream.RestartStrip();
    
    _outstream.Append(output[0]);
    _outstream.Append(output[2]);
    _outstream.Append(output[3]);
    _outstream.RestartStrip();
}

float4 PS_ParticleRender(GS_OUT _in) : SV_Target
{    
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    
    if (g_btex_0)
    {
        float2 vUV = _in.vUV;
        
        if (ModuleData.Animation)
        {
            vUV /= float2((float) ModuleData.TileX, (float) ModuleData.TileY);
            int Col = ParticleBuffer[_in.iInstID].AnimFrame / ModuleData.TileX;
            int Row = ParticleBuffer[_in.iInstID].AnimFrame % ModuleData.TileX;
            float2 vSlice = float2(1.f / ModuleData.TileX, 1.f / ModuleData.TileY);
            
            vUV += vSlice * float2((float) Row, (float) Col);
        }

        vOutColor = g_tex_0.Sample(g_sam_0, vUV);
        vOutColor *= ParticleBuffer[_in.iInstID].vColor;
    }
    
    return vOutColor;
}

#endif