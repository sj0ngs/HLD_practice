#ifndef _ROOM_SHADOW
#define _ROOM_SHADOW

#include "value.fx"

RWTexture2D<float4> g_DrawTex : register(u0);
Texture2D<float4> Layer1Tex : register(t0);
Texture2D<float4> ShadowTex : register(t1);

#define TEX_RESOL       g_vec2_0
#define SHADOW_RESOL    g_vec2_1
#define SHADOW_POS      g_vec2_2
#define ERASE_MODE      g_int_0

[numthreads(32, 32, 1)]
void CS_RoomShadow(int3 _ID : SV_DispatchThreadID)
{
    if (_ID.x >= (int) TEX_RESOL.x || _ID.y >= (int) TEX_RESOL.y)
        return;
    
    if (ERASE_MODE)
    {
        g_DrawTex[_ID.xy] = (float4) 0.f;
        return;
    }
    
    // 레이어 1이 실제 바닥 모양 텍스쳐인데 이곳에 아무것도 없으면 당연히 그림자도 그려지면 안됨
    if (0.f == Layer1Tex[_ID.xy].a)
        return;
    
    // Shadow 좌표가 id 로 변환되서 들어올 것임
    float2 fShadowLT = SHADOW_POS - SHADOW_RESOL / 2.f;
    float2 fShadowRB = SHADOW_POS + SHADOW_RESOL / 2.f;
    float2 fID = (float2) _ID.xy;
 
    // 그림자가 그려질 영역보다 크면 호출되면 안됨
    if (fShadowLT.x > fID.x || fShadowLT.y > fID.y || fShadowRB.x < fID.x || fShadowRB.y < fID.y)
        return;

    float2 fShadowID = fID - fShadowLT;
    float4 vShadowCol = ShadowTex[(int2) fShadowID];
    
    // 만약 그림자 이미지의 알파가 없으면 그곳은 그려질 영역이 아님
    if (vShadowCol.a == 0.f)
        return;
    
    g_DrawTex[_ID.xy] = float4(0.1f, 0.1f, 0.1f, 0.5f);
}

#endif