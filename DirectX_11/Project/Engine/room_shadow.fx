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
    
    // ���̾� 1�� ���� �ٴ� ��� �ؽ����ε� �̰��� �ƹ��͵� ������ �翬�� �׸��ڵ� �׷����� �ȵ�
    if (0.f == Layer1Tex[_ID.xy].a)
        return;
    
    // Shadow ��ǥ�� id �� ��ȯ�Ǽ� ���� ����
    float2 fShadowLT = SHADOW_POS - SHADOW_RESOL / 2.f;
    float2 fShadowRB = SHADOW_POS + SHADOW_RESOL / 2.f;
    float2 fID = (float2) _ID.xy;
 
    // �׸��ڰ� �׷��� �������� ũ�� ȣ��Ǹ� �ȵ�
    if (fShadowLT.x > fID.x || fShadowLT.y > fID.y || fShadowRB.x < fID.x || fShadowRB.y < fID.y)
        return;

    float2 fShadowID = fID - fShadowLT;
    float4 vShadowCol = ShadowTex[(int2) fShadowID];
    
    // ���� �׸��� �̹����� ���İ� ������ �װ��� �׷��� ������ �ƴ�
    if (vShadowCol.a == 0.f)
        return;
    
    g_DrawTex[_ID.xy] = float4(0.1f, 0.1f, 0.1f, 0.5f);
}

#endif