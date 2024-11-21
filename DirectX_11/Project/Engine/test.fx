#ifndef _TEST
#define _TEST

#include "value.fx"

// vs �Է� ����ü
struct VS_IN
{
	float3 vPos : POSITION;	// semantic
	float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
};

// vs ��� ����ü
struct VS_OUT
{
	float4 vPostion : SV_POSITION; // SV �� �޸��� �ý��� �ø�ƽ�̴�
    float4 vOutColor : COLOR;
    float2 vOutUV : TEXCOORD;
};

// Vertex Shader 
VS_OUT VS_Test(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // ���庯ȯ, �亯ȯ, ������ȯ
    float4 vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    float4 vViewPos = mul(vWorldPos, g_matView);
    float4 vProjPos = mul(vViewPos, g_matProj);

    output.vPostion = vProjPos;
    output.vOutColor = _in.vColor;
    output.vOutUV = _in.vUV;

    return output;
};

// Pixel Shader
// ��ȯ Ÿ���� ����ü�� ������൵ ������
float4 PS_Test(VS_OUT _in) : SV_Target // Render Target�� ���� �����̴�
{
	float4 vColor = (float4) 0.f;
	
    if (g_btex_0)
    {
        if (0 == g_int_0)
        {
            vColor = g_tex_0.Sample(g_sam_0, _in.vOutUV);
        }
        else if (1 == g_int_0)
        {
            vColor = g_tex_0.Sample(g_sam_1, _in.vOutUV);
        }
    }
    else
    {
        vColor = _in.vOutColor;
    }

	return vColor;
}

// ===============
// SetColor Shader
// Mesh : RectMesh
// ===============
VS_OUT VS_SetColor(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPostion = float4(_in.vPos.xy * 2.f, 0.5f, 1.f);
    output.vOutUV = _in.vUV;
    
    return output;
}

float4 PS_SetColor(VS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    float fDist = distance(float2(0.5f, 0.5f), float2(_in.vOutUV.xy));
    
    if (0.2f < fDist)
    {
        discard;
    }
    
    vColor = float4(0.f, 1.f, 1.f, 1.f);
    
    return vColor;
}

#endif