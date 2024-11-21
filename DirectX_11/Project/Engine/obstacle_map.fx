#ifndef _OBSTACLE_MAP
#define _OBSTACLE_MAP

#include "value.fx"

RWTexture2D<float4> g_DrawTex : register(u0);
Texture2D<float4>   g_EdgeTex : register(t0);
Texture2D<float4>   g_WallTex : register(t1);

#define WIDTH g_int_0
#define HEIGHT g_int_1
#define COL_MODE g_int_2

#define CollPos     g_vec2_0
#define CollSize    g_vec2_1

[numthreads(32, 32, 1)]
void CS_ObstacleMap(int3 _ID : SV_DispatchThreadID)
{
    if(_ID.x < 0 || _ID.x >= WIDTH || _ID.y <0 || _ID.y >= HEIGHT)
        return;
    
    float4 vCol = float4(1.f, 0.f, 1.f, 1.f);
    
    if (COL_MODE == 0)
    {
        if (g_EdgeTex[_ID.xy].a <= 0.f && g_WallTex[_ID.xy].a <= 0.f)
            return;
        
        g_DrawTex[_ID.xy] = vCol;
    }
    else
    {
        float2 vLefTop = CollPos - CollSize / 2.f;
        float2 vRightBottom = CollPos + CollSize / 2.f;
        
        if (_ID.x < vLefTop.x || _ID.y < vLefTop.y || 
            _ID.x > vRightBottom.x || _ID.y > vRightBottom.y)
            return;
        
        g_DrawTex[_ID.xy] = vCol;
    }
}


#endif