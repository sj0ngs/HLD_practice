#ifndef _ROOM_COLLISION
#define _ROOM_COLLISION

#include "value.fx"

struct tRoomCollisionResult
{
    int iResult; // 0 : �浹 ����, 1 : ��������, 2 : ��
    float fDist;
    float2 vDir;
    uint uDist;  // ���� ����ȭ �񱳿����� �ʿ�
    float3 vPadding;
};

RWStructuredBuffer<tRoomCollisionResult> pOutput : register(u0);
Texture2D<float4> Layer1Tex : register(t0);
Texture2D<float4> Layer2Tex : register(t1);

#define TEX_RESOL   g_vec2_0
#define ROOM_SIZE   g_vec2_1
#define OBJ_POS     g_vec2_2
#define OBJ_SIZE    g_float_0

[numthreads(32, 32, 1)]
void CS_RoomCollision(int3 _ID : SV_DispatchThreadID)
{
    if (_ID.x >= (int) TEX_RESOL.x || _ID.y >= (int) TEX_RESOL.y)
        return;
    
    // ��ǻƮ ���̴��� �ѱ�� ���� �̸� ������Ʈ ��ġ�� uv ��ǥ�� ��ȯ�ص�
    float2 vID = float2((float) _ID.x, (float) _ID.y);
    float2 vDir = OBJ_POS - vID;
    float fDist = length(vDir);
    
    if (fDist > OBJ_SIZE)
        return;

    int iResult = 0;
    if (0.f != Layer2Tex[_ID.xy].a)
        iResult = 2;
    else if (0.f != Layer1Tex[_ID.xy].a)
        iResult = 1;
    else
        return;
    
    uint iDist = (uint) fDist;
    InterlockedMin(pOutput[0].uDist, iDist);
    
    if (pOutput[0].uDist == iDist)
    {
        pOutput[0].vDir = normalize(vDir);
        pOutput[0].fDist = fDist;
        pOutput[0].iResult = iResult;
    }
}

#endif
