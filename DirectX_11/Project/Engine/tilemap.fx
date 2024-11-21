#ifndef _TILEMAP
#define _TILEMAP

#include "value.fx"
#include "func.fx"

struct tTile
{
    float2 vLeftTop;
    float2 vSlice;
};

// ============================
// TileMap Shader
// 
// RS_TYPE : CULL_NONE
// DS_TYPE : LESS
// BS_TYPE : MASK

// Parameter
// g_tex_0 : Tile Atlas Texture
// g_int_0 : X
// g_int_1 : Y
StructuredBuffer<tTile> TileBuffer : register(t20);
// ============================

struct VS_TILEMAP_IN
{
    float3 vPos     : POSITION;
    float2 vUV      : TEXCOORD;
};

struct VS_TILEMAP_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD;
    float3 vWorldPos : POSITION;
};

VS_TILEMAP_OUT VS_TileMap(VS_TILEMAP_IN _in)
{
    VS_TILEMAP_OUT output = (VS_TILEMAP_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos.xyz, 1.f), g_matWVP);
    output.vUV = _in.vUV * float2(g_int_0, g_int_1);
    output.vWorldPos = mul(float4(_in.vPos.xyz, 1.f), g_matWorld).xyz;
    
    return output;
}

float4 PS_TileMap(VS_TILEMAP_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    
    // 실수 파트 frac : 타일 한칸 내에서 픽셀의 상대적이 위치
    // 정수 파트 floor : 전체 타일 중의 본인의 인덱스(행, 열)
    
    // 타일 구조화 버퍼내에서 자신이 접근해야하는 인덱스
    int2 Idx = floor(_in.vUV);
    int BufferIdx = g_int_0 * Idx.y + Idx.x;
    
    if (0.f == TileBuffer[BufferIdx].vSlice.x ||
        0.f == TileBuffer[BufferIdx].vSlice.y)
    {
        discard;
    }

    float2 vUV = TileBuffer[BufferIdx].vLeftTop + TileBuffer[BufferIdx].vSlice * frac(_in.vUV);
    
    if (g_btex_0)
    {
        vOutColor = g_tex_0.Sample(g_sam_0, vUV);
    }
    
    tLightColor LightColor = (tLightColor) 0.f;
    CalcLight2D(_in.vWorldPos.xyz, LightColor);
    vOutColor.rgb *= (LightColor.vDiffuse + LightColor.vAmbient).xyz;

    return vOutColor;
}

#endif