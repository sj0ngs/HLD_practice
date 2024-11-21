#include "pch.h"
#include "CPlayerTrailScript.h"

CPlayerTrailScript::CPlayerTrailScript()	:
	CScript(SCRIPT_TYPE::PLAYERTRAILSCRIPT),
	m_fLifeTime(0.f),
	m_fAccTime(0.f)
{
}

CPlayerTrailScript::~CPlayerTrailScript()
{
}


void CPlayerTrailScript::begin()
{
}

void CPlayerTrailScript::tick()
{
	// 색상 변경
	if (m_fAccTime >= m_fLifeTime)
	{
		Destory();
	}

	m_fAccTime += DT;
	int bUse = 1;
	MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_0, &bUse);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC2_0, &m_tFrm.LeftTopUV);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC2_1, &m_tFrm.SliceUV);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC2_2, &m_tFrm.Offset);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC2_3, &m_vBackSize);
	MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_0, m_pTex.Get());

	MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_1, &bUse);
	Vec4 vColor = Vec4(3.f, 0.f, 3.f, 1.f);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(VEC4_0, &vColor);
	MeshRender()->GetDynamicMaterial()->SetScalarParam(INT_2, &bUse);
	float fValue = (1.f - m_fAccTime / m_fLifeTime) * 0.9f;
	MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, &fValue);
}

