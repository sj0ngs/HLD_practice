#include "pch.h"
#include "CProjectileScript.h"

#include <Engine\CAnim2D.h>

CProjectileScript::CProjectileScript() :
	CScript((UINT)SCRIPT_TYPE::PROJECTILESCRIPT),
	m_vDir{ 0.f, 1.f, 0.f },
	m_fSpeed(100.f),
	m_fDist(1000.f),
	m_faccDist(0.f),
	m_bHomming(false),
	m_fHommingRatio(0.5f),
	m_pTarget(nullptr),
	m_bIsFade(false),
	m_fDmg(0),
	m_pEndSound(nullptr)
{
}

CProjectileScript::~CProjectileScript()
{
}

void CProjectileScript::begin()
{
}

void CProjectileScript::tick()
{
	if (m_fDist <= 0)
	{
		if (m_strDeadAnimName.empty())
		{
			Destory();
			return;
		}
		else if(!m_bIsFade)
		{
			Animator2D()->Play(m_strDeadAnimName, false);
			m_bIsFade = true;
			Transform()->SetRelativeScale(m_vDeadSize);
		}
	}
	else
	{
		Vec3 vPos = Transform()->GetRelativePos();
		if (m_bHomming && IsValid(m_pTarget))
		{
			Vec3 vTargetDir = m_pTarget->Transform()->GetRelativePos();
			vTargetDir -= vPos;
			vTargetDir.z = 0;
			vTargetDir.Normalize();
			m_vDir = (m_vDir  + vTargetDir * (m_fHommingRatio));
			m_vDir.Normalize();
			vPos += m_vDir * DT * m_fSpeed;
		}
		else
		{
			vPos += m_vDir * DT * m_fSpeed;
		}
		m_fDist -= m_fSpeed * DT;
		Transform()->SetRelativePos(vPos);
	}

	if (m_bIsFade)
	{
		if (Animator2D()->GetCurAnim()->IsFinish())
		{
			Destory();
			return;
		}
	}
}

void CProjectileScript::BeginOverlap(CCollider2D* _Other)
{
	if (!GetOwner()->IsLifeSpan())
	{
		Destory();

		if (m_pEndSound != NULL)
		{
			m_pEndSound->Play(1, 10.f, true);
		}
	}
}

void CProjectileScript::OnOverlap(CCollider2D* _Other)
{
}

void CProjectileScript::EndOverlap(CCollider2D* _Other)
{
}

