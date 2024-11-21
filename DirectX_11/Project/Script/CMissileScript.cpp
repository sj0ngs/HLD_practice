#include "pch.h"
#include "CMissileScript.h"

CMissileScript::CMissileScript() :
	CScript((UINT)SCRIPT_TYPE::MISSILESCRIPT),
	m_vDir{ 0.f, 1.f, 0.f},
	m_fSpeed(100.f),
	m_vStartPos{},
	m_fDist(1000.f),
	m_faccDist(0.f)
{
}

CMissileScript::~CMissileScript()
{
}

void CMissileScript::begin()
{
	MeshRender()->GetDynamicMaterial();
}

void CMissileScript::tick()
{
	Vec3 vDist = Transform()->GetWorldPos() - m_vStartPos;

	if (m_fDist <= vDist.Length())
	{
		Destory();
		return;
	}

	Vec3 vPos = Transform()->GetRelativePos();
	vPos += m_vDir * DT * m_fSpeed;
	Transform()->SetRelativePos(vPos);
}

void CMissileScript::BeginOverlap(CCollider2D* _Other)
{
	Destory();
}

void CMissileScript::OnOverlap(CCollider2D* _Other)
{
}

void CMissileScript::EndOverlap(CCollider2D* _Other)
{
}
