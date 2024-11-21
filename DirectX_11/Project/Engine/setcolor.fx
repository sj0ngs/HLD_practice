#ifndef _SETCOLOR
#define _SETCOLOR

#include "value.fx"

RWTexture2D<float4> g_Output : register(u0);
#define Color       g_vec4_0
#define Width       g_int_0
#define Height      g_int_1

[numthreads(32, 32, 1)]   // 쓰레드 그룹 개수
void CS_SetColor(int3 _ID : SV_DispatchThreadID)
{
    if (Width / 2.f - 10.f <= _ID.x && Width / 2.f + 10.f >= _ID.x ||
        Height / 2.f - 10.f <= _ID.y && Height / 2.f + 10.f >= _ID.y)
        g_Output[_ID.xy] = float4(Color.xyz, 1.f);
}

#endif